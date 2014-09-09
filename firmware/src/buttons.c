/**
 * Copyright (c) 2014, Przemyslaw Wegrzyn <pwegrzyn@codepainters.com>
 * All rights reserved.
 *
 * This file is distributed under the Modified BSD License.
 * See LICENSE.txt for details.
 */

#include "buttons.h"
#include "buttons_defs.h"
#include "gpio_pin.h"

// encoder FSM states
typedef enum {
    ENC_START = 0, ENC_CW_BEGIN, ENC_CW_NEXT, ENC_CW_FINAL, ENC_CCW_BEGIN, ENC_CCW_NEXT, ENC_CCW_FINAL,

    ENC_EMIT_CW = 0x10,
    ENC_EMIT_CCW = 0x20
} EncoderState_t;

// encoder FSM transitions
static EncoderState_t encoderFsm[][4] = {
    /* ENC_START */		{ ENC_START, ENC_CCW_BEGIN, ENC_CW_BEGIN, ENC_START },
    /* ENC_CW_BEGIN */	{ ENC_START, ENC_START, ENC_CW_BEGIN, ENC_CW_NEXT },
    /* ENC_CW_NEXT */	{ ENC_START, ENC_CW_FINAL, ENC_CW_BEGIN, ENC_CW_NEXT },
    /* ENC_CW_FINAL */	{ ENC_START | ENC_EMIT_CW, ENC_CW_FINAL, ENC_START, ENC_CW_NEXT },
    /* ENC_CCW_BEGIN */	{ ENC_START, ENC_CCW_BEGIN, ENC_START, ENC_CCW_NEXT },
    /* ENC_CCW_NEXT */	{ ENC_START, ENC_CCW_BEGIN, ENC_CCW_FINAL, ENC_CCW_NEXT },
    /* ENC_CCW_FINAL */	{ ENC_START | ENC_EMIT_CCW, ENC_START, ENC_CCW_FINAL, ENC_CCW_NEXT }
};

// current state of the encoder FSM
static EncoderState_t encoderState = ENC_START;

// state of individual panel button, used for debouncing
typedef struct {
    unsigned int previousState;
    unsigned int pressDuration;
} ButtonState_t;

// note: we only have a single push button
ButtonState_t pushButton = {0, 0};

static void Buttons_HandlePButton();
static void Buttons_HandleEncoder();

void Buttons_Init()
{
    // configure row pins - GPIO, input, pull-up
    for (unsigned int i = 0; i < sizeof(ROW_PINS) / sizeof(ROW_PINS[0]); i++) {
        GpioPin_ConfigureIn(&ROW_PINS[i]);
    }
}

void Buttons_CheckState(int column)
{
    // Note: we only have one push button and one encoder, hence the code
    // is not too generic
    if (column == COLUMN_ENCODER) {
        Buttons_HandleEncoder();
    }
    else if (column == COLUMN_PBUTTON) {
        Buttons_HandlePButton(&pushButton);
    }
}

static void Buttons_HandlePButton()
{
    uint32_t val = GpioPin_GetState(&ROW_PINS[ROW_PB2]);

    if (val == 0 && pushButton.previousState == 0) {
        if (pushButton.pressDuration <= LONG_PRESS_DURATION) {
            pushButton.pressDuration++;
        }

        if (pushButton.pressDuration == DEBOUNCE_DURATION)  {
            Buttons_ButtonEventCallback(BUTTON_EVENT_PB_PRESSED);
        }
        else if (pushButton.pressDuration == LONG_PRESS_DURATION)  {
            Buttons_ButtonEventCallback(BUTTON_EVENT_PB_LONG_PRESSED);
        }
    }
    else if (val == 1) {
        // button was released, emit release event, unless it is a long press
        if (pushButton.pressDuration >= DEBOUNCE_DURATION &&
                pushButton.pressDuration < LONG_PRESS_DURATION) {
            Buttons_ButtonEventCallback(BUTTON_EVENT_PB_RELEASED);
        }
        pushButton.pressDuration = 0;
    }

    pushButton.previousState = val;
}

static void Buttons_HandleEncoder()
{
    // Pin 1 & 2 are 0 when encoder is idle
    uint32_t inputs = (GpioPin_GetState(&ROW_PINS[ROW_PB2]) << 1) | GpioPin_GetState(&ROW_PINS[ROW_PB1]);

    // make the FSM transition (note that we need to mask the ENC_EMIT_xxx flags)
    int nextState = encoderFsm[encoderState][inputs];
    encoderState = nextState & 0x0F;

    if (nextState & ENC_EMIT_CW) {
        Buttons_ButtonEventCallback(BUTTON_EVENT_ENCODER_CW);
    }
    else if (nextState & ENC_EMIT_CCW) {
        Buttons_ButtonEventCallback(BUTTON_EVENT_ENCODER_CCW);
    }
}
