#include <stdbool.h>

#include "keyboard.h"
#include "keyboard_defs.h"
#include "settings.h"
#include "midi.h"

typedef struct {
    // debounce counter is initailized with DEBOUNCE_PERIOD and counts down
    int debounceCounter;
    // previous-tick state of the key (without debouncing)
    int previousState;
    // true, if key is considered pressed at the moment
    bool pressed;

    // if true, NOTE ON message was sent, and NOTE OFF shall be enqueued
    // once the key is released
    bool noteOnSent;

    // MIDI note params for the pressed key
    unsigned int midiNote;
    unsigned int midiVelocity;
    unsigned int midiChannel;

} KeyState_t;

static KeyState_t keys[KBD_TOTAL_KEYS];

static unsigned int currentScanRow = 0;

static void Keyboard_HandleKeyAction(unsigned int index);

void Keyboard_Init()
{
    for (int i = 0; i < KBD_TOTAL_KEYS; i++) {
        keys[i].debounceCounter = 0;
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
    KeyState_t* key = &keys[index];

    // Note: it is assumed that MIDI OUT is fast enough (~500 notes/s), that
    // there is no need to control if key's NOTE ON/OFF were sent already when
    // key press is detected

    if(key->pressed) {
        // set note parameters
        key->midiChannel = settings.midiChannel;
        key->midiVelocity = settings.velocity;
        key->midiNote = KBD_LEFTMOST_NOTE + (settings.octave * 12) + index;

        key->noteOnSent = MIDI_QueueNoteOn(key->midiChannel, key->midiNote, key->midiVelocity);
    }
    else if (key->noteOnSent) {
        // NOTE ON was previously sent and key was just depressed -> send NOTE OFF
        MIDI_QueueNoteOff(key->midiChannel, key->midiNote);
        key->noteOnSent = false;
    }
}

