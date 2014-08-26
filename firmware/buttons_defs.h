#ifndef __BUTTONS_DEFS_H__
#define __BUTTONS_DEFS_H__

#include "gpio_pin.h"

/*
 * Panel buttons
 */

// Input pins of the front panel matrix, with pull-ups
GpioPin_t ROW_PINS[] ={
    { &IOCON_JTAG_TMS_PIO1_0, 1, 0,
        IOCON_JTAG_TMS_PIO1_0_HYS_DISABLE | IOCON_JTAG_TMS_PIO1_0_MODE_PULLUP |
        IOCON_JTAG_TMS_PIO1_0_FUNC_GPIO | IOCON_JTAG_TMS_PIO1_0_ADMODE_DIGITAL },
    { &IOCON_JTAG_TDO_PIO1_1, 1, 1,
        IOCON_JTAG_TDO_PIO1_1_HYS_DISABLE | IOCON_JTAG_TDO_PIO1_1_MODE_PULLUP |
        IOCON_JTAG_TDO_PIO1_1_FUNC_GPIO | IOCON_JTAG_TDO_PIO1_1_ADMODE_DIGITAL }
};

#define ROW_PB1         (0)
#define ROW_PB2         (1)

#define COLUMN_ENCODER  (0)
#define COLUMN_PBUTTON  (1)

#endif // __BUTTONS_DEFS_H__
