
#include <RegistryString.h>

#include "Endian.h"
#include <string.h>
#include "SimdUtf/SimdUtf.h"

#define REGISTRY_WCHAR16 (WCHAR_MAX == UINT16_MAX || WCHAR_MAX == INT16_MAX)
#define REGISTRY_WCHAR32 (WCHAR_MAX == UINT32_MAX || WCHAR_MAX == INT32_MAX)
#define BCD_MIN(A, B) ((A) < (B) ? (A) : (B))

static inline void swap_string_utf16le(
	const uint16_t* aInput,
	size_t nInput,
	uint16_t* aOutput
) {
	for (size_t i = 0; i < nInput; ++i)
		aOutput[i] = SwapBytes16Le(aInput[i]);
}

size_t Registry_CountString16(uint16_t* sInput) {
	size_t i;
	for (i = 0; sInput[i] != 0; ++i);
	return i;
}

size_t Registry_FixString16(
	registry_string16 Input,
	uint16_t* aOutput
) {
	return to_well_formed_utf16le(Input.aBuffer, Input.Length, aOutput);
}

size_t Registry_CountWideStringFromString16(
	registry_string16 Input
) {
#if REGISTRY_WCHAR16

	return Input.Length;

#elif REGISTRY_WCHAR32

	return utf32_length_from_valid_utf16le(Input.aBuffer, Input.Length);

#endif
}

size_t Registry_ConvertString16ToWideString(
	registry_string16 Input,
	wchar_t* aOutput,
	size_t nOutput
) {
#if REGISTRY_WCHAR16

	nOutput = BCD_MIN(Input.Length, nOutput);

	#if BCD_BIG_ENDIAN

	swap_string_utf16le(Input.aBuffer, nOutput, (uint16_t*)aOutput);

	#else

	memcpy(aOutput, Input.aBuffer, nOutput * sizeof(uint16_t));

	#endif

	return nOutput;

#elif REGISTRY_WCHAR32

	return convert_valid_utf16le_to_utf32(
		Input.aBuffer,
		Input.Length, 
		(uint32_t*)aOutput,
		nOutput
	);

#endif
}

size_t Registry_FixWideString(
	const wchar_t* aInput,
	size_t nInput,
	wchar_t* aOutput
) {
#if REGISTRY_WCHAR16

	return to_well_formed_utf16((uint16_t*)aInput, nInput, (uint16_t*)aOutput);

#elif REGISTRY_WCHAR32

	return to_well_formed_utf32((uint32_t*)aInput, nInput, (uint32_t*)aOutput);

#endif
}

size_t Registry_CountString16FromWideString(
	const wchar_t* aInput,
	size_t nInput
) {
#if REGISTRY_WCHAR16

	return nInput;

#elif REGISTRY_WCHAR32

	return utf16le_length_from_valid_utf32((uint32_t*)aInput, nInput);

#endif
}

void Registry_ConvertWideStringToString16(
	const wchar_t* aInput,
	size_t nInput,
	registry_string16* pOutput
) {
#if REGISTRY_WCHAR16

	pOutput->Length = BCD_MIN(nInput, pOutput->AllocatedLength);

	#if BCD_BIG_ENDIAN

	swap_string_utf16le((uint16_t*)aInput, pOutput->Length, pOutput->aBuffer);

	#else

	memcpy(pOutput->aBuffer, aInput, pOutput->Length * sizeof(uint16_t));

	#endif

#elif REGISTRY_WCHAR32

	pOutput->Length =
		convert_valid_utf32_to_utf16le(
			(uint32_t*)aInput,
			nInput,
			pOutput->aBuffer,
			pOutput->AllocatedLength
		);

#endif
}

size_t Registry_ConvertString16ToWideStringAscii(
	registry_string16 Input,
	wchar_t* aOutput,
	size_t nOutput
) {
	nOutput = BCD_MIN(Input.Length, nOutput);

#if REGISTRY_WCHAR16

	#if BCD_BIG_ENDIAN

	swap_string_utf16le(Input.aBuffer, nOutput, (uint16_t*)aOutput);

	#else

	memcpy(aOutput, Input.aBuffer, nOutput * sizeof(uint16_t));

	#endif

#elif REGISTRY_WCHAR32

	// For ASCII-only input, convert UTF-16LE code units (0x00XX) to UTF-32 (0x000000XX)
	
	for (size_t i = 0; i < nOutput; ++i)
		aOutput[i] = SwapBytes16Le(Input.aBuffer[i]);

#endif

	return nOutput;
}

void Registry_ConvertWideStringToString16Ascii(
	const wchar_t* aInput,
	size_t nInput,
	registry_string16* pOutput
) {
	pOutput->Length = BCD_MIN(nInput, pOutput->AllocatedLength);

#if REGISTRY_WCHAR16

	#if BCD_BIG_ENDIAN

	swap_string_utf16le((uint16_t*)aInput, pOutput->Length, pOutput->aBuffer);

	#else

	memcpy(pOutput->aBuffer, aInput, pOutput->Length * sizeof(uint16_t));

	#endif

#elif REGISTRY_WCHAR32

	for (size_t i = 0; i < pOutput->Length; ++i)
		pOutput->aBuffer[i] = SwapBytes16Le((uint16_t)aInput[i]);

#endif
}
