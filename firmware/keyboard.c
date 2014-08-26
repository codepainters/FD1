#include "keyboard.h"
#include "keyboard_defs.h"

// special value used to denote a state when key event was triggered already
#define DEBOUNCE_IDLE   (-1)

// debounce period (ticks)
#define DEBOUNCE_PERIOD (100)

typedef struct {
    int debounceCounter;
    int previousState;
} KeyState_t;

static KeyState_t keys[KBD_ROWS][KBD_COLUMNS];

void Keyboard_Init()
{
    for (int i = 0; i < KBD_ROWS; i++) {
        for (int j = 0; j < KBD_COLUMNS; j++) {
            keys[i][j].debounceCounter = DEBOUNCE_IDLE;
            keys[i][j].previousState = 0;
        }
    }

    for (int i = 0; i < KBD_ROWS; i++) {
        GpioPin_ConfigureOut(&KBD_ROW_PINS[i], 1);
    }

    for (int i = 0; i < KBD_COLUMNS; i++) {
        GpioPin_ConfigureIn(&KBD_COLUMN_PINS[i]);
    }

}

void Keyboard_TimerTick()
{

}
