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
  * @brief   初始化STM32内部资源
*****************************************************/

/*
  * @func   RCC_Config
  * @brief  设置RCC选择外部高速晶振，9倍后作为系统时钟SYSCLK，
  *         开启全GPIO端口，TIM1/2，USART1，AFIO，PWR，BKP
  * @param  None
  * @retval None
*/
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
	RCC_APB2PeriphClockCmd((RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE | RCC_APB2Periph_USART1 | RCC_APB2Periph_SPI1), ENABLE); // RCC_APB2Periph_AFIO 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2| RCC_APB1Periph_TIM6 | RCC_APB1Periph_I2C1 | RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
	//RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
	//RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
}

/*
  * @func   GPIO_Config
  * @brief  设置GPIOE端口：Pin0-15 /50M翻转速度/互推挽输出
  *         设置GPIOA端口：Pin9    /50M翻转速度/复用推挽输出
  *      	设置GPIOA端口：Pin10   /50M翻转速度/浮空输入
  *         设置GPIOD端口：Pin10-15/10M翻转速度/浮空输入
  * 		设置GPIOD端口：Pin10-15为中断输入线路
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
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;			  //复用功能
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
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//GPIO_Mode_IPU;   	 		//内部上拉输入
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
  * @brief  设置USART1工作模式：波特率115200/8位传输模式/1位停止位/无奇偶校验/无硬件控制流
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

	USART_Cmd(USART1, ENABLE);  //暂时不开启串口通信中断
}

/*
  * @func   USART_Config
  * @brief  设置优先级组：			主优先级0~3/从优先级0~3
  *			设置中断向量表头地址：	VEC_TAB_FLASH-->0x08000000/VEC_TAB_RAM-->0x20000000/默认表头地址为0x08000000
  *			设置外部中断10-15：		主优先级2/从优先级0
  *			设置TIM2中断：			主优先级0/从优先级0
  * @param  None
  * @retval None
*/
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
	//__set_PRIMASK(1);  //允许不可屏蔽中断和硬件错误产生中断，关闭其他所有中断
	//__set_FAULTMASK(1);	

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

	#ifdef VECT_TAB_RAM
		NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0);
	#else  //VEC_TAB_FLASH
		NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);	//参数：1用户程序起始地址，中断向量偏移地址
	#endif

//	NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_Init(&NVIC_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;				 //外部中断2
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	     //抢占优先级 0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;			 //子优先级0  
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				 //使能
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
	//中断总开关的实现是调整CPU运行优先级高于其他中断优先级，以达到屏蔽的效果，其实还可以按照中断号进行屏蔽
	//__set_BASEPRI(0x60);  //屏蔽中断号高于0x60的所有中断请求

	//__set_PRIMASK(0);  //开启中断总开关
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

/*
  * @func   EXTI_Config
  * @brief  设置外部中断线路10-15：  中断模式（非事件模式）/上跳沿触发中断
  * @param  None
  * @retval None
*/

void EXTI_Config(void)
{
	EXTI_InitTypeDef EXTI_InitStructure;

	//EXTI_InitStructure.EXTI_Line = EXTI_Line0 | EXTI_Line10 | EXTI_Line11 | EXTI_Line12 | EXTI_Line13 | EXTI_Line14 | EXTI_Line15;
	EXTI_InitStructure.EXTI_Line = EXTI_Line1;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;	 //比上升沿触发EXTI_Trigger_Rising抖动少
	EXTI_InitStructure.EXTI_LineCmd	= ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	EXTI_InitStructure.EXTI_Line = EXTI_Line0;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;	 //比上升沿触发EXTI_Trigger_Rising抖动少
	EXTI_InitStructure.EXTI_LineCmd	= ENABLE;
	EXTI_Init(&EXTI_InitStructure);
}

/*
  * @func   CommonTIM_Config
  * @brief  设置通用定时器：周期10000/预分频719/内部时钟源
  *  		CCR1：			比较输出翻转OC模式/比较值5000/有效极性为高电平
  *			CCR2：			禁止OC输出/开启比较中断/比较值7500/有效极性为高电平
  * @param  TIM_TypeDef*
  * @retval None
*/
void TIM_Config(TIM_TypeDef* TIMx)
{
/*
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;

	TIM_InternalClockConfig(TIMx);

	TIM_TimeBaseInitStructure.TIM_Period = 10000;	//周期为1s
	TIM_TimeBaseInitStructure.TIM_Prescaler = 720-1;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;  //在定时器时钟(CK_INT)频率与数字滤波器(ETR,TIx)使用的采样频率之间的分频比例
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	//TIM2初始化不需要设置TIM_RepetitionCounter
	//TIM_TimeBaseInitStructure.TIM_RepetitionCounter = ***;
	TIM_TimeBaseInit(TIMx, &TIM_TimeBaseInitStructure);

	//TIM_OCMode为设置比较输出模式，模式如下，TIM_OCMode修改的是CCMR寄存器中的OCxM[2:0]：
	//TIM_OCMode_Timing(0x00)：  输出比较寄存器TIMx_CCR1与计数器TIMx_CNT 间的比较对OC1REF不起作用
	//TIM_OCMode_Active(0x01):   当计数器TIMx_CNT的值与捕获/比较寄存器1(TIMx_CCR1)相同时，强制OC1REF为高
	//TIM_OCMode_Inactive(0x02): 当计数器TIMx_CNT的值与捕获/比较寄存器1(TIMx_CCR1)相同时，强制OC1REF为低
	//TIM_OCMode_Toggle(0x03):   当TIMx_CCR1=TIMx_CNT 时，翻转OC1REF的电平
	//stm32f10x_tim.h中未定义的0x04:  强制OC1REF为低
	//stm32f10x_tim.h中未定义的0x05:  强制OC1REF为高
	//TIM_OCMode_PWM1(0x06):     PWM模式1－  在向上计数时，一旦TIMx_CNT<TIMx_CCR1时通道1为有效电平，否则为无效电平；在向下计数时，一旦TIMx_CNT>TIMx_CCR1时通道1为无效电平(OC1REF=0)，否则为有效电平(OC1REF=1) 
	//TIM_OCMode_PWM2(0x07):     PWM模式2－  在向上计数时，一旦TIMx_CNT<TIMx_CCR1时通道1为无效电平，否则为有效电平；在向下计数时，一旦TIMx_CNT>TIMx_CCR1时通道1为有效电平，否则为无效电平
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Toggle;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	 //OC输出使能
	TIM_OCInitStructure.TIM_Pulse = 5000;	//修改输出比较寄存器的值
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	//其中TIM_OutputNState、TIM_OCNPolarity、TIM_OCNIdleState\TIM_OCIdleState均为高级定时器使用
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

	TIM_TimeBaseInitStructure.TIM_Period = 6000;	//周期为1s
	TIM_TimeBaseInitStructure.TIM_Prescaler = 720-1;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;  //在定时器时钟(CK_INT)频率与数字滤波器(ETR,TIx)使用的采样频率之间的分频比例
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIMx, &TIM_TimeBaseInitStructure);

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	 //OC输出使能
	TIM_OCInitStructure.TIM_Pulse = 3000;	//修改输出比较寄存器的值
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
	//其中TIM_OutputNState、TIM_OCNPolarity、TIM_OCNIdleState\TIM_OCIdleState均为高级定时器使用
	TIM_OC2Init(TIMx, &TIM_OCInitStructure);

	//TIM_PrescalerConfig(TIMx, 720-1, TIM_PSCReloadMode_Update);
	TIM_ARRPreloadConfig(TIMx, ENABLE);
	//TIMx->CCER |= 0x0001;
	
	TIM_UpdateRequestConfig(TIMx, TIM_UpdateSource_Regular);  //只有计数器计数溢出才产生中断
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
	TIM_TimeBaseInitStructure.TIM_Period = 30000;	//周期为1s
	TIM_TimeBaseInitStructure.TIM_Prescaler = 72-1;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;  //在定时器时钟(CK_INT)频率与数字滤波器(ETR,TIx)使用的采样频率之间的分频比例
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIMx, &TIM_TimeBaseInitStructure);

	TIM_ARRPreloadConfig(TIMx, ENABLE);
	
	TIM_UpdateRequestConfig(TIMx, TIM_UpdateSource_Regular);  //只有计数器计数溢出才产生中断
	TIM_SelectOnePulseMode(TIMx, TIM_OPMode_Repetitive);

	TIM_ClearFlag(TIMx, TIM_FLAG_Update);
	TIM_ITConfig(TIMx, TIM_IT_Update, ENABLE);
}

void RTC_Config(void)
{
	//RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
	//对RTC相关寄存器进行读写前，必须置位RTC_CRL中的CNF位，退出读写时，清楚CNF位
	//还必须读取RTOFF，即RTC是否空闲

	PWR_BackupAccessCmd(ENABLE);   //可修改备份寄存器
	BKP_DeInit();

	//设置低速晶振源
	RCC_LSEConfig(RCC_LSE_ON);
	while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET);  //等待外部低速晶振起振
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
	RCC_RTCCLKCmd(ENABLE);

	RTC_WaitForSynchro();
	RTC_WaitForLastTask();
	Time_SetCalendarTime(Current_Time);  //后期改为由串口输入或者由外部RTC获取
	RTC_WaitForLastTask();
	RTC_ITConfig(RTC_IT_ALR | RTC_IT_SEC, ENABLE);
	RTC_WaitForLastTask();
	RTC_SetPrescaler(32767);	//使用外部晶振32.768kHz时的预分频值
	RTC_WaitForLastTask();
}

void RTC_Init(void)
{
	if(BKP_ReadBackupRegister(BKP_DR1) != 0x5A5A){  //初次进入此函数，检测备份寄存器的值是否被修改为0xA5A5，未修改则初始化RTC并修改DR1为0xA5A5
		RTC_Config();
		//Time_SetCalendarTime(Init_Time);  //后期改为由串口输入或者由外部RTC获取
		BKP_WriteBackupRegister(BKP_DR1, 0x5A5A);
		//printf("RTC configured...\n");
	}
	//RTC_PRL、RTC_ALR、RTC_CNT和RTC_DIV寄存器在硬件复位或者上电复位时，内部存储的值不受影响
	//但需要重新开启RTC中断开关
	else{
		RTC_WaitForSynchro();
		RTC_WaitForLastTask();
		RTC_ITConfig(RTC_IT_ALR | RTC_IT_SEC, ENABLE);
		RTC_WaitForLastTask();
	}
	RCC_ClearFlag();  //清除复位标志位
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
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;  //I2C_Ack_Disable;  //由后面驱动按需设置CR1的ACK位产生应答信号，使用I2C_AcknowledgeConfig(I2C_TypeDef* I2Cx, FunctionalState NewState)函数即可
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
