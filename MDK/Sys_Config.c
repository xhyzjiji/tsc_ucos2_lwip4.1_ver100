#include "stm32f10x.h"
#include "stdio.h"
#include "RA8806_CTRL.h"
#include "led_op.h"
#include "ds1302_op.h"
#include "at24c64_op.h"

/*****************************************************
  * @file    Sys_Config.c
  * @author  xhyzjiji
  * @version V1.0.0
  * @date    7/28/2012
  * @brief   ��ʼ��STM32�ڲ���Դ
*****************************************************/

/*
  * @func   RCC_Config
  * @brief  ����RCCѡ���ⲿ���پ���9������Ϊϵͳʱ��SYSCLK��
  *         ����ȫGPIO�˿ڣ�TIM1/2��USART1��AFIO��PWR��BKP
  * @param  None
  * @retval None
*/
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
	RCC_APB2PeriphClockCmd((RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE | RCC_APB2Periph_USART1 | RCC_APB2Periph_SPI1), ENABLE); // RCC_APB2Periph_AFIO 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2| RCC_APB1Periph_TIM6 | RCC_APB1Periph_I2C1 | RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
	//RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
	//RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
}

/*
  * @func   GPIO_Config
  * @brief  ����GPIOE�˿ڣ�Pin0-15 /50M��ת�ٶ�/���������
  *         ����GPIOA�˿ڣ�Pin9    /50M��ת�ٶ�/�����������
  *      	����GPIOA�˿ڣ�Pin10   /50M��ת�ٶ�/��������
  *         ����GPIOD�˿ڣ�Pin10-15/10M��ת�ٶ�/��������
  * 		����GPIOD�˿ڣ�Pin10-15Ϊ�ж�������·
  * @param  None
  * @retval None
*/
void GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	//LCD_Interface
	//GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All ^ (GPIO_Pin_8 | GPIO_Pin_9);
	GPIO_InitStructure.GPIO_Pin = DB | RS | CS | WR | RD | RST | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	//GPIO_Init(GPIOE, &GPIO_InitStructure);
	GPIO_Init(LCDPort, &GPIO_InitStructure);

	//GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;	//LCD_BUSY/LCD_INT
	GPIO_InitStructure.GPIO_Pin = BUSY | INT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	//GPIO_Init(GPIOE, &GPIO_InitStructure);
	GPIO_Init(LCDPort, &GPIO_InitStructure);

	//LED_IO Interface
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(LED_Port, &GPIO_InitStructure);

	//Expend_IO for 74HC573_LE Interface
	GPIO_InitStructure.GPIO_Pin = LE1 | LE2 | LE3 | LE4 | LE5 | LE6;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(LE_Port, &GPIO_InitStructure);

	//USART1_Interface
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	//KEY_Interface
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  //GPIO_Mode_IPU;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	//I2C_Interface
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7; //PB6--SCL  PB7--SDA
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	#ifndef i2c_simulation
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
	#else
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	#endif
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	//SPI_Interface
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

	//RTC_Interface
	GPIO_InitStructure.GPIO_Pin = DS1302_sck | DS1302_IO | DS1302_nrst;				     //enc_RST
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(DS1302_port, &GPIO_InitStructure);
    
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;					//enc_nint
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//GPIO_Mode_IPU;   	 		//�ڲ���������
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource0);
/*
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOD, GPIO_PinSource10);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOD, GPIO_PinSource11);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOD, GPIO_PinSource12);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOD, GPIO_PinSource13);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOD, GPIO_PinSource14);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOD, GPIO_PinSource15);
*/
	//TIME_OC_Interface
	/*
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	*/
	/*
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	*/
}

void GPIO_Initial(void)
{
	GPIO_SetBits(LCDPort, CS);
	delay_100ns(1);
	GPIO_SetBits(LCDPort, GPIO_Pin_All);

	GPIO_ResetBits(LE_Port, LE1 | LE2 | LE3 | LE4 | LE5 | LE6);
	GPIO_SetBits(LED_Port, GPIO_Pin_All);

	GPIO_ResetBits(GPIOB, GPIO_Pin_5);
	delay(1000);
	GPIO_SetBits(GPIOB, GPIO_Pin_5);

	reset_scl;
	delay_100ns(2);
	set_sda;
}

/*
  * @func   USART_Config
  * @brief  ����USART1����ģʽ��������115200/8λ����ģʽ/1λֹͣλ/����żУ��/��Ӳ��������
  * @param  None
  * @retval None
*/
void USART_Config(void)
{
	USART_InitTypeDef USART_InitStructure;

	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_Init(USART1, &USART_InitStructure);

	USART_Cmd(USART1, ENABLE);  //��ʱ����������ͨ���ж�
}

/*
  * @func   USART_Config
  * @brief  �������ȼ��飺			�����ȼ�0~3/�����ȼ�0~3
  *			�����ж�������ͷ��ַ��	VEC_TAB_FLASH-->0x08000000/VEC_TAB_RAM-->0x20000000/Ĭ�ϱ�ͷ��ַΪ0x08000000
  *			�����ⲿ�ж�10-15��		�����ȼ�2/�����ȼ�0
  *			����TIM2�жϣ�			�����ȼ�0/�����ȼ�0
  * @param  None
  * @retval None
*/
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
	//__set_PRIMASK(1);  //�����������жϺ�Ӳ����������жϣ��ر����������ж�
	//__set_FAULTMASK(1);	

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

	#ifdef VECT_TAB_RAM
		NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0);
	#else  //VEC_TAB_FLASH
		NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);	//������1�û�������ʼ��ַ���ж�����ƫ�Ƶ�ַ
	#endif

//	NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_Init(&NVIC_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;				 //�ⲿ�ж�2
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	     //��ռ���ȼ� 0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;			 //�����ȼ�0  
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				 //ʹ��
	NVIC_Init(&NVIC_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = TIM6_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	//SysTick_Config(72000);
	//�ж��ܿ��ص�ʵ���ǵ���CPU�������ȼ����������ж����ȼ����Դﵽ���ε�Ч������ʵ�����԰����жϺŽ�������
	//__set_BASEPRI(0x60);  //�����жϺŸ���0x60�������ж�����

	//__set_PRIMASK(0);  //�����ж��ܿ���
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

/*
  * @func   EXTI_Config
  * @brief  �����ⲿ�ж���·10-15��  �ж�ģʽ�����¼�ģʽ��/�����ش����ж�
  * @param  None
  * @retval None
*/

void EXTI_Config(void)
{
	EXTI_InitTypeDef EXTI_InitStructure;

	//EXTI_InitStructure.EXTI_Line = EXTI_Line0 | EXTI_Line10 | EXTI_Line11 | EXTI_Line12 | EXTI_Line13 | EXTI_Line14 | EXTI_Line15;
	EXTI_InitStructure.EXTI_Line = EXTI_Line1;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;	 //�������ش���EXTI_Trigger_Rising������
	EXTI_InitStructure.EXTI_LineCmd	= ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	EXTI_InitStructure.EXTI_Line = EXTI_Line0;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;	 //�������ش���EXTI_Trigger_Rising������
	EXTI_InitStructure.EXTI_LineCmd	= ENABLE;
	EXTI_Init(&EXTI_InitStructure);
}

/*
  * @func   CommonTIM_Config
  * @brief  ����ͨ�ö�ʱ��������10000/Ԥ��Ƶ719/�ڲ�ʱ��Դ
  *  		CCR1��			�Ƚ������תOCģʽ/�Ƚ�ֵ5000/��Ч����Ϊ�ߵ�ƽ
  *			CCR2��			��ֹOC���/�����Ƚ��ж�/�Ƚ�ֵ7500/��Ч����Ϊ�ߵ�ƽ
  * @param  TIM_TypeDef*
  * @retval None
*/
void TIM_Config(TIM_TypeDef* TIMx)
{
/*
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;

	TIM_InternalClockConfig(TIMx);

	TIM_TimeBaseInitStructure.TIM_Period = 10000;	//����Ϊ1s
	TIM_TimeBaseInitStructure.TIM_Prescaler = 720-1;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;  //�ڶ�ʱ��ʱ��(CK_INT)Ƶ���������˲���(ETR,TIx)ʹ�õĲ���Ƶ��֮��ķ�Ƶ����
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	//TIM2��ʼ������Ҫ����TIM_RepetitionCounter
	//TIM_TimeBaseInitStructure.TIM_RepetitionCounter = ***;
	TIM_TimeBaseInit(TIMx, &TIM_TimeBaseInitStructure);

	//TIM_OCModeΪ���ñȽ����ģʽ��ģʽ���£�TIM_OCMode�޸ĵ���CCMR�Ĵ����е�OCxM[2:0]��
	//TIM_OCMode_Timing(0x00)��  ����ȽϼĴ���TIMx_CCR1�������TIMx_CNT ��ıȽ϶�OC1REF��������
	//TIM_OCMode_Active(0x01):   ��������TIMx_CNT��ֵ�벶��/�ȽϼĴ���1(TIMx_CCR1)��ͬʱ��ǿ��OC1REFΪ��
	//TIM_OCMode_Inactive(0x02): ��������TIMx_CNT��ֵ�벶��/�ȽϼĴ���1(TIMx_CCR1)��ͬʱ��ǿ��OC1REFΪ��
	//TIM_OCMode_Toggle(0x03):   ��TIMx_CCR1=TIMx_CNT ʱ����תOC1REF�ĵ�ƽ
	//stm32f10x_tim.h��δ�����0x04:  ǿ��OC1REFΪ��
	//stm32f10x_tim.h��δ�����0x05:  ǿ��OC1REFΪ��
	//TIM_OCMode_PWM1(0x06):     PWMģʽ1��  �����ϼ���ʱ��һ��TIMx_CNT<TIMx_CCR1ʱͨ��1Ϊ��Ч��ƽ������Ϊ��Ч��ƽ�������¼���ʱ��һ��TIMx_CNT>TIMx_CCR1ʱͨ��1Ϊ��Ч��ƽ(OC1REF=0)������Ϊ��Ч��ƽ(OC1REF=1) 
	//TIM_OCMode_PWM2(0x07):     PWMģʽ2��  �����ϼ���ʱ��һ��TIMx_CNT<TIMx_CCR1ʱͨ��1Ϊ��Ч��ƽ������Ϊ��Ч��ƽ�������¼���ʱ��һ��TIMx_CNT>TIMx_CCR1ʱͨ��1Ϊ��Ч��ƽ������Ϊ��Ч��ƽ
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Toggle;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	 //OC���ʹ��
	TIM_OCInitStructure.TIM_Pulse = 5000;	//�޸�����ȽϼĴ�����ֵ
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	//����TIM_OutputNState��TIM_OCNPolarity��TIM_OCNIdleState\TIM_OCIdleState��Ϊ�߼���ʱ��ʹ��
	TIM_OC1Init(TIMx, &TIM_OCInitStructure);

	TIM_OCInitStructure.TIM_Pulse = 7500;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Disable;
	TIM_OC2Init(TIMx, &TIM_OCInitStructure);

	TIM_ITConfig(TIMx, TIM_IT_CC2, ENABLE);

	TIM_OC1PreloadConfig(TIMx, TIM_OCPreload_Enable);
	TIM_OC2PreloadConfig(TIMx, TIM_OCPreload_Enable);
	TIM_ARRPreloadConfig(TIMx, ENABLE);
*/
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;

	TIM_InternalClockConfig(TIMx);

	TIM_TimeBaseInitStructure.TIM_Period = 6000;	//����Ϊ1s
	TIM_TimeBaseInitStructure.TIM_Prescaler = 720-1;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;  //�ڶ�ʱ��ʱ��(CK_INT)Ƶ���������˲���(ETR,TIx)ʹ�õĲ���Ƶ��֮��ķ�Ƶ����
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIMx, &TIM_TimeBaseInitStructure);

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	 //OC���ʹ��
	TIM_OCInitStructure.TIM_Pulse = 3000;	//�޸�����ȽϼĴ�����ֵ
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
	//����TIM_OutputNState��TIM_OCNPolarity��TIM_OCNIdleState\TIM_OCIdleState��Ϊ�߼���ʱ��ʹ��
	TIM_OC2Init(TIMx, &TIM_OCInitStructure);

	//TIM_PrescalerConfig(TIMx, 720-1, TIM_PSCReloadMode_Update);
	TIM_ARRPreloadConfig(TIMx, ENABLE);
	//TIMx->CCER |= 0x0001;
	
	TIM_UpdateRequestConfig(TIMx, TIM_UpdateSource_Regular);  //ֻ�м�������������Ų����ж�
	TIM_SelectOnePulseMode(TIMx, TIM_OPMode_Single);
	//TIM_SelectOnePulseMode(TIMx, TIM_OPMode_Repetitive);

	TIM_ClearFlag(TIMx, TIM_FLAG_Update);
	TIM_ITConfig(TIMx, TIM_IT_Update, ENABLE);
}

void TIM_Basic_Config(TIM_TypeDef* TIMx)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;

	TIM_DeInit(TIMx);

	TIM_InternalClockConfig(TIMx);
	TIM_TimeBaseInitStructure.TIM_Period = 30000;	//����Ϊ1s
	TIM_TimeBaseInitStructure.TIM_Prescaler = 72-1;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;  //�ڶ�ʱ��ʱ��(CK_INT)Ƶ���������˲���(ETR,TIx)ʹ�õĲ���Ƶ��֮��ķ�Ƶ����
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIMx, &TIM_TimeBaseInitStructure);

	TIM_ARRPreloadConfig(TIMx, ENABLE);
	
	TIM_UpdateRequestConfig(TIMx, TIM_UpdateSource_Regular);  //ֻ�м�������������Ų����ж�
	TIM_SelectOnePulseMode(TIMx, TIM_OPMode_Repetitive);

	TIM_ClearFlag(TIMx, TIM_FLAG_Update);
	TIM_ITConfig(TIMx, TIM_IT_Update, ENABLE);
}

void RTC_Config(void)
{
	//RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
	//��RTC��ؼĴ������ж�дǰ��������λRTC_CRL�е�CNFλ���˳���дʱ�����CNFλ
	//�������ȡRTOFF����RTC�Ƿ����

	PWR_BackupAccessCmd(ENABLE);   //���޸ı��ݼĴ���
	BKP_DeInit();

	//���õ��پ���Դ
	RCC_LSEConfig(RCC_LSE_ON);
	while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET);  //�ȴ��ⲿ���پ�������
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
	RCC_RTCCLKCmd(ENABLE);

	RTC_WaitForSynchro();
	RTC_WaitForLastTask();
	Time_SetCalendarTime(Current_Time);  //���ڸ�Ϊ�ɴ�������������ⲿRTC��ȡ
	RTC_WaitForLastTask();
	RTC_ITConfig(RTC_IT_ALR | RTC_IT_SEC, ENABLE);
	RTC_WaitForLastTask();
	RTC_SetPrescaler(32767);	//ʹ���ⲿ����32.768kHzʱ��Ԥ��Ƶֵ
	RTC_WaitForLastTask();
}

void RTC_Init(void)
{
	if(BKP_ReadBackupRegister(BKP_DR1) != 0x5A5A){  //���ν���˺�������ⱸ�ݼĴ�����ֵ�Ƿ��޸�Ϊ0xA5A5��δ�޸����ʼ��RTC���޸�DR1Ϊ0xA5A5
		RTC_Config();
		//Time_SetCalendarTime(Init_Time);  //���ڸ�Ϊ�ɴ�������������ⲿRTC��ȡ
		BKP_WriteBackupRegister(BKP_DR1, 0x5A5A);
		//printf("RTC configured...\n");
	}
	//RTC_PRL��RTC_ALR��RTC_CNT��RTC_DIV�Ĵ�����Ӳ����λ�����ϵ縴λʱ���ڲ��洢��ֵ����Ӱ��
	//����Ҫ���¿���RTC�жϿ���
	else{
		RTC_WaitForSynchro();
		RTC_WaitForLastTask();
		RTC_ITConfig(RTC_IT_ALR | RTC_IT_SEC, ENABLE);
		RTC_WaitForLastTask();
	}
	RCC_ClearFlag();  //�����λ��־λ
	return;
}

void I2C1_Config(void)
{
	I2C_InitTypeDef I2C_InitStructure;

	I2C_DeInit(I2C1);

	I2C_InitStructure.I2C_ClockSpeed = 100000;
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_InitStructure.I2C_OwnAddress1 = 0x003A;
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;  //I2C_Ack_Disable;  //�ɺ���������������CR1��ACKλ����Ӧ���źţ�ʹ��I2C_AcknowledgeConfig(I2C_TypeDef* I2Cx, FunctionalState NewState)��������
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;

	I2C_Init(I2C1, &I2C_InitStructure);
	I2C_Cmd(I2C1, ENABLE);
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
