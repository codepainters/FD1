#include "lpc134x.h"
#include "ssp/ssp.h"

#include "display.h"
#include "buttons.h"
#include "gpio_pin.h"
#include "board.h"

// index of the currently displayed digit
static volatile int digitIdx = 0;

// current state of each digit's segments
static volatile int digit[DISPLAY_DIGITS] = { SEGMENTS_BLANK, SEGMENTS_BLANK, SEGMENTS_BLANK };

static void Display_SetDigitPin(uint32_t digit, bool active);
static void Display_SetSegments(uint32_t digit, uint8_t state);

void Display_Init()
{
    int i;

    // HC595 shifts on rising clock edge, hence we set clock to be high when idle
    // and let the transition occur on the falling edge
    // Note: sspInit() reconfigures PIO0_2!
    sspInit(0, sspClockPolarity_High, sspClockPhase_FallingEdge);

    GpioPin_ConfigureOut(&HC595_STROBE, 1);
    for (i = 0; i < sizeof(DISPLAY_DIGIT_PIN) / sizeof(DISPLAY_DIGIT_PIN[0]); i++) {
        GpioPin_ConfigureOut(&DISPLAY_DIGIT_PIN[i], 0);
    }
}

void Display_TimerTick()
{
    Display_SetSegments(digitIdx, digit[digitIdx]);
    Buttons_CheckState(digitIdx);

    digitIdx = (digitIdx + 1) % DISPLAY_DIGITS;
}

void Display_SetInt(int aValue)
{
    // Note: value has to be -19 .. 99
    int absValue = (aValue < 0) ? -aValue : aValue;
    int l = absValue % 10;
    int h = absValue / 10;

    digit[1] = SEGMENTS_HEX[l];

    if (aValue < -9) {
        // if -10 or less, show "minus one" on the 1st digit
        digit[0] = SEGMENTS_MONE;
    }
    else if (aValue < 0) {
        // if -9..-1, show dash on the 1st digit
        digit[0] = SEGMENTS_DASH;
    }
    else if (h == 0) {
        // no leading zero
        digit[0] = SEGMENTS_BLANK;
    }
    else {
        digit[0] = SEGMENTS_HEX[h];
    }
}

void Display_SetHex(int aValue)
{
    digit[0] = SEGMENTS_HEX[(aValue >> 4) & 0x0F];
    digit[1] = SEGMENTS_HEX[aValue & 0x0F];
}

void Display_SetLeds(int state)
{
    digit[2] =
            (state & Display_LED1 ? SEGMENTS_LED1 : 0x0FF) &
            (state & Display_LED2 ? SEGMENTS_LED2 : 0x0FF) &
            (state & Display_LED3 ? SEGMENTS_LED3 : 0x0FF);
}

static void Display_SetDigitPin(uint32_t i, bool active)
{
    GpioPin_t* pin = &DISPLAY_DIGIT_PIN[i];

    // Note: LPC1343 doesn't support open-drain GPIO mode, unfortunately, so what's done here
    // is a bit of a hack - switching between output mode pulling down and input mode

    if (active) {
        gpioSetDir(pin->portNum, DISPLAY_DIGIT_PIN[i].pinNum, gpioDirection_Output);
        // Note: when GPIO is switched from input to output, it is set to
        // its previous input value, hence we need to set the pin to 0 again
        gpioSetValue(pin->portNum, pin->pinNum, 0);
    }
    else {
        gpioSetDir(pin->portNum, pin->pinNum, gpioDirection_Input);
    }
}

static void Display_SetSegments(uint32_t digit, uint8_t state)
{
    unsigned int i;

    // send segments byte to HC595
    sspSend(0, &state, 1);

    // blank all
    for (i = 0; i < sizeof(DISPLAY_DIGIT_PIN) / sizeof(DISPLAY_DIGIT_PIN[0]); i++) {
        Display_SetDigitPin(i, 0);
    }

    // pulse STRB, so shit register is loaded into output register
    gpioSetValue(HC595_STROBE_PORT, HC595_STROBE_PIN, 0);
    gpioSetValue(HC595_STROBE_PORT, HC595_STROBE_PIN, 1);

    // enable given digit
    Display_SetDigitPin(digit, 1);
}


