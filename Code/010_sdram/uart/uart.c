#include "../s3c2440_soc.h"
#include "uart.h"

/*
uart是通用异步接受发送器，异步是时钟同步，即不需要时钟线。uart是全双工，即可以同时进行发送和接受。
uart的比特率，数据帧构成
*/
/*

ULCON0      0x50000000                      W       R/W     UART 0 line control         数据帧构成设置
UCON0       0x50000004                                      UART 0 control              UART时钟设置(主要)、Tx/Rx使能以及中断等设置
UFCON0      0x50000008                                      UART 0 FIFO control         FIFO使能、触发设置以及重置设置
UMCON0      0x5000000C                                      UART 0 modem control        AFC使能及相关
UTRSTAT0    0x50000010                              R       UART 0 Tx/Rx status         UART 接受和发送器状态相关
UERSTAT0    0x50000014                                      UART 0 Rx error status      UART 接收器error相关设置
UFSTAT0     0x50000018                                      UART 0 FIFO status          UART FIFO状态查询
UMSTAT0     0x5000001C                                      UART 0 modem status         UART modem状态查询
UTXH0       0x50000023      0x50000020      B       W       UART 0 transmission hold    UTXH0 transmit buffer中的数据发送处
URXH0       0x50000027      0x50000024      R               UART 0 receive buffer       UART0 receive buffer中的数据存储处
UBRDIV0     0x50000028                      W       R/W     UART 0 baud rate divisor    波特率时钟相关


由于程序是用于与PC实时显示，未使用FIFO、modem以及忽略了UART Rx的Error(因为硬件好，所以本程序是简单的UART程序，实现UART初始化和发送接受。
*/


void uart0_init()
{
    //配置引脚用于URAT，由于使用UART0，相关管脚是TXD0/GPH2,RXD0/GPH3;同时使能内部上拉,UART空闲时为高电平
    GPHCON &= ~((2<<6)|(2<<4));
    GPHCON |= (2<<6)|(2<<4);
    GPHUP &= ~((1<<2)|(1<<3));
    //配置数据帧8n1-8位数据位，无校验位，1位停止位
    ULCON0 |= (3<0);
    //配置UART时钟源、Tx/Rx使能(选择中断及查询模式)
    //UART时钟源三种: PCLK, UEXTCLK or FCLK/n;选择PCLK，最简单也不用想FCLK设置系数
    UCON0 |= (1<<2)|(1<<0);
    //配置时钟后，配置UART的波特率
    /*
        UBRDIVn = (int)( UART clock / ( buad rate x 16) ) –1
        ( UART clock: PCLK, FCLK/n or UEXTCLK )

        For example, if the baud-rate is 115200 bps and UART clock is 40 MHz, UBRDIVn is:
        UBRDIVn = (int)(40000000 / (115200 x 16) ) -1
        = (int)(21.7) -1 [round to the nearest whole number]
        = 22 -1 = 21

        按上例计算jz2440,    UBRDIVn = (int)(50000000 / (115200 x 16) ) -1
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


UART程序最后是烧写到开发板，那么开发板是主设备，读数据时是读取PC上的数据，写数据时是往PC上写数据。
getchar是读取PC上的数据，而且只能是一个个字符读（没使用FIFO）；putchar是一个个字符往PC上发送。
put只是调用putchar将字符串逐个字符发送而已。简化开发板往PC上发送数据而已。为什么没提供get函数，
因为这是PC上往开发板写入数据，不属于开发板UART的事，开发板只需要从相应的寄存器中读取从PC上获取数据即可。

*/

/*
    开发板发送给pc显示,开发板是Tx

    为什么putchar输入参数是int,仿的c标准库的接口
    库函数 int putchar(int char) 把参数 char 指定的字符（一个无符号字符）写入到标准输出 stdout 中。
    声明
    int putchar(int char)
    参数
        char -- 这是要被写入的字符。该字符以其对应的 int 值进行传递。
    返回值
    该函数以无符号 char 强制转换为 int 的形式返回写入的字符，如果发生错误则返回 EOF。
*/
int putchar(int c)
{
/**
  为什么发送时存在存在Transmitter empty和Transmitter buffer empty；而在接受时只有Receive buffer data ready,而没有Receiver empty?
  发送数据时，无法得知PC上的状态，有可能PC忙，此时Transmitter buffer register已有数据给了Transmit shift register，
  但Transmit shift register因为PC忙还未送出。所以不能只依靠Transmitter buffer empty来判断可以接着发送下一次数据了，
  而应该是根据Transmitter empty来判断Transmitter buffer empty和Transmit shift register都为空后才能继续发送下一次数据。

  接受数据时，CPU是通过Receive buffer register来获取的，对于CPU来说，不需要知道Receiver empty。

  UTRSTATn寄存器提供Transmitter empty和Receive buffer data ready就够了，为什么提供了Transmitter buffer empty而不提供Receiver empty呢？
  对于S3C2440来说，接受数据时，是经过自己内部处理，自己清楚是否处于忙的状态；即使忙也是自己进行后续数据处理；但是对于发送则不一样，
  PC机器忙我们无法知道，Transmitter buffer empty和Transmitter empty两个选项结合可以判断是否PC机器状态，根据需要处理PC机器忙时相关处理功能。
*/
#define DEBUUG
#ifdef DEBUG
    while(!((UTRSTAT0>>2)&1)))
#else
    while(!(UTRSTAT0 & (1<<2)))
#endif
    {
        //UTXH0/URXH0都是byte
        UTXH0=(unsigned char)c;
        return UTXH0;
    }
    return -1;

}

/*
* 开发板发送给pc，开发板是Rx
*/
int getchar(void)
{
    while(!(UTRSTAT0 & 1))
    {
        return URXH0;
    }

}


/*简化字符串输出，避免使用putchar一个个输出字符*/
/*
C 库函数 int puts(const char *str) 把一个字符串写入到标准输出 stdout，直到空字符，但不包括空字符。换行符会被追加到输出中。
声明
int puts(const char *str)
参数
    str -- 这是要被写入的 C 字符串。
返回值
如果成功，该函数返回一个非负值为字符串长度（包括末尾的 \0），如果发生错误则返回 EOF。

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
    return ++i; //包括'\0'
}



