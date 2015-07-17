#ifndef _Traffic_Param_H_
#define _Traffic_Param_H_

#include "stm32f10x.h"

#define False 0x00
#define True (~False)

//����Ϊ���δ��ţ�ѡ����Ӧ���Σ���ϵ�ɫ��λ������Current_LED
//���磺����Ҫѡ��LeftGreen StraightGreen RightRed BackRed SideWalk1Red	SideWalk2Green
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

//һ�б���Դ˴洢
//  LG     SG     RG 	 BG 	 W1G 	   W2G 		LY 	   SY 	  RY 	 BY 	LR 	   SR 	  RR 	 BR 	 W1R 	   W2R
//��ת�� ֱ���� ��ת�� ��ͷ�� ���е�1�� ���е�2�� ��ת�� ֱ�л� ��ת�� ��ͷ�� ��ת�� ֱ�к� ��ת�� ��ͷ�� ���е�1�� ���е�2��
#define CROSSWAY_MAX 6
#define LED_SN_OFFSET 0x0000
#define LED_SN_SIZE 16

typedef struct
{
	//0x0000��ʾ�õ�λû�б�ʹ��
	//���ڴ洢��Ӧ�˿����1-96
	uint8_t LEDColor_Sn[16];  //Sn:1-96/0Ϊδʹ��
	//LEDColor_Sn[0]Ϊ��ת�̵ƵĶ˿����
	//LEDColor_Sn[1]Ϊֱ���̵ƵĶ˿����
	//������
	//LEDColor_Sn[15]Ϊ��ͷ��ƵĶ˿����
}LEDColor_TypeDef;  //48*half 0f word

typedef struct
{
	uint8_t DayNight_hour;
	uint8_t DayNight_min;
	uint8_t DayNight_sec;
}DayNight_TypeDef;

typedef struct
{
//ÿ������ɶ�������
	uint16_t FLASH_Allowence;  //һ���ĵ�����Щ�Ƶ�������
	uint16_t Day_FLASH_ON;	 //���ǳ�������
	uint16_t Night_FLASH_ON;
}FlashON_TypeDef;

typedef struct
{
	uint8_t Phase_Number;
	uint8_t Last_Time[16];		 //Last_Time[��λ��]
	uint16_t Crossway_LED[6][16];	//Crossway_Led[�����][��λ��]	
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

//һ������16ʱ��*16��λ���
struct Schedule_TypeDef
{
	uint8_t TimeTable_Number;
	ShortTime_TypeDef TimeTable[16];	 //TimeTable[ʱ�κ�]
	LEDColor_TypeDef Crossway_LED_Sn[6]; //Crossway_LED_Sn[�����]=�˿����
	FlashON_TypeDef Crossway_Flash[6];	 //Crossway_Flash[�����]
	DayNight_TypeDef DayNight_Mode;		 //
	PhaseTable_TypeDef PhaseTable[16];	 //PhaseTable[ʱ�κ�]
};	//һ���������ռ��907Bytes

#endif
