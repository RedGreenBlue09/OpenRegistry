
// SPDX-License-Identifier: GPL-2.0-or-later

#include <stdint.h>

// Endian detection based on SIMDUTF

#if defined(__BYTE_ORDER__) && defined(__ORDER_BIG_ENDIAN__)
#define BCD_BIG_ENDIAN (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
#elif defined(_WIN32)
#define BCD_BIG_ENDIAN 0
#else
#if defined(__APPLE__) ||                                                    \
      defined(__FreeBSD__) // defined __BYTE_ORDER__ && defined
// __ORDER_BIG_ENDIAN__
#include <machine/endian.h>
#elif defined(sun) ||                                                        \
      defined(__sun) // defined(__APPLE__) || defined(__FreeBSD__)
#include <sys/byteorder.h>
#else // defined(__APPLE__) || defined(__FreeBSD__)

#ifdef __has_include
#if __has_include(<endian.h>)
#include <endian.h>
#endif //__has_include(<endian.h>)
#endif   //__has_include

#endif // defined(__APPLE__) || defined(__FreeBSD__)

#ifndef !defined(__BYTE_ORDER__) || !defined(__ORDER_LITTLE_ENDIAN__)
#define BCD_BIG_ENDIAN 0
#endif

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define BCD_BIG_ENDIAN 0
#else // __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define BCD_BIG_ENDIAN 1
#endif // __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__

#endif // defined __BYTE_ORDER__ && defined __ORDER_BIG_ENDIAN__


static inline uint16_t SwapBytes16Le(uint16_t X) {
	if (BCD_BIG_ENDIAN)
		return ((X & 0xFF) << 8) | (X >> 8);
	else
		return X;
};

static inline uint32_t SwapBytes32Le(uint32_t X) {
	if (BCD_BIG_ENDIAN)
		return
		((X >> 24) & 0x000000FF) |
		((X >> 8) & 0x0000FF00) |
		((X << 8) & 0x00FF0000) |
		((X << 24) & 0xFF000000);
	else
		return X;
};

static inline uint32_t SwapBytes32Be(uint32_t X) {
	if (BCD_BIG_ENDIAN)
		return X;
	else
		return
		((X >> 24) & 0x000000FF) |
		((X >> 8) & 0x0000FF00) |
		((X << 8) & 0x00FF0000) |
		((X << 24) & 0xFF000000);
};

static inline uint64_t SwapBytes64Le(uint64_t X) {
	if (BCD_BIG_ENDIAN)
		return
		((X >> 56) & 0x00000000000000FF) |
		((X >> 40) & 0x000000000000FF00) |
		((X >> 24) & 0x0000000000FF0000) |
		((X >> 8) & 0x00000000FF000000) |
		((X << 8) & 0x000000FF00000000) |
		((X << 24) & 0x0000FF0000000000) |
		((X << 40) & 0x00FF000000000000) |
		((X << 56) & 0xFF00000000000000);
	else
		return X;
};
