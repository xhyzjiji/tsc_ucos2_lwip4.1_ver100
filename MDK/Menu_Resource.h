#ifndef _MENU_RESOURCE_H_
#define _MENU_RESOURCE_H_

//以下数据将写入Resource.c和Resource.h文件中去
#include "stm32f10x.h"

#define ManualModeMenu_ItemNum 3
#define RTCSetMenu_ItemNum 2
#define TimeSetMenu_ItemNum	2
#define LEDColorMenu_ItemNum 3
#define FlashMenu_ItemNum 3
#define PhaseMenu_ItemNum 3
#define PeriodOfTimeMenu_ItemNum 3
#define MainMenu_ItemNum 10
/*
struct Menu_TypeDef MainMenu;
struct MenuItem_TypeDef MainMenu_Items[MainMenu_ItemNum];
struct Menu_TypeDef TimeSetMenu;
struct MenuItem_TypeDef TimeSetMenu_Items[TimeSetMenu_ItemNum];
struct Menu_TypeDef LEDColorMenu;
struct MenuItem_TypeDef LEDColorMenu_Items[LEDColorMenu_ItemNum];
struct Menu_TypeDef RTCSetMenu;
struct MenuItem_TypeDef RTCSetMenu_Items[RTCSetMenu_ItemNum];
*/
void PhaseMenu_Init(void);
void PeriodOfTimeMenu_Init(void);
void ManualModeMenu_Init(void);
void RTCSetMenu_Init(void);
void TimeSetMenu_Init(void);
void LEDColorMenu_Init(void);
void FlashMenu_Init(void);
void MainMenu_Init(void);
void Menu_Init(void);

#endif
