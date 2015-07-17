//#include "include_h_files.h"
//系统内部设备头文件
#include "stm32f10x.h"
#include "stm32f10x_conf.h"
#include "Sys_Config.h"
//#include "system_conf.h"
#include "stm32f10x_it.h"

//系统数据类型
#include "Traffic_Param.h"
#include "time.h"
#include "Delay.h"
#include "RA8806_CTRL.h"
#include "core_cm3.h"

//系统外部接口使用函数
#include "USART_OP.h"
#include "SPI_OP.h"
#include "TIME_OP.h"
#include "KEY_OP.h"
#include "LED_OP.h"
#include "E2PROM_OP.h"
#include "ENC28J60.h"

//外围器件使用函数
#include "netconf.h"
#include "ethernetif.h"
#include "stdio.h"

//Lwip
#include "lwip/err.h"
#include "lwip/dhcp.h"
#include "etharp.h"

//UCOS
#include "includes.h"

//网络应用层
#include "tcp_server.h"
#include "tcp_client.h"
#include "arch/cc.h"

//全局变量区
#include "Ex_Var.h"
#include "global_val.h"

//系统标志位
uint8_t RTC_FLAG_SECSET;  //秒中断时置位
uint8_t RTC_FLAG_ALRSET;  //下一时段中断置位
uint8_t RTC_FLAG_HalfSecSET;  //半秒可调中断置位
uint8_t Time_FLAG_DayOrNight;  //每一个相位结束后检测当前时间
uint8_t Phase_FLAG_Start;
uint8_t Cycle_FLAG_Start;
uint8_t Init_FLAG_Finished;
uint8_t Manual_FLAG;
uint8_t No_UpdateLED_FLAG;

uint8_t LCD_FLAG_Using;
uint8_t LCD_FLAG_Oping;
uint8_t LCD_FLAG_UpdateIrq;

struct tm Current_Time;  //记录当前时间

struct Schedule_TypeDef my_Schedule;  //记录当前方案

//一些设置不允许马上被更新，如当前亮灯组合等，所以要进行存储，一些可马上反应的设置则可以直接修改到当前读取的方案中，
//这是一下全局变量设定的规则和原因
uint8_t Current_CutDown;
uint8_t Current_PhaseTable;
uint8_t Current_TimeTable;

uint16_t Current_Crossway_LED[6];
uint16_t temp_Current_Crossway_LED[6];
uint16_t temp_Current_Crossway_Yellow[6];
uint16_t Current_Crossway_Flash[6];
uint16_t Current_Crossway_Flash_Allowence[6];
PhaseTable_TypeDef Phase_OneShot;

uint8_t KEY_Buffer[KEYBuffer_Size];
uint8_t KEY_BufferPointer;
uint8_t KEY_BufferBottomOfStack;
uint8_t KEY_Press;

uint8_t Trg;
uint8_t Cont;
KEY_TypeDef CKey;
uint8_t press_cycle;

struct Menu_TypeDef* Current_Menu;  //当前指向的菜单指针
uint8_t CurrentMenu_ItemNumber;	 //当前菜单选项数目

struct Menu_TypeDef MainMenu;
struct MenuItem_TypeDef MainMenu_Items[MainMenu_ItemNum];
struct Menu_TypeDef PhaseMenu;
struct MenuItem_TypeDef PhaseMenu_Items[PhaseMenu_ItemNum];
struct Menu_TypeDef PeriodOfTimeMenu;
struct MenuItem_TypeDef PeriodOfTimeMenu_Items[PeriodOfTimeMenu_ItemNum];
struct Menu_TypeDef FlashMenu;
struct MenuItem_TypeDef FlashMenu_Items[FlashMenu_ItemNum];
struct Menu_TypeDef TimeSetMenu;
struct MenuItem_TypeDef TimeSetMenu_Items[TimeSetMenu_ItemNum];
struct Menu_TypeDef LEDColorMenu;
struct MenuItem_TypeDef LEDColorMenu_Items[LEDColorMenu_ItemNum];
struct Menu_TypeDef RTCSetMenu;
struct MenuItem_TypeDef RTCSetMenu_Items[RTCSetMenu_ItemNum];
struct Menu_TypeDef ManualModeMenu;
struct MenuItem_TypeDef ManualModeMenu_Items[ManualModeMenu_ItemNum];

uint16_t used_if_record[CROSSWAY_MAX];

time_syn time_update_status = DS1302_NEED_SYN;

uint8_t lcd_refresh_time;

/*__ASM uint32_t __R16BIT(uint16_t value)
{
  UXTH r0, r0
  rbit r0, r0
  bx lr
}

uint8_t addfunc(uint8_t x, uint8_t y, uint8_t z, uint8_t m, uint8_t n, uint8_t o)
{
	uint8_t p;
	p = x+y+z+m+n+o;
	return p;
}
*/
INT8U global_err;
OS_EVENT *sem_screen = (OS_EVENT *)0;
OS_EVENT *mbox_key = (OS_EVENT *)0;
void *key_q[5];
OS_EVENT *mbox_traffic_signal = (OS_EVENT *)0;
OS_EVENT *mbox_client_connected = (OS_EVENT *)0;

#define printf_addtime(msg) printf("time:%d--%s",OSTimeGet(),msg)

void task7_system_init(void *arg){
	sem_screen = OSSemCreate(1);
	if(sem_screen == (OS_EVENT *)0)
		printf_addtime("no free sem block\n");
	
	mbox_key = OSQCreate(key_q, 5);
	if(mbox_key == (OS_EVENT *)0)
		printf_addtime("no free mbox block\n");
	
	mbox_traffic_signal = OSMboxCreate((void *)0);
	if(mbox_traffic_signal == (OS_EVENT *)0)
		printf_addtime("no free mbox block\n");
	
	mbox_client_connected = OSMboxCreate((void *)0);
	if(mbox_client_connected == (OS_EVENT *)0)
		printf_addtime("no free mbox block\n");
	
	TIM_Cmd(TIM6, ENABLE);
	
	OSTaskDel(OS_PRIO_SELF);
}

void task1_traffic_control(void *arg){
	INT8U err;
	uint8_t *signal;
	
	for(;;){
		signal = (uint8_t*)OSMboxPend(mbox_traffic_signal, 200, &err);
		if(signal == (void *)0){
			//very serious problem occurs, restart or terminate system
			printf_addtime("system crash!\n");
			OSTaskDel(OS_PRIO_SELF);			
		}
		//printf_addtime("debug info\n");
		if(*signal == 1)
			LED_Sec_Display();
		else{
			LED_HalfSec_Display();
			if(Manual_FLAG==0){
				//update phase or period when current phase's time down to zero
				if(Current_CutDown == 0){
				Phase_FLAG_Start = 1;  //相位开始标志
				//判断昼夜-一相位检测一次
				Time_JudgeDayOrNight();
		
				//检测是否完成周期，否-继续完成这个周期，是-检测时段是否要更改
				if(Current_PhaseTable < (Phase_OneShot.Phase_Number-1)){
					Current_PhaseTable ++;
				}
				else{  //Current_PhaseTable == (my_Schedule.Phase_OneTable[Current_TimeTable].Phase_Number-1)
					Cycle_FLAG_Start = 1;	//周期开始标志
					Current_PhaseTable = 0;
					
					//周期开始，检测是否进入下一个时段
					//日交替时，如何判断？
					Time_JudgeTimeTable();

					Phase_OneShot = my_Schedule.PhaseTable[Current_TimeTable];
				}
		
				Current_CutDown = Phase_OneShot.Last_Time[Current_PhaseTable];
		
				LED_Interpretation(Current_TimeTable, Current_PhaseTable);
				}
				else
					Current_CutDown --;
			}
			//进入手动调试模式，获取时段与相位方法改变
			else{
				if(Current_CutDown == 0){
					Time_JudgeDayOrNight();
					Current_CutDown = my_Schedule.PhaseTable[Current_TimeTable].Last_Time[Current_PhaseTable];
				}
				else{
					Current_CutDown --;
				}
			}
		}
	}
}

void task2_key_control_screen(void *arg){
	INT8U err;
	KEY_TypeDef *key_ptr;
	
	for(;;){
		while(1){
			key_ptr = (KEY_TypeDef *)OSQPend(mbox_key, 100, &err);
			if(key_ptr==(void *)0){
				switch(err){
					case OS_ERR_TIMEOUT:
						printf_addtime("q pend time out\n");
						break;
					case OS_ERR_PEND_ABORT:
					case OS_ERR_EVENT_TYPE:
					case OS_ERR_PEVENT_NULL:
					case OS_ERR_PEND_ISR:
					case OS_ERR_PEND_LOCKED:
					default:
						printf_addtime("q pend error\n");
						break;
				}
			}
			else
				break;
		}
		if(Current_Menu->cur_funptr != NULL){
			while(1){	
				OSSemPend(sem_screen, 100, &err);
				if(err != OS_ERR_NONE){
					switch(err){
						case OS_ERR_TIMEOUT:
							printf_addtime("pend screen sem timeout\n");
							break;  
						case OS_ERR_PEND_ABORT:
						case OS_ERR_EVENT_TYPE:
						case OS_ERR_PEND_ISR:
						case OS_ERR_PEVENT_NULL:
						case OS_ERR_PEND_LOCKED:
						default:
							printf_addtime("cancel pend screen sem\n");
							break;
					}
					OSTimeDly(20);  //delay 20ticks then try again
				}
				else
					break;
			}
			Current_Menu->cur_funptr((KEY_TypeDef)*key_ptr);
			OSSemPost(sem_screen); 
		}
	}
}

void task3_refresh_time_screen(void *arg){
	INT8U err;
	
	for(;;){
		while(1){	
			OSSemPend(sem_screen, 200, &err);
			if(err != OS_ERR_NONE){
				switch(err){
					case OS_ERR_TIMEOUT:
						printf_addtime("pend screen sem timeout\n");
						break;  
					case OS_ERR_PEND_ABORT:
					case OS_ERR_EVENT_TYPE:
					case OS_ERR_PEND_ISR:
					case OS_ERR_PEVENT_NULL:
					case OS_ERR_PEND_LOCKED:
					default:
						printf_addtime("cancel pend screen sem\n");
						break;
				}
				OSTimeDly(40);  //delay 20ticks then try again
			}
			else
				break;
		}
		Current_Time = Time_ConvUnixToCalendar(Time_GetUnixTime());	  //acquire current time from RTC
		LCD_UpdateInfo(); //更新时需要保证时间不被修改
		OSSemPost(sem_screen);
		
		OSTaskSuspend(OS_PRIO_SELF);
	}
}

void task4_network(void *arg){
	for(;;){
		lwip_eth_poll(LocalTime);
	}
}

void task5_client_reconnect(void *arg){
	INT8U err;
	INT8U *signal;
	
	for(;;){
		signal = (INT8U *)OSQPend(mbox_client_connected, 10000, &err);
		if(signal != (void *)0){
			switch(*signal){
				case 1:
					OSTaskDel(OS_PRIO_SELF);
					break;
				case 2:
					break;
				default:
					break;
			}
		}
		else
			//it must decide to create client reconnect task ,according to state of client_pcb
			tcp_client_request(OS_FALSE);
	}
}

#define need_debug 0
OS_STK task7_stk[128];
OS_STK task3_stk[128];
OS_STK task1_stk[128];
OS_STK task2_stk[512];
OS_STK task4_stk[1000];
OS_STK task5_stk[128];

int main(void)
{
	//uint8_t i;
	
	#if need_debug
	uint8_t a,b;
		
	//抓紧时间插入JTAG下载程序
	for(a=255; a>0; a--){
		for(b=255; b>0; b--){
			delay_us(20);
		}
	}
	#endif

	RCC_Config();
	#if need_debug
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);  //上电复位后，IO口默认处于JTAG复用模式，关闭JTAG调试口，请拔除JTAG线
	#endif
	delay_us(1);

	NVIC_Config();

	GPIO_Config();
	GPIO_Initial();

	USART_Config();
	I2C1_Config();
	SPI1_Config();
	TIM_Config(TIM2);
	TIM_Basic_Config(TIM6);
	
	Var_Init();
	LCD_Init();
	LCD_IconInit();
	Menu_Init();
	LCD_InterfaceInit();

	Get_Schedule();
	LED_Init();
	LwIP_Init();
	EXTI_Config();

	tcp_client_request(OS_TRUE);
	tcp_server_init();
	
	printf("BSP Config...\n");
	OS_CPU_SysTickInit(720000);
	printf("Welcome to UCOS-II\n");
	printf("OS Config...\n");
	OSInit();
	OS_MemInit();
	global_err = OSTaskCreate(task7_system_init, (void *)0, &task7_stk[127], 1);
	global_err = OSTaskCreate(task3_refresh_time_screen, (void *)0, &task3_stk[127], 20);
	global_err = OSTaskCreate(task1_traffic_control, (void *)0, &task1_stk[127], 2);
	global_err = OSTaskCreate(task2_key_control_screen, (void *)0, &task2_stk[511], 10);
	global_err = OSTaskCreate(task4_network, (void *)0, &task4_stk[1000-1], 25);
	printf("Welcome to UCOS...\n");
	OSStart();
	
	while(1){
	}	
}
