
#include "gpio/gpio.h"
#include "lpc134x.h"

#include "buttons.h"
#include "buttons_defs.h"
#include "gpio_pin.h"

// encoder FSM states
typedef enum {
    ENC_START = 0, ENC_CW_FINAL, ENC_CW_BEGIN, ENC_CW_NEXT, ENC_CCW_BEGIN, ENC_CCW_FINAL, ENC_CCW_NEXT,

    ENC_EMIT_CW = 0x10,
    ENC_EMIT_CCW = 0x20
} EncoderState_t;

// encoder FSM transitions
static EncoderState_t encoderFsm[][4] = {
    [ENC_START] = 		{ ENC_START, ENC_CW_BEGIN, ENC_CCW_BEGIN, ENC_START },
    [ENC_CW_BEGIN] =	{ ENC_CW_NEXT, ENC_CW_BEGIN, ENC_START, ENC_START },
    [ENC_CW_NEXT] =		{ ENC_CW_NEXT, ENC_CW_BEGIN, ENC_CW_FINAL, ENC_START },
    [ENC_CW_FINAL] =	{ ENC_CW_NEXT, ENC_START, ENC_CW_FINAL, ENC_START | ENC_EMIT_CW },
    [ENC_CCW_BEGIN] =	{ ENC_CCW_NEXT, ENC_START, ENC_CCW_BEGIN, ENC_START },
    [ENC_CCW_FINAL] =	{ ENC_CCW_NEXT, ENC_CCW_FINAL, ENC_START, ENC_START | ENC_EMIT_CCW },
    [ENC_CCW_NEXT] =	{ ENC_CCW_NEXT, ENC_CCW_FINAL, ENC_CCW_BEGIN, ENC_START }
};

static EncoderState_t encoderState;

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
        Buttons_HandlePButton();
    }
}

void Display_SetInt(int aValue);

static void Buttons_HandlePButton()
{
    uint32_t val = gpioGetValue(ROW_PINS[ROW_PB2].portNum, ROW_PINS[ROW_PB2].pinNum);
    if (val == 0) {
        Display_SetInt(77);
    }
}

static int32_t v = 50;

static void Buttons_HandleEncoder()
{
    uint32_t pin1 = gpioGetValue(ROW_PINS[ROW_PB1].portNum, ROW_PINS[ROW_PB1].pinNum);
    uint32_t pin2 = gpioGetValue(ROW_PINS[ROW_PB2].portNum, ROW_PINS[ROW_PB2].pinNum);
    uint32_t inputs = ((pin1 << 1) | pin2) ^0x03;

    // make the FSM transition (note that we need to mask the ENC_EMIT_xxx flags)
    int nextState = encoderFsm[encoderState][inputs];
    encoderState = nextState & 0x0F;

    // note: encoderDelta is basically the last tick, it is zero'ed after reading
    if (nextState & ENC_EMIT_CW) {
        Display_SetInt(--v);
    }
    else if (nextState & ENC_EMIT_CCW) {
        Display_SetInt(++v);
    }
}
