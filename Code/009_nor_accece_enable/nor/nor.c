#include "../s3c2440_soc.h"
#include "nor.h"

/*
Memory Controllers
BWSCON      0x48000000  W   R/W         Bus width & wait status control
BANKCON0    0x48000004                  Boot ROM control
BANKCON1    0x48000008                  BANK1 control
BANKCON2    0x4800000C                  BANK2 control
BANKCON3    0x48000010                  BANK3 control
BANKCON4    0x48000014                  BANK4 control
BANKCON5    0x48000018                  BANK5 control
BANKCON6    0x4800001C                  BANK6 control
BANKCON7    0x48000020                  BANK7 control
REFRESH     0x48000024                  DRAM/SDRAM refresh control
BANKSIZE    0x48000028                  Flexible bank size
MRSRB6      0x4800002C                  Mode register set for SDRAM BANK6
MRSRB7      0x48000030                  Mode register set for SDRAM BANK7
*/

/*
    由于NOR flash和Nand Flash启动通过拨码开关选择，在寄存器中就是OM[0:1],00是Nand Flash启动方式，00/01是Nor flash启动，只是根据nor的位宽选择是16bit还是32bit,11只有在生产s3c2240工厂阶段使用。
    OM1 (Operating Mode 1)  OM0 (Operating Mode 0)   Booting ROM Data width
        0                       0                           Nand Flash Mode
        0                       1                           16-bit
        1                       0                           32-bit
        1                       1                           Test Mode
    
    而Nor Flash是接在BANK0上，即我们关心BWSCON/BANKCON0寄存器，这是设置bank0的寄存器
    BANK1~BANK5是ROM或SRAM类，BANK6/BANK7一般用于SDRAM，当然也可以用于ROM或SRAM。
    一旦BANK6或BANK7接了SDRAM，那么REFRESH/BANKSIZE/MRSRB6/MRSRB7才需要设置，这是SDRAM相关设置寄存器。
    
    只要是BANK0~BANK5，需要参考Figure 5-12. S3C2440A nGCS Timing Diagram，这是s3c2440访问ROM/SRAM的时序图
**/

void bank0_nor_init()
{
    //BWSCON只有[2:1]是属于bank0,但是只能读，获取bank0的总线位宽的，位宽通过OM[0:1]相关管脚控制（拨码开关），所以不用管该寄存器
/* Symbol Description                                                              Min. Typ. Max.  Uni
    Taa     Valid data output after address                                                    70   ns

为了简单，我们让地址信息/片选信号/读写信号，一同发出（即让Tacs/Tcos为0此时再发Tacc取最大值，同时满足读写周期的相关参数即可设置好nor Flash），
等待这三者的最大时间值， 最终我们发现在2240手册中，只有Tacc能知道到70ns以上，因为我们时钟设置好后，HCLK=100MHz=10ns，只要等时间大于7个周期才能满足nor flash的条件。
而结束后需要设置的Tcoh/Tcah时间是为了避免此时写或读的数据由于浮动而影响其他内存或下次读写，而我们开始设置的时间一般设置好后，释放数据的数据浮动的时间问题也满足了，
所以一般不需要特别设置。
*/
    BANKCON0 |= val<<8;
}


