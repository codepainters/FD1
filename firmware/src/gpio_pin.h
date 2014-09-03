/**
 * Copyright (c) 2014, Przemyslaw Wegrzyn <pwegrzyn@codepainters.com>
 * All rights reserved.
 *
 * This file is distributed under the Modified BSD License.
 * See LICENSE.txt for details.
 */

#ifndef __GPIO_PIN_H__
#define __GPIO_PIN_H__

#include "sysdefs.h"
#include "lpc134x.h"

/**
 * Helper structure wrapping all the info needed to manipulate particular pin.
 *
 * Thanks to NXP silly idea, it's not enough to know the base address of the port
 * and a pin number - we also need to know the address of IOCON register.
 */
typedef const struct GpioPin_tag {
    /** address od the IOCON register */
    volatile uint32_t* ioconReg;
    /** PIO port number */
    uint32_t portNum;
    /** PIO pin number */
    uint32_t pinNum;
    /** initial value of the IOCON register */
    uint32_t ioconInitVal;
} GpioPin_t;

/**
 * Configure a given pin as GPIO output
 */
void GpioPin_ConfigureOut(const GpioPin_t* pin, int initialState);

/**
 * Configure a given pin as GPIO input
 */
void GpioPin_ConfigureIn(const GpioPin_t* pin);

/**
 * Sets the output pin state (0 or 1)
 */
INLINE void GpioPin_SetState(const GpioPin_t* pin, const uint32_t state) INLINE_POST;

/**
 * Returns current state (0 or 1) of a given input pin
 */
INLINE uint32_t GpioPin_GetState(const GpioPin_t* pin) INLINE_POST;

/*
 * Inline functions implementation
 */

INLINE void GpioPin_SetState(const GpioPin_t* pin, const uint32_t state)
{
    (*(pREG32 ((GPIO_GPIO0_BASE + (pin->portNum << 16)) + ((1 << pin->pinNum) << 2)))) =
            state ? 0xFFF : 0;
}

INLINE uint32_t GpioPin_GetState(const GpioPin_t* pin)
{
    return (*(pREG32 ((GPIO_GPIO0_BASE + (pin->portNum << 16)) + ((1 << pin->pinNum) << 2)))) &
            (1 << pin->pinNum) ? 1 : 0;
}

#endif // __GPIO_PIN_H__
