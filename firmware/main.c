#include "cpu/cpu.h"
#include "timer16/timer16.h"
#include "gpio/gpio.h"

volatile int timerCount = 0;
volatile int idx = -9;

int main() {
	cpuInit();

	timer16Init(0, TIMER16_CCLK_100US);
	timer16Enable(0);

	while (1) {
	}

	return 0;
}

