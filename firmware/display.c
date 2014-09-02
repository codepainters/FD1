/**
 * Copyright (c) 2014, Przemyslaw Wegrzyn <pwegrzyn@codepainters.com>
 * All rights reserved.
 *
 * This file is distributed under the Modified BSD License.
 * See LICENSE.txt for details.
 */

#include "lpc134x.h"

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
    // reset SSP
    SCB_PRESETCTRL &= ~SCB_PRESETCTRL_SSP0_MASK;
    SCB_PRESETCTRL |= SCB_PRESETCTRL_SSP0_RESETDISABLED;

    // enable SSP clock, divide by 1
    SCB_SYSAHBCLKCTRL |= (SCB_SYSAHBCLKCTRL_SSP0);
    SCB_SSP0CLKDIV = SCB_SSP0CLKDIV_DIV1;

    // configure PIO0.9 as SSP MOSI
    IOCON_PIO0_9 &= ~IOCON_PIO0_9_FUNC_MASK;
    IOCON_PIO0_9 |= IOCON_PIO0_9_FUNC_MOSI0;

    // configure PIO2.11 as SSP SCK
    IOCON_SCKLOC = IOCON_SCKLOC_SCKPIN_PIO2_11;
    IOCON_PIO2_11 = IOCON_PIO2_11_FUNC_SCK0;

    // configure SSP format
    SSP_SSP0CR0 = SSP_SSP0CR0_DSS_8BIT |    // 8-bit data
                  SSP_SSP0CR0_FRF_SPI |     // SPI frame format
                  SSP_SSP0CR0_SCR_8 |       // Serial clock rate = 8
                  SSP_SSP0CR0_CPOL_HIGH |   // Clock polarity - high
                  SSP_SSP0CR0_CPHA_SECOND;  // Clock phase - falling edge

    // clock prescale register
    SSP_SSP0CPSR = SSP_SSP0CPSR_CPSDVSR_DIV2;

    // enable SSP in master mode, no loopback
    SSP_SSP0CR1 = SSP_SSP0CR1_SSE_ENABLED | SSP_SSP0CR1_MS_MASTER | SSP_SSP0CR1_LBM_NORMAL;

    // configure HC595 strobe and digit seleciton pins
    GpioPin_ConfigureOut(&HC595_STROBE, 1);
    for (unsigned int i = 0; i < sizeof(DISPLAY_DIGIT_PIN) / sizeof(DISPLAY_DIGIT_PIN[0]); i++) {
        GpioPin_ConfigureOut(&DISPLAY_DIGIT_PIN[i], 0);
    }
}

void Display_TimerTick()
{
    // Note: as silly as it sounds, LPC1343 is unable to generate an interrupt at the end
    // of the SPI transfer! There's only "TX FIFO half-empty interrupt"!
    // See e.g. http://www.lpcware.com/content/forum/interrupt-driven-spi
    //
    // For this reason we initiated SPI transfer at the end of the tick, simply assuming it
    // is going to be ready before the next tick


    // blank all
    for (unsigned int i = 0; i < sizeof(DISPLAY_DIGIT_PIN) / sizeof(DISPLAY_DIGIT_PIN[0]); i++) {
        Display_SetDigitPin(i, 0);
    }

    // pulse STRB, so shit register is loaded into output register
    GpioPin_SetState(&HC595_STROBE, 0);
    GpioPin_SetState(&HC595_STROBE, 1);

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
        GpioPin_ConfigureOut(pin, 0);
    }
    else {
        GpioPin_ConfigureIn(pin);
    }
}

