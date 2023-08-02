#include "s3c2440_soc.h"
#include "led/led.h"
#include "uart/uart.h"
#include "nor/nor.h"
#include "sdram/sdram.h"


int main(void)
{
	uart0_init();

	sdram_init();

	if (sdram_test() == 0)
		led_cycle_light();

	return 0;
}

