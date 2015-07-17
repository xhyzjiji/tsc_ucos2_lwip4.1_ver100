#include "stm32f10x.h"

void delay(uint16_t time)
{
	for(; time>0; time--);
}

void RCC_Config(void)
{
	ErrorStatus HSEStartUpStatus;
	
	RCC_DeInit();  //��ֹRCC�Ĵ�������
	RCC_HSEConfig(RCC_HSE_ON);	 //ʹ���ⲿ���پ���
	HSEStartUpStatus == RCC_WaitForHSEStartUp();  //�ȴ��ⲿ����HSE�ȶ���
	//��ʹ�þ�������ж�
	if(HSEStartUpStatus == SUCCESS){
		//PLLʱ�ӹ���USBCLK(���ٷ�Ƶ)
		RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);  //HSEΪ8MHz��9��Ƶ��Ϊ72MHz��Ҳ��STM32�ܽ��ܵ������Ƶ��	
		RCC_PLLCmd(ENABLE);	 //����PLL
		//ϵͳʱ�ӹ���I2S2��I2S3
		RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);  //ѡ��PLL�����Ϊϵͳʱ�ӣ�
		//AHBʱ�ӹ���SDIO��FSMC��AHB���ߡ�оƬ�ںˡ�DMA��(8��Ƶ��)ϵͳ����ʱ�ӡ�FCLK��(2��Ƶ��)SDIO��AHB�ӿ�
		RCC_HCLKConfig(RCC_SYSCLK_Div1);  //AHBʱ��ȡ����ϵͳʱ�ӵķ�Ƶ�����72MHz
		//�����Ǹ���ͬ��������ʱ������
		//APB1ʱ�ӹ���DAC��PWR��BKP��bxCAN��USB��I2C1��I2C2��UART5��UART4��USART3��USART2��
		//SPI1��SPI2��IWDG��WWDG��RTC��TIM7~TIM2(���ٷ�Ƶ)�������֧��36MHz����Ƶ��
		RCC_PCLK1Config(RCC_HCLK_Div2);	 //APB1ʱ��ȡ����AHBʱ�ӷ�Ƶ�����36MHz
		//APB2ʱ�ӹ���ADC1~3(���ٷ�Ƶ)��USART1��SPI1��TIM1(���ٷ�Ƶ)��TIM8(���ٷ�Ƶ)��GPIOA~F��EXTI��AFIO�������֧��72MHz����Ƶ��
		RCC_PCLK2Config(RCC_HCLK_Div1);  //APB2ʱ��ȡ����AHBʱ�ӷ�Ƶ�����72MHz
	}
	else{
		/*
		//ѡ���ڲ�ʱ����Ϊϵͳʱ��8MHz
		RCC_AdjustHSICalibrationValue(***);	 //�ڲ�ʱ��У׼
		RCC_HSICmd(ENABLE);
		RCC_SYSCLKConfig(RCC_SYSCLKSource_HSI);
		RCC_HCLKConfig(RCC_SYSCLK_Div1);
		RCC_PCLK1Config(RCC_HCLK_Div1);
		RCC_PCLK2Config(RCC_HCLK_Div1);
		*/
	}
	//ʹ������ʱ��
	RCC_APB2PeriphClockCmd((RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE | RCC_APB2Periph_USART1 | RCC_APB2Periph_SPI1 | RCC_APB2Periph_AFIO), ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
	//RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2| RCC_APB1Periph_TIM6, ENABLE);
	//RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
}

void NVIC_Config(void)
{
	//�ж�ͨ�����ȼ����ȷ������ȼ�(ռ���ȼ��������ȼ�)
	//�������ȼ����÷��������֣�
	//1.ͨ��NVIC_Init(NVIC_InitTypeDef* NVIC_InitStruct)��������
	//2.��ʹ��NVIC_EncodePriority(uint32_t PriorityGroup, uint32_t PreemptPriority, uint32_t SubPriority)�������ȼ����뺯�������ȼ��顢��ռ���ȼ��������ȼ���һ��uint32_t�������ݷ���
	//  ��ʹ��NVIC_EncodePriority(...)�ķ���ֵ��������ʹ��NVIC_SetPriority(IRQn_Type IRQn, uint32_t priority)��������
	//  ���Ǳ���ɶ�ʹ�ã������ĺ�������NVIC_GetPriority(...)---->NVIC_DecodePriority(...)
	NVIC_InitTypeDef NVIC_InitStructure;
	
	//��STM32/Cortex-M3����ͨ���ı�CPU�ĵ�ǰ���ȼ���ʹ�ܻ��ֹ�ж�
	//�ر��ж��ܿ��أ������ַ�ʽ
	//1.��λPRIMASK����ֻ����NMI(���������ж�)��HARD FAULT(Ӳ������)�����ж�
	//2.��λFAULTMASK����ֻ����NMI�����ж�
	__set_PRIMASK(1);  //�����������жϺ�Ӳ����������жϣ��ر����������ж�
	//__set_FAULTMASK(1);	

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

	#ifdef VECT_TAB_RAM
		NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0);
	#else  //VEC_TAB_FLASH
		NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);
	#endif

	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;				 //�ⲿ�ж�2
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	     //��ռ���ȼ� 0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;			 //�����ȼ�0  
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				 //ʹ��
	NVIC_Init(&NVIC_InitStructure);	 

	SysTick_Config(72000);	//����ϵͳ����ʱ����������������SysTick_IRQn�жϿ���  //ʱ�ӽ����ж�ʱ1msһ��  ���ڶ�ʱ
//	NVIC_SetPriority (SysTick_IRQn, 1);	  //�趨ϵͳ����ʱ���ж����ȼ�
	//�ж��ܿ��ص�ʵ���ǵ���CPU�������ȼ����������ж����ȼ����Դﵽ���ε�Ч������ʵ�����԰����жϺŽ�������
	//__set_BASEPRI(0x60);  //�����жϺŸ���0x60�������ж�����

	__set_PRIMASK(0);  //�����ж��ܿ���
	//__set_FAULTMASK(1);

	//һ����core_cm3.h���ú�������һ����
	//__get_PSP();  //��ȡ���̶�ջָ��PSP������uint32_t����
	//__set_PSP(uint32_t topOfProcStack);  //����PSP

	//__get_MSP();  //��ȡ����ջָ��MSP������uint32_t����
	//__set_MSP(uint32_t mainStackPointer);  //����MSP

	//__REV(uint32_t value);  //����������ֵ��ת�ֽ�˳�򣬷���uint32_t����
	//__REV16(uint16_t value);  //��ת�������ֽ�˳����0xABCD��ת��õ�0xCDAB������uint32_t����
	//__REVSH(int16_t value);  //��ת�ֽ�˳�򣬲���������չ��������__REV16�����õ��Ľ�����ٽ���һ�η�����չ��������������Ҫ�Ƿ�����д�С�˵��л�������int32_t����
	//__RBIT(uint32_t value);  //��תλ˳�򣬷���uint32_t����

	//__LDREXB(uint8_t *addr);  //��ȡaddr��ַ�мĴ�������ֵ������uint8_t���ͣ�8λ����ר��ȡֵ����
	//__LDREXH(uint16_t *addr);  //��ȡaddr��ַ�мĴ�������ֵ������uint16_t���ͣ�16λ����ר��ȡֵ����
	//__LDREXW(uint32_t *addr);  //��ȡaddr��ַ�мĴ�������ֵ������uint32_t���ͣ�32λ����ר��ȡֵ����

	//__STREXB(uint8_t value, uint8_t *addr);  //��addr��ַ�мĴ���д��valueֵ��8λ����ר�ô洢����
	//__STREXH(uint16_t value, uint16_t *addr);  //��addr��ַ�мĴ���д��valueֵ��16λ����ר�ô洢����
	//__STREXW(uint32_t value, uint32_t *addr);  //��addr��ַ�мĴ���д��valueֵ��32λ����ר�ô洢����
	
	//__CLREX();  //�����LDREXָ����ɵĻ�������LDREX��STREX��Cortex����ʵ�ֻ�����ʣ������ٽ���Դ��ָ�LDREXִ�к�ֻ�����������һ���洢ָ�STR��STREX������ִ�У������Ĵ洢ָ��ᱻ���أ���CLREX������������������״̬�ı��
	//__get_BASEPRI();  //��ȡBASEPRI�Ĵ�����ֵ�����ȼ��Ÿ��ڸüĴ������ж϶��ᱻ���Σ����ȼ���Խ�����ȼ�Խ�ͣ���Ϊ��ʱ�������κ��жϣ�����uint32_t����
	//__set_BASEPRI(uint32_t basePri);  //����BASEPRI��ֵ
	//__get_PRIMASK();  //PRIMASK��һ��ֻ��һλ�ļĴ�������λʱ���ξ��󲿷ֵ��쳣�жϣ�ֻʣ��NMI��HardFault������Ӧ������uint32_t����
	//__set_PRIMASK(uint32_t priMask);  //����PRIMASK��ֵ
	//__get_FAULTMASK();  //FAULTMASKҲ��һ��ֻ��һλ�ļĴ�����Ϊ1ʱֻ��NMI������Ӧ�������쳣���ж�ȫ�������Σ�����uint32_t����
	//__set_FAULTMASK(uint32_t faultMask);  //����FAULTMASK��ֵ
	//__get_CONTROL();  //��ȡCONTROL��ֵ���Ĵ���CONTROLֻ����λ��CONTROL[0]ѡ����Ȩ����0Ϊ��Ȩ����1Ϊ���û�����CONTROL[1]����ѡ���ջָ�룬0ΪMSP��1ΪPSP������uint32_t����
	//__set_CONTROL(uint32_t control);  //����CONTROL�Ĵ�����ֵ
	//----------------------------------------------------------------//
	//BASEPRI��PRIMASK��FAULTMASK��CONTROL��ֻ������Ȩģʽ�±��޸�//
	//----------------------------------------------------------------//
	/*
	static __INLINE void __enable_irq()               { __ASM volatile ("cpsie i"); }
	static __INLINE void __disable_irq()              { __ASM volatile ("cpsid i"); }
	static __INLINE void __enable_fault_irq()         { __ASM volatile ("cpsie f"); }
	static __INLINE void __disable_fault_irq()        { __ASM volatile ("cpsid f"); }
	static __INLINE void __NOP()                      { __ASM volatile ("nop"); }
	static __INLINE void __WFI()                      { __ASM volatile ("wfi"); }
	static __INLINE void __WFE()                      { __ASM volatile ("wfe"); }
	static __INLINE void __SEV()                      { __ASM volatile ("sev"); }
	static __INLINE void __ISB()                      { __ASM volatile ("isb"); }
	static __INLINE void __DSB()                      { __ASM volatile ("dsb"); }
	static __INLINE void __DMB()                      { __ASM volatile ("dmb"); }
	static __INLINE void __CLREX()                    { __ASM volatile ("clrex"); }
	*/

	//��NVIC_PriorityGroupConfig(uint32_t NVIC_PriorityGroup)�������ƣ�����Ҫע�����������NVIC_PriorityGroupConfig(PriorityGroup_0)���ӦNVIC_SetPriorityGrouping(1)��NVIC_PriorityGroupConfig(PriorityGroup_1)���ӦNVIC_SetPriorityGrouping(2)�������
	//NVIC_SetPriorityGrouping(uint32_t PriorityGroup);  //�������ȼ�����
	//NVIC_EnableIRQ(IRQn_Type IRQn);  //����ĳ�ж�ͨ������
	//NVIC_DisableIRQ(IRQn_Type IRQn);  //�ر�ĳ�ж�ͨ������
	//NVIC_GetPendingIRQ(IRQn_Type IRQn);  //���ĳ�ж�ͨ���Ƿ����
	//NVIC_SetPendingIRQ(IRQn_Type IRQn);  /ǿ��ĳ�ж�ͨ������(��λ�жϹ���Ĵ�����Ӧλ)
	//NVIC_ClearPendingIRQ(IRQn_Type IRQn);  //���ĳ�ж�ͨ������״̬(��λ�жϹ�������Ĵ�����Ӧλ)�������Ѿ�������ж�û��Ӱ�죬���Ǹ��жϴ��ڹ���״̬
	//NVIC_GetActive(IRQn_Type IRQn);  //���ĳ�ж�ͨ���Ƿ񱻴��ڼ������л��߱���ռ(����)����ѹջ
	//NVIC_SetPriority(IRQn_Type IRQn, uint32_t priority);  //����ĳ�ж�ͨ���������ȼ����ⲿ�ж����ȼ�����Ϊ�������ڲ�ϵͳ(�ں�)�ж����ȼ�����Ϊ����
	//NVIC_GetPriority(IRQn_Type IRQn);  //����ĳ�ж�ͨ���ķ������ȼ�
	//NVIC_EncodePriority(uint32_t PriorityGroup, uint32_t PreemptPriority, uint32_t SubPriority);  //�ú����ķ���ֵ������NVIC_SetPriority(IRQn_Type IRQn, uint32_t priority)�еĲ���priority
	//NVIC_DecodePriority(uint32_t Priority, uint32_t PriorityGroup, uint32_t* pPreemptPriority, uint32_t* pSubPriority);  //��NVIC_GetPriority(...)�з��ص�ѹ�����ȼ����н�������ȼ��顢��ռ���ȼ��������ȼ�
	//SysTick_Config(uint32_t ticks);  //����ϵͳ����ʱ����������������SysTick_IRQn�жϿ���
}

void GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;	         		 		//USART1 TX
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;    		 		//�����������
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;	         	 		//USART1 RX
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;   	 		//���ÿ�©����
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;				     //enc_RST
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	//GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;	  //sck-->PA5  SI-->PA7  SO-->PA6
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;			  //���ù���
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;				  //enc_ncs
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    //GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
    
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;					//enc_nint
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;   	 		//�ڲ���������
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource0);
}

void EXTI_Config(void)
{
	EXTI_InitTypeDef EXTI_InitStructure;

	EXTI_InitStructure.EXTI_Line = EXTI_Line0;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;	 //�������ش���EXTI_Trigger_Rising������
	EXTI_InitStructure.EXTI_LineCmd	= ENABLE;
	EXTI_Init(&EXTI_InitStructure);
}

void USART1_Config(void)
{
	USART_InitTypeDef USART_InitStructure;

	USART_InitStructure.USART_BaudRate = 115200;						//����115200bps
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;		//����λ8λ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;			//ֹͣλ1λ
	USART_InitStructure.USART_Parity = USART_Parity_No;				//��У��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;   //��Ӳ������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure);

	USART_Cmd(USART1, ENABLE);
}

void SPI1_Config(void)
{
	SPI_InitTypeDef SPI_InitStructure;
	
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init(SPI1, &SPI_InitStructure);

	SPI_Cmd(SPI1, ENABLE);
}

void Project_Config(void)
{
	RCC_Config();
	NVIC_Config();
	GPIO_Config();
	EXTI_Config();
	USART1_Config();
	SPI1_Config();
}

void GPIOs_Init(void)
{
	GPIO_ResetBits(GPIOB, GPIO_Pin_5);
	delay(1000);
	GPIO_SetBits(GPIOB, GPIO_Pin_5);
}
