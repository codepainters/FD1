#include "display.h"

#include "lpc134x.h"
#include "ssp/ssp.h"

static void Display_SetDigitPin(uint32_t digit, bool active);
static void Display_SetSegments(uint32_t digit, uint8_t state);

#define HC595_STRB_PORT     (1)
#define HC595_STRB_PIN      (2)

struct {
    volatile uint32_t* ioconReg;
    uint32_t port;
    uint32_t pin;
} digitPin[] ={
    { &IOCON_PIO2_0, 2, 0 },
    { &IOCON_PIO2_6, 2, 6 },
    { &IOCON_PIO2_3, 2, 3 }
};

#define P_DIG1_PORT (2)
#define P_DIG1_PIN  (3)
#define P_DIG2_PORT (2)
#define P_DIG2_PIN  (6)
#define P_DIG3_PORT (2)
#define P_DIG3_PIN  (0)

#define BLANK (0xFF)

static const uint8_t hexDigit[] = {
    0x11, 0xD7, 0x32, 0x92, // 0, 1, 2, 3
    0xD4, 0x98, 0x18, 0xD3, // 4, 5, 6, 7
    0x10, 0x90, 0x50, 0x1C, // 8, 9, A, B
    0x39, 0x16, 0x38, 0x78  // C, D, E, F
};

void Display_Init()
{
    int i;

    // HC595 shifts on rising clock edge, hence we set clock to be high when idle
    // and let the transition occur on the falling edge
    // Note: sspInit() reconfigures PIO0_2!
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
        gpioSetValue(digitPin[i].port, digitPin[i].pin, 0);
    }

    Display_SetSegments(0, 0x70);

    showDigit(0);
}

void showDigit(int i)
{
    Display_SetSegments(0, hexDigit[i]);
}

static void Display_SetDigitPin(uint32_t i, bool active)
{
    gpioSetDir(digitPin[i].port, digitPin[i].pin, active ? gpioDirection_Output : gpioDirection_Input);
}

static void Display_SetSegments(uint32_t digit, uint8_t state)
{
    unsigned int i;

    // send single byte
    sspSend(0, &state, 1);

    // blank all
    for (i = 0; i < sizeof(digitPin) / sizeof(digitPin[0]); i++) {
        Display_SetDigitPin(i, 0);
    }

    // pulse STRB, so shit register is loaded into output register
    gpioSetValue(HC595_STRB_PORT, HC595_STRB_PIN, 0);
    gpioSetValue(HC595_STRB_PORT, HC595_STRB_PIN, 1);

    // enable given digit
    Display_SetDigitPin(digit, 1);
}
