#include "stm32f10x.h"

void delay(uint16_t time)
{
	for(; time>0; time--);
}

void RCC_Config(void)
{
	ErrorStatus HSEStartUpStatus;
	
	RCC_DeInit();  //防止RCC寄存器错误
	RCC_HSEConfig(RCC_HSE_ON);	 //使用外部高速晶振
	HSEStartUpStatus == RCC_WaitForHSEStartUp();  //等待外部晶振HSE稳定振荡
	//不使用晶振就绪中断
	if(HSEStartUpStatus == SUCCESS){
		//PLL时钟供给USBCLK(可再分频)
		RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);  //HSE为8MHz，9倍频后为72MHz，也是STM32能接受的最大工作频率	
		RCC_PLLCmd(ENABLE);	 //开启PLL
		//系统时钟供给I2S2、I2S3
		RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);  //选择PLL输出作为系统时钟，
		//AHB时钟供给SDIO、FSMC、AHB总线、芯片内核、DMA、(8分频后)系统节拍时钟、FCLK、(2分频后)SDIO的AHB接口
		RCC_HCLKConfig(RCC_SYSCLK_Div1);  //AHB时钟取自于系统时钟的分频输出，72MHz
		//下面是各不同外设总线时钟设置
		//APB1时钟供给DAC、PWR、BKP、bxCAN、USB、I2C1、I2C2、UART5、UART4、USART3、USART2、
		//SPI1、SPI2、IWDG、WWDG、RTC、TIM7~TIM2(可再分频)，且最高支持36MHz工作频率
		RCC_PCLK1Config(RCC_HCLK_Div2);	 //APB1时钟取自于AHB时钟分频输出，36MHz
		//APB2时钟供给ADC1~3(可再分频)、USART1、SPI1、TIM1(可再分频)、TIM8(可再分频)、GPIOA~F、EXTI、AFIO，且最高支持72MHz工作频率
		RCC_PCLK2Config(RCC_HCLK_Div1);  //APB2时钟取自于AHB时钟分频输出，72MHz
	}
	else{
		/*
		//选择内部时钟作为系统时钟8MHz
		RCC_AdjustHSICalibrationValue(***);	 //内部时钟校准
		RCC_HSICmd(ENABLE);
		RCC_SYSCLKConfig(RCC_SYSCLKSource_HSI);
		RCC_HCLKConfig(RCC_SYSCLK_Div1);
		RCC_PCLK1Config(RCC_HCLK_Div1);
		RCC_PCLK2Config(RCC_HCLK_Div1);
		*/
	}
	//使能外设时钟
	RCC_APB2PeriphClockCmd((RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE | RCC_APB2Periph_USART1 | RCC_APB2Periph_SPI1 | RCC_APB2Periph_AFIO), ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
	//RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2| RCC_APB1Periph_TIM6, ENABLE);
	//RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
}

void NVIC_Config(void)
{
	//中断通道优先级分先分组优先级(占优先级、从优先级)
	//分组优先级设置方法有两种：
	//1.通过NVIC_Init(NVIC_InitTypeDef* NVIC_InitStruct)进行设置
	//2.先使用NVIC_EncodePriority(uint32_t PriorityGroup, uint32_t PreemptPriority, uint32_t SubPriority)分组优先级编码函数将优先级组、先占优先级、从优先级以一个uint32_t类型数据返回
	//  在使用NVIC_EncodePriority(...)的返回值做参数，使用NVIC_SetPriority(IRQn_Type IRQn, uint32_t priority)进行设置
	//  它们必须成对使用，这样的函数还有NVIC_GetPriority(...)---->NVIC_DecodePriority(...)
	NVIC_InitTypeDef NVIC_InitStructure;
	
	//在STM32/Cortex-M3中是通过改变CPU的当前优先级来使能或禁止中断
	//关闭中断总开关，有两种方式
	//1.置位PRIMASK，则只允许NMI(不可屏蔽中断)和HARD FAULT(硬件错误)产生中断
	//2.置位FAULTMASK，则只允许NMI产生中断
	__set_PRIMASK(1);  //允许不可屏蔽中断和硬件错误产生中断，关闭其他所有中断
	//__set_FAULTMASK(1);	

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

	#ifdef VECT_TAB_RAM
		NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0);
	#else  //VEC_TAB_FLASH
		NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);
	#endif

	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;				 //外部中断2
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	     //抢占优先级 0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;			 //子优先级0  
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				 //使能
	NVIC_Init(&NVIC_InitStructure);	 

	SysTick_Config(72000);	//设置系统节拍时钟重载数，并开启SysTick_IRQn中断开关  //时钟节拍中断时1ms一次  用于定时
//	NVIC_SetPriority (SysTick_IRQn, 1);	  //设定系统节拍时钟中断优先级
	//中断总开关的实现是调整CPU运行优先级高于其他中断优先级，以达到屏蔽的效果，其实还可以按照中断号进行屏蔽
	//__set_BASEPRI(0x60);  //屏蔽中断号高于0x60的所有中断请求

	__set_PRIMASK(0);  //开启中断总开关
	//__set_FAULTMASK(1);

	//一下是core_cm3.h内置函数功能一览表
	//__get_PSP();  //获取进程堆栈指针PSP，返回uint32_t类型
	//__set_PSP(uint32_t topOfProcStack);  //设置PSP

	//__get_MSP();  //获取主堆栈指针MSP，返回uint32_t类型
	//__set_MSP(uint32_t mainStackPointer);  //设置MSP

	//__REV(uint32_t value);  //以整数型数值反转字接顺序，返回uint32_t类型
	//__REV16(uint16_t value);  //反转半字中字节顺序，如0xABCD反转后得到0xCDAB，返回uint32_t类型
	//__REVSH(int16_t value);  //反转字节顺序，并做符号拓展。就是在__REV16函数得到的结果上再进行一次符号拓展。这两个函数主要是方便进行大小端的切换，返回int32_t类型
	//__RBIT(uint32_t value);  //反转位顺序，返回uint32_t类型

	//__LDREXB(uint8_t *addr);  //读取addr地址中寄存器的数值，返回uint8_t类型，8位数据专用取值命令
	//__LDREXH(uint16_t *addr);  //读取addr地址中寄存器的数值，返回uint16_t类型，16位数据专用取值命令
	//__LDREXW(uint32_t *addr);  //读取addr地址中寄存器的数值，返回uint32_t类型，32位数据专用取值命令

	//__STREXB(uint8_t value, uint8_t *addr);  //向addr地址中寄存器写入value值，8位数据专用存储命令
	//__STREXH(uint16_t value, uint16_t *addr);  //向addr地址中寄存器写入value值，16位数据专用存储命令
	//__STREXW(uint32_t value, uint32_t *addr);  //向addr地址中寄存器写入value值，32位数据专用存储命令
	
	//__CLREX();  //清除由LDREX指令造成的互斥锁。LDREX和STREX是Cortex用来实现互斥访问，保护临界资源的指令，LDREX执行后，只有离它最近的一条存储指令（STR，STREX）才能执行，其他的存储指令都会被驳回，而CLREX就是用于清除互斥访问状态的标记
	//__get_BASEPRI();  //获取BASEPRI寄存器的值，优先级号高于该寄存器的中断都会被屏蔽（优先级号越大，优先级越低），为零时不屏蔽任何中断，返回uint32_t类型
	//__set_BASEPRI(uint32_t basePri);  //设置BASEPRI的值
	//__get_PRIMASK();  //PRIMASK是一个只有一位的寄存器，置位时屏蔽绝大部分的异常中断，只剩下NMI和HardFault可以响应，返回uint32_t类型
	//__set_PRIMASK(uint32_t priMask);  //设置PRIMASK的值
	//__get_FAULTMASK();  //FAULTMASK也是一个只有一位的寄存器，为1时只有NMI才能响应，其他异常与中断全部被屏蔽，返回uint32_t类型
	//__set_FAULTMASK(uint32_t faultMask);  //设置FAULTMASK的值
	//__get_CONTROL();  //获取CONTROL的值。寄存器CONTROL只有两位。CONTROL[0]选择特权级别，0为特权级，1为敌用户级。CONTROL[1]用于选择堆栈指针，0为MSP，1为PSP，返回uint32_t类型
	//__set_CONTROL(uint32_t control);  //设置CONTROL寄存器的值
	//----------------------------------------------------------------//
	//BASEPRI，PRIMASK，FAULTMASK，CONTROL都只能在特权模式下被修改//
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

	//与NVIC_PriorityGroupConfig(uint32_t NVIC_PriorityGroup)功能类似，但需要注意的是若设置NVIC_PriorityGroupConfig(PriorityGroup_0)则对应NVIC_SetPriorityGrouping(1)，NVIC_PriorityGroupConfig(PriorityGroup_1)则对应NVIC_SetPriorityGrouping(2)如此类推
	//NVIC_SetPriorityGrouping(uint32_t PriorityGroup);  //设置优先级分组
	//NVIC_EnableIRQ(IRQn_Type IRQn);  //开启某中断通道开关
	//NVIC_DisableIRQ(IRQn_Type IRQn);  //关闭某中断通道开关
	//NVIC_GetPendingIRQ(IRQn_Type IRQn);  //检测某中断通道是否挂起
	//NVIC_SetPendingIRQ(IRQn_Type IRQn);  /强制某中断通道挂起(置位中断挂起寄存器相应位)
	//NVIC_ClearPendingIRQ(IRQn_Type IRQn);  //清除某中断通道挂起状态(置位中断挂起清除寄存器相应位)，但对已经激活的中断没有影响，除非该中断处于挂起状态
	//NVIC_GetActive(IRQn_Type IRQn);  //检测某中断通道是否被处于激活运行或者被抢占(挂起)或者压栈
	//NVIC_SetPriority(IRQn_Type IRQn, uint32_t priority);  //设置某中断通道分组优先级，外部中断优先级必须为正数，内部系统(内核)中断优先级可以为负数
	//NVIC_GetPriority(IRQn_Type IRQn);  //返回某中断通道的分组优先级
	//NVIC_EncodePriority(uint32_t PriorityGroup, uint32_t PreemptPriority, uint32_t SubPriority);  //该函数的返回值，用于NVIC_SetPriority(IRQn_Type IRQn, uint32_t priority)中的参数priority
	//NVIC_DecodePriority(uint32_t Priority, uint32_t PriorityGroup, uint32_t* pPreemptPriority, uint32_t* pSubPriority);  //将NVIC_GetPriority(...)中返回的压缩优先级进行解码成优先级组、先占优先级、从优先级
	//SysTick_Config(uint32_t ticks);  //设置系统节拍时钟重载数，并开启SysTick_IRQn中断开关
}

void GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;	         		 		//USART1 TX
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;    		 		//复用推挽输出
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;	         	 		//USART1 RX
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;   	 		//复用开漏输入
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;				     //enc_RST
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	//GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;	  //sck-->PA5  SI-->PA7  SO-->PA6
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;			  //复用功能
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;				  //enc_ncs
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    //GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
    
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;					//enc_nint
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;   	 		//内部上拉输入
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource0);
}

void EXTI_Config(void)
{
	EXTI_InitTypeDef EXTI_InitStructure;

	EXTI_InitStructure.EXTI_Line = EXTI_Line0;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;	 //比上升沿触发EXTI_Trigger_Rising抖动少
	EXTI_InitStructure.EXTI_LineCmd	= ENABLE;
	EXTI_Init(&EXTI_InitStructure);
}

void USART1_Config(void)
{
	USART_InitTypeDef USART_InitStructure;

	USART_InitStructure.USART_BaudRate = 115200;						//速率115200bps
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;		//数据位8位
	USART_InitStructure.USART_StopBits = USART_StopBits_1;			//停止位1位
	USART_InitStructure.USART_Parity = USART_Parity_No;				//无校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;   //无硬件流控
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
