#include "stm32f10x.h"
#include "Menu_Resource.h"
#include "RA8806_CTRL.h"
#include "KEY_OP.h"
#include "Ex_Var.h" 

void ManualModeMenu_Init(void)
{
	ManualModeMenu_Items[0].Item_Context = "相位时间延长";
	ManualModeMenu_Items[0].Item_x = 2;
	ManualModeMenu_Items[0].Item_y = 80;
	ManualModeMenu_Items[0].Item_HelpInfo = "手动调校当前相位时间";
	ManualModeMenu_Items[0].Children_Menu = NULL;
	ManualModeMenu_Items[0].key_function = ManualMode_CutDown_main;//NULL; //ManualMode_CutDown;
	ManualModeMenu_Items[0].key_function_init = ManualMode_CutDown_init;

	ManualModeMenu_Items[1].Item_Context = "运行锁定相位";
	ManualModeMenu_Items[1].Item_x = 2;
	ManualModeMenu_Items[1].Item_y = 100;
	ManualModeMenu_Items[1].Item_HelpInfo = "手动指定当前循环运行的相位";
	ManualModeMenu_Items[1].Children_Menu = NULL;
	ManualModeMenu_Items[1].key_function = ManualMode_TimePhase_main;//NULL; //ManualMode_TimePhase;
	ManualModeMenu_Items[1].key_function_init = ManualMode_TimePhase_datainit;

	ManualModeMenu_Items[2].Item_Context = "交通灯检测";
	ManualModeMenu_Items[2].Item_x = 2;
	ManualModeMenu_Items[2].Item_y = 120;
	ManualModeMenu_Items[2].Item_HelpInfo = "点亮指定序号的交通灯";
	ManualModeMenu_Items[2].Children_Menu = NULL;
	ManualModeMenu_Items[2].key_function = LED_Check_main;//NULL; //LED_Check;
	ManualModeMenu_Items[2].key_function_init = LED_Check_datainit;

	ManualModeMenu.Item_Num = ManualModeMenu_ItemNum;
	ManualModeMenu.Item_Sel = 0;
	ManualModeMenu.Menu_Title = "手动模式";
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
	RTCSetMenu_Items[0].Item_Context = "内部RTC时间设置";
	RTCSetMenu_Items[0].Item_x = 2;
	RTCSetMenu_Items[0].Item_y = 80;
	RTCSetMenu_Items[0].Item_HelpInfo = "调整当前时钟时间";
	RTCSetMenu_Items[0].Children_Menu = NULL;
	RTCSetMenu_Items[0].key_function = RTC_SetDate;//NULL; //RTC_SetDate;
	RTCSetMenu_Items[0].key_function_init = RTC_SetDate_Init;

	RTCSetMenu_Items[1].Item_Context = "保存设置至外部RTC";
	RTCSetMenu_Items[1].Item_x = 2;
	RTCSetMenu_Items[1].Item_y = 100;
	RTCSetMenu_Items[1].Item_HelpInfo = "将当前时间保存到外部时钟电路";
	RTCSetMenu_Items[1].Children_Menu = NULL;
	RTCSetMenu_Items[1].key_function = NULL; //DS1302_SaveTime;
	RTCSetMenu_Items[1].key_function_init = NULL;
	RTCSetMenu_Items[1].custom_function = time_saveto_ds1302;

	RTCSetMenu.Item_Num = RTCSetMenu_ItemNum;
	RTCSetMenu.Item_Sel = 0;
	RTCSetMenu.Menu_Title = "RTC 设置";
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
	TimeSetMenu_Items[0].Item_Context = "当前时间设置";
	TimeSetMenu_Items[0].Item_x = 2;
	TimeSetMenu_Items[0].Item_y = 80;
	TimeSetMenu_Items[0].Item_HelpInfo = "调整DS1302时钟时间";
	TimeSetMenu_Items[0].Children_Menu = &RTCSetMenu;
	TimeSetMenu_Items[0].key_function = NULL;
	TimeSetMenu_Items[0].key_function_init = NULL;

	TimeSetMenu_Items[1].Item_Context = "昼夜时间设置";
	TimeSetMenu_Items[1].Item_x = 2;
	TimeSetMenu_Items[1].Item_y = 110;
	TimeSetMenu_Items[1].Item_HelpInfo = "调整昼夜闪烁时间段";
	TimeSetMenu_Items[1].Children_Menu = NULL;
	TimeSetMenu_Items[1].key_function = DayNight_PeriodofTime;//NULL; //DayNight_PeriodofTime;
	TimeSetMenu_Items[1].key_function_init = DayNight_PeriodofTime_Init;
/*
	TimeSetMenu_Items[2].Item_Context = "时段时间设置";
	TimeSetMenu_Items[2].Item_x = 2;
	TimeSetMenu_Items[2].Item_y = 140;
	TimeSetMenu_Items[2].Item_HelpInfo = "增减当前方案时段，调整时段时间";
	TimeSetMenu_Items[2].Children_Menu = NULL;
	TimeSetMenu_Items[2].function = NULL;
*/
	TimeSetMenu.Item_Num = TimeSetMenu_ItemNum;
	TimeSetMenu.Item_Sel = 0;
	TimeSetMenu.Menu_Title = "时间设置";
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
	LEDColorMenu_Items[0].Item_Context = "修改灯组序号";
	LEDColorMenu_Items[0].Item_x = 2;
	LEDColorMenu_Items[0].Item_y = 80;
	LEDColorMenu_Items[0].Item_HelpInfo = "修改灯与输出口的映射关系";
	LEDColorMenu_Items[0].Children_Menu = NULL;
	LEDColorMenu_Items[0].key_function = LCD_LedGPIO;//NULL; //LCD_Crossway1_LedIO; //LCD_LedGPIO_Table;
	LEDColorMenu_Items[0].key_function_init	= LCD_LedGPIO_Init;

	LEDColorMenu_Items[1].Item_Context = "保存灯组接口设置";
	LEDColorMenu_Items[1].Item_x = 2;
	LEDColorMenu_Items[1].Item_y = 100;
	LEDColorMenu_Items[1].Item_HelpInfo = "保存灯组序号到外部存储器";
	LEDColorMenu_Items[1].Children_Menu = NULL;
	LEDColorMenu_Items[1].key_function = NULL; //LCD_Crossway2_LedIO;
	LEDColorMenu_Items[1].key_function_init	= NULL;
	LEDColorMenu_Items[1].custom_function = ledgpio_saveto_e2prom;

	LEDColorMenu_Items[2].Item_Context = "恢复出厂设置";
	LEDColorMenu_Items[2].Item_x = 2;
	LEDColorMenu_Items[2].Item_y = 120;
	LEDColorMenu_Items[2].Item_HelpInfo = "将接口设置恢复为默认出厂状态";
	LEDColorMenu_Items[2].Children_Menu = NULL;
	LEDColorMenu_Items[2].key_function = NULL; //LCD_Crossway3_LedIO;
	LEDColorMenu_Items[2].key_function_init	= NULL;
	LEDColorMenu_Items[2].custom_function = reset_ledgpio_map;
/*	
	LEDColorMenu_Items[3].Item_Context = "灯组四";
	LEDColorMenu_Items[3].Item_x = 2;
	LEDColorMenu_Items[3].Item_y = 140;
	LEDColorMenu_Items[3].Item_HelpInfo = "修改灯组四灯色与IO口的关联";
	LEDColorMenu_Items[3].Children_Menu = NULL;
	LEDColorMenu_Items[3].key_function = NULL; //LCD_Crossway4_LedIO;
	
	LEDColorMenu_Items[4].Item_Context = "灯组五";
	LEDColorMenu_Items[4].Item_x = 2;
	LEDColorMenu_Items[4].Item_y = 160;
	LEDColorMenu_Items[4].Item_HelpInfo = "修改灯组五灯色与IO口的关联";
	LEDColorMenu_Items[4].Children_Menu = NULL;
	LEDColorMenu_Items[4].key_function = NULL; //LCD_Crossway5_LedIO;
	
	LEDColorMenu_Items[5].Item_Context = "灯组六";
	LEDColorMenu_Items[5].Item_x = 2;
	LEDColorMenu_Items[5].Item_y = 180;
	LEDColorMenu_Items[5].Item_HelpInfo = "修改灯组六灯色与IO口的关联";
	LEDColorMenu_Items[5].Children_Menu = NULL;
	LEDColorMenu_Items[5].key_function = NULL; //LCD_Crossway6_LedIO;
*/	
	LEDColorMenu.Item_Num = LEDColorMenu_ItemNum;
	LEDColorMenu.Item_Sel = 0;
	LEDColorMenu.Menu_Title = "灯位灯色";
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
	FlashMenu_Items[0].Item_Context = "闪烁许可设置";
	FlashMenu_Items[0].Item_x = 2;
	FlashMenu_Items[0].Item_y = 80;
	FlashMenu_Items[0].Children_Menu = NULL;
	FlashMenu_Items[0].Item_HelpInfo = "设置允许闪烁的灯色";
	FlashMenu_Items[0].key_function = flash_allowed_main; //Set_FlashAllowence;
	FlashMenu_Items[0].key_function_init = flash_allowed_datainit;
	
	FlashMenu_Items[1].Item_Context = "白天常闪设置";
	FlashMenu_Items[1].Item_x = 2;
	FlashMenu_Items[1].Item_y = 110;
	FlashMenu_Items[1].Children_Menu = NULL;
	FlashMenu_Items[1].Item_HelpInfo = "设置白昼常闪灯色";
	FlashMenu_Items[1].key_function = dayflash_main; //Set_FlashAllowence;
	FlashMenu_Items[1].key_function_init = dayflash_datainit;

	FlashMenu_Items[2].Item_Context = "夜间常闪设置";
	FlashMenu_Items[2].Item_x = 2;
	FlashMenu_Items[2].Item_y = 130;
	FlashMenu_Items[2].Children_Menu = NULL;
	FlashMenu_Items[2].Item_HelpInfo = "设置晚间常闪灯色";
	FlashMenu_Items[2].key_function = nightflash_main; //Set_FlashAllowence;
	FlashMenu_Items[2].key_function_init = nightflash_datainit;

	FlashMenu.Item_Num = FlashMenu_ItemNum;
	FlashMenu.Item_Sel = 0;
	FlashMenu.Menu_Title = "闪烁设置";
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
	PhaseMenu_Items[0].Item_Context = "增加相位";
	PhaseMenu_Items[0].Item_x = 2;
	PhaseMenu_Items[0].Item_y = 80;
	PhaseMenu_Items[0].Item_HelpInfo = "增加一个相位并设置该相位参数";
	PhaseMenu_Items[0].Children_Menu = NULL;
	PhaseMenu_Items[0].key_function = add_phase_main;//NULL; //Add_Phase;
	PhaseMenu_Items[0].key_function_init = add_phase_datainit;

	PhaseMenu_Items[1].Item_Context = "删除相位";
	PhaseMenu_Items[1].Item_x = 2;
	PhaseMenu_Items[1].Item_y = 105;
	PhaseMenu_Items[1].Children_Menu = NULL;
	PhaseMenu_Items[1].Item_HelpInfo = "删除一个已知相位";
	PhaseMenu_Items[1].key_function = del_phase_main;//NULL; //Delete_Phase;
	PhaseMenu_Items[1].key_function_init = del_phase_datainit;

	PhaseMenu_Items[2].Item_Context = "相位设置";
	PhaseMenu_Items[2].Item_x = 2;
	PhaseMenu_Items[2].Item_y = 130;
	PhaseMenu_Items[2].Children_Menu = NULL;
	PhaseMenu_Items[2].Item_HelpInfo = "修改现有相位参数";
	PhaseMenu_Items[2].key_function = set_phase_main;//NULL; //Set_Phase;
	PhaseMenu_Items[2].key_function_init = set_phase_datainit;

	PhaseMenu.Item_Num = PhaseMenu_ItemNum;
	PhaseMenu.Item_Sel = 0;
	PhaseMenu.Menu_Title = "相位菜单";
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
	PeriodOfTimeMenu_Items[0].Item_Context = "新增时段";
	PeriodOfTimeMenu_Items[0].Item_x = 2;
	PeriodOfTimeMenu_Items[0].Item_y = 80;
	PeriodOfTimeMenu_Items[0].Item_HelpInfo = "增加一个时段并设置该时段参数";
	PeriodOfTimeMenu_Items[0].Children_Menu = NULL;
	PeriodOfTimeMenu_Items[0].key_function = add_period_main;//NULL; //Add_PeriodOfTime;
	PeriodOfTimeMenu_Items[0].key_function_init = add_period_datainit;

	PeriodOfTimeMenu_Items[1].Item_Context = "删除时段";
	PeriodOfTimeMenu_Items[1].Item_x = 2;
	PeriodOfTimeMenu_Items[1].Item_y = 105;
	PeriodOfTimeMenu_Items[1].Children_Menu = NULL;
	PeriodOfTimeMenu_Items[1].Item_HelpInfo = "删除一个已知时段";
	PeriodOfTimeMenu_Items[1].key_function = del_period_main;//NULL; //Delete_PeriodOfTime;
	PeriodOfTimeMenu_Items[1].key_function_init = del_period_datainit;

	PeriodOfTimeMenu_Items[2].Item_Context = "时段设置";
	PeriodOfTimeMenu_Items[2].Item_x = 2;
	PeriodOfTimeMenu_Items[2].Item_y = 130;
	PeriodOfTimeMenu_Items[2].Children_Menu = NULL;
	PeriodOfTimeMenu_Items[2].Item_HelpInfo = "修改已知时段参数";
	PeriodOfTimeMenu_Items[2].key_function = set_period_main;//NULL; //Set_PeriodOfTime;
	PeriodOfTimeMenu_Items[2].key_function_init = set_period_datainit;

	PeriodOfTimeMenu.Item_Num = PeriodOfTimeMenu_ItemNum;
	PeriodOfTimeMenu.Item_Sel = 0;
	PeriodOfTimeMenu.Menu_Title = "时段设置";
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
	MainMenu_Items[0].Item_Context = "灯位灯色";
	MainMenu_Items[0].Item_x = 2;
	MainMenu_Items[0].Item_y = 61;
	MainMenu_Items[0].Item_HelpInfo = "更改GPIO引脚与灯色关联关系";
	MainMenu_Items[0].Children_Menu = &LEDColorMenu;
	MainMenu_Items[0].key_function = NULL;
	MainMenu_Items[0].custom_function = NULL;

	MainMenu_Items[1].Item_Context = "闪烁设置";
	MainMenu_Items[1].Item_x = 2;
	MainMenu_Items[1].Item_y = 82;
	MainMenu_Items[1].Children_Menu = &FlashMenu;
	MainMenu_Items[1].Item_HelpInfo = "设置常闪灯色和允许闪烁灯色";
	MainMenu_Items[1].key_function = NULL;
	MainMenu_Items[1].custom_function = NULL;

	MainMenu_Items[2].Item_Context = "时段设置";
	MainMenu_Items[2].Item_x = 2;
	MainMenu_Items[2].Item_y = 103;
	MainMenu_Items[2].Children_Menu = &PeriodOfTimeMenu;
	MainMenu_Items[2].Item_HelpInfo = "添加删除时段或更改时段参数";
	MainMenu_Items[2].key_function = NULL;
	MainMenu_Items[2].custom_function = NULL;

	MainMenu_Items[3].Item_Context = "相位设置";
	MainMenu_Items[3].Item_x = 2;
	MainMenu_Items[3].Item_y = 124;
	MainMenu_Items[3].Children_Menu = &PhaseMenu;
	MainMenu_Items[3].Item_HelpInfo = "添加删除相位或更改相位参数";
	MainMenu_Items[3].key_function = NULL;
	MainMenu_Items[3].custom_function = NULL;
	
	MainMenu_Items[4].Item_Context = "时间设置";
	MainMenu_Items[4].Item_x = 2;
	MainMenu_Items[4].Item_y = 145;
	MainMenu_Items[4].Children_Menu = &TimeSetMenu;
	MainMenu_Items[4].Item_HelpInfo = "更改当前时间、更改昼夜时间和休眠时间";
	MainMenu_Items[4].key_function = NULL;
	MainMenu_Items[4].custom_function = NULL;
	
	MainMenu_Items[5].Item_Context = "方案选择";
	MainMenu_Items[5].Item_x = 2;
	MainMenu_Items[5].Item_y = 166;
	MainMenu_Items[5].Item_HelpInfo = "可选16个已知方案和4个自定义修改方案";
	MainMenu_Items[5].Children_Menu = NULL;
	MainMenu_Items[5].key_function = NULL;
	MainMenu_Items[5].custom_function = NULL;
	
	MainMenu_Items[6].Item_Context = "手动模式";
	MainMenu_Items[6].Item_x = 21;
	MainMenu_Items[6].Item_y = 61;
	MainMenu_Items[6].Item_HelpInfo = "手动调试模式，只在测试用";
	MainMenu_Items[6].Children_Menu = &ManualModeMenu;
	MainMenu_Items[6].key_function = NULL;	  //设定参数用
	MainMenu_Items[6].custom_function = NULL;	//立即执行的函数
	
	MainMenu_Items[7].Item_Context = "特殊工作模式";
	MainMenu_Items[7].Item_x = 21;
	MainMenu_Items[7].Item_y = 82;
	MainMenu_Items[7].Item_HelpInfo = "将修改方案存至自定义修改方案";
	MainMenu_Items[7].Children_Menu = NULL;
	MainMenu_Items[7].key_function = NULL;
	MainMenu_Items[7].custom_function = NULL;

	MainMenu_Items[8].Item_Context = "进入休眠模式";
	MainMenu_Items[8].Item_x = 21;
	MainMenu_Items[8].Item_y = 103;
	MainMenu_Items[8].Item_HelpInfo = "使信号机进入休眠状态";
	MainMenu_Items[8].Children_Menu = NULL;
	MainMenu_Items[8].key_function = NULL;
	MainMenu_Items[8].custom_function = NULL;

	MainMenu_Items[9].Item_Context = "保存设置到PROM";
	MainMenu_Items[9].Item_x = 21;
	MainMenu_Items[9].Item_y = 124;
	MainMenu_Items[9].Item_HelpInfo = "将修改方案存至自定义修改方案";
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
