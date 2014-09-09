/**
 * Copyright (c) 2014, Przemyslaw Wegrzyn <pwegrzyn@codepainters.com>
 * All rights reserved.
 *
 * This file is distributed under the Modified BSD License.
 * See LICENSE.txt for details.
 */

#ifndef __BUTTONS_DEFS_H__
#define __BUTTONS_DEFS_H__

#include "gpio_pin.h"

// how long (ticks) the button state has to remain stable
#define DEBOUNCE_DURATION  (100)

// how long (ticks) the button must be pressed to emit long press event
#define LONG_PRESS_DURATION  (1000)

// Input pins of the front panel matrix, with pull-ups
GpioPin_t ROW_PINS[] ={
    { &IOCON_JTAG_TMS_PIO1_0, 1, 0,
        IOCON_JTAG_TMS_PIO1_0_HYS_DISABLE | IOCON_JTAG_TMS_PIO1_0_MODE_PULLUP |
        IOCON_JTAG_TMS_PIO1_0_FUNC_GPIO | IOCON_JTAG_TMS_PIO1_0_ADMODE_DIGITAL },
    { &IOCON_JTAG_TDO_PIO1_1, 1, 1,
        IOCON_JTAG_TDO_PIO1_1_HYS_DISABLE | IOCON_JTAG_TDO_PIO1_1_MODE_PULLUP |
        IOCON_JTAG_TDO_PIO1_1_FUNC_GPIO | IOCON_JTAG_TDO_PIO1_1_ADMODE_DIGITAL }
};

// indexes for ROW_PINS
#define ROW_PB1         (0)
#define ROW_PB2         (1)

// indexes for columns
#define COLUMN_ENCODER  (0)
#define COLUMN_PBUTTON  (1)

#endif // __BUTTONS_DEFS_H__
