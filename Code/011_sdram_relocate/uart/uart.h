
#ifndef _UART_H
#define _UART_H

#include "../s3c2440_soc.h"

void uart0_init();
int putchar(int c);
int getchar(void);
int puts(const char *s);
void printHex(unsigned int val);
#endif




