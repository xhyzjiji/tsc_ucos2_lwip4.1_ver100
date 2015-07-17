#include "stm32f10x.h"
#include "RA8806_CTRL.h"
#include "Delay.h"
#include "Menu.h"
#include "KEY_OP.h"
#include "Ex_Var.h"
#include "stdio.h"

//重写之前反白的选项
void LCD_RefreshItem(void)
{
	uint8_t lcd_flag_temp;
	
	lcd_flag_temp = LCD_FLAG_Using;

LCD_FLAG_Using = 1;

	if(Current_Menu->Items[Current_Menu->Item_Sel].Children_Menu != NULL){
		LCD_WriteText2(Current_Menu->Items[Current_Menu->Item_Sel].Item_x, Current_Menu->Items[Current_Menu->Item_Sel].Item_y, "+ ");
	}
	else if(Current_Menu->Items[Current_Menu->Item_Sel].key_function != NULL){
		LCD_WriteText2(Current_Menu->Items[Current_Menu->Item_Sel].Item_x, Current_Menu->Items[Current_Menu->Item_Sel].Item_y, "\x10 ");
	}
	else{
		LCD_WriteText2(Current_Menu->Items[Current_Menu->Item_Sel].Item_x, Current_Menu->Items[Current_Menu->Item_Sel].Item_y, "  ");
	}
	LCD_WriteText1(Current_Menu->Items[Current_Menu->Item_Sel].Item_Context);

LCD_FLAG_Using = lcd_flag_temp;
if(LCD_FLAG_UpdateIrq == 1 && LCD_FLAG_Using == 0){
	EXTI_GenerateSWInterrupt(EXTI_Line0);
}
}
//设置反白并重写当前选中的选项
void LCD_NorItem(void)
{
	uint8_t lcd_flag_temp;
	
	lcd_flag_temp = LCD_FLAG_Using;

	LCD_Inv();
	if(Current_Menu->Items[Current_Menu->Item_Sel].Children_Menu != NULL){
		LCD_WriteText2(Current_Menu->Items[Current_Menu->Item_Sel].Item_x, Current_Menu->Items[Current_Menu->Item_Sel].Item_y, "+ ");
	}
	else if(Current_Menu->Items[Current_Menu->Item_Sel].key_function != NULL){
		LCD_WriteText2(Current_Menu->Items[Current_Menu->Item_Sel].Item_x, Current_Menu->Items[Current_Menu->Item_Sel].Item_y, "\x10 ");
	}
	else{
		LCD_WriteText2(Current_Menu->Items[Current_Menu->Item_Sel].Item_x, Current_Menu->Items[Current_Menu->Item_Sel].Item_y, "  ");
	}
	LCD_WriteText1(Current_Menu->Items[Current_Menu->Item_Sel].Item_Context);
	LCD_NoInv();
}
//菜单显示框图1
void disp_frame1(void){
	uint8_t lcd_flag_temp;
	
	lcd_flag_temp = LCD_FLAG_Using;

//-----------------------------------------
	LCD_DrawForm(1, 53, 2, 1, 19, 143);
	LCD_DrawForm(1, 196, 1, 2, 38, 20);	
//-----------------------------------------
}
//菜单显示框图2
void disp_frame2(void){
	LCD_AccessLayer(Layer_Selection_DDRAM2);
	LCD_SetAppwindow(0, 0, 39, 239);
	LCD_DrawForm(1, 53, 1, 1, 38, 20);
	LCD_DrawForm(1, 73, 1, 1, 28, 163);
	LCD_DrawForm(29, 73, 1, 1, 10, 163);
}

//显示菜单选项
void disp_item(void)
{
	uint8_t lcd_flag_temp;
	uint8_t i;
	
	lcd_flag_temp = LCD_FLAG_Using;

//-----------------------------------------
	LCD_WriteText2(2, 55, Current_Menu->Menu_Title);

	for(i=0; i<Current_Menu->Item_Num; i++){
		if(Current_Menu->Item_Sel == i)  LCD_Inv();
		if(Current_Menu->Items[i].Children_Menu != NULL){
			LCD_WriteText2(Current_Menu->Items[i].Item_x, Current_Menu->Items[i].Item_y, "+ ");
		}
		else if(Current_Menu->Items[i].key_function != NULL){
			LCD_WriteText2(Current_Menu->Items[i].Item_x, Current_Menu->Items[i].Item_y, "\x10 ");
		}
		else{
			LCD_WriteText2(Current_Menu->Items[i].Item_x, Current_Menu->Items[i].Item_y, "  ");
		}
		LCD_WriteText1(Current_Menu->Items[i].Item_Context);
		if(Current_Menu->Item_Sel == i)  LCD_NoInv();
	}
//-----------------------------------------
}
//显示帮助信息-框架1
void disp_help1(void){
	uint8_t lcd_flag_temp;
	
	lcd_flag_temp = LCD_FLAG_Using;

//-----------------------------------------
	LCD_AccessLayer(Layer_Selection_DDRAM1);
	LCD_SetAppwindow(218, 235, 2, 37);
	LCD_ClearAppWindow();
	LCD_WriteText2(2, 218, Current_Menu->Items[Current_Menu->Item_Sel].Item_HelpInfo);
	LCD_SetAppwindow(0, 239, 0, 39);

	LCD_WriteText2(2, 198, "\x18\x19\x1A\x1B:移动 ENT:选择 ESC:退出");
//-----------------------------------------
}
//显示帮助信息-框架2
void disp_help2(void){
	uint8_t lcd_flag_temp;
	
	lcd_flag_temp = LCD_FLAG_Using;

//-----------------------------------------
	LCD_AccessLayer(Layer_Selection_DDRAM1);
	LCD_SetAppwindow(96, 170, 29, 38);
	LCD_ClearAppWindow();
	LCD_WriteText2(29, 96, Current_Menu->Items[Current_Menu->Item_Sel].Item_HelpInfo);
	LCD_SetAppwindow(0, 239, 0, 39);

	LCD_WriteText2(29, 176, "\x18\x19\x1A\x1B:移动");
	LCD_WriteText2(29, 196, "ENT:选择");
	LCD_WriteText2(29, 216, "ESC:退出");
//-----------------------------------------
}

void KEY_PressMinus(void)
{
	if(KEY_Press>0) KEY_Press --;
	KEY_Buffer[KEY_BufferPointer] = Nothing;
	if(KEY_BufferPointer < KEYBuffer_Size-1){
		KEY_BufferPointer ++;
	}
	else{
		KEY_BufferPointer = 0;
	}	
	//CKey = Nothing;
}
//UP
void KEY1_OP()  //函数直接操作Current_Menu
{
	//KEY_PressMinus();

	LCD_RefreshItem();

	if(Current_Menu->Item_Sel == 0){
		Current_Menu->Item_Sel = Current_Menu->Item_Num-1;
	}
	else{
		Current_Menu->Item_Sel --;
	}

	//重绘反白区
	LCD_NorItem();

//LCD_FLAG_Using = 1;
	//重绘帮助信息
	/*
	LCD_AccessLayer(Layer_Selection_DDRAM1);
	if(Current_Menu == &MainMenu){
		LCD_SetAppwindow(218, 235, 2, 37);
		LCD_ClearAppWindow();
		LCD_WriteText2(2, 218, Current_Menu->Items[Current_Menu->Item_Sel].Item_HelpInfo);
		LCD_SetAppwindow(0, 239, 0, 39);
	}
	else{
		LCD_SetAppwindow(96, 170, 29, 38);
		LCD_ClearAppWindow();
		LCD_WriteText2(29, 96, Current_Menu->Items[Current_Menu->Item_Sel].Item_HelpInfo);
		LCD_SetAppwindow(0, 239, 0, 39);
	}
	*/
	Current_Menu->help_display();
/*
LCD_FLAG_Using = 0;
if(LCD_FLAG_UpdateIrq == 1){
	EXTI_GenerateSWInterrupt(EXTI_Line0);
}*/
}

//DOWN
void KEY2_OP()
{
/*
	KEY_Press --;
	if(KEY_BufferPointer < KEYBuffer_Size-1){
		KEY_BufferPointer ++;
	}
	else{
		KEY_BufferPointer = 0;
	}
*/	
	//KEY_PressMinus();

//LCD_FLAG_Using = 1;

	LCD_RefreshItem();

	if(Current_Menu->Item_Sel == Current_Menu->Item_Num-1){
		Current_Menu->Item_Sel = 0;
	}
	else{
		Current_Menu->Item_Sel ++;
	}

	//重绘反白区
	LCD_NorItem();

//LCD_FLAG_Using = 1;
	//重绘帮助信息
	/*
	LCD_AccessLayer(Layer_Selection_DDRAM1);
	if(Current_Menu == &MainMenu){
		LCD_SetAppwindow(218, 235, 2, 37);
		LCD_ClearAppWindow();
		LCD_WriteText2(2, 218, Current_Menu->Items[Current_Menu->Item_Sel].Item_HelpInfo);
		LCD_SetAppwindow(0, 239, 0, 39);
	}
	else{
		LCD_SetAppwindow(96, 170, 29, 38);
		LCD_ClearAppWindow();
		LCD_WriteText2(29, 96, Current_Menu->Items[Current_Menu->Item_Sel].Item_HelpInfo);
		LCD_SetAppwindow(0, 239, 0, 39);
	}
	*/
	Current_Menu->help_display();
/*
LCD_FLAG_Using = 0;
if(LCD_FLAG_UpdateIrq == 1){
	EXTI_GenerateSWInterrupt(EXTI_Line0);
}*/
//LCD_FLAG_Using = 0;
//if(LCD_FLAG_UpdateIrq == 1){
//	EXTI_GenerateSWInterrupt(EXTI_Line0);
//}
}

void KEY3_OP(void)	//Left
{
/*
	KEY_Press --;
	if(KEY_BufferPointer < KEYBuffer_Size-1){
		KEY_BufferPointer ++;
	}
	else{
		KEY_BufferPointer = 0;
	}
*/
	//KEY_PressMinus();

	LCD_RefreshItem();

	if(Current_Menu == &MainMenu){
		if(Current_Menu->Item_Sel >= 6){
			Current_Menu->Item_Sel -= 6;
		}
	}
	else{	
	}

	//重绘反白区
	LCD_NorItem();

//LCD_FLAG_Using = 1;
	/*
	LCD_AccessLayer(Layer_Selection_DDRAM1);
	if(Current_Menu == &MainMenu){
		LCD_SetAppwindow(218, 235, 2, 37);
		LCD_ClearAppWindow();
		LCD_WriteText2(2, 218, Current_Menu->Items[Current_Menu->Item_Sel].Item_HelpInfo);
		LCD_SetAppwindow(0, 239, 0, 39);
	}
	else{
	}
	*/
	Current_Menu->help_display();
/*
LCD_FLAG_Using = 0;
if(LCD_FLAG_UpdateIrq == 1){
	EXTI_GenerateSWInterrupt(EXTI_Line0);
}*/
}

void KEY4_OP(void)  //RIGHT
{
/*
	KEY_Press --;
	if(KEY_BufferPointer < KEYBuffer_Size-1){
		KEY_BufferPointer ++;
	}
	else{
		KEY_BufferPointer = 0;
	}
*/
	//KEY_PressMinus();

	LCD_RefreshItem();

	if(Current_Menu == &MainMenu){
		if((Current_Menu->Item_Sel+6) < Current_Menu->Item_Num){
			Current_Menu->Item_Sel += 6;
		}
		else{
			Current_Menu->Item_Sel = Current_Menu->Item_Num-1;
		}
	}
	else{
	}

	//重绘反白区
	LCD_NorItem();
/*
LCD_FLAG_Using = 1;

	LCD_AccessLayer(Layer_Selection_DDRAM1);
	if(Current_Menu == &MainMenu){
		LCD_SetAppwindow(218, 235, 2, 37);
		LCD_ClearAppWindow();
		LCD_WriteText2(2, 218, Current_Menu->Items[Current_Menu->Item_Sel].Item_HelpInfo);
		LCD_SetAppwindow(0, 239, 0, 39);
	}
	else{
	}

LCD_FLAG_Using = 0;
if(LCD_FLAG_UpdateIrq == 1){
	EXTI_GenerateSWInterrupt(EXTI_Line0);
}*/
	Current_Menu->help_display();
}

void KEY5_OP(void)	 //Confirm
{
	struct Menu_TypeDef* menu;

	menu = Current_Menu;

	//KEY_PressMinus();

	if(menu->Items[menu->Item_Sel].Children_Menu != NULL){
		Current_Menu = menu->Items[menu->Item_Sel].Children_Menu;
		if(menu->menu_frame != Current_Menu->menu_frame){
			LCD_ClearMenuForm();
			Current_Menu->menu_frame();//LCD_DrawMenuForm();
		}
		LCD_ClearMenuText();		
		Current_Menu->item_display();//LCD_DisplayOtherMenu(Current_Menu);
		Current_Menu->help_display();
	}
	else if(menu->Items[menu->Item_Sel].key_function !=NULL){
		menu->Items[menu->Item_Sel].key_function_init();
		menu->cur_funptr = menu->Items[menu->Item_Sel].key_function;
	}
//keyfunctiondata_calloc_fail:
	else if(menu->Items[menu->Item_Sel].custom_function != NULL){
		menu->Items[menu->Item_Sel].custom_function(); //按需填入用户程序类型
	}
}

void KEY6_OP(void)	 //Return
{
	struct Menu_TypeDef* menu;

	menu = Current_Menu;

	//KEY_PressMinus();

	if(menu->Parent_Menu != NULL){
		menu->Item_Sel = 0;
		menu->cur_funptr = menu->key_function;
		Current_Menu = menu->Parent_Menu;
		if(menu->menu_frame != Current_Menu->menu_frame){
			LCD_ClearMenuForm();
			Current_Menu->menu_frame();//LCD_DrawMenuForm();
		}
		LCD_ClearMenuText();		
		Current_Menu->item_display();//LCD_DisplayOtherMenu(Current_Menu);
		Current_Menu->help_display();
	}
}

void menu_keywork_mode1(KEY_TypeDef key){
	switch(key){
		case KEY1:  KEY1_OP();  break;//delay_us(30000);  break;
		case KEY2:  KEY2_OP();  break;//delay_us(30000);  break;
		case KEY3:	KEY3_OP();  break;//delay_us(30000);  break;
		case KEY4:	KEY4_OP();  break;//delay_us(30000);  break;
		case KEY5:  KEY5_OP();  break;
		case KEY6:  KEY6_OP();  break;
		//case KEY3_Cont: break;
		case KEY1_Cont: if(Current_CutDown<99&&Current_CutDown>6) Current_CutDown++;  //KEY_PressMinus();  break;
		case KEY2_Cont: if(Current_CutDown>7) Current_CutDown--;  //KEY_PressMinus();  break;
		case KEY4_Cont: break;
		case Nothing: return;
		default:  printf("Unknown KEY\n");  break;
	}
	KEY_PressMinus();
}

void menu_keywork_mode2(KEY_TypeDef key){
	switch(key){
		case KEY1:  KEY1_OP();  break;//delay_us(30000);  break;
		case KEY2:  KEY2_OP();  break;//delay_us(30000);  break;
		//case KEY3:	KEY3_OP();  break;//delay_us(30000);  break;
		//case KEY4:	KEY4_OP();  break;//delay_us(30000);  break;
		case KEY5:  KEY5_OP();  break;
		case KEY6:  KEY6_OP();  break;
		//case KEY1_Cont: break;
		case KEY1_Cont: if(Current_CutDown<99&&Current_CutDown>6) Current_CutDown++;  KEY_PressMinus();  break;
		case KEY2_Cont: if(Current_CutDown>7) Current_CutDown--;  KEY_PressMinus();  break;
		//case KEY4_Cont: break;
		case Nothing: return;
		default:  printf("Unknown KEY\n");  break;
	}
	KEY_PressMinus();
}
