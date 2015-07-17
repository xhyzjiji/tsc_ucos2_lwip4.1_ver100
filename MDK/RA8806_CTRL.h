#ifndef _RA8806_CTRL_H_
#define _RA8806_CTRL_H_

#include "stm32f10x.h"
#include "Menu.h"
#include "Traffic_Param.h"
#include "time.h"

//使用8080Intel总线模式
#define BUSY GPIO_Pin_8
#define INT GPIO_Pin_9
#define CS GPIO_Pin_10
#define RS GPIO_Pin_11
#define WR GPIO_Pin_12
#define RD GPIO_Pin_13
#define RST GPIO_Pin_14
#define DB (GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7)
#define LCDPort GPIOE

//选择以下功能，该位将被置位
//第一次设置功能时，功能选项之间使用或，取消功能时将寄存器的数据与该功能的反
#define WLCR	0x00						  	 //芯片控制寄存器
#define Power_Mode_SLEEP      	((uint8_t)0x80)  //睡眠模式(反选为正常模式)
#define Linear_Decode_Mode    	((uint8_t)0x40)  //使用自定义ROM地址(反选为BIG5/GB ROM地址)
#define Software_Reset        	((uint8_t)0x20)  //除DDRAM数据不变外，其他寄存器被设置为默认值
#define Text_Mode_Selection  	((uint8_t)0x08)  //文字模式(反选为绘图模式)
#define Display_ON			  	((uint8_t)0x04)  //开启显示
#define Blink_Mode_Selection  	((uint8_t)0x02)  //整屏闪烁
#define Inverse_Mode_Selection	((uint8_t)0x01)	 //整屏反白

#define MISC	0x01							 //杂项寄存器
#define Eliminating_Flicker_ON 	((uint8_t)0x80)  //雪花消除模式
#define Pin_CLK_OUT				((uint8_t)0x40)  //CLK_OUT引脚输出模式，0：正常模式/1：睡眠模式
												 //(反选输出系统频率)
#define Busy_Polarity_High		((uint8_t)0x20)  //设置系统忙时，BUSY引脚输出高电平(反选是系统忙BUSY输出0)
#define Interrupt_Polarity_High	((uint8_t)0x10)  //设置中断输出电平为高电平(反选时中断触发后，INT引脚输出低电平)
#define Driver_Clock_Div8 		((uint8_t)0x00)  //XCK频率选择
#define Driver_Clock_Div4		((uint8_t)0x04)  
#define Driver_Clock_Div2		((uint8_t)0x08)
#define Driver_Clock_Div1		((uint8_t)0x0C)
#define Seg_Scan_Direction		((uint8_t)0x02)	 //SEG扫描方向319-0(反选为0-319)
#define COM_Scan_Direction		((uint8_t)0x01)	 //COM扫描方向239-0(反选为0-239)

#define ADSR	0x03
#define Scroll_Function_Pending	((uint8_t)0x80)  //卷动功能暂停
#define Bit_Order				((uint8_t)0x08)	 //反向输出数据DB7-DB0
#define SCR_DIR					((uint8_t)0x04)	 //卷动方向，SCR_HV=1由下至上(反之为上至下)
												 //SCR_HV=0由右至左(反之为左至右)
#define SCR_HV					((uint8_t)0x02)  //垂直卷动/水平卷动
#define	SCR_EN					((uint8_t)0x01)  //卷动功能开启/关闭

#define INTR	0x0F						//中断设置与中断标志位

#define WCCR	0x10						//光标控制寄存器
#define CUS_INC					((uint8_t)0x80)  //禁止对DDRAM进行读写操作时，光标位自动加1
#define FULL_OFS				((uint8_t)0x40)  //全型字与半型字混合显示，汉字以全型字位置对齐
#define Inverse_Bit_Order		((uint8_t)0x20)  //输入DDRAM的数据被取反
#define	Bold_Font				((uint8_t)0x10)  //文字模式下有效，显示为粗体字
#define T90DEG					((uint8_t)0x08)  //文字旋转90度
#define Cursor_Display_ON		((uint8_t)0x04)  //显示光标
#define Cursor_Blink			((uint8_t)0x02)  //光标闪烁

#define CHWI    0x11						//光标高度与行距new(have change)

#define MAMR	0x12						//图层缓存模式
#define Cursor_Direction_Vert	((uint8_t)0x80)	 //光标垂直移动
#define	Display_Layer_Gray		((uint8_t)0x00)  //灰阶模式
#define Display_Layer_DDRAM1	((uint8_t)0x10)  //单层DDRAM1
#define Display_Layer_DDRAM2	((uint8_t)0x20)  //单层DDRAM2
#define Display_Layer_DDRAMs	((uint8_t)0x30)  //双层显示
#define Display_Layer_640240	((uint8_t)0x60)  //扩展模式640*240
#define Display_Layer_320480	((uint8_t)0x60)  //扩展模式320*480
#define TwoLayer_Rule_OR		((uint8_t)0x00)  //双层DDRAM按位或
#define TwoLayer_Rule_XOR		((uint8_t)0x04)  //双层DDRAM按位异或
#define TwoLayer_Rule_NOR		((uint8_t)0x08)  //双层DDRAM按位同或
#define TwoLayer_Rule_AND		((uint8_t)0x0C)  //双层DDRAM按位与
#define Layer_Selection_CGRAM	((uint8_t)0x00)  //选择CGRAM为当前操作图层
#define Layer_Selection_DDRAM1	((uint8_t)0x01)  //选择DDRAM1为当前操作图层
#define Layer_Selection_DDRAM2	((uint8_t)0x02)  //选择DDRAM2为当前操作图层
#define Layer_Selection_DDRAMs	((uint8_t)0x03)  //同时选择DDRAM1和DDRAM2为当前操作图层

#define CGRAM_odd				((uint8_t)0x00)
#define CGRAM_even				((uint8_t)0x10)

//工作窗口大小为自定义，显示窗口大小为由屏幕像素计算出来
#define AWRR	0x20						//工作窗口右边界(计算方法：边界像素(8的倍数)/8-1)
#define DWWR	0x21						//显示窗口宽度(计算方法：屏幕像素/8-1)
#define AWBR	0x30						//工作窗口下边界(边界像素-1)
#define DWHR	0x31						//显示窗口高度(屏幕像素-1)new(have change)
#define AWLR	0x40						//工作窗口左边界
#define AWTR	0x50						//工作窗口上边界

#define CURX	0x60						//设置光标水平SEG位置(计算方法：水平像素位置(8的倍数)/8-1)new(have change)
											//当操作CGRAM图层时，该坐标为写入数据的位地址
#define BGSG	0x61						//卷动窗口左边界(计算方法与其他窗口类似)
#define EDSG    0x62						//卷动窗口右边界new
#define CURY	0x70						//设置光标垂直COM位置(像素位置-1)new(have change)	
											//当操作CGRAM图层时，该寄存器[3:0]位表示哪个字被创造，CGRAM最多支持创造16个字
#define BGCM	0x71						//卷动窗口上边界
#define EDCM	0x72						//卷动窗口下边界
#define	BTMR	0x80						//光标闪烁与卷动周期
#define ITCR	0x90						//空闲时间计数new(have change)
#define KSCR1	0xA0						//键盘扫描控制1
#define KSCR2	0xA1						//键盘扫描控制2
#define KSDR0  	0xA2						//new(have change)
#define KSDR1  	0xA3
#define KSDR2  	0xA4
#define MWCR	0xB0						//内存写入指令，在写入CGRAM/DDRAM前必须写入该指令new(have change)
#define MRCR	0xB1						//new
#define TPCR	0xC0
#define TPXR	0xC1
#define TPYR	0xC2
#define TPZR	0xC3
#define PCR     0xD0						//new(have change)
#define PDCR    0xD1						//new

#define PNTR	0xE0						//写入DDRAM的数据，用于填满工作窗口的预备数据
#define FNCR	0xF0
#define ISO8859 				((uint8_t)0x80)  //使用国际字符
#define Fill_AW					((uint8_t)0x08)  //内存清除，使用PNTR内容填满工作窗口，在完成操作后自动置0
#define ASCII_Smart_Decode		((uint8_t)0x00)  //智能实现半字全型字解码
#define ASCII_Common_Decode		((uint8_t)0x04)  //均解释为半字型(无法显示中文)
#define ASCII_Block1			((uint8_t)0x00)  //选择ASCII表块区
#define ASCII_Block2			((uint8_t)0x01)
#define ASCII_Block3			((uint8_t)0x02)
#define ASCII_Block4			((uint8_t)0x03)

#define FVHT	0xF1						//字符放大
#define WidthX1					((uint8_t)0x00)  //字符宽度放大倍数
#define WidthX2					((uint8_t)0x40)
#define WidthX3					((uint8_t)0x80)
#define WidthX4					((uint8_t)0xC0)
#define HeightX1				((uint8_t)0x00)  //字符高度放大倍数
#define HeightX2				((uint8_t)0x10)
#define HeightX3				((uint8_t)0x20)
#define HeightX4				((uint8_t)0x30)

void LCD_WaitForIDLE_Low(void);
void LCD_WaitForIDLE_High(void);

uint8_t LCD_ReadData(void);
void LCD_WriteData(uint8_t data);
void LCD_WriteCommand(uint8_t cmd);

void LCD_Reset(void);
void LCD_ResetBuffers(void);

uint8_t LCD_ReadREG(uint8_t RegAddr);
void LCD_WriteREG(uint8_t RegAddr, uint8_t data);

uint8_t LCD_ReadSTATUSReg(void);

void LCD_Inv(void);
void LCD_NoInv(void);

//void LCD_EliminateFlickerON(void);
void LCD_EliminateFlickerOFF(void);

void LCD_TextHybridMode(void);
void LCD_TextAsciiMode(void);
void LCD_TextMode(void);
void LCD_PicsMode(void);

void LCD_IconInit(void);
void LCD_Init(void);

void LCD_ClearAppWindow(void);
void LCD_FillAppWindow(uint8_t ascii);
void LCD_SetAppwindow(uint8_t toppixel, uint8_t bottompixel, uint8_t leftpixel, uint8_t rightpixel);

void LCD_SetCursor(uint8_t x, uint8_t y);
void LCD_ShowLayer(uint8_t layer);
void LCD_AccessLayer(uint8_t layer);
void LCD_DDRAMs_ShowRule(uint8_t rule);
void LCD_DrawPics(uint8_t x, uint8_t y, uint8_t hpixel, uint8_t vpixel, uint8_t* pic);
void LCD_DrawRectangle(uint8_t x, uint8_t y, uint8_t width, uint8_t height);
void LCD_DrawForm(uint8_t x, uint8_t y, uint8_t row, uint8_t rank, uint8_t width, uint8_t height);
void LCD_WriteChar1(uint8_t ch);
void LCD_WriteChar2(uint8_t x, uint8_t y, uint8_t ch);
void LCD_WriteText1(uint8_t* str);
void LCD_WriteText2(uint8_t x, uint8_t y, uint8_t* str);
void LCD_TextBold(FunctionalState state);
void LCD_SetCursorHeight(uint8_t curheight);
void LCD_SetWordInterval(uint8_t line_width);
void LCD_EnlargeText(uint8_t hmul, uint8_t vmul);
void LCD_AsciiBlock(uint8_t standard, uint8_t blockarea);
void LCD_CGRAM_CreatWord(uint8_t word_number, uint8_t* cg);
void LCD_CGRAM_CreatHalfWord(uint8_t word_number, uint8_t odd_even, uint8_t* cg);
void LCD_WriteWordFromCGRAM(uint8_t x, uint8_t y, uint8_t word_number);
void LCD_WriteHalfWordFromCGRAM(uint8_t x, uint8_t y, uint8_t word_number, uint8_t odd_even);

void LCD_NorString(uint8_t x, uint8_t y, uint8_t* str, FunctionalState invstate);
void LCD_DisplayMainPannel(void);
void LCD_InterfaceInit(void);
void LCD_ClearMenuText(void);
void LCD_ClearItemText(void);
void LCD_ClearItemAllText(void);
void LCD_ClearMenuForm(void);
void LCD_DrawMenuForm(void);
void LCD_DisplayOtherMenu(struct Menu_TypeDef* menu);
void LCD_UpdateInfo(void);
//void LCD_RefreshItem(void);
//void LCD_NorItem(void);
typedef enum{
	lcdapp_ready = 0x00U,
	lcdapp_saved,
	lcdapp_notsaved,
	lcdapp_error_repeat,
	lcdapp_error_full,
	lcdapp_wait,
	lcdapp_locked,
	lcdapp_unlocked,
	lcdapp_cmd,
	lcdapp_return,
	lcdapp_codeerror,
	lcdapp_auto,
	lcdapp_set,
	lcdapp_need_confirm
}lcd_appstate;

/**********************************************************
公共密码解锁函数
**********************************************************/
#define CODE_LEN 6
typedef struct{
	uint8_t code[CODE_LEN];
	uint8_t code_sel;		
}menulock_fundata;
void MenuLock_Update(uint8_t sel, menulock_fundata* menucode);
void MenuLock_init(uint8_t sel, uint8_t* code);
void Menu_Lock(KEY_TypeDef key, lcd_appstate* state, uint8_t* sel, menulock_fundata* menucode);
/**********************************************************
手动模式-路灯测试函数
**********************************************************/
void LED_Check_datainit(void);
void LED_Check_Update(uint8_t LED_Sn);
void LED_Check_interfaceinit(void);
void LED_Check(KEY_TypeDef key);
void LED_Check_main(KEY_TypeDef key);
/**********************************************************
手动模式-调整当前相位时间
**********************************************************/
void ManualMode_CutDown_init(void);
void ManualMode_CutDown_interfaceinit(void);
void ManualMode_CutDown(KEY_TypeDef key);//void ManualMode_CutDown(void);
void ManualMode_CutDown_main(KEY_TypeDef key);
/**********************************************************
手动模式-循环运行指定相位
**********************************************************/
void ManualMode_TimePhase_datainit(void);
void ManualMode_TimePhase_Update(uint8_t sel, uint8_t TimeTable_temp, uint8_t PhaseTable_temp);
void ManualMode_TimePhase_interface(void);
void ManualMode_TimePhase(KEY_TypeDef key);
void ManualMode_TimePhase_main(KEY_TypeDef key);
/****************************************************************
RTC设置菜单
****************************************************************/
void RTC_SetDate_Init(void);
void RTC_SetDate_Update(uint8_t sel, struct tm time);
void RTC_SetDate(KEY_TypeDef key);//void RTC_SetDate(void);
/******************************************************
保存当前时间到外部RTC
******************************************************/
void time_saveto_ds1302(void);
/**********************************************************************
恢复灯组接口设置到出厂状态
**********************************************************************/
void reset_ledgpio_map(void);
/******************************************************
昼夜时段设置
******************************************************/
void DayNight_PeriodofTime_Update(uint8_t sel, uint8_t hour, uint8_t min, uint8_t sec);
void DayNight_PeriodofTime_Init(void);
void DayNight_PeriodofTime(KEY_TypeDef key);//void DayNight_PeriodofTime(void);
/****************************************************************
灯号--路口led映射关系设置菜单
****************************************************************/
void LCD_LedGPIO_Update(uint8_t sel, uint8_t* PinNum);
void LCD_LedGPIO_Init(void);
void LCD_LedGPIO(KEY_TypeDef key);
/**********************************************************************
保存灯组接口设置
**********************************************************************/
void ledgpio_saveto_e2prom(void);
/**********************************************************************
闪烁许可设置
**********************************************************************/
void flashfunc_update(uint8_t sel, uint8_t crossway, uint16_t flashdata);	//公用
void flash_allowed_datainit(void);
void Set_FlashAllowence_Interface(void);
void Set_FlashAllowence(KEY_TypeDef key);
void flash_allowed_main(KEY_TypeDef key);
/**********************************************************************
日昼常闪设置
**********************************************************************/
void dayflash_datainit(void);
void dayflash_Interface(void);
void dayflash(KEY_TypeDef key);
void dayflash_main(KEY_TypeDef key);
/**********************************************************************
晚间常闪设置(与日昼常闪设置非常相似，直接套用)
**********************************************************************/
void nightflash_datainit(void);
void nightflash_Interface(void);
void nightflash(KEY_TypeDef key);
void nightflash_main(KEY_TypeDef key);
/**********************************************************************
相位表设置
**********************************************************************/
void set_phase_datainit(void);
void set_phase_update(uint8_t crossway, uint8_t sel, uint16_t crossway_phase, uint8_t phase_sel, uint8_t period_sel, uint8_t cutdown);
void set_phase_interface(void);
void set_phase(KEY_TypeDef key);
void set_phase_main(KEY_TypeDef key);
/**********************************************************************
删除相位
**********************************************************************/
void del_phase_datainit(void);
void del_phase_update(uint8_t crossway, uint8_t sel, uint16_t crossway_phase, uint8_t phase_sel, uint8_t period_sel, uint8_t cutdown);
void del_phase_interface(void);
void del_phase(KEY_TypeDef key);
void del_phase_main(KEY_TypeDef key);
/**********************************************************************
添加相位
**********************************************************************/
void add_phase_datainit(void);
void add_phase_update(uint8_t sel, uint8_t period_sel, uint8_t phase_sel, uint8_t op);
void add_phase_interface(void);
void add_phase(KEY_TypeDef key);
void add_phase_main(KEY_TypeDef key);
/**********************************************************************
时段表设置
**********************************************************************/
void set_period_datainit(void);
void set_period_update(uint8_t sel, uint8_t period_sel, struct ShortTime time);
void set_period_interface(void);
void set_period(KEY_TypeDef key);
void set_period_main(KEY_TypeDef key);
/**********************************************************************
删除时段表
**********************************************************************/
void del_period_datainit(void);
void del_period_update(struct ShortTime start_time, struct ShortTime end_time, uint8_t sel, uint8_t period_sel);
void del_period_interface(void);	  
void del_period(KEY_TypeDef key);
void del_period_main(KEY_TypeDef key);
/**********************************************************************
添加时段表
**********************************************************************/
void add_period_datainit(void);
void add_period_update(uint8_t sel, ShortTime_TypeDef time);
void add_period_interface(void);
void add_period(KEY_TypeDef key);
void add_period_main(KEY_TypeDef key);
//void Delete_PhaseInfo_Update(uint8_t sel, uint8_t crossway, uint8_t PeriodOfTimeNum, uint8_t PhaseNum, uint16_t phase, uint8_t cutdown);
//void Delete_Phase(void);

//void Add_PhaseInfo_Update(uint8_t periodoftime, uint8_t phasenum, uint8_t sel, uint8_t options);
//void Add_Phase(void);

//void Set_PeriodOfTime_Update(uint8_t sel, uint8_t periodoftime, struct ShortTime time);
//void Set_PeriodOfTime(void);

//void Delete_PeriodOfTime_Update(struct ShortTime start_time, struct ShortTime end_time, uint8_t sel, uint8_t periodoftime);
//void Delete_PeriodOfTime(void);

//void Add_PeriodOfTime_Update(uint8_t sel, ShortTime_TypeDef time);
//void Add_PeriodOfTime(void);

#define BUSY_CheckForHigh 	while(GPIO_ReadInputDataBit(LCDPort, BUSY)==(uint8_t)Bit_SET) 
#define BUSY_CheckForLow 	while(GPIO_ReadInputDataBit(LCDPort, BUSY)==(uint8_t)Bit_RESET) 

#define CN_EN_HybridDisplay	LCD_WriteREG(FNCR, LCD_ReadREG(FNCR)&0xFB)
#define EN_AllDisplay		LCD_WriteREG(FNCR, LCD_ReadREG(FNCR)&0xFB | ASCII_Common_Decode)

#endif
