//���벻�࣬��������Ҫ����֪��Ϊʲô��ôд����Ҫ���Ǳ����֪ʶ�����Ǵ��룬����ͳ������Զ���ʵ��·���Ĺ��ߡ�
.text
.global _start

_start:

	//�رչ��Ź�
	ldr r0,=0x53000000
	ldr r1,=0
	str r1,[r0]
	
	//����ʱ��
	/**
	 *ʱ����Դ�Ǿ��񣬾��񾭹�MPLL(PLL���໷,M=main)������FCLKʱ�ӣ�FCLKʱ�ӷ�Ƶ����PCLK��HCLK��
	 *MPLL��Ҫ����PMS,P M S�����Ƿ�Ƶϵ�������ݾ�������FCLKƵ�ʸ����ֲ����͹�ʽ��������,һ�����ú�PMS,��ȴ�locktime(һ��ʱ��ʱ�Ӿ��Զ��������)
	 *��FCLK/PCLK/HCLK�������ã�һ��HCLK����Ϊ1����Ҫ����CPUΪ�첽ģʽ
	 */
		//ʱ����ؼĴ���
		//LOCKTIME 0x4C000000  W R/W PLL lock time counter
		//MPLLCON 0x4C000004 MPLL control
		//UPLLCON 0x4C000008 UPLL control
		//CLKCON 0x4C00000C Clock generator control
		//CLKSLOW 0x4C000010 Slow clock control
		//CLKDIVN 0x4C000014 Clock divider control
		//CAMDIVN 0x4C000018 Camera clock divider contro

		//��δʹ�üĴ���
		//UPLLCON��UPLL��صģ��Ǹ�USBʹ�õġ������ݲ�ʹ�ã�����
		//CLKCON�ǿ��Ƹ���ģ���Ƿ����ʱ�ӣ�Ĭ���ǽӵģ�Ҳ���Բ��ù�
		//CLKSLOW�ǵ�Դ����ʱ�õ������ڲ���
		//CAMDIVN�ʺ�����ͷʱ����أ�ֻ�ǵ�HCLK��Ƶϵ��HDIV������ʱ�ֲ���ʾ��Ҫ����CAMDIVN���λ����CAMDIVNĬ��ʱ����Ҫ��ģ����Բ��ùܡ�

		//�����üĴ���LOCKTIME/MPLLCON/CLKDIVN��
		//LOCKTIME/MPLLCON��MPLL��أ�LOCKTIME����MPLL��lockʱ��,MPLLCON����PMS������Ƶϵ����
		//CLKDIVN������FCLK/HCLK/PCLK��Ƶϵ����


	//��Ϊһ�����ú�MPLL��PMS,PLL֮��ͻ�lock��ֱ��ʱ�䵽������ʱ�Ӿ������������FCLK,��������������FCLK/HCLK/PCLK��Ƶϵ����
	//��������FCLK=400M��FCLK:HCLK:PCLK=1:4:8,HCLK=100M,PCLK=50M��Ϊʲô��ô���ã����ֲ�����FCLK:HCLK:PCLK���������������ߵ�Ӳ���豸���������ǵ�ʱ��Ƶ�ʵ�ѡ��
	ldr r0,=0x4C000014  //CLKDIVN�Ĵ�����ַ
	ldr r1,=(2<<1)|(1<<0) // 10 : HCLK = FCLK/4 when CAMDIVN[9] = 0.
	                      // 1: PCLK has the clock same as the HCLK/2.
	str r1,[r0]
	//��Ϊ�ֲ����FCLK��HCLK��PCLK�����и���Note,��HDIVN=1��������CPUΪ�첽ģʽ�������ֲ��Ѹ���
	mrc p15,0,r0,c1,c0,0
	orr r0,r0,#0xc0000000  //#R1_nF:OR:R1_iA  �����õ�
	mcr p15,0,r0,c1,c0,0
	

	//����MPLL��locktime
	//��PLL��Чǰ��CPU��ʱ��Դ�Ǿ���jz2440����12M���ֲ���ʾlocktime��С300us, 300us/1/12M Լ= 361 ��Ӧ0x169,����ȡ��㣬����Ϊ0x200
	ldr r0,=0x4C000000   //LOCKTIME�Ĵ�����ַ
	ldr r1, =0xFFFF0200  //��16λ��UPLL������ʹ��Ĭ��ֵFFFF,0200��MPLL��
	str r1,[r0]
    //���ú�locktime������MPLL��PMS������Ƶϵ��
    //�ֲ�����֪��12M��������MPLL���Ϊ400M,��MDIV=92��PDIV=1��SDIV=1
    //��PMS��ֵ��ͨ����ʽ�õ�����ʽ�����ֲ᣺Mpll=(2*M*Fin)/(P*2^S) FinΪʱ��Դ��������Ƶ��         
    //M=MDIV+8,P=PDIV+2,S=SDIV����M=100, P=3, S=1��ͨ�����湫˾���ƣ����Եõ�Mpll=400M
	ldr r0,=0x4C000004    //MPLLCON�Ĵ�����ַ
	ldr r1,=(92<<12)|(1<<4)|(1<<0)
	str r1,[r0]

	/*һ�����ú�MPLL��PMS,PLL֮����locktimeʱ���ڵ���Ƶ�ʣ�ʱ�䵽��MPLL���400M��֮�����CLKDIVN����HCLK/PCLKƵ�ʣ�
	 *һ��Ϊ1.5��HCLK��HCLK/PCLK������(ֻ��FCLK:HCLK:PCLK����Ϊ��������ʱ��1��HCLK)
	 */


	//�ж�Flash������ʽ,֮�����ö�Ӧջ
	/**
	 * ����jz2400��Nor Flash��Nand Flash�����ж�
	 * Nor Flahsֻ�ܶ�����д��Nand Flash����ʱ�Ὣ��ǰ4k�����Ƶ�SRAM������Nand Flash����ʱ��д�ܶ�
	 * ����Nor Flash��Nand Flash����,���ǵ���������ַ����0���������ǿ����ڵ�ַ0��д0�ٶ�0��
	 */
  	mov r0, #0
  	ldr r1, [r0]  //����0��ַ��ֵ��r1
  	str r0, [r0]  //д0->[0]
  	ldr r2, [r0]  //r2��ȡ[0]
	cmp r0, r2
	ldr sp,=0x40000000+4096 //����ʱnor������nor����ʱSRAM�ڵ�ַ0x40000000��������SRAMֻ��4K��ջһ�������ڴ�ߴ����׳�ջ��
	moveq sp,#4096	//���ܶ�д,�����˵��Nand������Nand����ʱSRAM�ڵ�ַ0�����������ʹ���Ͼ�sp����
	streq r1,[r0]   //�ָ�[0]��ַ������
	bl main

halt:

	b halt






	