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

static EditMode_t editMode = EDIT_CHANNEL;


static void Panel_EditValue(int delta);
static void Panel_UpdateDisplay();

void Panel_Init()
{
    Display_Init();
    Buttons_Init();
}

void Panel_HandleButtonAction(PanelButtonAction_t action)
{
    switch(action) {
    case ACTION_PBUTTON_PRESS:
        editMode = (editMode + 1) % _EDIT_MODE_COUNT;
        break;

    case ACTION_ENCODER_CW:
        Panel_EditValue(1);
        break;

    case ACTION_ENCODER_CCW:
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
        settings.midiChannel += delta;
        break;

    case EDIT_VELOCITY:
        settings.velocity += delta;
        break;

    case EDIT_OCTAVE:
        settings.octave += delta;
        break;

    default:
        break;
    }
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

