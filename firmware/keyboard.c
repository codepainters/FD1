#include "keyboard.h"
#include "keyboard_defs.h"

// special value used to denote a state when key event was triggered already
#define DEBOUNCE_IDLE   (-1)

// debounce period (ticks)
#define DEBOUNCE_PERIOD (100)

typedef struct {
    int debounceCounter;
    int previousState;
    bool pressed;
} KeyState_t;

static KeyState_t keys[KBD_TOTAL_KEYS];

static unsigned int currentScanRow = 0;

static void Keyboard_HandleKeyAction(unsigned int index);

void Keyboard_Init()
{
    for (int i = 0; i < KBD_TOTAL_KEYS; i++) {
        keys[i].debounceCounter = DEBOUNCE_IDLE;
        keys[i].previousState = 0;
        keys[i].pressed = false;
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
    // Disable current row and advance to next one
    gpioSetValue(KBD_ROW_PINS[currentScanRow].portNum, KBD_ROW_PINS[currentScanRow].pinNum, 1);
    currentScanRow = (currentScanRow + 1) % KBD_ROWS;
    gpioSetValue(KBD_ROW_PINS[currentScanRow].portNum, KBD_ROW_PINS[currentScanRow].pinNum, 0);

    for (unsigned int column = 0; column < KBD_COLUMNS; column++) {
        unsigned int state = gpioGetValue(KBD_COLUMN_PINS[column].portNum, KBD_COLUMN_PINS[column].pinNum) ^ 0x01;

        unsigned int index = KBD_KEY_INDEX(currentScanRow, column);
        KeyState_t* key = &keys[index];

        if (!key->pressed) {
            // for press detection we accept first rising edge (i.e. no debouncing) to make attack timing better
            if (key->previousState == 0 && state == 1) {
                key->pressed = true;
                Keyboard_HandleKeyAction(index);
            }
        }
        else {
            // for key release detection regular debouncing is performed
            if (state == 0) {
                if (key->previousState == 1) {
                    key->debounceCounter = DEBOUNCE_PERIOD;
                }
                else if(--key->debounceCounter == 0) {
                    key->pressed = false;
                    Keyboard_HandleKeyAction(index);
                }
            }
        }

        key->previousState = state;
    }
}

static void Keyboard_HandleKeyAction(unsigned int index)
{
}

