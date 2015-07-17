#ifndef _Traffic_Param_H_
#define _Traffic_Param_H_

#include "stm32f10x.h"

#define False 0x00
#define True (~False)

//以下为灯饰代号，选择相应灯饰，结合灯色灯位表生成Current_LED
//例如：现需要选中LeftGreen StraightGreen RightRed BackRed SideWalk1Red	SideWalk2Green
//CrossWay1_LED = LeftGreen | StraightGreen | RightRed | BackRed | SideWalk1Red | SideWalk2Green;
//Current_LED = ((CrossWay_LED&LeftGreen) == 0x0000)?     0x0000:my_Schedule.LEDColor_Mode.CrossWay1_LED.LED_LeftGreen | \
//				((CrossWay_LED&StraightGreen) == 0x0000)? 0x0000:my_Schedule.LEDColor_Mode.CrossWay1_LED.StraightGreen | \
//				((CrossWay_LED&RightGreen) == 0x0000)?    0x0000:my_Schedule.LEDColor_Mode.CrossWay1_LED.RightGreen | \
//				((CrossWay_LED&BackGreen) == 0x0000)?     0x0000:my_Schedule.LEDColor_Mode.CrossWay1_LED.BackGreen | \
//				((CrossWay_LED&LeftRed) == 0x0000)?       0x0000:my_Schedule.LEDColor_Mode.CrossWay1_LED.LeftRed | \
//				((CrossWay_LED&StraightRed) == 0x0000)?   0x0000:my_Schedule.LEDColor_Mode.CrossWay1_LED.StraightRed | \
//				((CrossWay_LED&RightRed) == 0x0000)?      0x0000:my_Schedule.LEDColor_Mode.CrossWay1_LED.RightRed | \
//				((CrossWay_LED&BackRed) == 0x0000)?       0x0000:my_Schedule.LEDColor_Mode.CrossWay1_LED.BackRed | \
//				((CrossWay_LED&SideWalk1Green) == 0x0000)?0x0000:my_Schedule.LEDColor_Mode.CrossWay1_LED.SideWalk1Green | \
//				((CrossWay_LED&SideWalk1Red) == 0x0000)?  0x0000:my_Schedule.LEDColor_Mode.CrossWay1_LED.SideWalk1Red | \
//				((CrossWay_LED&SideWalk2Green) == 0x0000)?0x0000:my_Schedule.LEDColor_Mode.CrossWay1_LED.SideWalk2Green | \
//				((CrossWay_LED&SideWalk2Red) == 0x0000)?  0x0000:my_Schedule.LEDColor_Mode.CrossWay1_LED.SideWalk2Red | \
//
//				((CrossWay_LED&LeftYellow) == 0x0000)?    0x0000:my_Schedule.LEDColor_Mode.CrossWay1_LED.LeftYellow | \
//				((CrossWay_LED&StraightYellow) == 0x0000)?0x0000:my_Schedule.LEDColor_Mode.CrossWay1_LED.StraightYellow | \
//				((CrossWay_LED&RightYellow) == 0x0000)?   0x0000:my_Schedule.LEDColor_Mode.CrossWay1_LED.RightYellow | \
//				((CrossWay_LED&BackYellow) == 0x0000)?    0x0000:my_Schedule.LEDColor_Mode.CrossWay1_LED.BackYellow;

/*
#define None 0x0000
#define LeftGreen 0x0001      //LG
#define StraightGreen 0x0002  //SG
#define RightGreen 0x0004     //RG
#define BackGreen 0x0008	  //BG
#define LeftYellow 0x0010	  //LY
#define StraightYellow 0x0020 //SY
#define RightYellow 0x0040	  //RY
#define BackYellow 0x0080	  //BY
#define LeftRed 0x0100		  //LR
#define StraightRed 0x0200	  //SR
#define RightRed 0x0400		  //RR
#define BackRed 0x0800		  //BR
#define SideWalk1Green 0x1000 //W1G
#define SideWalk1Red 0x2000	  //W1R
#define SideWalk2Green 0x4000 //W2G
#define SideWalk2Red 0x8000	  //W2R
*/

#define None 0x0000
#define LeftGreen 0x0001      //LG
#define StraightGreen 0x0002  //SG
#define RightGreen 0x0004     //RG
#define BackGreen 0x0008	  //BG
#define SideWalk1Green 0x0010 //W1G
#define SideWalk2Green 0x0020 //W2G

#define LeftYellow 0x0040	  //LY
#define StraightYellow 0x0080 //SY
#define RightYellow 0x0100	  //RY
#define BackYellow 0x0200	  //BY

#define LeftRed 0x0400		  //LR
#define StraightRed 0x0800	  //SR
#define RightRed 0x1000		  //RR
#define BackRed 0x2000		  //BR
#define SideWalk1Red 0x4000	  //W1R
#define SideWalk2Red 0x8000	  //W2R

//一切表格以此存储
//  LG     SG     RG 	 BG 	 W1G 	   W2G 		LY 	   SY 	  RY 	 BY 	LR 	   SR 	  RR 	 BR 	 W1R 	   W2R
//左转绿 直行绿 右转绿 调头绿 人行道1绿 人行道2绿 左转黄 直行黄 右转黄 调头黄 左转红 直行红 右转红 调头红 人行道1红 人行道2红
#define CROSSWAY_MAX 6
#define LED_SN_OFFSET 0x0000
#define LED_SN_SIZE 16

typedef struct
{
	//0x0000表示该灯位没有被使用
	//用于存储对应端口序号1-96
	uint8_t LEDColor_Sn[16];  //Sn:1-96/0为未使用
	//LEDColor_Sn[0]为左转绿灯的端口序号
	//LEDColor_Sn[1]为直行绿灯的端口序号
	//。。。
	//LEDColor_Sn[15]为调头红灯的端口序号
}LEDColor_TypeDef;  //48*half 0f word

typedef struct
{
	uint8_t DayNight_hour;
	uint8_t DayNight_min;
	uint8_t DayNight_sec;
}DayNight_TypeDef;

typedef struct
{
//每个灯组可独立设置
	uint16_t FLASH_Allowence;  //一个的灯组哪些灯倒数可闪
	uint16_t Day_FLASH_ON;	 //这是常闪设置
	uint16_t Night_FLASH_ON;
}FlashON_TypeDef;

typedef struct
{
	uint8_t Phase_Number;
	uint8_t Last_Time[16];		 //Last_Time[相位号]
	uint16_t Crossway_LED[6][16];	//Crossway_Led[灯组号][相位号]	
}PhaseTable_TypeDef;

typedef struct ShortTime
{	
	uint8_t hour;
	uint8_t min;
	uint8_t sec;
}ShortTime_TypeDef;

struct ShortTime_TypeDef
{	
	uint8_t hour;
	uint8_t min;
	uint8_t sec;
};

//一个完整16时段*16相位表格
struct Schedule_TypeDef
{
	uint8_t TimeTable_Number;
	ShortTime_TypeDef TimeTable[16];	 //TimeTable[时段号]
	LEDColor_TypeDef Crossway_LED_Sn[6]; //Crossway_LED_Sn[灯组号]=端口序号
	FlashON_TypeDef Crossway_Flash[6];	 //Crossway_Flash[灯组号]
	DayNight_TypeDef DayNight_Mode;		 //
	PhaseTable_TypeDef PhaseTable[16];	 //PhaseTable[时段号]
};	//一个完整表格占用907Bytes

#endif
