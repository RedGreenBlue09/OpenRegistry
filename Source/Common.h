
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <Registry.h>
#include "Endian.h"

#include <cmlib.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>

#define STATUS_UNIMPLEMENTED        ((NTSTATUS)0xC0000002)
#define STATUS_BUFFER_OVERFLOW      ((NTSTATUS)0x80000005)
#define STATUS_NO_MORE_ENTRIES      ((NTSTATUS)0x8000001A)
#define STATUS_UNSUCCESSFUL         ((NTSTATUS)0xC0000001)
#define STATUS_NOT_IMPLEMENTED      ((NTSTATUS)0xC0000002)
#define STATUS_INFO_LENGTH_MISMATCH ((NTSTATUS)0xC0000004)
#define STATUS_INVALID_DATA         ((NTSTATUS)0xC000000D)
#define STATUS_OBJECT_TYPE_MISMATCH ((NTSTATUS)0xC0000024)
#define STATUS_NAME_TOO_LONG        ((NTSTATUS)0xC0000106)
#define STATUS_NO_LOG_SPACE 	    ((NTSTATUS)0xC000017D)
#define STATUS_NOT_FOUND            ((NTSTATUS)0xC0000225)

typedef struct {
	HHIVE Hive;

	// Simple memory file.
	uint8_t* pBuffer;
	size_t nBuffer;

	// Hive save handler
	hive_save_handler* pHiveSaveHandler;
	void* HiveSaveParameter;
} hive_ex;


static inline registry_status RegistryStringToUnicodeString(
	registry_string16 RegistryString,
	UNICODE_STRING* pUnicodeString
) {
	if (
		RegistryString.Length > USHORT_MAX / sizeof(uint16_t) ||
		RegistryString.AllocatedLength > USHORT_MAX / sizeof(uint16_t)
	) {
		return STATUS_NAME_TOO_LONG;
	}
	USHORT Size = (USHORT)(RegistryString.Length * sizeof(uint16_t));
	USHORT AllocatedSize = (USHORT)(RegistryString.AllocatedLength * sizeof(uint16_t));
	*pUnicodeString = (UNICODE_STRING){Size, AllocatedSize, RegistryString.aBuffer};
	return STATUS_SUCCESS;
}
