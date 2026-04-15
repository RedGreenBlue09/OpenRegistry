
// SPDX-License-Identifier: MIT

#include "SimdUtf.h"
#include <assert.h>

#include "../RegistryInternal.h"

static inline uint32_t conditional_select_u32(uint32_t condition, uint32_t if_true, uint32_t if_false) {
    assert(condition <= 1);
    uint32_t mask = 0 - condition;
    return (mask & if_true) | (~mask & if_false);
}

static inline uint16_t conditional_select_u16(uint16_t condition, uint16_t if_true, uint16_t if_false) {
    assert(condition <= 1);
    uint16_t mask = 0 - condition;
    return (mask & if_true) | (~mask & if_false);
}

// Implemented based on SimdUtf but without SIMD

size_t convert_valid_utf16le_to_utf32(
    const uint16_t* data,
    size_t len, uint32_t*
    utf32_output,
    size_t out_len
) {
    size_t pos = 0;
    size_t out_pos = 0;
    while ((pos + 1 < len) && (out_pos < out_len)) {
        uint16_t word = SwapBytes16Le(data[pos]);
        uint32_t b = ((word & 0xF800) == 0xD800);

        uint16_t diff = word - 0xD800;
        assert(!b || diff <= 0x3FF);
        uint16_t next_word = SwapBytes16Le(data[pos + 1]);
        uint16_t diff2 = next_word - 0xDC00;
        assert(!b || diff2 <= 0x3FF);
        uint32_t value = (diff << 10) + diff2 + 0x10000;

        utf32_output[out_pos++] = conditional_select_u32(b, value, word);
        pos += 1 + b;
    }

    // If there's a remainder char, write it
    if ((pos + 1 == len) && (out_pos < out_len)) {
        uint16_t word = SwapBytes16Le(data[pos]);
        utf32_output[out_pos++] = word;
    }
    return out_pos;
}

size_t to_well_formed_utf16le(const uint16_t* input, size_t len, uint16_t* output) {
    if (len == 0)
        return 0;

    uint16_t replacement = 0xFFFD;

    uint16_t c = SwapBytes16Le(input[0]);
    uint16_t high_surrogate = ((0xD800 <= c) & (c <= 0xDBFF));
    uint16_t low_surrogate = ((0xDC00 <= c) & (c <= 0xDFFF));
    uint16_t b = low_surrogate;
    size_t replaced = b;

    uint16_t c1 = conditional_select_u16(b, replacement, c);
    output[0] = SwapBytes16Le(c1);

    if (len == 1)
        return replaced;

    uint16_t high_surrogate_prev = high_surrogate;

    size_t i;
    for (i = 1; i < len; i++) {
        c = SwapBytes16Le(input[i]);
        high_surrogate = ((0xD800 <= c) & (c <= 0xDBFF));
        low_surrogate = ((0xDC00 <= c) & (c <= 0xDFFF));

        b = (high_surrogate_prev & (!low_surrogate));
        replaced += b;
        c1 = conditional_select_u16(b, replacement, c1);
        output[i - 1] = SwapBytes16Le(c1);

        b = ((!high_surrogate_prev) & low_surrogate);
        replaced += b;
        c1 = conditional_select_u16(b, replacement, c);
        output[i] = SwapBytes16Le(c1);

        high_surrogate_prev = high_surrogate;
    }

    b = high_surrogate_prev;
    replaced += b;
    c1 = conditional_select_u16(b, replacement, c1);
    output[i - 1] = SwapBytes16Le(c1);

    return replaced;
}

size_t to_well_formed_utf16(const uint16_t* input, size_t len, uint16_t* output) {
    if (len == 0)
        return 0;

    uint16_t replacement = 0xFFFD;

    uint16_t c = input[0];
    uint16_t high_surrogate = ((0xD800 <= c) & (c <= 0xDBFF));
    uint16_t low_surrogate = ((0xDC00 <= c) & (c <= 0xDFFF));
    uint16_t b = low_surrogate;
    size_t replaced = b;

    uint16_t c1 = conditional_select_u16(b, replacement, c);
    output[0] = c1;

    if (len == 1)
        return replaced;

    uint16_t high_surrogate_prev = high_surrogate;

    size_t i;
    for (i = 1; i < len; i++) {
        c = input[i];
        high_surrogate = ((0xD800 <= c) & (c <= 0xDBFF));
        low_surrogate = ((0xDC00 <= c) & (c <= 0xDFFF));

        b = (high_surrogate_prev & (!low_surrogate));
        replaced += b;
        c1 = conditional_select_u16(b, replacement, c1);
        output[i - 1] = c1;

        b = ((!high_surrogate_prev) & low_surrogate);
        replaced += b;
        c1 = conditional_select_u16(b, replacement, c);
        output[i] = c1;

        high_surrogate_prev = high_surrogate;
    }

    b = high_surrogate_prev;
    replaced += b;
    c1 = conditional_select_u16(b, replacement, c1);
    output[i - 1] = c1;

    return replaced;
}

size_t utf32_length_from_valid_utf16le(const uint16_t* p, size_t len) {
    // We are not BOM aware.
    size_t counter = 0;
    for (size_t i = 0; i < len; i++) {
        uint16_t word = SwapBytes16Le(p[i]);
        // TODO: Some correctness asserts
        counter += ((word & 0xFC00) != 0xDC00);
    }
    return counter;
}

size_t convert_valid_utf32_to_utf16le(
    const uint32_t* data,
    size_t len,
    uint16_t* utf16_output,
    size_t out_len
) {
    size_t out_pos = 0;
    for (size_t pos = 0; pos < len; pos++) {
        uint32_t word = data[pos];
        assert(!((word > 0x10FFFF) | ((word >= 0xD800) & (word <= 0xDFFF))));
        uint16_t b = ((word & 0xFFFF0000) != 0);
        if (out_pos + b >= out_len)
            break;
        word -= 0x10000; // This does not change (uint16_t)word
        uint16_t high_surrogate = (uint16_t)(0xD800 + (word >> 10));
        uint16_t low_surrogate = (uint16_t)(0xDC00 + (word & 0x3FF));
        utf16_output[out_pos] = conditional_select_u16(b, high_surrogate, 0);
        utf16_output[out_pos + b] = conditional_select_u16(b, low_surrogate, (uint16_t)word);
        out_pos += 1 + b;
    }
    return out_pos;
}

size_t to_well_formed_utf32(const uint32_t* input, size_t len, uint32_t* output) {
    static const uint32_t replacement = 0xFFFD;
    size_t replaced = 0;
    for (size_t i = 0; i < len; i++) {
        uint32_t word = input[i];
        uint8_t b = ((word > 0x10FFFF) | ((word >= 0xD800) & (word <= 0xDFFF)));
        replaced += b;
        output[i] = conditional_select_u32(b, replacement, word);
    }
    return replaced;
}

size_t utf16le_length_from_valid_utf32(const uint32_t* p, size_t len) {
    // We are not BOM aware.
    size_t counter = 0;
    for (size_t i = 0; i < len; i++) {
        uint32_t word = p[i];
        assert(!(word > 0x10FFFF) | ((word >= 0xD800) & (word <= 0xDFFF)));
        counter++; // non-surrogate word
        counter += (word > 0xFFFF); // surrogate pair
    }
    return counter;
}
