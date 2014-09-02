/**
 * Copyright (c) 2014, Przemyslaw Wegrzyn <pwegrzyn@codepainters.com>
 * All rights reserved.
 *
 * This file is distributed under the Modified BSD License.
 * See LICENSE.txt for details.
 */

#ifndef __DISPLAY_DEFS_H__
#define __DISPLAY_DEFS_H__

#include "gpio_pin.h"

// Note: LEDs act as a special 3rd digit
#define DISPLAY_DIGITS      (3)

// how many clock cycles to keep DP led ON
#define DP_BLINK_DURATION   (500)

// Multiplexed LED display digit pins. Should be left floating or pulled low (pseudo open-drain).
GpioPin_t DISPLAY_DIGIT_PIN[] ={
    { &IOCON_PIO2_0, 2, 0, IOCON_COMMON_HYS_DISABLE | IOCON_COMMON_MODE_INACTIVE | IOCON_PIO2_0_FUNC_GPIO },
    { &IOCON_PIO2_6, 2, 6, IOCON_COMMON_HYS_DISABLE | IOCON_COMMON_MODE_INACTIVE | IOCON_PIO2_6_FUNC_GPIO },
    { &IOCON_PIO2_3, 2, 3, IOCON_COMMON_HYS_DISABLE | IOCON_COMMON_MODE_INACTIVE | IOCON_PIO2_3_FUNC_GPIO }
};

// strobe signal for HC595 register - PIO1.2
GpioPin_t HC595_STROBE = { &IOCON_JTAG_nTRST_PIO1_2, 1, 2,
                           IOCON_COMMON_HYS_DISABLE | IOCON_COMMON_MODE_INACTIVE | IOCON_JTAG_nTRST_PIO1_2_FUNC_GPIO };

/*
 * Values to put into HC595 to display particular symbols.
 */

static const uint8_t SEGMENTS_HEX[] = {
    0x11, 0xD7, 0x32, 0x92, // 0, 1, 2, 3
    0xD4, 0x98, 0x18, 0xD3, // 4, 5, 6, 7
    0x10, 0x90, 0x50, 0x1C, // 8, 9, A, B
    0x39, 0x16, 0x38, 0x78  // C, D, E, F
};

#define SEGMENTS_DP         (0x10)
#define SEGMENTS_DP_MASK    (0xEF)

#define SEGMENTS_BLANK (0xFF)
#define SEGMENTS_DASH  (0xFE)
// minus one, used for numbers -19..-10
#define SEGMENTS_MONE  (0xD6)

// LEDs act as a 3rd digit
#define SEGMENTS_LED1   (0xFB)
#define SEGMENTS_LED2   (0xFE)
#define SEGMENTS_LED3   (0xF7)

#endif // __DISPLAY_DEFS_H__
