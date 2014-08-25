#ifndef __BUTTONS_H__
#define __BUTTONS_H__

void Buttons_Init();

/**
 * This function is called from the display driver, right after displaying next digit.
 *
 * For the sake of simplicity it's called directly (as opposed to pointer-to-function
 * callback), hence buttons/display circular reference.
 *
 * Note: this is called in the context of the timer interrupt.
 */
void Buttons_CheckState(int column);

#endif // __BUTTONS_H__
