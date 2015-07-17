#ifndef _MENU_H_
#define _MENU_H_

#include "stm32f10x.h"
#include "key_op.h"
/*
typedef enum
{
	NoMenu = 0,
	MainMenu,  //���˵������Լ��˵�д��һ��
	LEDColor_Menu,
	LEDFlash_Menu,
	LEDPhase_Menu,
	LEDTime_Menu,
	Running_Info,
	Lovers
}Menu_ID;  //�˵�������
*/
//����ID��ȡ��ǰ�˵���Ϣ
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
	uint8_t* Item_Context;	//ÿ��ѡ�����Ϣ������8������
	uint8_t* Item_HelpInfo;
	uint8_t Item_x;
	uint8_t Item_y;
	//uint8_t Item_Index;  //ѡ���Ӧ������
	
	//Menu_ID Parent_ID;
	//Menu_ID Children_ID;
	
	//struct Menu_TypeDef* Parent_Menu;
	struct Menu_TypeDef* Children_Menu;
	//ѡ���Ӧ�Ĺ��ܺ���ָ��
	void (*custom_function)(void);  //��ʱִ�еĺ��������谴������
		 
	void (*key_function)(KEY_TypeDef key);	//ִ���趨�����ĺ���
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

	//�˵�������ͣ�����Ƿ��ػ�˵�����
	void (*menu_frame)(void);
	void (*item_display)(void);
	void (*help_display)(void);

	void (*cur_funptr)(KEY_TypeDef key);  //��ǰִ�в˵�������Ŀ�ĺ���
	void* fundata;	 //���浱ǰ���к����Ĳ������������û����ɶ���

};
//��ID���ģ��������»�ȡ�²˵���Ϣ��������LCD

/*
typedef struct
{
	uint8_t* Item_Context;	//ÿ��ѡ�����Ϣ������8������
	Menu_TypeDef* Parent_Menu;
	Menu_TypeDef* Children_Menu;
	void (*function)(void);	 //ѡ���Ӧ�Ĺ��ܺ���ָ��
}MenuItem_TypeDef;

typedef struct
{
	uint8_t Item_Num;
	uint8_t* Menu_Title;
	MenuItem_TypeDef* Items;
}Menu_TypeDef;
*/
#endif
