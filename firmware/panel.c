#include "panel.h"
#include "settings.h"
#include "display.h"
#include "buttons.h"

typedef enum {
    EDIT_CHANNEL = 0,
    EDIT_VELOCITY,
    EDIT_OCTAVE,
    _EDIT_MODE_COUNT
} EditMode_t;

static void Panel_EditValue(int delta);
static void Panel_UpdateDisplay();
static int Panel_AdjustValue(int value, int delta, int min, int max);

static EditMode_t editMode = EDIT_CHANNEL;

void Panel_Init()
{
    Display_Init();
    Buttons_Init();

    Panel_UpdateDisplay();
}

void Panel_TimerTick()
{
    Display_TimerTick();
}

void Buttons_ButtonEventCallback(ButtonAction_t action)
{
    switch(action) {
    case BUTTON_EVENT_PB_PRESSED:
        editMode = (editMode + 1) % _EDIT_MODE_COUNT;
        break;

    case BUTTON_EVENT_ENCODER_CW:
        Panel_EditValue(1);
        break;

    case BUTTON_EVENT_ENCODER_CCW:
        Panel_EditValue(-1);
        break;

    default:
        break;
    }

    Panel_UpdateDisplay();
}

static void Panel_EditValue(int delta)
{
    switch(editMode) {
    case EDIT_CHANNEL:
        settings.midiChannel = Panel_AdjustValue(settings.midiChannel, delta, MIDI_CHANNEL_MIN, MIDI_CHANNEL_MAX);
        break;

    case EDIT_VELOCITY:
        settings.velocity += Panel_AdjustValue(settings.velocity, delta, MIDI_VELOCITY_MIN, MIDI_VELOCITY_MAX);
        break;

    case EDIT_OCTAVE:
        settings.octave += Panel_AdjustValue(settings.octave, delta, OCTAVE_SHIFT_MIN, OCTAVE_SHIFT_MAX);;
        break;

    default:
        break;
    }
}

static int Panel_AdjustValue(int value, int delta, int min, int max)
{
    value += delta;
    if (value < min) {
        return min;
    }
    else if (value > max) {
        return max;
    }
    return value;
}

static void Panel_UpdateDisplay()
{
    switch(editMode) {
    case EDIT_CHANNEL:
        Display_SetLeds(Display_LED1);
        Display_SetInt(settings.midiChannel + 1);
        break;

    case EDIT_VELOCITY:
        Display_SetLeds(Display_LED2);
        Display_SetHex(settings.velocity);
        break;

    case EDIT_OCTAVE:
        Display_SetLeds(Display_LED3);
        Display_SetInt(settings.octave);
        break;

    default:
        break;
    }
}

