#ifndef __KEYBOARD_DEFS_H__
#define __KEYBOARD_DEFS_H__

#include "gpio_pin.h"

/*
 * Keyboard matrix is scanned row by row, one of the row outputs is pulled low.
 * Column inputs are configured with pull-ups, pressed key yields value of 0.
 *
 * R0/C0 is the right-most key, R3/C4 is the left-most key.
 */
#define KBD_ROWS        (4)
#define KBD_COLUMNS     (5)
#define KBD_TOTAL_KEYS  (KBD_ROWS * KBD_COLUMNS)

// MIDI note number of the left-most keyboard key
#define KBD_LEFTMOST_NOTE   (21)

// convert row/column to key index, the left-most key has index 0
#define KBD_KEY_INDEX(row, column)     (KBD_TOTAL_KEYS - ((row) * KBD_COLUMNS + (column)) - 1)

// Input pins of the front panel matrix, with pull-ups
GpioPin_t KBD_ROW_PINS[KBD_ROWS] ={
    { &IOCON_PIO3_0, 3, 0, IOCON_PIO3_0_HYS_DISABLE | IOCON_PIO3_0_MODE_INACTIVE | IOCON_PIO3_0_FUNC_GPIO },
    { &IOCON_PIO3_1, 3, 1, IOCON_PIO3_1_HYS_DISABLE | IOCON_PIO3_1_MODE_INACTIVE | IOCON_PIO3_1_FUNC_GPIO },
    { &IOCON_PIO3_2, 3, 2, IOCON_PIO3_2_HYS_DISABLE | IOCON_PIO3_2_MODE_INACTIVE | IOCON_PIO3_2_FUNC_GPIO },
    { &IOCON_PIO3_3, 3, 3, IOCON_PIO3_3_HYS_DISABLE | IOCON_PIO3_3_MODE_INACTIVE | IOCON_PIO3_3_FUNC_GPIO }
};

GpioPin_t KBD_COLUMN_PINS[KBD_COLUMNS] ={
    { &IOCON_PIO1_8, 1, 8, IOCON_PIO1_8_HYS_DISABLE | IOCON_PIO1_8_MODE_PULLUP | IOCON_PIO1_8_FUNC_GPIO },
    { &IOCON_PIO0_2, 0, 2, IOCON_PIO0_2_HYS_DISABLE | IOCON_PIO0_2_MODE_PULLUP | IOCON_PIO0_2_FUNC_GPIO },
    { &IOCON_PIO2_7, 2, 7, IOCON_PIO2_7_HYS_DISABLE | IOCON_PIO2_7_MODE_PULLUP | IOCON_PIO2_7_FUNC_GPIO },
    { &IOCON_PIO2_8, 2, 8, IOCON_PIO2_8_HYS_DISABLE | IOCON_PIO2_8_MODE_PULLUP | IOCON_PIO2_8_FUNC_GPIO },
    { &IOCON_PIO2_1, 2, 1, IOCON_PIO2_1_HYS_DISABLE | IOCON_PIO2_1_MODE_PULLUP | IOCON_PIO2_1_FUNC_GPIO }
};

// debounce period (ticks)
#define DEBOUNCE_PERIOD (100)

#endif // __KEYBOARD_DEFS_H__
