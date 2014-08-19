#ifndef __DISPLAY_H__
#define __DISPLAY_H__

void Display_Init();

void Display_TimerTick();

void Display_SetInt(int aValue);

void Display_SetHex(int aValue);

enum DisplayLed {
    Display_LED1 = 1 << 0,
    Display_LED2 = 1 << 1,
    Display_LED3 = 1 << 2
};

void Display_SetLeds(int state);

#endif // __DISPLAY_H__
