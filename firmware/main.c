#include "cpu/cpu.h"
#include "timer16/timer16.h"
#include "gpio/gpio.h"
#include "uart/uart.h"
#include "lpc134x.h"

#include "display.h"

volatile int timerCount = 0;
volatile int idx = 0;

int main() {
    int i;

	cpuInit();

    Display_Init();

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

    if ((timerCount & 0x0F) == 0) {
        Display_TimerTick();
    }

    if ((timerCount % 10000) == 0) {
        idx ++;
        Display_SetHex(idx);
        Display_SetLeds(idx);
    }

}
