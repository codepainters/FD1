#include "cpu/cpu.h"
#include "timer16/timer16.h"
#include "gpio/gpio.h"
#include "lpc134x.h"

#include "panel.h"
#include "keyboard.h"
#include "midi.h"

volatile int timerCount = 0;

int main() {
	cpuInit();

    // intiailize sub-modules
    MIDI_Init();
    Keyboard_Init();
    Panel_Init();

    // configure timer to kick at 10kHz
    timer16Init(0, TIMER16_CCLK_100US);
    timer16Enable(0);

    // keyboard and panel handling is performed in the timer interrupt context
    // all we need to do in the main code is to send any pendind MIDI data
    // and go to sleep until next interrupt

    while (1) {
        MIDI_SendQueued();

        // Wait For Interrupt
        __asm volatile ("wfi");
	}

	return 0;
}

void TIMER16_0_IRQHandler(void) {
    // Clear the interrupt flag
    TMR_TMR16B0IR = TMR_TMR16B0IR_MR0;

    Keyboard_TimerTick();

    // Note: encoder needs such a high rate (10kHz / 3) to work reliably!
    Panel_TimerTick();
}
