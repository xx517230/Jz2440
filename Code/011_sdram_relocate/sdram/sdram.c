#include "../s3c2440_soc.h"
#include "sdram.h"

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
	由于SDRAM只能接在S3C2440的bank6/bank7，所以我们只关注 BWSCON,BANKCON6/BANKCON7,和SDRAM相关的REFRESH/BANKSIZE/MRSRB6/MRSRB7
**/

void sdram_init()
{
	BWSCON = 0x22000000;
	
	BANKCON6 =0x00018001;
	BANKCON7 =0x00018001;
	
	REFRESH = 0x008004f5;
	BANKSIZE = 0x000000b1;

	MRSRB6 = 0x00000020;
	MRSRB7 = 0x00000020;

}


int sdram_test(void)
{
	volatile unsigned char *p = (volatile unsigned char *)0x30000000;
	int i;

	// write sdram
	for (i = 0; i < 1000; i++)
		p[i] = 0x55;

	// read sdram
	for (i = 0; i < 1000; i++)
		if (p[i] != 0x55)
			return -1;

	return 0;
}


void copy2sdram(void)
{	
	extern int __code_start,__bss_start;
	
	volatile unsigned int * code_start = (volatile unsigned int *)&__code_start;
	volatile unsigned int * bss_start = (volatile unsigned int *)&__bss_start;
	volatile unsigned int * src = (volatile unsigned int *)0;
	
	while(code_start<bss_start)
	{
		*code_start++ = *src++;
	}


}

void clean_bss(void)
{
	extern int __bss_start,_end;

	volatile unsigned int * bss_start = (volatile unsigned int *)&__bss_start;
	volatile unsigned int * bss_end = (volatile unsigned int *)&_end;
	while(bss_start<bss_end)
	{
		*bss_start++ = 0;
	}

}




