#include "s3c2440_soc.h"
#include "led/led.h"
#include "uart/uart.h"
#include "nor/nor.h"


int main(void)
{
	unsigned char c;

	led_cycle_light();
	uart0_init();
	puts("Hello, world!\n\r");
	
	while(1)
	{
		c = getchar();
		if (c == '\r')
		{
			putchar('\n');
		}

		if (c == '\n')
		{
			putchar('\r');
		}

		putchar(c);
	}
    
    
    
    return 0;
}

