#include "display.h"

#include "lpc134x.h"
#include "ssp/ssp.h"

static void Display_SetSegments(uint8_t state);

#define HC595_STRB_PORT     (1)
#define HC595_STRB_PIN      (2)

struct {
    volatile uint32_t* ioconReg;
    uint32_t port;
    uint32_t pin;
} digitPin[] ={
    { &IOCON_PIO2_3, 2, 3 },
    { &IOCON_PIO2_6, 2, 6 },
    { &IOCON_PIO2_0, 2, 0 }
};

#define P_DIG1_PORT (2)
#define P_DIG1_PIN  (3)
#define P_DIG2_PORT (2)
#define P_DIG2_PIN  (6)
#define P_DIG3_PORT (2)
#define P_DIG3_PIN  (0)

void Display_Init()
{
    int i;
    // Note: sspInit() reconfigures PIO0_2!

    // HC595 shifts on rising clock edge, hence we set clock to be high when idle
    // and let the transition occur on the falling edge
    sspInit(0, sspClockPolarity_High, sspClockPhase_FallingEdge);

    // HC595 load strobe pin - PIO1.2
    IOCON_JTAG_nTRST_PIO1_2 &= ~IOCON_JTAG_nTRST_PIO1_2_FUNC_MASK;
    IOCON_JTAG_nTRST_PIO1_2 |= IOCON_JTAG_nTRST_PIO1_2_FUNC_GPIO;
    gpioSetDir(HC595_STRB_PORT, HC595_STRB_PIN, gpioDirection_Output);
    gpioSetValue(HC595_STRB_PORT, HC595_STRB_PIN, 1);

    for (i = 0; i < sizeof(digitPin) / sizeof(digitPin[0]); i++) {
        // hack alert - all pins have the same IOCON mask/gpio bits
        *digitPin[i].ioconReg &= ~IOCON_PIO2_0_FUNC_MASK;
        *digitPin[i].ioconReg |= IOCON_PIO2_0_FUNC_GPIO;

        gpioSetDir(digitPin[i].port, digitPin[i].pin, gpioDirection_Output);
        gpioSetValue(digitPin[i].port, digitPin[i].pin, 1);
    }

    Display_SetSegments(0xF0);

    gpioSetValue(digitPin[0].port, digitPin[0].pin, 1);
    gpioSetValue(digitPin[1].port, digitPin[1].pin, 0);
    gpioSetValue(digitPin[2].port, digitPin[2].pin, 0);
}

static void Display_SetSegments(uint8_t state)
{
    // send single byte
    sspSend(0, &state, 1);

    // pulse STRB, so shit register is loaded into output register
    gpioSetValue(HC595_STRB_PORT, HC595_STRB_PIN, 0);
    gpioSetValue(HC595_STRB_PORT, HC595_STRB_PIN, 1);
}
