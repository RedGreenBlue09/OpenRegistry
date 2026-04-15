#pragma once

// SPDX-License-Identifier: GPL-2.0-or-later

#include <stdint.h>
#include <stddef.h>

#include "RegistryString.h"

typedef void* h_hive;

typedef struct {
	h_hive hHive;
	void* pCellData;
	uint32_t CellIndex;
} h_key;

typedef struct {
	h_hive hHive;
	void* pCellData;
	uint32_t CellIndex;
} h_value;

typedef struct {
	h_hive hHive;
	void* pData;
	uint32_t nData;
	uint32_t CellIndex;
} h_value_data;

typedef struct {
	h_hive hHive;
	void* pKeyCellData;
	uint32_t iCurrent;
} registry_subkey_iterator;

typedef struct {
	h_hive hHive;
	void* aValueList;
	uint32_t nValueList;
	uint32_t ValueListCellIndex;
	uint32_t iCurrent;
} registry_value_iterator;

#define REGISTRY_STATUS_NO_MORE_ENTRIES ((registry_status)0x8000001A)

typedef int32_t registry_status;

typedef uint8_t hive_save_handler(void* pBuffer, size_t nBuffer, void* Parameter);

typedef enum {
	REGISTRY_NONE                       = 0,
	REGISTRY_STRING                     = 1,
	REGISTRY_EXPAND_STRING              = 2,
	REGISTRY_BINARY                     = 3,
	REGISTRY_INT32LE                    = 4, // Same as REG_DWORD
	REGISTRY_INT32BE                    = 5,
	REGISTRY_LINK                       = 6,
	REGISTRY_MULTI_STRING               = 7,
	REGISTRY_RESOURCE_LIST              = 8,
	REGISTRY_FULL_RESOURCE_DESCRIPTOR   = 9,
	REGISTRY_RESOURCE_REQUIREMENTS_LIST = 10,
	REGISTRY_INT64LE                    = 11,
} registry_value_type;

#ifndef REGISTRY_STDERR
#define REGISTRY_STDERR(...) fprintf(stderr, __VA_ARGS__);
#endif

registry_status Registry_LoadHive(
	h_hive* phHive,
	void* pFileData,
	size_t nFileData,
	hive_save_handler* pHiveSaveHandler,
	void* HiveSaveParameter
);

registry_status Registry_UnloadHive(
	h_hive hHive,
	uint8_t bForce
);

registry_status Registry_OpenRootKey(
	h_hive hHive,
	h_key* phResultKey
);

registry_status Registry_OpenKey(
	h_key hParentKey,
	const registry_string16 Name,
	h_key* phResultKey
);

registry_status Registry_CloseKey(
	h_key hKey
);

registry_status Registry_QueryKeyName(
	h_key hKey,
	registry_string16* pResult,
	size_t* pRealNameLength
);

registry_status Registry_GetSubKeyCount(
	h_key hKey,
	uint32_t* pResult
);

registry_status Registry_OpenSubKeyIterator(
	h_key hKey,
	registry_subkey_iterator* piIterator
);

registry_status Registry_CloseSubKeyIterator(
	registry_subkey_iterator iIterator
);

registry_status Registry_IsSubKeyIteratorEnd(
	registry_subkey_iterator piNext,
	uint8_t* pbResult
);

// May return REGISTRY_STATUS_NO_MORE_ENTRIES
registry_status Registry_OpenNextSubKey(
	registry_subkey_iterator* piNext,
	h_key* phResultKey
);

registry_status Registry_OpenValue(
	h_key hKey,
	const registry_string16 Name,
	h_value* phResultValue
);

registry_status Registry_CloseValue(
	h_value hValue
);

registry_status Registry_QueryValueName(
	h_value hValue,
	registry_string16* pResult,
	size_t* pRealNameLength
);

registry_status Registry_SetValueName(
	h_key hKey,
	registry_value_iterator iValue,
	h_value hValue,
	registry_string16 Name
);

registry_status Registry_GetValueType(
	h_value hValue,
	uint32_t* pType
);

registry_status Registry_OpenValueData(
	h_value hValue,
	h_value_data* phValueData
);

registry_status Registry_CloseValueData(
	h_value_data hValueData
);

registry_status Registry_GetValueData(
	h_value_data hValueData,
	void** ppData,
	uint32_t* pnData
);

registry_status Registry_SetValueData(
	h_key hKey,
	h_value hValue,
	h_value_data* phValueData,
	const void* pData,
	uint32_t nData
);

registry_status Registry_GetValueCount(
	h_key hKey,
	uint32_t* pResult
);

registry_status Registry_OpenValueIterator(
	h_key hKey,
	registry_value_iterator* piIterator
);

registry_status Registry_CloseValueIterator(
	registry_value_iterator iIterator
);

registry_status Registry_IsValueIteratorEnd(
	registry_value_iterator piNext,
	uint8_t* pbResult
);

// May return REGISTRY_STATUS_NO_MORE_ENTRIES
registry_status Registry_OpenNextValue(
	registry_value_iterator* piNext,
	h_value* phResultValue
);

registry_status Registry_DeleteValue(
	h_key hKey,
	registry_value_iterator* piValue,
	h_value hValue
);

static registry_status Registry_ValidateValueData(
	uint32_t Type,
	void* pData,
	uint32_t nData
);

// Returns error if the existing data is invalid
// or the existing type is different.
registry_status Registry_OpenAndGetValueDataWithType(
	h_value hValue,
	h_value_data* phValueData,
	void** ppData,
	uint32_t* pnData,
	registry_value_type ExpectedType
);

// Returns error if the existing data is invalid
// or the existing type is different. In this case, no data is copied.
// If the function succeed, the data copied is truncated to nData.
// Check pRealSize to detect this.
registry_status Registry_QueryValueDataWithType(
	h_value hValue,
	void* pData,
	uint32_t nData,
	uint32_t* pRealSize,
	registry_value_type ExpectedType
);

// Converts data to native endianness for integer types.
// Returns error if the existing data is invalid
// or the existing type is different. In this case, no data is copied.
// If the function succeed, the data copied is truncated to nData.
// Check pRealSize to detect this.
registry_status Registry_QueryValueDataWithTypeNative(
	h_value hValue,
	void* pData,
	uint32_t nData,
	uint32_t* pRealSize,
	registry_value_type ExpectedType
);
