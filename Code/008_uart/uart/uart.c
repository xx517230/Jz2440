#include "../s3c2440_soc.h"
#include "uart.h"

/*
uart是通用异步接受发送器，异步是时钟同步，即不需要时钟线。uart是全双工，即可以同时进行发送和接受。
uart的比特率，数据帧构成
*/
/*

ULCON0      0x50000000                      W       R/W     UART 0 line control     数据帧构成设置
UCON0       0x50000004                                      UART 0 control          UART时钟设置(主要)以及中断等设置
UFCON0      0x50000008                                      UART 0 FIFO control     FIFO使能、触发设置以及重置设置
UMCON0      0x5000000C                                      UART 0 modem control    
UTRSTAT0    0x50000010                              R       UART 0 Tx/Rx status
UERSTAT0    0x50000014                                      UART 0 Rx error status
UFSTAT0     0x50000018                                      UART 0 FIFO status
UMSTAT0     0x5000001C                                      UART 0 modem status
UTXH0       0x50000023      0x50000020      B       W       UART 0 transmission hold
URXH0       0x50000027      0x50000024      R               UART 0 receive buffer
UBRDIV0     0x50000028                      W       R/W     UART 0    baud rate divisor
*/
void uart0_init()
{
    
}

int putchar(int c);
int getchar(void);
int puts(const char *s);



