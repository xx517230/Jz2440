#include "s3c2440_soc.h"
#include "led/led.h"
#include "uart/uart.h"
#include "nor/nor.h"


int main(void)
{
	unsigned int c;
	uart0_init();
	while(1)
	{
		puts("Enter the Tacc val: \n\r");
		c = getchar();
		if (c >= '0' && c <= '7')
		{
			puts("val = ");
			putchar(c);
			puts("\n\r");
			bank0_nor_init(c - '0');
			led_cycle_light();
		}
		else
		{
			puts("Error, val should between 0~7\r\n");
			puts("Enter the Tacc val: \r\n");
		}
	}
    
    
    
    return 0;
}

