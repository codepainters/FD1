
#include "buttons.h"
#include "buttons_defs.h"
#include "gpio_pin.h"

// how long (ticks) the button state has to remain stable
#define DEBOUNCE_DURATION  (100)

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

static EncoderState_t encoderState = ENC_START;

typedef struct {
    unsigned int pressDuration;
    unsigned int previousState;
} ButtonState_t;

// Note: we only have one push button, but let's make it somewhat generic
ButtonState_t pushButton = {0, 0};

static void Buttons_HandlePButton();
static void Buttons_HandleEncoder();

void Buttons_Init()
{
    int i;

    // Configure row pins - GPIO, input, pull-up
    for (i = 0; i < sizeof(ROW_PINS) / sizeof(ROW_PINS[0]); i++) {
        GpioPin_ConfigureIn(&ROW_PINS[i]);
    }
}

void Buttons_CheckState(int column)
{
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

    // debouncing logic
    if (val == pushButton.previousState) {
        if (pushButton.pressDuration < DEBOUNCE_DURATION) {
            pushButton.pressDuration++;
        }
        else if (pushButton.pressDuration == DEBOUNCE_DURATION)  {
            Buttons_ButtonEventCallback(val == 0 ? BUTTON_EVENT_PB_PRESSED : BUTTON_EVENT_PB_RELEASED);
            pushButton.pressDuration++;
        }
    }
    else {
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

    // note: encoderDelta is basically the last tick, it is zero'ed after reading
    if (nextState & ENC_EMIT_CW) {
        Buttons_ButtonEventCallback(BUTTON_EVENT_ENCODER_CW);
    }
    else if (nextState & ENC_EMIT_CCW) {
        Buttons_ButtonEventCallback(BUTTON_EVENT_ENCODER_CCW);
    }
}
