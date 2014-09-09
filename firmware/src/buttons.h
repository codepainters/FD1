/**
 * Copyright (c) 2014, Przemyslaw Wegrzyn <pwegrzyn@codepainters.com>
 * All rights reserved.
 *
 * This file is distributed under the Modified BSD License.
 * See LICENSE.txt for details.
 */

#ifndef __BUTTONS_H__
#define __BUTTONS_H__

typedef enum {
    BUTTON_EVENT_PB_PRESSED = 0,
    BUTTON_EVENT_PB_LONG_PRESSED,
    BUTTON_EVENT_PB_RELEASED,
    BUTTON_EVENT_ENCODER_CCW,
    BUTTON_EVENT_ENCODER_CW
} ButtonAction_t;

void Buttons_Init();

/**
 * Called periodically to check the buttons state and perform denouncing.
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
