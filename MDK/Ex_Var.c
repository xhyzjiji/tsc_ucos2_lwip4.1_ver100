#include "stm32f10x.h"
#include "Ex_Var.h"
#include "KEY_OP.h"
#include "string.h"

void Var_Init(void)
{
//////初始化全局变量-LCD Project
/*
	KEY_Press = 0;
	KEY_BufferPointer = 0;
	KEY_BufferBottomOfStack = 0;
*/

	CKey = Nothing;
	press_cycle = 0;
	Trg = 0x00;
	Cont = 0x00;			

//////系统标志位初始化-JOY Project
	RTC_FLAG_SECSET = 0;  //秒中断时置位
	RTC_FLAG_ALRSET = 0;  //下一时段中断置位
	RTC_FLAG_HalfSecSET = 0;  //半秒可调中断置位
	Time_FLAG_DayOrNight = 0;  //每一个相位结束后检测当前时间
	Phase_FLAG_Start = 0;
	Cycle_FLAG_Start = 0;
	Init_FLAG_Finished = 0;
	Manual_FLAG = 0;
	No_UpdateLED_FLAG = 0;

	LCD_FLAG_Using = 0;
	LCD_FLAG_Oping = 0;
	LCD_FLAG_UpdateIrq = 0;

	Current_CutDown = 0;
	Current_PhaseTable = 0;
	Current_TimeTable = 0;

	Current_Menu = &MainMenu;
	CurrentMenu_ItemNumber = Current_Menu->Item_Num;

	memset(used_if_record, 0, sizeof(uint16_t)*CROSSWAY_MAX);
}
