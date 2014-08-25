#ifndef __PANEL_H__
#define __PANEL_H__

typedef enum {
    ACTION_PBUTTON_PRESS = 0,
    ACTION_PBUTTON_RELEASE,
    ACTION_ENCODER_CCW,
    ACTION_ENCODER_CW
} PanelButtonAction_t;

void Panel_Init();

void Panel_HandleButtonAction(PanelButtonAction_t action);

#endif // __PANEL_H__
