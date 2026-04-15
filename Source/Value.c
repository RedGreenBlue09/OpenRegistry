
// SPDX-License-Identifier: GPL-2.0-or-later

#include "Common.h"

// Value

registry_status Registry_OpenValue(
	h_key hKey,
	const registry_string16 Name,
	h_value* phResultValue
) {
	hive_ex* pHiveEx = (hive_ex*)hKey.hHive;
	HHIVE* pHive = &pHiveEx->Hive;
	CELL_DATA* pCellData = hKey.pCellData;
	CM_KEY_NODE* pKeyNode = &pCellData->u.KeyNode;
	assert(pKeyNode->Signature == CM_KEY_NODE_SIGNATURE);

	UNICODE_STRING NameUnicode;
	registry_status Status = RegistryStringToUnicodeString(Name, &NameUnicode);
	if (Status != STATUS_SUCCESS) {
		REGISTRY_STDERR("Error: Error initalizing registry string. NTSTATUS: 0x%"PRIX32"\n", Status);
		return Status;
	}

	HCELL_INDEX ResultCellIndex;
	if (
		!CmpFindNameInList(
			pHive,
			&pKeyNode->ValueList,
			&NameUnicode,
			NULL,
			&ResultCellIndex
		)
	) {
		REGISTRY_STDERR("Error finding name in value list.\n");
		return STATUS_INSUFFICIENT_RESOURCES;
	}
	if (ResultCellIndex == HCELL_NIL) {
		return STATUS_NOT_FOUND;
	}

	CELL_DATA* pResultCellData = HvGetCell(pHive, ResultCellIndex);
	if (pResultCellData == NULL) {
		REGISTRY_STDERR("Error opening registry cell.\n");
		return STATUS_INSUFFICIENT_RESOURCES;
	}
	assert(pResultCellData->u.KeyValue.Signature == CM_KEY_VALUE_SIGNATURE);

	*phResultValue = (h_value){pHiveEx, pResultCellData, ResultCellIndex};
	return STATUS_SUCCESS;
}

registry_status Registry_CloseValue(
	h_value hValue
) {
	CELL_DATA* pCellData = hValue.pCellData;
	(void)pCellData;
	assert(pCellData->u.KeyValue.Signature == CM_KEY_VALUE_SIGNATURE);

	HHIVE* pHive = &(*(hive_ex*)hValue.hHive).Hive;
	HvReleaseCell(pHive, hValue.CellIndex);
	return STATUS_SUCCESS;
}

// Create & delete

registry_status Registry_CreateAndOpenValue(
	h_key hKey,
	registry_value_iterator iValue,
	registry_string16 Name,
	registry_value_type Type,
	h_value* phValue
) {
	hive_ex* pHiveEx = (hive_ex*)hKey.hHive;
	HHIVE* pHive = &pHiveEx->Hive;
	registry_status Status;

	UNICODE_STRING NameUnicode;
	Status = RegistryStringToUnicodeString(Name, &NameUnicode);
	if (Status) {
		REGISTRY_STDERR("Error: The registry string is too long.\n");
		goto CleanupDone;
	}
	uint16_t NameLengthNew = CmpNameSize(pHive, &NameUnicode);

	if (!HvMarkCellDirty(pHive, hKey.CellIndex, TRUE)) {
		REGISTRY_STDERR("Error marking parent cell dirty.\n");
		Status = STATUS_NO_LOG_SPACE;
		goto CleanupDone;
	}

	CELL_DATA* pKeyCellData = hKey.pCellData;
	assert(pKeyCellData->u.KeyNode.Signature == CM_KEY_NODE_SIGNATURE);
	CM_KEY_NODE* pKeyNode = &pKeyCellData->u.KeyNode;

	if (iValue.nValueList > 0) {
		if (!HvMarkCellDirty(pHive, iValue.ValueListCellIndex, TRUE)) {
			REGISTRY_STDERR("Error marking value list cell dirty.\n");
			Status = STATUS_NO_LOG_SPACE;
			goto CleanupDone;
		}
	}

	HSTORAGE_TYPE Storage = HvGetCellType(hKey.CellIndex);
	HCELL_INDEX CellIndex = HvAllocateCell(
		pHive,
		FIELD_OFFSET(CM_KEY_VALUE, Name) + CmpNameSize(pHive, &NameUnicode),
		Storage,
		HCELL_NIL
	);
	if (CellIndex == HCELL_NIL) {
		REGISTRY_STDERR("Error allocating value cell.\n");
		Status = STATUS_NO_MEMORY;
		goto CleanupDone;
	}

	CELL_DATA* pCellData = HvGetCell(pHive, CellIndex);
	assert(pCellData != NULL);
	CM_KEY_VALUE* pKeyValue = &pCellData->u.KeyValue;

	pKeyValue->Signature = CM_KEY_VALUE_SIGNATURE;
	pKeyValue->NameLength = CmpCopyName(pHive, pKeyValue->Name, &NameUnicode);
	pKeyValue->Flags = (pKeyValue->NameLength < NameUnicode.Length) ? VALUE_COMP_NAME : 0;
	pKeyValue->DataLength = 0;
	pKeyValue->Data = HCELL_NIL;
	pKeyValue->Type = Type;
	// ReactOS doesn't do anything with pKeyValue->Spare

	// The index parameter to this function doesn't seem to matter
	// ReactOS uses 0 by default but since it's backed by an array,
	// it's better to use nValueList.
	Status = CmpAddValueToList(
		pHive,
		CellIndex,
		iValue.nValueList,
		Storage,
		&pKeyNode->ValueList
	);
	if (Status) {
		REGISTRY_STDERR("Error adding value to value list.\n");
		Status = STATUS_NO_MEMORY;
		goto CleanupValueCell;
	}

	// Update Key
	pKeyNode->MaxValueNameLen = REGISTRY_MAX(pKeyNode->MaxValueNameLen, NameUnicode.Length);
	KeQuerySystemTime(&pKeyNode->LastWriteTime);

	*phValue = (h_value){pHive, pKeyValue, CellIndex};
	return STATUS_SUCCESS;

	CleanupValueCell:
	HvReleaseCell(pHive, CellIndex);
	HvFreeCell(pHive, CellIndex);

	CleanupDone:
	return Status;
}

registry_status Registry_DeleteValue(
	h_key hKey,
	registry_value_iterator* piValue,
	h_value hValue
) {
	hive_ex* pHiveEx = (hive_ex*)hValue.hHive;
	HHIVE* pHive = &pHiveEx->Hive;
	registry_status Status;

	if (!HvMarkCellDirty(pHive, hValue.CellIndex, TRUE)) {
		REGISTRY_STDERR("Error marking value cell dirty.\n");
		Status = STATUS_NO_LOG_SPACE;
		goto CleanupDone;
	}

	CELL_DATA* pCellData = hValue.pCellData;
	assert(pCellData->u.KeyValue.Signature == CM_KEY_VALUE_SIGNATURE);
	if (!CmpMarkValueDataDirty(pHive, &pCellData->u.KeyValue)) {
		REGISTRY_STDERR("Error marking value data dirty.\n");
		Status = STATUS_NO_LOG_SPACE;
		goto CleanupDone;
	}

	if (!HvMarkCellDirty(pHive, hKey.CellIndex, TRUE)) {
		REGISTRY_STDERR("Error marking parent cell dirty.\n");
		Status = STATUS_NO_LOG_SPACE;
		goto CleanupDone;
	}

	CELL_DATA* pKeyCellData = hKey.pCellData;
	assert(pKeyCellData->u.KeyNode.Signature == CM_KEY_NODE_SIGNATURE);
	CM_KEY_NODE* pKeyNode = &pKeyCellData->u.KeyNode;

	if (!HvMarkCellDirty(pHive, piValue->ValueListCellIndex, TRUE)) {
		REGISTRY_STDERR("Error marking value list cell dirty.\n");
		Status = STATUS_NO_LOG_SPACE;
		goto CleanupDone;
	};

	// Find the value's cell index in the list
	// Use iValue as a hint
	HCELL_INDEX* aValueList = piValue->aValueList;
	uint32_t iValueInt;
	if (
		piValue->iCurrent < piValue->nValueList &&
		aValueList[piValue->iCurrent] == hValue.CellIndex
	) {
		iValueInt = piValue->iCurrent;
	} else {
		for (iValueInt = 0; iValueInt < piValue->nValueList; ++iValueInt) {
			if (aValueList[iValueInt] == hValue.CellIndex)
				break;
		}
		if (iValueInt == piValue->nValueList) {
			// The hint didn't match, this is an error - iterator is stale or invalid
			REGISTRY_STDERR("Error: Value is not in key's value list.\n");
			Status = STATUS_NOT_FOUND;
			goto CleanupDone;
		}
	}

	// Remove from value list
	HvReleaseCell(pHive, pKeyNode->ValueList.List);
	Status = CmpRemoveValueFromList(pHive, iValueInt, &pKeyNode->ValueList);
	if (Status) {
		REGISTRY_STDERR("Error removing value from value list.\n");
		Status = STATUS_INSUFFICIENT_RESOURCES;
		goto CleanupDone;
	}

	// Free value
	HvReleaseCell(pHive, hValue.CellIndex);
	if (!CmpFreeValue(pHive, hValue.CellIndex)) {
		REGISTRY_STDERR("Error freeing value cell.\n");
		Status = STATUS_INSUFFICIENT_RESOURCES;
		goto CleanupDone;
	}

	KeQuerySystemTime(&pKeyNode->LastWriteTime);
	if (pKeyNode->ValueList.Count == 0) {
		pKeyNode->MaxValueNameLen = 0;
		pKeyNode->MaxValueDataLen = 0;
	}

	piValue->nValueList -= 1;
	piValue->iCurrent -= (piValue->iCurrent > iValueInt);

	return STATUS_SUCCESS;

CleanupDone:
	return Status;
}


// Value name

registry_status Registry_QueryValueName(
	h_value hValue,
	registry_string16* pResult,
	size_t* pRealNameLength
) {
	CELL_DATA* pCellData = hValue.pCellData;
	CM_KEY_VALUE* pKeyValue = &pCellData->u.KeyValue;
	assert(pKeyValue->Signature == CM_KEY_VALUE_SIGNATURE);

	USHORT RealNameSize;
	if (pKeyValue->Flags & VALUE_COMP_NAME)
		RealNameSize = CmpCompressedNameSize(pKeyValue->Name, pKeyValue->NameLength);
	else
		RealNameSize = pKeyValue->NameLength;

	USHORT NameLength = RealNameSize / sizeof(uint16_t);
	*pRealNameLength = NameLength;
	NameLength = (USHORT)((size_t)NameLength, pResult->AllocatedLength);
	pResult->Length = NameLength;
	USHORT NameSize = NameLength * sizeof(uint16_t);

	if (pKeyValue->Flags & VALUE_COMP_NAME) {
		CmpCopyCompressedName(
			pResult->aBuffer,
			NameSize,
			pKeyValue->Name,
			RealNameSize
		);
	} else {
		memcpy(pResult->aBuffer, pKeyValue->Name, NameSize);
	}
	return STATUS_SUCCESS;
}

registry_status Registry_SetValueName(
	h_key hKey,
	registry_value_iterator iValue,
	h_value hValue,
	registry_string16 Name
) {
	CELL_DATA* pCellData = hValue.pCellData;
	CM_KEY_VALUE* pKeyValue = &pCellData->u.KeyValue;
	assert(pKeyValue->Signature == CM_KEY_VALUE_SIGNATURE);
	hive_ex* pHiveEx = (hive_ex*)hValue.hHive;
	HHIVE* pHive = &pHiveEx->Hive;

	registry_status Status;
	if (!HvMarkCellDirty(pHive, hValue.CellIndex, TRUE)) {
		REGISTRY_STDERR("Error marking registry cell dirty.\n");
		Status = STATUS_NO_LOG_SPACE;
		goto CleanupDone;
	}

	// Calculate name size diff

	UNICODE_STRING NameUnicode;
	Status = RegistryStringToUnicodeString(Name, &NameUnicode);
	if (Status) {
		REGISTRY_STDERR("Error: The registry string is too long.\n");
		goto CleanupDone;
	}
	uint16_t NameLengthNew = CmpNameSize(pHive, &NameUnicode);
	int32_t NameLengthDiff = (int32_t)NameLengthNew - (int32_t)pKeyValue->NameLength;

	// We need to open the key's cell to update
	// MaxValueNameLen, LastWriteTime and this value's cell index.

	if (!HvMarkCellDirty(pHive, hKey.CellIndex, TRUE)) {
		REGISTRY_STDERR("Error marking key cell dirty.\n");
		Status = STATUS_NO_LOG_SPACE;
		goto CleanupDone;
	}

	CELL_DATA* pKeyCellData = hKey.pCellData;
	assert(pKeyCellData->u.KeyNode.Signature == CM_KEY_NODE_SIGNATURE);
	CM_KEY_NODE* pKeyNode = &pKeyCellData->u.KeyNode;

	// Reallocate the value's cell if the name size increases, and update the value list.

	HCELL_INDEX CellIndexNew;
	if (NameLengthDiff > 0) {
		// Lock the value list because the value's cell index might change
		// during reallocation, and we don't want a stale value list.
		if (iValue.nValueList == 0) {
			REGISTRY_STDERR("Error: Value is not in key's value list.\n");
			Status = STATUS_NOT_FOUND;
			goto CleanupDone;
		}
		if (!HvMarkCellDirty(pHive, iValue.ValueListCellIndex, TRUE)) {
			REGISTRY_STDERR("Error marking value list cell dirty.\n");
			Status = STATUS_NO_LOG_SPACE;
			goto CleanupDone;
		};

		// Find the value's cell index in the list
		// Use iValue as a hint
		HCELL_INDEX* aValueList = iValue.aValueList;
		uint32_t iValueInt;
		if (aValueList[iValue.iCurrent] == hValue.CellIndex) {
			iValueInt = iValue.iCurrent;
		} else {
			for (iValueInt = 0; iValueInt < iValue.nValueList; ++iValueInt) {
				if (aValueList[iValueInt] == hValue.CellIndex)
					break;
			}
			if (iValueInt == iValue.nValueList) {
				REGISTRY_STDERR("Error: Value is not in key's value list.\n");
				Status = STATUS_NOT_FOUND;
				goto CleanupDone;
			}
		}

		// HvReallocateCell will acquire the existing cell, so we need to release it.
		// I think it's safe to release as we already locked the value and value list
		// so we are the only one who can access the value's cell,
		// and no other thread can acquire it here.
		HvReleaseCell(pHive, hValue.CellIndex);

		// Reallocate the value's cell
		int32_t CellSizeNew = HvGetCellSize(pHive, pKeyValue) + NameLengthDiff;
		CellIndexNew = HvReallocateCell(pHive, hValue.CellIndex, (uint32_t)CellSizeNew);
		if (CellIndexNew == HCELL_NIL) {
			REGISTRY_STDERR("Error reallocating registry cell.\n");
			Status = STATUS_NO_MEMORY;
			goto CleanupDone;
		}
		// New cell is already acquired by HvReallocateCell for some stupid reason.
		// There is actual code in CmpAddValueToList that double acquires the cell
		// so I'm not sure what's the intention here, or it's a bug.
		// If we want to have RW protection, we have to fix this,
		// or ditch cell level locking and use handle level locking instead.
		// It's weird to have a "get address" combined with acquiring function anyways.

		// Replace the cell index
		aValueList[iValueInt] = CellIndexNew;

		// Update hValue
		pCellData = HvGetCell(pHive, CellIndexNew);
		assert(pCellData != NULL);
		pKeyValue = &pCellData->u.KeyValue;
		hValue.CellIndex = CellIndexNew;
		hValue.pCellData = pKeyValue;
	}

	// It's non-compressed size here
	// https://github.com/reactos/reactos/blob/4671b481a38f906a8ffc822b9492f2c942ad1b3e/ntoskrnl/config/cmapi.c#L854
	pKeyNode->MaxValueNameLen = REGISTRY_MAX(pKeyNode->MaxValueNameLen, NameUnicode.Length);
	KeQuerySystemTime(&pKeyNode->LastWriteTime);

	// Now, do the copy

	CmpCopyName(
		pHive,
		pKeyValue->Name,
		&NameUnicode
	);

	return STATUS_SUCCESS;

	CleanupDone:
	return Status;
}

// Value type

registry_status Registry_GetValueType(
	h_value hValue,
	uint32_t* pType
) {
	CELL_DATA* pCellData = hValue.pCellData;
	assert(pCellData->u.KeyValue.Signature == CM_KEY_VALUE_SIGNATURE);

	*pType = pCellData->u.KeyValue.Type;
	return STATUS_SUCCESS;
}

registry_status Registry_SetValueType(
	h_value hValue,
	uint32_t Type
) {
	CELL_DATA* pCellData = hValue.pCellData;
	assert(pCellData->u.KeyValue.Signature == CM_KEY_VALUE_SIGNATURE);

	HHIVE* pHive = &(*(hive_ex*)hValue.hHive).Hive;
	HvMarkCellDirty(pHive, hValue.CellIndex, TRUE);
	pCellData->u.KeyValue.Type = Type;

	return STATUS_SUCCESS;
}

// Value data

registry_status Registry_OpenValueData(
	h_value hValue,
	h_value_data* phValueData
) {
	CELL_DATA* pCellData = hValue.pCellData;
	assert(pCellData->u.KeyValue.Signature == CM_KEY_VALUE_SIGNATURE);

	BOOLEAN bAllocated; // Always FALSE
	h_value_data ValueData;
	hive_ex* pHiveEx = (hive_ex*)hValue.hHive;
	if (
		CmpGetValueData(
			&pHiveEx->Hive,
			&pCellData->u.KeyValue,
			&ValueData.nData,
			&ValueData.pData,
			&bAllocated,
			&ValueData.CellIndex
		)
	) {
		assert(!bAllocated);
		ValueData.hHive = &pHiveEx->Hive;
		*phValueData = ValueData;
		return STATUS_SUCCESS;
	} else {
		REGISTRY_STDERR("Error getting registry value data.\n");
		return STATUS_INSUFFICIENT_RESOURCES;
	}
}

registry_status Registry_CloseValueData(
	h_value_data hValueData
) {
	HHIVE* pHive = &(*(hive_ex*)hValueData.hHive).Hive;
	if (hValueData.CellIndex != HCELL_NIL) {
		HvReleaseCell(pHive, hValueData.CellIndex);
	}
	return STATUS_SUCCESS;
}

registry_status Registry_GetValueData(
	h_value_data hValueData,
	void** ppData,
	uint32_t* pnData
) {
	*ppData = hValueData.pData;
	*pnData = hValueData.nData;
	return STATUS_SUCCESS;
}

// Value enum

registry_status Registry_GetValueCount(
	h_key hKey,
	uint32_t* pResult
) {
	CELL_DATA* pCellData = hKey.pCellData;
	CM_KEY_NODE* pKeyNode = &pCellData->u.KeyNode;
	assert(pKeyNode->Signature == CM_KEY_NODE_SIGNATURE);

	*pResult = pKeyNode->ValueList.Count;
	return STATUS_SUCCESS;
}

registry_status Registry_OpenValueIterator(
	h_key hKey,
	registry_value_iterator* piIterator
) {
	CELL_DATA* pCellData = hKey.pCellData;
	CM_KEY_NODE* pKeyNode = &pCellData->u.KeyNode;
	assert(pKeyNode->Signature == CM_KEY_NODE_SIGNATURE);

	hive_ex* pHiveEx = (hive_ex*)hKey.hHive;
	HCELL_INDEX ValueListCellIndex = pKeyNode->ValueList.List;
	if (ValueListCellIndex == HCELL_NIL) {
		// This key has no value
		*piIterator = (registry_value_iterator){
			pHiveEx,
			NULL,
			0,
			ValueListCellIndex,
			0
		};
		return STATUS_SUCCESS;
	}

	CELL_DATA* pListCellData = HvGetCell(&pHiveEx->Hive, ValueListCellIndex);
	if (pListCellData == NULL) {
		REGISTRY_STDERR("Error opening registry cell.\n");
		return STATUS_INSUFFICIENT_RESOURCES;
	}

	HCELL_INDEX* aValueList = pListCellData->u.KeyList;
	assert(aValueList != NULL);

	*piIterator = (registry_value_iterator){
		pHiveEx,
		aValueList,
		pKeyNode->ValueList.Count,
		ValueListCellIndex,
		0
	};
	return STATUS_SUCCESS;
}

registry_status Registry_CloseValueIterator(
	registry_value_iterator iIterator
) {
	hive_ex* pHiveEx = (hive_ex*)iIterator.hHive;
	HvReleaseCell(&pHiveEx->Hive, iIterator.ValueListCellIndex);
	return STATUS_SUCCESS;
}

registry_status Registry_IsValueIteratorEnd(
	registry_value_iterator piNext,
	uint8_t* pbResult
) {
	*pbResult = piNext.iCurrent >= piNext.nValueList;
	return STATUS_SUCCESS;
}

registry_status Registry_OpenNextValue(
	registry_value_iterator* piNext,
	h_value* phResultValue
) {
	if (piNext->iCurrent >= piNext->nValueList)
		return STATUS_NO_MORE_ENTRIES;

	HCELL_INDEX* aValueList = piNext->aValueList;
	HCELL_INDEX ResultCellIndex = aValueList[piNext->iCurrent];

	hive_ex* pHiveEx = (hive_ex*)piNext->hHive;
	CELL_DATA* pResultCellData = HvGetCell(&pHiveEx->Hive, ResultCellIndex);
	if (pResultCellData == NULL) {
		REGISTRY_STDERR("Error opening registry cell.\n");
		return STATUS_INSUFFICIENT_RESOURCES;
	}
	assert(pResultCellData->u.KeyValue.Signature == CM_KEY_VALUE_SIGNATURE);

	++piNext->iCurrent;
	*phResultValue = (h_value){
		pHiveEx,
		pResultCellData,
		ResultCellIndex
	};
	return STATUS_SUCCESS;
}

// TODO: Registry_MoveValue
