#ifndef _SYS_CONFIG_H_
#define _SYS_CONFIG_H_

#include "stm32f10x.h"
//#include "time.h"
//#include "TIME_OP.h"

//static函数只允许本文件其他函数进行调用，好处在于其他文件可以存在相同名字的函数
void RCC_Config(void);
void GPIO_Config(void);
void GPIO_Initial(void);
void USART_Config(void);
void NVIC_Config(void);
void EXTI_Config(void);
void RTC_Config(void);
void RTC_Init(void);
void TIM_Config(TIM_TypeDef* TIMx);
void TIM_Basic_Config(TIM_TypeDef* TIMx);
void I2C1_Config(void);
void SPI1_Config(void);

#endif
