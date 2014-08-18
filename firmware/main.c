#include "cpu/cpu.h"
#include "timer16/timer16.h"
#include "gpio/gpio.h"
#include "uart/uart.h"
#include "lpc134x.h"

volatile int timerCount = 0;
volatile int idx = -9;

int main() {
    int i;

	cpuInit();

    // main clock to CLKOUT pin
    SCB_CLKOUTCLKSEL = 0x3;
    SCB_CLKOUTCLKUEN = SCB_CLKOUTCLKUEN_UPDATE;
    SCB_CLKOUTCLKUEN = SCB_CLKOUTCLKUEN_DISABLE;
    SCB_CLKOUTCLKUEN = SCB_CLKOUTCLKUEN_UPDATE;
    while ( !(SCB_CLKOUTCLKUEN & 0x01) );
    SCB_CLKOUTCLKDIV = SCB_CLKOUTCLKDIV_DIV1;

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

