#include "s3c2440_soc.h"

/* 
Interrupt Controller
    SRCPND 0X4A000000 W R/W Interrupt request status
INTMOD 0X4A000004 W Interrupt mode control
INTMSK 0X4A000008 R/W Interrupt mask control
    PRIORITY 0X4A00000C W IRQ priority control
INTPND 0X4A000010 R/W Interrupt request status
INTOFFSET 0X4A000014 R Interrupt request source offset
    // SUBSRCPND 0X4A000018 R/W Sub source pending
    // INTSUBMSK 0X4A00001C R/W Interrupt sub mask 
*/
/* 
I/O port
    // GPACON 0x56000000 ? W R/W Port A control
    // GPADAT 0x56000004 Port A data
    // GPBCON 0x56000010 Port B control
    // GPBDAT 0x56000014 Port B data
    // GPBUP 0x56000018 Pull-up control B
    // GPCCON 0x56000020 Port C control
    // GPCDAT 0x56000024 Port C data
    // GPCUP 0x56000028 Pull-up control C
    // GPDCON 0x56000030 Port D control
    // GPDDA1T 0x56000034 Port D data
    // GPDUP 0x56000038 Pull-up control D
    // GPECON 0x56000040 Port E control
    // GPEDAT 0x56000044 Port E data
    // GPEUP 0x56000048 Pull-up control E
GPFCON 0x56000050 Port F control
    // GPFDAT 0x56000054 Port F data
    // GPFUP 0x56000058 Pull-up control F
GPGCON 0x56000060 Port G control
    // GPGDAT 0x56000064 Port G data
    // GPGUP 0x56000068 Pull-up control G
    // GPHCON 0x56000070 Port H control
    // GPHDAT 0x56000074 Port H data
    // GPHUP 0x56000078 Pull-up control H
    // GPJCON 0x560000D0 Port J control
    // GPJDAT 0x560000D4 Port J data
    // GPJUP 0x560000D8 Pull-up control J
    // MISCCR 0x56000080 Miscellaneous control
    // DCLKCON 0x56000084 DCLK0/1 control
EXTINT0 0x56000088 External interrupt control register 0
EXTINT1 0x5600008C External interrupt control register 1
EXTINT2 0x56000090 External interrupt control register 2
    // EINTFLT0 0x56000094 ? W R/W Reserved
    // EINTFLT1 0x56000098 Reserved
    // EINTFLT2 0x5600009C External interrupt filter control register 2
    // EINTFLT3 0x560000A0 External interrupt filter control register 3
EINTMASK 0x560000A4 External interrupt mask
EINTPEND 0x560000A8 External interrupt pending
    // GSTATUS0 0x560000AC R External pin status
    // GSTATUS1 0x560000B0 R/W Chip ID
    // GSTATUS2 0x560000B4 Reset status
    // GSTATUS3 0x560000B8 Inform register
    // GSTATUS4 0x560000BC Inform register
    // MSLCON 0x560000CC Memory sleep control register 
*/

/* 1.�жϳ�ʼ��
    1.1 �ж�Դʹ��(��ʼ��)
        1.1 ���ð���Ϊ�ж�����
        1.2 �ж�Դʹ��
        1.3 �����ж�����Ϊ˫���ش���
    1.2 �жϿ�����ʹ��
    1.3 ����cpu�����ж�Դ */
 

void key_eint_init()
{
    //�ĸ���������
    // EINT0/GPF0
    // EINT2/GPF2
    // EINT11/GPG3
    // EINT19/GPG11
//0 cpu�����ж�Դʹ�ܣ�ͨ������CPRS�Ĵ�����I flag(irq)�� F flag(fiq), start.S�л��ʵ��

//1.�ж�Դʹ��
    //1.1 ����Ϊ�ж����� ������GPNCON�Ĵ�������ΪGPFCON(GPF0/GPF2)  GPGCON(GPG3/GPG11)
    GPFCON &=~((3<<0)|(3<<4));
    GPFCON |=((2<<0)|(2<<4));
    GPGCON &=~((3<<6)|(3<<22));
    GPGCON |=((2<<6)|(2<<22));
    //1.2 �ж��������ã��ж�Դʹ��/�жϴ�����ʽ��
    //1.3�ж�Դʹ�� EIINT0��EINT3ֱ������CPU��ϵͳ������Ϊʹ�ܣ���ֻ������EINT11/EINT19
    EINTMASK &=~((1<<11)|(1<<19)); //EINT11[11]/EINT19[19] 0:enable interrupt 1:masked interrupt
    //1.4�����ж����ŵĴ�����ʽ->�����ж���Ϊ�����ɿ�Ϊһ��������Ϣ->˫���ش���
    EXTINT0 &=~((7<<0)|(7<<8));//EXIT0/EINT2
    EXTINT0 |=((7<<0)|(7<<8));//EXIT0/EINT2
    EXTINT1 &=~(7<<12);//EINT11
    EXTINT1 |=(7<<12);//EINT11
    EXTINT2 &=~(7<<12);//EINT19
    EXTINT2 |=(7<<12);//EINT19
    

//2 �жϿ�����ʹ��
    //2.1 ���÷��͵��ж�ģʽ ����irq�жϻ���ifq�ж� 
    INTMOD &= ~(1<<0);//0 = IRQ mode 1 = FIQ mode ����ֻ��1bit
    //2.2 �����жϿ������ܽ��ܵ��ж�Դ ->���ý���ΪEINT11/EINT19��EINT0/2ϵͳĬ�Ͻӵ�CPU�ˣ�����Ҫ����
    /*  ˵��:����EINT4~23ֻ����2bit��ʹ��EINT4~23,����֮�������޷�ȷ����4-23�е��Ǹ��������ж�,��ʱ��������EINTPEND�Ĵ���
        �������������ж�EINT4~23���Ǹ��������ж� ,������ΪSRCPND/INTPND�Ĵ�����ֻ����2bitȷ��EINT4~23�������޷��ֱ�����Ǹ��жϣ�
        ����EINTPEND�Ǹ���ȷ�ϵ�
    */
    INTMSK &=~((1<<0)|(1<<2)|(1<<5));//EINT8_23 [5]    EINT1 [1] EINT0 [0]    0 = Service available, 1 = Masked               
}
/*  
    ������
        handle_irqΪ�жϴ���������Ҫ�ж��Ǹ��ж�Դ�������жϣ�֮������ж�Դ
        ִ�ж�Ӧ�Ĵ������
 */
void handle_irq()
{
//1.�ֱ��ж�Դ
    //EINT0��1ֱ����INTPND�жϣ�EINT4��23��ҪEINTPEND�Ĵ�����INTPND�Ĵ���һ�����ж�
    volatile unsigned int cpuIrqSatus= INTPND; 
    volatile unsigned int srcIrqSatus= EINTPEND;
    volatile unsigned int irqIrqNo= INTOFFSET;//INTOFFSETֻ��irqģʽ��Ч
//2.�õ��ж�Դ��ִ�ж�Ӧ�Ķ�Ӧ�Ĵ�����
    //�ĸ���������
    // EINT0/GPF0
    // EINT2/GPF2
    // EINT11/GPG3
    // EINT19/GPG11
    if(irqIrqNo == 0)//EINT0
    {
        if (GPFDAT & (1<<0)) /* s2 --> gpf6 */
        {
            /* �ɿ� */
            GPFDAT |= (1<<6);
        }
        else
        {
            /* ���� */
            GPFDAT &= ~(1<<6);
        }
    }else if(irqIrqNo == 2)//EINT2
    {
        if (GPFDAT & (1<<2)) /* s3 --> gpf5 */
        {
            /* �ɿ� */
            GPFDAT |= (1<<5);
        }
        else
        {
            /* ���� */
            GPFDAT &= ~(1<<5);
        }
    }
    
    else if( irqIrqNo ==5 && (EINTPEND & (1<<11)) ) //EINT11
    {
        
		if (GPGDAT & (1<<3)) /* s4 --> gpf4 */
		{
			/* �ɿ� */
			GPFDAT |= (1<<4);
		}
		else
		{
			/* ���� */
			GPFDAT &= ~(1<<4);
		}
    }
    else if( irqIrqNo ==5 && (EINTPEND & (1<<19)) ) //EINT19
    {
			if (GPGDAT & (1<<11))
			{
				/* �ɿ� */
				/* Ϩ������LED */
				GPFDAT |= ((1<<4) | (1<<5) | (1<<6));
			}
			else
			{
				/* ����: ��������LED */
				GPFDAT &= ~((1<<4) | (1<<5) | (1<<6));
			}
    }
//3.ִ����������ж�
    // ���ж�Դ��SRCPND/EINTPEND��->�жϿ���������(INTMSK/INTPND)
    EINTPEND = srcIrqSatus; 
    SRCPND =(1<<irqIrqNo);//EINT0~3��Ӧ[0:3],EINT4~7��Ӧ[4] EINT8~23��Ӧ[5]
    INTPND =(1<<irqIrqNo);
}
