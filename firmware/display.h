#ifndef __DISPLAY_H__
#define __DISPLAY_H__

enum DisplayLed {
    Display_LED1 = 1 << 0,
    Display_LED2 = 1 << 1,
    Display_LED3 = 1 << 2
};

void Display_Init();

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

#endif // __DISPLAY_H__
