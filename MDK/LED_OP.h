#ifndef _LED_OP_H_
#define _LED_OP_H_

#include "stm32f10x.h"
#include "Sys_Config.h"
#include "Ex_Var.h"
#include "Delay.h"

#include "Menu.h"
#include "Menu_Resource.h"
#include "Traffic_Param.h"
#include "time.h"

#include "KEY_OP.h"
#include "TIME_OP.h"
#include "USART_OP.h"
#include "E2PROM_OP.h"

#include "stdio.h"

#define LE_Port GPIOC
#define LED_Port GPIOD
#define LE1 GPIO_Pin_5
#define LE2 GPIO_Pin_6
#define LE3 GPIO_Pin_7
#define LE4 GPIO_Pin_2
#define LE5 GPIO_Pin_3
#define LE6 GPIO_Pin_4

uint16_t LE_sel(uint8_t x);

//__ASM uint32_t __REV16BIT(uint16_t value);  //°´Î»·­×ª°ë×Ö
uint16_t REVLast8BIT(uint16_t date);
uint16_t REVFore8BIT(uint16_t date);

ErrorStatus ledgpio_map_check_used(uint8_t sn);

void CrossWay_LED_Display(uint16_t CrossWay1_LED, \
				 uint16_t CrossWay2_LED, \
				 uint16_t CrossWay3_LED, \
				 uint16_t CrossWay4_LED, \
				 uint16_t CrossWay5_LED, \
				 uint16_t CrossWay6_LED);
void CrossWay_LED_Die(uint16_t CrossWay1_LED, \
				 	  uint16_t CrossWay2_LED, \
				 	  uint16_t CrossWay3_LED, \
				 	  uint16_t CrossWay4_LED, \
				 	  uint16_t CrossWay5_LED, \
				 	  uint16_t CrossWay6_LED);
void All_LED_Die(void);
void LED_Init(void);
void LED_Sec_Display(void);
void LED_HalfSec_Display(void);
void LED_Interpretation(uint8_t TimeTable, uint8_t PhaseTable);

#endif
