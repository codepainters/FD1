
#include "gpio/gpio.h"
#include "lpc134x.h"

struct {
    volatile uint32_t* ioconReg;
    uint32_t port;
    uint32_t pin;
} rowPin[] ={
    { &IOCON_JTAG_TMS_PIO1_0, 1, 0 },
    { &IOCON_JTAG_TDO_PIO1_1, 1, 1 }
};

void Buttons_Init()
{
    int i;

    // Configure row pins - GPIO, input, pull-up
    for (i = 0; i < sizeof(rowPin) / sizeof(rowPin[0]); i++) {
        *rowPin[i].ioconReg &= ~IOCON_JTAG_TMS_PIO1_0_FUNC_MASK;
        *rowPin[i].ioconReg |= IOCON_JTAG_TMS_PIO1_0_FUNC_GPIO;

        gpioSetDir(rowPin[i].port, rowPin[i].pin, gpioDirection_Input);
        gpioSetPullup(&rowPin[i].ioconReg, gpioPullupMode_PullUp);
    }
}

void Buttons_CheckState(int column)
{

}

