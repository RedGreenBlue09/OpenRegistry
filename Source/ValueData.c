
// SPDX-License-Identifier: GPL-2.0-or-later

#include "Common.h"

// TODO: More logging
static registry_status Registry_ValidateValueData(
	uint32_t Type,
	void* pData,
	uint32_t nData
) {
	switch (Type) {
		case REGISTRY_NONE:
			break;
		case REGISTRY_STRING:
			if (nData % sizeof(uint16_t) != 0 || nData < sizeof(uint16_t))
				return STATUS_INFO_LENGTH_MISMATCH;
			if (((uint16_t*)pData)[nData / sizeof(uint16_t) - 1] != '\0')
				return STATUS_INVALID_DATA;
			break;
		case REGISTRY_EXPAND_STRING:
			if (nData % sizeof(uint16_t) != 0 || nData < sizeof(uint16_t))
				return STATUS_INFO_LENGTH_MISMATCH;
			if (((uint16_t*)pData)[nData / sizeof(uint16_t) - 1] != '\0')
				return STATUS_INVALID_DATA;
			break;
		case REGISTRY_BINARY:
			break;
		case REGISTRY_INT32LE:
			if (nData != sizeof(uint32_t))
				return STATUS_INFO_LENGTH_MISMATCH;
			break;
		case REGISTRY_INT32BE:
			if (nData != sizeof(uint32_t))
				return STATUS_INFO_LENGTH_MISMATCH;
			break;
		case REGISTRY_LINK:
			if (nData % sizeof(uint16_t) != 0 || nData < sizeof(uint16_t))
				return STATUS_INFO_LENGTH_MISMATCH;
			if (((uint16_t*)pData)[nData / sizeof(uint16_t) - 1] != '\0')
				return STATUS_INVALID_DATA;
			break;
		case REGISTRY_MULTI_STRING: {
			uint16_t* ss = pData;
			if (nData % sizeof(uint16_t) == 0 || nData >= sizeof(uint16_t)) {
				size_t n = nData / sizeof(uint16_t);
				if (ss[n - 1] == '\0') {
					size_t iOffset = 0;
					while (iOffset < n && ss[iOffset] != '\0') {
						uint16_t* s = &ss[iOffset];
						size_t Length = Registry_CountString16(s);
						iOffset += Length + 1;
					};
				} else {
					return STATUS_INVALID_DATA;
				}
			} else {
				return STATUS_INFO_LENGTH_MISMATCH;
			}
			break;
		}
		case REGISTRY_RESOURCE_LIST:
			return STATUS_NOT_IMPLEMENTED;
		case REGISTRY_FULL_RESOURCE_DESCRIPTOR:
			return STATUS_NOT_IMPLEMENTED;
		case REGISTRY_RESOURCE_REQUIREMENTS_LIST:
			return STATUS_NOT_IMPLEMENTED;
		case REGISTRY_INT64LE:
			if (nData != sizeof(uint64_t))
				return STATUS_INFO_LENGTH_MISMATCH;
			break;
		default:
			return STATUS_INVALID_PARAMETER;
	}
	return STATUS_SUCCESS;
}

// Value type

registry_status Registry_OpenAndGetValueDataWithType(
	h_value hValue,
	h_value_data* phValueData,
	void** ppData,
	uint32_t* pnData,
	registry_value_type ExpectedType
) {
	registry_status Status;

	uint32_t ValueType;
	Status = Registry_GetValueType(hValue, &ValueType);
	if (Status) {
		REGISTRY_STDERR("Error getting value type. NTSTATUS: 0x%"PRIX32"\n", Status);
		return STATUS_UNSUCCESSFUL;
	}
	if (ValueType != ExpectedType) {
		REGISTRY_STDERR("Error: Value type mismatch.\n");
		return STATUS_OBJECT_TYPE_MISMATCH;
	}

	h_value_data hValueData;
	Status = Registry_OpenValueData(hValue, &hValueData);
	if (Status) {
		REGISTRY_STDERR("Error opening value data. NTSTATUS: 0x%"PRIX32"\n", Status);
		return STATUS_UNSUCCESSFUL;
	}

	void* pData;
	uint32_t nData;
	Status = Registry_GetValueData(hValueData, &pData, &nData);
	if (Status) {
		REGISTRY_STDERR("Error getting value data. NTSTATUS: 0x%"PRIX32"\n", Status);
		Status = STATUS_UNSUCCESSFUL;
		goto CleanupValueData;
	}
	
	Status = Registry_ValidateValueData(ValueType, pData, nData);
	if (Status) {
		//REGISTRY_STDERR("Value data validation failed. NTSTATUS: 0x%"PRIX32"\n", Status);
		Status = STATUS_INVALID_DATA;
		goto CleanupValueData;
	}

	*phValueData = hValueData;
	*ppData = pData;
	*pnData = nData;
	return STATUS_SUCCESS;

	CleanupValueData:
	Registry_CloseValueData(hValueData);

	return Status;
}

registry_status Registry_QueryValueDataWithType(
	h_value hValue,
	void* pData,
	uint32_t nData,
	uint32_t* pRealSize,
	registry_value_type ExpectedType
) {
	h_value_data hValueData;
	void* pValueData;
	registry_status Status;
	Status = Registry_OpenAndGetValueDataWithType(
		hValue,
		&hValueData,
		&pValueData,
		pRealSize,
		ExpectedType
	);
	if (Status) {
		return Status;
	}

	uint32_t CopySize = (nData, *pRealSize);
	memcpy(pData, pValueData, CopySize);

	Registry_CloseValueData(hValueData);
	return STATUS_SUCCESS;
}

registry_status Registry_QueryValueDataWithTypeNative(
	h_value hValue,
	void* pData,
	uint32_t nData,
	uint32_t* pRealSize,
	registry_value_type ExpectedType
) {
	h_value_data hValueData;
	void* pValueData;
	uint32_t ValueRealSize;
	registry_status Status;

	Status = Registry_OpenAndGetValueDataWithType(
		hValue,
		&hValueData,
		&pValueData,
		&ValueRealSize,
		ExpectedType
	);
	if (Status) {
		return Status;
	}

	switch (ExpectedType) {
		case REGISTRY_NONE:
		case REGISTRY_BINARY:
			memcpy(pData, pValueData, (nData, ValueRealSize));
			*pRealSize = ValueRealSize;
			Status = STATUS_SUCCESS;
			break;

		case REGISTRY_INT32LE:
			*(uint32_t*)pData = SwapBytes32Le(*(uint32_t*)pValueData);
			*pRealSize = ValueRealSize;
			Status = STATUS_SUCCESS;
			break;

		case REGISTRY_INT32BE:
			*(uint32_t*)pData = SwapBytes32Be(*(uint32_t*)pValueData);
			*pRealSize = ValueRealSize;
			Status = STATUS_SUCCESS;
			break;

		case REGISTRY_INT64LE:
			*(uint64_t*)pData = SwapBytes64Le(*(uint64_t*)pValueData);
			*pRealSize = ValueRealSize;
			Status = STATUS_SUCCESS;
			break;

		case REGISTRY_STRING:
		case REGISTRY_EXPAND_STRING:
		case REGISTRY_LINK:
		case REGISTRY_MULTI_STRING: {
			/*
			size_t nCodeUnits = RealSize32 / sizeof(uint16_t);
			registry_string16 SourceString = {
				nCodeUnits,
				nCodeUnits,
				(uint16_t*)pValueData
			};

			size_t RealWideLength = Registry_CountWideStringFromString16(SourceString);
			size_t RealWideSize = RealWideLength * sizeof(wchar_t);

			Registry_ConvertString16ToWideString(
				SourceString,
				pData,
				nData / sizeof(wchar_t)
			);
			*pRealSize = RealWideSize;
			Status = STATUS_SUCCESS;
			break;
			*/

			memcpy(pData, pValueData, (nData, ValueRealSize));
			*pRealSize = ValueRealSize;
			Status = STATUS_SUCCESS;
			break;
		}

		case REGISTRY_RESOURCE_LIST:
		case REGISTRY_FULL_RESOURCE_DESCRIPTOR:
		case REGISTRY_RESOURCE_REQUIREMENTS_LIST:
			Status = STATUS_NOT_IMPLEMENTED;
			break;

		default:
			Status = STATUS_INVALID_PARAMETER;
			break;
	}

	Registry_CloseValueData(hValueData);
	return Status;
}

// Requiring a pointer here is not ideal,
// but I want to avoid malloc as much as possible.
registry_status Registry_SetValueData(
	h_key hKey,
	h_value hValue,
	h_value_data* phValueData,
	const void* pData,
	uint32_t nData
) {
	hive_ex* pHiveEx = (hive_ex*)hValue.hHive;
	HHIVE* pHive = &pHiveEx->Hive;
	registry_status Status;

	if (nData > (uint32_t)INT32_MAX) {
		REGISTRY_STDERR("Error: Value data exceeds INT32_MAX.\n");
		Status = STATUS_INVALID_PARAMETER;
		goto CleanupDone;
	}
	if (CmpIsKeyValueBig(pHive, nData)) {
		REGISTRY_STDERR("Error: Big value data is not supported.\n");
		Status = STATUS_UNIMPLEMENTED;
		goto CleanupDone;
	}

	if (!HvMarkCellDirty(pHive, hValue.CellIndex, TRUE)) {
		REGISTRY_STDERR("Error marking value cell dirty.\n");
		Status = STATUS_NO_LOG_SPACE;
		goto CleanupDone;
	}

	if (!HvMarkCellDirty(pHive, hKey.CellIndex, TRUE)) {
		REGISTRY_STDERR("Error marking key cell dirty.\n");
		Status = STATUS_NO_LOG_SPACE;
		goto CleanupDone;
	}

	CELL_DATA* pCellData = hValue.pCellData;
	CM_KEY_VALUE* pKeyValue = &pCellData->u.KeyValue;
	assert(pKeyValue->Signature == CM_KEY_VALUE_SIGNATURE);

	uint32_t nDataOld;
	uint8_t bSmallOld = CmpIsKeyValueSmall(&nDataOld, pKeyValue->DataLength);
	uint8_t bSmallNew = nData <= CM_KEY_VALUE_SMALL;

	uint8_t bCellExistOld = (!bSmallOld && nDataOld > 0);
	if (bCellExistOld) {
		if (!HvMarkCellDirty(pHive, phValueData->CellIndex, TRUE)) {
			REGISTRY_STDERR("Error marking value data cell dirty.\n");
			Status = STATUS_NO_LOG_SPACE;
			goto CleanupDone;
		}
	}

	// Allocate new cell & update pKeyValue->Data
	void* pDataNew;
	HCELL_INDEX CellIndexNew;
	if (bSmallNew) {

		if (bCellExistOld) {
			HvReleaseCell(pHive, phValueData->CellIndex);
			CmpFreeValueData(pHive, pKeyValue->Data, nDataOld);
		}
		pDataNew = &pKeyValue->Data;
		CellIndexNew = HCELL_NIL;

	} else if (bCellExistOld) {

		if (nData <= (uint32_t)HvGetCellSize(pHive, pKeyValue)) {

			pDataNew = phValueData->pData;
			CellIndexNew = phValueData->CellIndex;

		} else {

			HvReleaseCell(pHive, phValueData->CellIndex);
			CellIndexNew = HvReallocateCell(pHive, phValueData->CellIndex, nData);
			if (CellIndexNew == HCELL_NIL) {
				REGISTRY_STDERR("Error reallocating registry cell.\n");
				Status = STATUS_NO_MEMORY;
				goto CleanupDone;
			}

			pKeyValue->Data = CellIndexNew;

			pDataNew = HvGetCell(pHive, CellIndexNew);

		}

	} else {
		// ReactOS uses the key's storage here
		HSTORAGE_TYPE Storage = HvGetCellType(hKey.CellIndex);
		CellIndexNew = HvAllocateCell(pHive, nData, Storage, HCELL_NIL);
		if (CellIndexNew == HCELL_NIL) {
			REGISTRY_STDERR("Error allocating registry cell.\n");
			Status = STATUS_NO_MEMORY;
			goto CleanupDone;
		}
		pKeyValue->Data = CellIndexNew;

		pDataNew = HvGetCell(pHive, CellIndexNew);
	}
	assert(HvIsCellDirty(pHive, CellIndexNew));

	// Copy data
	pKeyValue->DataLength = nData;
	phValueData->CellIndex = CellIndexNew;
	phValueData->pData = pDataNew;
	memcpy(phValueData->pData, pData, nData);
	phValueData->nData = nData;

	// Update key node
	CELL_DATA* pKeyCellData = hKey.pCellData;
	assert(pKeyCellData->u.KeyNode.Signature == CM_KEY_NODE_SIGNATURE);
	CM_KEY_NODE* pKeyNode = &pKeyCellData->u.KeyNode;

	pKeyNode->MaxValueDataLen = REGISTRY_MAX(pKeyNode->MaxValueDataLen, nData);
	KeQuerySystemTime(&pKeyNode->LastWriteTime);

	return STATUS_SUCCESS;

	CleanupDone:
	return Status;
}
