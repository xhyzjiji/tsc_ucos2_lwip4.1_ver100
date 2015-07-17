#ifndef _RA8806_CTRL_H_
#define _RA8806_CTRL_H_

#include "stm32f10x.h"
#include "Menu.h"
#include "Traffic_Param.h"
#include "time.h"

//ʹ��8080Intel����ģʽ
#define BUSY GPIO_Pin_8
#define INT GPIO_Pin_9
#define CS GPIO_Pin_10
#define RS GPIO_Pin_11
#define WR GPIO_Pin_12
#define RD GPIO_Pin_13
#define RST GPIO_Pin_14
#define DB (GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7)
#define LCDPort GPIOE

//ѡ�����¹��ܣ���λ������λ
//��һ�����ù���ʱ������ѡ��֮��ʹ�û�ȡ������ʱ���Ĵ�����������ù��ܵķ�
#define WLCR	0x00						  	 //оƬ���ƼĴ���
#define Power_Mode_SLEEP      	((uint8_t)0x80)  //˯��ģʽ(��ѡΪ����ģʽ)
#define Linear_Decode_Mode    	((uint8_t)0x40)  //ʹ���Զ���ROM��ַ(��ѡΪBIG5/GB ROM��ַ)
#define Software_Reset        	((uint8_t)0x20)  //��DDRAM���ݲ����⣬�����Ĵ���������ΪĬ��ֵ
#define Text_Mode_Selection  	((uint8_t)0x08)  //����ģʽ(��ѡΪ��ͼģʽ)
#define Display_ON			  	((uint8_t)0x04)  //������ʾ
#define Blink_Mode_Selection  	((uint8_t)0x02)  //������˸
#define Inverse_Mode_Selection	((uint8_t)0x01)	 //��������

#define MISC	0x01							 //����Ĵ���
#define Eliminating_Flicker_ON 	((uint8_t)0x80)  //ѩ������ģʽ
#define Pin_CLK_OUT				((uint8_t)0x40)  //CLK_OUT�������ģʽ��0������ģʽ/1��˯��ģʽ
												 //(��ѡ���ϵͳƵ��)
#define Busy_Polarity_High		((uint8_t)0x20)  //����ϵͳæʱ��BUSY��������ߵ�ƽ(��ѡ��ϵͳæBUSY���0)
#define Interrupt_Polarity_High	((uint8_t)0x10)  //�����ж������ƽΪ�ߵ�ƽ(��ѡʱ�жϴ�����INT��������͵�ƽ)
#define Driver_Clock_Div8 		((uint8_t)0x00)  //XCKƵ��ѡ��
#define Driver_Clock_Div4		((uint8_t)0x04)  
#define Driver_Clock_Div2		((uint8_t)0x08)
#define Driver_Clock_Div1		((uint8_t)0x0C)
#define Seg_Scan_Direction		((uint8_t)0x02)	 //SEGɨ�跽��319-0(��ѡΪ0-319)
#define COM_Scan_Direction		((uint8_t)0x01)	 //COMɨ�跽��239-0(��ѡΪ0-239)

#define ADSR	0x03
#define Scroll_Function_Pending	((uint8_t)0x80)  //��������ͣ
#define Bit_Order				((uint8_t)0x08)	 //�����������DB7-DB0
#define SCR_DIR					((uint8_t)0x04)	 //������SCR_HV=1��������(��֮Ϊ������)
												 //SCR_HV=0��������(��֮Ϊ������)
#define SCR_HV					((uint8_t)0x02)  //��ֱ��/ˮƽ��
#define	SCR_EN					((uint8_t)0x01)  //�����ܿ���/�ر�

#define INTR	0x0F						//�ж��������жϱ�־λ

#define WCCR	0x10						//�����ƼĴ���
#define CUS_INC					((uint8_t)0x80)  //��ֹ��DDRAM���ж�д����ʱ�����λ�Զ���1
#define FULL_OFS				((uint8_t)0x40)  //ȫ����������ֻ����ʾ��������ȫ����λ�ö���
#define Inverse_Bit_Order		((uint8_t)0x20)  //����DDRAM�����ݱ�ȡ��
#define	Bold_Font				((uint8_t)0x10)  //����ģʽ����Ч����ʾΪ������
#define T90DEG					((uint8_t)0x08)  //������ת90��
#define Cursor_Display_ON		((uint8_t)0x04)  //��ʾ���
#define Cursor_Blink			((uint8_t)0x02)  //�����˸

#define CHWI    0x11						//���߶����о�new(have change)

#define MAMR	0x12						//ͼ�㻺��ģʽ
#define Cursor_Direction_Vert	((uint8_t)0x80)	 //��괹ֱ�ƶ�
#define	Display_Layer_Gray		((uint8_t)0x00)  //�ҽ�ģʽ
#define Display_Layer_DDRAM1	((uint8_t)0x10)  //����DDRAM1
#define Display_Layer_DDRAM2	((uint8_t)0x20)  //����DDRAM2
#define Display_Layer_DDRAMs	((uint8_t)0x30)  //˫����ʾ
#define Display_Layer_640240	((uint8_t)0x60)  //��չģʽ640*240
#define Display_Layer_320480	((uint8_t)0x60)  //��չģʽ320*480
#define TwoLayer_Rule_OR		((uint8_t)0x00)  //˫��DDRAM��λ��
#define TwoLayer_Rule_XOR		((uint8_t)0x04)  //˫��DDRAM��λ���
#define TwoLayer_Rule_NOR		((uint8_t)0x08)  //˫��DDRAM��λͬ��
#define TwoLayer_Rule_AND		((uint8_t)0x0C)  //˫��DDRAM��λ��
#define Layer_Selection_CGRAM	((uint8_t)0x00)  //ѡ��CGRAMΪ��ǰ����ͼ��
#define Layer_Selection_DDRAM1	((uint8_t)0x01)  //ѡ��DDRAM1Ϊ��ǰ����ͼ��
#define Layer_Selection_DDRAM2	((uint8_t)0x02)  //ѡ��DDRAM2Ϊ��ǰ����ͼ��
#define Layer_Selection_DDRAMs	((uint8_t)0x03)  //ͬʱѡ��DDRAM1��DDRAM2Ϊ��ǰ����ͼ��

#define CGRAM_odd				((uint8_t)0x00)
#define CGRAM_even				((uint8_t)0x10)

//�������ڴ�СΪ�Զ��壬��ʾ���ڴ�СΪ����Ļ���ؼ������
#define AWRR	0x20						//���������ұ߽�(���㷽�����߽�����(8�ı���)/8-1)
#define DWWR	0x21						//��ʾ���ڿ��(���㷽������Ļ����/8-1)
#define AWBR	0x30						//���������±߽�(�߽�����-1)
#define DWHR	0x31						//��ʾ���ڸ߶�(��Ļ����-1)new(have change)
#define AWLR	0x40						//����������߽�
#define AWTR	0x50						//���������ϱ߽�

#define CURX	0x60						//���ù��ˮƽSEGλ��(���㷽����ˮƽ����λ��(8�ı���)/8-1)new(have change)
											//������CGRAMͼ��ʱ��������Ϊд�����ݵ�λ��ַ
#define BGSG	0x61						//��������߽�(���㷽����������������)
#define EDSG    0x62						//�������ұ߽�new
#define CURY	0x70						//���ù�괹ֱCOMλ��(����λ��-1)new(have change)	
											//������CGRAMͼ��ʱ���üĴ���[3:0]λ��ʾ�ĸ��ֱ����죬CGRAM���֧�ִ���16����
#define BGCM	0x71						//�������ϱ߽�
#define EDCM	0x72						//�������±߽�
#define	BTMR	0x80						//�����˸�������
#define ITCR	0x90						//����ʱ�����new(have change)
#define KSCR1	0xA0						//����ɨ�����1
#define KSCR2	0xA1						//����ɨ�����2
#define KSDR0  	0xA2						//new(have change)
#define KSDR1  	0xA3
#define KSDR2  	0xA4
#define MWCR	0xB0						//�ڴ�д��ָ���д��CGRAM/DDRAMǰ����д���ָ��new(have change)
#define MRCR	0xB1						//new
#define TPCR	0xC0
#define TPXR	0xC1
#define TPYR	0xC2
#define TPZR	0xC3
#define PCR     0xD0						//new(have change)
#define PDCR    0xD1						//new

#define PNTR	0xE0						//д��DDRAM�����ݣ����������������ڵ�Ԥ������
#define FNCR	0xF0
#define ISO8859 				((uint8_t)0x80)  //ʹ�ù����ַ�
#define Fill_AW					((uint8_t)0x08)  //�ڴ������ʹ��PNTR���������������ڣ�����ɲ������Զ���0
#define ASCII_Smart_Decode		((uint8_t)0x00)  //����ʵ�ְ���ȫ���ֽ���
#define ASCII_Common_Decode		((uint8_t)0x04)  //������Ϊ������(�޷���ʾ����)
#define ASCII_Block1			((uint8_t)0x00)  //ѡ��ASCII�����
#define ASCII_Block2			((uint8_t)0x01)
#define ASCII_Block3			((uint8_t)0x02)
#define ASCII_Block4			((uint8_t)0x03)

#define FVHT	0xF1						//�ַ��Ŵ�
#define WidthX1					((uint8_t)0x00)  //�ַ���ȷŴ���
#define WidthX2					((uint8_t)0x40)
#define WidthX3					((uint8_t)0x80)
#define WidthX4					((uint8_t)0xC0)
#define HeightX1				((uint8_t)0x00)  //�ַ��߶ȷŴ���
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
���������������
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
�ֶ�ģʽ-·�Ʋ��Ժ���
**********************************************************/
void LED_Check_datainit(void);
void LED_Check_Update(uint8_t LED_Sn);
void LED_Check_interfaceinit(void);
void LED_Check(KEY_TypeDef key);
void LED_Check_main(KEY_TypeDef key);
/**********************************************************
�ֶ�ģʽ-������ǰ��λʱ��
**********************************************************/
void ManualMode_CutDown_init(void);
void ManualMode_CutDown_interfaceinit(void);
void ManualMode_CutDown(KEY_TypeDef key);//void ManualMode_CutDown(void);
void ManualMode_CutDown_main(KEY_TypeDef key);
/**********************************************************
�ֶ�ģʽ-ѭ������ָ����λ
**********************************************************/
void ManualMode_TimePhase_datainit(void);
void ManualMode_TimePhase_Update(uint8_t sel, uint8_t TimeTable_temp, uint8_t PhaseTable_temp);
void ManualMode_TimePhase_interface(void);
void ManualMode_TimePhase(KEY_TypeDef key);
void ManualMode_TimePhase_main(KEY_TypeDef key);
/****************************************************************
RTC���ò˵�
****************************************************************/
void RTC_SetDate_Init(void);
void RTC_SetDate_Update(uint8_t sel, struct tm time);
void RTC_SetDate(KEY_TypeDef key);//void RTC_SetDate(void);
/******************************************************
���浱ǰʱ�䵽�ⲿRTC
******************************************************/
void time_saveto_ds1302(void);
/**********************************************************************
�ָ�����ӿ����õ�����״̬
**********************************************************************/
void reset_ledgpio_map(void);
/******************************************************
��ҹʱ������
******************************************************/
void DayNight_PeriodofTime_Update(uint8_t sel, uint8_t hour, uint8_t min, uint8_t sec);
void DayNight_PeriodofTime_Init(void);
void DayNight_PeriodofTime(KEY_TypeDef key);//void DayNight_PeriodofTime(void);
/****************************************************************
�ƺ�--·��ledӳ���ϵ���ò˵�
****************************************************************/
void LCD_LedGPIO_Update(uint8_t sel, uint8_t* PinNum);
void LCD_LedGPIO_Init(void);
void LCD_LedGPIO(KEY_TypeDef key);
/**********************************************************************
�������ӿ�����
**********************************************************************/
void ledgpio_saveto_e2prom(void);
/**********************************************************************
��˸�������
**********************************************************************/
void flashfunc_update(uint8_t sel, uint8_t crossway, uint16_t flashdata);	//����
void flash_allowed_datainit(void);
void Set_FlashAllowence_Interface(void);
void Set_FlashAllowence(KEY_TypeDef key);
void flash_allowed_main(KEY_TypeDef key);
/**********************************************************************
���糣������
**********************************************************************/
void dayflash_datainit(void);
void dayflash_Interface(void);
void dayflash(KEY_TypeDef key);
void dayflash_main(KEY_TypeDef key);
/**********************************************************************
��䳣������(�����糣�����÷ǳ����ƣ�ֱ������)
**********************************************************************/
void nightflash_datainit(void);
void nightflash_Interface(void);
void nightflash(KEY_TypeDef key);
void nightflash_main(KEY_TypeDef key);
/**********************************************************************
��λ������
**********************************************************************/
void set_phase_datainit(void);
void set_phase_update(uint8_t crossway, uint8_t sel, uint16_t crossway_phase, uint8_t phase_sel, uint8_t period_sel, uint8_t cutdown);
void set_phase_interface(void);
void set_phase(KEY_TypeDef key);
void set_phase_main(KEY_TypeDef key);
/**********************************************************************
ɾ����λ
**********************************************************************/
void del_phase_datainit(void);
void del_phase_update(uint8_t crossway, uint8_t sel, uint16_t crossway_phase, uint8_t phase_sel, uint8_t period_sel, uint8_t cutdown);
void del_phase_interface(void);
void del_phase(KEY_TypeDef key);
void del_phase_main(KEY_TypeDef key);
/**********************************************************************
�����λ
**********************************************************************/
void add_phase_datainit(void);
void add_phase_update(uint8_t sel, uint8_t period_sel, uint8_t phase_sel, uint8_t op);
void add_phase_interface(void);
void add_phase(KEY_TypeDef key);
void add_phase_main(KEY_TypeDef key);
/**********************************************************************
ʱ�α�����
**********************************************************************/
void set_period_datainit(void);
void set_period_update(uint8_t sel, uint8_t period_sel, struct ShortTime time);
void set_period_interface(void);
void set_period(KEY_TypeDef key);
void set_period_main(KEY_TypeDef key);
/**********************************************************************
ɾ��ʱ�α�
**********************************************************************/
void del_period_datainit(void);
void del_period_update(struct ShortTime start_time, struct ShortTime end_time, uint8_t sel, uint8_t period_sel);
void del_period_interface(void);	  
void del_period(KEY_TypeDef key);
void del_period_main(KEY_TypeDef key);
/**********************************************************************
���ʱ�α�
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
