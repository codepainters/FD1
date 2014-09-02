#include "gpio_pin.h"

void GpioPin_ConfigureOut(const GpioPin_t* pin, int initialState)
{
    *pin->ioconReg = pin->ioconInitVal;
    gpioSetDir(pin->portNum, pin->pinNum, gpioDirection_Output);
}

void GpioPin_ConfigureIn(const GpioPin_t* pin)
{
    *pin->ioconReg = pin->ioconInitVal;
    gpioSetDir(pin->portNum, pin->pinNum, gpioDirection_Input);
}

inline void GpioPin_SetValue(const GpioPin_t* pin, const uint32_t state)
{
    (*(pREG32 ((GPIO_GPIO0_BASE + (pin->portNum << 16)) + ((1 << pin->bitPos) << 2)))) =
            state ? 0xFFF : 0;
}

inline uint32_t GpioPin_GetValue(const GpioPin_t* pin)
{
    return (*(pREG32 ((GPIO_GPIO0_BASE + (pin->portNum << 16)) + ((1 << pin->pinNum) << 2)))) &
            (1 << pin->pinNum) ? 1 : 0;
}
