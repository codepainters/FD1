#include "lpc134x.h"
#include "ssp/ssp.h"

#include "display.h"
#include "display_defs.h"
#include "buttons.h"
#include "gpio_pin.h"

// index of the currently displayed digit
static volatile int digitIdx = 0;

// current state of each digit's segments
static volatile int digit[DISPLAY_DIGITS] = { SEGMENTS_BLANK, SEGMENTS_BLANK, SEGMENTS_BLANK };

// time (ticks) until turning DP off
static volatile unsigned int dpCountdown = 0;

static void Display_SetSegments(uint32_t segments);
static void Display_SetDigitPin(uint32_t digit, bool active);

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
    unsigned int i;

    // Note: as silly as it sounds, LPC1343 is unable to generate an interrupt at the end
    // of the SPI transfer! There's only "TX FIFO half-empty interrupt"!
    // See e.g. http://www.lpcware.com/content/forum/interrupt-driven-spi
    //
    // For this reason we initiated SPI transfer at the end of the tick, simply assuming it
    // is going to be ready before the next tick


    // blank all
    for (i = 0; i < sizeof(DISPLAY_DIGIT_PIN) / sizeof(DISPLAY_DIGIT_PIN[0]); i++) {
        Display_SetDigitPin(i, 0);
    }

    // pulse STRB, so shit register is loaded into output register
    gpioSetValue(HC595_STROBE_PORT, HC595_STROBE_PIN, 0);
    gpioSetValue(HC595_STROBE_PORT, HC595_STROBE_PIN, 1);

    // enable given digit
    Display_SetDigitPin(digitIdx, 1);

    // ..and let buttons check the state
    Buttons_CheckState(digitIdx);

    // execute the DP logic
    if (dpCountdown > 0) {
        dpCountdown--;
    }

    // now we can prepare for the next digit
    digitIdx = (digitIdx + 1) % DISPLAY_DIGITS;

    if (digitIdx == 1) {
        Display_SetSegments((digit[digitIdx] & SEGMENTS_DP_MASK) | (dpCountdown ? 0 : SEGMENTS_DP));
    }
    else {
        Display_SetSegments(digit[digitIdx]);
    }
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

void Display_BlinkDP()
{
    dpCountdown = DP_BLINK_DURATION;
}

static void Display_SetSegments(uint32_t segments)
{
    uint8_t Dummy = Dummy;

    // Note: TX FIFO should be empty here
    while ((SSP_SSP0SR & (SSP_SSP0SR_TNF_NOTFULL | SSP_SSP0SR_BSY_BUSY)) != SSP_SSP0SR_TNF_NOTFULL);
    SSP_SSP0DR = segments;

    // TODO: is there any way to avoid draining RX FIFO? or do it in the background?
    while ( (SSP_SSP0SR & (SSP_SSP0SR_BSY_BUSY|SSP_SSP0SR_RNE_NOTEMPTY)) != SSP_SSP0SR_RNE_NOTEMPTY );
    Dummy = SSP_SSP0DR;
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

