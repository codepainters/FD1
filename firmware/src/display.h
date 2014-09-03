/**
 * Copyright (c) 2014, Przemyslaw Wegrzyn <pwegrzyn@codepainters.com>
 * All rights reserved.
 *
 * This file is distributed under the Modified BSD License.
 * See LICENSE.txt for details.
 */

#ifndef __DISPLAY_H__
#define __DISPLAY_H__

enum DisplayLed {
    Display_LED1 = 1 << 0,
    Display_LED2 = 1 << 1,
    Display_LED3 = 1 << 2
};

void Display_Init();

/**
 * Returns currently selected column, used by the buttons module
 */
unsigned int Display_CurrentDigit();

/**
 * Display multiplexing occurs in this call, on each call next digit is displayed.
 */
void Display_TimerTick();

/**
 * Displays integer number
 *
 * @param aValue    range -19..99
 */
void Display_SetInt(int aValue);

/**
 * Displays a hexadecimal number
 *
 * @param aValue    range 0..255
 */
void Display_SetHex(int aValue);

/**
 * Sets the panel LEDs state
 *
 * @param state     OR-ed Display_LEDn values
 */
void Display_SetLeds(int state);

/**
 * Blinks the decmal point (used to show MIDI activity)
 */
void Display_BlinkDP();

#endif // __DISPLAY_H__
