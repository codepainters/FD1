#include "cpu/cpu.h"
#include "timer16/timer16.h"
#include "gpio/gpio.h"
#include "uart/uart.h"
#include "lpc134x.h"

#include "panel.h"
#include "keyboard.h"

volatile int timerCount = 0;
volatile int idx = 0;

int main() {
    int i;

	cpuInit();

    Keyboard_Init();
    Panel_Init();

    timer16Init(0, TIMER16_CCLK_100US);
    timer16Enable(0);

    uartInit(31250);

    while (1) {
        //uartSend((uint8_t*) "\x91\x11\x40\x81\x11\x40", 6);
        uartSend((uint8_t*) "\x91\x11\x40", 3);

        for (i = 0; i < 10000; i++) {
             __asm volatile ("NOP");
        }
	}

	return 0;
}

void TIMER16_0_IRQHandler(void) {
    /* Clear the interrupt flag */
    TMR_TMR16B0IR = TMR_TMR16B0IR_MR0;

    timerCount++;

    // Note: encoder needs such a high rate (10kHz / 3) to work reliably!
    Panel_TimerTick();

#if 0
    if ((timerCount % 10000) == 0) {
        idx ++;
        //Display_SetHex(idx);
        Display_SetLeds(idx);
    }
#endif

}
