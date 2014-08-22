#include "gpio_pin.h"

#define IOCON_COMMON_FUNC_GPIO  0x00

void GpioPin_ConfigureOut(const GpioPin_t* pin, int initialState)
{
    *pin->ioconReg = pin->ioconInitVal;
    gpioSetDir(pin->portNum, pin->pinNum, gpioDirection_Output);
}
