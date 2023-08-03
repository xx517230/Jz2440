//代码不多，但是最重要的是知道为什么这么写。重要的是背后的知识，而非代码，代码和程序语言都是实现路径的工具。
.text
.global _start

_start:

	//关闭关门狗
	ldr r0,=0x53000000
	ldr r1,=0
	str r1,[r0]
	
	//设置时钟
	/**
	 *时钟来源是晶振，晶振经过MPLL(PLL锁相环,M=main)，产生FCLK时钟，FCLK时钟分频产生PCLK和HCLK。
	 *MPLL需要设置PMS,P M S三个是分频系数，根据晶振和输出FCLK频率根据手册查表和公式计算所得,一旦设置好PMS,需等待locktime(一段时间时钟就自动设置完成)
	 *而FCLK/PCLK/HCLK按需配置，一旦HCLK设置为1，需要设置CPU为异步模式
	 */
		//时钟相关寄存器
		//LOCKTIME 0x4C000000  W R/W PLL lock time counter
		//MPLLCON 0x4C000004 MPLL control
		//UPLLCON 0x4C000008 UPLL control
		//CLKCON 0x4C00000C Clock generator control
		//CLKSLOW 0x4C000010 Slow clock control
		//CLKDIVN 0x4C000014 Clock divider control
		//CAMDIVN 0x4C000018 Camera clock divider contro

		//暂未使用寄存器
		//UPLLCON是UPLL相关的，是给USB使用的。我们暂不使用，不管
		//CLKCON是控制各个模块是否接上时钟，默认是接的，也可以不用管
		//CLKSLOW是电源管理时用到，现在不管
		//CAMDIVN适合摄像头时钟相关，只是当HCLK分频系数HDIV在设置时手册提示需要设置CAMDIVN相关位，而CAMDIVN默认时符合要求的，可以不用管。

		//需设置寄存器LOCKTIME/MPLLCON/CLKDIVN。
		//LOCKTIME/MPLLCON是MPLL相关，LOCKTIME设置MPLL的lock时间,MPLLCON设置PMS三个分频系数；
		//CLKDIVN是配置FCLK/HCLK/PCLK分频系数的


	//因为一旦配置好MPLL的PMS,PLL之后就会lock，直到时间到达后输出时钟就是我们所需的FCLK,所以我们先设置FCLK/HCLK/PCLK分频系数的
	//我们设置FCLK=400M，FCLK:HCLK:PCLK=1:4:8,HCLK=100M,PCLK=50M。为什么这么设置，见手册中有FCLK:HCLK:PCLK比例表。具体总线的硬件设备决定了我们的时钟频率的选择。
	ldr r0,=0x4C000014  //CLKDIVN寄存器地址
	ldr r1,=(2<<1)|(1<<0) // 10 : HCLK = FCLK/4 when CAMDIVN[9] = 0.
	                      // 1: PCLK has the clock same as the HCLK/2.
	str r1,[r0]
	//因为手册介绍FCLK、HCLK和PCLK部分有个后Note,当HDIVN=1，需设置CPU为异步模式，代码手册已给出
	mrc p15,0,r0,c1,c0,0
	orr r0,r0,#0xc0000000  //#R1_nF:OR:R1_iA  搜索得到
	mcr p15,0,r0,c1,c0,0
	

	//配置MPLL的locktime
	//在PLL生效前，CPU的时钟源是晶振，jz2440晶振12M，手册提示locktime最小300us, 300us/1/12M 约= 361 对应0x169,我们取大点，设置为0x200
	ldr r0,=0x4C000000   //LOCKTIME寄存器地址
	ldr r1, =0xFFFF0200  //高16位是UPLL的我们使用默认值FFFF,0200是MPLL的
	str r1,[r0]
    //配置好locktime，配置MPLL的PMS三个分频系数
    //手册查表得知，12M晶振设置MPLL输出为400M,需MDIV=92，PDIV=1，SDIV=1
    //而PMS的值是通过公式得到，公式来自手册：Mpll=(2*M*Fin)/(P*2^S) Fin为时钟源，即晶振频率         
    //M=MDIV+8,P=PDIV+2,S=SDIV，故M=100, P=3, S=1。通过上面公式反推，可以得到Mpll=400M
	ldr r0,=0x4C000004    //MPLLCON寄存器地址
	ldr r1,=(92<<12)|(1<<4)|(1<<0)
	str r1,[r0]

	/*一旦设置好MPLL的PMS,PLL之后在locktime时间内调整频率，时间到后，MPLL输出400M，之后根据CLKDIVN调整HCLK/PCLK频率，
	 *一般为1.5个HCLK后HCLK/PCLK调整完(只有FCLK:HCLK:PCLK调整为其他比例时是1个HCLK)
	 */


	//判断Flash启动方式,之后设置对应栈
	/**
	 * 根据jz2400对Nor Flash和Nand Flash特性判断
	 * Nor Flahs只能读不能写，Nand Flash启动时会将把前4k程序复制到SRAM，所以Nand Flash启动时能写能读
	 * 不论Nor Flash和Nand Flash启动,它们的物理基地址都是0，所以我们可以在地址0处写0再读0。
	 */
  	mov r0, #0
  	ldr r1, [r0]  //备份0地址数值到r1
  	str r0, [r0]  //写0->[0]
  	ldr r2, [r0]  //r2读取[0]
	cmp r0, r2
	ldr sp,=0x40000000+4096 //假设时nor启动；nor启动时SRAM在地址0x40000000处，由于SRAM只有4K，栈一般设在内存高处，俗称栈顶
	moveq sp,#4096	//若能读写,则相等说明Nand启动，Nand启动时SRAM在地址0处。不相等则使用上句sp设置
	streq r1,[r0]   //恢复[0]地址的数据

	//初始化SDRAM
	bl sdram_init
	//将nor或SDRAM的代码重定位到SDRAM
	bl copy2sdram
	bl clean_bss

	//将pc指向重定位的main函数
	ldr pc,=main

	
	bl main

halt:

	b halt






	
