#ifndef _KEY_OP_H_
#define _KEY_OP_H_

#include "stm32f10x.h"
#include "Delay.h"
//#include "Menu.h"
#include "Sys_Config.h"
#include "stdio.h"

typedef enum{
	Nothing = 0x00U,
	KEY1 = 1,
	KEY2,
	KEY3,
	KEY4,
	KEY5,
	KEY6,
	KEY1_Cont,
	KEY2_Cont,
	KEY3_Cont,
	KEY4_Cont,
	KEY5_Cont,
	KEY6_Cont,
	KEY1_2,
	KEY1_3,
	KEY1_4,
	KEY2_3,
	KEY2_4,
	KEY3_4	   //LEFT
}KEY_TypeDef;

void LCD_RefreshItem(void);
void LCD_NorItem(void);
void disp_frame1(void);
void disp_frame2(void);
void disp_item(void);
void disp_help1(void);
void disp_help2(void);

void KEY_PressMinus(void);
void KEY1_OP(void);
void KEY2_OP(void);
void KEY3_OP(void);
void KEY4_OP(void);
void KEY5_OP(void);
void KEY6_OP(void);

void menu_keywork_mode1(KEY_TypeDef key);
void menu_keywork_mode2(KEY_TypeDef key);

#endif
