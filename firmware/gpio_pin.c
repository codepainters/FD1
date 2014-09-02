/**
 * Copyright (c) 2014, Przemyslaw Wegrzyn <pwegrzyn@codepainters.com>
 * All rights reserved.
 *
 * This file is distributed under the Modified BSD License.
 * See LICENSE.txt for details.
 */

#include "gpio_pin.h"

static inline REG32* GpioPin_GetDirReg(const GpioPin_t* pin)
{
    switch (pin->portNum)
    {
      case 0:
        return &GPIO_GPIO0DIR;
        break;
      case 1:
        return &GPIO_GPIO1DIR;
        break;
      case 2:
        return &GPIO_GPIO2DIR;
        break;
      case 3:
        return &GPIO_GPIO3DIR;
        break;

      default:
        return &GPIO_GPIO0DIR;
        break;
    }
}

void GpioPin_ConfigureOut(const GpioPin_t* pin, int initialState)
{
    *pin->ioconReg = pin->ioconInitVal;

    REG32* gpioDirReg = GpioPin_GetDirReg(pin);
    *gpioDirReg |= (1 << pin->pinNum);

    GpioPin_SetState(pin, initialState);
}

void GpioPin_ConfigureIn(const GpioPin_t* pin)
{
    *pin->ioconReg = pin->ioconInitVal;

    REG32* gpioDirReg = GpioPin_GetDirReg(pin);
    *gpioDirReg &= ~(1 << pin->pinNum);
}

