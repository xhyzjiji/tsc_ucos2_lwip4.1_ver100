#ifndef _SYSTEM_CONF_H_
#define _SYSTEM_CONF_H_

void delay(uint16_t time);
void RCC_Config(void);
void NVIC_Config(void);
void GPIO_Config(void);
void EXTI_Config(void);
void USART1_Config(void);
void SPI1_Config(void);
void Project_Config(void);
void GPIOs_Init(void);

#endif
