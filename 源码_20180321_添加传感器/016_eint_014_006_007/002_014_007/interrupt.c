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

/* 1.中断初始化
    1.1 中断源使能(初始化)
        1.1 设置按键为中断引脚
        1.2 中断源使能
        1.3 设置中断引脚为双边沿触发
    1.2 中断控制器使能
    1.3 配置cpu接受中断源 */
 

void key_eint_init()
{
    //四个按键引脚
    // EINT0/GPF0
    // EINT2/GPF2
    // EINT11/GPG3
    // EINT19/GPG11
//0 cpu接受中断源使能，通过设置CPRS寄存器的I flag(irq)或 F flag(fiq), start.S中汇编实现

//1.中断源使能
    //1.1 配置为中断引脚 即设置GPNCON寄存器，现为GPFCON(GPF0/GPF2)  GPGCON(GPG3/GPG11)
    GPFCON &=~((3<<0)|(3<<4));
    GPFCON |=((2<<0)|(2<<4));
    GPGCON &=~((3<<6)|(3<<22));
    GPGCON |=((2<<6)|(2<<22));
    //1.2 中断引脚设置（中断源使能/中断触发方式）
    //1.3中断源使能 EIINT0～EINT3直接连到CPU且系统已设置为使能，故只需设置EINT11/EINT19
    EINTMASK &=~((1<<11)|(1<<19)); //EINT11[11]/EINT19[19] 0:enable interrupt 1:masked interrupt
    //1.4设置中断引脚的触发方式->按键中断设为按下松开为一个触发信息->双边沿触发
    EXTINT0 &=~((7<<0)|(7<<8));//EXIT0/EINT2
    EXTINT0 |=((7<<0)|(7<<8));//EXIT0/EINT2
    EXTINT1 &=~(7<<12);//EINT11
    EXTINT1 |=(7<<12);//EINT11
    EXTINT2 &=~(7<<12);//EINT19
    EXTINT2 |=(7<<12);//EINT19
    

//2 中断控制器使能
    //2.1 设置发送的中断模式 即是irq中断还是ifq中断 
    INTMOD &= ~(1<<0);//0 = IRQ mode 1 = FIQ mode 有且只有1bit
    //2.2 设置中断控制器能接受的中断源 ->设置接受为EINT11/EINT19，EINT0/2系统默认接到CPU了，不需要设置
    /*  说明:由于EINT4~23只用了2bit来使能EINT4~23,但是之后我们无法确定是4-23中的那个产生的中断,此时就增加了EINTPEND寄存器
        用来方便我们判断EINT4~23是那个产生了中断 ,这是因为SRCPND/INTPND寄存器都只用了2bit确定EINT4~23，所以无法分辨具体那个中断，
        所以EINTPEND是辅助确认的
    */
    INTMSK &=~((1<<0)|(1<<2)|(1<<5));//EINT8_23 [5]    EINT1 [1] EINT0 [0]    0 = Service available, 1 = Masked               
}
/*  
    描述：
        handle_irq为中断处理函数，需要判断那个中断源产生的中断，之后根据中断源
        执行对应的处理程序
 */
void handle_irq()
{
//1.分辨中断源
    //EINT0～1直接由INTPND判断，EINT4～23需要EINTPEND寄存器和INTPND寄存器一起来判断
    volatile unsigned int cpuIrqSatus= INTPND; 
    volatile unsigned int srcIrqSatus= EINTPEND;
    volatile unsigned int irqIrqNo= INTOFFSET;//INTOFFSET只在irq模式有效
//2.得到中断源后执行对应的对应的处理函数
    //四个按键引脚
    // EINT0/GPF0
    // EINT2/GPF2
    // EINT11/GPG3
    // EINT19/GPG11
    if(irqIrqNo == 0)//EINT0
    {
        if (GPFDAT & (1<<0)) /* s2 --> gpf6 */
        {
            /* 松开 */
            GPFDAT |= (1<<6);
        }
        else
        {
            /* 按下 */
            GPFDAT &= ~(1<<6);
        }
    }else if(irqIrqNo == 2)//EINT2
    {
        if (GPFDAT & (1<<2)) /* s3 --> gpf5 */
        {
            /* 松开 */
            GPFDAT |= (1<<5);
        }
        else
        {
            /* 按下 */
            GPFDAT &= ~(1<<5);
        }
    }
    
    else if( irqIrqNo ==5 && (EINTPEND & (1<<11)) ) //EINT11
    {
        
		if (GPGDAT & (1<<3)) /* s4 --> gpf4 */
		{
			/* 松开 */
			GPFDAT |= (1<<4);
		}
		else
		{
			/* 按下 */
			GPFDAT &= ~(1<<4);
		}
    }
    else if( irqIrqNo ==5 && (EINTPEND & (1<<19)) ) //EINT19
    {
			if (GPGDAT & (1<<11))
			{
				/* 松开 */
				/* 熄灭所有LED */
				GPFDAT |= ((1<<4) | (1<<5) | (1<<6));
			}
			else
			{
				/* 按下: 点亮所有LED */
				GPFDAT &= ~((1<<4) | (1<<5) | (1<<6));
			}
    }
//3.执行完后清理中断
    // 由中断源（SRCPND/EINTPEND）->中断控制器清理(INTMSK/INTPND)
    EINTPEND = srcIrqSatus; 
    SRCPND =(1<<irqIrqNo);//EINT0~3对应[0:3],EINT4~7对应[4] EINT8~23对应[5]
    INTPND =(1<<irqIrqNo);
}
