#ifndef _MENU_H_
#define _MENU_H_

#include "stm32f10x.h"
#include "key_op.h"
/*
typedef enum
{
	NoMenu = 0,
	MainMenu,  //主菜单与其自己菜单写在一起
	LEDColor_Menu,
	LEDFlash_Menu,
	LEDPhase_Menu,
	LEDTime_Menu,
	Running_Info,
	Lovers
}Menu_ID;  //菜单索引号
*/
//根据ID获取当前菜单信息
/*
typedef void (*function1)(void);
typedef void (*function2)(uint8_t);
typedef void (*function3)(uint8_t, uint8_t);

union Fullfunc{
	function1 func1;
	function2 func2;
	function3 func3;
};
*/
struct window_area_TypeDef{
	uint8_t y_top;
	uint8_t y_bottom;
	uint16_t x_left;
	uint16_t x_right;
};

struct Menu_TypeDef;

struct MenuItem_TypeDef
{
	//uint8_t Item_Num;
	uint8_t* Item_Context;	//每个选项的信息不超过8个汉字
	uint8_t* Item_HelpInfo;
	uint8_t Item_x;
	uint8_t Item_y;
	//uint8_t Item_Index;  //选项对应索引号
	
	//Menu_ID Parent_ID;
	//Menu_ID Children_ID;
	
	//struct Menu_TypeDef* Parent_Menu;
	struct Menu_TypeDef* Children_Menu;
	//选项对应的功能函数指针
	void (*custom_function)(void);  //即时执行的函数，无需按键控制
		 
	void (*key_function)(KEY_TypeDef key);	//执行设定参数的函数
	void (*key_function_init)(void);
	//union Fullfunc func;
};

struct Menu_TypeDef
{
	uint8_t Item_Num;
	//Menu_ID ID;
	uint8_t Item_Sel;
	uint8_t* Menu_Title;
	struct Menu_TypeDef* Parent_Menu;
	struct MenuItem_TypeDef* Items;
	void (*key_function)(KEY_TypeDef key);

	//菜单框架类型：检测是否重绘菜单轮廓
	void (*menu_frame)(void);
	void (*item_display)(void);
	void (*help_display)(void);

	void (*cur_funptr)(KEY_TypeDef key);  //当前执行菜单或者项目的函数
	void* fundata;	 //保存当前运行函数的参数，类型由用户自由定义

};
//若ID更改，必须重新获取新菜单信息，并更新LCD

/*
typedef struct
{
	uint8_t* Item_Context;	//每个选项的信息不超过8个汉字
	Menu_TypeDef* Parent_Menu;
	Menu_TypeDef* Children_Menu;
	void (*function)(void);	 //选项对应的功能函数指针
}MenuItem_TypeDef;

typedef struct
{
	uint8_t Item_Num;
	uint8_t* Menu_Title;
	MenuItem_TypeDef* Items;
}Menu_TypeDef;
*/
#endif
