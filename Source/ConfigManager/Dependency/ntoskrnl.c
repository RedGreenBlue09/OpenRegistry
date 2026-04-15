
// SPDX-License-Identifier: GPL-2.0-or-later

#include <intrin.h>
#include <inttypes.h>
#include <stdio.h>
#include <time.h>
#include <typedefs.h>
#include <wchar.h>

#define UNICODE_NULL ((WCHAR)0)
#define CONST const

// https://github.com/reactos/reactos/blob/4cba65d760a730ea4b77ab6fee857e563c0984d8/sdk/lib/rtl/nls.c#L204
// https://github.com/reactos/reactos/blob/4cba65d760a730ea4b77ab6fee857e563c0984d8/modules/rostests/apitests/advapi32/IsTextUnicode.c#L24
// https://github.com/reactos/reactos/blob/4cba65d760a730ea4b77ab6fee857e563c0984d8/media/nls/l_intl.nls
// https://github.com/reactos/reactos/blob/4cba65d760a730ea4b77ab6fee857e563c0984d8/sdk/tools/create_nls/create_nls.c#L926

USHORT NlsUnicodeUpcaseTable[] = {
    0x0100, 0x0110, 0x0120, 0x0130, 0x0140, 0x0150, 0x0160, 0x0160,
    0x0160, 0x0160, 0x0160, 0x0160, 0x0160, 0x0160, 0x0160, 0x0160,
    0x0160, 0x0160, 0x0160, 0x0160, 0x0160, 0x0160, 0x0160, 0x0160,
    0x0160, 0x0160, 0x0160, 0x0160, 0x0160, 0x0160, 0x0170, 0x0180,
    0x0160, 0x0190, 0x0160, 0x0160, 0x01A0, 0x0160, 0x0160, 0x0160,
    0x0160, 0x0160, 0x0160, 0x0160, 0x0160, 0x0160, 0x0160, 0x0160,
    0x0160, 0x0160, 0x0160, 0x0160, 0x0160, 0x0160, 0x0160, 0x0160,
    0x0160, 0x0160, 0x0160, 0x0160, 0x0160, 0x0160, 0x0160, 0x0160,
    0x0160, 0x0160, 0x0160, 0x0160, 0x0160, 0x0160, 0x0160, 0x0160,
    0x0160, 0x0160, 0x0160, 0x0160, 0x0160, 0x0160, 0x0160, 0x0160,
    0x0160, 0x0160, 0x0160, 0x0160, 0x0160, 0x0160, 0x0160, 0x0160,
    0x0160, 0x0160, 0x0160, 0x0160, 0x0160, 0x0160, 0x0160, 0x0160,
    0x0160, 0x0160, 0x0160, 0x0160, 0x0160, 0x0160, 0x0160, 0x0160,
    0x0160, 0x0160, 0x0160, 0x0160, 0x0160, 0x0160, 0x0160, 0x0160,
    0x0160, 0x0160, 0x0160, 0x0160, 0x0160, 0x0160, 0x0160, 0x0160,
    0x0160, 0x0160, 0x0160, 0x0160, 0x0160, 0x0160, 0x0160, 0x0160,
    0x0160, 0x0160, 0x0160, 0x0160, 0x0160, 0x0160, 0x0160, 0x0160,
    0x0160, 0x0160, 0x0160, 0x0160, 0x0160, 0x0160, 0x0160, 0x0160,
    0x0160, 0x0160, 0x0160, 0x0160, 0x0160, 0x0160, 0x0160, 0x0160,
    0x0160, 0x0160, 0x0160, 0x0160, 0x0160, 0x0160, 0x0160, 0x0160,
    0x0160, 0x0160, 0x0160, 0x0160, 0x0160, 0x0160, 0x0160, 0x0160,
    0x0160, 0x0160, 0x0160, 0x0160, 0x0160, 0x0160, 0x0160, 0x0160,
    0x0160, 0x0160, 0x0160, 0x0160, 0x0160, 0x0160, 0x0160, 0x0160,
    0x0160, 0x0160, 0x0160, 0x0160, 0x0160, 0x0160, 0x0160, 0x0160,
    0x0160, 0x0160, 0x0160, 0x0160, 0x0160, 0x0160, 0x0160, 0x0160,
    0x0160, 0x0160, 0x0160, 0x0160, 0x0160, 0x0160, 0x0160, 0x0160,
    0x0160, 0x0160, 0x0160, 0x0160, 0x0160, 0x0160, 0x0160, 0x0160,
    0x0160, 0x0160, 0x0160, 0x0160, 0x0160, 0x0160, 0x0160, 0x0160,
    0x0160, 0x0160, 0x0160, 0x0160, 0x0160, 0x0160, 0x0160, 0x0160,
    0x0160, 0x0160, 0x0160, 0x0160, 0x0160, 0x0160, 0x0160, 0x0160,
    0x0160, 0x0160, 0x0160, 0x0160, 0x0160, 0x0160, 0x0160, 0x0160,
    0x0160, 0x0160, 0x0160, 0x0160, 0x0160, 0x0160, 0x0160, 0x01B0,
    0x01C0, 0x01C0, 0x01C0, 0x01C0, 0x01C0, 0x01C0, 0x01D0, 0x01E0,
    0x01C0, 0x01C0, 0x01C0, 0x01F0, 0x01C0, 0x01C0, 0x0200, 0x0210,
    0x0220, 0x0220, 0x0220, 0x0230, 0x0240, 0x0220, 0x0220, 0x0250,
    0x0260, 0x0270, 0x0280, 0x0290, 0x02A0, 0x02B0, 0x0220, 0x02C0,
    0x0220, 0x0220, 0x02D0, 0x02E0, 0x01C0, 0x02F0, 0x0300, 0x0310,
    0x0320, 0x0330, 0x01C0, 0x01C0, 0x01C0, 0x01C0, 0x01C0, 0x01C0,
    0x01C0, 0x01C0, 0x01C0, 0x01C0, 0x0340, 0x01C0, 0x01C0, 0x01C0,
    0x01C0, 0x01C0, 0x0350, 0x01D0, 0x0360, 0x0370, 0x0220, 0x0380,
    0x01C0, 0x01C0, 0x0390, 0x03A0, 0x03B0, 0x03C0, 0x0220, 0x0220,
    0x03D0, 0x0220, 0x0220, 0x0220, 0x03E0, 0x0220, 0x0220, 0x03F0,
    0x01C0, 0x01C0, 0x01C0, 0x01C0, 0x01C0, 0x01C0, 0x0400, 0x0410,
    0x0420, 0x01C0, 0x01C0, 0x01C0, 0x01C0, 0x01C0, 0x01C0, 0x01C0,
    0x01C0, 0x01C0, 0x01C0, 0x01C0, 0x01C0, 0x01C0, 0x01C0, 0x01C0,
    0x01C0, 0x01C0, 0x01C0, 0x01C0, 0x01C0, 0x01C0, 0x01C0, 0x01C0,
    0x0220, 0x0220, 0x0220, 0x0220, 0x0220, 0x0220, 0x0220, 0x0220,
    0x0220, 0x0430, 0x0220, 0x0220, 0x0220, 0x0220, 0x0220, 0x0440,
    0x0450, 0x0460, 0x0450, 0x0450, 0x0460, 0x0470, 0x0450, 0x0480,
    0x0450, 0x0450, 0x0450, 0x0490, 0x04A0, 0x04B0, 0x04C0, 0x04A0,
    0x01C0, 0x01C0, 0x01C0, 0x01C0, 0x01C0, 0x01C0, 0x01C0, 0x04D0,
    0x01C0, 0x01C0, 0x01C0, 0x01C0, 0x01C0, 0x01C0, 0x01C0, 0x01C0,
    0x01C0, 0x01C0, 0x01C0, 0x01C0, 0x01C0, 0x01C0, 0x01C0, 0x01C0,
    0x01C0, 0x01C0, 0x01C0, 0x01C0, 0x01C0, 0x04E0, 0x04F0, 0x01C0,
    0x01C0, 0x01C0, 0x01C0, 0x01C0, 0x01D0, 0x01E0, 0x01C0, 0x01C0,
    0x01C0, 0x01C0, 0x01C0, 0x01C0, 0x01C0, 0x01C0, 0x01C0, 0x01C0,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0,
    0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0,
    0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0,
    0xFFE0, 0xFFE0, 0xFFE0, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x02E7, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0,
    0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0,
    0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0x0000,
    0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0x0079,
    0x0000, 0xFFFF, 0x0000, 0xFFFF, 0x0000, 0xFFFF, 0x0000, 0xFFFF,
    0x0000, 0xFFFF, 0x0000, 0xFFFF, 0x0000, 0xFFFF, 0x0000, 0xFFFF,
    0x0000, 0xFF18, 0x0000, 0xFFFF, 0x0000, 0xFFFF, 0x0000, 0xFFFF,
    0x0000, 0x0000, 0xFFFF, 0x0000, 0xFFFF, 0x0000, 0xFFFF, 0x0000,
    0xFFFF, 0x0000, 0xFFFF, 0x0000, 0xFFFF, 0x0000, 0xFFFF, 0x0000,
    0xFFFF, 0x0000, 0x0000, 0xFFFF, 0x0000, 0xFFFF, 0x0000, 0xFFFF,
    0x0000, 0xFFFF, 0x0000, 0xFFFF, 0x0000, 0xFFFF, 0x0000, 0xFFFF,
    0x0000, 0x0000, 0xFFFF, 0x0000, 0xFFFF, 0x0000, 0xFFFF, 0xFED4,
    0x0000, 0x0000, 0x0000, 0xFFFF, 0x0000, 0xFFFF, 0x0000, 0x0000,
    0xFFFF, 0x0000, 0x0000, 0x0000, 0xFFFF, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0xFFFF, 0x0000, 0x0000, 0x0061, 0x0000, 0x0000,
    0x0000, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0xFFFF, 0x0000, 0xFFFF, 0x0000, 0xFFFF, 0x0000, 0x0000,
    0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF, 0x0000, 0x0000,
    0xFFFF, 0x0000, 0x0000, 0x0000, 0xFFFF, 0x0000, 0xFFFF, 0x0000,
    0x0000, 0xFFFF, 0x0000, 0x0000, 0x0000, 0xFFFF, 0x0000, 0x0038,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF, 0xFFFE, 0x0000,
    0xFFFF, 0xFFFE, 0x0000, 0xFFFF, 0xFFFE, 0x0000, 0xFFFF, 0x0000,
    0xFFFF, 0x0000, 0xFFFF, 0x0000, 0xFFFF, 0x0000, 0xFFFF, 0x0000,
    0xFFFF, 0x0000, 0xFFFF, 0x0000, 0xFFFF, 0xFFB1, 0x0000, 0xFFFF,
    0x0000, 0x0000, 0xFFFF, 0xFFFE, 0x0000, 0xFFFF, 0x0000, 0x0000,
    0x0000, 0xFFFF, 0x0000, 0xFFFF, 0x0000, 0xFFFF, 0x0000, 0xFFFF,
    0x0000, 0x0000, 0x0000, 0xFFFF, 0x0000, 0xFFFF, 0x0000, 0xFFFF,
    0x0000, 0xFFFF, 0x0000, 0xFFFF, 0x0000, 0xFFFF, 0x0000, 0xFFFF,
    0x0000, 0xFFFF, 0x0000, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0xFF2E, 0xFF32, 0x0000, 0xFF33, 0xFF33,
    0x0000, 0xFF36, 0x0000, 0xFF35, 0x0000, 0x0000, 0x0000, 0x0000,
    0xFF33, 0x0000, 0x0000, 0xFF31, 0x0000, 0x0000, 0x0000, 0x0000,
    0xFF2F, 0xFF2D, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFF2D,
    0x0000, 0x0000, 0xFF2B, 0x0000, 0x0000, 0xFF2A, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0xFF26, 0x0000, 0x0000, 0xFF26, 0x0000, 0x0000, 0x0000, 0x0000,
    0xFF26, 0x0000, 0xFF27, 0xFF27, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0xFF25, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0054, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0xFFDA, 0xFFDB, 0xFFDB, 0xFFDB,
    0xFFE0, 0xFFE0, 0xFFE1, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0,
    0xFFE0, 0xFFE0, 0xFFE0, 0xFFE0, 0xFFC0, 0xFFC1, 0xFFC1, 0x0000,
    0xFFC2, 0xFFC7, 0x0000, 0x0000, 0x0000, 0xFFD1, 0xFFCA, 0x0000,
    0x0000, 0x0000, 0x0000, 0xFFFF, 0x0000, 0xFFFF, 0x0000, 0xFFFF,
    0xFFAA, 0xFFB0, 0xFFB1, 0x0000, 0x0000, 0xFFA0, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0xFFD8, 0xFFD8, 0xFFD8, 0xFFD8, 0xFFD8, 0xFFD8, 0xFFD8, 0xFFD8,
    0xFFD8, 0xFFD8, 0xFFD8, 0xFFD8, 0xFFD8, 0xFFD8, 0xFFD8, 0xFFD8,
    0xFFD8, 0xFFD8, 0xFFD8, 0xFFD8, 0xFFD8, 0xFFD8, 0xFFD8, 0xFFD8,
    0xFFD8, 0xFFD8, 0xFFD8, 0xFFD8, 0xFFD8, 0xFFD8, 0xFFD8, 0xFFD8,
    0xFFD8, 0xFFD8, 0xFFD8, 0xFFD8, 0xFFD8, 0xFFD8, 0xFFE0, 0xFFE0,
    0xFFB0, 0xFFB0, 0xFFB0, 0xFFB0, 0xFFB0, 0xFFB0, 0xFFB0, 0xFFB0,
    0xFFB0, 0xFFB0, 0xFFB0, 0xFFB0, 0xFFB0, 0xFFB0, 0xFFB0, 0xFFB0,
    0x0000, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF, 0x0000, 0xFFFF,
    0x0000, 0x0000, 0xFFFF, 0x0000, 0xFFFF, 0x0000, 0x0000, 0x0000,
    0xFFFF, 0x0000, 0x0000, 0x0000, 0xFFFF, 0x0000, 0x0000, 0x0000,
    0x0000, 0xFFFF, 0x0000, 0xFFFF, 0x0000, 0xFFFF, 0x0000, 0x0000,
    0x0000, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0xFFD0, 0xFFD0, 0xFFD0, 0xFFD0, 0xFFD0, 0xFFD0, 0xFFD0,
    0xFFD0, 0xFFD0, 0xFFD0, 0xFFD0, 0xFFD0, 0xFFD0, 0xFFD0, 0xFFD0,
    0xFFD0, 0xFFD0, 0xFFD0, 0xFFD0, 0xFFD0, 0xFFD0, 0xFFD0, 0xFFD0,
    0xFFD0, 0xFFD0, 0xFFD0, 0xFFD0, 0xFFD0, 0xFFD0, 0xFFD0, 0xFFD0,
    0xFFD0, 0xFFD0, 0xFFD0, 0xFFD0, 0xFFD0, 0xFFD0, 0xFFD0, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0xFFFF, 0x0000, 0xFFFF, 0x0000, 0xFFFF, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0xFFC5, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0xFFFF, 0x0000, 0xFFFF, 0x0000, 0xFFFF, 0x0000, 0xFFFF,
    0x0000, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0008, 0x0008, 0x0008, 0x0008, 0x0008, 0x0008, 0x0008, 0x0008,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0008, 0x0008, 0x0008, 0x0008, 0x0008, 0x0008, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0008, 0x0000, 0x0008, 0x0000, 0x0008, 0x0000, 0x0008,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x004A, 0x004A, 0x0056, 0x0056, 0x0056, 0x0056, 0x0064, 0x0064,
    0x0080, 0x0080, 0x0070, 0x0070, 0x007E, 0x007E, 0x0000, 0x0000,
    0x0008, 0x0008, 0x0000, 0x0009, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xE3DB, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0009, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0008, 0x0008, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0008, 0x0008, 0x0000, 0x0000, 0x0000, 0x0007, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0xFFF0, 0xFFF0, 0xFFF0, 0xFFF0, 0xFFF0, 0xFFF0, 0xFFF0, 0xFFF0,
    0xFFF0, 0xFFF0, 0xFFF0, 0xFFF0, 0xFFF0, 0xFFF0, 0xFFF0, 0xFFF0,
    0xFFE6, 0xFFE6, 0xFFE6, 0xFFE6, 0xFFE6, 0xFFE6, 0xFFE6, 0xFFE6,
    0xFFE6, 0xFFE6, 0xFFE6, 0xFFE6, 0xFFE6, 0xFFE6, 0xFFE6, 0xFFE6,
    0xFFE6, 0xFFE6, 0xFFE6, 0xFFE6, 0xFFE6, 0xFFE6, 0xFFE6, 0xFFE6,
    0xFFE6, 0xFFE6, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0481,
};

// https://github.com/reactos/reactos/blob/4cba65d760a730ea4b77ab6fee857e563c0984d8/sdk/lib/rtl/unicode.c#L83

VOID
NTAPI
RtlInitUnicodeString(
    IN OUT PUNICODE_STRING DestinationString,
    IN PCWSTR SourceString)
{
    SIZE_T Size;
    CONST SIZE_T MaxSize = (MAXUSHORT & ~1) - sizeof(UNICODE_NULL); // an even number

    if (SourceString)
    {
        Size = wcslen(SourceString) * sizeof(WCHAR);
        __analysis_assume(Size <= MaxSize);

        if (Size > MaxSize)
            Size = MaxSize;
        DestinationString->Length = (USHORT)Size;
        DestinationString->MaximumLength = (USHORT)Size + sizeof(UNICODE_NULL);
    }
    else
    {
        DestinationString->Length = 0;
        DestinationString->MaximumLength = 0;
    }

    DestinationString->Buffer = (PWCHAR)SourceString;
}

// https://github.com/reactos/reactos/blob/4cba65d760a730ea4b77ab6fee857e563c0984d8/sdk/lib/rtl/nls.c#L691

WCHAR NTAPI
RtlpUpcaseUnicodeChar(IN WCHAR Source)
{
    USHORT Offset;

    if (Source < 'a')
        return Source;

    if (Source <= 'z')
        return (Source - ('a' - 'A'));

    Offset = ((USHORT)Source >> 8) & 0xFF;
    Offset = NlsUnicodeUpcaseTable[Offset];

    Offset += ((USHORT)Source >> 4) & 0xF;
    Offset = NlsUnicodeUpcaseTable[Offset];

    Offset += ((USHORT)Source & 0xF);
    Offset = NlsUnicodeUpcaseTable[Offset];

    return Source + (SHORT)Offset;
}

// https://github.com/reactos/reactos/blob/4cba65d760a730ea4b77ab6fee857e563c0984d8/sdk/lib/rtl/unicode.c#L2193

LONG
NTAPI
RtlCompareUnicodeString(
    IN PCUNICODE_STRING s1,
    IN PCUNICODE_STRING s2,
    IN BOOLEAN  CaseInsensitive)
{
    unsigned int len;
    LONG ret = 0;
    LPCWSTR p1, p2;

    len = min(s1->Length, s2->Length) / sizeof(WCHAR);
    p1 = s1->Buffer;
    p2 = s2->Buffer;

    if (CaseInsensitive)
    {
        while (!ret && len--) ret = RtlpUpcaseUnicodeChar(*p1++) - RtlpUpcaseUnicodeChar(*p2++);
    }
    else
    {
        while (!ret && len--) ret = *p1++ - *p2++;
    }

    if (!ret) ret = s1->Length - s2->Length;

    return ret;
}

// FIXME: DECLSPEC_NORETURN
VOID
NTAPI
KeBugCheckEx(
    IN ULONG BugCheckCode,
    IN ULONG_PTR BugCheckParameter1,
    IN ULONG_PTR BugCheckParameter2,
    IN ULONG_PTR BugCheckParameter3,
    IN ULONG_PTR BugCheckParameter4)
{
    printf("Configuration manager library crashed.\n");
    printf("BugCheckCode: 0x%"PRIX32"\n", BugCheckCode);
    printf("BugCheckParameter1: 0x%"PRIuPTR"\n", BugCheckParameter1);
    printf("BugCheckParameter2: 0x%"PRIuPTR"\n", BugCheckParameter2);
    printf("BugCheckParameter3: 0x%"PRIuPTR"\n", BugCheckParameter3);
    printf("BugCheckParameter4: 0x%"PRIuPTR"\n", BugCheckParameter4);
    abort();
}

VOID NTAPI
KeQuerySystemTime(
    OUT PLARGE_INTEGER CurrentTime)
{
    time_t Time = time(NULL);
    struct tm* pGmTime = gmtime(&Time);
    UINT YearsSince1601 = (UINT)pGmTime->tm_year + (1900 - 1601);
    UINT LeapYearsSince1601 = YearsSince1601 / 4 - YearsSince1601 / 100 + YearsSince1601 / 400;
    ULONGLONG DaysSince1601 =
        LeapYearsSince1601 * (ULONGLONG)366 +
        (YearsSince1601 - LeapYearsSince1601) * (ULONGLONG)365 +
        (UINT)pGmTime->tm_yday;
    static const ULONGLONG TicksPerSecond = (ULONGLONG)10000000;
    ULONGLONG TicksSince1601 = DaysSince1601 * (TicksPerSecond * 60 * 60 * 24);
    TicksSince1601 += (UINT)pGmTime->tm_hour * (TicksPerSecond * 60 * 60);
    TicksSince1601 += (UINT)pGmTime->tm_min * (TicksPerSecond * 60);
    TicksSince1601 += (UINT)pGmTime->tm_sec * TicksPerSecond;
    CurrentTime->QuadPart = (LONGLONG)TicksSince1601;
}

// https://github.com/reactos/reactos/blob/4cba65d760a730ea4b77ab6fee857e563c0984d8/sdk/lib/rtl/nls.c#L717

WCHAR NTAPI
RtlUpcaseUnicodeChar(IN WCHAR Source)
{
    return RtlpUpcaseUnicodeChar(Source);
}

#define _BITCOUNT 32
#define MAXINDEX 0xFFFFFFFF
typedef ULONG BITMAP_INDEX, *PBITMAP_INDEX;
typedef ULONG BITMAP_BUFFER, *PBITMAP_BUFFER;

VOID RtlFillMemoryUlong(
    OUT PVOID  Destination,
    IN  SIZE_T Length,
    IN  ULONG  Pattern
) {
    ULONG* a = Destination;
    SIZE_T n = Length / sizeof(*a);
    ASSERT(Length % sizeof(*a) == 0);
    for (SIZE_T i = 0; i < n; ++i)
        a[i] = Pattern;
}

// https://github.com/reactos/reactos/blob/4cba65d760a730ea4b77ab6fee857e563c0984d8/sdk/lib/rtl/bitmap.c#L258

VOID NTAPI
RtlInitializeBitMap(
    IN PRTL_BITMAP BitMapHeader,
    IN PULONG BitMapBuffer,
    IN ULONG SizeOfBitMap)
{
    /* Setup the bitmap header */
    BitMapHeader->SizeOfBitMap = SizeOfBitMap;
    BitMapHeader->Buffer = BitMapBuffer;
}

VOID
NTAPI
RtlClearBits(
    IN PRTL_BITMAP BitMapHeader,
    IN BITMAP_INDEX StartingIndex,
    IN BITMAP_INDEX NumberToClear)
{
    BITMAP_INDEX Bits, Mask;
    PBITMAP_BUFFER Buffer;

    ASSERT(StartingIndex + NumberToClear <= BitMapHeader->SizeOfBitMap);

    /* Calculate buffer start and first bit index */
    Buffer = &BitMapHeader->Buffer[StartingIndex / _BITCOUNT];
    Bits = StartingIndex & (_BITCOUNT - 1);

    /* Are we unaligned? */
    if (Bits)
    {
        /* Create an inverse mask by shifting MAXINDEX */
        Mask = MAXINDEX << Bits;

        /* This is what's left in the first ULONG */
        Bits = _BITCOUNT - Bits;

        /* Even less bits to clear? */
        if (NumberToClear < Bits)
        {
            /* Calculate how many bits are left */
            Bits -= NumberToClear;

            /* Fixup the mask on the high side */
            Mask = Mask << Bits >> Bits;

            /* Clear bits and return */
            *Buffer &= ~Mask;
            return;
        }

        /* Clear bits */
        *Buffer &= ~Mask;

        /* Update buffer and left bits */
        Buffer++;
        NumberToClear -= Bits;
    }

    /* Clear all full ULONGs */
    RtlFillMemoryUlong(Buffer, NumberToClear >> 3, 0);
    Buffer += NumberToClear / _BITCOUNT;

    /* Clear what's left */
    NumberToClear &= (_BITCOUNT - 1);
    if (NumberToClear != 0)
    {
        Mask = MAXINDEX << NumberToClear;
        *Buffer &= Mask;
    }
}


// Bit scan forward

#if _MSC_VER

#pragma intrinsic(_BitScanForward)

static inline uint8_t bsf_u32(uint32_t X) {
    unsigned long Result;
    _BitScanForward(&Result, X);
    return (uint8_t)Result;
}

#elif __GNUC__

static inline uint8_t bsf_u32(uint32_t X) {
    return (uint8_t)__builtin_ctz(X);
}

#else

// Source: https://www.chessprogramming.org/Kim_Walisch#Bitscan

static const uint8_t aLogTable32[32] = {
     0,  9,  1, 10, 13, 21,  2, 29,
    11, 14, 16, 18, 22, 25,  3, 30,
     8, 12, 20, 28, 15, 17, 24,  7,
    19, 27, 23,  6, 26,  5,  4, 31
};

static inline uint8_t bsf_u32(uint32_t X) {
    return aLogTable32[((X ^ (X - 1)) * 0x07C4ACDD) >> 27];
}

#endif

static inline unsigned char BitScanForward(unsigned long* pResult, unsigned long X) {
    *pResult = bsf_u32(X);
    return (X == 0);
}

// https://github.com/reactos/reactos/blob/4cba65d760a730ea4b77ab6fee857e563c0984d8/sdk/lib/rtl/bitmap.c#L97-L149

static __inline
BITMAP_INDEX
RtlpGetLengthOfRunClear(
    _In_ PRTL_BITMAP BitMapHeader,
    _In_ BITMAP_INDEX StartingIndex,
    _In_ BITMAP_INDEX MaxLength)
{
    BITMAP_INDEX Value, BitPos, Length;
    PBITMAP_BUFFER Buffer, MaxBuffer;

    /* If we are already at the end, the length of the run is zero */
    ASSERT(StartingIndex <= BitMapHeader->SizeOfBitMap);
    if (StartingIndex >= BitMapHeader->SizeOfBitMap)
        return 0;

    /* Calculate positions */
    Buffer = BitMapHeader->Buffer + StartingIndex / _BITCOUNT;
    BitPos = StartingIndex & (_BITCOUNT - 1);

    /* Calculate the maximum length */
    MaxLength = min(MaxLength, BitMapHeader->SizeOfBitMap - StartingIndex);
    MaxBuffer = Buffer + (BitPos + MaxLength + _BITCOUNT - 1) / _BITCOUNT;

    /* Clear the bits that don't belong to this run */
    Value = *Buffer++ >> BitPos << BitPos;

    /* Skip all clear ULONGs */
    while (Value == 0 && Buffer < MaxBuffer)
    {
        Value = *Buffer++;
    }

    /* Did we reach the end? */
    if (Value == 0)
    {
        /* Return maximum length */
        return MaxLength;
    }

    /* We hit a set bit, check how many clear bits are left */
    BitScanForward(&BitPos, Value);

    /* Calculate length up to where we read */
    Length = (BITMAP_INDEX)(Buffer - BitMapHeader->Buffer) * _BITCOUNT - StartingIndex;
    Length += BitPos - _BITCOUNT;

    /* Make sure we don't go past the last bit */
    if (Length > BitMapHeader->SizeOfBitMap - StartingIndex)
        Length = BitMapHeader->SizeOfBitMap - StartingIndex;

    /* Return the result */
    return Length;
}

// https://github.com/reactos/reactos/blob/4cba65d760a730ea4b77ab6fee857e563c0984d8/sdk/lib/rtl/bitmap.c#L151-L203

static __inline
BITMAP_INDEX
RtlpGetLengthOfRunSet(
    _In_ PRTL_BITMAP BitMapHeader,
    _In_ BITMAP_INDEX StartingIndex,
    _In_ BITMAP_INDEX MaxLength)
{
    BITMAP_INDEX InvValue, BitPos, Length;
    PBITMAP_BUFFER Buffer, MaxBuffer;

    /* If we are already at the end, the length of the run is zero */
    ASSERT(StartingIndex <= BitMapHeader->SizeOfBitMap);
    if (StartingIndex >= BitMapHeader->SizeOfBitMap)
        return 0;

    /* Calculate positions */
    Buffer = BitMapHeader->Buffer + StartingIndex / _BITCOUNT;
    BitPos = StartingIndex & (_BITCOUNT - 1);

    /* Calculate the maximum length */
    MaxLength = min(MaxLength, BitMapHeader->SizeOfBitMap - StartingIndex);
    MaxBuffer = Buffer + (BitPos + MaxLength + _BITCOUNT - 1) / _BITCOUNT;

    /* Get the inversed value, clear bits that don't belong to the run */
    InvValue = ~(*Buffer++) >> BitPos << BitPos;

    /* Skip all set ULONGs */
    while (InvValue == 0 && Buffer < MaxBuffer)
    {
        InvValue = ~(*Buffer++);
    }

    /* Did we reach the end? */
    if (InvValue == 0)
    {
        /* Yes, return maximum */
        return MaxLength;
    }

    /* We hit a clear bit, check how many set bits are left */
    BitScanForward(&BitPos, InvValue);

    /* Calculate length up to where we read */
    Length = (ULONG)(Buffer - BitMapHeader->Buffer) * _BITCOUNT - StartingIndex;
    Length += BitPos - _BITCOUNT;

    /* Make sure we don't go past the last bit */
    if (Length > BitMapHeader->SizeOfBitMap - StartingIndex)
        Length = BitMapHeader->SizeOfBitMap - StartingIndex;

    /* Return the result */
    return Length;
}

// https://github.com/reactos/reactos/blob/4cba65d760a730ea4b77ab6fee857e563c0984d8/sdk/lib/rtl/bitmap.c#L575

ULONG NTAPI
RtlFindSetBits(
    IN PRTL_BITMAP BitMapHeader,
    IN ULONG NumberToFind,
    IN ULONG HintIndex)
{
    BITMAP_INDEX CurrentBit, Margin, CurrentLength;

    /* Check for valid parameters */
    if (!BitMapHeader || NumberToFind > BitMapHeader->SizeOfBitMap)
    {
        return MAXINDEX;
    }

    /* Check if the hint is outside the bitmap */
    if (HintIndex >= BitMapHeader->SizeOfBitMap) HintIndex = 0;

    /* Check for trivial case */
    if (NumberToFind == 0)
    {
        /* Return hint rounded down to byte margin */
        return HintIndex & ~7;
    }

    /* First margin is end of bitmap */
    Margin = BitMapHeader->SizeOfBitMap;

retry:
    /* Start with hint index, length is 0 */
    CurrentBit = HintIndex;

    /* Loop until something is found or the end is reached */
    while (CurrentBit + NumberToFind <= Margin)
    {
        /* Search for the next set run, by skipping a clear run */
        CurrentBit += RtlpGetLengthOfRunClear(BitMapHeader,
            CurrentBit,
            MAXINDEX);

        /* Get length of the set bit run */
        CurrentLength = RtlpGetLengthOfRunSet(BitMapHeader,
            CurrentBit,
            NumberToFind);

        /* Is this long enough? */
        if (CurrentLength >= NumberToFind)
        {
            /* It is */
            return CurrentBit;
        }

        CurrentBit += CurrentLength;
    }

    /* Did we start at a hint? */
    if (HintIndex)
    {
        /* Retry at the start */
        Margin = min(HintIndex + NumberToFind, BitMapHeader->SizeOfBitMap);
        HintIndex = 0;
        goto retry;
    }

    /* Nothing found */
    return MAXINDEX;
}

// https://github.com/reactos/reactos/blob/4cba65d760a730ea4b77ab6fee857e563c0984d8/sdk/lib/rtl/bitmap.c#L372

VOID NTAPI
RtlSetBits(
    IN PRTL_BITMAP BitMapHeader,
    IN ULONG StartingIndex,
    IN ULONG NumberToSet)
{
    BITMAP_INDEX Bits, Mask;
    PBITMAP_BUFFER Buffer;

    ASSERT(StartingIndex + NumberToSet <= BitMapHeader->SizeOfBitMap);

    /* Calculate buffer start and first bit index */
    Buffer = &BitMapHeader->Buffer[StartingIndex / _BITCOUNT];
    Bits = StartingIndex & (_BITCOUNT - 1);

    /* Are we unaligned? */
    if (Bits)
    {
        /* Create a mask by shifting MAXINDEX */
        Mask = MAXINDEX << Bits;

        /* This is what's left in the first ULONG */
        Bits = _BITCOUNT - Bits;

        /* Even less bits to clear? */
        if (NumberToSet < Bits)
        {
            /* Calculate how many bits are left */
            Bits -= NumberToSet;

            /* Fixup the mask on the high side */
            Mask = Mask << Bits >> Bits;

            /* Set bits and return */
            *Buffer |= Mask;
            return;
        }

        /* Set bits */
        *Buffer |= Mask;

        /* Update buffer and left bits */
        Buffer++;
        NumberToSet -= Bits;
    }

    /* Set all full ULONGs */
    RtlFillMemoryUlong(Buffer, NumberToSet >> 3, MAXINDEX);
    Buffer += NumberToSet / _BITCOUNT;

    /* Set what's left */
    NumberToSet &= (_BITCOUNT - 1);
    if (NumberToSet != 0)
    {
        Mask = MAXINDEX << NumberToSet;
        *Buffer |= ~Mask;
    }
}

// https://github.com/reactos/reactos/blob/4cba65d760a730ea4b77ab6fee857e563c0984d8/sdk/lib/rtl/bitmap.c#L281

VOID NTAPI
RtlSetAllBits(
    IN PRTL_BITMAP BitMapHeader)
{
    BITMAP_INDEX LengthInUlongs;

    LengthInUlongs = (BitMapHeader->SizeOfBitMap + _BITCOUNT - 1) / _BITCOUNT;
    RtlFillMemoryUlong(BitMapHeader->Buffer, LengthInUlongs * sizeof(BITMAP_INDEX), ~0);
}

// https://github.com/reactos/reactos/blob/4cba65d760a730ea4b77ab6fee857e563c0984d8/sdk/lib/rtl/bitmap.c#L270

VOID NTAPI
RtlClearAllBits(
    IN PRTL_BITMAP BitMapHeader)
{
    BITMAP_INDEX LengthInUlongs;

    LengthInUlongs = (BitMapHeader->SizeOfBitMap + _BITCOUNT - 1) / _BITCOUNT;
    RtlFillMemoryUlong(BitMapHeader->Buffer, LengthInUlongs * sizeof(BITMAP_INDEX), 0);
}
