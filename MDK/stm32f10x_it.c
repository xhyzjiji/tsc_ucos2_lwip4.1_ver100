/**
  ******************************************************************************
  * @file    Project/STM32F10x_StdPeriph_Template/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.4.0
  * @date    10/15/2010
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @copy
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2010 STMicroelectronics</center></h2>
  */ 

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"
#include "stdio.h"
#include "USART_OP.h"
#include "RA8806_CTRL.h"
#include "KEY_OP.h"
#include "Delay.h"
#include "Ex_Var.h"
#include "time_op.h"
#include "LED_OP.h"
#include "includes.h"

#include "stm32f10x.h"
#include "enc28j60.h"
#include "include_global_val.h"

#define SYSTEMTICK_PERIOD_MS  10

/** @addtogroup STM32F10x_StdPeriph_Template
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief   This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
//void PendSV_Handler(void)
//{
//}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */

//void SysTick_Handler(void)
//{
//	lcd_refresh_time ++;
//	LocalTime += SYSTEMTICK_PERIOD_MS;
//	system_time ++;
//}

/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */
void EXTI0_IRQHandler(void)
{
	uint8_t EIR_dat = 0x00;
	
	OSIntEnter();
	EIR_dat = enc28j60Read(EIR);
	
	if(EXTI_GetITStatus(EXTI_Line0) != RESET)				  //判别是否有数据包到达
	{
		if(EIR_dat&0x40){	//检测PKTIF中断请求标志位，接收中断
			ETH_INT=1;
			printf("PKTIF\n");
		}
		else if(EIR_dat&0x01){	//检测RXERIF中断请求标志位，接收错误中断
			ETH_INT=1;
			printf("RXERIF\n");		
		}
		else if(EIR_dat&0x10){
			//链路状态改变中断需要读取PHIR清除中断标志位
			printf("link state changed\n");
			if(enc28j60PhyRead(PHIR)&0x14){
				if(enc28j60PhyRead(PHSTAT2)&0x0400){
					printf("link activate\n");
				}
				else{
					printf("link invalid\n");
					//enc28j60WriteOp(ENC28J60_SOFT_RESET, 0, ENC28J60_SOFT_RESET);
				}
			}
		}
		//ethernetif_input(网口名);								  //设置接收完成标志
		EXTI_ClearITPendingBit(EXTI_Line0);					  //清除中断请求标志
	}
	OSIntExit();
}

//void EXTI1_IRQHandler(void)
//{
//	if(LCD_FLAG_UpdateIrq == 1){
//		LCD_FLAG_UpdateIrq = 0;
//		LCD_UpdateInfo();
//	}
//	EXTI_ClearITPendingBit(EXTI_Line1);
//}

extern OS_EVENT *mbox_traffic_signal;
uint8_t traffic_signal = 0;
void RTC_IRQHandler(void)  //整秒操作
{
	INT8U err;
	
	OSIntEnter();
	if(RTC_GetFlagStatus(RTC_FLAG_SEC) == SET){
		TIM_Cmd(TIM2, ENABLE);
		RTC_ClearITPendingBit(RTC_FLAG_SEC);

		RTC_FLAG_SECSET = 1;  //use in Led_Init() only
		
		OSTaskResume(20);
		if(Init_FLAG_Finished == 1){
			traffic_signal = 1;
			err = OSMboxPost(mbox_traffic_signal, &traffic_signal);
			if(err != OS_ERR_NONE)
				printf("RTC:OSMboxPost failed!\n");
		}
	}
	if(RTC_GetFlagStatus(RTC_FLAG_ALR) == SET){
		RTC_ClearITPendingBit(RTC_FLAG_ALR);
	}
	OSIntExit();
}

void TIM2_IRQHandler(void)	//半秒操作
{
	INT8U err;
	
	OSIntEnter();
	if(TIM_GetFlagStatus(TIM2, TIM_IT_Update) == SET){
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
		
		RTC_FLAG_HalfSecSET = 1;  //use in Led_Init() only

		if(Init_FLAG_Finished == 1){
			traffic_signal = 2;
			err = OSMboxPost(mbox_traffic_signal, &traffic_signal);
			if(err != OS_ERR_NONE)
				printf("TIM2:OSMboxPost failed!\n");
		}
	}
	OSIntExit();
}

const KEY_TypeDef key_mesg[]={
	Nothing, KEY1, KEY2, KEY3, KEY4,
	KEY5, KEY6, KEY1_Cont, KEY2_Cont,
	KEY3_Cont, KEY4_Cont, KEY5_Cont,
	KEY6_Cont, KEY1_2, KEY1_3, KEY1_4,
	KEY2_3, KEY2_4, KEY3_4
};
extern OS_EVENT *mbox_key;
void TIM6_IRQHandler(void)
{
	uint8_t cont_temp;
	uint16_t gpioc_data;
	INT8U err;

	OSIntEnter();
	gpioc_data = GPIO_ReadInputData(GPIOC);
	
	if(TIM_GetFlagStatus(TIM6, TIM_IT_Update) == SET){		
		cont_temp = Cont;  //上一次的按键状态

		//Trg = ((~(GPIO_ReadInputData(GPIOC))>>8)&0x3F) & (~Cont);
		//Cont = (~(GPIO_ReadInputData(GPIOC))>>8)&0x3F;

		Trg = ((~gpioc_data>>8)&0x3F) & (~Cont);
		Cont = (~gpioc_data>>8)&0x3F;

		if(Trg != 0x00){  //短按应改为弹起触发
			switch(Trg){  
				case 0x01:  CKey = KEY2;  break;
				case 0x02:  CKey = KEY1;  break;
				case 0x04:  CKey = KEY3;  break;
				case 0x08:  CKey = KEY4;  break;
				case 0x10:  CKey = KEY5;  break;
				case 0x20:  CKey = KEY6;  break;
				default:  CKey = Nothing;  break;
			}
			press_cycle = 0;
		}
		else{  //Trg == 0x00
			if(Cont != 0x00){
				if(cont_temp == Cont){
					if(press_cycle>10){
						switch(Cont){
							case 0x01:  CKey = KEY2_Cont;  break;
							case 0x02:  CKey = KEY1_Cont;  break;
							case 0x04:  CKey = KEY3_Cont;  break;
							case 0x08:  CKey = KEY4_Cont;  break;
							case 0x10:  CKey = KEY5_Cont;  press_cycle = 0; break;
							case 0x20:  CKey = KEY6_Cont;  press_cycle = 0; break;
							case 0x03:  CKey = KEY1_2;  break;
							case 0x05:  CKey = KEY2_3;  break;
							case 0x09:  CKey = KEY2_4;  break;
							case 0x06:  CKey = KEY1_3;  break;
							case 0x0A:  CKey = KEY1_4;  break;
							case 0x0C:  CKey = KEY3_4;  break;
							default:  CKey = Nothing;  break;
						}
					}
					else{
						press_cycle ++;
						CKey = Nothing;
					}
				}
			}
			else{
				press_cycle = 0;
				CKey = Nothing;
			}
		}

		if(CKey != Nothing){
//			if(KEY_Press < KEYBuffer_Size){	 //KEY_Press为待处理按键数
//				KEY_Buffer[KEY_BufferBottomOfStack] = CKey;	 //RIGHT
//				if(KEY_BufferBottomOfStack == KEYBuffer_Size-1){
//					KEY_BufferBottomOfStack = 0;
//				}
//				else{
//					KEY_BufferBottomOfStack ++;
//				}
//				KEY_Press ++;
//				//if(KEY_Press<KEYBuffer_Size) KEY_Buffer[KEY_BufferBottomOfStack] = Nothing;
//			}
			err = OSQPost(mbox_key, (void *)&key_mesg[CKey]);
			if(err == OS_ERR_Q_FULL)
				printf("mbox key queue is full\n");
		}
		TIM_ClearITPendingBit(TIM6, TIM_IT_Update);
	}
	OSIntExit();
}
/******************* (C) COPYRIGHT 2010 STMicroelectronics *****END OF FILE****/
