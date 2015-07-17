#include "stm32f10x.h"
#include "Menu_Resource.h"
#include "RA8806_CTRL.h"
#include "KEY_OP.h"
#include "Ex_Var.h" 

void ManualModeMenu_Init(void)
{
	ManualModeMenu_Items[0].Item_Context = "��λʱ���ӳ�";
	ManualModeMenu_Items[0].Item_x = 2;
	ManualModeMenu_Items[0].Item_y = 80;
	ManualModeMenu_Items[0].Item_HelpInfo = "�ֶ���У��ǰ��λʱ��";
	ManualModeMenu_Items[0].Children_Menu = NULL;
	ManualModeMenu_Items[0].key_function = ManualMode_CutDown_main;//NULL; //ManualMode_CutDown;
	ManualModeMenu_Items[0].key_function_init = ManualMode_CutDown_init;

	ManualModeMenu_Items[1].Item_Context = "����������λ";
	ManualModeMenu_Items[1].Item_x = 2;
	ManualModeMenu_Items[1].Item_y = 100;
	ManualModeMenu_Items[1].Item_HelpInfo = "�ֶ�ָ����ǰѭ�����е���λ";
	ManualModeMenu_Items[1].Children_Menu = NULL;
	ManualModeMenu_Items[1].key_function = ManualMode_TimePhase_main;//NULL; //ManualMode_TimePhase;
	ManualModeMenu_Items[1].key_function_init = ManualMode_TimePhase_datainit;

	ManualModeMenu_Items[2].Item_Context = "��ͨ�Ƽ��";
	ManualModeMenu_Items[2].Item_x = 2;
	ManualModeMenu_Items[2].Item_y = 120;
	ManualModeMenu_Items[2].Item_HelpInfo = "����ָ����ŵĽ�ͨ��";
	ManualModeMenu_Items[2].Children_Menu = NULL;
	ManualModeMenu_Items[2].key_function = LED_Check_main;//NULL; //LED_Check;
	ManualModeMenu_Items[2].key_function_init = LED_Check_datainit;

	ManualModeMenu.Item_Num = ManualModeMenu_ItemNum;
	ManualModeMenu.Item_Sel = 0;
	ManualModeMenu.Menu_Title = "�ֶ�ģʽ";
	ManualModeMenu.Items = ManualModeMenu_Items;
	ManualModeMenu.Parent_Menu = &MainMenu;

	ManualModeMenu.key_function = menu_keywork_mode2;
	ManualModeMenu.menu_frame = disp_frame2;
	ManualModeMenu.item_display = disp_item;
	ManualModeMenu.help_display = disp_help2;
	ManualModeMenu.cur_funptr = ManualModeMenu.key_function;
	ManualModeMenu.fundata = NULL;
}

void RTCSetMenu_Init(void)
{
	RTCSetMenu_Items[0].Item_Context = "�ڲ�RTCʱ������";
	RTCSetMenu_Items[0].Item_x = 2;
	RTCSetMenu_Items[0].Item_y = 80;
	RTCSetMenu_Items[0].Item_HelpInfo = "������ǰʱ��ʱ��";
	RTCSetMenu_Items[0].Children_Menu = NULL;
	RTCSetMenu_Items[0].key_function = RTC_SetDate;//NULL; //RTC_SetDate;
	RTCSetMenu_Items[0].key_function_init = RTC_SetDate_Init;

	RTCSetMenu_Items[1].Item_Context = "�����������ⲿRTC";
	RTCSetMenu_Items[1].Item_x = 2;
	RTCSetMenu_Items[1].Item_y = 100;
	RTCSetMenu_Items[1].Item_HelpInfo = "����ǰʱ�䱣�浽�ⲿʱ�ӵ�·";
	RTCSetMenu_Items[1].Children_Menu = NULL;
	RTCSetMenu_Items[1].key_function = NULL; //DS1302_SaveTime;
	RTCSetMenu_Items[1].key_function_init = NULL;
	RTCSetMenu_Items[1].custom_function = time_saveto_ds1302;

	RTCSetMenu.Item_Num = RTCSetMenu_ItemNum;
	RTCSetMenu.Item_Sel = 0;
	RTCSetMenu.Menu_Title = "RTC ����";
	RTCSetMenu.Items = RTCSetMenu_Items;
	RTCSetMenu.Parent_Menu = &TimeSetMenu;

	RTCSetMenu.key_function = menu_keywork_mode2;
	RTCSetMenu.menu_frame = disp_frame2;
	RTCSetMenu.item_display = disp_item;
	RTCSetMenu.help_display = disp_help2;
	RTCSetMenu.cur_funptr = RTCSetMenu.key_function;
	RTCSetMenu.fundata = NULL;
}

void TimeSetMenu_Init(void)
{
	TimeSetMenu_Items[0].Item_Context = "��ǰʱ������";
	TimeSetMenu_Items[0].Item_x = 2;
	TimeSetMenu_Items[0].Item_y = 80;
	TimeSetMenu_Items[0].Item_HelpInfo = "����DS1302ʱ��ʱ��";
	TimeSetMenu_Items[0].Children_Menu = &RTCSetMenu;
	TimeSetMenu_Items[0].key_function = NULL;
	TimeSetMenu_Items[0].key_function_init = NULL;

	TimeSetMenu_Items[1].Item_Context = "��ҹʱ������";
	TimeSetMenu_Items[1].Item_x = 2;
	TimeSetMenu_Items[1].Item_y = 110;
	TimeSetMenu_Items[1].Item_HelpInfo = "������ҹ��˸ʱ���";
	TimeSetMenu_Items[1].Children_Menu = NULL;
	TimeSetMenu_Items[1].key_function = DayNight_PeriodofTime;//NULL; //DayNight_PeriodofTime;
	TimeSetMenu_Items[1].key_function_init = DayNight_PeriodofTime_Init;
/*
	TimeSetMenu_Items[2].Item_Context = "ʱ��ʱ������";
	TimeSetMenu_Items[2].Item_x = 2;
	TimeSetMenu_Items[2].Item_y = 140;
	TimeSetMenu_Items[2].Item_HelpInfo = "������ǰ����ʱ�Σ�����ʱ��ʱ��";
	TimeSetMenu_Items[2].Children_Menu = NULL;
	TimeSetMenu_Items[2].function = NULL;
*/
	TimeSetMenu.Item_Num = TimeSetMenu_ItemNum;
	TimeSetMenu.Item_Sel = 0;
	TimeSetMenu.Menu_Title = "ʱ������";
	TimeSetMenu.Items = TimeSetMenu_Items;
	TimeSetMenu.Parent_Menu = &MainMenu;

	TimeSetMenu.key_function = menu_keywork_mode2;
	TimeSetMenu.menu_frame = disp_frame2;
	TimeSetMenu.item_display = disp_item;
	TimeSetMenu.help_display = disp_help2;
	TimeSetMenu.cur_funptr = TimeSetMenu.key_function;
	TimeSetMenu.fundata = NULL;
}

void LEDColorMenu_Init(void)
{
	LEDColorMenu_Items[0].Item_Context = "�޸ĵ������";
	LEDColorMenu_Items[0].Item_x = 2;
	LEDColorMenu_Items[0].Item_y = 80;
	LEDColorMenu_Items[0].Item_HelpInfo = "�޸ĵ�������ڵ�ӳ���ϵ";
	LEDColorMenu_Items[0].Children_Menu = NULL;
	LEDColorMenu_Items[0].key_function = LCD_LedGPIO;//NULL; //LCD_Crossway1_LedIO; //LCD_LedGPIO_Table;
	LEDColorMenu_Items[0].key_function_init	= LCD_LedGPIO_Init;

	LEDColorMenu_Items[1].Item_Context = "�������ӿ�����";
	LEDColorMenu_Items[1].Item_x = 2;
	LEDColorMenu_Items[1].Item_y = 100;
	LEDColorMenu_Items[1].Item_HelpInfo = "���������ŵ��ⲿ�洢��";
	LEDColorMenu_Items[1].Children_Menu = NULL;
	LEDColorMenu_Items[1].key_function = NULL; //LCD_Crossway2_LedIO;
	LEDColorMenu_Items[1].key_function_init	= NULL;
	LEDColorMenu_Items[1].custom_function = ledgpio_saveto_e2prom;

	LEDColorMenu_Items[2].Item_Context = "�ָ���������";
	LEDColorMenu_Items[2].Item_x = 2;
	LEDColorMenu_Items[2].Item_y = 120;
	LEDColorMenu_Items[2].Item_HelpInfo = "���ӿ����ûָ�ΪĬ�ϳ���״̬";
	LEDColorMenu_Items[2].Children_Menu = NULL;
	LEDColorMenu_Items[2].key_function = NULL; //LCD_Crossway3_LedIO;
	LEDColorMenu_Items[2].key_function_init	= NULL;
	LEDColorMenu_Items[2].custom_function = reset_ledgpio_map;
/*	
	LEDColorMenu_Items[3].Item_Context = "������";
	LEDColorMenu_Items[3].Item_x = 2;
	LEDColorMenu_Items[3].Item_y = 140;
	LEDColorMenu_Items[3].Item_HelpInfo = "�޸ĵ����ĵ�ɫ��IO�ڵĹ���";
	LEDColorMenu_Items[3].Children_Menu = NULL;
	LEDColorMenu_Items[3].key_function = NULL; //LCD_Crossway4_LedIO;
	
	LEDColorMenu_Items[4].Item_Context = "������";
	LEDColorMenu_Items[4].Item_x = 2;
	LEDColorMenu_Items[4].Item_y = 160;
	LEDColorMenu_Items[4].Item_HelpInfo = "�޸ĵ������ɫ��IO�ڵĹ���";
	LEDColorMenu_Items[4].Children_Menu = NULL;
	LEDColorMenu_Items[4].key_function = NULL; //LCD_Crossway5_LedIO;
	
	LEDColorMenu_Items[5].Item_Context = "������";
	LEDColorMenu_Items[5].Item_x = 2;
	LEDColorMenu_Items[5].Item_y = 180;
	LEDColorMenu_Items[5].Item_HelpInfo = "�޸ĵ�������ɫ��IO�ڵĹ���";
	LEDColorMenu_Items[5].Children_Menu = NULL;
	LEDColorMenu_Items[5].key_function = NULL; //LCD_Crossway6_LedIO;
*/	
	LEDColorMenu.Item_Num = LEDColorMenu_ItemNum;
	LEDColorMenu.Item_Sel = 0;
	LEDColorMenu.Menu_Title = "��λ��ɫ";
	LEDColorMenu.Items = LEDColorMenu_Items;
	LEDColorMenu.Parent_Menu = &MainMenu;

	LEDColorMenu.key_function = menu_keywork_mode2;
	LEDColorMenu.menu_frame = disp_frame2;
	LEDColorMenu.item_display = disp_item;
	LEDColorMenu.help_display = disp_help2;
	LEDColorMenu.cur_funptr = LEDColorMenu.key_function;
	LEDColorMenu.fundata = NULL;
}

void FlashMenu_Init(void)
{
	FlashMenu_Items[0].Item_Context = "��˸�������";
	FlashMenu_Items[0].Item_x = 2;
	FlashMenu_Items[0].Item_y = 80;
	FlashMenu_Items[0].Children_Menu = NULL;
	FlashMenu_Items[0].Item_HelpInfo = "����������˸�ĵ�ɫ";
	FlashMenu_Items[0].key_function = flash_allowed_main; //Set_FlashAllowence;
	FlashMenu_Items[0].key_function_init = flash_allowed_datainit;
	
	FlashMenu_Items[1].Item_Context = "���쳣������";
	FlashMenu_Items[1].Item_x = 2;
	FlashMenu_Items[1].Item_y = 110;
	FlashMenu_Items[1].Children_Menu = NULL;
	FlashMenu_Items[1].Item_HelpInfo = "���ð��糣����ɫ";
	FlashMenu_Items[1].key_function = dayflash_main; //Set_FlashAllowence;
	FlashMenu_Items[1].key_function_init = dayflash_datainit;

	FlashMenu_Items[2].Item_Context = "ҹ�䳣������";
	FlashMenu_Items[2].Item_x = 2;
	FlashMenu_Items[2].Item_y = 130;
	FlashMenu_Items[2].Children_Menu = NULL;
	FlashMenu_Items[2].Item_HelpInfo = "������䳣����ɫ";
	FlashMenu_Items[2].key_function = nightflash_main; //Set_FlashAllowence;
	FlashMenu_Items[2].key_function_init = nightflash_datainit;

	FlashMenu.Item_Num = FlashMenu_ItemNum;
	FlashMenu.Item_Sel = 0;
	FlashMenu.Menu_Title = "��˸����";
	FlashMenu.Items = FlashMenu_Items;
	FlashMenu.Parent_Menu = &MainMenu;

	FlashMenu.key_function = menu_keywork_mode2;
	FlashMenu.menu_frame = disp_frame2;
	FlashMenu.item_display = disp_item;
	FlashMenu.help_display = disp_help2;
	FlashMenu.cur_funptr = FlashMenu.key_function;
	FlashMenu.fundata = NULL;
}

void PhaseMenu_Init(void)
{
	PhaseMenu_Items[0].Item_Context = "������λ";
	PhaseMenu_Items[0].Item_x = 2;
	PhaseMenu_Items[0].Item_y = 80;
	PhaseMenu_Items[0].Item_HelpInfo = "����һ����λ�����ø���λ����";
	PhaseMenu_Items[0].Children_Menu = NULL;
	PhaseMenu_Items[0].key_function = add_phase_main;//NULL; //Add_Phase;
	PhaseMenu_Items[0].key_function_init = add_phase_datainit;

	PhaseMenu_Items[1].Item_Context = "ɾ����λ";
	PhaseMenu_Items[1].Item_x = 2;
	PhaseMenu_Items[1].Item_y = 105;
	PhaseMenu_Items[1].Children_Menu = NULL;
	PhaseMenu_Items[1].Item_HelpInfo = "ɾ��һ����֪��λ";
	PhaseMenu_Items[1].key_function = del_phase_main;//NULL; //Delete_Phase;
	PhaseMenu_Items[1].key_function_init = del_phase_datainit;

	PhaseMenu_Items[2].Item_Context = "��λ����";
	PhaseMenu_Items[2].Item_x = 2;
	PhaseMenu_Items[2].Item_y = 130;
	PhaseMenu_Items[2].Children_Menu = NULL;
	PhaseMenu_Items[2].Item_HelpInfo = "�޸�������λ����";
	PhaseMenu_Items[2].key_function = set_phase_main;//NULL; //Set_Phase;
	PhaseMenu_Items[2].key_function_init = set_phase_datainit;

	PhaseMenu.Item_Num = PhaseMenu_ItemNum;
	PhaseMenu.Item_Sel = 0;
	PhaseMenu.Menu_Title = "��λ�˵�";
	PhaseMenu.Items = PhaseMenu_Items;
	PhaseMenu.Parent_Menu = &MainMenu;

	PhaseMenu.key_function = menu_keywork_mode2;
	PhaseMenu.menu_frame = disp_frame2;
	PhaseMenu.item_display = disp_item;
	PhaseMenu.help_display = disp_help2;
	PhaseMenu.cur_funptr = PhaseMenu.key_function;
	PhaseMenu.fundata = NULL;
}

void PeriodOfTimeMenu_Init(void)
{
	PeriodOfTimeMenu_Items[0].Item_Context = "����ʱ��";
	PeriodOfTimeMenu_Items[0].Item_x = 2;
	PeriodOfTimeMenu_Items[0].Item_y = 80;
	PeriodOfTimeMenu_Items[0].Item_HelpInfo = "����һ��ʱ�β����ø�ʱ�β���";
	PeriodOfTimeMenu_Items[0].Children_Menu = NULL;
	PeriodOfTimeMenu_Items[0].key_function = add_period_main;//NULL; //Add_PeriodOfTime;
	PeriodOfTimeMenu_Items[0].key_function_init = add_period_datainit;

	PeriodOfTimeMenu_Items[1].Item_Context = "ɾ��ʱ��";
	PeriodOfTimeMenu_Items[1].Item_x = 2;
	PeriodOfTimeMenu_Items[1].Item_y = 105;
	PeriodOfTimeMenu_Items[1].Children_Menu = NULL;
	PeriodOfTimeMenu_Items[1].Item_HelpInfo = "ɾ��һ����֪ʱ��";
	PeriodOfTimeMenu_Items[1].key_function = del_period_main;//NULL; //Delete_PeriodOfTime;
	PeriodOfTimeMenu_Items[1].key_function_init = del_period_datainit;

	PeriodOfTimeMenu_Items[2].Item_Context = "ʱ������";
	PeriodOfTimeMenu_Items[2].Item_x = 2;
	PeriodOfTimeMenu_Items[2].Item_y = 130;
	PeriodOfTimeMenu_Items[2].Children_Menu = NULL;
	PeriodOfTimeMenu_Items[2].Item_HelpInfo = "�޸���֪ʱ�β���";
	PeriodOfTimeMenu_Items[2].key_function = set_period_main;//NULL; //Set_PeriodOfTime;
	PeriodOfTimeMenu_Items[2].key_function_init = set_period_datainit;

	PeriodOfTimeMenu.Item_Num = PeriodOfTimeMenu_ItemNum;
	PeriodOfTimeMenu.Item_Sel = 0;
	PeriodOfTimeMenu.Menu_Title = "ʱ������";
	PeriodOfTimeMenu.Items = PeriodOfTimeMenu_Items;
	PeriodOfTimeMenu.Parent_Menu = &MainMenu;

	PeriodOfTimeMenu.key_function = menu_keywork_mode2;
	PeriodOfTimeMenu.menu_frame = disp_frame2;
	PeriodOfTimeMenu.item_display = disp_item;
	PeriodOfTimeMenu.help_display = disp_help2;
	PeriodOfTimeMenu.cur_funptr	= PeriodOfTimeMenu.key_function;
	PeriodOfTimeMenu.fundata = NULL;
}

void MainMenu_Init(void)
{
	MainMenu_Items[0].Item_Context = "��λ��ɫ";
	MainMenu_Items[0].Item_x = 2;
	MainMenu_Items[0].Item_y = 61;
	MainMenu_Items[0].Item_HelpInfo = "����GPIO�������ɫ������ϵ";
	MainMenu_Items[0].Children_Menu = &LEDColorMenu;
	MainMenu_Items[0].key_function = NULL;
	MainMenu_Items[0].custom_function = NULL;

	MainMenu_Items[1].Item_Context = "��˸����";
	MainMenu_Items[1].Item_x = 2;
	MainMenu_Items[1].Item_y = 82;
	MainMenu_Items[1].Children_Menu = &FlashMenu;
	MainMenu_Items[1].Item_HelpInfo = "���ó�����ɫ��������˸��ɫ";
	MainMenu_Items[1].key_function = NULL;
	MainMenu_Items[1].custom_function = NULL;

	MainMenu_Items[2].Item_Context = "ʱ������";
	MainMenu_Items[2].Item_x = 2;
	MainMenu_Items[2].Item_y = 103;
	MainMenu_Items[2].Children_Menu = &PeriodOfTimeMenu;
	MainMenu_Items[2].Item_HelpInfo = "���ɾ��ʱ�λ����ʱ�β���";
	MainMenu_Items[2].key_function = NULL;
	MainMenu_Items[2].custom_function = NULL;

	MainMenu_Items[3].Item_Context = "��λ����";
	MainMenu_Items[3].Item_x = 2;
	MainMenu_Items[3].Item_y = 124;
	MainMenu_Items[3].Children_Menu = &PhaseMenu;
	MainMenu_Items[3].Item_HelpInfo = "���ɾ����λ�������λ����";
	MainMenu_Items[3].key_function = NULL;
	MainMenu_Items[3].custom_function = NULL;
	
	MainMenu_Items[4].Item_Context = "ʱ������";
	MainMenu_Items[4].Item_x = 2;
	MainMenu_Items[4].Item_y = 145;
	MainMenu_Items[4].Children_Menu = &TimeSetMenu;
	MainMenu_Items[4].Item_HelpInfo = "���ĵ�ǰʱ�䡢������ҹʱ�������ʱ��";
	MainMenu_Items[4].key_function = NULL;
	MainMenu_Items[4].custom_function = NULL;
	
	MainMenu_Items[5].Item_Context = "����ѡ��";
	MainMenu_Items[5].Item_x = 2;
	MainMenu_Items[5].Item_y = 166;
	MainMenu_Items[5].Item_HelpInfo = "��ѡ16����֪������4���Զ����޸ķ���";
	MainMenu_Items[5].Children_Menu = NULL;
	MainMenu_Items[5].key_function = NULL;
	MainMenu_Items[5].custom_function = NULL;
	
	MainMenu_Items[6].Item_Context = "�ֶ�ģʽ";
	MainMenu_Items[6].Item_x = 21;
	MainMenu_Items[6].Item_y = 61;
	MainMenu_Items[6].Item_HelpInfo = "�ֶ�����ģʽ��ֻ�ڲ�����";
	MainMenu_Items[6].Children_Menu = &ManualModeMenu;
	MainMenu_Items[6].key_function = NULL;	  //�趨������
	MainMenu_Items[6].custom_function = NULL;	//����ִ�еĺ���
	
	MainMenu_Items[7].Item_Context = "���⹤��ģʽ";
	MainMenu_Items[7].Item_x = 21;
	MainMenu_Items[7].Item_y = 82;
	MainMenu_Items[7].Item_HelpInfo = "���޸ķ��������Զ����޸ķ���";
	MainMenu_Items[7].Children_Menu = NULL;
	MainMenu_Items[7].key_function = NULL;
	MainMenu_Items[7].custom_function = NULL;

	MainMenu_Items[8].Item_Context = "��������ģʽ";
	MainMenu_Items[8].Item_x = 21;
	MainMenu_Items[8].Item_y = 103;
	MainMenu_Items[8].Item_HelpInfo = "ʹ�źŻ���������״̬";
	MainMenu_Items[8].Children_Menu = NULL;
	MainMenu_Items[8].key_function = NULL;
	MainMenu_Items[8].custom_function = NULL;

	MainMenu_Items[9].Item_Context = "�������õ�PROM";
	MainMenu_Items[9].Item_x = 21;
	MainMenu_Items[9].Item_y = 124;
	MainMenu_Items[9].Item_HelpInfo = "���޸ķ��������Զ����޸ķ���";
	MainMenu_Items[9].Children_Menu = NULL;
	MainMenu_Items[9].key_function = NULL;
	MainMenu_Items[9].custom_function = NULL;
	
	MainMenu.Item_Num = MainMenu_ItemNum;
	MainMenu.Item_Sel = 0;
	MainMenu.Menu_Title = " ";
	MainMenu.Items = MainMenu_Items;
	MainMenu.Parent_Menu = NULL;

	MainMenu.key_function = menu_keywork_mode1;
	MainMenu.menu_frame = disp_frame1;
	MainMenu.item_display = disp_item;
	MainMenu.help_display = disp_help1;
	MainMenu.cur_funptr	= MainMenu.key_function;
	MainMenu.fundata = NULL;
}

void Menu_Init(void)
{
	ManualModeMenu_Init();
	RTCSetMenu_Init();
	TimeSetMenu_Init();
	LEDColorMenu_Init();
	FlashMenu_Init();
	PhaseMenu_Init();
	PeriodOfTimeMenu_Init();
	MainMenu_Init();
}
