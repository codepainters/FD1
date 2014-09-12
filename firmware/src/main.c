#include "cpu/cpu.h"
#include "lpc134x.h"

#include "gpio_pin.h"
#include "panel.h"
#include "keyboard.h"
#include "midi.h"
#include "settings_store.h"

#define USE_DEBUG_PIN 1

#if USE_DEBUG_PIN
static const GpioPin_t DEBUG_PIN = { &IOCON_PIO0_1, 0, 1, IOCON_PIO0_1_FUNC_GPIO | IOCON_PIO0_1_HYS_DISABLE | IOCON_PIO0_1_MODE_INACTIVE };
#define SET_DEBUG_PIN(v) do { GpioPin_SetState(&DEBUG_PIN, (v)); } while(0)
#else
#define SET_DEBUG_PIN(v)
#endif

#define TIMER16_CCLK_100US      ((CFG_CPU_CCLK/SCB_SYSAHBCLKDIV) / 10000)

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

    // enable TMR16B0 clock
    SCB_SYSAHBCLKCTRL |= SCB_SYSAHBCLKCTRL_CT16B0;

    // configure TMR16B0 to kick at 10kHz
    TMR_TMR16B0MR0 = TIMER16_CCLK_100US;
    TMR_TMR16B0MCR = TMR_TMR16B0MCR_MR0_INT_ENABLED | TMR_TMR16B0MCR_MR0_RESET_ENABLED;

    // Enable the TIMER0 interrupt, end eventually enable the timer itself
    NVIC_EnableIRQ(TIMER_16_0_IRQn);
    TMR_TMR16B0TCR = TMR_TMR16B0TCR_COUNTERENABLE_ENABLED;

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
