
// SPDX-License-Identifier: MIT

#include "Common.h"

// Key
// I'm not sure how to use HvGetCell and HvReleaseCell
// Looking at the cmlib code,
// they doesn't seem to do any locking yet.

registry_status Registry_OpenRootKey(
	h_hive hHive,
	h_key* phResultKey
) {
	HHIVE* pHive = &(*(hive_ex*)hHive).Hive;
	HCELL_INDEX CellIndex = pHive->BaseBlock->RootCell;
	if (CellIndex == HCELL_NIL) {
		REGISTRY_STDERR("Error: Registry hive has no root key.\n");
		return STATUS_NOT_FOUND;
	}

	CELL_DATA* pCellData = HvGetCell(pHive, CellIndex);
	if (pCellData == NULL) {
		REGISTRY_STDERR("Error opening registry cell.\n");
		return STATUS_INSUFFICIENT_RESOURCES;
	}
	assert(pCellData->u.KeyNode.Signature == CM_KEY_NODE_SIGNATURE);

	*phResultKey = (h_key){ hHive, pCellData, CellIndex };
	return STATUS_SUCCESS;
}

registry_status Registry_OpenKey(
	h_key hParentKey,
	const registry_string16 Name,
	h_key* phResultKey
) {
	hive_ex* pHiveEx = (hive_ex*)hParentKey.hHive;
	HHIVE* pHive = &pHiveEx->Hive;
	CELL_DATA* pParentCellData = hParentKey.pCellData;
	assert(pParentCellData->u.KeyNode.Signature == CM_KEY_NODE_SIGNATURE);

	UNICODE_STRING NameUnicode;
	registry_status Status = RegistryStringToUnicodeString(Name, &NameUnicode);
	if (Status != STATUS_SUCCESS) {
		REGISTRY_STDERR("Error: The registry string is too long.\n");
		return Status;
	}

	HCELL_INDEX ResultCellIndex =
		CmpFindSubKeyByName(pHive, &pParentCellData->u.KeyNode, &NameUnicode);
	if (ResultCellIndex == HCELL_NIL) {
		return STATUS_NOT_FOUND;
	}

	CELL_DATA* pResultCellData = HvGetCell(pHive, ResultCellIndex);
	if (pResultCellData == NULL) {
		REGISTRY_STDERR("Error opening registry cell.\n");
		return STATUS_INSUFFICIENT_RESOURCES;
	}
	assert(pResultCellData->u.KeyNode.Signature == CM_KEY_NODE_SIGNATURE);

	*phResultKey = (h_key){pHiveEx, pResultCellData, ResultCellIndex};
	return STATUS_SUCCESS;
}

registry_status Registry_CloseKey(
	h_key hKey
) {
	CELL_DATA* pCellData = hKey.pCellData;
	(void)pCellData;
	assert(pCellData->u.KeyNode.Signature == CM_KEY_NODE_SIGNATURE);

	HHIVE* pHive = &(*(hive_ex*)hKey.hHive).Hive;
	HvReleaseCell(pHive, hKey.CellIndex);
	return STATUS_SUCCESS;
}

registry_status Registry_QueryKeyName(
	h_key hKey,
	registry_string16* pResult,
	size_t* pRealNameLength
) {
	CELL_DATA* pCellData = hKey.pCellData;
	CM_KEY_NODE* pKeyNode = &pCellData->u.KeyNode;
	assert(pKeyNode->Signature == CM_KEY_NODE_SIGNATURE);

	USHORT RealNameSize;
	if (pKeyNode->Flags & KEY_COMP_NAME)
		RealNameSize = CmpCompressedNameSize(pKeyNode->Name, pKeyNode->NameLength);
	else
		RealNameSize = pKeyNode->NameLength;

	USHORT NameLength = RealNameSize / sizeof(uint16_t);
	*pRealNameLength = NameLength;
	NameLength = (USHORT)min((size_t)NameLength, pResult->AllocatedLength);
	pResult->Length = NameLength;
	USHORT NameSize = NameLength * sizeof(uint16_t);

	if (pKeyNode->Flags & KEY_COMP_NAME) {
		CmpCopyCompressedName(
			pResult->aBuffer,
			NameSize,
			pKeyNode->Name,
			RealNameSize
		);
	} else {
		memcpy(pResult->aBuffer, pKeyNode->Name, NameSize);
	}
	return STATUS_SUCCESS;
}

// Enum key

registry_status Registry_GetSubKeyCount(
	h_key hKey,
	uint32_t* pResult
) {
	HHIVE* pHive = &(*(hive_ex*)hKey.hHive).Hive;

	CELL_DATA* pKeyCellData = hKey.pCellData;
	assert(pKeyCellData->u.KeyNode.Signature == CM_KEY_NODE_SIGNATURE);

	uint32_t Result = 0;
	while (CmpFindSubKeyByNumber(pHive, &pKeyCellData->u.KeyNode, Result) != HCELL_NIL)
		++Result;

	*pResult = Result;
	return STATUS_SUCCESS;
}

registry_status Registry_OpenSubKeyIterator(
	h_key hKey,
	registry_subkey_iterator* piIterator
) {
	*piIterator = (registry_subkey_iterator){ hKey.hHive, hKey.pCellData, 0 };
	return STATUS_SUCCESS;
}

registry_status Registry_CloseSubKeyIterator(
	registry_subkey_iterator iIterator
) {
	(void)iIterator;
	return STATUS_SUCCESS;
}

registry_status Registry_IsSubKeyIteratorEnd(
	registry_subkey_iterator piNext,
	uint8_t* pbResult
) {
	*pbResult = piNext.iCurrent == UINT32_MAX;
	return STATUS_SUCCESS;
}

registry_status Registry_OpenNextSubKey(
	registry_subkey_iterator* piNext,
	h_key* phResultKey
) {
	hive_ex* pHiveEx = (hive_ex*)piNext->hHive;
	HHIVE* pHive = &pHiveEx->Hive;

	CELL_DATA* pKeyCellData = piNext->pKeyCellData;
	assert(pKeyCellData->u.KeyNode.Signature == CM_KEY_NODE_SIGNATURE);

	HCELL_INDEX ResultCellIndex = CmpFindSubKeyByNumber(
		pHive,
		&pKeyCellData->u.KeyNode,
		piNext->iCurrent
	);

	if (ResultCellIndex == HCELL_NIL) {
		piNext->iCurrent = UINT32_MAX;
		return STATUS_NO_MORE_ENTRIES;
	}

	CELL_DATA* pResultCellData = HvGetCell(pHive, ResultCellIndex);
	if (pResultCellData == NULL) {
		REGISTRY_STDERR("Error opening registry cell.\n");
		return STATUS_INSUFFICIENT_RESOURCES;
	}
	assert(pResultCellData->u.KeyNode.Signature == CM_KEY_NODE_SIGNATURE);

	++piNext->iCurrent;
	*phResultKey = (h_key){pHiveEx, pResultCellData, ResultCellIndex};
	return STATUS_SUCCESS;
}
