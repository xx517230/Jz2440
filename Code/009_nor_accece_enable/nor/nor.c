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
    ����NOR flash��Nand Flash����ͨ�����뿪��ѡ���ڼĴ����о���OM[0:1],00��Nand Flash������ʽ��00/01��Nor flash������ֻ�Ǹ���nor��λ��ѡ����16bit����32bit,11ֻ��������s3c2240�����׶�ʹ�á�
    OM1 (Operating Mode 1)  OM0 (Operating Mode 0)   Booting ROM Data width
        0                       0                           Nand Flash Mode
        0                       1                           16-bit
        1                       0                           32-bit
        1                       1                           Test Mode
    
    ��Nor Flash�ǽ���BANK0�ϣ������ǹ���BWSCON/BANKCON0�Ĵ�������������bank0�ļĴ���
    BANK1~BANK5��ROM��SRAM�࣬BANK6/BANK7һ������SDRAM����ȻҲ��������ROM��SRAM��
    һ��BANK6��BANK7����SDRAM����ôREFRESH/BANKSIZE/MRSRB6/MRSRB7����Ҫ���ã�����SDRAM������üĴ�����
    
    ֻҪ��BANK0~BANK5����Ҫ�ο�Figure 5-12. S3C2440A nGCS Timing Diagram������s3c2440����ROM/SRAM��ʱ��ͼ
**/

void bank0_nor_init()
{
    //BWSCONֻ��[2:1]������bank0,����ֻ�ܶ�����ȡbank0������λ��ģ�λ��ͨ��OM[0:1]��عܽſ��ƣ����뿪�أ������Բ��ùܸüĴ���
/* Symbol Description                                                              Min. Typ. Max.  Uni
    Taa     Valid data output after address                                                    70   ns

Ϊ�˼򵥣������õ�ַ��Ϣ/Ƭѡ�ź�/��д�źţ�һͬ����������Tacs/TcosΪ0��ʱ�ٷ�Taccȡ���ֵ��ͬʱ�����д���ڵ���ز����������ú�nor Flash����
�ȴ������ߵ����ʱ��ֵ�� �������Ƿ�����2240�ֲ��У�ֻ��Tacc��֪����70ns���ϣ���Ϊ����ʱ�����úú�HCLK=100MHz=10ns��ֻҪ��ʱ�����7�����ڲ�������nor flash��������
����������Ҫ���õ�Tcoh/Tcahʱ����Ϊ�˱����ʱд������������ڸ�����Ӱ�������ڴ���´ζ�д�������ǿ�ʼ���õ�ʱ��һ�����úú��ͷ����ݵ����ݸ�����ʱ������Ҳ�����ˣ�
����һ�㲻��Ҫ�ر����á�
*/
    BANKCON0 |= val<<8;
}


