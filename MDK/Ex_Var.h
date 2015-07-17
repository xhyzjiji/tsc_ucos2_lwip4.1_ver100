#ifndef	_EX_VAR_H_
#define _EX_VAR_H_

#include "stm32f10x.h"
#include "Sys_Config.h"
#include "Delay.h"
/*
#include "Menu.h"
#include "Menu_Resource.h"
#include "Traffic_Param.h"
#include "time.h"
*/
#include "KEY_OP.h"
#include "LED_OP.h"
#include "TIME_OP.h"
#include "USART_OP.h"
#include "E2PROM_OP.h"
#include "stdio.h"
#include "menu_resource.h"
#include "Traffic_Param.h"

#define KEYBuffer_Size 16

//系统标志位
extern uint8_t RTC_FLAG_SECSET;  //秒中断时置位
extern uint8_t RTC_FLAG_ALRSET;  //下一时段中断置位
extern uint8_t RTC_FLAG_HalfSecSET;  //半秒可调中断置位
extern uint8_t Time_FLAG_DayOrNight;  //每一个相位结束后检测当前时间
extern uint8_t Phase_FLAG_Start;
extern uint8_t Cycle_FLAG_Start;
extern uint8_t Init_FLAG_Finished;
extern uint8_t Manual_FLAG;
extern uint8_t No_UpdateLED_FLAG;

extern uint8_t LCD_FLAG_Using;
extern uint8_t LCD_FLAG_Oping;
extern uint8_t LCD_FLAG_UpdateIrq;

extern struct tm Current_Time;  //记录当前时间

extern struct Schedule_TypeDef my_Schedule;  //记录当前方案

//一些设置不允许马上被更新，如当前亮灯组合等，所以要进行存储，一些可马上反应的设置则可以直接修改到当前读取的方案中，
//这是一下全局变量设定的规则和原因
extern uint8_t Current_CutDown;
extern uint8_t Current_PhaseTable;
extern uint8_t Current_TimeTable;

extern uint16_t Current_Crossway_LED[6];  //Current_Crossway_LED[0]指的是第一组扩展IO口的输出
extern uint16_t temp_Current_Crossway_LED[6];

extern uint16_t Current_Crossway_Flash[6];		  //Current_CrossWay_Flash[0]指的是第一组扩展IO口的闪烁输出
extern uint16_t Current_Crossway_Flash_Allowence[6];
extern uint16_t temp_Current_Crossway_Yellow[6];
extern PhaseTable_TypeDef Phase_OneShot;

extern uint8_t KEY_Buffer[KEYBuffer_Size];
extern uint8_t KEY_BufferPointer;
extern uint8_t KEY_BufferBottomOfStack;
extern uint8_t KEY_Press;

extern uint8_t Trg;
extern uint8_t Cont;
extern KEY_TypeDef CKey;
extern uint8_t press_cycle;

extern struct Menu_TypeDef* Current_Menu;  //当前指向的菜单指针
extern uint8_t CurrentMenu_ItemNumber;	 //当前菜单选项数目

extern struct Menu_TypeDef MainMenu;
extern struct MenuItem_TypeDef MainMenu_Items[MainMenu_ItemNum];
extern struct Menu_TypeDef PhaseMenu;
extern struct MenuItem_TypeDef PhaseMenu_Items[PhaseMenu_ItemNum];
extern struct Menu_TypeDef PeriodOfTimeMenu;
extern struct MenuItem_TypeDef PeriodOfTimeMenu_Items[PeriodOfTimeMenu_ItemNum];
extern struct Menu_TypeDef FlashMenu;
extern struct MenuItem_TypeDef FlashMenu_Items[FlashMenu_ItemNum];
extern struct Menu_TypeDef TimeSetMenu;
extern struct MenuItem_TypeDef TimeSetMenu_Items[TimeSetMenu_ItemNum];
extern struct Menu_TypeDef LEDColorMenu;
extern struct MenuItem_TypeDef LEDColorMenu_Items[LEDColorMenu_ItemNum];
extern struct Menu_TypeDef RTCSetMenu;
extern struct MenuItem_TypeDef RTCSetMenu_Items[RTCSetMenu_ItemNum];
extern struct Menu_TypeDef ManualModeMenu;
extern struct MenuItem_TypeDef ManualModeMenu_Items[ManualModeMenu_ItemNum];

extern uint8_t lcd_refresh_time;

extern uint16_t used_if_record[CROSSWAY_MAX];

void Var_Init(void);

#endif
