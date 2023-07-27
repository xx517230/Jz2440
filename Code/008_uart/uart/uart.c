#include "../s3c2440_soc.h"
#include "uart.h"

/*
uart��ͨ���첽���ܷ��������첽��ʱ��ͬ����������Ҫʱ���ߡ�uart��ȫ˫����������ͬʱ���з��ͺͽ��ܡ�
uart�ı����ʣ�����֡����
*/
/*

ULCON0      0x50000000                      W       R/W     UART 0 line control     ����֡��������
UCON0       0x50000004                                      UART 0 control          UARTʱ������(��Ҫ)�Լ��жϵ�����
UFCON0      0x50000008                                      UART 0 FIFO control     FIFOʹ�ܡ����������Լ���������
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



