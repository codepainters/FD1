#ifndef __BUTTONS_H__
#define __BUTTONS_H__

typedef enum {
    BUTTON_EVENT_PB_PRESSED = 0,
    BUTTON_EVENT_PB_RELEASED,
    BUTTON_EVENT_ENCODER_CCW,
    BUTTON_EVENT_ENCODER_CW
} ButtonAction_t;

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

/**
 * Buttons_OnButtonAction
 *
 * Callback called in the context of Buttons_CheckState.
 */
void Buttons_ButtonEventCallback(ButtonAction_t action);

#endif // __BUTTONS_H__
