
// SPDX-License-Identifier: GPL-2.0-or-later

#include "Common.h"

// Alloc

PVOID
NTAPI
CmpAllocate(
	IN SIZE_T Size,
	IN BOOLEAN Paged,
	IN ULONG Tag
) {
	(void)Paged;
	(void)Tag;
	return malloc(Size);
}

VOID
NTAPI
CmpFree(
	IN PVOID Ptr,
	IN ULONG Quota
) {
	(void)Quota;
	free(Ptr);
}

// Memory file

static BOOLEAN CMAPI MemoryFileSetSize(
	HHIVE* pRegistryHive,
	ULONG FileType,
	ULONG FileSize,
	ULONG OldfileSize
) {
	(void)FileType;
	(void)OldfileSize;
	hive_ex* pHiveEx = (hive_ex*)pRegistryHive;

	uint8_t* pNewBuffer = realloc(pHiveEx->pBuffer, FileSize);
	if (pNewBuffer == NULL) {
		// FIXME: Is this leaking?
		return FALSE;
	} else {
		pHiveEx->pBuffer = pNewBuffer;
		pHiveEx->nBuffer = FileSize;
		return TRUE;
	}
}

static BOOLEAN CMAPI MemoryFileWrite(
	HHIVE* pRegistryHive,
	ULONG FileType,
	PULONG pFileOffset,
	PVOID pBuffer,
	SIZE_T BufferLength
) {
	// IDK why pFileOffset is a pointer.
	// It doesn't seem like I have to write anything back to it.
	(void)FileType;
	hive_ex* pHiveEx = (hive_ex*)pRegistryHive;
	memcpy(&pHiveEx->pBuffer[*pFileOffset], pBuffer, BufferLength);
	return TRUE;
}

static BOOLEAN CMAPI MemoryFileRead(
	HHIVE* pRegistryHive,
	ULONG FileType,
	PULONG pFileOffset,
	PVOID pBuffer,
	SIZE_T BufferLength
) {
	// IDK why pFileOffset is a pointer.
	// It doesn't seem like I have to write anything back to it.
	(void)FileType;
	hive_ex* pHiveEx = (hive_ex*)pRegistryHive;
	memcpy(pBuffer, &pHiveEx->pBuffer[*pFileOffset], BufferLength);
	return TRUE;
}

static BOOLEAN CMAPI MemoryFileFlush(
	HHIVE* pRegistryHive,
	ULONG FileType,
	PLARGE_INTEGER pFileOffset,
	ULONG Length
) {
	// Doesn't seem like pFileOffset and Length is used at all.
	(void)FileType;
	(void)pFileOffset;
	(void)Length;

	hive_ex* pHiveEx = (hive_ex*)pRegistryHive;
	return pHiveEx->pHiveSaveHandler(
		pHiveEx->pBuffer,
		pHiveEx->nBuffer,
		pHiveEx->HiveSaveParameter
	);
}

// Hive

registry_status Registry_LoadHive(
	h_hive* phHive,
	void* pFileData,
	size_t nFileData,
	hive_save_handler* pHiveSaveHandler,
	void* HiveSaveParameter
) {
	hive_ex* pHiveEx = malloc(sizeof(*pHiveEx));
	if (pHiveEx == NULL) {
		REGISTRY_STDERR("Error allocating memory for registry hive.\n");
		return STATUS_NO_MEMORY;
	}

	pHiveEx->pBuffer = malloc(nFileData);
	if (pHiveEx->pBuffer == NULL) {
		REGISTRY_STDERR("Error allocating memory for registry hive.\n");
		free(pHiveEx->pBuffer);
		return STATUS_NO_MEMORY;
	}
	memcpy(pHiveEx->pBuffer, pFileData, nFileData);

	registry_status Status = HvInitialize(
		&pHiveEx->Hive,
		HINIT_FILE,
		0,
		HFILE_TYPE_PRIMARY,
		NULL,
		CmpAllocate,
		CmpFree,
		MemoryFileSetSize,
		MemoryFileWrite,
		MemoryFileRead,
		MemoryFileFlush,
		1,
		NULL
	);
	if (Status != STATUS_SUCCESS && Status != STATUS_REGISTRY_RECOVERED) {
		REGISTRY_STDERR("Error initializing registry hive. NTSTATUS: 0x%"PRIX32"\n", Status);
		free(pHiveEx->pBuffer);
		free(pHiveEx);
		return Status;
	}

	assert(pHiveSaveHandler != NULL);
	pHiveEx->pHiveSaveHandler = pHiveSaveHandler;
	pHiveEx->HiveSaveParameter = HiveSaveParameter;

	*phHive = pHiveEx;
	return STATUS_SUCCESS;
}

registry_status Registry_UnloadHive(
	h_hive hHive,
	uint8_t bForce
) {
	hive_ex* pHiveEx = hHive;
	registry_status Status = STATUS_SUCCESS;
	if (!HvSyncHive(&pHiveEx->Hive)) {
		REGISTRY_STDERR("Error synchronizing registry hive.\n");
		Status = STATUS_REGISTRY_IO_FAILED;
		if (!bForce)
			return Status;
	}

	HvFree(&pHiveEx->Hive);
	free(pHiveEx->pBuffer);
	free(pHiveEx);
	return Status;
}
