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

static void cpuSetup();

int main() {
    cpuSetup();

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

static void cpuSetup()
{
    uint32_t i;

    // Power up system oscillator
    SCB_PDRUNCFG &= ~(SCB_PDRUNCFG_SYSOSC_MASK);

    // Setup the crystal input (bypass disabled, 1-20MHz crystal)
    SCB_SYSOSCCTRL = (SCB_SYSOSCCTRL_BYPASS_DISABLED | SCB_SYSOSCCTRL_FREQRANGE_1TO20MHZ);

    for (i = 0; i < 200; i++) {
        __asm volatile ("NOP");
    }

    // external crystal as PLL clock source
    SCB_PLLCLKSEL = SCB_CLKSEL_SOURCE_MAINOSC;
    SCB_PLLCLKUEN = SCB_PLLCLKUEN_UPDATE;
    SCB_PLLCLKUEN = SCB_PLLCLKUEN_DISABLE;
    SCB_PLLCLKUEN = SCB_PLLCLKUEN_UPDATE;

    // wait until the clock is updated
    while (!(SCB_PLLCLKUEN & SCB_PLLCLKUEN_UPDATE));

    // set x6 multiplier, FCCO = 2 * 2 * 72 = 288MHz
    SCB_PLLCTRL = (SCB_PLLCTRL_MSEL_6 | SCB_PLLCTRL_PSEL_2);

    // enable system PLL, wait for lock
    SCB_PDRUNCFG &= ~(SCB_PDRUNCFG_SYSPLL_MASK);
    while (!(SCB_PLLSTAT & SCB_PLLSTAT_LOCK));

    // setup main clock to use PLL output
    SCB_MAINCLKSEL = SCB_MAINCLKSEL_SOURCE_SYSPLLCLKOUT;
    SCB_MAINCLKUEN = SCB_MAINCLKUEN_UPDATE;
    SCB_MAINCLKUEN = SCB_MAINCLKUEN_DISABLE;
    SCB_MAINCLKUEN = SCB_MAINCLKUEN_UPDATE;

    // Wait until the clock is updated
    while (!(SCB_MAINCLKUEN & SCB_MAINCLKUEN_UPDATE));

    // set system AHB clock - div by 1
    SCB_SYSAHBCLKDIV = SCB_SYSAHBCLKDIV_DIV1;

    // enabled IOCON clock for GPIO
    SCB_SYSAHBCLKCTRL |= SCB_SYSAHBCLKCTRL_IOCON;
}

