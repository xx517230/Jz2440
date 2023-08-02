#include "../s3c2440_soc.h"
#include "uart.h"

/*
uart��ͨ���첽���ܷ��������첽��ʱ��ͬ����������Ҫʱ���ߡ�uart��ȫ˫����������ͬʱ���з��ͺͽ��ܡ�
uart�ı����ʣ�����֡����
*/
/*

ULCON0      0x50000000                      W       R/W     UART 0 line control         ����֡��������
UCON0       0x50000004                                      UART 0 control              UARTʱ������(��Ҫ)��Tx/Rxʹ���Լ��жϵ�����
UFCON0      0x50000008                                      UART 0 FIFO control         FIFOʹ�ܡ����������Լ���������
UMCON0      0x5000000C                                      UART 0 modem control        AFCʹ�ܼ����
UTRSTAT0    0x50000010                              R       UART 0 Tx/Rx status         UART ���ܺͷ�����״̬���
UERSTAT0    0x50000014                                      UART 0 Rx error status      UART ������error�������
UFSTAT0     0x50000018                                      UART 0 FIFO status          UART FIFO״̬��ѯ
UMSTAT0     0x5000001C                                      UART 0 modem status         UART modem״̬��ѯ
UTXH0       0x50000023      0x50000020      B       W       UART 0 transmission hold    UTXH0 transmit buffer�е����ݷ��ʹ�
URXH0       0x50000027      0x50000024      R               UART 0 receive buffer       UART0 receive buffer�е����ݴ洢��
UBRDIV0     0x50000028                      W       R/W     UART 0 baud rate divisor    ������ʱ�����


���ڳ�����������PCʵʱ��ʾ��δʹ��FIFO��modem�Լ�������UART Rx��Error(��ΪӲ���ã����Ա������Ǽ򵥵�UART����ʵ��UART��ʼ���ͷ��ͽ��ܡ�
*/


void uart0_init()
{
    //������������URAT������ʹ��UART0����عܽ���TXD0/GPH2,RXD0/GPH3;ͬʱʹ���ڲ�����,UART����ʱΪ�ߵ�ƽ
    GPHCON &= ~((2<<6)|(2<<4));
    GPHCON |= (2<<6)|(2<<4);
    GPHUP &= ~((1<<2)|(1<<3));
    //��������֡8n1-8λ����λ����У��λ��1λֹͣλ
    ULCON0 |= (3<0);
    //����UARTʱ��Դ��Tx/Rxʹ��(ѡ���жϼ���ѯģʽ)
    //UARTʱ��Դ����: PCLK, UEXTCLK or FCLK/n;ѡ��PCLK�����Ҳ������FCLK����ϵ��
    UCON0 |= (1<<2)|(1<<0);
    //����ʱ�Ӻ�����UART�Ĳ�����
    /*
        UBRDIVn = (int)( UART clock / ( buad rate x 16) ) �C1
        ( UART clock: PCLK, FCLK/n or UEXTCLK )

        For example, if the baud-rate is 115200 bps and UART clock is 40 MHz, UBRDIVn is:
        UBRDIVn = (int)(40000000 / (115200 x 16) ) -1
        = (int)(21.7) -1 [round to the nearest whole number]
        = 22 -1 = 21

        ����������jz2440,    UBRDIVn = (int)(50000000 / (115200 x 16) ) -1
                                  = (int)(27.1267) -1 
                                  = 26 
    */
    UBRDIV0 = 26;
}

/*
          putchar
         ------->
          ARM Tx
    ARM             PC
   
          getchar         
         <-------
          ARM Rx


UART�����������д�������壬��ô�����������豸��������ʱ�Ƕ�ȡPC�ϵ����ݣ�д����ʱ����PC��д���ݡ�
getchar�Ƕ�ȡPC�ϵ����ݣ�����ֻ����һ�����ַ�����ûʹ��FIFO����putchar��һ�����ַ���PC�Ϸ��͡�
putֻ�ǵ���putchar���ַ�������ַ����Ͷ��ѡ��򻯿�������PC�Ϸ������ݶ��ѡ�Ϊʲôû�ṩget������
��Ϊ����PC����������д�����ݣ������ڿ�����UART���£�������ֻ��Ҫ����Ӧ�ļĴ����ж�ȡ��PC�ϻ�ȡ���ݼ��ɡ�

*/

/*
    �����巢�͸�pc��ʾ,��������Tx

    Ϊʲôputchar���������int,�µ�c��׼��Ľӿ�
    �⺯�� int putchar(int char) �Ѳ��� char ָ�����ַ���һ���޷����ַ���д�뵽��׼��� stdout �С�
    ����
    int putchar(int char)
    ����
        char -- ����Ҫ��д����ַ������ַ������Ӧ�� int ֵ���д��ݡ�
    ����ֵ
    �ú������޷��� char ǿ��ת��Ϊ int ����ʽ����д����ַ���������������򷵻� EOF��
*/
int putchar(int c)
{
/**
  Ϊʲô����ʱ���ڴ���Transmitter empty��Transmitter buffer empty�����ڽ���ʱֻ��Receive buffer data ready,��û��Receiver empty?
  ��������ʱ���޷���֪PC�ϵ�״̬���п���PCæ����ʱTransmitter buffer register�������ݸ���Transmit shift register��
  ��Transmit shift register��ΪPCæ��δ�ͳ������Բ���ֻ����Transmitter buffer empty���жϿ��Խ��ŷ�����һ�������ˣ�
  ��Ӧ���Ǹ���Transmitter empty���ж�Transmitter buffer empty��Transmit shift register��Ϊ�պ���ܼ���������һ�����ݡ�

  ��������ʱ��CPU��ͨ��Receive buffer register����ȡ�ģ�����CPU��˵������Ҫ֪��Receiver empty��

  UTRSTATn�Ĵ����ṩTransmitter empty��Receive buffer data ready�͹��ˣ�Ϊʲô�ṩ��Transmitter buffer empty�����ṩReceiver empty�أ�
  ����S3C2440��˵����������ʱ���Ǿ����Լ��ڲ������Լ�����Ƿ���æ��״̬����ʹæҲ���Լ����к������ݴ������Ƕ��ڷ�����һ����
  PC����æ�����޷�֪����Transmitter buffer empty��Transmitter empty����ѡ���Ͽ����ж��Ƿ�PC����״̬��������Ҫ����PC����æʱ��ش����ܡ�
*/
#define DEBUUG
#ifdef DEBUG
    while(!((UTRSTAT0>>2)&1)))
#else
    while(!(UTRSTAT0 & (1<<2)))
#endif
    {
        //UTXH0/URXH0����byte
        UTXH0=(unsigned char)c;
        return UTXH0;
    }
    return -1;

}

/*
* �����巢�͸�pc����������Rx
*/
int getchar(void)
{
    while(!(UTRSTAT0 & 1))
    {
        return URXH0;
    }

}


/*���ַ������������ʹ��putcharһ��������ַ�*/
/*
C �⺯�� int puts(const char *str) ��һ���ַ���д�뵽��׼��� stdout��ֱ�����ַ��������������ַ������з��ᱻ׷�ӵ�����С�
����
int puts(const char *str)
����
    str -- ����Ҫ��д��� C �ַ�����
����ֵ
����ɹ����ú�������һ���Ǹ�ֵΪ�ַ������ȣ�����ĩβ�� \0����������������򷵻� EOF��

*/
#define NULL 0
int puts(const char *str)
{
    int i=0;
    if( NULL == str) return 0;
    while(str[i])
    {
        putchar(str[i]);
        i++;
    }
    return ++i; //����'\0'
}



