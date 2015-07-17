#include "stm32f10x.h"
#include "Delay.h"
#include "RA8806_CTRL.h"
#include "Ex_Var.h"
#include "string.h"
#include "time.h"
#include "ds1302_op.h"
#include "at24c64_op.h"
#include "TIME_OP.h"
#include "traffic_param.h"
#include "LED_OP.h"
#include "stdlib.h"
#include "includes.h"

#define MOVE 0

#define SET_RS 		GPIO_SetBits(LCDPort, RS)
#define RESET_RS 	GPIO_ResetBits(LCDPort, RS)
#define SET_CS 		GPIO_SetBits(LCDPort, CS)
#define RESET_CS 	GPIO_ResetBits(LCDPort, CS)
#define SET_WR 		GPIO_SetBits(LCDPort, WR)
#define RESET_WR 	GPIO_ResetBits(LCDPort, WR)
#define SET_RD 		GPIO_SetBits(LCDPort, RD)
#define RESET_RD 	GPIO_ResetBits(LCDPort, RD)
#define SET_RST 	GPIO_SetBits(LCDPort, RST)
#define RESET_RST 	GPIO_ResetBits(LCDPort, RST)

const uint8_t Night_Icon[32] = {0x00, 0x00, 0x21, 0x22, 0x34, 0x32, 0x39, 0x38, 0x1C, 0x1E, 0x0F, 0x07, 0x03, 0x00, 0x00, 0x00, \
								0x00, 0x00, 0x00, 0x80, 0x40, 0x80, 0x10, 0x28, 0x44, 0x28, 0x10, 0xC0, 0xF0, 0xFC, 0x00, 0x00};
const uint8_t Manual_Icon[32] = {0x00, 0x02, 0x05, 0x05, 0x05, 0x05, 0x05, 0x35, 0x2D, 0x2C, 0x24, 0x10, 0x08, 0x04, 0x04, 0x00, \
								 0x00, 0x00, 0x00, 0x00, 0x80, 0x60, 0x50, 0x58, 0x54, 0x04, 0x04, 0x08, 0x08, 0x30, 0x20, 0x00};
								 
const uint8_t Number_Table[10] = {'0','1','2','3','4','5','6','7','8','9'};
const uint8_t* Weekday_Table[11] = {"日", "一", "二", "三", "四", "五", "六", "七", "八", "九", "十"};
const uint8_t* Tick_Cross_Table[2] = {"×", "√"};
const uint8_t* LedColor_Table[3] = {"绿", "红", "黄"};

const uint8_t Func_Run_PeriodPhase_Place[2][2] = {{18, 120}, {18, 138}};
const uint8_t Func_RTCDate_Place[12][2] = {{9, 120}, {10, 120}, {12, 120}, {13, 120}, {15, 120}, {16, 120}, {9, 150}, {10, 150}, {12, 150}, {13, 150}, {15, 150}, {16, 150}};
const uint8_t Func_DayNight_Place[6][2] = {{10, 130}, {11, 130}, {13, 130}, {14, 130}, {16, 130}, {17, 130}};
const uint8_t Func_LedGPIO_Place[17][2] = {{6, 75}, {11, 111}, {11, 129}, {11, 147}, {11, 165}, {11, 183}, {11, 201}, {16, 111}, {16, 129}, {16, 147}, {16, 165}, {21, 111}, {21, 129}, {21, 147}, {21, 165}, {21, 183}, {21, 201}};
const uint8_t Func_SetFlash_Place[17][2] = {{11, 111}, {11, 129}, {11, 147}, {11, 165}, {11, 183}, {11, 201}, {16, 111}, {16, 129}, {16, 147}, {16, 165}, {21, 111}, {21, 129}, {21, 147}, {21, 165}, {21, 183}, {21, 201}, {7, 75}};
const uint8_t Func_PhaseInfo_Place[7][2] = {{8, 126}, {18, 126}, {8, 146}, {18, 146}, {8, 166}, {18, 166}, {15, 186}};
const uint8_t Func_AddPhase_Place[4][2] = {{11, 150}, {22, 150}, {4, 180}, {16, 180}};
const uint8_t Func_SetPeriodOfTime_Place[4][2] = {{18, 100}, {11, 120}, {14, 120}, {17, 120}};
/*
typedef enum{
	lcdapp_ready = 0x00U,
	lcdapp_saved,
	lcdapp_notsaved,
	lcdapp_error,
	lcdapp_wait,
	lcdapp_locked,
	lcdapp_unlocked,
	lcdapp_cmd,
	lcdapp_return,
	lcdapp_codeerror,
	lcdapp_auto
}lcd_appstate;
*/
void LCD_WaitForIDLE_Low(void)
{
	//直接读取BUSY电平并等待系统闲置
	while(GPIO_ReadInputDataBit(LCDPort, BUSY)==(uint8_t)Bit_RESET);
}

//选择使用忙信号为高电平
void LCD_WaitForIDLE_High(void)
{
	while(GPIO_ReadInputDataBit(LCDPort, BUSY)==(uint8_t)Bit_SET);	
}

//状态寄存器没有地址，直接对LCD进行读操作
uint8_t LCD_ReadSTATUSReg(void)
{
	uint8_t rtn;
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin = DB;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(LCDPort, &GPIO_InitStructure);

//TIM_ITConfig(TIM2, TIM_IT_Update, DISABLE);
//RTC_ITConfig(RTC_IT_ALR | RTC_IT_SEC, DISABLE);

	GPIO_ResetBits(LCDPort, CS);
	GPIO_SetBits(LCDPort, RS);
	GPIO_SetBits(LCDPort, WR | RD);
	GPIO_ResetBits(LCDPort, RD);
	//delay_100ns(2);
	rtn = (uint8_t)(GPIO_ReadInputData(LCDPort)&0x00FF);
	GPIO_SetBits(LCDPort, RD);
	GPIO_SetBits(LCDPort, CS);

//TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
//RTC_ITConfig(RTC_IT_ALR | RTC_IT_SEC, ENABLE);

	GPIO_InitStructure.GPIO_Pin = DB;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(LCDPort, &GPIO_InitStructure);

	return rtn;
}

uint8_t LCD_ReadData(void)
{
	uint8_t rtn;
	GPIO_InitTypeDef GPIO_InitStructure;

	BUSY_CheckForLow;
//	while((LCD_ReadSTATUSReg()&0x80)!=0x00);
	
	GPIO_InitStructure.GPIO_Pin = DB;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(LCDPort, &GPIO_InitStructure);

//TIM_ITConfig(TIM2, TIM_IT_Update, DISABLE);
//RTC_ITConfig(RTC_IT_ALR | RTC_IT_SEC, DISABLE);

	GPIO_SetBits(LCDPort, WR | RD);
	GPIO_ResetBits(LCDPort, CS);
	GPIO_ResetBits(LCDPort, RS);
	
	GPIO_ResetBits(LCDPort, RD);
	//delay_100ns(2);
	rtn = (uint8_t)(GPIO_ReadInputData(LCDPort)&0x00FF);
	GPIO_SetBits(LCDPort, RD);
	GPIO_SetBits(LCDPort, CS);

//TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
//RTC_ITConfig(RTC_IT_ALR | RTC_IT_SEC, ENABLE);

	GPIO_InitStructure.GPIO_Pin = DB;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(LCDPort, &GPIO_InitStructure);
	
	return rtn;
}

void LCD_WriteData(uint8_t data)
{
	BUSY_CheckForLow;
//	while((LCD_ReadSTATUSReg()&0x80)!=0x00);

//TIM_ITConfig(TIM2, TIM_IT_Update, DISABLE);
//RTC_ITConfig(RTC_IT_ALR | RTC_IT_SEC, DISABLE);
	GPIO_SetBits(LCDPort, WR | RD);
	GPIO_Write(LCDPort, (GPIO_ReadOutputData(LCDPort)&0xFF00|data));
	GPIO_ResetBits(LCDPort, CS);	  //使能控制芯片
	//GPIO_SetBits(LCDPort, WR | RD);	  //屏蔽读写操作
	GPIO_ResetBits(LCDPort, RS);	  //选择数据
	GPIO_ResetBits(LCDPort, WR);	  //选择写入
	
	//GPIO_Write(LCDPort, (GPIO_ReadOutputData(LCDPort)&0xFF00|data));   //准备写入的数据内容

	//delay_100ns(2);
	
	GPIO_SetBits(LCDPort, WR);		  //屏蔽写操作
	GPIO_SetBits(LCDPort, CS);		  //失能控制芯片
	//delay_100ns(5);

//TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
//RTC_ITConfig(RTC_IT_ALR | RTC_IT_SEC, ENABLE);
}

void LCD_WriteCommand(uint8_t cmd)
{
	BUSY_CheckForLow;
//	while((LCD_ReadSTATUSReg()&0x80)!=0x00);

//TIM_ITConfig(TIM2, TIM_IT_Update, DISABLE);
//RTC_ITConfig(RTC_IT_ALR | RTC_IT_SEC, DISABLE);
	GPIO_SetBits(LCDPort, WR | RD);
	GPIO_Write(LCDPort, (GPIO_ReadOutputData(LCDPort)&0xFF00|cmd));
	GPIO_ResetBits(LCDPort, CS);	  //使能控制芯片
	
	//GPIO_SetBits(LCDPort, WR | RD);	  //屏蔽读写操作
	GPIO_SetBits(LCDPort, RS);	  //选择命令
	GPIO_ResetBits(LCDPort, WR);	  //使能控制芯片
	//GPIO_Write(LCDPort, (GPIO_ReadOutputData(LCDPort)&0xFF00|cmd));   //准备写入的数据内容
	
	//delay_100ns(2);					  //数据保持
	GPIO_SetBits(LCDPort, WR);		  //屏蔽写操作
	GPIO_SetBits(LCDPort, CS);		  //失能控制芯片
	//delay_100ns(5);

//TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
//RTC_ITConfig(RTC_IT_ALR | RTC_IT_SEC, ENABLE);
}

void LCD_Reset(void)
{
	GPIO_ResetBits(LCDPort, CS);
	GPIO_SetBits(LCDPort, WR | RD);
	GPIO_ResetBits(LCDPort, RST);
	delay_us(15000);
	GPIO_SetBits(LCDPort, RST);
	GPIO_SetBits(LCDPort, CS);
}

uint8_t LCD_ReadREG(uint8_t RegAddr)
{
	uint8_t rtn;
	
	LCD_WriteCommand(RegAddr);
	rtn = LCD_ReadData();

	return rtn;	
}

//被调用
void LCD_WriteREG(uint8_t RegAddr, uint8_t data)
{
	LCD_WriteCommand(RegAddr);
	LCD_WriteData(data);
}

//被调用
void LCD_Inv(void)
{
	LCD_WriteREG(WCCR, LCD_ReadREG(WCCR) | Inverse_Bit_Order);
}

//被调用
void LCD_NoInv(void)
{
	LCD_WriteREG(WCCR, LCD_ReadREG(WCCR) & (~Inverse_Bit_Order));
	delay_100ns(1);
}
//在文本方式下，无法使用消除雪花模式，在图形模式下可以用
/*
void LCD_EliminateFlickerON(void)
{
	LCD_WriteREG(MISC, LCD_ReadREG(MISC) | Eliminating_Flicker_ON);
}
*/

//被调用
void LCD_EliminateFlickerOFF(void)
{
	LCD_WriteREG(MISC, LCD_ReadREG(MISC) & (~Eliminating_Flicker_ON));
}

//被调用
void LCD_ResetBuffers(void)	 //不影响DDRAM内容
{
	LCD_WriteREG(WLCR, Software_Reset);
	delay_us(10);
	BUSY_CheckForHigh;
	LCD_WriteREG(WLCR, LCD_ReadREG(WLCR)&(~Software_Reset));
}

//被调用
void LCD_TextHybridMode(void)
{
	LCD_EliminateFlickerOFF();
	LCD_WriteREG(WLCR, LCD_ReadREG(WLCR)|Text_Mode_Selection);
	CN_EN_HybridDisplay;
}

//被调用
void LCD_TextAsciiMode(void)
{
	LCD_EliminateFlickerOFF();
	LCD_WriteREG(WLCR, LCD_ReadREG(WLCR)|Text_Mode_Selection);
	EN_AllDisplay;
}

//被调用
void LCD_TextMode(void)
{
	LCD_WriteREG(WLCR, LCD_ReadREG(WLCR)|Text_Mode_Selection);
	CN_EN_HybridDisplay;
	LCD_EliminateFlickerOFF();
	delay_100ns(5);
}

//被调用
void LCD_PicsMode(void)
{
	LCD_WriteREG(WLCR, LCD_ReadREG(WLCR)&(~Text_Mode_Selection));
	//LCD_EliminateFlickerON();
	delay_100ns(5);
}

//被调用
void LCD_ClearAppWindow(void)
{  
	LCD_PicsMode();							   //选择绘图模式
	LCD_EliminateFlickerOFF();				   //取消雪花消除功能
	LCD_WriteREG(PNTR, 0x00);				   //填充当前操作图层内容，以绘图形式填写，1为该像素为黑，0为该像素为白
	//LCD_WriteREG(FNCR, LCD_ReadREG(WLCR) | Fill_AW);
	LCD_WriteREG(FNCR, LCD_ReadREG(WLCR) | 0x08);
	delay_100ns(1);	 //5
	while((LCD_ReadREG(WLCR)&0x08) != 0x00);
	//LCD_EliminateFlickerON();
}

//被调用
void LCD_FillAppWindow(uint8_t bytedata)
{
	LCD_PicsMode();
	LCD_EliminateFlickerOFF();
	LCD_WriteREG(PNTR, bytedata);
	//LCD_WriteREG(FNCR, LCD_ReadREG(WLCR) | Fill_AW);
	LCD_WriteREG(FNCR, LCD_ReadREG(WLCR) | 0x08);
	//LCD_EliminateFlickerON();
}

//被调用
void LCD_SetAppwindow(uint8_t toppixel, uint8_t bottompixel, uint8_t leftpixel, uint8_t rightpixel)
{
	LCD_WriteREG(AWLR, leftpixel);					   
	LCD_WriteREG(AWTR, toppixel);
	LCD_WriteREG(AWRR, rightpixel);					   
	LCD_WriteREG(AWBR, bottompixel);
}

//被调用																 
void LCD_SetCursor(uint8_t x, uint8_t y)	   //x由水平像素位置/8-1得出，y有垂直像素位置-1得出
{											   //x范围0-27h，y范围0-EFh
	LCD_WriteREG(CURX, x);
	LCD_WriteREG(CURY, y);	
}

//被调用
void LCD_ShowLayer(uint8_t layer)
{
	LCD_WriteREG(MAMR, (LCD_ReadREG(MAMR)&0x8F) | layer);
}

//被调用
void LCD_AccessLayer(uint8_t layer)
{
	LCD_WriteREG(MAMR, (LCD_ReadREG(MAMR)&0xFC) | layer);
	delay_100ns(1);	  //5
}

//被调用
void LCD_DDRAMs_ShowRule(uint8_t rule)
{
	LCD_WriteREG(MAMR, (LCD_ReadREG(MAMR)&0xF3) | rule);
}

//被调用
void LCD_DrawPics(uint8_t x, uint8_t y, uint8_t hpixel, uint8_t vpixel, uint8_t* pic)
{
	uint8_t hp, vp;
	uint8_t* picaddr;

	picaddr = pic;
	
	LCD_AccessLayer(Layer_Selection_DDRAM2);
	
	LCD_PicsMode();

	LCD_SetCursor(x, y);
	LCD_WriteREG(AWLR, x);		 //设置工作窗口为图片大小
	LCD_WriteREG(AWTR, y);
	LCD_WriteREG(AWRR, x+hpixel);					   
	LCD_WriteREG(AWBR, y+vpixel);
	LCD_WriteCommand(MWCR);		 //光标设置为水平移动
	
	for(vp=0; vp<vpixel; vp++)
		for(hp=0; hp<hpixel; hp++){
			LCD_WriteData(*picaddr++);
		}

	LCD_WriteREG(AWLR, 0);		 //重设工作窗口为320*240
	LCD_WriteREG(AWTR, 0);
	LCD_WriteREG(AWRR, 39);					   
	LCD_WriteREG(AWBR, 239);
}

//被调用
void LCD_DrawRectangle(uint8_t x, uint8_t y, uint8_t width, uint8_t height)
{
	uint8_t hp, vp;
	uint8_t DDRAM_data;

	LCD_AccessLayer(Layer_Selection_DDRAM2);

	LCD_PicsMode();
	
	for(vp=0; vp<height; vp++){
		if(vp==0){
			LCD_SetCursor(x, y+vp);
			LCD_WriteCommand(MRCR);
			DDRAM_data = LCD_ReadData();
			
			LCD_SetCursor(x, vp+y);
			LCD_WriteCommand(MWCR);
			LCD_WriteData(0x0F|DDRAM_data);
			for(hp=1; hp<width; hp++)
				LCD_WriteData(0xFF);

			LCD_WriteCommand(MRCR);
			DDRAM_data = LCD_ReadData();

			LCD_SetCursor(x+width, vp+y);
			LCD_WriteCommand(MWCR);
			LCD_WriteData(0xF0|DDRAM_data);
		}
		
		else if(vp==height-1){
			LCD_SetCursor(x, vp+y);
			LCD_WriteCommand(MRCR);
			DDRAM_data = LCD_ReadData();

			LCD_SetCursor(x, vp+y);
			LCD_WriteCommand(MWCR);
			LCD_WriteData(0x0F|DDRAM_data);
			for(hp=1; hp<width; hp++)
				LCD_WriteData(0xFF);

			LCD_WriteCommand(MRCR);
			DDRAM_data = LCD_ReadData();

			LCD_SetCursor(x+width, vp+y);
			LCD_WriteCommand(MWCR);
			LCD_WriteData(0xF0);
		}
		else{
			LCD_SetCursor(x, y+vp);
			LCD_WriteCommand(MRCR);
			DDRAM_data = LCD_ReadData();
			LCD_SetCursor(x, y+vp);
			LCD_WriteCommand(MWCR);
			LCD_WriteData(0x08|DDRAM_data);

			LCD_SetCursor(x+width-1, y+vp);
			LCD_WriteCommand(MRCR);
			DDRAM_data = LCD_ReadData();
			LCD_SetCursor(x+width, y+vp);
			LCD_WriteCommand(MWCR);
			LCD_WriteData(0x10|DDRAM_data);
		}
	}

	LCD_WriteCommand(MRCR);
}

//被调用
void LCD_DrawForm(uint8_t x, uint8_t y, uint8_t row, uint8_t rank, uint8_t width, uint8_t height)
{
	uint8_t i;
	uint8_t hp, vp;
	uint8_t DDRAM_data;

	LCD_AccessLayer(Layer_Selection_DDRAM2);

	LCD_PicsMode();

	//先画横线
	for(i=0; i<rank+1; i++){
		LCD_SetCursor(x, y+height*i);
		LCD_WriteCommand(MWCR);
		for(hp=0; hp<width*row; hp++){
			LCD_WriteData(0xFF);
			delay_100ns(1);
		} 
	}
	

	//再画竖线
	for(i=0; i<row+1; i++){
		//LCD_SetCursor(x+width*i, y);
		for(vp=0; vp<height*rank; vp++){
			LCD_SetCursor(x+width*i, y+vp+1);
			LCD_WriteCommand(MRCR);		   //预读指令，光标自增
			DDRAM_data = LCD_ReadData();   //读出预读数据，光标又自增。。。

			//LCD_WriteREG(CURY, (LCD_ReadREG(CURY)-2));
			LCD_SetCursor(x+width*i, y+vp+1);
			LCD_WriteCommand(MWCR);
			LCD_WriteData(0x80|DDRAM_data);
		}
	}
}

//被调用
void LCD_WriteChar1(uint8_t ch){	
	LCD_AccessLayer(Layer_Selection_DDRAM1);

	LCD_TextMode();

	//LCD_TextAsciiMode();
	//LCD_TextHybridMode();

	LCD_WriteCommand(MWCR);
	LCD_WriteData(ch);
}

//被调用
void LCD_WriteChar2(uint8_t x, uint8_t y, uint8_t ch){

	LCD_AccessLayer(Layer_Selection_DDRAM1);

	LCD_TextMode();
	
	LCD_SetCursor(x, y);

	//LCD_TextAsciiMode();
	//LCD_TextHybridMode();

	LCD_WriteCommand(MWCR);
	LCD_WriteData(ch);
}

//被调用
void LCD_WriteText1(uint8_t* str)
{
	uint8_t* straddr;
	
	straddr = str;

	LCD_AccessLayer(Layer_Selection_DDRAM1);
	LCD_TextMode();
	LCD_WriteCommand(MWCR);

	while(*straddr!='\0'){
		LCD_WriteData(*straddr++);
	}
}

//被调用
void LCD_WriteText2(uint8_t x, uint8_t y, uint8_t* str)
{
	uint8_t* straddr;

	straddr = str;

	LCD_AccessLayer(Layer_Selection_DDRAM1);

	LCD_TextMode();

	LCD_SetCursor(x, y);
	LCD_WriteCommand(MWCR);

	while(*straddr!='\0'){
		LCD_WriteData(*straddr++);
		delay_100ns(1);
	}
}

//被调用
void LCD_TextBold(FunctionalState state)
{
	if(state == ENABLE){
		LCD_WriteREG(WCCR, LCD_ReadREG(WCCR) | Bold_Font);
	}
	else{  //state == DISABLE
		LCD_WriteREG(WCCR, LCD_ReadREG(WCCR)&0xEF);
	}
}

//被调用
void LCD_SetCursorHeight(uint8_t curheight)
{
	LCD_WriteREG(CHWI, (LCD_ReadREG(CHWI)&0x0F) | (curheight<<4));
}

//被调用
void LCD_SetWordInterval(uint8_t line_width)
{
	LCD_WriteREG(CHWI, (LCD_ReadREG(CHWI)&0xF0) | line_width);
}

//被调用
void LCD_EnlargeText(uint8_t hmul, uint8_t vmul)
{
	LCD_WriteREG(FVHT, (LCD_ReadREG(FVHT)&0x3F) | ((hmul-1)<<6));
	LCD_WriteREG(FVHT, (LCD_ReadREG(FVHT)&0xCF) | ((vmul-1)<<4));
}

//被调用
void LCD_AsciiBlock(uint8_t standard, uint8_t blockarea)
{
	LCD_WriteREG(FNCR, (LCD_ReadREG(FNCR)&0x7F) | standard);
	LCD_WriteREG(FNCR, (LCD_ReadREG(FNCR)&0xFC) | blockarea);
}

//被调用
void LCD_CGRAM_CreatWord(uint8_t word_number, uint8_t* cg)	 //在CGRAM中创造全字型
{
	uint8_t i;
	uint8_t exlayer;
	uint8_t* cgaddr;

	cgaddr = cg;

	exlayer = LCD_ReadREG(MAMR)&0x03;

	LCD_WriteREG(MAMR, (LCD_ReadREG(MAMR)&0xFC) | Layer_Selection_CGRAM);
	LCD_WriteREG(CURX, 0x00);
	LCD_WriteREG(CURY, word_number);

	LCD_WriteCommand(MWCR);
	for(i=0; i<32; i++){
		LCD_WriteData(*cgaddr++);
	}

	LCD_WriteREG(MAMR, (LCD_ReadREG(MAMR)&0xFC) | exlayer);  //创造字型后返回以前的操作图层
}

//被调用
void LCD_CGRAM_CreatHalfWord(uint8_t word_number, uint8_t odd_even, uint8_t* cg)
{
	uint8_t i;
	uint8_t exlayer;
	uint8_t* cgaddr;
	uint8_t lcd_flag_temp;

	cgaddr = cg;

	exlayer = LCD_ReadREG(MAMR)&0x03;

	lcd_flag_temp = LCD_FLAG_Using;

LCD_FLAG_Using = 1;

	LCD_WriteREG(MAMR, (LCD_ReadREG(MAMR)&0xFC) | Layer_Selection_CGRAM);
	LCD_WriteREG(CURX, odd_even);
	LCD_WriteREG(CURY, word_number);

	LCD_WriteCommand(MWCR);
	for(i=0; i<32; i++){
		LCD_WriteData(*cgaddr++);
	}

	LCD_WriteREG(MAMR, (LCD_ReadREG(MAMR)&0xFC) | exlayer);  //创造字型后返回以前的操作图层

LCD_FLAG_Using = lcd_flag_temp;
if(LCD_FLAG_UpdateIrq == 1 && LCD_FLAG_Using == 0){
	EXTI_GenerateSWInterrupt(EXTI_Line1);
}
}

//被调用
void LCD_WriteWordFromCGRAM(uint8_t x, uint8_t y, uint8_t word_number){
	LCD_AccessLayer(Layer_Selection_DDRAM1);
	
	LCD_TextMode();
	LCD_WriteREG(WLCR, LCD_ReadREG(WLCR) | Linear_Decode_Mode);

	LCD_SetCursor(x, y);
	LCD_WriteCommand(MWCR);
	LCD_WriteData(0x9F);
	LCD_WriteData(word_number);

	LCD_WriteREG(WLCR, LCD_ReadREG(WLCR)&(~Linear_Decode_Mode));
}

//被调用
void LCD_WriteHalfWordFromCGRAM(uint8_t x, uint8_t y, uint8_t word_number, uint8_t odd_even){
	LCD_AccessLayer(Layer_Selection_DDRAM1);
	
	LCD_TextMode();
	LCD_WriteREG(WLCR, LCD_ReadREG(WLCR) | Linear_Decode_Mode);

	LCD_SetCursor(x, y);
	LCD_WriteCommand(MWCR);
	LCD_WriteData(0x8F);
	LCD_WriteData(word_number*2+odd_even);

	LCD_WriteREG(WLCR, LCD_ReadREG(WLCR)&(~Linear_Decode_Mode));
}

//被调用
void LCD_IconInit(void)
{
	LCD_CGRAM_CreatWord(0, (uint8_t*)Night_Icon);
	LCD_CGRAM_CreatWord(1, (uint8_t*)Manual_Icon);
}

//被调用
//Item数量不要超过屏的显示容量
//根据坐标反白相关Item
void LCD_NorString(uint8_t x, uint8_t y, uint8_t* str, FunctionalState invstate)
{
	uint8_t lcd_flag_temp;
	
	lcd_flag_temp = LCD_FLAG_Using;

LCD_FLAG_Using = 1;

	if(invstate == ENABLE){
		LCD_Inv();
	}
	else{
		LCD_NoInv();
	}

	LCD_WriteText2(x, y, str);

	LCD_NoInv();
	
LCD_FLAG_Using = lcd_flag_temp;
if(LCD_FLAG_UpdateIrq == 1 && LCD_FLAG_Using == 0){
	EXTI_GenerateSWInterrupt(EXTI_Line1);
}
	
}

void LCD_ClearMenuText(void)
{
	LCD_AccessLayer(Layer_Selection_DDRAM1);
	LCD_SetAppwindow(53, 236, 0, 39);
	LCD_ClearAppWindow();
}

void LCD_ClearItemText(void)
{
	LCD_AccessLayer(Layer_Selection_DDRAM1);
	LCD_SetAppwindow(74, 236, 1, 28);
	//LCD_SetAppwindow(74, 236, 1, 38);
	LCD_ClearAppWindow();
}

void LCD_ClearItemAllText(void)
{
	LCD_AccessLayer(Layer_Selection_DDRAM1);
	LCD_SetAppwindow(74, 236, 0, 39);
	LCD_ClearAppWindow();
}

void LCD_ClearMenuForm(void)
{
	LCD_AccessLayer(Layer_Selection_DDRAM2);
	LCD_SetAppwindow(53, 236, 1, 38);
	LCD_ClearAppWindow();
//	LCD_WriteChar2(2, 61, ' ');
}

void LCD_DrawMenuForm(void){
	LCD_AccessLayer(Layer_Selection_DDRAM2);
	LCD_SetAppwindow(0, 0, 39, 239);
	LCD_DrawForm(1, 53, 1, 1, 38, 20);
	LCD_DrawForm(1, 73, 1, 1, 28, 163);
	LCD_DrawForm(29, 73, 1, 1, 10, 163);
}

#if MOVE
void LCD_RefreshItem(void)
{
	uint8_t lcd_flag_temp;
	
	lcd_flag_temp = LCD_FLAG_Using;

	if(Current_Menu->Items[Current_Menu->Item_Sel].Children_Menu != NULL){
		LCD_WriteText2(Current_Menu->Items[Current_Menu->Item_Sel].Item_x, Current_Menu->Items[Current_Menu->Item_Sel].Item_y, "+ ");
	}
	else if(Current_Menu->Items[Current_Menu->Item_Sel].function != NULL){
		LCD_WriteText2(Current_Menu->Items[Current_Menu->Item_Sel].Item_x, Current_Menu->Items[Current_Menu->Item_Sel].Item_y, "\x10 ");
	}
	else{
		LCD_WriteText2(Current_Menu->Items[Current_Menu->Item_Sel].Item_x, Current_Menu->Items[Current_Menu->Item_Sel].Item_y, "  ");
	}
	LCD_WriteText1(Current_Menu->Items[Current_Menu->Item_Sel].Item_Context);
}
#endif

#if MOVE
void LCD_NorItem(void)
{
	uint8_t lcd_flag_temp;
	
	lcd_flag_temp = LCD_FLAG_Using;

	LCD_Inv();
	if(Current_Menu->Items[Current_Menu->Item_Sel].Children_Menu != NULL){
		LCD_WriteText2(Current_Menu->Items[Current_Menu->Item_Sel].Item_x, Current_Menu->Items[Current_Menu->Item_Sel].Item_y, "+ ");
	}
	else if(Current_Menu->Items[Current_Menu->Item_Sel].function != NULL){
		LCD_WriteText2(Current_Menu->Items[Current_Menu->Item_Sel].Item_x, Current_Menu->Items[Current_Menu->Item_Sel].Item_y, "\x10 ");
	}
	else{
		LCD_WriteText2(Current_Menu->Items[Current_Menu->Item_Sel].Item_x, Current_Menu->Items[Current_Menu->Item_Sel].Item_y, "  ");
	}
	LCD_WriteText1(Current_Menu->Items[Current_Menu->Item_Sel].Item_Context);
	LCD_NoInv();
}
#endif
//--------------------------------------------------------
//以下为应用函数
void LCD_Init(void)
{
	LCD_Reset();	

	LCD_WriteREG(WLCR, Display_ON);		   	   //开启屏幕显示
	LCD_WriteREG(MISC, Driver_Clock_Div4);	   //忙信号有效电平为高电平，关闭雪花消除功能，进入绘图模式
	LCD_WriteREG(ADSR, 0x00);				   //关闭卷动功能
	//LCD_WriteREG(ADSR, SCR_HV | SCR_EN);
	LCD_WriteREG(INTR, 0x00);				   //屏蔽扫描中断
	//LCD_WriteREG(WCCR, Cursor_Display_ON);	   //显示光标
	LCD_WriteREG(WCCR, 0x00);
	LCD_WriteREG(CHWI, 0x00);				   //光标高度1像素，行距1像素

	LCD_AccessLayer(Layer_Selection_DDRAMs);
	LCD_ShowLayer(Display_Layer_DDRAMs);
	LCD_DDRAMs_ShowRule(TwoLayer_Rule_XOR);
	LCD_SetAppwindow(0, 239, 0, 39);
	LCD_FillAppWindow(0x00);

	LCD_WriteREG(DWWR, 39);					   //显示窗口320*240
	LCD_WriteREG(DWHR, 239);

	LCD_WriteREG(BGSG, 0);					   //卷动窗口(卷动功能没开启)0*0
	LCD_WriteREG(BGCM, 0);
	LCD_WriteREG(EDSG, 0);
	LCD_WriteREG(EDCM, 0);
	//LCD_WriteREG(BTMR, 0x0F);				   //调整卷动速度
	/*
	LCD_WriteREG(BGSG, 0);
	LCD_WriteREG(BGCM, 0);
	LCD_WriteREG(EDSG, 19);
	LCD_WriteREG(EDCM, 64);
	*/

	LCD_WriteREG(CURX, 0);
	LCD_WriteREG(CURY, 8);

	LCD_WriteREG(BTMR, 0x02);
	LCD_WriteREG(ITCR, 0x00);

	LCD_WriteREG(FVHT, 0x00);				   //不放大字符

	//LCD_FillAppWindow(0xFF);				   //测屏坏点
	//填充屏幕，汉字英语混合显示模式，这样无法使用ASCII字符表80h以上的字符
	
	LCD_TextBold(ENABLE);
	CN_EN_HybridDisplay;
	
	LCD_AccessLayer(Layer_Selection_DDRAM1);	//DDRAM1用于处理文字，DDRAM2用于处理表格与反白
}

void LCD_DisplayMainPannel(void)
{
	uint8_t i;
	
	LCD_AccessLayer(Layer_Selection_DDRAM1);
	LCD_SetAppwindow(53, 236, 1, 38);
	LCD_ClearAppWindow();
	LCD_AccessLayer(Layer_Selection_DDRAM2);
	LCD_SetAppwindow(53, 236, 1, 38);
	LCD_ClearAppWindow();	

	//LCD_AccessLayer(Layer_Selection_DDRAM2);
	LCD_DrawForm(1, 53, 2, 1, 19, 143);
	LCD_DrawForm(1, 196, 1, 2, 38, 20);

//	LCD_SetAppwindow(53, 236, 0, 39);
	LCD_WriteChar2(2, 198, ' ');
	
	for(i=0; i<MainMenu.Item_Num; i++){
		if(MainMenu.Item_Sel == i)  LCD_Inv();
		if(MainMenu_Items[i].Children_Menu != NULL){
			LCD_WriteText2(MainMenu_Items[i].Item_x, MainMenu_Items[i].Item_y, "+ ");
		}
		else if(MainMenu_Items[i].key_function != NULL){
			LCD_WriteText2(MainMenu_Items[i].Item_x, MainMenu_Items[i].Item_y, "\x10 ");
		}
		else{
			LCD_WriteText2(MainMenu_Items[i].Item_x, MainMenu_Items[i].Item_y, "  ");
		}
		LCD_WriteText1(MainMenu_Items[i].Item_Context);
		if(MainMenu.Item_Sel == i)  LCD_NoInv();
	}

	LCD_WriteText2(2, 198, "\x18\x19\x1A\x1B:移动 ENT:选择 ESC:退出");

	LCD_WriteText2(2, 218, MainMenu_Items[MainMenu.Item_Sel].Item_HelpInfo);

	LCD_SetAppwindow(0, 239, 0, 39);
}

void LCD_InterfaceInit(void)
{
	LCD_DrawRectangle(0, 0, 39, 239);

	LCD_WriteText2(1, 2, "当前时间:[    -  -  ][  :  :  ] 星期");
	LCD_WriteText2(1, 19, "运行时段:  (  )");
	LCD_WriteText2(20, 19, "运行相位:  (  )");
	LCD_WriteText2(1, 36, "当前相位剩余时间:   秒");	 //同行紧跟模式图标

	LCD_DisplayMainPannel();
}

void LCD_UpdateInfo(void)
{
	//保存上下文
	uint8_t xtemp;
	uint8_t ytemp;

	xtemp = LCD_ReadREG(CURX);
	ytemp = LCD_ReadREG(CURY);

	while(LCD_ReadREG(FNCR)&0x08 != 0x00);
	//FNCR_temp = LCD_ReadREG(FNCR);
	/*	
	LCD_WriteText2(1, 2, "当前时间:[    -  -  ][  :  :  ] 星期");
	LCD_WriteText2(1, 19, "运行时段:  (  )");
	LCD_WriteText2(20, 19, "运行相位:  (  )");
	LCD_WriteText2(1, 36, "当前相位剩余时间:   秒");
	*/
	LCD_SetAppwindow(0, 239, 0, 39);
	//更新年月日
	LCD_WriteChar2(11, 2, Number_Table[Current_Time.tm_year/1000]);
	LCD_WriteChar1(Number_Table[(Current_Time.tm_year/100)%10]);
	LCD_WriteChar1(Number_Table[(Current_Time.tm_year%100)/10]);
	LCD_WriteChar1(Number_Table[Current_Time.tm_year%10]);

	LCD_WriteChar1('-');
	
	//LCD_WriteChar2(16, 2, ((Current_Time.tm_mon+1)/10==0)?' ':Number_Table[(Current_Time.tm_mon+1)/10]);
	LCD_WriteChar1(((Current_Time.tm_mon+1)/10==0)?' ':Number_Table[(Current_Time.tm_mon+1)/10]);
	LCD_WriteChar1(Number_Table[(Current_Time.tm_mon+1)%10]);

	LCD_WriteChar1('-');
	
	//LCD_WriteChar2(19, 2, (Current_Time.tm_mday/10==0)?' ':Number_Table[Current_Time.tm_mday/10]);
	LCD_WriteChar1((Current_Time.tm_mday/10==0)?' ':Number_Table[Current_Time.tm_mday/10]);
	LCD_WriteChar1(Number_Table[Current_Time.tm_mday%10]);

	LCD_WriteText2(37, 2, (uint8_t*)Weekday_Table[Current_Time.tm_wday]);

	//更新时分秒
	LCD_WriteChar2(23, 2, (Current_Time.tm_hour/10==0)?' ':Number_Table[Current_Time.tm_hour/10]);
	LCD_WriteChar1(Number_Table[Current_Time.tm_hour%10]);

	LCD_WriteChar1(':');

	//LCD_WriteChar2(26, 2, (Current_Time.tm_min/10==0)?' ':Number_Table[Current_Time.tm_min/10]);
	LCD_WriteChar1((Current_Time.tm_min/10==0)?' ':Number_Table[Current_Time.tm_min/10]);
	LCD_WriteChar1(Number_Table[Current_Time.tm_min%10]);

	LCD_WriteChar1(':');

	//LCD_WriteChar2(29, 2, (Current_Time.tm_sec/10==0)?' ':Number_Table[Current_Time.tm_sec/10]);
	LCD_WriteChar1((Current_Time.tm_sec/10==0)?' ':Number_Table[Current_Time.tm_sec/10]);
	LCD_WriteChar1(Number_Table[Current_Time.tm_sec%10]);
	
	LCD_WriteChar2(10, 19, ((Current_TimeTable+1)/10==0)?' ':Number_Table[Current_TimeTable/10]);
	LCD_WriteChar1(Number_Table[(Current_TimeTable+1)%10]);

	LCD_WriteChar2(13, 19, (my_Schedule.TimeTable_Number/10==0)?' ':Number_Table[my_Schedule.TimeTable_Number/10]);
	LCD_WriteChar1(Number_Table[my_Schedule.TimeTable_Number%10]);
	
	LCD_WriteChar2(29, 19, ((Current_PhaseTable+1)/10==0)?' ':Number_Table[Current_PhaseTable/10]);
	LCD_WriteChar1(Number_Table[(Current_PhaseTable+1)%10]);
	
	LCD_WriteChar2(32, 19, (my_Schedule.PhaseTable[Current_TimeTable].Phase_Number/10==0)?' ':Number_Table[my_Schedule.PhaseTable[Current_TimeTable].Phase_Number/10]);
	LCD_WriteChar1(Number_Table[my_Schedule.PhaseTable[Current_TimeTable].Phase_Number%10]); 

	LCD_WriteChar2(18, 36, (Current_CutDown/10==0)?' ':Number_Table[Current_CutDown/10]);
	LCD_WriteChar1(Number_Table[Current_CutDown%10]);

	if(Time_FLAG_DayOrNight == 1)	
		LCD_WriteWordFromCGRAM(28, 36, 0);	
	else    
		LCD_WriteText2(28, 36, "  ");

	if(Manual_FLAG == 1)	
		LCD_WriteWordFromCGRAM(31, 36, 1);	
	else  
		LCD_WriteText2(31, 36, "  ");

	LCD_SetCursor(xtemp, ytemp);
}

void LCD_DisplayOtherMenu(struct Menu_TypeDef* menu)
{
	uint8_t i;

	LCD_ClearMenuText();

	LCD_WriteText2(2, 55, menu->Menu_Title);

	for(i=0; i<menu->Item_Num; i++){
		if(menu->Item_Sel == i)  LCD_Inv();
		if(menu->Items[i].Children_Menu != NULL){
			LCD_WriteText2(menu->Items[i].Item_x, menu->Items[i].Item_y, "+ ");
		}
		else if(menu->Items[i].key_function != NULL){
			LCD_WriteText2(menu->Items[i].Item_x, menu->Items[i].Item_y, "\x10 ");
		}
		else{
			LCD_WriteText2(menu->Items[i].Item_x, menu->Items[i].Item_y, "  ");
		}
		LCD_WriteText1(menu->Items[i].Item_Context);
		if(menu->Item_Sel == i)  LCD_NoInv();
	}

	LCD_WriteText2(29, 176, "\x18\x19\x1A\x1B:移动");
	LCD_WriteText2(29, 196, "ENT:选择");
	LCD_WriteText2(29, 216, "ESC:退出");

	LCD_WriteText2(29, 78, "帮助信息");

	LCD_SetAppwindow(96, 170, 29, 38);
	LCD_WriteText2(29, 96, menu->Items[menu->Item_Sel].Item_HelpInfo);
	
	LCD_SetAppwindow(0, 239, 0, 39);
}






void keyfunction_help(uint8_t* str){
	LCD_AccessLayer(Layer_Selection_DDRAM1);
	LCD_SetAppwindow(76, 135, 29, 38);
	LCD_WriteText2(29, 76, str);
	LCD_SetAppwindow(0, 239, 0, 39);
}
void keyfunction_clear_help(void){
	LCD_AccessLayer(Layer_Selection_DDRAM1);
	LCD_SetAppwindow(76, 135, 29, 38);
	LCD_ClearAppWindow();
	LCD_SetAppwindow(0, 239, 0, 39);
}

/**********************************************************
公共密码解锁函数
**********************************************************/
void MenuLock_Update(uint8_t sel, menulock_fundata* menucode){
	uint8_t i;

	LCD_SetCursor(11, 110);
	for(i=0; i<6; i++){
		if(i == sel){
			LCD_Inv();
			LCD_WriteCommand(MWCR);
			LCD_WriteData(Number_Table[menucode->code[i]%10]);
			LCD_NoInv();
		}
		else{
			LCD_WriteCommand(MWCR);
			LCD_WriteData('*');
		}
	}
}
void MenuLock_init(uint8_t sel, uint8_t* code){
	uint8_t i;

	LCD_ClearItemText();

	//LCD_AccessLayer(Layer_Selection_DDRAM1);
	LCD_SetAppwindow(96, 170, 29, 38);
	LCD_ClearAppWindow();

	LCD_SetAppwindow(0, 239, 0, 39);
	
	LCD_WriteText2(2, 80, "请输入六位密码:(初始为0)");
	LCD_SetCursor(11, 110);
	for(i=0; i<CODE_LEN; i++){
		if(i == sel){
			LCD_Inv();
			LCD_WriteCommand(MWCR);
			LCD_WriteData(Number_Table[code[i]%10]);
			LCD_NoInv();
		}
		else{
			LCD_WriteCommand(MWCR);
			LCD_WriteData('*');
		}
	}
}
//注意：菜单信息对menulock函数是不可见的
void Menu_Lock(KEY_TypeDef key, lcd_appstate* state, uint8_t* sel, menulock_fundata* menucode){
	switch(key){
		case KEY3:	if(*sel==0) *sel=5; else (*sel)--;  break;
		case KEY4:  if(*sel==5) *sel=0; else (*sel)++;  break;
		case KEY1:	if(menucode->code[*sel]<9) menucode->code[*sel]++;  break;
		case KEY2:	if(menucode->code[*sel]>0) menucode->code[*sel]--;  break;
		case KEY5:	{
			if(menucode->code[0]==1 && menucode->code[1]==1 && menucode->code[2]==1 && menucode->code[3]==1 && menucode->code[4]==1 && menucode->code[5]==1){
				*state = lcdapp_unlocked;
				*sel = 0;
			}
			else{
				*state = lcdapp_codeerror;
			}
			break;
		}
		case KEY6:{
			*state = lcdapp_return;
			/*
			Current_Menu.cur_funptr = Current_Menu.key_function;
			free(tempdata);
			Current_Menu.fundata = NULL;   //可用可不用
	
			LCD_ClearMenuText();
			Current_Menu.item_display();
			Current_Menu.help_display();
			*/
			break;
		}
		case Nothing: goto menulock_refresh;  //return; 
		default: break;
	}

	goto menulock_havekey;
menulock_refresh:
	switch(*state){
		case lcdapp_codeerror:{
			*state = lcdapp_locked;
			LCD_AccessLayer(Layer_Selection_DDRAM1);
			LCD_SetAppwindow(76, 170, 29, 38);
			LCD_ClearAppWindow();
			LCD_SetAppwindow(0, 239, 0, 39);
		}
	}
	return;

menulock_havekey:
	KEY_PressMinus();

	switch(*state){
		case lcdapp_codeerror:{
			keyfunction_help("密码错误");
			break;
		}
		default: break;
	}

	MenuLock_Update(*sel, menucode);
}



/**********************************************************
手动模式-路灯测试函数
**********************************************************/
typedef struct{
	//uint8_t KEY_FLAG_Return;
	uint8_t LED_Sn;
	uint8_t sel;
	uint8_t delay;
	uint8_t test;
	lcd_appstate state;
	menulock_fundata codedata;
}ledcheck_fundata;
void LED_Check_datainit(void){
	ledcheck_fundata* tempdata;
			
	ManualModeMenu.fundata = calloc(1, sizeof(ledcheck_fundata));
	tempdata = (ledcheck_fundata*) ManualModeMenu.fundata;

	if(tempdata != NULL){
		//tempdata->KEY_FLAG_Return = 0;
		tempdata->LED_Sn = 1;
		tempdata->test = 0;
		tempdata->sel = 0;
		tempdata->state = lcdapp_ready;
		memset(&tempdata->codedata.code[0], 0, CODE_LEN);

		LED_Check_main(Nothing);
	}
}
void LED_Check_Update(uint8_t LED_Sn){
	LCD_Inv();											 
	LCD_SetCursor(16, 140);
	LCD_WriteCommand(MWCR);
	LCD_WriteData(Number_Table[LED_Sn/10]);
	LCD_WriteData(Number_Table[LED_Sn%10]);
	LCD_NoInv();
}
void LED_Check_interfaceinit(void){
	ledcheck_fundata* tempdata;

	tempdata = (ledcheck_fundata*) ManualModeMenu.fundata;

	LCD_ClearItemText();
		
	LCD_SetAppwindow(0, 239, 0, 39);
	
	LCD_WriteText2(2, 80, "设置要检测的交通灯序号");
	LCD_WriteText2(2, 100, "(1-48号)");
	LCD_WriteText2(2, 120, "\x1B\x1A:序号±10 \x18\x19:序号±1");
	LCD_WriteText2(2, 140, "当前测试序号【  】");

	LED_Check_Update(tempdata->LED_Sn);

	All_LED_Die();
	No_UpdateLED_FLAG = 1;	//设置全局标志位
}
void LED_Check(KEY_TypeDef key){
	ledcheck_fundata* tempdata;

	tempdata = (ledcheck_fundata*) ManualModeMenu.fundata;

	switch (key){
		case KEY3:	if(tempdata->LED_Sn<39) tempdata->LED_Sn+=10;  break;
		case KEY4:  if(tempdata->LED_Sn>10) tempdata->LED_Sn-=10;  break;
		case KEY1:	if(tempdata->LED_Sn<48) tempdata->LED_Sn++; break;
		case KEY1_Cont:  if(tempdata->LED_Sn<48) tempdata->LED_Sn++; break;			
		case KEY2:	if(tempdata->LED_Sn>1)  tempdata->LED_Sn--; break;
		case KEY2_Cont:  if(tempdata->LED_Sn>1) tempdata->LED_Sn--;  break;
		case KEY5:	{
			if(tempdata->state == lcdapp_auto) break;
			else{
				//GPIO_Write(LED_Port, ~(0x0001<<((tempdata->LED_Sn-1)%16)));
				GPIO_SetBits(LED_Port, GPIO_Pin_All);
				GPIO_ResetBits(LED_Port, (0x0001<<((tempdata->LED_Sn-1)%16)));
				delay_100ns(1);
				//GPIO_SetBits(LE_Port, (0x0001<<((tempdata->LED_Sn-1)/16)));
				GPIO_SetBits(LE_Port, LE_sel((tempdata->LED_Sn-1)/16));
				delay_100ns(1);
				//GPIO_ResetBits(LE_Port, (0x0001<<((tempdata->LED_Sn-1)/16)));
				GPIO_SetBits(LE_Port, LE_sel((tempdata->LED_Sn-1)/16));
				delay_100ns(1);
	
				tempdata->state = lcdapp_saved;
				break;
			}
		}
		case KEY6:{
			if(tempdata->state == lcdapp_auto){
				//TIM_SetCounter(TIM6, 0x0000);
				//TIM_Cmd(TIM6, ENABLE);
				tempdata->state = lcdapp_cmd;
				 /*
LCD_FLAG_Using = 1;
				LCD_AccessLayer(Layer_Selection_DDRAM1);
				LCD_SetAppwindow(76, 170, 29, 38);
				LCD_ClearAppWindow();
				LCD_SetAppwindow(0, 239, 0, 39);
LCD_FLAG_Using = lcd_flag_temp;
if(LCD_FLAG_UpdateIrq == 1 && LCD_FLAG_Using == 0){
	EXTI_GenerateSWInterrupt(EXTI_Line1);
}*/
				keyfunction_clear_help();
			}
			else
				tempdata->state = lcdapp_return;
			break;
		}
		case KEY2_3:  {
			//TIM_Cmd(TIM6, DISABLE);  //禁止键盘输入
			
			tempdata->state = lcdapp_auto;	//自动检测

			All_LED_Die();
			tempdata->test = 1;
			tempdata->delay = 0;
			/*
			for(test=0; test<=48; test++){
			   All_LED_Die();
			   GPIO_Write(LED_Port, (0x0001<<((test-1)%16)));
			   delay_100ns(1);
			   GPIO_SetBits(LE_Port, (0x0001<<((test-1)/16)));
			   delay_100ns(1);
			   GPIO_ResetBits(LE_Port, (0x0001<<((test-1)/16)));
			   delay_100ns(1);
			
			   delay_us(5000);
			}
			
			Menu_FLAG_Init_Finished = 0;
			
			TIM_SetCounter(TIM6, 0x0000);
			TIM_Cmd(TIM6, ENABLE);	*/
			break;
		}
		case Nothing: goto led_check_refresh; //break;
		default: break;
	}

	goto led_check_havekey;
led_check_refresh:
	switch(tempdata->state){
		case lcdapp_saved:{
			tempdata->state = lcdapp_cmd;
			//清除帮助显示
			keyfunction_clear_help();
			break;
		}
		case lcdapp_auto:{
			if(tempdata->delay>1){
				tempdata->delay = 0;
				GPIO_Write(LED_Port, ~(0x0001<<((tempdata->test-1)%16)));
				delay_100ns(1);
				//GPIO_SetBits(LE_Port, (0x0001<<((tempdata->test-1)/16)));
				GPIO_SetBits(LE_Port, LE_sel((tempdata->test-1)/16));
				delay_100ns(1);
				//GPIO_ResetBits(LE_Port, (0x0001<<((tempdata->test-1)/16)));
				GPIO_ResetBits(LE_Port, LE_sel((tempdata->test-1)/16));
				delay_100ns(1);
				if(tempdata->test<48) 
					tempdata->test++;
				else{
					keyfunction_clear_help();
					tempdata->state = lcdapp_cmd;
				}
			}
			else tempdata->delay++;
			break;
		}
		default:break;
	}
	return;

led_check_havekey:
	KEY_PressMinus();

	switch(tempdata->state){
		case lcdapp_saved:{
			keyfunction_help("更改成功");
			break;
		}
		case lcdapp_auto:{
			keyfunction_help("自动测试开始");
			break;
		}
		default: break;
	}
	
	LED_Check_Update(tempdata->LED_Sn);
}
void LED_Check_main(KEY_TypeDef key){
	ledcheck_fundata* tempdata;
//	uint8_t lcd_flag_temp;
	
//	lcd_flag_temp = LCD_FLAG_Using;
	tempdata = (ledcheck_fundata*) ManualModeMenu.fundata;

	switch(tempdata->state){
		case lcdapp_ready: MenuLock_init(tempdata->codedata.code_sel, &tempdata->codedata.code[0]); tempdata->state = lcdapp_locked; return; //break;
		case lcdapp_codeerror:
		case lcdapp_locked:	Menu_Lock(key, &tempdata->state, &tempdata->codedata.code_sel, &tempdata->codedata); break;
		case lcdapp_unlocked: LED_Check_interfaceinit(); tempdata->state = lcdapp_cmd; return; //break;
		case lcdapp_saved:
		case lcdapp_auto:
		case lcdapp_cmd: LED_Check(key); break;
		case lcdapp_return:{
			ManualModeMenu.cur_funptr = ManualModeMenu.key_function;
			free(tempdata);
			ManualModeMenu.fundata = NULL;   //可用可不用
	
			LCD_ClearMenuText();
			ManualModeMenu.item_display();
			ManualModeMenu.help_display();

			//LCD_DisplayOtherMenu(Current_Menu);
			Current_PhaseTable = 0;	 //重新执行相位
			Phase_FLAG_Start = 0;
			Cycle_FLAG_Start = 0;
			Time_JudgeDayOrNight();	 //更改昼夜模式
			Time_JudgeTimeTable();	 //更新当前时段
			Phase_OneShot = my_Schedule.PhaseTable[Current_TimeTable];
			Current_CutDown = Phase_OneShot.Last_Time[Current_PhaseTable];
			LED_Interpretation(Current_TimeTable, Current_PhaseTable);
			No_UpdateLED_FLAG = 0;
			return;
		} 
		default: break;
	}
}




/**********************************************************
手动模式-调整当前相位时间
**********************************************************/
typedef struct{
	lcd_appstate state;
	uint8_t sel;
	menulock_fundata codedata;
}manual_cutdown_fundata;
void ManualMode_CutDown_init(void){
	uint8_t i;
	manual_cutdown_fundata* tempdata;
	
	ManualModeMenu.fundata = calloc(1, sizeof(manual_cutdown_fundata));
	tempdata = (manual_cutdown_fundata*) ManualModeMenu.fundata;

	if(tempdata != NULL){
		tempdata->sel = 0;
		tempdata->state = lcdapp_ready;
		for(i=0; i<CODE_LEN; i++)  tempdata->codedata.code[i] = 0;
		tempdata->codedata.code_sel = 0;
	}

	ManualMode_CutDown_main(Nothing);	
}
void ManualMode_CutDown_interfaceinit(void){
	LCD_ClearItemText();

	LCD_SetAppwindow(0, 239, 0, 39);
	
	LCD_WriteText2(2, 80, "调整当前相位时间(6-99秒)");
	LCD_WriteText2(2, 100, "\x18\x19:加减时间1秒");
	LCD_WriteText2(2, 120, "\x1B\x1A:加减时间10秒");
}
void ManualMode_CutDown(KEY_TypeDef key){
	manual_cutdown_fundata* tempdata;
	
	tempdata = (manual_cutdown_fundata*)ManualModeMenu.fundata;

	switch (key){
		case KEY3:	if(Current_CutDown<90&&Current_CutDown>6) Current_CutDown+=10;  break;
		case KEY4:  if(Current_CutDown>16) Current_CutDown-=10;  break;
		case KEY1:	
		case KEY1_Cont: if(Current_CutDown<99&&Current_CutDown>6) Current_CutDown++;  break;
		case KEY2:
		case KEY2_Cont:	if(Current_CutDown>7) Current_CutDown--;  break;
		case KEY5:	break;
		case KEY6:	tempdata->state = lcdapp_return;  break;
		case Nothing:  return;
		default: break;
	}
	//goto manual_have_key;

	KEY_PressMinus();

	switch(tempdata->state){
		case lcdapp_return:{
			ManualModeMenu.cur_funptr = ManualModeMenu.key_function;
			free(tempdata);
			ManualModeMenu.fundata = NULL;   //可用可不用
	
			LCD_ClearMenuText();
			ManualModeMenu.item_display();
			ManualModeMenu.help_display();
			break;
		}
		default:break;
	}
}
void ManualMode_CutDown_main(KEY_TypeDef key){
	ledcheck_fundata* tempdata;

	tempdata = (ledcheck_fundata*) ManualModeMenu.fundata;

	switch(tempdata->state){
		case lcdapp_ready: MenuLock_init(tempdata->codedata.code_sel, &tempdata->codedata.code[0]); tempdata->state = lcdapp_locked; return; //break;
		case lcdapp_codeerror:
		case lcdapp_locked:	Menu_Lock(key, &tempdata->state, &tempdata->codedata.code_sel, &tempdata->codedata); break;
		case lcdapp_unlocked: ManualMode_CutDown_interfaceinit(); tempdata->state = lcdapp_cmd; return; //break;
		//case lcdapp_saved:
		//case lcdapp_auto:
		case lcdapp_cmd: ManualMode_CutDown(key); break;
		case lcdapp_return:{
			ManualModeMenu.cur_funptr = ManualModeMenu.key_function;
			free(tempdata);
			ManualModeMenu.fundata = NULL;   //可用可不用
	
			LCD_ClearMenuText();
			ManualModeMenu.item_display();
			ManualModeMenu.help_display();
			return;
		} 
		default: break;
	}
}


/**********************************************************
手动模式-循环运行指定相位
**********************************************************/
typedef struct{
	lcd_appstate state;
	uint8_t sel;
	menulock_fundata codedata;
	uint8_t timetable_index;
	uint8_t phasetable_index;
}run_phase_fundata;
void ManualMode_TimePhase_datainit(void){
	run_phase_fundata* tempdata;
	uint8_t i;

	ManualModeMenu.fundata = calloc(1, sizeof(run_phase_fundata));
	tempdata = (run_phase_fundata*) ManualModeMenu.fundata;

	if(tempdata != NULL){
		tempdata->state = lcdapp_ready;
		tempdata->sel = 0;
		tempdata->codedata.code_sel = 0;
		for(i=0; i<CODE_LEN; i++) tempdata->codedata.code[i] = 0;
	}

	ManualMode_TimePhase_main(Nothing);
}
void ManualMode_TimePhase_Update(uint8_t sel, uint8_t TimeTable_temp, uint8_t PhaseTable_temp){
	uint8_t i;
	
	for(i=0; i<2; i++){
		if(i == sel) LCD_Inv();
		LCD_SetCursor(Func_Run_PeriodPhase_Place[i][0], Func_Run_PeriodPhase_Place[i][1]);
		switch(i){
			case 0:	{
				LCD_WriteCommand(MWCR);
				LCD_WriteData(Number_Table[(TimeTable_temp+1)/10]);
				LCD_WriteData(Number_Table[(TimeTable_temp+1)%10]);
				break;
			}
			case 1: {
				LCD_WriteCommand(MWCR);
				LCD_WriteData(Number_Table[(PhaseTable_temp+1)/10]);
				LCD_WriteData(Number_Table[(PhaseTable_temp+1)%10]);
				break;
			}
			default: break;
		}
		if(i == sel) LCD_NoInv();
	}
}
void ManualMode_TimePhase_interface(void){
	run_phase_fundata* tempdata;

	tempdata = (run_phase_fundata*) ManualModeMenu.fundata; 

	LCD_ClearItemText();

	LCD_SetAppwindow(0, 239, 0, 39);
	
	LCD_WriteText2(2, 80, "设置并锁定运行时段及相位");
	LCD_WriteText2(2, 100, "\x1B\x1A:移动 \x18\x19:加减");
	LCD_WriteText2(2, 120, "当前选择: 时段【  】");
	LCD_WriteText2(12, 138, "相位【  】");

	ManualMode_TimePhase_Update(tempdata->sel, tempdata->timetable_index, tempdata->phasetable_index);

	RTC_ITConfig(RTC_IT_SEC, DISABLE);
	Manual_FLAG = 1;  
	LED_Interpretation(0, 0);  
	RTC_ITConfig(RTC_IT_SEC, ENABLE);
}
void ManualMode_TimePhase(KEY_TypeDef key){
	run_phase_fundata* tempdata;

	tempdata = (run_phase_fundata*) ManualModeMenu.fundata;	

	switch (key){
		case KEY3:	if(tempdata->sel==0) tempdata->sel=1; else tempdata->sel=0;  break;
		case KEY4:  if(tempdata->sel==0) tempdata->sel=1; else tempdata->sel=0;  break;
		case KEY1:	{
			if(tempdata->sel==0&&tempdata->timetable_index<my_Schedule.TimeTable_Number-1){
				tempdata->timetable_index++;
				tempdata->phasetable_index=0;
			} 
			else if(tempdata->sel==1&&tempdata->phasetable_index<my_Schedule.PhaseTable[tempdata->timetable_index].Phase_Number-1){
				tempdata->phasetable_index++;
			}  
			break;
		}
		case KEY2:	{
			if(tempdata->sel==0&&tempdata->timetable_index>0){
				tempdata->timetable_index--; 
				tempdata->phasetable_index=0;
			} 
			else if(tempdata->sel==1&&tempdata->phasetable_index>0){ 
				tempdata->phasetable_index--;  
			}
			break;
		}
		case KEY5:	{ 
			Current_TimeTable=tempdata->timetable_index;  
			Current_PhaseTable=tempdata->phasetable_index;  
			RTC_ITConfig(RTC_IT_SEC, DISABLE);  
			LED_Interpretation(Current_TimeTable, Current_PhaseTable);
			Current_CutDown = my_Schedule.PhaseTable[Current_TimeTable].Last_Time[Current_PhaseTable];  
			RTC_ITConfig(RTC_IT_SEC, ENABLE); 
			
			tempdata->state = lcdapp_set; 
			break;
		}
		case KEY6:	tempdata->state = lcdapp_return;  break;
		case Nothing: goto manualmode_timephase_refresh;
		default: break;
	}
	goto manualmode_timephase_have_key;

manualmode_timephase_refresh:
	switch(tempdata->state){
		case lcdapp_set:{
			tempdata->state = lcdapp_cmd;
			keyfunction_clear_help();
		}
	}
	return;

manualmode_timephase_have_key:
	KEY_PressMinus();

	switch(tempdata->state){
		case lcdapp_set:{
			keyfunction_help("设置成功");
			break;
		}
	}

	ManualMode_TimePhase_Update(tempdata->sel, tempdata->timetable_index, tempdata->phasetable_index);
}
void ManualMode_TimePhase_main(KEY_TypeDef key){
	run_phase_fundata* tempdata;
	tempdata = (run_phase_fundata*) ManualModeMenu.fundata;

	switch(tempdata->state){
		case lcdapp_ready: MenuLock_init(tempdata->sel, &tempdata->codedata.code[0]); tempdata->state = lcdapp_locked; return; //break;
		case lcdapp_codeerror:
		case lcdapp_locked:	Menu_Lock(key, &tempdata->state, &tempdata->sel, &tempdata->codedata); break;
		case lcdapp_unlocked: ManualMode_TimePhase_interface();  tempdata->state = lcdapp_cmd; return; //break;
		case lcdapp_set:
		case lcdapp_cmd: ManualMode_TimePhase(key); break;
		case lcdapp_return:{
			ManualModeMenu.cur_funptr = ManualModeMenu.key_function;
			free(tempdata);
			ManualModeMenu.fundata = NULL;   //可用可不用
	
			LCD_ClearMenuText();
			ManualModeMenu.item_display();
			ManualModeMenu.help_display();

			//LCD_DisplayOtherMenu(Current_Menu);
			Current_PhaseTable = 0;	 //重新执行相位
			Phase_FLAG_Start = 0;
			Cycle_FLAG_Start = 0;
			Time_JudgeDayOrNight();	 //更改昼夜模式
			Time_JudgeTimeTable();	 //更新当前时段
			Phase_OneShot = my_Schedule.PhaseTable[Current_TimeTable];
			Current_CutDown = Phase_OneShot.Last_Time[Current_PhaseTable];
			LED_Interpretation(Current_TimeTable, Current_PhaseTable);
			Manual_FLAG = 0;
			return;
		} 
		default: break;
	}
}
/****************************************************************
RTC设置菜单
****************************************************************/
typedef struct{
	uint8_t KEY_FLAG_Return;
	uint8_t sel;
	//uint8_t i;
	struct tm time;
} RTC_SetDate_fundata;
void RTC_SetDate_Update(uint8_t sel, struct tm time){
	uint8_t i;
	
	for(i=2; i<14; i++){
		if(i == sel){
			LCD_Inv();
		}
		LCD_SetCursor(Func_RTCDate_Place[i-2][0], Func_RTCDate_Place[i-2][1]);
		LCD_WriteCommand(MWCR);
		switch (i){
			//case 0:	 LCD_WriteData(Number_Table[time.tm_year/1000]);  break;
			//case 1:  LCD_WriteData(Number_Table[(time.tm_year/100)%10]);  break;
			case 2:	 LCD_WriteData(Number_Table[(time.tm_year%100)/10]);  break;
			case 3:	 LCD_WriteData(Number_Table[time.tm_year%10]);  break;
			case 4:	 LCD_WriteData(Number_Table[(time.tm_mon+1)/10]);  break;
			case 5:	 LCD_WriteData(Number_Table[(time.tm_mon+1)%10]);  break;
			case 6:	 LCD_WriteData(Number_Table[time.tm_mday/10]);  break;
			case 7:	 LCD_WriteData(Number_Table[time.tm_mday%10]);  break;
			case 8:	 LCD_WriteData(Number_Table[time.tm_hour/10]);  break;
			case 9:	 LCD_WriteData(Number_Table[time.tm_hour%10]);  break;
			case 10: LCD_WriteData(Number_Table[time.tm_min/10]);  break;
			case 11: LCD_WriteData(Number_Table[time.tm_min%10]);  break;
			case 12: LCD_WriteData(Number_Table[time.tm_sec/10]);  break;
			case 13: LCD_WriteData(Number_Table[time.tm_sec%10]);  break;
			default: break;
		}
		if(i == sel){
			LCD_NoInv();
		}
	}
}
void RTC_SetDate_Init(void){
	RTC_SetDate_fundata* tempdata;
	//uint8_t i;
	uint8_t lcd_flag_temp;
	
	lcd_flag_temp = LCD_FLAG_Using;
		
	RTCSetMenu.fundata = calloc(1, sizeof(RTC_SetDate_fundata));
	tempdata = (RTC_SetDate_fundata*) RTCSetMenu.fundata;

	if(RTCSetMenu.fundata != NULL){
		tempdata->KEY_FLAG_Return = 0;
		tempdata->sel = 2;
		tempdata->time.tm_year = 2012;   //<2038
		tempdata->time.tm_mon = 7;   //0-11
		tempdata->time.tm_mday = 18;	//1-31
		tempdata->time.tm_hour = 12;	//0-23
		tempdata->time.tm_min = 0;   //0-59
		tempdata->time.tm_sec = 0;   //0-59

LCD_FLAG_Using = 1;
/*	
	LCD_AccessLayer(Layer_Selection_DDRAM1);
	LCD_SetAppwindow(74, 162, 2, 27);
	LCD_ClearAppWindow();
*/
		LCD_ClearItemAllText();
		//LCD_SetAppwindow(0, 239, 0, 39);
		
		LCD_WriteText2(2, 80, "当光标置于个位时加1，十位时加10");
		LCD_WriteText2(2, 120, "日期:20  -  -  ");
		LCD_WriteText2(2, 150, "时间:    :  :  ");
		LCD_WriteText2(2, 200, "\x1B\x1A:移动 \x18\x19:加减");
	
		LCD_SetAppwindow(0, 239, 0, 39);
		
		RTC_SetDate_Update(tempdata->sel, tempdata->time);

LCD_FLAG_Using = lcd_flag_temp;
if(LCD_FLAG_UpdateIrq == 1 && LCD_FLAG_Using == 0){
	EXTI_GenerateSWInterrupt(EXTI_Line1);
}
	}
}
void RTC_SetDate(KEY_TypeDef key){
	RTC_SetDate_fundata* tempdata;
	uint8_t lcd_flag_temp;

	lcd_flag_temp = LCD_FLAG_Using;
	tempdata = (RTC_SetDate_fundata*) RTCSetMenu.fundata;
	
	switch (key){
		case KEY3:	if(tempdata->sel==2) tempdata->sel=13; else tempdata->sel--;  break;
		case KEY4:  if(tempdata->sel==13) tempdata->sel=2; else tempdata->sel++;  break;
		case KEY1:	{
			switch (tempdata->sel){
				//case 0:	 LCD_WriteData(Number_Table[time.tm_year/1000]);  break;
				//case 1:  LCD_WriteData(Number_Table[(time.tm_year/100)%10]);  break;
				case 2:	 {
					if(tempdata->time.tm_year<2028){ 
						tempdata->time.tm_year+=10;
						if(tempdata->time.tm_mon == 1&&tempdata->time.tm_mday>=29){  //检查当前设置月份是否2月
							if((tempdata->time.tm_year%4==0) && (tempdata->time.tm_year%100!=0) || (tempdata->time.tm_year%400==0)){
								tempdata->time.tm_mday = 29;
							}
							else{
								tempdata->time.tm_mday = 28;
							}
						}
					}  
					break;
				}
				case 3:	 {
					if(tempdata->time.tm_year<2037){
						tempdata->time.tm_year++;
						if(tempdata->time.tm_mon == 1&&tempdata->time.tm_mday>=29){  //检查当前设置月份是否2月
							if((tempdata->time.tm_year%4==0) && (tempdata->time.tm_year%100!=0) || (tempdata->time.tm_year%400==0)){
								tempdata->time.tm_mday = 29;
							}
							else{
								tempdata->time.tm_mday = 28;
							}
						}
					}  
					break;
				}
				case 4:	 {
					if(tempdata->time.tm_mon<2){ 
						tempdata->time.tm_mon+=10;
						if(tempdata->time.tm_mday==31){
							if(tempdata->time.tm_mon==11){
								tempdata->time.tm_mday = 30;
							}
						}
					}
					break;
				}  
				case 5:	 {
					if(tempdata->time.tm_mon<11){
						tempdata->time.tm_mon++;
						if(tempdata->time.tm_mon==3 || tempdata->time.tm_mon==5 || tempdata->time.tm_mon==8 || tempdata->time.tm_mon==10){
							if(tempdata->time.tm_mday==31)
								tempdata->time.tm_mday = 30;
						}
						else if(tempdata->time.tm_mon==1){
							if(tempdata->time.tm_mday>28){
								if((tempdata->time.tm_year%4==0) && (tempdata->time.tm_year%100!=0) || (tempdata->time.tm_year%400==0)){
									tempdata->time.tm_mday = 29;
								}
								else{
									tempdata->time.tm_mday = 28;
								}
							}
						}
					}
					break;
				}
				case 6:	 {
					if(tempdata->time.tm_mon==3 || tempdata->time.tm_mon==5 || tempdata->time.tm_mon==8 || tempdata->time.tm_mon==10){	 
						if(tempdata->time.tm_mday<21){
							tempdata->time.tm_mday += 10;
						}
					}
					else if(tempdata->time.tm_mon == 1){
						if((tempdata->time.tm_year%4==0) && (tempdata->time.tm_year%100!=0) || (tempdata->time.tm_year%400==0)){	//闰年
							if(tempdata->time.tm_mday<20)
								tempdata->time.tm_mday += 10;
						}
						else{
							if(tempdata->time.tm_mday<19)
								tempdata->time.tm_mday += 10;
						}
					}
					else{
						if(tempdata->time.tm_mday<22){
							tempdata->time.tm_mday += 10;
						}
					}
					break;
				}
				case 7:	 {
					if(tempdata->time.tm_mon==3 || tempdata->time.tm_mon==5 || tempdata->time.tm_mon==8 || tempdata->time.tm_mon==10){	 
						if(tempdata->time.tm_mday<30){
							tempdata->time.tm_mday ++;
						}
					}
					else if(tempdata->time.tm_mon == 1){
						if((tempdata->time.tm_year%4==0) && (tempdata->time.tm_year%100!=0) || (tempdata->time.tm_year%400==0)){	//闰年
							if(tempdata->time.tm_mday<29)
								tempdata->time.tm_mday ++;
						}
						else{
							if(tempdata->time.tm_mday<28)
								tempdata->time.tm_mday ++;
						}
					}
					else{
						if(tempdata->time.tm_mday<31){
							tempdata->time.tm_mday ++;
						}
					}
					break;
				}
				case 8:	 if(tempdata->time.tm_hour<14) tempdata->time.tm_hour+=10;  break;
				case 9:	 if(tempdata->time.tm_hour<23) tempdata->time.tm_hour++;  break;
				case 10: if(tempdata->time.tm_min<50) tempdata->time.tm_min+=10;  break;
				case 11: if(tempdata->time.tm_min<59) tempdata->time.tm_min++;   break;
				case 12: if(tempdata->time.tm_sec<50) tempdata->time.tm_sec+=10;  break;
				case 13: if(tempdata->time.tm_sec<59) tempdata->time.tm_sec++;  break;
				default: break;
			}
			break;
		}
		case KEY2:	{
			switch (tempdata->sel){
				//case 0:	 LCD_WriteData(Number_Table[time.tm_year/1000]);  break;
				//case 1:  LCD_WriteData(Number_Table[(time.tm_year/100)%10]);  break;
				case 2:	 {
					if(tempdata->time.tm_year>2009){ 
						tempdata->time.tm_year -= 10;
						if(tempdata->time.tm_mon == 1&&tempdata->time.tm_mday>=29){  //检查当前设置月份是否2月
							if((tempdata->time.tm_year%4==0) && (tempdata->time.tm_year%100!=0) || (tempdata->time.tm_year%400==0)){
								tempdata->time.tm_mday = 29;
							}
							else{
								tempdata->time.tm_mday = 28;
							}
						}
					}  
					break;
				}
				case 3:	 {
					if(tempdata->time.tm_year>2000){
						tempdata->time.tm_year--;
						if(tempdata->time.tm_mon == 1&&tempdata->time.tm_mday>=29){  //检查当前设置月份是否2月
							if((tempdata->time.tm_year%4==0) && (tempdata->time.tm_year%100!=0) || (tempdata->time.tm_year%400==0)){
								tempdata->time.tm_mday = 29;
							}
							else{
								tempdata->time.tm_mday = 28;
							}
						}
					}
					break;
				}
				case 4:	 {
					if(tempdata->time.tm_mon>9){ 
						tempdata->time.tm_mon-=10;
						if(tempdata->time.tm_mon==1&&tempdata->time.tm_mday>=29){
							if((tempdata->time.tm_year%4==0) && (tempdata->time.tm_year%100!=0) || (tempdata->time.tm_year%400==0)){
								tempdata->time.tm_mday = 29;
							}
							else{
								tempdata->time.tm_mday = 28;
							}
						}
					}
					break;
				}  
				case 5:	 {
					if(tempdata->time.tm_mon>0){
						tempdata->time.tm_mon--;
						if(tempdata->time.tm_mon==3 || tempdata->time.tm_mon==5 || tempdata->time.tm_mon==8 || tempdata->time.tm_mon==10){
							if(tempdata->time.tm_mday==31)
								tempdata->time.tm_mday = 30;
						}
						else if(tempdata->time.tm_mon==1){
							if(tempdata->time.tm_mday>28){
								if((tempdata->time.tm_year%4==0) && (tempdata->time.tm_year%100!=0) || (tempdata->time.tm_year%400==0)){
									tempdata->time.tm_mday = 29;
								}
								else{
									tempdata->time.tm_mday = 28;
								}
							}
						}
					}
					break;
				}
				case 6:	 {
					if(tempdata->time.tm_mday > 10){
						tempdata->time.tm_mday -= 10;
					}
				 	break;
				}
				case 7:	 {
					if(tempdata->time.tm_mday >1){
						tempdata->time.tm_mday --;
					}
					break;
				}
				case 8:	 if(tempdata->time.tm_hour>9) tempdata->time.tm_hour-=10;  break;
				case 9:	 if(tempdata->time.tm_hour>0) tempdata->time.tm_hour--;  break;
				case 10: if(tempdata->time.tm_min>9) tempdata->time.tm_min-=10;  break;
				case 11: if(tempdata->time.tm_min<0) tempdata->time.tm_min--;   break;
				case 12: if(tempdata->time.tm_sec>9) tempdata->time.tm_sec-=10;  break;
				case 13: if(tempdata->time.tm_sec>0) tempdata->time.tm_sec--;  break;
				default: break;
			}
			break;
		}
		case KEY5:	{
			RTC_WaitForLastTask();
			RTC_ITConfig(RTC_IT_SEC, DISABLE);  
			Time_SetCalendarTime(tempdata->time);  
			RTC_ITConfig(RTC_IT_SEC, ENABLE);  
			tempdata->KEY_FLAG_Return = 1;  
			break;
		}
		case KEY6:	{
			tempdata->KEY_FLAG_Return = 1;			  
			break;	//功能函数返回
		}
		case Nothing: return;
		default: break;
	}

	KEY_PressMinus();

	if(tempdata->KEY_FLAG_Return == 1){
		RTCSetMenu.cur_funptr = RTCSetMenu.key_function;
		free(tempdata);
		RTCSetMenu.fundata = NULL;   //可用可不用

		LCD_ClearMenuText();
		RTCSetMenu.item_display();
		RTCSetMenu.help_display();
		return;
	}

LCD_FLAG_Using = 1;
	
	RTC_SetDate_Update(tempdata->sel, tempdata->time);

LCD_FLAG_Using = lcd_flag_temp;
if(LCD_FLAG_UpdateIrq == 1 && LCD_FLAG_Using == 0){
	EXTI_GenerateSWInterrupt(EXTI_Line1);
}
}
/******************************************************
保存当前时间到外部RTC
******************************************************/
void time_saveto_ds1302(void){
	struct tm time = Current_Time;

	DS1302_SetSec(time.tm_sec, DISABLE);
	DS1302_SetMin(time.tm_min);
	DS1302_SetHour(time.tm_hour, DISABLE);
	DS1302_SetDate(time.tm_mday);
	DS1302_SetMon(time.tm_mon+1); //struct tm与ds1302内部存储范围不一致
	DS1302_SetYear(time.tm_year%100);
}
/******************************************************
昼夜时段设置
******************************************************/
typedef struct{
	uint8_t KEY_FLAG_Return;
	uint8_t hour, min, sec;
	uint8_t sel;	
} DayNight_fundata;
void DayNight_PeriodofTime_Update(uint8_t sel, uint8_t hour, uint8_t min, uint8_t sec){
	uint8_t i;

	for(i=0; i<6; i++){
		if(i == sel){
			LCD_Inv();
		}
		LCD_SetCursor(Func_DayNight_Place[i][0], Func_DayNight_Place[i][1]);
		LCD_WriteCommand(MWCR);
		switch (i){
			case 0:	 LCD_WriteData(Number_Table[hour/10]);  break;
			case 1:	 LCD_WriteData(Number_Table[hour%10]);  break;
			case 2:  LCD_WriteData(Number_Table[min/10]);  break;
			case 3:  LCD_WriteData(Number_Table[min%10]);  break;
			case 4:  LCD_WriteData(Number_Table[sec/10]);  break;
			case 5:  LCD_WriteData(Number_Table[sec%10]);  break;
			default: break;
		}
		if(i == sel){
			LCD_NoInv();
		}
	}
}
void DayNight_PeriodofTime_Init(void){
	DayNight_fundata* tempdata;
	uint8_t lcd_flag_temp;
	
	lcd_flag_temp = LCD_FLAG_Using;
		
	TimeSetMenu.fundata = calloc(1, sizeof(DayNight_fundata));
	tempdata = (DayNight_fundata*) TimeSetMenu.fundata;

	if(tempdata != NULL){
		tempdata->KEY_FLAG_Return = 0;
		tempdata->hour = my_Schedule.DayNight_Mode.DayNight_hour;
		tempdata->min = my_Schedule.DayNight_Mode.DayNight_min;
		tempdata->sec = my_Schedule.DayNight_Mode.DayNight_sec;
		tempdata->sel = 0;

LCD_FLAG_Using = 1;
		
		LCD_ClearItemAllText();
		//LCD_SetAppwindow(0, 239, 0, 39);
		
		LCD_WriteText2(2, 80, "当光标置于个位时加1，十位时加10");
		LCD_WriteText2(10, 130, "  :  :  ");
		LCD_WriteText2(2, 200, "\x1B\x1A:移动 \x18\x19:加减");
	
		LCD_SetAppwindow(0, 239, 0, 39);

		DayNight_PeriodofTime_Update(tempdata->sel, tempdata->hour, tempdata->min, tempdata->sec);

LCD_FLAG_Using = lcd_flag_temp;
if(LCD_FLAG_UpdateIrq == 1 && LCD_FLAG_Using == 0){
	EXTI_GenerateSWInterrupt(EXTI_Line1);
}
	}
}
void DayNight_PeriodofTime(KEY_TypeDef key){
	DayNight_fundata* tempdata;
	uint8_t lcd_flag_temp;
	
	lcd_flag_temp = LCD_FLAG_Using;
	tempdata = (DayNight_fundata*)TimeSetMenu.fundata;

	switch(key){
		case KEY3:	if(tempdata->sel==0) tempdata->sel=5; else tempdata->sel--;  break;
		case KEY4:	if(tempdata->sel==5) tempdata->sel=0; else tempdata->sel++;  break;
		case KEY1:  {
			switch (tempdata->sel){
				case 0: if(tempdata->hour < 14) tempdata->hour+=10; break;
				case 1: if(tempdata->hour < 23) tempdata->hour++;  break;
				case 2: if(tempdata->min < 50) tempdata->min+=10;  break;
				case 3: if(tempdata->min < 59) tempdata->min++;  break;
				case 4: if(tempdata->sec < 50) tempdata->sec+=10;  break;
				case 5: if(tempdata->sec < 59) tempdata->sec++;  break;
				default: break;
			}
			break;
		}
		case KEY1_Cont:{
			switch (tempdata->sel){
				case 0: if(tempdata->hour < 14) tempdata->hour+=10; break;
				case 1: if(tempdata->hour < 23) tempdata->hour++;  break;
				case 2: if(tempdata->min < 50) tempdata->min+=10;  break;
				case 3: if(tempdata->min < 59) tempdata->min++;  break;
				case 4: if(tempdata->sec < 50) tempdata->sec+=10;  break;
				case 5: if(tempdata->sec < 59) tempdata->sec++;  break;
				default: break;
			}
			break;
		}
		case KEY2:  {
			switch (tempdata->sel){
				case 0: if(tempdata->hour > 9) tempdata->hour-=10;  break;
				case 1: if(tempdata->hour > 0) tempdata->hour--;  break;
				case 2: if(tempdata->min > 9) tempdata->min-=10;  break;
				case 3: if(tempdata->min > 0) tempdata->min--;  break;
				case 4: if(tempdata->sec > 9) tempdata->sec-=10;  break;
				case 5: if(tempdata->sec > 0) tempdata->sec--;  break;
				default: break;
			}
			break;
		}
		case KEY2_Cont:{
			switch (tempdata->sel){
				case 0: if(tempdata->hour > 9) tempdata->hour-=10;  break;
				case 1: if(tempdata->hour > 0) tempdata->hour--;  break;
				case 2: if(tempdata->min > 9) tempdata->min-=10;  break;
				case 3: if(tempdata->min > 0) tempdata->min--;  break;
				case 4: if(tempdata->sec > 9) tempdata->sec-=10;  break;
				case 5: if(tempdata->sec > 0) tempdata->sec--;  break;
				default: break;
			}
			break;
		}
		case KEY5:  {
			RTC_WaitForLastTask();
			RTC_ITConfig(RTC_IT_SEC, DISABLE);  
			my_Schedule.DayNight_Mode.DayNight_hour = tempdata->hour;
			my_Schedule.DayNight_Mode.DayNight_min = tempdata->min;
			my_Schedule.DayNight_Mode.DayNight_sec = tempdata->sec; 
			RTC_WaitForLastTask(); 
			RTC_ITConfig(RTC_IT_SEC, ENABLE);  
			tempdata->KEY_FLAG_Return = 1;  
			break;
		}
		case KEY6:  tempdata->KEY_FLAG_Return = 1;  break;
		case Nothing: return; //break;	  //定时刷新屏幕，如果检测按键无输入且函数无需更新，返回
		default: break;
	}

	KEY_PressMinus();

	if(tempdata->KEY_FLAG_Return == 1){
		TimeSetMenu.cur_funptr = TimeSetMenu.key_function;
		free(tempdata);
		RTCSetMenu.fundata = NULL;   //可用可不用

		LCD_ClearMenuText();
		TimeSetMenu.item_display();
		TimeSetMenu.help_display();
		return;
	}

LCD_FLAG_Using = 1;

	DayNight_PeriodofTime_Update(tempdata->sel, tempdata->hour, tempdata->min, tempdata->sec);

LCD_FLAG_Using = lcd_flag_temp;
if(LCD_FLAG_UpdateIrq == 1 && LCD_FLAG_Using == 0){
	EXTI_GenerateSWInterrupt(EXTI_Line1);
}
}


/****************************************************************
灯号--路口led映射关系设置菜单
****************************************************************/
typedef struct{
	uint8_t KEY_FLAG_Return;
	uint8_t sel;
	uint8_t PinNum[16];
	uint8_t crossway;
	lcd_appstate state;
	uint16_t used_shadow[6];
} LedGPIO_fundata;
void LCD_LedGPIO_Update(uint8_t sel, uint8_t* PinNum){
	uint8_t i;
	LedGPIO_fundata* tempdata;

	tempdata = (LedGPIO_fundata*) LEDColorMenu.fundata;

	for(i=0; i<17; i++){
		if(i == tempdata->sel){
			LCD_Inv();
		}
		if(i == 0){
			LCD_WriteText2(Func_LedGPIO_Place[i][0], Func_LedGPIO_Place[i][1], (uint8_t*)Weekday_Table[tempdata->crossway+1]);	
		}
		else{
			LCD_SetCursor(Func_LedGPIO_Place[i][0], Func_LedGPIO_Place[i][1]);
			if(tempdata->PinNum[i-1]==0x00){
				LCD_WriteText1("×");
			}
			else{
				LCD_WriteCommand(MWCR);
				LCD_WriteData((tempdata->PinNum[i-1]/10==0x00)?' ':Number_Table[tempdata->PinNum[i-1]/10]);
				LCD_WriteData(Number_Table[tempdata->PinNum[i-1]%10]);
			}
		}
		if(i == tempdata->sel){
			LCD_NoInv();
		}
	}
}
void LCD_LedGPIO_Init(void){
	LedGPIO_fundata* tempdata;
	uint8_t lcd_flag_temp;
	uint8_t i;
	
	lcd_flag_temp = LCD_FLAG_Using;
		
	LEDColorMenu.fundata = calloc(1, sizeof(LedGPIO_fundata));
	tempdata = (LedGPIO_fundata*) LEDColorMenu.fundata;

	if(tempdata != NULL){
		tempdata->KEY_FLAG_Return = 0;
		tempdata->sel = 0;
		tempdata->crossway = 0;
		tempdata->state = lcdapp_ready;
		for(i=0; i<16; i++){
			tempdata->PinNum[i] = my_Schedule.Crossway_LED_Sn[tempdata->crossway].LEDColor_Sn[i];  //存储灯组1的第i个已定义灯色输出端口序号
		}
		for(i=0; i<6; i++){
			tempdata->used_shadow[i] = used_if_record[i]; //保存端口占用记录
		}

LCD_FLAG_Using = 1;

		LCD_ClearItemAllText();	
		LCD_SetAppwindow(0, 239, 0, 39);	
		LCD_WriteText2(2, 75, "灯组  (端口序号:1-96 ×:禁用)");
		//LCD_WriteText2(6, 80, (uint8_t*)Weekday_Table[tempdata->crossway+1]);
		LCD_WriteText2(11, 93, "绿   黄   红");
		LCD_WriteText2(5, 111, "左转 [  ] [  ] [  ]");
		LCD_WriteText2(5, 129, "直行 [  ] [  ] [  ]");
		LCD_WriteText2(5, 147, "右转 [  ] [  ] [  ]");
		LCD_WriteText2(5, 165, "调头 [  ] [  ] [  ]");
		LCD_WriteText2(5, 183, "人行1[  ]      [  ]");
		LCD_WriteText2(5, 201, "人行2[  ]      [  ]");
	
		LCD_WriteText2(29, 176, "\x1A\x1B:移动");
		LCD_WriteText2(29, 196, "\x18\x19:选择端口");
		LCD_WriteText2(2, 220, "请长按确定键以保存设置");

		LCD_LedGPIO_Update(tempdata->sel, tempdata->PinNum);

LCD_FLAG_Using = lcd_flag_temp;
if(LCD_FLAG_UpdateIrq == 1 && LCD_FLAG_Using == 0){
	EXTI_GenerateSWInterrupt(EXTI_Line1);
}
	}
}
void LCD_LedGPIO(KEY_TypeDef key){
	LedGPIO_fundata* tempdata;
	uint8_t lcd_flag_temp;
	uint8_t i;
	
	lcd_flag_temp = LCD_FLAG_Using;
	tempdata = (LedGPIO_fundata*) LEDColorMenu.fundata;

	switch(key){
		case KEY3:
		case KEY3_Cont:  if(tempdata->sel==0) tempdata->sel=16; else tempdata->sel--; break;
		case KEY4:
		case KEY4_Cont:  if(tempdata->sel==16) tempdata->sel=0; else tempdata->sel++; break;
		case KEY1:{
			if(tempdata->sel == 0){
				if(tempdata->crossway<CROSSWAY_MAX-1) tempdata->crossway ++;
				else tempdata->crossway = 0;

				for(i=0; i<16; i++){
					tempdata->PinNum[i] = my_Schedule.Crossway_LED_Sn[tempdata->crossway].LEDColor_Sn[i];  //存储灯组1的第i个已定义灯色输出端口序号
				}
				for(i=0; i<6; i++){
					tempdata->used_shadow[i] = used_if_record[i]; //保存端口占用记录
				}
			}
			else{
la1:
				//删除原记录
				tempdata->used_shadow[(tempdata->PinNum[tempdata->sel-1]-1)/16] ^= 0x0001<<((tempdata->PinNum[tempdata->sel-1]-1)%16);
				if(tempdata->PinNum[tempdata->sel-1]<96){
					tempdata->PinNum[tempdata->sel-1]++;
					//检测此序号是否被使用
					if(ledgpio_map_check_used(tempdata->PinNum[tempdata->sel-1]) == ERROR)
						goto la1;
				} 
				else tempdata->PinNum[tempdata->sel-1]=0;
				if(tempdata->PinNum[tempdata->sel-1] != 0x00)
					tempdata->used_shadow[(tempdata->PinNum[tempdata->sel-1]-1)/16] |= 0x0001<<((tempdata->PinNum[tempdata->sel-1]-1)%16);
			}
			break;
		}
		case KEY1_Cont:{
			if(tempdata->sel != 0){
//la2:
				if(tempdata->PinNum[tempdata->sel-1]<96){
					tempdata->PinNum[tempdata->sel-1]++;
					//if(ledgpio_map_check_used(tempdata->PinNum[tempdata->sel-1]) == ERROR)
						//goto la2;
				} 
				else tempdata->PinNum[tempdata->sel-1]=0;
			}
			break;
		}
		case KEY2:{
			if(tempdata->sel == 0){
				if(tempdata->crossway>0) tempdata->crossway --;
				else tempdata->crossway = 5;

				for(i=0; i<16; i++){
					tempdata->PinNum[i] = my_Schedule.Crossway_LED_Sn[tempdata->crossway].LEDColor_Sn[i];  //存储灯组1的第i个已定义灯色输出端口序号
				}
			}
			else{
//la3:
				if(tempdata->PinNum[tempdata->sel-1]>0) tempdata->PinNum[tempdata->sel-1]--;
				else tempdata->PinNum[tempdata->sel-1]=96; 
				//if(ledgpio_map_check_used(tempdata->PinNum[tempdata->sel-1]) == ERROR && tempdata->PinNum[tempdata->sel-1] != 0x00)
					//goto la3;
			}
			break;
		}	
		case KEY2_Cont:{
			if(tempdata->sel != 0){
//la4:
				if(tempdata->PinNum[tempdata->sel-1]>0) tempdata->PinNum[tempdata->sel-1]--; 
				else tempdata->PinNum[tempdata->sel-1]=96;
				//if(ledgpio_map_check_used(tempdata->PinNum[tempdata->sel-1]) == ERROR && tempdata->PinNum[tempdata->sel-1] != 0x00)
					//goto la4;
			}
			break;
		}
		case KEY5_Cont:{
			RTC_WaitForLastTask();
			RTC_ITConfig(RTC_IT_SEC, DISABLE);

			//从不使用（0）到分配端口要记录到used_if_record里面，从使用到不使用，要修改记录
			for(i=0; i<16; i++){
				my_Schedule.Crossway_LED_Sn[tempdata->crossway].LEDColor_Sn[i] = tempdata->PinNum[i];
			}
			
			RTC_WaitForLastTask();
			RTC_ITConfig(RTC_IT_SEC, ENABLE);

			//tempdata->KEY_FLAG_Return = 1;
			tempdata->state = lcdapp_saved;
			break;
		}
		case KEY6:	tempdata->KEY_FLAG_Return = 1;  break;
		case Nothing: goto refresh;						//定时刷屏进入，非按键触发进入
		default: break;
	}
	
	goto have_key;

refresh:
	if(tempdata->state != lcdapp_ready){
		tempdata->state = lcdapp_ready;
		keyfunction_clear_help();
	}
	return;

have_key:
	KEY_PressMinus();

	switch(tempdata->state){
		case lcdapp_saved:{
			keyfunction_help("设置成功");
			break;
		}
		default: break;
	}

	if(tempdata->KEY_FLAG_Return == 1){
		LEDColorMenu.cur_funptr = LEDColorMenu.key_function;
		free(tempdata);
		LEDColorMenu.fundata = NULL;   //可用可不用

		LCD_ClearMenuText();
		LEDColorMenu.item_display();
		LEDColorMenu.help_display();		
		return;
	}

LCD_FLAG_Using = 1;
	
	LCD_LedGPIO_Update(tempdata->sel, tempdata->PinNum);

LCD_FLAG_Using = lcd_flag_temp;
if(LCD_FLAG_UpdateIrq == 1 && LCD_FLAG_Using == 0){
	EXTI_GenerateSWInterrupt(EXTI_Line1);
}
}
/**********************************************************************
保存灯组接口设置
**********************************************************************/
void ledgpio_saveto_e2prom(void){
	uint8_t crossway;
	uint8_t* data_ptr;

	RTC_WaitForLastTask();
	RTC_ITConfig(RTC_IT_SEC, DISABLE);
	for(crossway=0; crossway<CROSSWAY_MAX; crossway++){
		data_ptr = my_Schedule.Crossway_LED_Sn[crossway].LEDColor_Sn;
		e2prom_write_bytes(LED_SN_OFFSET+crossway*LED_SN_SIZE, LED_SN_SIZE, data_ptr);
		delay_100ns(1000);
	}
	RTC_WaitForLastTask();
	RTC_ITConfig(RTC_IT_SEC, ENABLE);
}
/**********************************************************************
恢复灯组接口设置到出厂状态
**********************************************************************/
void reset_ledgpio_map(void){
	RTC_WaitForLastTask();
	RTC_ITConfig(RTC_IT_SEC, DISABLE);
	my_Schedule.Crossway_LED_Sn[0].LEDColor_Sn[0] = 1;
	my_Schedule.Crossway_LED_Sn[0].LEDColor_Sn[1] = 6;
	my_Schedule.Crossway_LED_Sn[0].LEDColor_Sn[2] = 9;
	my_Schedule.Crossway_LED_Sn[0].LEDColor_Sn[3] = 14;
	my_Schedule.Crossway_LED_Sn[0].LEDColor_Sn[4] = 5;
	my_Schedule.Crossway_LED_Sn[0].LEDColor_Sn[5] = 13;
	my_Schedule.Crossway_LED_Sn[0].LEDColor_Sn[6] = 2;
	my_Schedule.Crossway_LED_Sn[0].LEDColor_Sn[7] = 7;
	my_Schedule.Crossway_LED_Sn[0].LEDColor_Sn[8] = 10;
	my_Schedule.Crossway_LED_Sn[0].LEDColor_Sn[9] = 15;
	my_Schedule.Crossway_LED_Sn[0].LEDColor_Sn[10] = 3;
	my_Schedule.Crossway_LED_Sn[0].LEDColor_Sn[11] = 8;
	my_Schedule.Crossway_LED_Sn[0].LEDColor_Sn[12] = 11;
	my_Schedule.Crossway_LED_Sn[0].LEDColor_Sn[13] = 16;
	my_Schedule.Crossway_LED_Sn[0].LEDColor_Sn[14] = 4;
	my_Schedule.Crossway_LED_Sn[0].LEDColor_Sn[15] = 12;

	my_Schedule.Crossway_LED_Sn[1].LEDColor_Sn[0] = 17;
	my_Schedule.Crossway_LED_Sn[1].LEDColor_Sn[1] = 22;
	my_Schedule.Crossway_LED_Sn[1].LEDColor_Sn[2] = 25;
	my_Schedule.Crossway_LED_Sn[1].LEDColor_Sn[3] = 32;
	my_Schedule.Crossway_LED_Sn[1].LEDColor_Sn[4] = 21;
	my_Schedule.Crossway_LED_Sn[1].LEDColor_Sn[5] = 28;
	my_Schedule.Crossway_LED_Sn[1].LEDColor_Sn[6] = 18;
	my_Schedule.Crossway_LED_Sn[1].LEDColor_Sn[7] = 23;
	my_Schedule.Crossway_LED_Sn[1].LEDColor_Sn[8] = 26;
	my_Schedule.Crossway_LED_Sn[1].LEDColor_Sn[9] = 31;
	my_Schedule.Crossway_LED_Sn[1].LEDColor_Sn[10] = 19;
	my_Schedule.Crossway_LED_Sn[1].LEDColor_Sn[11] = 24;
	my_Schedule.Crossway_LED_Sn[1].LEDColor_Sn[12] = 29;
	my_Schedule.Crossway_LED_Sn[1].LEDColor_Sn[13] = 30;
	my_Schedule.Crossway_LED_Sn[1].LEDColor_Sn[14] = 20;
	my_Schedule.Crossway_LED_Sn[1].LEDColor_Sn[15] = 19;

	my_Schedule.Crossway_LED_Sn[2].LEDColor_Sn[0] = 33;
	my_Schedule.Crossway_LED_Sn[2].LEDColor_Sn[1] = 36;
	my_Schedule.Crossway_LED_Sn[2].LEDColor_Sn[2] = 39;
	my_Schedule.Crossway_LED_Sn[2].LEDColor_Sn[3] = 42;
	my_Schedule.Crossway_LED_Sn[2].LEDColor_Sn[4] = 45;
	my_Schedule.Crossway_LED_Sn[2].LEDColor_Sn[5] = 47;
	my_Schedule.Crossway_LED_Sn[2].LEDColor_Sn[6] = 34;
	my_Schedule.Crossway_LED_Sn[2].LEDColor_Sn[7] = 37;
	my_Schedule.Crossway_LED_Sn[2].LEDColor_Sn[8] = 40;
	my_Schedule.Crossway_LED_Sn[2].LEDColor_Sn[9] = 43;
	my_Schedule.Crossway_LED_Sn[2].LEDColor_Sn[10] = 35;
	my_Schedule.Crossway_LED_Sn[2].LEDColor_Sn[11] = 38;
	my_Schedule.Crossway_LED_Sn[2].LEDColor_Sn[12] = 41;
	my_Schedule.Crossway_LED_Sn[2].LEDColor_Sn[13] = 44;
	my_Schedule.Crossway_LED_Sn[2].LEDColor_Sn[14] = 46;
	my_Schedule.Crossway_LED_Sn[2].LEDColor_Sn[15] = 48;

	my_Schedule.Crossway_LED_Sn[3].LEDColor_Sn[0] = 37;
	my_Schedule.Crossway_LED_Sn[3].LEDColor_Sn[1] = 41;
	my_Schedule.Crossway_LED_Sn[3].LEDColor_Sn[2] = 45;
	my_Schedule.Crossway_LED_Sn[3].LEDColor_Sn[3] = 0;
	my_Schedule.Crossway_LED_Sn[3].LEDColor_Sn[4] = 0;
	my_Schedule.Crossway_LED_Sn[3].LEDColor_Sn[5] = 0;
	my_Schedule.Crossway_LED_Sn[3].LEDColor_Sn[6] = 38;
	my_Schedule.Crossway_LED_Sn[3].LEDColor_Sn[7] = 42;
	my_Schedule.Crossway_LED_Sn[3].LEDColor_Sn[8] = 46;
	my_Schedule.Crossway_LED_Sn[3].LEDColor_Sn[9] = 0;
	my_Schedule.Crossway_LED_Sn[3].LEDColor_Sn[10] = 39;
	my_Schedule.Crossway_LED_Sn[3].LEDColor_Sn[11] = 43;
	my_Schedule.Crossway_LED_Sn[3].LEDColor_Sn[12] = 47;
	my_Schedule.Crossway_LED_Sn[3].LEDColor_Sn[13] = 0;
	my_Schedule.Crossway_LED_Sn[3].LEDColor_Sn[14] = 0;
	my_Schedule.Crossway_LED_Sn[3].LEDColor_Sn[15] = 0;

	my_Schedule.Crossway_LED_Sn[4].LEDColor_Sn[0] = 0;
	my_Schedule.Crossway_LED_Sn[4].LEDColor_Sn[1] = 0;
	my_Schedule.Crossway_LED_Sn[4].LEDColor_Sn[2] = 0;
	my_Schedule.Crossway_LED_Sn[4].LEDColor_Sn[3] = 0;
	my_Schedule.Crossway_LED_Sn[4].LEDColor_Sn[4] = 0;
	my_Schedule.Crossway_LED_Sn[4].LEDColor_Sn[5] = 0;
	my_Schedule.Crossway_LED_Sn[4].LEDColor_Sn[6] = 0;
	my_Schedule.Crossway_LED_Sn[4].LEDColor_Sn[7] = 0;
	my_Schedule.Crossway_LED_Sn[4].LEDColor_Sn[8] = 0;
	my_Schedule.Crossway_LED_Sn[4].LEDColor_Sn[9] = 0;
	my_Schedule.Crossway_LED_Sn[4].LEDColor_Sn[10] = 0;
	my_Schedule.Crossway_LED_Sn[4].LEDColor_Sn[11] = 0;
	my_Schedule.Crossway_LED_Sn[4].LEDColor_Sn[12] = 0;
	my_Schedule.Crossway_LED_Sn[4].LEDColor_Sn[13] = 0;
	my_Schedule.Crossway_LED_Sn[4].LEDColor_Sn[14] = 0;
	my_Schedule.Crossway_LED_Sn[4].LEDColor_Sn[15] = 0;

	my_Schedule.Crossway_LED_Sn[5].LEDColor_Sn[0] = 0;
	my_Schedule.Crossway_LED_Sn[5].LEDColor_Sn[1] = 0;
	my_Schedule.Crossway_LED_Sn[5].LEDColor_Sn[2] = 0;
	my_Schedule.Crossway_LED_Sn[5].LEDColor_Sn[3] = 0;
	my_Schedule.Crossway_LED_Sn[5].LEDColor_Sn[4] = 0;
	my_Schedule.Crossway_LED_Sn[5].LEDColor_Sn[5] = 0;
	my_Schedule.Crossway_LED_Sn[5].LEDColor_Sn[6] = 0;
	my_Schedule.Crossway_LED_Sn[5].LEDColor_Sn[7] = 0;
	my_Schedule.Crossway_LED_Sn[5].LEDColor_Sn[8] = 0;
	my_Schedule.Crossway_LED_Sn[5].LEDColor_Sn[9] = 0;
	my_Schedule.Crossway_LED_Sn[5].LEDColor_Sn[10] = 0;
	my_Schedule.Crossway_LED_Sn[5].LEDColor_Sn[11] = 0;
	my_Schedule.Crossway_LED_Sn[5].LEDColor_Sn[12] = 0;
	my_Schedule.Crossway_LED_Sn[5].LEDColor_Sn[13] = 0;
	my_Schedule.Crossway_LED_Sn[5].LEDColor_Sn[14] = 0;
	my_Schedule.Crossway_LED_Sn[5].LEDColor_Sn[15] = 0;
	RTC_WaitForLastTask();
	RTC_ITConfig(RTC_IT_SEC, ENABLE);
}
/**********************************************************************
闪烁设置菜单函数
**********************************************************************/
//公共更新子程序
void flashfunc_update(uint8_t sel, uint8_t crossway, uint16_t flashdata){
	uint8_t i;
	uint8_t lcd_flag_temp = LCD_FLAG_Using;

LCD_FLAG_Using = 1;	
	for(i=0; i<17; i++){
		if(i == sel){
			LCD_Inv();
		}
		LCD_SetCursor(Func_SetFlash_Place[i][0], Func_SetFlash_Place[i][1]);
		if(i != 16){
			LCD_WriteText1(((flashdata&(0x0001<<i))==0x0000)?	(uint8_t*)Tick_Cross_Table[0]:(uint8_t*)Tick_Cross_Table[1]);
		}
		else{
			//LCD_WriteText1("灯组");
			LCD_WriteText1((uint8_t*)Weekday_Table[crossway+1]);
		}
		if(i == sel){
			LCD_NoInv();
		}
	}
LCD_FLAG_Using = lcd_flag_temp;
if(LCD_FLAG_UpdateIrq == 1 && LCD_FLAG_Using == 0){
	EXTI_GenerateSWInterrupt(EXTI_Line1);
}
}
/**********************************************************************
闪烁许可设置
**********************************************************************/
typedef struct{
	uint8_t sel;
	uint8_t crossway;
	uint16_t flash_allowed[CROSSWAY_MAX];
	lcd_appstate state;
}flash_fundata;
void flash_allowed_datainit(void){
	uint8_t i;
	flash_fundata* tempdata;
	
	FlashMenu.fundata = calloc(1, sizeof(flash_fundata));
	tempdata = (flash_fundata*) FlashMenu.fundata;

	if(tempdata != NULL){
		tempdata->sel = 0;
		tempdata->crossway = 0;
		tempdata->state = lcdapp_ready;
		for(i=0; i<CROSSWAY_MAX; i++)  tempdata->flash_allowed[i] = my_Schedule.Crossway_Flash[i].FLASH_Allowence;
	}

	flash_allowed_main(Nothing);
}
void Set_FlashAllowence_Interface(void){
	uint8_t lcd_flag_temp;

	lcd_flag_temp = LCD_FLAG_Using;

LCD_FLAG_Using = 1;
	LCD_ClearItemAllText();
	
	LCD_SetAppwindow(0, 239, 0, 39);
	
	LCD_WriteText2(2, 75, "\x11灯组  \x10");
	LCD_WriteText2(12, 93, "绿  黄  红");
	LCD_WriteText2(5, 111, "左转 [  ] [  ] [  ]");
	LCD_WriteText2(5, 129, "直行 [  ] [  ] [  ]");
	LCD_WriteText2(5, 147, "右转 [  ] [  ] [  ]");
	LCD_WriteText2(5, 165, "调头 [  ] [  ] [  ]");
	LCD_WriteText2(5, 183, "人行1[  ]      [  ]");
	LCD_WriteText2(5, 201, "人行2[  ]      [  ]");

	LCD_WriteText2(29, 136, "√:允许闪烁");
	LCD_WriteText2(29, 156, "×:禁止闪烁");

	LCD_WriteText2(29, 176, "\x1A\x1B:移动");
	LCD_WriteText2(29, 196, "\x18\x19:设置");
	LCD_WriteText2(2, 218, "长按确认键进行保存"); 
LCD_FLAG_Using = lcd_flag_temp;
if(LCD_FLAG_UpdateIrq == 1 && LCD_FLAG_Using == 0){
	EXTI_GenerateSWInterrupt(EXTI_Line1);
}
}
void Set_FlashAllowence(KEY_TypeDef key){
	flash_fundata* tempdata;
	//uint8_t lcd_flag_temp;
	uint8_t i;

	tempdata = (flash_fundata*) FlashMenu.fundata;
	switch (key){
		case KEY3:  if(tempdata->sel==0) tempdata->sel=16; else tempdata->sel--; break;
		case KEY4:  if(tempdata->sel==16) tempdata->sel=0; else tempdata->sel++; break;
		case KEY1:{
			if(tempdata->sel != 16){
				tempdata->flash_allowed[tempdata->crossway] = tempdata->flash_allowed[tempdata->crossway] ^ (0x0001<<(tempdata->sel));
			}
			else{
				if(tempdata->crossway == 5)  tempdata->crossway = 0;
				else tempdata->crossway ++;
			}
			break;
		}
		case KEY2:{
			if(tempdata->sel != 16){
				tempdata->flash_allowed[tempdata->crossway] = tempdata->flash_allowed[tempdata->crossway] ^ (0x0001<<(tempdata->sel));
			}
			else{
				if(tempdata->crossway == 0)  tempdata->crossway = 5;
				else  tempdata->crossway --;
			}
			break;
		}
		case KEY5_Cont:  {
			for(i=0; i<6; i++){
				my_Schedule.Crossway_Flash[i].FLASH_Allowence = tempdata->flash_allowed[i];
			}
			tempdata->state = lcdapp_saved;
			//KEY_FLAG_Return = 1;
			break;
		}
		case KEY6:	tempdata->state = lcdapp_return;  break;
		case Nothing: goto flash_allowence_refresh; //break;
		default: break;
	}
	goto flash_allowence_havekey;

flash_allowence_refresh:
	switch(tempdata->state){
		case lcdapp_cmd: break;
		default:{
			keyfunction_clear_help();
			tempdata->state = lcdapp_cmd;
			break;
		} 
	}
	return;

flash_allowence_havekey:
	KEY_PressMinus();

	switch(tempdata->state){
		case lcdapp_saved:{
			keyfunction_help("保存成功");			
		}
		default: break;
	}
	flashfunc_update(tempdata->sel, tempdata->crossway, tempdata->flash_allowed[tempdata->crossway]);
}
void flash_allowed_main(KEY_TypeDef key){
	flash_fundata* tempdata;
	tempdata = (flash_fundata*) FlashMenu.fundata;

	switch(tempdata->state){
		case lcdapp_ready:{
			Set_FlashAllowence_Interface(); 
			flashfunc_update(tempdata->sel, tempdata->crossway, tempdata->flash_allowed[tempdata->crossway]); 
			tempdata->state = lcdapp_cmd; 
			break;
		}
		case lcdapp_saved:
		case lcdapp_cmd: Set_FlashAllowence(key); break;
		case lcdapp_return:{
			FlashMenu.cur_funptr = FlashMenu.key_function;
			free(tempdata);
			FlashMenu.fundata = NULL;   //可用可不用
	
			LCD_ClearMenuText();
			FlashMenu.item_display();
			FlashMenu.help_display();
			break;
		}
	}
}

/**********************************************************************
日昼常闪设置
**********************************************************************/
typedef struct{
	uint8_t sel;
	uint8_t crossway;
	uint16_t dayflash[CROSSWAY_MAX];
	lcd_appstate state;
}dayflash_fundata;
void dayflash_datainit(void){
	uint8_t i;
	dayflash_fundata* tempdata;
	
	FlashMenu.fundata = calloc(1, sizeof(dayflash_fundata));
	tempdata = (dayflash_fundata*) FlashMenu.fundata;

	if(tempdata != NULL){
		tempdata->sel = 0;
		tempdata->crossway = 0;
		tempdata->state = lcdapp_ready;
		for(i=0; i<CROSSWAY_MAX; i++)  tempdata->dayflash[i] = my_Schedule.Crossway_Flash[i].Day_FLASH_ON;
	}

	dayflash_main(Nothing);
}
void dayflash_Interface(void){
	uint8_t lcd_flag_temp;

	lcd_flag_temp = LCD_FLAG_Using;

LCD_FLAG_Using = 1;
	LCD_ClearItemAllText();
	
	LCD_SetAppwindow(0, 239, 0, 39);
	
	LCD_WriteText2(2, 75, "\x11灯组  \x10");
	LCD_WriteText2(12, 93, "绿  黄  红");
	LCD_WriteText2(5, 111, "左转 [  ] [  ] [  ]");
	LCD_WriteText2(5, 129, "直行 [  ] [  ] [  ]");
	LCD_WriteText2(5, 147, "右转 [  ] [  ] [  ]");
	LCD_WriteText2(5, 165, "调头 [  ] [  ] [  ]");
	LCD_WriteText2(5, 183, "人行1[  ]      [  ]");
	LCD_WriteText2(5, 201, "人行2[  ]      [  ]");

	LCD_WriteText2(29, 136, "√:白天常闪");
	LCD_WriteText2(29, 156, "×:不闪烁");

	LCD_WriteText2(29, 176, "\x1A\x1B:移动");
	LCD_WriteText2(29, 196, "\x18\x19:设置");
	LCD_WriteText2(2, 218, "长按确认键进行保存"); 
LCD_FLAG_Using = lcd_flag_temp;
if(LCD_FLAG_UpdateIrq == 1 && LCD_FLAG_Using == 0){
	EXTI_GenerateSWInterrupt(EXTI_Line1);
}
}
void dayflash(KEY_TypeDef key){
	dayflash_fundata* tempdata;
	uint8_t i;

	tempdata = (dayflash_fundata*) FlashMenu.fundata;

	switch (key){
		case KEY3:  if(tempdata->sel==0) tempdata->sel=16; else tempdata->sel--; break;
		case KEY4:  if(tempdata->sel==16) tempdata->sel=0; else tempdata->sel++; break;
		case KEY1:	{
			if(tempdata->sel != 16){
				tempdata->dayflash[tempdata->crossway] = tempdata->dayflash[tempdata->crossway] ^ (0x0001<<(tempdata->sel));
			}
			else{
				if(tempdata->crossway == CROSSWAY_MAX-1)  tempdata->crossway = 0;
				else  tempdata->crossway ++;
			}
			break;
		}
		case KEY2:	{
			if(tempdata->sel != 16){
				tempdata->dayflash[tempdata->crossway] = tempdata->dayflash[tempdata->crossway] ^ (0x0001<<(tempdata->sel));
			}
			else{
				if(tempdata->crossway == 0)  tempdata->crossway = CROSSWAY_MAX-1;
				else  tempdata->crossway --;
			}
			break;
		}
		case KEY5_Cont: {
			for(i=0; i<CROSSWAY_MAX; i++){
				my_Schedule.Crossway_Flash[i].Day_FLASH_ON = tempdata->dayflash[i];
			}
			tempdata->state = lcdapp_saved;

			//KEY_FLAG_Return = 1;
			break;
		}
		case KEY6:	tempdata->state = lcdapp_return;  break;
		case Nothing: goto dayflash_refresh;
		default: break;
	}
	goto dayflash_havekey;

dayflash_refresh:
	switch(tempdata->state){
		case lcdapp_cmd: break;
		default:{
			keyfunction_clear_help();
			tempdata->state = lcdapp_cmd;
			break;
		}
	}
	return;

dayflash_havekey:
	KEY_PressMinus();

	switch(tempdata->state){
		case lcdapp_saved:{
			keyfunction_help("保存成功");
			break;
		}
		default: break;
	}

	flashfunc_update(tempdata->sel, tempdata->crossway, tempdata->dayflash[tempdata->crossway]);
}
void dayflash_main(KEY_TypeDef key){
	dayflash_fundata* tempdata;
	tempdata = (dayflash_fundata*) FlashMenu.fundata;

	switch(tempdata->state){
		case lcdapp_ready:{
			dayflash_Interface(); 
			flashfunc_update(tempdata->sel, tempdata->crossway, tempdata->dayflash[tempdata->crossway]); 
			tempdata->state = lcdapp_cmd; 
			break;
		}
		case lcdapp_saved:
		case lcdapp_cmd: dayflash(key); break;
		case lcdapp_return:{
			FlashMenu.cur_funptr = FlashMenu.key_function;
			free(tempdata);
			FlashMenu.fundata = NULL;   //可用可不用
	
			LCD_ClearMenuText();
			FlashMenu.item_display();
			FlashMenu.help_display();
			break;
		}
	}
}
/**********************************************************************
晚间常闪设置(与日昼常闪设置非常相似，直接套用)
**********************************************************************/
typedef struct{
	uint8_t sel;
	uint8_t crossway;
	uint16_t nightflash[CROSSWAY_MAX];
	lcd_appstate state;
}nightflash_fundata;
void nightflash_datainit(void){
	uint8_t i;
	nightflash_fundata* tempdata;
	
	FlashMenu.fundata = calloc(1, sizeof(nightflash_fundata));
	tempdata = (nightflash_fundata*) FlashMenu.fundata;

	if(tempdata != NULL){
		tempdata->sel = 0;
		tempdata->crossway = 0;
		tempdata->state = lcdapp_ready;
		for(i=0; i<CROSSWAY_MAX; i++)  tempdata->nightflash[i] = my_Schedule.Crossway_Flash[i].Night_FLASH_ON;
	}

	nightflash_main(Nothing);
}
void nightflash_Interface(void){
	uint8_t lcd_flag_temp;

	lcd_flag_temp = LCD_FLAG_Using;

LCD_FLAG_Using = 1;
	LCD_ClearItemAllText();
	
	LCD_SetAppwindow(0, 239, 0, 39);
	
	LCD_WriteText2(2, 80, "\x11灯组  \x10");
	LCD_WriteText2(12, 93, "绿  黄  红");
	LCD_WriteText2(5, 111, "左转 [  ] [  ] [  ]");
	LCD_WriteText2(5, 129, "直行 [  ] [  ] [  ]");
	LCD_WriteText2(5, 147, "右转 [  ] [  ] [  ]");
	LCD_WriteText2(5, 165, "调头 [  ] [  ] [  ]");
	LCD_WriteText2(5, 183, "人行1[  ]      [  ]");
	LCD_WriteText2(5, 201, "人行2[  ]      [  ]");

	LCD_WriteText2(29, 136, "√:晚间常闪");
	LCD_WriteText2(29, 156, "×:不闪烁");

	LCD_WriteText2(29, 176, "\x1A\x1B:移动");
	LCD_WriteText2(29, 196, "\x18\x19:设置");
	LCD_WriteText2(2, 218, "长按确认键进行保存"); 
LCD_FLAG_Using = lcd_flag_temp;
if(LCD_FLAG_UpdateIrq == 1 && LCD_FLAG_Using == 0){
	EXTI_GenerateSWInterrupt(EXTI_Line1);
}
}
void nightflash(KEY_TypeDef key){
	nightflash_fundata* tempdata;
	uint8_t lcd_flag_temp;
	uint8_t i;

	lcd_flag_temp = LCD_FLAG_Using;
	tempdata = (nightflash_fundata*) FlashMenu.fundata;

	switch (key){
		case KEY3:  if(tempdata->sel==0) tempdata->sel=16; else tempdata->sel--; break;
		case KEY4:  if(tempdata->sel==16) tempdata->sel=0; else tempdata->sel++; break;
		case KEY1:	{
			if(tempdata->sel != 16){
				tempdata->nightflash[tempdata->crossway] = tempdata->nightflash[tempdata->crossway] ^ (0x0001<<(tempdata->sel));
			}
			else{
				if(tempdata->crossway == CROSSWAY_MAX-1)  tempdata->crossway = 0;
				else  tempdata->crossway ++;
			}
			break;
		}
		case KEY2:	{
			if(tempdata->sel != 16){
				tempdata->nightflash[tempdata->crossway] = tempdata->nightflash[tempdata->crossway] ^ (0x0001<<(tempdata->sel));
			}
			else{
				if(tempdata->crossway == 0)  tempdata->crossway = CROSSWAY_MAX-1;
				else  tempdata->crossway --;
			}
			break;
		}
		case KEY5_Cont: {
			for(i=0; i<6; i++){
				my_Schedule.Crossway_Flash[i].Night_FLASH_ON = tempdata->nightflash[i];
			}
			tempdata->state = lcdapp_saved;

			//KEY_FLAG_Return = 1;
			break;
		}
		case KEY6:	tempdata->state = lcdapp_return;  break;
		case Nothing: goto dayflash_refresh;
		default: break;
	}
	goto dayflash_havekey;

dayflash_refresh:
	switch(tempdata->state){
		case lcdapp_cmd: break;
		default:{
			keyfunction_clear_help();
			tempdata->state = lcdapp_cmd;
			break;
		}
	}
	return;

dayflash_havekey:
	KEY_PressMinus();

LCD_FLAG_Using = 1;
	switch(tempdata->state){
		case lcdapp_saved:{
			keyfunction_help("保存成功");
			break;
		}
		default: break;
	}

	flashfunc_update(tempdata->sel, tempdata->crossway, tempdata->nightflash[tempdata->crossway]);
LCD_FLAG_Using = lcd_flag_temp;
if(LCD_FLAG_UpdateIrq == 1 && LCD_FLAG_Using == 0){
	EXTI_GenerateSWInterrupt(EXTI_Line1);
}
}
void nightflash_main(KEY_TypeDef key){
	nightflash_fundata* tempdata;
	tempdata = (nightflash_fundata*) FlashMenu.fundata;

	switch(tempdata->state){
		case lcdapp_ready:{
			nightflash_Interface(); 
			flashfunc_update(tempdata->sel, tempdata->crossway, tempdata->nightflash[tempdata->crossway]); 
			tempdata->state = lcdapp_cmd; 
			break;
		}
		case lcdapp_saved:
		case lcdapp_cmd: dayflash(key); break;
		case lcdapp_return:{
			FlashMenu.cur_funptr = FlashMenu.key_function;
			free(tempdata);
			FlashMenu.fundata = NULL;   //可用可不用
	
			LCD_ClearMenuText();
			FlashMenu.item_display();
			FlashMenu.help_display();
			break;
		}
	}
}
/**********************************************************************
相位表设置
**********************************************************************/
typedef struct{
	uint8_t sel;
	uint8_t period_sel;
	uint8_t phase_sel;
	uint8_t crossway;
	uint8_t cutdown;
	uint16_t crossway_phase;
	lcd_appstate state;
}phase_fundata;
void set_phase_datainit(void){
	phase_fundata* tempdata;
	
	PhaseMenu.fundata = calloc(1, sizeof(phase_fundata));
	tempdata = (phase_fundata*) PhaseMenu.fundata;

	if(tempdata != NULL){
		tempdata->sel = 0;
		tempdata->period_sel = 0;
		tempdata->phase_sel = 0;
		tempdata->cutdown = my_Schedule.PhaseTable[tempdata->period_sel].Last_Time[tempdata->phase_sel];
		tempdata->crossway_phase = my_Schedule.PhaseTable[tempdata->period_sel].Crossway_LED[tempdata->crossway][tempdata->phase_sel];
		tempdata->state = lcdapp_ready;
	}
	else{
		tempdata->state = lcdapp_return;
	}
	set_phase_main(Nothing);
}
const uint8_t Func_SetPhase_Place[10][2] = {{7, 80}, {8, 98}, {19, 98}, {8, 126}, {18, 126}, {8, 146}, {18, 146}, {8, 166}, {18, 166}, {15, 186}};
void set_phase_update(uint8_t crossway, uint8_t sel, uint16_t crossway_phase, uint8_t phase_sel, uint8_t period_sel, uint8_t cutdown){
	uint8_t i;
	uint8_t lcd_flag_temp;

	lcd_flag_temp = LCD_FLAG_Using;
LCD_FLAG_Using = 1;	
	for(i=0; i<10; i++){
		if(i == sel){
			LCD_Inv();
		}
		switch (i){
			case 0: {
				LCD_WriteText2(7, 80, (uint8_t*)Weekday_Table[crossway+1]);
				break;
			}
			case 1: {
				LCD_WriteChar2(8, 98, Number_Table[(period_sel+1)/10]);
				LCD_WriteChar1(Number_Table[(period_sel+1)%10]);
				break;
			}
			case 2: {
				LCD_WriteChar2(19, 98, Number_Table[(phase_sel+1)/10]);
				LCD_WriteChar1(Number_Table[(phase_sel+1)%10]);
				break;
			}
			case 3: {	//当绿红黄都为0时，如LG LY LR位均为0时，则该灯不亮
				LCD_SetCursor(8, 126);
				if((crossway_phase&LeftGreen) != 0x0000){
					LCD_WriteText1((uint8_t*)LedColor_Table[0]);
				}
				else if((crossway_phase&LeftRed) != 0x0000){
					LCD_WriteText1((uint8_t*)LedColor_Table[1]);
				}
				else if((crossway_phase&LeftYellow) != 0x0000){
					LCD_WriteText1((uint8_t*)LedColor_Table[2]);
				}
				else{
					LCD_WriteText1((uint8_t*)Tick_Cross_Table[0]);
				}
				break;
			}
			case 4: {
				LCD_SetCursor(18, 126);
				if((crossway_phase&StraightGreen) != 0x0000){
					LCD_WriteText1((uint8_t*)LedColor_Table[0]);
				}
				else if((crossway_phase&StraightRed) != 0x0000){
					LCD_WriteText1((uint8_t*)LedColor_Table[1]);
				}
				else if((crossway_phase&StraightYellow) != 0x0000){
					LCD_WriteText1((uint8_t*)LedColor_Table[2]);
				}
				else{
					LCD_WriteText1((uint8_t*)Tick_Cross_Table[0]);
				}
				break;
			}
			case 5: {
				LCD_SetCursor(8, 146);
				if((crossway_phase&RightGreen) != 0x0000){
					LCD_WriteText1((uint8_t*)LedColor_Table[0]);
				}
				else if((crossway_phase&RightRed) != 0x0000){
					LCD_WriteText1((uint8_t*)LedColor_Table[1]);
				}
				else if((crossway_phase&RightYellow) != 0x0000){
					LCD_WriteText1((uint8_t*)LedColor_Table[2]);
				}
				else{
					LCD_WriteText1((uint8_t*)Tick_Cross_Table[0]);
				}
				break;
			}
			case 6: {
				LCD_SetCursor(18, 146);
				if((crossway_phase&BackGreen) != 0x0000){
					LCD_WriteText1((uint8_t*)LedColor_Table[0]);
				}
				else if((crossway_phase&BackRed) != 0x0000){
					LCD_WriteText1((uint8_t*)LedColor_Table[1]);
				}
				else if((crossway_phase&BackYellow) != 0x0000){
					LCD_WriteText1((uint8_t*)LedColor_Table[2]);
				}
				else{
					LCD_WriteText1((uint8_t*)Tick_Cross_Table[0]);
				}
				break;
			}
			case 7: {
				LCD_SetCursor(8, 166);
				if((crossway_phase&SideWalk1Green) != 0x0000){
					LCD_WriteText1((uint8_t*)LedColor_Table[0]);
				}
				else if((crossway_phase&SideWalk1Red) != 0x0000){
					LCD_WriteText1((uint8_t*)LedColor_Table[1]);
				}
				else{
					LCD_WriteText1((uint8_t*)Tick_Cross_Table[0]);
				}
				break;
			}
			case 8: {
				LCD_SetCursor(18, 166);
				if((crossway_phase&SideWalk2Green) != 0x0000){
					LCD_WriteText1((uint8_t*)LedColor_Table[0]);
				}
				else if((crossway_phase&SideWalk2Red) != 0x0000){
					LCD_WriteText1((uint8_t*)LedColor_Table[1]);
				}
				else{
					LCD_WriteText1((uint8_t*)Tick_Cross_Table[0]);
				}
				break;
			}
			case 9: {
				LCD_SetCursor(15, 186);
				LCD_WriteChar1(Number_Table[cutdown/10]);
				LCD_WriteChar1(Number_Table[cutdown%10]);
				break;
			}
			default: break;
		}
		if(i == sel){
			LCD_NoInv();
		}
	}
LCD_FLAG_Using = lcd_flag_temp;
if(LCD_FLAG_UpdateIrq == 1 && LCD_FLAG_Using == 0){
	EXTI_GenerateSWInterrupt(EXTI_Line1);
}
}
void set_phase_interface(void){
	phase_fundata* tempdata;
	uint8_t lcd_flag_temp = LCD_FLAG_Using;

	tempdata = (phase_fundata*) PhaseMenu.fundata;

LCD_FLAG_Using = 1;
	LCD_ClearItemAllText();
	
	LCD_SetAppwindow(0, 239, 0, 39);
	
	LCD_WriteText2(2, 80, "\x11灯组  \x10 (×:表示该灯不亮)" );
	LCD_WriteText2(2, 98, "时段【  】 相位【  】");
	LCD_WriteText2(2, 126, "左转 [  ] 直行 [  ]");
	LCD_WriteText2(2, 146, "右转 [  ] 调头 [  ]");
	LCD_WriteText2(2, 166, "人行1[  ] 人行2[  ]");
	LCD_WriteText2(2, 186, "相位运行时间:  秒");

	LCD_SetAppwindow(96, 170, 29, 38);
	LCD_WriteText2(29, 96, "长按确定键对设置进行保存，否则设置将会丢失");
	
	LCD_SetAppwindow(0, 239, 0, 39); 

	LCD_WriteText2(29, 176, "\x1A\x1B:移动");
	LCD_WriteText2(29, 196, "\x18\x19:选择");
	
	set_phase_update(tempdata->crossway, tempdata->sel, tempdata->crossway_phase, tempdata->phase_sel, tempdata->period_sel, tempdata->cutdown); 
LCD_FLAG_Using = lcd_flag_temp;
if(LCD_FLAG_UpdateIrq == 1 && LCD_FLAG_Using == 0){
	EXTI_GenerateSWInterrupt(EXTI_Line1);
}
}
void set_phase(KEY_TypeDef key){
	phase_fundata* tempdata;
	
	tempdata = (phase_fundata*) PhaseMenu.fundata;

	switch (key){
		case KEY3:  if(tempdata->sel==0) tempdata->sel=9; else tempdata->sel--; break;
		case KEY4:  if(tempdata->sel==9) tempdata->sel=0; else tempdata->sel++; break;
		case KEY1:	{
			switch (tempdata->sel){
				case 0: {
					if(tempdata->crossway==5) tempdata->crossway=0; 
					else tempdata->crossway++;
					
					//Set_PhaseUpdate(crossway, PeriodOfTimeNum, PhaseNum, &cutdown, &Crossway_Phase);
					tempdata->crossway_phase = my_Schedule.PhaseTable[tempdata->period_sel].Crossway_LED[tempdata->crossway][tempdata->phase_sel];
					tempdata->cutdown = my_Schedule.PhaseTable[tempdata->period_sel].Last_Time[tempdata->phase_sel];
					break;
				}
				case 1: {
					if(tempdata->period_sel<my_Schedule.TimeTable_Number-1) tempdata->period_sel++;
					tempdata->phase_sel = 0;
					tempdata->crossway_phase = my_Schedule.PhaseTable[tempdata->period_sel].Crossway_LED[tempdata->crossway][tempdata->phase_sel];
					tempdata->cutdown = my_Schedule.PhaseTable[tempdata->period_sel].Last_Time[tempdata->phase_sel];
					break;
				}
				case 2: {
					if(tempdata->phase_sel<my_Schedule.PhaseTable[tempdata->period_sel].Phase_Number-1) tempdata->phase_sel++; 
					tempdata->crossway_phase = my_Schedule.PhaseTable[tempdata->period_sel].Crossway_LED[tempdata->crossway][tempdata->phase_sel];
					tempdata->cutdown = my_Schedule.PhaseTable[tempdata->period_sel].Last_Time[tempdata->phase_sel];
					break;
				}
				case 3: {
					if((tempdata->crossway_phase&LeftGreen) != 0x0000){
						tempdata->crossway_phase = (tempdata->crossway_phase^LeftGreen)|LeftRed;
					}
					else if((tempdata->crossway_phase&LeftRed) != 0x0000){
						tempdata->crossway_phase = (tempdata->crossway_phase^LeftRed)|LeftYellow;
					}
					else if((tempdata->crossway_phase&LeftYellow) != 0x0000){
						tempdata->crossway_phase = tempdata->crossway_phase^LeftYellow;
					}
					else{
						tempdata->crossway_phase = tempdata->crossway_phase|LeftGreen;
					}
					break;
				}
				case 4:	{
					if((tempdata->crossway_phase&StraightGreen) != 0x00){
						tempdata->crossway_phase = (tempdata->crossway_phase^StraightGreen)|StraightRed;
					}
					else if((tempdata->crossway_phase&StraightRed) != 0x00){
						tempdata->crossway_phase = (tempdata->crossway_phase^StraightRed)|StraightYellow;
					}
					else if((tempdata->crossway_phase&StraightYellow) != 0x00){
						tempdata->crossway_phase = tempdata->crossway_phase^StraightYellow;
					}
					else{
						tempdata->crossway_phase = tempdata->crossway_phase|StraightGreen;
					}
					break;
				}
				case 5:	{
					if((tempdata->crossway_phase&RightGreen) != 0x00){
						tempdata->crossway_phase = (tempdata->crossway_phase^RightGreen)|RightRed;
					}
					else if((tempdata->crossway_phase&RightRed) != 0x00){
						tempdata->crossway_phase = (tempdata->crossway_phase^RightRed)|RightYellow;
					}
					else if((tempdata->crossway_phase&RightYellow) != 0x00){
						tempdata->crossway_phase = tempdata->crossway_phase^RightYellow;
					}
					else{
						tempdata->crossway_phase = tempdata->crossway_phase|RightGreen;
					}
					break;
				}
				case 6:	{
					if((tempdata->crossway_phase&BackGreen) != 0x0000){
						tempdata->crossway_phase = (tempdata->crossway_phase^BackGreen)|BackRed;
					}
					else if((tempdata->crossway_phase&BackRed) != 0x0000){
						tempdata->crossway_phase = (tempdata->crossway_phase^BackRed)|BackYellow;
					}
					else if((tempdata->crossway_phase&BackYellow) != 0x0000){
						tempdata->crossway_phase = tempdata->crossway_phase^BackYellow;
					}
					else{
						tempdata->crossway_phase = tempdata->crossway_phase|BackGreen;
					}
					break;
				}
				case 7:	{
					if((tempdata->crossway_phase&SideWalk1Green) != 0x0000){
						tempdata->crossway_phase = (tempdata->crossway_phase^SideWalk1Green)|SideWalk1Red;
					}
					else if((tempdata->crossway_phase&SideWalk1Red) != 0x0000){
						tempdata->crossway_phase = tempdata->crossway_phase^SideWalk1Red;
					}
					else{
						tempdata->crossway_phase = tempdata->crossway_phase|SideWalk1Green;
					}
					break;
				}
				case 8:	{
					if((tempdata->crossway_phase&SideWalk2Green) != 0x0000){
						tempdata->crossway_phase = (tempdata->crossway_phase^SideWalk2Green)|SideWalk2Red;
					}
					else if((tempdata->crossway_phase&SideWalk2Red) != 0x0000){
						tempdata->crossway_phase = tempdata->crossway_phase^SideWalk2Red;
					}
					else{
						tempdata->crossway_phase = tempdata->crossway_phase|SideWalk2Green;
					}
					break;
				}
				case 9: {
					if(tempdata->cutdown<99) tempdata->cutdown++;
					break;
				}
				default: break;
			}
			break;
		}
		case KEY2:	{
			switch (tempdata->sel){
				case 0: {
					if(tempdata->crossway==0) tempdata->crossway=5; 
					else tempdata->crossway--;
					tempdata->crossway_phase = my_Schedule.PhaseTable[tempdata->period_sel].Crossway_LED[tempdata->crossway][tempdata->phase_sel];
					break;
				}
				case 1: {
					if(tempdata->period_sel>0) tempdata->period_sel--;
					tempdata->phase_sel = 0;
					tempdata->crossway_phase = my_Schedule.PhaseTable[tempdata->period_sel].Crossway_LED[tempdata->crossway][tempdata->phase_sel]; 
					tempdata->cutdown = my_Schedule.PhaseTable[tempdata->period_sel].Last_Time[tempdata->phase_sel];
					break;
				}
				case 2: {
					if(tempdata->phase_sel>0) tempdata->phase_sel--; 
					tempdata->crossway_phase = my_Schedule.PhaseTable[tempdata->period_sel].Crossway_LED[tempdata->crossway][tempdata->phase_sel];
					break;
				}
				case 3: {
					if((tempdata->crossway_phase&LeftGreen) != 0x0000){
						tempdata->crossway_phase = tempdata->crossway_phase^LeftGreen;
					}
					else if((tempdata->crossway_phase&LeftRed) != 0x0000){
						tempdata->crossway_phase = (tempdata->crossway_phase^LeftRed)|LeftGreen;
					}
					else if((tempdata->crossway_phase&LeftYellow) != 0x0000){
						tempdata->crossway_phase = (tempdata->crossway_phase^LeftYellow)|LeftRed;
					}
					else{
						tempdata->crossway_phase = tempdata->crossway_phase|LeftYellow;
					}
					break;
				}
				case 4:	{
					if((tempdata->crossway_phase&StraightGreen) != 0x0000){
						tempdata->crossway_phase = tempdata->crossway_phase^StraightGreen;
					}
					else if((tempdata->crossway_phase&StraightRed) != 0x0000){
						tempdata->crossway_phase = (tempdata->crossway_phase^StraightRed)|StraightGreen;
					}
					else if((tempdata->crossway_phase&LeftYellow) != 0x0000){
						tempdata->crossway_phase = (tempdata->crossway_phase^StraightYellow)|StraightRed;
					}
					else{
						tempdata->crossway_phase = tempdata->crossway_phase|StraightYellow;
					}
					break;
				}
				case 5:	{
					if((tempdata->crossway_phase&RightGreen) != 0x0000){
						tempdata->crossway_phase = tempdata->crossway_phase^RightGreen;
					}
					else if((tempdata->crossway_phase&LeftRed) != 0x0000){
						tempdata->crossway_phase = (tempdata->crossway_phase^RightRed)|RightGreen;
					}
					else if((tempdata->crossway_phase&LeftYellow) != 0x0000){
						tempdata->crossway_phase = (tempdata->crossway_phase^RightYellow)|RightRed;
					}
					else{
						tempdata->crossway_phase = tempdata->crossway_phase|RightYellow;
					}
					break;
				}
				case 6:	{
					if((tempdata->crossway_phase&BackGreen) != 0x0000){
						tempdata->crossway_phase = tempdata->crossway_phase^BackGreen;
					}
					else if((tempdata->crossway_phase&LeftRed) != 0x0000){
						tempdata->crossway_phase = (tempdata->crossway_phase^BackRed)|BackGreen;
					}
					else if((tempdata->crossway_phase&LeftYellow) != 0x00){
						tempdata->crossway_phase = (tempdata->crossway_phase^BackYellow)|BackRed;
					}
					else{
						tempdata->crossway_phase = tempdata->crossway_phase|BackYellow;
					}
					break;
				}
				case 7:	{
					if((tempdata->crossway_phase&SideWalk1Green) != 0x0000){
						tempdata->crossway_phase = tempdata->crossway_phase^SideWalk1Green;
					}
					else if((tempdata->crossway_phase&SideWalk1Red) != 0x0000){
						tempdata->crossway_phase = (tempdata->crossway_phase^SideWalk1Red)|SideWalk1Green;
					}
					else{
						tempdata->crossway_phase = tempdata->crossway_phase|SideWalk1Red;
					}
					break;
				}
				case 8:	{
					if((tempdata->crossway_phase&SideWalk2Green) != 0x0000){
						tempdata->crossway_phase = tempdata->crossway_phase^SideWalk2Green;
					}
					else if((tempdata->crossway_phase&SideWalk2Red) != 0x0000){
						tempdata->crossway_phase = (tempdata->crossway_phase^SideWalk2Red)|SideWalk2Green;
					}
					else{
						tempdata->crossway_phase = tempdata->crossway_phase|SideWalk2Red;
					}
					break;
				}
				case 9: {
					if(tempdata->cutdown>7) tempdata->cutdown--;
					break;
				}
				default: break;
			}
			break;
		}
		case KEY5_Cont:  {
			while(Current_CutDown==0 && Current_PhaseTable==Phase_OneShot.Phase_Number-1);

			RTC_WaitForLastTask();
			RTC_ITConfig(RTC_IT_SEC, DISABLE);

			my_Schedule.PhaseTable[tempdata->period_sel].Crossway_LED[tempdata->crossway][tempdata->phase_sel] = tempdata->crossway_phase;
			my_Schedule.PhaseTable[tempdata->period_sel].Last_Time[tempdata->phase_sel] = tempdata->cutdown;
			
			RTC_WaitForLastTask();
			RTC_ITConfig(RTC_IT_SEC, ENABLE);

			tempdata->state = lcdapp_saved;
			break;
		}
		case KEY6:	tempdata->state = lcdapp_return;  break;
		case KEY2_Cont: {
			if(tempdata->sel == 9){
				if(tempdata->cutdown>7) tempdata->cutdown--;
				break;
			}
		}
		case KEY1_Cont: {
			if(tempdata->sel == 9){
				if(tempdata->cutdown<99) tempdata->cutdown++;
				break;
			}
		}
		case Nothing: goto set_phase_refresh; 
		default: break;
	}
	goto set_phase_havekey;

set_phase_refresh:
	switch(tempdata->state){
		case lcdapp_saved: break;
		default:{
			tempdata->state = lcdapp_cmd;
			keyfunction_clear_help();
		}
	}
	return;

set_phase_havekey:
	KEY_PressMinus();

	switch(tempdata->state){
		case lcdapp_saved:{
			keyfunction_help("设置成功");
			break;
		}
		default: break;
	}
	set_phase_update(tempdata->crossway, tempdata->sel, tempdata->crossway_phase, tempdata->phase_sel, tempdata->period_sel, tempdata->cutdown);
}
void set_phase_main(KEY_TypeDef key){
	phase_fundata* tempdata;

	tempdata = (phase_fundata*) PhaseMenu.fundata;

	switch(tempdata->state){
		case lcdapp_ready: set_phase_interface(); set_phase_update(tempdata->crossway, tempdata->sel, tempdata->crossway_phase, tempdata->phase_sel, tempdata->period_sel, tempdata->cutdown); tempdata->state = lcdapp_cmd; break;
		case lcdapp_saved:
		case lcdapp_cmd:{
			set_phase(key);
			break;
		}
		case lcdapp_return:{
			PhaseMenu.cur_funptr = PhaseMenu.key_function;
			free(tempdata);
			PhaseMenu.fundata = NULL;   //可用可不用
	
			LCD_ClearMenuText();
			PhaseMenu.item_display();
			PhaseMenu.help_display();
			break;
		}
	}
}
/**********************************************************************
删除相位
**********************************************************************/
void del_phase_datainit(void){
	phase_fundata* tempdata;
	
	PhaseMenu.fundata = calloc(1, sizeof(phase_fundata));
	tempdata = (phase_fundata*) PhaseMenu.fundata;

	if(tempdata != NULL){
		tempdata->sel = 0;
		tempdata->period_sel = 0;
		tempdata->phase_sel = 0;
		tempdata->crossway = 0;
		tempdata->cutdown = my_Schedule.PhaseTable[tempdata->period_sel].Last_Time[tempdata->phase_sel];
		tempdata->crossway_phase = my_Schedule.PhaseTable[tempdata->period_sel].Crossway_LED[tempdata->crossway][tempdata->phase_sel];
		tempdata->state = lcdapp_ready;
	}
	else{
		return;
		//tempdata->state = lcdapp_return;
	}
	
	del_phase_main(Nothing);
}
void del_phase_update(uint8_t crossway, uint8_t sel, uint16_t crossway_phase, uint8_t phase_sel, uint8_t period_sel, uint8_t cutdown){
	uint8_t i;
	uint8_t lcd_flag_temp = LCD_FLAG_Using;

LCD_FLAG_Using = 1;
	for(i=0; i<4; i++){
		if(i == sel){
			LCD_Inv();
		}
		switch (i){
			case 0: {
				//LCD_WriteText2(3, 80, "灯组");
				LCD_WriteText2(7, 80, (uint8_t*)Weekday_Table[crossway+1]);
				break;
			}
			case 1: {
				LCD_WriteChar2(8, 98, Number_Table[(period_sel+1)/10]);
				LCD_WriteChar1(Number_Table[(period_sel+1)%10]);
				break;
			}
			case 2: {
				LCD_WriteChar2(19, 98, Number_Table[(phase_sel+1)/10]);
				LCD_WriteChar1(Number_Table[(phase_sel+1)%10]);
				break;
			}
			case 3: {
				LCD_WriteText2(9, 206, "删除");
				break;
			}
			default: break;
		}
		if(i == sel){
			LCD_NoInv();
		}
	}
	for(i=0; i<7; i++){
		LCD_SetCursor(Func_PhaseInfo_Place[i][0], Func_PhaseInfo_Place[i][1]);
		switch (i){
			case 0: {
				if((crossway_phase&LeftGreen) != 0x0000){
					LCD_WriteText1((uint8_t*)LedColor_Table[0]);
				}
				else if((crossway_phase&LeftRed) != 0x0000){
					LCD_WriteText1((uint8_t*)LedColor_Table[1]);
				}
				else if((crossway_phase&LeftYellow) != 0x0000){
					LCD_WriteText1((uint8_t*)LedColor_Table[2]);
				}
				else{
					LCD_WriteText1((uint8_t*)Tick_Cross_Table[0]);
				}
				break;
			}
			case 1: {
				if((crossway_phase&StraightGreen) != 0x0000){
					LCD_WriteText1((uint8_t*)LedColor_Table[0]);
				}
				else if((crossway_phase&StraightRed) != 0x0000){
					LCD_WriteText1((uint8_t*)LedColor_Table[1]);
				}
				else if((crossway_phase&StraightYellow) != 0x0000){
					LCD_WriteText1((uint8_t*)LedColor_Table[2]);
				}
				else{
					LCD_WriteText1((uint8_t*)Tick_Cross_Table[0]);
				}
				break;
			}
			case 2: {
				if((crossway_phase&RightGreen) != 0x0000){
					LCD_WriteText1((uint8_t*)LedColor_Table[0]);
				}
				else if((crossway_phase&RightRed) != 0x0000){
					LCD_WriteText1((uint8_t*)LedColor_Table[1]);
				}
				else if((crossway_phase&RightYellow) != 0x0000){
					LCD_WriteText1((uint8_t*)LedColor_Table[2]);
				}
				else{
					LCD_WriteText1((uint8_t*)Tick_Cross_Table[0]);
				}
				break;
			}
			case 3: {
				if((crossway_phase&BackGreen) != 0x0000){
					LCD_WriteText1((uint8_t*)LedColor_Table[0]);
				}
				else if((crossway_phase&BackRed) != 0x0000){
					LCD_WriteText1((uint8_t*)LedColor_Table[1]);
				}
				else if((crossway_phase&BackYellow) != 0x0000){
					LCD_WriteText1((uint8_t*)LedColor_Table[2]);
				}
				else{
					LCD_WriteText1((uint8_t*)Tick_Cross_Table[0]);
				}
				break;
			}
			case 4: {
				if((crossway_phase&SideWalk1Green) != 0x0000){
					LCD_WriteText1((uint8_t*)LedColor_Table[0]);
				}
				else if((crossway_phase&SideWalk1Red) != 0x0000){
					LCD_WriteText1((uint8_t*)LedColor_Table[1]);
				}
				else{
					LCD_WriteText1((uint8_t*)Tick_Cross_Table[0]);
				}
				break;
			}
			case 5: {
				if((crossway_phase&SideWalk2Green) != 0x0000){
					LCD_WriteText1((uint8_t*)LedColor_Table[0]);
				}
				else if((crossway_phase&SideWalk2Red) != 0x0000){
					LCD_WriteText1((uint8_t*)LedColor_Table[1]);
				}
				else{
					LCD_WriteText1((uint8_t*)Tick_Cross_Table[0]);
				}
				break;
			}
			case 6: {
				LCD_WriteChar1(Number_Table[cutdown/10]);
				LCD_WriteChar1(Number_Table[cutdown%10]);
				break;
			}
			default: break;
		}
	}
LCD_FLAG_Using = lcd_flag_temp;
if(LCD_FLAG_UpdateIrq == 1 && LCD_FLAG_Using == 0){
	EXTI_GenerateSWInterrupt(EXTI_Line1);
}
}
void del_phase_interface(void){
	phase_fundata* tempdata;
    uint8_t lcd_flag_temp = LCD_FLAG_Using;

	tempdata = (phase_fundata*) PhaseMenu.fundata;

LCD_FLAG_Using = 1;
	LCD_ClearItemAllText();
	
	LCD_SetAppwindow(0, 239, 0, 39);
	
	LCD_WriteText2(2, 80, "\x11灯组  \x10 (×:表示该灯不亮)" );
	LCD_WriteText2(2, 98, "时段【  】 相位【  】");
	LCD_WriteText2(2, 126, "左转 [  ] 直行 [  ]");
	LCD_WriteText2(2, 146, "右转 [  ] 调头 [  ]");
	LCD_WriteText2(2, 166, "人行1[  ] 人行2[  ]");
	LCD_WriteText2(2, 186, "相位运行时间:  秒");
	LCD_WriteText2(7, 206, "【    】");
	
	LCD_SetAppwindow(96, 170, 29, 38);
	LCD_WriteText2(29, 76, "删除操作将删除所有灯组的选定时段的相位");
	
	LCD_SetAppwindow(0, 239, 0, 39); 

	LCD_WriteText2(29, 176, "\x1A\x1B:移动");
	LCD_WriteText2(29, 196, "\x18\x19:选择");
	
	del_phase_update(tempdata->crossway, tempdata->sel, tempdata->crossway_phase, tempdata->phase_sel, tempdata->period_sel, tempdata->cutdown);

LCD_FLAG_Using = lcd_flag_temp;
if(LCD_FLAG_UpdateIrq == 1 && LCD_FLAG_Using == 0){
	EXTI_GenerateSWInterrupt(EXTI_Line1);
}
}
void del_phase(KEY_TypeDef key){
	phase_fundata* tempdata;
	uint8_t i,j;

	tempdata = (phase_fundata*) PhaseMenu.fundata;
	
	switch(tempdata->state){
		case lcdapp_cmd: 
		case lcdapp_saved: 
		case lcdapp_notsaved:{
			switch (key){
				case KEY3:  if(tempdata->sel==0) tempdata->sel=3; else tempdata->sel--; break;
				case KEY4:  if(tempdata->sel==3) tempdata->sel=0; else tempdata->sel++; break;
				case KEY1:	{
					switch(tempdata->sel){
						case 0: if(tempdata->crossway < 5) tempdata->crossway ++; else tempdata->crossway = 0; break;
						case 1: if(tempdata->period_sel < my_Schedule.TimeTable_Number-1) tempdata->period_sel ++; else tempdata->period_sel = 0; tempdata->phase_sel=0; break;
						case 2: if(tempdata->phase_sel < my_Schedule.PhaseTable[tempdata->period_sel].Phase_Number-1) tempdata->phase_sel ++; else tempdata->phase_sel = 0; break;
						default: break;
					}
					tempdata->crossway_phase = my_Schedule.PhaseTable[tempdata->period_sel].Crossway_LED[tempdata->crossway][tempdata->phase_sel];
					tempdata->cutdown = my_Schedule.PhaseTable[tempdata->period_sel].Last_Time[tempdata->phase_sel];
					break;
				}
				case KEY2:	{
					switch(tempdata->sel){
						case 0: if(tempdata->crossway > 0) tempdata->crossway --; else tempdata->crossway = 5; break;
						case 1: if(tempdata->period_sel > 0) tempdata->period_sel --; else tempdata->period_sel= my_Schedule.TimeTable_Number-1; tempdata->phase_sel=0;break;
						case 2: if(tempdata->phase_sel > 0) tempdata->phase_sel --; else tempdata->phase_sel = my_Schedule.PhaseTable[tempdata->period_sel].Phase_Number-1; break;
						default: break;
					}
					tempdata->crossway_phase = my_Schedule.PhaseTable[tempdata->period_sel].Crossway_LED[tempdata->crossway][tempdata->phase_sel];
					tempdata->cutdown = my_Schedule.PhaseTable[tempdata->period_sel].Last_Time[tempdata->phase_sel];
					break;
				}
				case KEY5:  {
					if(tempdata->sel == 3){
						if(tempdata->state == lcdapp_cmd){
							tempdata->state = lcdapp_need_confirm;
							tempdata->sel = 4;
							
							LCD_WriteText2(2, 223, "【  】 【  】");
							keyfunction_clear_help();
							keyfunction_help("再按一次确定键予以保存");
							/*
							LCD_Inv();
							LCD_WriteText2(4, 223, "是");
							LCD_NoInv();
							*/
						}
						/*
						else if(tempdata->state == lcdapp_need_confirm){
							tempdata->state = lcdapp_need_saved;
		
							if(my_Schedule.PhaseTable[periodoftime].Phase_Number > 1){
								while(Current_CutDown==0 && Current_PhaseTable==Phase_OneShot.Phase_Number-1);
		
								RTC_WaitForLastTask();
								RTC_ITConfig(RTC_IT_SEC, DISABLE);
				
								for(j=0; j<6; j++){	 //j:crossway
									for(i=tempdata->phase_sel; i<my_Schedule.PhaseTable[tempdata->period_sel].Phase_Number-1; i++){
										my_Schedule.PhaseTable[tempdata->period_sel].Crossway_LED[j][i] = my_Schedule.PhaseTable[tempdata->period_sel].Crossway_LED[j][i+1];
									} 
								}
								for(i=tempdata->phase_sel; i<my_Schedule.PhaseTable[tempdata->period_sel].Phase_Number-1; i++){
									my_Schedule.PhaseTable[tempdata->period_sel].Last_Time[i] = my_Schedule.PhaseTable[tempdata->period_sel].Last_Time[i+1];;
								}
								for(j=0; j<6; j++){
									my_Schedule.PhaseTable[tempdata->period_sel].Crossway_LED[j][my_Schedule.PhaseTable[tempdata->period_sel].Phase_Number-1] = 0xFC00;
								}
								my_Schedule.PhaseTable[tempdata->period_sel].Last_Time[my_Schedule.PhaseTable[tempdata->period_sel].Phase_Number-1] = 0;
								my_Schedule.PhaseTable[tempdata->period_sel].Phase_Number -= 1;
								
								RTC_WaitForLastTask();
								RTC_ITConfig(RTC_IT_SEC, ENABLE);
			
								tempdata->phase_sel = my_Schedule.PhaseTable[tempdata->period_sel].Phase_Number-1;
							}
						}*/
					}	
					break;
				}
				case KEY6:	tempdata->state = lcdapp_return;  break;
				case Nothing: goto del_phase_refresh;
				default: break;
			}
			break;
		}
		case lcdapp_need_confirm:{
			switch(key){
				case KEY3: 
				case KEY4: if(tempdata->sel == 4) tempdata->sel = 5; else tempdata->sel = 4; break;
				case KEY5: {
					if(tempdata->sel == 4){
						tempdata->state = lcdapp_saved;
		
						if(my_Schedule.PhaseTable[tempdata->period_sel].Phase_Number > 1){
							while(Current_CutDown==0 && Current_PhaseTable==Phase_OneShot.Phase_Number-1);
	
							RTC_WaitForLastTask();
							RTC_ITConfig(RTC_IT_SEC, DISABLE);
			
							for(j=0; j<6; j++){	 //j:crossway
								for(i=tempdata->phase_sel; i<my_Schedule.PhaseTable[tempdata->period_sel].Phase_Number-1; i++){
									my_Schedule.PhaseTable[tempdata->period_sel].Crossway_LED[j][i] = my_Schedule.PhaseTable[tempdata->period_sel].Crossway_LED[j][i+1];
								} 
							}
							for(i=tempdata->phase_sel; i<my_Schedule.PhaseTable[tempdata->period_sel].Phase_Number-1; i++){
								my_Schedule.PhaseTable[tempdata->period_sel].Last_Time[i] = my_Schedule.PhaseTable[tempdata->period_sel].Last_Time[i+1];;
							}
							for(j=0; j<6; j++){
								my_Schedule.PhaseTable[tempdata->period_sel].Crossway_LED[j][my_Schedule.PhaseTable[tempdata->period_sel].Phase_Number-1] = 0xFC00;
							}
							my_Schedule.PhaseTable[tempdata->period_sel].Last_Time[my_Schedule.PhaseTable[tempdata->period_sel].Phase_Number-1] = 0;
							my_Schedule.PhaseTable[tempdata->period_sel].Phase_Number -= 1;
							
							RTC_WaitForLastTask();
							RTC_ITConfig(RTC_IT_SEC, ENABLE);
		
							tempdata->phase_sel = my_Schedule.PhaseTable[tempdata->period_sel].Phase_Number-1;
						}
					}
					else tempdata->state = lcdapp_notsaved;
					break;
				}
				case KEY6: {
					tempdata->state = lcdapp_notsaved;
					break;
				}
				case Nothing: goto del_phase_refresh;
				default: break;
			}
		}
	}
	goto del_phase_havekey;

del_phase_refresh:
	switch(tempdata->state){
		case lcdapp_need_confirm:
		case lcdapp_cmd: break;
		case lcdapp_saved:
		case lcdapp_notsaved:{
			tempdata->state = lcdapp_cmd;
			keyfunction_clear_help();
			break;
		}
	}
	return;

del_phase_havekey:
	KEY_PressMinus();

	switch(tempdata->state){
		case lcdapp_need_confirm:{
			for(i=4; i<6; i++){
				if(i == tempdata->sel) LCD_Inv();
				switch(i){
					case 4: LCD_WriteText2(4, 223, "是");  break;
					case 5: LCD_WriteText2(11, 223, "否"); break;
					default:break;
				}
				if(i == tempdata->sel) LCD_NoInv();
			}
			break;
		}
		case lcdapp_saved:{
			keyfunction_clear_help();
			keyfunction_help("操作成功");
			LCD_WriteText2(2, 223, "             ");
			break;
		}
		case lcdapp_notsaved:{
			keyfunction_clear_help();
			keyfunction_help("取消操作");
			LCD_WriteText2(2, 223, "             ");
			break;
		}
		//case lcdapp_cmd:{
			//del_phase_update(tempdata->crossway, tempdata->sel, tempdata->crossway_phase, tempdata->phase_sel, tempdata->period_sel, tempdata->cutdown);
			//break;
		//}
		default: break;
	}
	del_phase_update(tempdata->crossway, tempdata->sel, tempdata->crossway_phase, tempdata->phase_sel, tempdata->period_sel, tempdata->cutdown);
}
void del_phase_main(KEY_TypeDef key){
	phase_fundata* tempdata;

	tempdata = (phase_fundata*) PhaseMenu.fundata;

	if(tempdata != NULL){
		switch(tempdata->state){
			case lcdapp_ready: del_phase_interface(); /*set_phase_update(tempdata->crossway, tempdata->sel, tempdata->crossway_phase, tempdata->phase_sel, tempdata->period_sel, tempdata->cutdown);*/ tempdata->state = lcdapp_cmd; break;
			case lcdapp_saved:
			case lcdapp_notsaved:
			case lcdapp_need_confirm:
			case lcdapp_cmd:{
				del_phase(key);
				break;
			}
			case lcdapp_return:{
				PhaseMenu.cur_funptr = PhaseMenu.key_function;
				free(tempdata);
				PhaseMenu.fundata = NULL;   //可用可不用
		
				LCD_ClearMenuText();
				PhaseMenu.item_display();
				PhaseMenu.help_display();
				break;
			}
		}
	}
	else{
		LCD_ClearMenuText();
		PhaseMenu.item_display();
		PhaseMenu.help_display();
	}
}

/**********************************************************************
添加相位
**********************************************************************/
typedef struct{
	uint8_t sel;
	uint8_t period_sel, phase_sel;
	uint8_t option;
	lcd_appstate state;
}add_phase_fundata;
void add_phase_datainit(void){
	add_phase_fundata* tempdata;
	
	PhaseMenu.fundata = calloc(1, sizeof(add_phase_fundata));
	tempdata = (add_phase_fundata*) PhaseMenu.fundata;

	if(tempdata != NULL){
		tempdata->sel = 0;
		tempdata->period_sel = 0;
		tempdata->phase_sel = 0;
		tempdata->option = 0;
		tempdata->state = lcdapp_ready;
	}
	else{
		return;
	}
	
	add_phase_main(Nothing);
}
void add_phase_update(uint8_t sel, uint8_t period_sel, uint8_t phase_sel, uint8_t options){
	uint8_t i;
	uint8_t lcd_flag_temp = LCD_FLAG_Using;

LCD_FLAG_Using = 1;
	for(i=0; i<4; i++){
		if(i == sel)
		{
			LCD_Inv();
		}
		LCD_SetCursor(Func_AddPhase_Place[i][0], Func_AddPhase_Place[i][1]);
		switch (i){
			case 0:	LCD_WriteChar1(Number_Table[(period_sel+1)/10]); LCD_WriteChar1(Number_Table[(period_sel+1)%10]); break;
			case 1:	LCD_WriteChar1(Number_Table[(phase_sel+1)/10]); LCD_WriteChar1(Number_Table[(phase_sel+1)%10]); break;
			case 2:	if(options == 0) LCD_WriteText1("向上插入"); else LCD_WriteText1("向下插入");	break;
			case 3:	LCD_WriteText1("生成相位");	break;
			default: break;
		}
		if(i == sel){
			LCD_NoInv();
		}
	}
LCD_FLAG_Using = lcd_flag_temp;
if(LCD_FLAG_UpdateIrq == 1 && LCD_FLAG_Using == 0){
	EXTI_GenerateSWInterrupt(EXTI_Line1);
}
}
void add_phase_interface(void){
	add_phase_fundata* tempdata;
	uint8_t lcd_flag_temp = LCD_FLAG_Using;

	tempdata = (add_phase_fundata*) PhaseMenu.fundata;
LCD_FLAG_Using = 1;
	LCD_ClearItemAllText();
			
	LCD_SetAppwindow(75, 239, 2, 28);
	
	LCD_WriteText2(2, 80, "请设置要添加的相位位置,添加的相位初始时间为0,初始相位全为红灯");
	LCD_WriteText2(5, 150, "时段【  】 相位【  】");
	LCD_WriteText2(2, 180, "【        】【        】");
	
	LCD_SetAppwindow(96, 170, 29, 38);
	LCD_WriteText2(29, 96, "在选定时段和相位,按照设定添加一个初始相位");
	
	LCD_SetAppwindow(0, 239, 0, 39); 

	LCD_WriteText2(29, 176, "\x1A\x1B:移动");
	LCD_WriteText2(29, 196, "\x18\x19:选择");
	
LCD_FLAG_Using = lcd_flag_temp;
if(LCD_FLAG_UpdateIrq == 1 && LCD_FLAG_Using == 0){
	EXTI_GenerateSWInterrupt(EXTI_Line1);
}
	add_phase_update(tempdata->sel, tempdata->period_sel, tempdata->phase_sel, tempdata->option);
}
void add_phase(KEY_TypeDef key){
	add_phase_fundata* tempdata;
	uint8_t i, j;

	tempdata = (add_phase_fundata*) PhaseMenu.fundata;

	switch (key){
		case KEY3:  if(tempdata->sel==0) tempdata->sel=3; else tempdata->sel--; break;
		case KEY4:  if(tempdata->sel==3) tempdata->sel=0; else tempdata->sel++; break;
		case KEY1:	{
			switch(tempdata->sel){
				case 0: if(tempdata->period_sel < my_Schedule.TimeTable_Number-1) tempdata->period_sel++; break;
				case 1: if(tempdata->phase_sel < my_Schedule.PhaseTable[tempdata->period_sel].Phase_Number-1) tempdata->phase_sel ++;break;
				case 2: if(tempdata->option == 0) tempdata->option = 1; else tempdata->option = 0; break;
				//case 2: break;
				default: break;
			}
			break;
		}
		case KEY2:	{
			switch(tempdata->sel){
				case 0: if(tempdata->period_sel > 0) tempdata->period_sel --; break;
				case 1: if(tempdata->phase_sel > 0) tempdata->phase_sel --; break;
				case 2: if(tempdata->option == 0) tempdata->option = 1; else tempdata->option = 0; break;
				//case 2: break;
				default: break;
			}
			break;
		}
		case KEY5:  {
			if(tempdata->sel == 3){
				if(my_Schedule.PhaseTable[tempdata->period_sel].Phase_Number < 16){	 //判断相位数是否满了
					while(Current_CutDown==0 && Current_PhaseTable==Phase_OneShot.Phase_Number-1);
					
					RTC_WaitForLastTask();
					RTC_ITConfig(RTC_IT_SEC, DISABLE);

					//while(Current_CutDown==0 && Current_PhaseTable==Phase_OneShot.Phase_Number-1);
					
					if(tempdata->option == 1){   //向下插入相位模式
						if(tempdata->phase_sel != my_Schedule.PhaseTable[tempdata->period_sel].Phase_Number-1){
							for(i=my_Schedule.PhaseTable[tempdata->period_sel].Phase_Number; i>tempdata->phase_sel+1; i--){
								for(j=0; j<6; j++){
									my_Schedule.PhaseTable[tempdata->period_sel].Crossway_LED[j][i] = my_Schedule.PhaseTable[tempdata->period_sel].Crossway_LED[j][i-1];
								}
								my_Schedule.PhaseTable[tempdata->period_sel].Last_Time[i] = my_Schedule.PhaseTable[tempdata->period_sel].Last_Time[i-1];
							}
						}
						for(j=0; j<6; j++){
							my_Schedule.PhaseTable[tempdata->period_sel].Crossway_LED[j][tempdata->phase_sel+1] = 0xFC00;
							//my_Schedule.PhaseTable[periodoftime].Crossway_LED[j][i] = 0xFC00;
						}
						my_Schedule.PhaseTable[tempdata->period_sel].Last_Time[tempdata->phase_sel+1] = 7;
						//my_Schedule.PhaseTable[periodoftime].Last_Time[i] = 0;
					}
					else{  //options == 0  向上插入相位模式
						for(i=i=my_Schedule.PhaseTable[tempdata->period_sel].Phase_Number; i>tempdata->phase_sel; i--){
							for(j=0; j<6; j++){
								my_Schedule.PhaseTable[tempdata->period_sel].Crossway_LED[j][i] = my_Schedule.PhaseTable[tempdata->period_sel].Crossway_LED[j][i-1];
							}
							my_Schedule.PhaseTable[tempdata->period_sel].Last_Time[i] = my_Schedule.PhaseTable[tempdata->period_sel].Last_Time[i-1];
						}	
						for(j=0; j<6; j++){
							my_Schedule.PhaseTable[tempdata->period_sel].Crossway_LED[j][tempdata->phase_sel] = 0xFC00;
							//my_Schedule.PhaseTable[periodoftime].Crossway_LED[j][i] = 0xFC00;
						}
						my_Schedule.PhaseTable[tempdata->period_sel].Last_Time[tempdata->phase_sel] = 7;
						//my_Schedule.PhaseTable[periodoftime].Last_Time[i] = 0;
					}
					my_Schedule.PhaseTable[tempdata->period_sel].Phase_Number ++;
					
					RTC_WaitForLastTask();
					RTC_ITConfig(RTC_IT_SEC, ENABLE);

					tempdata->state = lcdapp_saved;
				}
				else tempdata->state = lcdapp_notsaved;
			}
			break;
		}
		case KEY6:	tempdata->state = lcdapp_return;  break;
		case Nothing: goto add_phase_refresh; //break;
		default: break;
	}
	goto add_phase_havekey;

add_phase_refresh:
	switch(tempdata->state){
		case lcdapp_saved:
		case lcdapp_notsaved:{
			tempdata->state = lcdapp_cmd;
			//keyfunction_clear_help();
			LCD_SetAppwindow(96, 170, 29, 38);
			LCD_WriteText2(29, 96, "在选定时段和相位,按照设定添加一个初始相位");	
			LCD_SetAppwindow(0, 239, 0, 39);
			break;
		}
		default: break;
	}
	return;

add_phase_havekey:
	KEY_PressMinus();
	switch(tempdata->state){
		case lcdapp_saved:{
			keyfunction_help("操作成功");
			break;
		}
		case lcdapp_notsaved:{
			keyfunction_help("取消操作");
			break;
		}
		case lcdapp_cmd:{
			add_phase_update(tempdata->sel, tempdata->period_sel, tempdata->phase_sel, tempdata->option);
			break;
		}
		default: break;
	}	
}
void add_phase_main(KEY_TypeDef key){
	add_phase_fundata* tempdata;

	tempdata = (add_phase_fundata*) PhaseMenu.fundata;

	if(tempdata != NULL){
		switch(tempdata->state){
			case lcdapp_ready: add_phase_interface(); tempdata->state = lcdapp_cmd; break;
			case lcdapp_saved:
			case lcdapp_notsaved:
			case lcdapp_cmd:{
				add_phase(key);
				break;
			}
			case lcdapp_return:{
				PhaseMenu.cur_funptr = PhaseMenu.key_function;
				free(tempdata);
				PhaseMenu.fundata = NULL;   //可用可不用
		
				LCD_ClearMenuText();
				PhaseMenu.item_display();
				PhaseMenu.help_display();
				break;
			}
		}
	}
	else{
		PhaseMenu.cur_funptr = PhaseMenu.key_function;
		LCD_ClearMenuText();
		PhaseMenu.item_display();
		PhaseMenu.help_display();
	}
}
/**********************************************************************
时段表设置
**********************************************************************/
typedef struct{
	uint8_t sel;
	uint8_t period_sel;
	ShortTime_TypeDef temptime;
	lcd_appstate state;
}set_period_fundata;
void set_period_datainit(void){
	set_period_fundata* tempdata;

	PeriodOfTimeMenu.fundata = calloc(1, sizeof(set_period_fundata));
	tempdata = (set_period_fundata*) PeriodOfTimeMenu.fundata;

	if(tempdata != NULL){
		tempdata->sel = 0;
		tempdata->period_sel = 0;
		tempdata->temptime = my_Schedule.TimeTable[tempdata->period_sel];
		tempdata->state = lcdapp_ready;
	}
	else{
		return;//考虑将数据初始化返回是否分配成功
	}
}
void set_period_update(uint8_t sel, uint8_t period_sel, struct ShortTime time){
	uint8_t i;
	uint8_t lcd_flag_temp = LCD_FLAG_Using;

LCD_FLAG_Using = 1;
	for(i=0; i<4; i++){
		if(i == sel){
			LCD_Inv();
		}
		LCD_SetCursor(Func_SetPeriodOfTime_Place[i][0], Func_SetPeriodOfTime_Place[i][1]);
		switch (i){
			case 0:	LCD_WriteChar1(Number_Table[(period_sel+1)/10]); LCD_WriteChar1(Number_Table[(period_sel+1)%10]); break;
			case 1:	LCD_WriteChar1(Number_Table[time.hour/10]); LCD_WriteChar1(Number_Table[time.hour%10]); break;
			case 2: LCD_WriteChar1(Number_Table[time.min/10]); LCD_WriteChar1(Number_Table[time.min%10]); break;
			case 3:	LCD_WriteChar1(Number_Table[time.sec/10]); LCD_WriteChar1(Number_Table[time.sec%10]); break;
			default: break;
		}
		if(i == sel){
			LCD_NoInv();
		}
	}
LCD_FLAG_Using = lcd_flag_temp;
if(LCD_FLAG_UpdateIrq == 1 && LCD_FLAG_Using == 0){
	EXTI_GenerateSWInterrupt(EXTI_Line1);
}
}
void set_period_interface(void){
	uint8_t lcd_flag_temp;
	set_period_fundata* tempdata;

	tempdata = (set_period_fundata*) PeriodOfTimeMenu.fundata;
	lcd_flag_temp = LCD_FLAG_Using;

LCD_FLAG_Using = 1;
		
	LCD_ClearItemAllText();

	LCD_SetAppwindow(96, 170, 29, 38);
	LCD_WriteText2(29, 96, "设置时段时间");
	
	LCD_SetAppwindow(0, 239, 0, 39); 
	
	LCD_WriteText2(2, 80, "请设置所选时段的时间");
	LCD_WriteText2(8, 100, "选择时段【  】");
	LCD_WriteText2(2, 120, "时段时间:  -  -  ");

	LCD_WriteText2(29, 176, "\x1A\x1B:移动");
	LCD_WriteText2(29, 196, "\x18\x19:选择");
		
LCD_FLAG_Using = lcd_flag_temp;
if(LCD_FLAG_UpdateIrq == 1 && LCD_FLAG_Using == 0){
	EXTI_GenerateSWInterrupt(EXTI_Line1);
}
	set_period_update(tempdata->sel, tempdata->period_sel, tempdata->temptime);
}
void set_period(KEY_TypeDef key){
	set_period_fundata* tempdata;

	tempdata = (set_period_fundata*) PeriodOfTimeMenu.fundata;

	switch (key){
		case KEY3:  if(tempdata->sel==0) tempdata->sel=3; else tempdata->sel--; break;
		case KEY4:  if(tempdata->sel==3) tempdata->sel=0; else tempdata->sel++; break;
		case KEY1:	{
			switch (tempdata->sel){
				case 0: {
					if(tempdata->period_sel<my_Schedule.TimeTable_Number-1){ 
						tempdata->period_sel++;
						tempdata->temptime = my_Schedule.TimeTable[tempdata->period_sel]; 
					}														    
					break;
				}
				case 1: {
					if(tempdata->period_sel<my_Schedule.TimeTable_Number-1){ 
						if(tempdata->temptime.hour<my_Schedule.TimeTable[tempdata->period_sel+1].hour-1){ 
							tempdata->temptime.hour ++;
						}
						else if(tempdata->temptime.min<=my_Schedule.TimeTable[tempdata->period_sel+1].min && tempdata->temptime.sec<=my_Schedule.TimeTable[tempdata->period_sel+1].sec){
							tempdata->temptime.hour ++;
						}	
					}
					else{
						if(tempdata->temptime.hour<23)
							tempdata->temptime.hour ++;
					}
					break;
				}
				case 2: {
					if(tempdata->period_sel<my_Schedule.TimeTable_Number-1 && tempdata->temptime.hour==my_Schedule.TimeTable[tempdata->period_sel+1].hour){
						if(tempdata->temptime.min<my_Schedule.TimeTable[tempdata->period_sel+1].min){
							tempdata->temptime.min ++;
						}
					}
					else{
						if(tempdata->temptime.min<59)
							tempdata->temptime.min ++;
					}
					break;
				}
				case 3: {
					if(tempdata->period_sel<my_Schedule.TimeTable_Number-1 && tempdata->temptime.hour==my_Schedule.TimeTable[tempdata->period_sel+1].hour && tempdata->temptime.min==my_Schedule.TimeTable[tempdata->period_sel+1].min){
						if(tempdata->temptime.sec<my_Schedule.TimeTable[tempdata->period_sel+1].sec-1){
							tempdata->temptime.sec ++;
						}
					}
					else{
						if(tempdata->temptime.sec<59)
							tempdata->temptime.sec ++;
					}
					break;
				}
				default: break;
			}
			break;
		}
		case KEY2:	{
			switch (tempdata->sel){
				case 0: {
					if(tempdata->period_sel>0){ 
						tempdata->period_sel --;
						tempdata->temptime = my_Schedule.TimeTable[tempdata->period_sel];
					} 
					break;
				}
				case 1: {
					if(tempdata->period_sel>0){
						if(tempdata->temptime.hour>my_Schedule.TimeTable[tempdata->period_sel-1].hour+1){
							tempdata->temptime.hour --;
						}
						else if(tempdata->temptime.min>=my_Schedule.TimeTable[tempdata->period_sel-1].min && tempdata->temptime.sec>my_Schedule.TimeTable[tempdata->period_sel-1].sec+1){
							tempdata->temptime.hour --;
						}
					}
					else{
						if(tempdata->temptime.hour>0) 
							tempdata->temptime.hour --;
					}
					break;
				}
				case 2: {
					if(tempdata->period_sel>0 && tempdata->temptime.hour==my_Schedule.TimeTable[tempdata->period_sel-1].hour){
						if(tempdata->temptime.min>my_Schedule.TimeTable[tempdata->period_sel-1].min)
							tempdata->temptime.min --;
					}
					else{
						if(tempdata->temptime.min>0)
							tempdata->temptime.min --;
					} 
					break;
				}
				case 3: {
					if(tempdata->period_sel>0 && tempdata->temptime.hour==my_Schedule.TimeTable[tempdata->period_sel-1].hour && tempdata->temptime.min==my_Schedule.TimeTable[tempdata->period_sel-1].min){
						if(tempdata->temptime.sec>my_Schedule.TimeTable[tempdata->period_sel-1].sec+1)
							tempdata->temptime.sec --;
					}
					else{
						if(tempdata->temptime.sec>0)
							tempdata->temptime.sec --;
					} 
					break;
				}
				default: break;
			}
			break;
		}
		case KEY5_Cont:  {							
			while(Current_CutDown==0 && Current_PhaseTable==Phase_OneShot.Phase_Number-1);
			
			RTC_WaitForLastTask();
			RTC_ITConfig(RTC_IT_SEC, DISABLE);
			
			//while(Current_CutDown==0 && Current_PhaseTable==Phase_OneShot.Phase_Number-1);

			my_Schedule.TimeTable[tempdata->period_sel] = tempdata->temptime;

			RTC_WaitForLastTask();
			RTC_ITConfig(RTC_IT_SEC, ENABLE);

			tempdata->state = lcdapp_saved;

			break;
		}
		case KEY6:	tempdata->state = lcdapp_return;  break;
		case Nothing: goto set_period_refresh;
		default: break;
	}
	goto set_period_havekey;

set_period_refresh:
	switch(tempdata->state){
		case lcdapp_saved:{
			tempdata->state = lcdapp_cmd;
			keyfunction_clear_help();
			break;
		}
	}
	return;

set_period_havekey:
	KEY_PressMinus();

	switch(tempdata->state){
		case lcdapp_saved:{
			keyfunction_help("操作成功");
			break;
		}
		case lcdapp_cmd:{
			set_period_update(tempdata->sel, tempdata->period_sel, tempdata->temptime);
			break;
		}
		default: break;
	}
}
void set_period_main(KEY_TypeDef key){
	set_period_fundata* tempdata;

	tempdata = (set_period_fundata*) PeriodOfTimeMenu.fundata;
	
	if(tempdata != NULL){
		switch(tempdata->state){
			case lcdapp_ready: set_period_interface(); tempdata->state = lcdapp_cmd; break;
			case lcdapp_saved:
			//case lcdapp_notsaved:
			case lcdapp_cmd:{
				set_period(key);
				break;
			}
			case lcdapp_return:{
				PeriodOfTimeMenu.cur_funptr = PeriodOfTimeMenu.key_function;
				free(tempdata);
				PeriodOfTimeMenu.fundata = NULL;   //可用可不用
		
				LCD_ClearMenuText();
				PeriodOfTimeMenu.item_display();
				PeriodOfTimeMenu.help_display();
				break;
			}
		}
	}
	else{
		PeriodOfTimeMenu.cur_funptr = PeriodOfTimeMenu.key_function;
		LCD_ClearMenuText();
		PeriodOfTimeMenu.item_display();
		PeriodOfTimeMenu.help_display();
	}
}
/**********************************************************************
删除时段表
**********************************************************************/
typedef struct{
	uint8_t sel;
	uint8_t period_sel;
	ShortTime_TypeDef start_time, end_time;
	lcd_appstate state; 
}del_period_fundata;
void del_period_datainit(void){
	del_period_fundata* tempdata;

	PeriodOfTimeMenu.fundata = calloc(1, sizeof(del_period_fundata));
	tempdata = (del_period_fundata*) PeriodOfTimeMenu.fundata;

	if(tempdata != NULL){
		tempdata->sel = 0;
		tempdata->period_sel = 0;
		tempdata->start_time = my_Schedule.TimeTable[tempdata->period_sel];
		if(my_Schedule.TimeTable_Number == 1) tempdata->end_time = my_Schedule.TimeTable[0];
		else tempdata->end_time = my_Schedule.TimeTable[1];
		tempdata->state = lcdapp_ready;
	}
	else{
		return;//考虑将数据初始化返回是否分配成功
	}
}
void del_period_update(struct ShortTime start_time, struct ShortTime end_time, uint8_t sel, uint8_t period_sel){
	uint8_t i;
	uint8_t lcd_flag_temp = LCD_FLAG_Using;

LCD_FLAG_Using = 1;
	for(i=0; i<2; i++){
		if(i == sel) LCD_Inv();
		switch(i){
			case 0:	LCD_SetCursor(18, 100); LCD_WriteChar1(Number_Table[(period_sel+1)/10]); LCD_WriteChar1(Number_Table[(period_sel+1)%10]); break;
			case 1: LCD_WriteText2(16, 160, "删除"); break;
			default: break;
		}
		if(i == sel) LCD_NoInv();
	}

	LCD_SetCursor(13, 120);
	LCD_WriteChar1((start_time.hour<10)?' ':Number_Table[start_time.hour/10]);
	LCD_WriteChar1(Number_Table[start_time.hour%10]);
	LCD_WriteChar1('-');
	LCD_WriteChar1((start_time.min<10)?' ':Number_Table[start_time.min/10]);
	LCD_WriteChar1(Number_Table[start_time.min%10]);
	LCD_WriteChar1('-');
	LCD_WriteChar1((start_time.sec<10)?' ':Number_Table[start_time.sec/10]);
	LCD_WriteChar1(Number_Table[start_time.sec%10]);

	LCD_SetCursor(13, 140);
	LCD_WriteChar1((end_time.hour<10)?' ':Number_Table[end_time.hour/10]);
	LCD_WriteChar1(Number_Table[end_time.hour%10]);
	LCD_WriteChar1('-');
	LCD_WriteChar1((end_time.min<10)?' ':Number_Table[end_time.min/10]);
	LCD_WriteChar1(Number_Table[end_time.min%10]);
	LCD_WriteChar1('-');
	LCD_WriteChar1((end_time.sec<10)?' ':Number_Table[end_time.sec/10]);
	LCD_WriteChar1(Number_Table[end_time.sec%10]);
LCD_FLAG_Using = lcd_flag_temp;
if(LCD_FLAG_UpdateIrq == 1 && LCD_FLAG_Using == 0){
	EXTI_GenerateSWInterrupt(EXTI_Line1);
}
}
void del_period_interface(void){
	del_period_fundata* tempdata;

	uint8_t lcd_flag_temp = LCD_FLAG_Using;
	tempdata = (del_period_fundata*) PeriodOfTimeMenu.fundata;

LCD_FLAG_Using = 1;
	LCD_ClearItemAllText();

	LCD_SetAppwindow(96, 170, 29, 38);
	LCD_WriteText2(29, 96, "删除所选的时段");
	
	LCD_SetAppwindow(0, 239, 0, 39); 
	
	LCD_WriteText2(2, 80, "请选择要删除的时段");
	LCD_WriteText2(8, 100, "选择时段【  】");
	LCD_WriteText2(4, 120, "起始时间:");
	LCD_WriteText2(4, 140, "终止时间:");
	LCD_WriteText2(14, 160, "【    】");

	LCD_WriteText2(29, 176, "\x1A\x1B:移动");
	LCD_WriteText2(29, 196, "\x18\x19:选择");
LCD_FLAG_Using = lcd_flag_temp;
if(LCD_FLAG_UpdateIrq == 1 && LCD_FLAG_Using == 0){
	EXTI_GenerateSWInterrupt(EXTI_Line1);
}	
	del_period_update(tempdata->start_time, tempdata->end_time, tempdata->sel, tempdata->period_sel);
}
void del_period(KEY_TypeDef key){
	uint8_t i;
	del_period_fundata* tempdata;

	tempdata = (del_period_fundata*) PeriodOfTimeMenu.fundata;

	switch (key){
		case KEY3:  //if(tempdata->sel==0) tempdata->sel=1; else tempdata->sel=0; break;
		case KEY4:  if(tempdata->sel==1) tempdata->sel=0; else tempdata->sel=1; break;
		case KEY1:	{
			if(tempdata->sel == 0){
				if(tempdata->period_sel<my_Schedule.TimeTable_Number-1){
					//Display_FLAG_NextDay = 0; 
					tempdata->period_sel++;
					tempdata->start_time = my_Schedule.TimeTable[tempdata->period_sel];
					if(tempdata->period_sel == my_Schedule.TimeTable_Number-1){
						tempdata->end_time = my_Schedule.TimeTable[0];
						//Display_FLAG_NextDay = 1;
					}
					else{
						tempdata->end_time = my_Schedule.TimeTable[tempdata->period_sel+1]; 
					}
				}
			}
			break;
		}
		case KEY2:	{
			if(tempdata->sel == 0){
				if(tempdata->period_sel>0){ 
					tempdata->period_sel --;
					tempdata->start_time = my_Schedule.TimeTable[tempdata->period_sel];
					tempdata->end_time = my_Schedule.TimeTable[tempdata->period_sel+1];
				} 
			}
			break;
		}
		case KEY5_Cont:  {
			if(tempdata->sel == 1){							
				if(my_Schedule.TimeTable_Number >1){
					while(Current_CutDown==0 && Current_PhaseTable==Phase_OneShot.Phase_Number-1);
	
					RTC_WaitForLastTask();
					RTC_ITConfig(RTC_IT_SEC, DISABLE);								
	
					//while(Current_CutDown==0 && Current_PhaseTable==Phase_OneShot.Phase_Number-1);
					
					for(i=tempdata->period_sel; i<my_Schedule.TimeTable_Number-2; i++){
						my_Schedule.TimeTable[i] = my_Schedule.TimeTable[i+1];
						my_Schedule.PhaseTable[i] = my_Schedule.PhaseTable[i+1];
					}
					my_Schedule.TimeTable_Number --;
	
					if(tempdata->period_sel>0)
						tempdata->period_sel --;
	
					tempdata->start_time = my_Schedule.TimeTable[tempdata->period_sel];
					if(tempdata->period_sel == my_Schedule.TimeTable_Number-1){
						tempdata->end_time = my_Schedule.TimeTable[0];
						//Display_FLAG_NextDay = 1;
					}
					else{
						tempdata->end_time = my_Schedule.TimeTable[tempdata->period_sel+1]; 
					}
	
					RTC_WaitForLastTask();
					RTC_ITConfig(RTC_IT_SEC, ENABLE);
	
				}
				tempdata->state = lcdapp_saved;
			}
			break;
		}
		case KEY6:	tempdata->state = lcdapp_return;  break;
		case Nothing: goto del_period_refresh;
		default: break;
	}
	goto del_period_havekey;

del_period_refresh:
	switch(tempdata->state){
		case lcdapp_saved:{
			tempdata->state = lcdapp_cmd;
			keyfunction_clear_help();
			break;
		}
		default: break;
	}
	return;

del_period_havekey:
	KEY_PressMinus();

	switch(tempdata->state){
		case lcdapp_saved:{
			keyfunction_help("操作成功");
			break;
		}
		default: break;
	}
	del_period_update(tempdata->start_time, tempdata->end_time, tempdata->sel, tempdata->period_sel);
}
void del_period_main(KEY_TypeDef key){
	del_period_fundata* tempdata;

	tempdata = (del_period_fundata*) PeriodOfTimeMenu.fundata;

	if(tempdata != NULL){
		switch(tempdata->state){
			case lcdapp_ready: del_period_interface(); tempdata->state = lcdapp_cmd; break;
			case lcdapp_saved:
			//case lcdapp_notsaved:
			case lcdapp_cmd:{
				del_period(key);
				break;
			}
			case lcdapp_return:{
				PeriodOfTimeMenu.cur_funptr = PeriodOfTimeMenu.key_function;
				free(tempdata);
				PeriodOfTimeMenu.fundata = NULL;   //可用可不用
		
				LCD_ClearMenuText();
				PeriodOfTimeMenu.item_display();
				PeriodOfTimeMenu.help_display();
				break;
			}
		}
	}
	else{
		PeriodOfTimeMenu.cur_funptr = PeriodOfTimeMenu.key_function;
		LCD_ClearMenuText();
		PeriodOfTimeMenu.item_display();
		PeriodOfTimeMenu.help_display();
	}
}
/**********************************************************************
添加时段表
**********************************************************************/
typedef struct{
	uint8_t sel;
	uint8_t period_sel;
	ShortTime_TypeDef time;
	lcd_appstate state;
}add_period_fundata;
void add_period_datainit(void){
	add_period_fundata* tempdata;

	PeriodOfTimeMenu.fundata = calloc(1, sizeof(add_period_fundata));
	tempdata = (add_period_fundata*) PeriodOfTimeMenu.fundata;

	if(tempdata != NULL){
		tempdata->sel = 0;
		tempdata->period_sel = 0;
		tempdata->time.hour = 12;
		tempdata->time.min = 0;
		tempdata->time.sec = 0;
		tempdata->state = lcdapp_ready;
	}
	else{
		return;//考虑将数据初始化返回是否分配成功
	}
}
void add_period_update(uint8_t sel, ShortTime_TypeDef time){
	uint8_t i;

	for(i=0; i<4; i++){
		if(i == sel) LCD_Inv();

		switch (i){
			case 0:	LCD_SetCursor(10, 150); LCD_WriteChar1(Number_Table[time.hour/10]); LCD_WriteChar1(Number_Table[time.hour%10]); break;
			case 1: LCD_SetCursor(13, 150); LCD_WriteChar1(Number_Table[time.min/10]); LCD_WriteChar1(Number_Table[time.min%10]); break;
			case 2:	LCD_SetCursor(16, 150); LCD_WriteChar1(Number_Table[time.sec/10]); LCD_WriteChar1(Number_Table[time.sec%10]); break;
			case 3: LCD_WriteText2(16, 180, "确定");
			default: break;
		}

		if(i == sel) LCD_NoInv();
	}
}
void add_period_interface(void){
	add_period_fundata* tempdata;

	uint8_t lcd_flag_temp = LCD_FLAG_Using;
	tempdata = (add_period_fundata*) PeriodOfTimeMenu.fundata;
LCD_FLAG_Using = 1;
	LCD_ClearItemAllText();
			
	LCD_SetAppwindow(75, 239, 2, 28);
	
	LCD_WriteText2(2, 80, "请设置要添加的时段初始时间，选确定键保存设置");
	LCD_WriteText2(2, 120, "时段起始时间：");
	LCD_WriteText2(10, 150, "  -  -  ");
	LCD_WriteText2(14, 180, "【    】");
	
	LCD_SetAppwindow(96, 170, 29, 38);
	LCD_WriteText2(29, 96, "设置时段，按确定键保存");
	
	LCD_SetAppwindow(0, 239, 0, 39); 

	LCD_WriteText2(29, 176, "\x1A\x1B:移动");
	LCD_WriteText2(29, 196, "\x18\x19:选择");
	
LCD_FLAG_Using = lcd_flag_temp;
if(LCD_FLAG_UpdateIrq == 1 && LCD_FLAG_Using == 0){
	EXTI_GenerateSWInterrupt(EXTI_Line1);
}
	add_period_update(tempdata->sel, tempdata->time);
}
void add_period(KEY_TypeDef key){
	uint8_t i;
	add_period_fundata* tempdata;

	tempdata = (add_period_fundata*) PeriodOfTimeMenu.fundata;
	
	switch (key){
		case KEY3:  if(tempdata->sel==0) tempdata->sel=3; else tempdata->sel--; break;
		case KEY4:  if(tempdata->sel==3) tempdata->sel=0; else tempdata->sel++; break;
		case KEY1:	{
			switch(tempdata->sel){
				case 0: if(tempdata->time.hour<23) tempdata->time.hour++; else tempdata->time.hour=0; break;
				case 1: if(tempdata->time.min<59) tempdata->time.min++; else tempdata->time.min=0; break;
				case 2: if(tempdata->time.sec<59) tempdata->time.sec++; else tempdata->time.sec=0; break;
				default: break;
			}
			break;
		}
		case KEY2:	{
			switch(tempdata->sel){
				case 0: if(tempdata->time.hour>0) tempdata->time.hour--; else tempdata->time.hour=23; break;
				case 1: if(tempdata->time.min>0) tempdata->time.min--; else tempdata->time.min=59; break;
				case 2: if(tempdata->time.sec>0) tempdata->time.sec--; else tempdata->time.sec=59; break;
				default: break;
			}
			break;
		}
		case KEY5:  {
			if(tempdata->sel == 3){
				if(my_Schedule.TimeTable_Number < 16){	 //判断时段数是否满了
					//判断设置时间位于的时段号
					for(i=0, tempdata->period_sel=my_Schedule.TimeTable_Number; i<my_Schedule.TimeTable_Number; i++){
						if(tempdata->time.hour < my_Schedule.TimeTable[i].hour){
							tempdata->period_sel = i;
							break;
						}
						else if(tempdata->time.hour > my_Schedule.TimeTable[i].hour) continue;
						else{
							if(tempdata->time.min < my_Schedule.TimeTable[i].min){
								tempdata->period_sel = i;
								break;
							}
							else if(tempdata->time.min > my_Schedule.TimeTable[i].min) continue;
							else{
								if(tempdata->time.sec < my_Schedule.TimeTable[i].sec){
									tempdata->period_sel = i;
									break;
								}
								else if(tempdata->time.sec > my_Schedule.TimeTable[i].sec) continue;
								else{
									//时段时间重复了
									//Time_FLAG_Repeatability = 1;
									tempdata->state = lcdapp_error_repeat;
									goto add_period_havekey;
									//break;
								}
							}
						}
					}				
															
					while(Current_CutDown==0 && Current_PhaseTable==Phase_OneShot.Phase_Number-1);

					RTC_WaitForLastTask();
					RTC_ITConfig(RTC_IT_SEC, DISABLE);

					for(i=my_Schedule.TimeTable_Number; i>tempdata->period_sel; i--){
						my_Schedule.TimeTable[i] = my_Schedule.TimeTable[i-1];
						my_Schedule.PhaseTable[i] = my_Schedule.PhaseTable[i-1];
					}
					my_Schedule.TimeTable[tempdata->period_sel] = tempdata->time;
					my_Schedule.PhaseTable[tempdata->period_sel].Phase_Number = 1;
					my_Schedule.PhaseTable[tempdata->period_sel].Last_Time[0] = 7;
					for(i=0; i<6; i++) my_Schedule.PhaseTable[tempdata->period_sel].Crossway_LED[i][0] = 0xFC00;
					my_Schedule.TimeTable_Number ++;
					
					RTC_WaitForLastTask();
					RTC_ITConfig(RTC_IT_SEC, ENABLE);

					tempdata->state = lcdapp_saved;
				}
				else{										
					tempdata->state = lcdapp_error_full;
				}
			}
			break;
		}
		case KEY6:	tempdata->state = lcdapp_return;  break;
		case Nothing: goto add_period_refresh;
		default: break;
	}				   
	goto add_period_havekey;

add_period_refresh:
	switch(tempdata->state){
		case lcdapp_error_full:
		case lcdapp_error_repeat:
		case lcdapp_saved:{
			tempdata->state = lcdapp_cmd;
			keyfunction_clear_help();
			break;
		}
		default: break;
	}	
	return;

add_period_havekey:
	KEY_PressMinus();

	switch(tempdata->state){
		case lcdapp_error_full:{
			keyfunction_help("时段表已满");
			break;
		}
		case lcdapp_error_repeat:{
			keyfunction_help("时段重叠");
			break;
		}
		case lcdapp_saved:{
			keyfunction_help("操作成功");
			break;
		}
		default: break;
	}
}
void add_period_main(KEY_TypeDef key){
	add_period_fundata* tempdata;

	tempdata = (add_period_fundata*) PeriodOfTimeMenu.fundata;
	
	if(tempdata != NULL){
		switch(tempdata->state){
			case lcdapp_ready: add_period_interface(); tempdata->state = lcdapp_cmd; break;
			case lcdapp_saved:
			//case lcdapp_notsaved:
			case lcdapp_error_repeat:
			case lcdapp_error_full:
			case lcdapp_cmd:{
				add_period(key);
				break;
			}
			case lcdapp_return:{
				PeriodOfTimeMenu.cur_funptr = PeriodOfTimeMenu.key_function;
				free(tempdata);
				PeriodOfTimeMenu.fundata = NULL;   //可用可不用
		
				LCD_ClearMenuText();
				PeriodOfTimeMenu.item_display();
				PeriodOfTimeMenu.help_display();
				break;
			}
		}
	}
	else{
		PeriodOfTimeMenu.cur_funptr = PeriodOfTimeMenu.key_function;
		LCD_ClearMenuText();
		PeriodOfTimeMenu.item_display();
		PeriodOfTimeMenu.help_display();
	}
}




#if MOVE
void Add_PeriodOfTime_Update(uint8_t sel, ShortTime_TypeDef time)
{
	uint8_t i;

	for(i=0; i<4; i++){
		if(i == sel) LCD_Inv();

		switch (i){
			case 0:	LCD_SetCursor(10, 150); LCD_WriteChar1(Number_Table[time.hour/10]); LCD_WriteChar1(Number_Table[time.hour%10]); break;
			case 1: LCD_SetCursor(13, 150); LCD_WriteChar1(Number_Table[time.min/10]); LCD_WriteChar1(Number_Table[time.min%10]); break;
			case 2:	LCD_SetCursor(16, 150); LCD_WriteChar1(Number_Table[time.sec/10]); LCD_WriteChar1(Number_Table[time.sec%10]); break;
			case 3: LCD_WriteText2(16, 180, "确定");
			default: break;
		}

		if(i == sel) LCD_NoInv();
	}
}

void Add_PeriodOfTime(void)
{
	uint8_t KEY_FLAG_Return;
	uint8_t sel;
	uint8_t Menu_Init_Finished;
	uint8_t periodoftime_temp;
	ShortTime_TypeDef time;
	uint8_t Time_FLAG_Repeatability;
	uint8_t i;

	periodoftime_temp = 0;
	Time_FLAG_Repeatability = 0;
	sel = 0;  //对应时 分 秒 确定键
	time.hour = 12;
	time.min = 0;
	time.sec = 0;
	KEY_FLAG_Return = 0;
	Menu_Init_Finished = 0;

	while(1){
		if(Menu_Init_Finished == 0){

LCD_FLAG_Using = 1;
		
			LCD_ClearItemAllText();
			
			LCD_SetAppwindow(75, 239, 2, 28);
			
			LCD_WriteText2(2, 80, "请设置要添加的时段初始时间，选确定键保存设置");
			LCD_WriteText2(2, 120, "时段起始时间：");
			LCD_WriteText2(10, 150, "  -  -  ");
			LCD_WriteText2(14, 180, "【    】");
			
			LCD_SetAppwindow(96, 170, 29, 38);
			LCD_WriteText2(29, 96, "设置时段，按确定键保存");
			
			LCD_SetAppwindow(0, 239, 0, 39); 
		
			LCD_WriteText2(29, 176, "\x1A\x1B:移动");
			LCD_WriteText2(29, 196, "\x18\x19:选择");
			
			Add_PeriodOfTime_Update(sel, time);
		
LCD_FLAG_Using = 0;
if(LCD_FLAG_UpdateIrq == 1){
	EXTI_GenerateSWInterrupt(EXTI_Line1);
}
			Menu_Init_Finished = 1;
		}

		if(KEY_Press > 0){
			switch (KEY_Buffer[KEY_BufferPointer]){
				case KEY6:  if(sel==0) sel=3; else sel--; break;
				case KEY1:  if(sel==3) sel=0; else sel++; break;
				case KEY2:	{
								switch(sel){
									case 0: if(time.hour<23) time.hour++; else time.hour=0; break;
									case 1: if(time.min<59) time.min++; else time.min=0; break;
									case 2: if(time.sec<59) time.sec++; else time.sec=0; break;
									default: break;
								}
								break;
							}
				case KEY3:	{
								switch(sel){
									case 0: if(time.hour>0) time.hour--; else time.hour=23; break;
									case 1: if(time.min>0) time.min--; else time.min=59; break;
									case 2: if(time.sec>0) time.sec--; else time.sec=59; break;
									default: break;
								}
								break;
							}
				case KEY4:  {
								if(sel == 3){
									if(my_Schedule.TimeTable_Number < 16){	 //判断时段数是否满了
										//判断设置时间位于的时段号
										for(i=0, periodoftime_temp=my_Schedule.TimeTable_Number; i<my_Schedule.TimeTable_Number; i++){
											if(time.hour < my_Schedule.TimeTable[i].hour){
												periodoftime_temp = i;
												break;
											}
											else if(time.hour > my_Schedule.TimeTable[i].hour) continue;
											else{
												if(time.min < my_Schedule.TimeTable[i].min){
													periodoftime_temp = i;
													break;
												}
												else if(time.min > my_Schedule.TimeTable[i].min) continue;
												else{
													if(time.sec < my_Schedule.TimeTable[i].sec){
														periodoftime_temp = i;
														break;
													}
													else if(time.sec > my_Schedule.TimeTable[i].sec) continue;
													else{
														//时段时间重复了
														Time_FLAG_Repeatability = 1;
														break;
													}
												}
											}
										}

										if(Time_FLAG_Repeatability == 1){
											LCD_FLAG_Using = 1;

											LCD_ClearItemText();
											LCD_SetAppwindow(0, 239, 0, 39);
											LCD_WriteText2(8, 150, "相位时间重复");										

LCD_FLAG_Using = 0;
if(LCD_FLAG_UpdateIrq == 1){
	EXTI_GenerateSWInterrupt(EXTI_Line1);
}
											delay_us(10000);
											Menu_Init_Finished = 0;
											Time_FLAG_Repeatability = 0;
										}
										else{										
											while(Current_CutDown==0 && Current_PhaseTable==Phase_OneShot.Phase_Number-1);

											RTC_WaitForLastTask();
											RTC_ITConfig(RTC_IT_SEC, DISABLE);

											for(i=my_Schedule.TimeTable_Number; i>periodoftime_temp; i--){
												my_Schedule.TimeTable[i] = my_Schedule.TimeTable[i-1];
												my_Schedule.PhaseTable[i] = my_Schedule.PhaseTable[i-1];
											}
											my_Schedule.TimeTable[periodoftime_temp] = time;
											my_Schedule.PhaseTable[periodoftime_temp].Phase_Number = 1;
											my_Schedule.PhaseTable[periodoftime_temp].Last_Time[0] = 0;
											for(i=0; i<6; i++)
												my_Schedule.PhaseTable[periodoftime_temp].Crossway_LED[i][0] = 0xFC00;
											my_Schedule.TimeTable_Number ++;
											
											RTC_WaitForLastTask();
											RTC_ITConfig(RTC_IT_SEC, ENABLE);

LCD_FLAG_Using = 1;

											LCD_ClearItemText();
											LCD_SetAppwindow(0, 239, 0, 39);
											LCD_WriteText2(8, 150, "相位添加成功");										

LCD_FLAG_Using = 0;
if(LCD_FLAG_UpdateIrq == 1){
	EXTI_GenerateSWInterrupt(EXTI_Line1);
}
											delay_us(10000);
											Menu_Init_Finished = 0;
										}
									}
									else{										
LCD_FLAG_Using = 1;

										LCD_ClearItemText();
										LCD_SetAppwindow(0, 239, 0, 39);
										LCD_WriteText2(8, 150, "时段数已满");										

LCD_FLAG_Using = 0;
if(LCD_FLAG_UpdateIrq == 1){
	EXTI_GenerateSWInterrupt(EXTI_Line1);
}
										delay_us(10000);
										Menu_Init_Finished = 0;
									}
								}
								break;
							}
				case KEY5:	KEY_FLAG_Return = 1;  break;
			}

			KEY_PressMinus();

			if(KEY_FLAG_Return == 1){
				LCD_DisplayOtherMenu(Current_Menu);
				return;
			}

LCD_FLAG_Using = 1;

			Add_PeriodOfTime_Update(sel, time);

LCD_FLAG_Using = 0;
if(LCD_FLAG_UpdateIrq == 1){
	EXTI_GenerateSWInterrupt(EXTI_Line1);
}
		}
	}
}
#endif
#if MOVE
void Add_PhaseInfo_Update(uint8_t periodoftime, uint8_t phasenum, uint8_t sel, uint8_t options)
{
	uint8_t i;

	for(i=0; i<4; i++){
		if(i == sel)
		{
			LCD_Inv();
		}
		LCD_SetCursor(Func_AddPhase_Place[i][0], Func_AddPhase_Place[i][1]);
		switch (i){
			case 0:	LCD_WriteChar1(Number_Table[(periodoftime+1)/10]); LCD_WriteChar1(Number_Table[(periodoftime+1)%10]); break;
			case 1:	LCD_WriteChar1(Number_Table[(phasenum+1)/10]); LCD_WriteChar1(Number_Table[(phasenum+1)%10]); break;
			case 2:	if(options == 0) LCD_WriteText1("向上插入"); else LCD_WriteText1("向下插入");	break;
			case 3:	LCD_WriteText1("生成相位");	break;
			default: break;
		}
		if(i == sel){
			LCD_NoInv();
		}
	}
}

void Add_Phase(void)
{
	uint8_t KEY_FLAG_Return;
	uint8_t sel;
	uint8_t periodoftime, phasenum;
	uint8_t options;
	uint8_t Menu_Init_Finished;
	//uint8_t cutdown;
	uint8_t i, j;

	KEY_FLAG_Return = 0;
	Menu_Init_Finished = 0;
	sel = 0;
	periodoftime = 0;
	phasenum = 0;
	options = 0;
	//cutdown = 0;

	while(1){
		if(Menu_Init_Finished == 0){

LCD_FLAG_Using = 1;
		
			LCD_ClearItemAllText();
			
			LCD_SetAppwindow(75, 239, 2, 28);
			
			LCD_WriteText2(2, 80, "请设置要添加的相位位置,添加的相位初始时间为0,初始相位全为红灯");
			LCD_WriteText2(5, 150, "时段【  】 相位【  】");
			LCD_WriteText2(2, 180, "【向上插入】【生成相位】");
			
			LCD_SetAppwindow(96, 170, 29, 38);
			LCD_WriteText2(29, 96, "在选定时段和相位,按照设定添加一个初始相位");
			
			LCD_SetAppwindow(0, 239, 0, 39); 
		
			LCD_WriteText2(29, 176, "\x1A\x1B:移动");
			LCD_WriteText2(29, 196, "\x18\x19:选择");
			
			Add_PhaseInfo_Update(periodoftime, phasenum, sel, options);
		
LCD_FLAG_Using = 0;
if(LCD_FLAG_UpdateIrq == 1){
	EXTI_GenerateSWInterrupt(EXTI_Line1);
}
			Menu_Init_Finished = 1;
		}

		if(KEY_Press > 0){
			switch (KEY_Buffer[KEY_BufferPointer]){
				case KEY6:  if(sel==0) sel=3; else sel--; break;
				case KEY1:  if(sel==3) sel=0; else sel++; break;
				case KEY2:	{
								switch(sel){
									case 0: if(periodoftime < my_Schedule.TimeTable_Number-1) periodoftime++; break;
									case 1: if(phasenum < my_Schedule.PhaseTable[periodoftime].Phase_Number-1) phasenum ++;break;
									case 2: if(options == 0) options = 1; else options = 0; break;
									//case 2: break;
									default: break;
								}
								break;
							}
				case KEY3:	{
								switch(sel){
									case 0: if(periodoftime > 0) periodoftime --; break;
									case 1: if(phasenum > 0) phasenum --; break;
									case 2: if(options == 0) options = 1; else options = 0; break;
									//case 2: break;
									default: break;
								}
								break;
							}
				case KEY4:  {
								if(sel == 3){
									if(my_Schedule.PhaseTable[periodoftime].Phase_Number < 16){	 //判断相位数是否满了
										RTC_WaitForLastTask();
										RTC_ITConfig(RTC_IT_SEC, DISABLE);
										
										while(Current_CutDown==0 && Current_PhaseTable==Phase_OneShot.Phase_Number-1);
										
										if(options == 1){   //向下插入相位模式
											if(phasenum != my_Schedule.PhaseTable[periodoftime].Phase_Number-1){
												for(i=my_Schedule.PhaseTable[periodoftime].Phase_Number; i>phasenum+1; i--){
													for(j=0; j<6; j++){
														my_Schedule.PhaseTable[periodoftime].Crossway_LED[j][i] = my_Schedule.PhaseTable[periodoftime].Crossway_LED[j][i-1];
													}
													my_Schedule.PhaseTable[periodoftime].Last_Time[i] = my_Schedule.PhaseTable[periodoftime].Last_Time[i-1];
												}
											}
											for(j=0; j<6; j++){
												my_Schedule.PhaseTable[periodoftime].Crossway_LED[j][phasenum+1] = 0xFC00;
												//my_Schedule.PhaseTable[periodoftime].Crossway_LED[j][i] = 0xFC00;
											}
											my_Schedule.PhaseTable[periodoftime].Last_Time[phasenum+1] = 0;
											//my_Schedule.PhaseTable[periodoftime].Last_Time[i] = 0;
										}
										else{  //options == 0  向上插入相位模式
											for(i=i=my_Schedule.PhaseTable[periodoftime].Phase_Number; i>phasenum; i--){
												for(j=0; j<6; j++){
													my_Schedule.PhaseTable[periodoftime].Crossway_LED[j][i] = my_Schedule.PhaseTable[periodoftime].Crossway_LED[j][i-1];
												}
												my_Schedule.PhaseTable[periodoftime].Last_Time[i] = my_Schedule.PhaseTable[periodoftime].Last_Time[i-1];
											}	
											for(j=0; j<6; j++){
												my_Schedule.PhaseTable[periodoftime].Crossway_LED[j][phasenum] = 0xFC00;
												//my_Schedule.PhaseTable[periodoftime].Crossway_LED[j][i] = 0xFC00;
											}
											my_Schedule.PhaseTable[periodoftime].Last_Time[phasenum] = 0;
											//my_Schedule.PhaseTable[periodoftime].Last_Time[i] = 0;
										}
										my_Schedule.PhaseTable[periodoftime].Phase_Number ++;
										
										RTC_WaitForLastTask();
										RTC_ITConfig(RTC_IT_SEC, ENABLE);

LCD_FLAG_Using = 1;

										LCD_ClearItemText();
										LCD_SetAppwindow(0, 239, 0, 39);
										LCD_WriteText2(8, 150, "相位添加成功");										

LCD_FLAG_Using = 0;
if(LCD_FLAG_UpdateIrq == 1){
	EXTI_GenerateSWInterrupt(EXTI_Line1);
}
										delay_us(10000);
										Menu_Init_Finished = 0;
									}
								}
								break;
							}
				case KEY5:	KEY_FLAG_Return = 1;  break;
			}

			KEY_PressMinus();

			if(KEY_FLAG_Return == 1){
				LCD_DisplayOtherMenu(Current_Menu);
				return;
			}

LCD_FLAG_Using = 1;

			Add_PhaseInfo_Update(periodoftime, phasenum, sel, options);

LCD_FLAG_Using = 0;
if(LCD_FLAG_UpdateIrq == 1){
	EXTI_GenerateSWInterrupt(EXTI_Line1);
}
		}
	}
}
#endif
#if MOVE
void Delete_PeriodOfTime_Update(struct ShortTime start_time, struct ShortTime end_time, uint8_t sel, uint8_t periodoftime)
{
	uint8_t i;

	for(i=0; i<2; i++){
		if(i == sel) LCD_Inv();
		switch(i){
			case 0:	LCD_SetCursor(18, 100); LCD_WriteChar1(Number_Table[(periodoftime+1)/10]); LCD_WriteChar1(Number_Table[(periodoftime+1)%10]); break;
			case 1: LCD_WriteText2(16, 160, "删除");
			default: break;
		}
		if(i == sel) LCD_NoInv();
	}

	LCD_SetCursor(13, 120);
	LCD_WriteChar1((start_time.hour<10)?' ':Number_Table[start_time.hour/10]);
	LCD_WriteChar1(Number_Table[start_time.hour%10]);
	LCD_WriteChar1('-');
	LCD_WriteChar1((start_time.min<10)?' ':Number_Table[start_time.min/10]);
	LCD_WriteChar1(Number_Table[start_time.min%10]);
	LCD_WriteChar1('-');
	LCD_WriteChar1((start_time.sec<10)?' ':Number_Table[start_time.sec/10]);
	LCD_WriteChar1(Number_Table[start_time.sec%10]);

	LCD_SetCursor(13, 140);
	LCD_WriteChar1((end_time.hour<10)?' ':Number_Table[end_time.hour/10]);
	LCD_WriteChar1(Number_Table[end_time.hour%10]);
	LCD_WriteChar1('-');
	LCD_WriteChar1((end_time.min<10)?' ':Number_Table[end_time.min/10]);
	LCD_WriteChar1(Number_Table[end_time.min%10]);
	LCD_WriteChar1('-');
	LCD_WriteChar1((end_time.sec<10)?' ':Number_Table[end_time.sec/10]);
	LCD_WriteChar1(Number_Table[end_time.sec%10]);
}

void Delete_PeriodOfTime(void)
{
	uint8_t KEY_FLAG_Return;
	uint8_t sel;
	uint8_t periodoftime;
	uint8_t Menu_Init_Finished;
	ShortTime_TypeDef start_time, end_time;
	//uint8_t Display_FLAG_NextDay;
	uint8_t i;

	KEY_FLAG_Return = 0;
	//Display_FLAG_NextDay = 0;
	sel = 0;   //这里只选择时段号与删除键
	periodoftime = 0;
	Menu_Init_Finished = 0;
	start_time = my_Schedule.TimeTable[0];
	if(my_Schedule.TimeTable_Number == 1){
		end_time = my_Schedule.TimeTable[0];
		//Display_FLAG_NextDay = 1;
	}
	else
		end_time = my_Schedule.TimeTable[1];

	while(1){
		if(Menu_Init_Finished == 0){

LCD_FLAG_Using = 1;
		
			LCD_ClearItemAllText();

			LCD_SetAppwindow(96, 170, 29, 38);
			LCD_WriteText2(29, 96, "删除所选的时段");
			
			LCD_SetAppwindow(0, 239, 0, 39); 
			
			LCD_WriteText2(2, 80, "请选择要删除的时段");
			LCD_WriteText2(8, 100, "选择时段【  】");
			LCD_WriteText2(4, 120, "起始时间:");
			LCD_WriteText2(4, 140, "终止时间:");
			LCD_WriteText2(14, 160, "【    】");
		
			LCD_WriteText2(29, 176, "\x1A\x1B:移动");
			LCD_WriteText2(29, 196, "\x18\x19:选择");
			
			Delete_PeriodOfTime_Update(start_time, end_time, sel, periodoftime);

LCD_FLAG_Using = 0;
if(LCD_FLAG_UpdateIrq == 1){
	EXTI_GenerateSWInterrupt(EXTI_Line1);
}
			Menu_Init_Finished = 1;
		}

		if(KEY_Press > 0){
			switch (KEY_Buffer[KEY_BufferPointer]){
				case KEY6:  if(sel==0) sel=1; else sel=0; break;
				case KEY1:  if(sel==1) sel=0; else sel=1; break;
				case KEY2:	{
								if(sel == 0){
									if(periodoftime<my_Schedule.TimeTable_Number-1){
										//Display_FLAG_NextDay = 0; 
										periodoftime++;
										start_time = my_Schedule.TimeTable[periodoftime];
										if(periodoftime == my_Schedule.TimeTable_Number-1){
											end_time = my_Schedule.TimeTable[0];
											//Display_FLAG_NextDay = 1;
										}
										else{
											end_time = my_Schedule.TimeTable[periodoftime+1]; 
										}
									}
								}
								break;
							}
				case KEY3:	{
								if(sel == 0){
									if(periodoftime>0){ 
										periodoftime --;
										start_time = my_Schedule.TimeTable[periodoftime];
										end_time = my_Schedule.TimeTable[periodoftime+1];
									} 
								}
								break;
							}
				case KEY4:  {							
								if(my_Schedule.TimeTable_Number >1){
									RTC_WaitForLastTask();
									RTC_ITConfig(RTC_IT_SEC, DISABLE);								
	
									while(Current_CutDown==0 && Current_PhaseTable==Phase_OneShot.Phase_Number-1);
									
									for(i=periodoftime; i<my_Schedule.TimeTable_Number-2; i++){
										my_Schedule.TimeTable[i] = my_Schedule.TimeTable[i+1];
										my_Schedule.PhaseTable[i] = my_Schedule.PhaseTable[i+1];
									}
									my_Schedule.TimeTable_Number --;
	
									if(periodoftime>0)
										periodoftime --;
	
									start_time = my_Schedule.TimeTable[periodoftime];
									if(periodoftime == my_Schedule.TimeTable_Number-1){
										end_time = my_Schedule.TimeTable[0];
										//Display_FLAG_NextDay = 1;
									}
									else{
										end_time = my_Schedule.TimeTable[periodoftime+1]; 
									}
	
									RTC_WaitForLastTask();
									RTC_ITConfig(RTC_IT_SEC, ENABLE);

LCD_FLAG_Using = 1;

									LCD_ClearItemText();
									LCD_SetAppwindow(0, 239, 0, 39);
									LCD_WriteText2(9, 150, "删除时段成功");										

LCD_FLAG_Using = 0;
if(LCD_FLAG_UpdateIrq == 1){
	EXTI_GenerateSWInterrupt(EXTI_Line1);
}

									delay_us(10000);
									Menu_Init_Finished = 0;
								}
								break;
							}
				case KEY5:	KEY_FLAG_Return = 1;  break;
			}

			KEY_PressMinus();

			if(KEY_FLAG_Return == 1){
				LCD_DisplayOtherMenu(Current_Menu);
				return;
			}

LCD_FLAG_Using = 1;

			Delete_PeriodOfTime_Update(start_time, end_time, sel, periodoftime);

LCD_FLAG_Using = 0;
if(LCD_FLAG_UpdateIrq == 1){
	EXTI_GenerateSWInterrupt(EXTI_Line1);
}
		}
	}
}
#endif
#if MOVE
void Set_PeriodOfTime_Update(uint8_t sel, uint8_t periodoftime, struct ShortTime time)
{
	uint8_t i;

	for(i=0; i<4; i++){
		if(i == sel){
			LCD_Inv();
		}
		LCD_SetCursor(Func_SetPeriodOfTime_Place[i][0], Func_SetPeriodOfTime_Place[i][1]);
		switch (i){
			case 0:	LCD_WriteChar1(Number_Table[(periodoftime+1)/10]); LCD_WriteChar1(Number_Table[(periodoftime+1)%10]); break;
			case 1:	LCD_WriteChar1(Number_Table[time.hour/10]); LCD_WriteChar1(Number_Table[time.hour%10]); break;
			case 2: LCD_WriteChar1(Number_Table[time.min/10]); LCD_WriteChar1(Number_Table[time.min%10]); break;
			case 3:	LCD_WriteChar1(Number_Table[time.sec/10]); LCD_WriteChar1(Number_Table[time.sec%10]); break;
			default: break;
		}
		if(i == sel){
			LCD_NoInv();
		}
	}
}

void Set_PeriodOfTime(void)
{
	uint8_t KEY_FLAG_Return;
	uint8_t sel;
	uint8_t periodoftime;
	uint8_t Menu_Init_Finished;
	ShortTime_TypeDef temptime;

	KEY_FLAG_Return = 0;
	sel = 0;
	periodoftime = 0;
	Menu_Init_Finished = 0;
	temptime = my_Schedule.TimeTable[periodoftime];

	while(1){
		if(Menu_Init_Finished == 0){

LCD_FLAG_Using = 1;
		
			LCD_ClearItemAllText();

			LCD_SetAppwindow(96, 170, 29, 38);
			LCD_WriteText2(29, 96, "设置时段时间");
			
			LCD_SetAppwindow(0, 239, 0, 39); 
			
			LCD_WriteText2(2, 80, "请设置所选时段的时间");
			LCD_WriteText2(8, 100, "选择时段【  】");
			LCD_WriteText2(2, 120, "时段时间:  -  -  ");
		
			LCD_WriteText2(29, 176, "\x1A\x1B:移动");
			LCD_WriteText2(29, 196, "\x18\x19:选择");
			
			Set_PeriodOfTime_Update(sel, periodoftime, temptime);
		
LCD_FLAG_Using = 0;
if(LCD_FLAG_UpdateIrq == 1){
	EXTI_GenerateSWInterrupt(EXTI_Line1);
}
			Menu_Init_Finished = 1;
		}

		if(KEY_Press > 0){
			switch (KEY_Buffer[KEY_BufferPointer]){
				case KEY6:  if(sel==0) sel=3; else sel--; break;
				case KEY1:  if(sel==3) sel=0; else sel++; break;
				case KEY2:	{
								switch (sel){
									case 0: {
												if(periodoftime<my_Schedule.TimeTable_Number-1){ 
													periodoftime++;
													temptime = my_Schedule.TimeTable[periodoftime]; 
												}
												break;
											}
									case 1: {
												if(periodoftime<my_Schedule.TimeTable_Number-1){ 
													if(temptime.hour<my_Schedule.TimeTable[periodoftime+1].hour-1){ 
														temptime.hour ++;
													}
													else if(temptime.min<=my_Schedule.TimeTable[periodoftime+1].min && temptime.sec<=my_Schedule.TimeTable[periodoftime+1].sec){
														temptime.hour ++;
													}	
												}
												else{
													if(temptime.hour<23)
														temptime.hour ++;
												}
												break;
											}
									case 2: {
												if(periodoftime<my_Schedule.TimeTable_Number-1 && temptime.hour==my_Schedule.TimeTable[periodoftime+1].hour){
													if(temptime.min<my_Schedule.TimeTable[periodoftime+1].min){
														temptime.min ++;
													}
												}
												else{
													if(temptime.min<59)
														temptime.min ++;
												}
												break;
											}
									case 3: {
												if(periodoftime<my_Schedule.TimeTable_Number-1 && temptime.hour==my_Schedule.TimeTable[periodoftime+1].hour && temptime.min==my_Schedule.TimeTable[periodoftime+1].min){
													if(temptime.sec<my_Schedule.TimeTable[periodoftime+1].sec-1){
														temptime.sec ++;
													}
												}
												else{
													if(temptime.sec<59)
														temptime.sec ++;
												}
												break;
											}
									default: break;
								}
								break;
							}
				case KEY3:	{
								switch (sel){
									case 0: {
												if(periodoftime>0){ 
													periodoftime --;
													temptime = my_Schedule.TimeTable[periodoftime];
												} 
												break;
											}
									case 1: {
												if(periodoftime>0){
													if(temptime.hour>my_Schedule.TimeTable[periodoftime-1].hour+1){
														temptime.hour --;
													}
													else if(temptime.min>=my_Schedule.TimeTable[periodoftime-1].min && temptime.sec>my_Schedule.TimeTable[periodoftime-1].sec+1){
														temptime.hour --;
													}
												}
												else{
													if(temptime.hour>0) 
														temptime.hour --;
												}
												break;
											}
									case 2: {
												if(periodoftime>0 && temptime.hour==my_Schedule.TimeTable[periodoftime-1].hour){
													if(temptime.min>my_Schedule.TimeTable[periodoftime-1].min)
														temptime.min --;
												}
												else{
													if(temptime.min>0)
														temptime.min --;
												} 
												break;
											}
									case 3: {
												if(periodoftime>0 && temptime.hour==my_Schedule.TimeTable[periodoftime-1].hour && temptime.min==my_Schedule.TimeTable[periodoftime-1].min){
													if(temptime.sec>my_Schedule.TimeTable[periodoftime-1].sec+1)
														temptime.sec --;
												}
												else{
													if(temptime.sec>0)
														temptime.sec --;
												} 
												break;
											}
									default: break;
								}
								break;
							}
				case KEY4:  {							
								RTC_WaitForLastTask();
								RTC_ITConfig(RTC_IT_SEC, DISABLE);
								
								while(Current_CutDown==0 && Current_PhaseTable==Phase_OneShot.Phase_Number-1);

								my_Schedule.TimeTable[periodoftime] = temptime;

								RTC_WaitForLastTask();
								RTC_ITConfig(RTC_IT_SEC, ENABLE);

LCD_FLAG_Using = 1;

								LCD_ClearItemText();
								LCD_SetAppwindow(0, 239, 0, 39);
								LCD_WriteText2(9, 150, "修改时段成功");										

LCD_FLAG_Using = 0;
if(LCD_FLAG_UpdateIrq == 1){
	EXTI_GenerateSWInterrupt(EXTI_Line1);
}
								delay_us(10000);
								Menu_Init_Finished = 0;

								break;
							}
				case KEY5:	KEY_FLAG_Return = 1;  break;
			}

			KEY_PressMinus();

			if(KEY_FLAG_Return == 1){
				LCD_DisplayOtherMenu(Current_Menu);
				return;
			}

LCD_FLAG_Using = 1;

			Set_PeriodOfTime_Update(sel, periodoftime, temptime);

LCD_FLAG_Using = 0;
if(LCD_FLAG_UpdateIrq == 1){
	EXTI_GenerateSWInterrupt(EXTI_Line1);
}
		}
	}
}
#endif
#if MOVE
void Delete_Phase(void)
{
	uint8_t KEY_FLAG_Return;
	uint8_t Menu_Init_Finished;
	uint8_t crossway;
	uint8_t sel;  //只对crossway periodoftime phasenum进行修改，还有删除确认键
	uint8_t periodoftime, phasenum;
	uint16_t phase;  //不允许修改，只用于观察用
	uint8_t cutdown;
	uint8_t i, j;

	KEY_FLAG_Return = 0;
	Menu_Init_Finished = 0;
	crossway = 0;
	sel = 0;
	periodoftime = 0;
	phasenum = 0;
	phase = my_Schedule.PhaseTable[periodoftime].Crossway_LED[crossway][phasenum];
	cutdown = my_Schedule.PhaseTable[periodoftime].Last_Time[phasenum];

	while(1){
		if(Menu_Init_Finished == 0){
LCD_FLAG_Using = 1;

			LCD_ClearItemAllText();
			
			LCD_SetAppwindow(0, 239, 0, 39);
			
			LCD_WriteText2(2, 80, "\x11      \x10 (×:表示该灯不亮)" );
			LCD_WriteText2(2, 98, "时段【  】 相位【  】");
			LCD_WriteText2(2, 126, "左转 [  ] 直行 [  ]");
			LCD_WriteText2(2, 146, "右转 [  ] 调头 [  ]");
			LCD_WriteText2(2, 166, "人行1[  ] 人行2[  ]");
			LCD_WriteText2(2, 186, "相位运行时间:  秒");
			LCD_WriteText2(10, 206, "【删除】");
			
			LCD_SetAppwindow(96, 170, 29, 38);
			LCD_WriteText2(29, 96, "删除操作将删除所有灯组的选定时段的相位");
			
			LCD_SetAppwindow(0, 239, 0, 39); 
		
			LCD_WriteText2(29, 176, "\x1A\x1B:移动");
			LCD_WriteText2(29, 196, "\x18\x19:选择");
			
			Delete_PhaseInfo_Update(sel, crossway, periodoftime, phasenum, phase, cutdown);

LCD_FLAG_Using = 0;
if(LCD_FLAG_UpdateIrq == 1){
	EXTI_GenerateSWInterrupt(EXTI_Line1);
}
			
			Menu_Init_Finished = 1;	
		}

		if(KEY_Press > 0){
			switch (KEY_Buffer[KEY_BufferPointer]){
				case KEY6:  if(sel==0) sel=3; else sel--; break;
				case KEY1:  if(sel==3) sel=0; else sel++; break;
				case KEY2:	{
								switch(sel){
									case 0: if(crossway < 5) crossway ++; else crossway = 0; break;
									case 1: if(periodoftime < my_Schedule.TimeTable_Number-1) periodoftime ++; else periodoftime = 0; phasenum=0; break;
									case 2: if(phasenum < my_Schedule.PhaseTable[periodoftime].Phase_Number-1) phasenum ++; else phasenum = 0; break;
									default: break;
								}
								phase = my_Schedule.PhaseTable[periodoftime].Crossway_LED[crossway][phasenum];
								cutdown = my_Schedule.PhaseTable[periodoftime].Last_Time[phasenum];
								break;
							}
				case KEY3:	{
								switch(sel){
									case 0: if(crossway > 0) crossway --; else crossway = 5; break;
									case 1: if(periodoftime > 0) periodoftime --; else periodoftime = my_Schedule.TimeTable_Number-1; phasenum=0;break;
									case 2: if(phasenum > 0) phasenum --; else phasenum = my_Schedule.PhaseTable[periodoftime].Phase_Number-1; break;
									default: break;
								}
								phase = my_Schedule.PhaseTable[periodoftime].Crossway_LED[crossway][phasenum];
								cutdown = my_Schedule.PhaseTable[periodoftime].Last_Time[phasenum];
								break;
							}
				case KEY4:  {
								if(sel == 3){
									if(my_Schedule.PhaseTable[periodoftime].Phase_Number > 1){
										RTC_WaitForLastTask();
										RTC_ITConfig(RTC_IT_SEC, DISABLE);

										while(Current_CutDown==0 && Current_PhaseTable==Phase_OneShot.Phase_Number-1);

										for(j=0; j<6; j++){	 //j:crossway
											for(i=phasenum; i<my_Schedule.PhaseTable[periodoftime].Phase_Number-1; i++){
												my_Schedule.PhaseTable[periodoftime].Crossway_LED[j][i] = my_Schedule.PhaseTable[periodoftime].Crossway_LED[j][i+1];
											} 
										}
										for(i=phasenum; i<my_Schedule.PhaseTable[periodoftime].Phase_Number-1; i++){
											my_Schedule.PhaseTable[periodoftime].Last_Time[i] = my_Schedule.PhaseTable[periodoftime].Last_Time[i+1];;
										}
										for(j=0; j<6; j++){
											my_Schedule.PhaseTable[periodoftime].Crossway_LED[j][my_Schedule.PhaseTable[periodoftime].Phase_Number-1] = 0xFC00;
										}
										my_Schedule.PhaseTable[periodoftime].Last_Time[my_Schedule.PhaseTable[periodoftime].Phase_Number-1] = 0;
										my_Schedule.PhaseTable[periodoftime].Phase_Number -= 1;
										
										RTC_WaitForLastTask();
										RTC_ITConfig(RTC_IT_SEC, ENABLE);

										phasenum = my_Schedule.PhaseTable[periodoftime].Phase_Number-1;

LCD_FLAG_Using = 1;

										LCD_ClearItemText();
										LCD_SetAppwindow(0, 239, 0, 39);
										LCD_WriteText2(8, 150, "删除相位成功");										
				
LCD_FLAG_Using = 0;
if(LCD_FLAG_UpdateIrq == 1){
	EXTI_GenerateSWInterrupt(EXTI_Line1);
}
										delay_us(10000);
										Menu_Init_Finished = 0;
									}
								}
								break;
							}
				case KEY5:	KEY_FLAG_Return = 1;  break;
			}

			KEY_PressMinus();

			if(KEY_FLAG_Return == 1){
				LCD_DisplayOtherMenu(Current_Menu);
				return;
			}

LCD_FLAG_Using = 1;

			Delete_PhaseInfo_Update(sel, crossway, periodoftime, phasenum, phase, cutdown);

LCD_FLAG_Using = 0;
if(LCD_FLAG_UpdateIrq == 1){
	EXTI_GenerateSWInterrupt(EXTI_Line1);
}
		}
	}
}
#endif


#if MOVE
void Delete_PhaseInfo_Update(uint8_t sel, uint8_t crossway, uint8_t PeriodOfTimeNum, uint8_t PhaseNum, uint16_t phase, uint8_t cutdown){
	uint8_t i;

	for(i=0; i<4; i++){
		if(i == sel){
			LCD_Inv();
		}
		switch (i){
			case 0: {
						LCD_WriteText2(3, 80, "灯组");
						LCD_WriteText1((uint8_t*)Weekday_Table[crossway+1]);
						break;
					}
			case 1: {
						LCD_WriteChar2(8, 98, Number_Table[(PeriodOfTimeNum+1)/10]);
						LCD_WriteChar1(Number_Table[(PeriodOfTimeNum+1)%10]);
						break;
					}
			case 2: {
						LCD_WriteChar2(19, 98, Number_Table[(PhaseNum+1)/10]);
						LCD_WriteChar1(Number_Table[(PhaseNum+1)%10]);
						break;
					}
			case 3: {
						LCD_WriteText2(12, 206, "删除");
						break;
					}
			default: break;
		}
		if(i == sel){
			LCD_NoInv();
		}
	}
	for(i=0; i<7; i++){
		LCD_SetCursor(Func_PhaseInfo_Place[i][0], Func_PhaseInfo_Place[i][1]);
		switch (i){
			case 0: {
						if((phase&LeftGreen) != 0x00){
							LCD_WriteText1((uint8_t*)LedColor_Table[0]);
						}
						else if((phase&LeftRed) != 0x00){
							LCD_WriteText1((uint8_t*)LedColor_Table[1]);
						}
						else if((phase&LeftYellow) != 0x00){
							LCD_WriteText1((uint8_t*)LedColor_Table[2]);
						}
						else{
							LCD_WriteText1((uint8_t*)Tick_Cross_Table[0]);
						}
						break;
					}
			case 1: {
						if((phase&StraightGreen) != 0x00){
							LCD_WriteText1((uint8_t*)LedColor_Table[0]);
						}
						else if((phase&StraightRed) != 0x00){
							LCD_WriteText1((uint8_t*)LedColor_Table[1]);
						}
						else if((phase&StraightYellow) != 0x00){
							LCD_WriteText1((uint8_t*)LedColor_Table[2]);
						}
						else{
							LCD_WriteText1((uint8_t*)Tick_Cross_Table[0]);
						}
						break;
					}
			case 2: {
						if((phase&RightGreen) != 0x00){
							LCD_WriteText1((uint8_t*)LedColor_Table[0]);
						}
						else if((phase&RightRed) != 0x00){
							LCD_WriteText1((uint8_t*)LedColor_Table[1]);
						}
						else if((phase&RightYellow) != 0x00){
							LCD_WriteText1((uint8_t*)LedColor_Table[2]);
						}
						else{
							LCD_WriteText1((uint8_t*)Tick_Cross_Table[0]);
						}
						break;
					}
			case 3: {
						if((phase&BackGreen) != 0x00){
							LCD_WriteText1((uint8_t*)LedColor_Table[0]);
						}
						else if((phase&BackRed) != 0x00){
							LCD_WriteText1((uint8_t*)LedColor_Table[1]);
						}
						else if((phase&BackYellow) != 0x00){
							LCD_WriteText1((uint8_t*)LedColor_Table[2]);
						}
						else{
							LCD_WriteText1((uint8_t*)Tick_Cross_Table[0]);
						}
						break;
					}
			case 4: {
						if((phase&SideWalk1Green) != 0x00){
							LCD_WriteText1((uint8_t*)LedColor_Table[0]);
						}
						else if((phase&SideWalk1Red) != 0x00){
							LCD_WriteText1((uint8_t*)LedColor_Table[1]);
						}
						else{
							LCD_WriteText1((uint8_t*)Tick_Cross_Table[0]);
						}
						break;
					}
			case 5: {
						if((phase&SideWalk2Green) != 0x00){
							LCD_WriteText1((uint8_t*)LedColor_Table[0]);
						}
						else if((phase&SideWalk2Red) != 0x00){
							LCD_WriteText1((uint8_t*)LedColor_Table[1]);
						}
						else{
							LCD_WriteText1((uint8_t*)Tick_Cross_Table[0]);
						}
						break;
					}
			case 6: {
						LCD_WriteChar1(Number_Table[cutdown/10]);
						LCD_WriteChar1(Number_Table[cutdown%10]);
						break;
					}
			default: break;
		}
	}
}

void Add_PhaseInfo_Update(uint8_t periodoftime, uint8_t phasenum, uint8_t sel, uint8_t options){
	uint8_t i;

	for(i=0; i<4; i++){
		if(i == sel)
		{
			LCD_Inv();
		}
		LCD_SetCursor(Func_AddPhase_Place[i][0], Func_AddPhase_Place[i][1]);
		switch (i){
			case 0:	LCD_WriteChar1(Number_Table[(periodoftime+1)/10]); LCD_WriteChar1(Number_Table[(periodoftime+1)%10]); break;
			case 1:	LCD_WriteChar1(Number_Table[(phasenum+1)/10]); LCD_WriteChar1(Number_Table[(phasenum+1)%10]); break;
			case 2:	if(options == 0) LCD_WriteText1("向上插入"); else LCD_WriteText1("向下插入");	break;
			case 3:	LCD_WriteText1("生成相位");	break;
			default: break;
		}
		if(i == sel){
			LCD_NoInv();
		}
	}
}
#endif
