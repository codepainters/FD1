#include "cpu/cpu.h"
#include "timer16/timer16.h"
#include "lpc134x.h"

#include "gpio_pin.h"
#include "panel.h"
#include "keyboard.h"
#include "midi.h"
#include "settings_store.h"

volatile int timerCount = 0;

#define USE_DEBUG_PIN 1

#if USE_DEBUG_PIN
static const GpioPin_t DEBUG_PIN = { &IOCON_PIO0_1, 0, 1, IOCON_PIO0_1_FUNC_GPIO | IOCON_PIO0_1_HYS_DISABLE | IOCON_PIO0_1_MODE_INACTIVE };
#define SET_DEBUG_PIN(v) do { GpioPin_SetState(&DEBUG_PIN, (v)); } while(0)
#else
#define SET_DEBUG_PIN(v)
#endif

int main() {
	cpuInit();

#if USE_DEBUG_PIN
    GpioPin_ConfigureOut(&DEBUG_PIN, 0);
#endif

    SettingsStore_Init();

    // intiailize sub-modules
    MIDI_Init();
    Panel_Init();

    // FIXME: panel must be configured first at the moment,
    // as LPC1343CodeBase SPI driver reconfigures PIO0.2
    Keyboard_Init();

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

    SET_DEBUG_PIN(1);

    Keyboard_TimerTick();

    // Note: encoder needs such a high rate (10kHz / 3) to work reliably!
    Panel_TimerTick();

    SET_DEBUG_PIN(0);
}
