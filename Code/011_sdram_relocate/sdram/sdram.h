
#ifndef _SDRAM_H
#define _SDRAM_H

#include "../s3c2440_soc.h"

void sdram_init(void);
int sdram_test(void);
void copy2sdram(void);
void clean_bss(void);
#endif




