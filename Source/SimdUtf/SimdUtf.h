#pragma once

// SPDX-License-Identifier: MIT

#include <stddef.h>
#include <stdint.h>

size_t convert_valid_utf16le_to_utf32(
	const uint16_t* data,
	size_t len,
	uint32_t* utf32_output,
	size_t out_len
);

size_t to_well_formed_utf16le(
	const uint16_t* input,
	size_t len,
	uint16_t* output
);

size_t to_well_formed_utf16(
	const uint16_t* input,
	size_t len,
	uint16_t* output
);

size_t utf32_length_from_valid_utf16le(
	const uint16_t* p,
	size_t len
);

size_t convert_valid_utf32_to_utf16le(
	const uint32_t* data,
	size_t len,
	uint16_t* utf16_output,
	size_t out_len
);

size_t to_well_formed_utf32(
	const uint32_t* input,
	size_t len,
	uint32_t* output
);

size_t utf16le_length_from_valid_utf32(
	const uint32_t* p,
	size_t len
);
