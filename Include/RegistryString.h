#pragma once

// SPDX-License-Identifier: GPL-2.0-or-later

#include <stdint.h>
#include <wchar.h>

typedef struct {
	size_t Length;
	size_t AllocatedLength;
	uint16_t* aBuffer;
} registry_string16;

#define REGISTRY_ARRLEN_LITERAL(X) (sizeof(X) / sizeof(*(X)))
#define REGISTRY_STRLEN_LITERAL(X) (REGISTRY_ARRLEN_LITERAL(X) - 1)

#define REGISTRY_STRING_INIT_BUFFER(aBuffer)   \
	{                                          \
		REGISTRY_ARRLEN_LITERAL(aBuffer),      \
		REGISTRY_ARRLEN_LITERAL(aBuffer),      \
		aBuffer                                \
	}

// Hopefully the compiler can optimize this into static data.
#define REGISTRY_STRING_DECLARE_ASCII_LITERAL(BufferName, StringName, Literal)                         \
	uint16_t BufferName[REGISTRY_STRLEN_LITERAL(Literal)];                                             \
	registry_string16 StringName = {0, REGISTRY_ARRLEN_LITERAL(BufferName), BufferName};               \
	Registry_ConvertWideStringToString16Ascii(Literal, REGISTRY_STRLEN_LITERAL(Literal), &StringName); \


#define REGISTRY_STRING_DECLARE_FIXED(BufferName, StringName, Length) \
	uint16_t BufferName[(Length)];                            \
	registry_string16 StringName = {0, (Length), BufferName}; \


// Assumes output length >= input length
// Return number of errors
size_t Registry_FixString16(
	registry_string16 Input,
	uint16_t *aOutput
);

// Requires correct UTF-16LE
size_t Registry_CountWideStringFromString16(
	registry_string16 Input
);

// Requires correct UTF-16LE
size_t Registry_ConvertString16ToWideString(
	registry_string16 Input,
	wchar_t* aOutput,
	size_t nOutput
);

// Assumes output length >= input length
// Return number of errors
size_t Registry_FixWideString(
	const wchar_t* aInput,
	size_t nInput,
	wchar_t* aOutput
);

// Requires correct UTF-32
size_t Registry_CountString16FromWideString(
	const wchar_t* aInput,
	size_t nInput
);

// Requires correct UTF-32
void Registry_ConvertWideStringToString16(
	const wchar_t* aInput,
	size_t nInput,
	registry_string16* pOutput
);

// Input is ASCII-only
size_t Registry_ConvertString16ToWideStringAscii(
	registry_string16 Input,
	wchar_t* aOutput,
	size_t nOutput
);

// Input is ASCII-only
void Registry_ConvertWideStringToString16Ascii(
	const wchar_t* aInput,
	size_t nInput,
	registry_string16* pOutput
);

// TODO: strcmp
