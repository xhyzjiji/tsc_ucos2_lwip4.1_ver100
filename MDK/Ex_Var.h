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

//ϵͳ��־λ
extern uint8_t RTC_FLAG_SECSET;  //���ж�ʱ��λ
extern uint8_t RTC_FLAG_ALRSET;  //��һʱ���ж���λ
extern uint8_t RTC_FLAG_HalfSecSET;  //����ɵ��ж���λ
extern uint8_t Time_FLAG_DayOrNight;  //ÿһ����λ�������⵱ǰʱ��
extern uint8_t Phase_FLAG_Start;
extern uint8_t Cycle_FLAG_Start;
extern uint8_t Init_FLAG_Finished;
extern uint8_t Manual_FLAG;
extern uint8_t No_UpdateLED_FLAG;

extern uint8_t LCD_FLAG_Using;
extern uint8_t LCD_FLAG_Oping;
extern uint8_t LCD_FLAG_UpdateIrq;

extern struct tm Current_Time;  //��¼��ǰʱ��

extern struct Schedule_TypeDef my_Schedule;  //��¼��ǰ����

//һЩ���ò��������ϱ����£��統ǰ������ϵȣ�����Ҫ���д洢��һЩ�����Ϸ�Ӧ�����������ֱ���޸ĵ���ǰ��ȡ�ķ����У�
//����һ��ȫ�ֱ����趨�Ĺ����ԭ��
extern uint8_t Current_CutDown;
extern uint8_t Current_PhaseTable;
extern uint8_t Current_TimeTable;

extern uint16_t Current_Crossway_LED[6];  //Current_Crossway_LED[0]ָ���ǵ�һ����չIO�ڵ����
extern uint16_t temp_Current_Crossway_LED[6];

extern uint16_t Current_Crossway_Flash[6];		  //Current_CrossWay_Flash[0]ָ���ǵ�һ����չIO�ڵ���˸���
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

extern struct Menu_TypeDef* Current_Menu;  //��ǰָ��Ĳ˵�ָ��
extern uint8_t CurrentMenu_ItemNumber;	 //��ǰ�˵�ѡ����Ŀ

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
