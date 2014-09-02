#ifndef __PINS_H__
#define __PINS_H__

#include "gpio/gpio.h"

/**
 * Helper structure wrapping all the info needed to manipulate particular pin.
 *
 * Thanks to NXP silly idea, it's not enough to know the base address of the port
 * and a pin number - we also need to know the address of IOCON register.
 */
typedef const struct GpioPin_tag {
    /** address od the IOCON register */
    volatile uint32_t* ioconReg;
    uint32_t portNum;
    uint32_t pinNum;
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
 * Sets the output pin state
 */
inline void GpioPin_SetValue(const GpioPin_t* pin, const uint32_t state);

/**
 * Returns value of a given input pin
 */
inline uint32_t GpioPin_GetValue(const GpioPin_t* pin);

#endif // __PINS_H__
