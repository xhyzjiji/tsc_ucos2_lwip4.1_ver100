#include "stm32f10x.h"
#include "Traffic_Param.h"
#include "RA8806_CTRL.h"
#include "Ex_Var.h"
#include "usart_op.h"
#include "time.h"
#include "time_op.h"
#include "stdio.h"
#include "Delay.h"
#include "Sys_Config.h"
#include "LED_OP.h"
#include "core_cm3.h"

//MDK��֧������Ƕ����ָ��
//__ASM uint32_t __REV16BIT(uint16_t value)
//{
//	UXTH r0, r0
//	RBIT r0, r0
//	BX lr
//}

//uint16_t REVLast8BIT(uint16_t date)
//{
//	date = (date&0xFF00)|(__REV16BIT(date)>>24);
//	return date;
//}

//uint16_t REVFore8BIT(uint16_t date)
//{
//	date = (date&0x00FF)|(__REV16BIT(date)>>8);
//	return date;
//}

uint16_t LE_sel(uint8_t x){
/*
	if(x==0x00) LE1; 
	else if(x==0x01) LE2; 
	else if(x==0x02) LE3; 
	else if(x==0x03) LE4; 
	else if(x==0x04) LE5; 
	else LE6;
*/
	switch(x){
		case 0: return LE1;
		case 1: return LE2;
		case 2: return LE3;
		case 3: return LE4;
		case 4: return LE5;
		case 5: return LE6;
		default: return 0x0000;
	} 
}

//extern uint16_t used_if_record[CROSSWAY_MAX];
ErrorStatus ledgpio_map_check_used(uint8_t sn){
	if((used_if_record[(sn-1)/16]&(0x0001<<((sn-1)%16))) == 0x0000) 
		return SUCCESS;
	else 
		return ERROR;
}
/*
void ledgpio_map_record_used(uint8_t sn){
	used_if_record[(sn-1)/16] |= 0x0001<<((sn-1)%16);	 
}*/

void CrossWay_LED_Display(uint16_t CrossWay1_LED, \
				 uint16_t CrossWay2_LED, \
				 uint16_t CrossWay3_LED, \
				 uint16_t CrossWay4_LED, \
				 uint16_t CrossWay5_LED, \
				 uint16_t CrossWay6_LED)
{
	//printf("CrossWay2_LED:%4x\n", CrossWay2_LED);
/*	��ʱ����Ҫ�õ�
	CrossWay1_LED = REVLast8BIT(CrossWay1_LED);
	CrossWay2_LED = REVLast8BIT(CrossWay2_LED);
	CrossWay3_LED = REVLast8BIT(CrossWay3_LED);
	CrossWay4_LED = REVLast8BIT(CrossWay4_LED);
	CrossWay5_LED = REVLast8BIT(CrossWay5_LED);
	CrossWay6_LED = REVLast8BIT(CrossWay6_LED);
*/
	GPIO_SetBits(LED_Port, GPIO_Pin_All);
	GPIO_ResetBits(LED_Port, CrossWay1_LED);
	delay_100ns(10);
	GPIO_SetBits(LE_Port, LE1);
	delay_100ns(10);
	GPIO_ResetBits(LE_Port, LE1);
	delay_100ns(10);

	GPIO_SetBits(LED_Port, GPIO_Pin_All);
	GPIO_ResetBits(LED_Port, CrossWay2_LED);
	delay_100ns(10);
	GPIO_SetBits(LE_Port, LE2);
	delay_100ns(10);
	GPIO_ResetBits(LE_Port, LE2);
	delay_100ns(10);

	GPIO_SetBits(LED_Port, GPIO_Pin_All);
	GPIO_ResetBits(LED_Port, CrossWay3_LED);
	delay_100ns(10);
	GPIO_SetBits(LE_Port, LE3);
	delay_100ns(10);
	GPIO_ResetBits(LE_Port, LE3);
	delay_100ns(10);

	GPIO_SetBits(LED_Port, GPIO_Pin_All);
	GPIO_ResetBits(LED_Port, CrossWay4_LED);
	delay_100ns(10);
	GPIO_SetBits(LE_Port, LE4);
	delay_100ns(10);
	GPIO_ResetBits(LE_Port, LE4);
	delay_100ns(10);

	GPIO_SetBits(LED_Port, GPIO_Pin_All);
	GPIO_ResetBits(LED_Port, CrossWay5_LED);
	delay_100ns(10);
	GPIO_SetBits(LE_Port, LE5);
	delay_100ns(20);
	GPIO_ResetBits(LE_Port, LE5);
	delay_100ns(10);

	GPIO_SetBits(LED_Port, GPIO_Pin_All);
	GPIO_ResetBits(LED_Port, CrossWay6_LED);
	delay_100ns(10);
	GPIO_SetBits(LE_Port, LE6);
	delay_100ns(20);
	GPIO_ResetBits(LE_Port, LE6);
	delay_100ns(10);
}

void All_LED_Die(void)
{
	/*˲��ر����е�
	GPIO_SetBits(LED_Port, GPIO_Pin_All);
	GPIO_SetBits(LE_Port, LE1 | LE2 | LE3 | LE4 | LE5 | LE6);
	delay_100ns(1);
	GPIO_ResetBits(LE_Port, LE1 | LE2 | LE3 | LE4 | LE5 | LE6);
	*/

	//����ر��źŵ�
	CrossWay_LED_Display(0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000);
}

void LED_Interpretation(uint8_t TimeTable, uint8_t PhaseTable)
{
	//////������ɫ����Ӧ��λ
	//my_Schedule.Phase_OneTable[TimeTable].CrossWayX_LED[PhaseTable]�洢������Ҫ���ĵ�ɫ
	//my_Schedule.LEDColor_Mode.CrossWay1_LED.LED_��ɫ���洢���Ǹõ�ɫ��Ӧ�ĵ�λ
	uint8_t i, crossway;
	
	for(i=0; i<6; i++){
		temp_Current_Crossway_Yellow[i] = 0x0000;
		Current_Crossway_LED[i] = 0x0000;
		Current_Crossway_Flash[i] = 0x0000;
		Current_Crossway_Flash_Allowence[i] = 0x0000;
	}
//!!!!!!!!!!!!�����������
	for(crossway=0; crossway<6; crossway++){
//------//for(i=0; i<6; i++){
		for(i=0; i<4; i++){
			if(my_Schedule.Crossway_LED_Sn[crossway].LEDColor_Sn[i] != 0x00){
				temp_Current_Crossway_Yellow[(my_Schedule.Crossway_LED_Sn[crossway].LEDColor_Sn[i]-1)/16] |= ((Phase_OneShot.Crossway_LED[crossway][PhaseTable]&(0x0001<<i))==0x0000)?	0x0000:(0x0001<<(my_Schedule.Crossway_LED_Sn[crossway].LEDColor_Sn[i]-1)%16);
				if(my_Schedule.Crossway_LED_Sn[crossway].LEDColor_Sn[i+6] != 0x00)
					temp_Current_Crossway_Yellow[(my_Schedule.Crossway_LED_Sn[crossway].LEDColor_Sn[i+6]-1)/16] |= ((Phase_OneShot.Crossway_LED[crossway][PhaseTable]&(0x0001<<i))==0x0000)?  0x0000:0x0001<<((my_Schedule.Crossway_LED_Sn[crossway].LEDColor_Sn[i+6]-1)%16);
			}
		}
	}

	for(crossway=0; crossway<6; crossway++){
		for(i=0; i<16; i++){
			if(my_Schedule.Crossway_LED_Sn[crossway].LEDColor_Sn[i] != 0x00){
				Current_Crossway_LED[(my_Schedule.Crossway_LED_Sn[crossway].LEDColor_Sn[i]-1)/16] |= ((Phase_OneShot.Crossway_LED[crossway][PhaseTable]&(0x0001<<i))==0x0000)?	0x0000:(0x0001<<(my_Schedule.Crossway_LED_Sn[crossway].LEDColor_Sn[i]-1)%16);
				Current_Crossway_Flash_Allowence[(my_Schedule.Crossway_LED_Sn[crossway].LEDColor_Sn[i]-1)/16] |= ((my_Schedule.Crossway_Flash[crossway].FLASH_Allowence&(0x0001<<i))==0x0000)?	0x0000:(0x0001<<(my_Schedule.Crossway_LED_Sn[crossway].LEDColor_Sn[i]-1)%16);
				//��ֵǰ�Ȱ�ǰ��״̬����
				if(Time_FLAG_DayOrNight == 0){
					Current_Crossway_Flash[(my_Schedule.Crossway_LED_Sn[crossway].LEDColor_Sn[i]-1)/16] |= ((my_Schedule.Crossway_Flash[crossway].Day_FLASH_ON&(0x0001<<i))==0x0000)?	0x0000:(0x0001<<(my_Schedule.Crossway_LED_Sn[crossway].LEDColor_Sn[i]-1)%16);
				}
				else{
					Current_Crossway_Flash[(my_Schedule.Crossway_LED_Sn[crossway].LEDColor_Sn[i]-1)/16] |= ((my_Schedule.Crossway_Flash[crossway].Night_FLASH_ON&(0x0001<<i))==0x0000)?	0x0000:(0x0001<<(my_Schedule.Crossway_LED_Sn[crossway].LEDColor_Sn[i]-1)%16);
				}
			}
		}
	}
}

void LED_Init(void)
{
	//uint8_t i;
	//printf("LED Init Enter\n");
	//Ϩ�����е�
	//All_LED_Die();
	uint8_t crossway, i;
	//����ָ������ĵ�ɫ���е�λ�����

	for(crossway=0; crossway<6; crossway++){
		for(i=10; i<16; i++){
			if(my_Schedule.Crossway_LED_Sn[crossway].LEDColor_Sn[i] != 0x00){
				Current_Crossway_LED[(my_Schedule.Crossway_LED_Sn[crossway].LEDColor_Sn[i]-1)/16] |= 0x0001<<(my_Schedule.Crossway_LED_Sn[crossway].LEDColor_Sn[i]-1)%16;
			}
		}
	}

	CrossWay_LED_Display(Current_Crossway_LED[0], Current_Crossway_LED[1], Current_Crossway_LED[2], Current_Crossway_LED[3], Current_Crossway_LED[4], Current_Crossway_LED[5]);
	Current_CutDown = 5;

	Get_CurrentTime();
	RTC_Config();

	while(Current_CutDown != 0){
		if(RTC_FLAG_SECSET == 1){
			Current_CutDown --;
			RTC_FLAG_SECSET = 0;
			//printf("CutDown:%d\n", Current_CutDown);
		}
	}

	for(i=0; i<6; i++){
		Current_Crossway_LED[i] = 0x0000;
	}

	for(crossway=0; crossway<6; crossway++){
		for(i=6; i<10; i++){
			if(my_Schedule.Crossway_LED_Sn[crossway].LEDColor_Sn[i] != 0x00){
				Current_Crossway_LED[(my_Schedule.Crossway_LED_Sn[crossway].LEDColor_Sn[i]-1)/16] |= 0x0001<<(my_Schedule.Crossway_LED_Sn[crossway].LEDColor_Sn[i]-1)%16;
			}
		}
	}

	Current_CutDown = 5;
	while(Current_CutDown != 0){
		if(RTC_FLAG_SECSET == 1){
			RTC_FLAG_SECSET = 0;
			
			CrossWay_LED_Display(Current_Crossway_LED[0], Current_Crossway_LED[1], Current_Crossway_LED[2], Current_Crossway_LED[3], Current_Crossway_LED[4], Current_Crossway_LED[5]);
			//printf("CutDown:%d\n", Current_CutDown);
		}
		if(RTC_FLAG_HalfSecSET == 1){
			Current_CutDown --;
			RTC_FLAG_HalfSecSET = 0;
			//CrossWay_LED_Die(Current_CrossWay1_LED, Current_CrossWay2_LED, Current_CrossWay3_LED, Current_CrossWay4_LED, Current_CrossWay5_LED, Current_CrossWay6_LED);
			All_LED_Die();
		}
	}
	
	RTC_ITConfig(RTC_IT_SEC, DISABLE);
	Time_JudgeDayOrNight();
	Time_JudgeTimeTable();

	Phase_OneShot = my_Schedule.PhaseTable[Current_TimeTable];

	Current_CutDown = Phase_OneShot.Last_Time[Current_PhaseTable];
	//Current_CrossWayx_LED�洢��ǰ��λ-��ɫ����
		
	//printf("LED Init Finished\n");
	Init_FLAG_Finished = 1;

	LED_Interpretation(Current_TimeTable, Current_PhaseTable);
	RTC_ITConfig(RTC_IT_SEC, ENABLE);
}

//��ͨ������
void LED_Sec_Display(void)
{
	uint8_t i;
/*
	if(Cycle_FLAG_Start == 1){
//		printf("Cycle Start\n");  //���ڿ�ʼ�����µ�ǰʱ��
//		printf("Current Time Number:%d\n", (Current_TimeTable+1));
		Cycle_FLAG_Start = 0;
	}
	if(Phase_FLAG_Start == 1){
		//ĳһ��λ��ʼʱ���ȹر����е�
		//ΪʲôҪ�ر����еƣ�
		//��Ϊ�̵����������ת���ɻƵƣ����϶���Ϩ��
		//�Ƶ�������������𣬵����ܱ���Ϊ�����ƣ���λ�����󲻻�Ϩ��
		//�����һ��������˸�����õ�ʱ���Ѿ��涨������λ������Ҳ����Ϩ��
		//����Ϩ���ԭ����ST��˾�Ĺ̼����йأ�����GPIO_ResetBits����GPIO_SetBits���ǲ����Ĵ���BRR��BSRR�ĵ�16λ����ֻ��Ҫ���������λ����Ӧλ���в���������λ����Ӱ��
		//��ʵ�����޸ĺ�������Ҫ����Ķ˿�ֵдBSRR��16λ�Ĵ�������1�����˿�ֵȡ����д��BSRR��16λ�Ĵ�������0������
		//Ϊȷ���ɿ��ԣ�����ʹ�ùٷ��̼���
		//printf("Current Phase Number:%d\n", (Current_PhaseTable+1));  //���µ�ǰʱ�δ����ĸ���λ
		Phase_FLAG_Start = 0;  //��ǰ��λ����ʱ��1
		//All_LED_Die();  //����GPIO_Write�󼴿ɸ�����������			
	}
*/
	//����ʣ��ʱ���жϵ�ǰ��ʾ��λ
	//��ͨ�����
	if(Current_CutDown >= 3){
		for(i=0; i<6; i++){
			temp_Current_Crossway_LED[i] = Current_Crossway_LED[i] | Current_Crossway_Flash[i];
		}
	}
	else if(Current_CutDown < 3){
		//�����������Ƿ����̵���//���������̵Ƶ�λ�趨Ϊ�˿ڵ�3λ��0x0007�ǿ����˹��޸ĵ�
		//���̵�����ĳ�����Ӧ�ĻƵƵ�λ����������
		for(i=0; i<6; i++){
			temp_Current_Crossway_LED[i] = (temp_Current_Crossway_Yellow[i] ^ Current_Crossway_LED[i]) | Current_Crossway_Flash[i];
		}
	}
	/*
	CrossWay_LED_Display(~temp_Current_Crossway_LED[0], \
						 ~temp_Current_Crossway_LED[1], \
						 ~temp_Current_Crossway_LED[2], \
						 ~temp_Current_Crossway_LED[3], \
						 ~temp_Current_Crossway_LED[4], \
						 ~temp_Current_Crossway_LED[5]);
	*/
	CrossWay_LED_Display(temp_Current_Crossway_LED[0], \
						 temp_Current_Crossway_LED[1], \
						 temp_Current_Crossway_LED[2], \
						 temp_Current_Crossway_LED[3], \
						 temp_Current_Crossway_LED[4], \
						 temp_Current_Crossway_LED[5]);
}

void LED_HalfSec_Display(void)
{
	//������Ҫ��˸�ĵ�λ
	uint16_t temp_Crossway_LED_Die[6];
	uint8_t i;
	
	//LED��ʾ�����
	//SysTick_FLAG_HalfSecSET = 1;
	if(Current_CutDown > 6){
		for(i=0; i<6; i++){
			temp_Crossway_LED_Die[i] = Current_Crossway_Flash[i];
		}
	}
	else if(Current_CutDown <= 6 && Current_CutDown >= 3){
		for(i=0; i<6; i++){
			temp_Crossway_LED_Die[i] = Current_Crossway_Flash[i] | (Current_Crossway_LED[i] & Current_Crossway_Flash_Allowence[i]);
		}
	}
	else if(Current_CutDown < 3){
		for(i=0; i<6; i++){
			temp_Crossway_LED_Die[i] = Current_Crossway_Flash[i] | ((Current_Crossway_LED[i]^temp_Current_Crossway_Yellow[i]) & Current_Crossway_Flash_Allowence[i]);
		}
	}
	/*
	CrossWay_LED_Display((~temp_Current_Crossway_LED[0]) ^ temp_Crossway_LED_Die[0], \
					 	 (~temp_Current_Crossway_LED[1]) ^ temp_Crossway_LED_Die[1], \
					 	 (~temp_Current_Crossway_LED[2]) ^ temp_Crossway_LED_Die[2], \
					 	 (~temp_Current_Crossway_LED[3]) ^ temp_Crossway_LED_Die[3], \
					 	 (~temp_Current_Crossway_LED[4]) ^ temp_Crossway_LED_Die[4], \
					 	 (~temp_Current_Crossway_LED[5]) ^ temp_Crossway_LED_Die[5]);
	*/
	CrossWay_LED_Display(~((~temp_Current_Crossway_LED[0]) ^ temp_Crossway_LED_Die[0]), \
					 	 ~((~temp_Current_Crossway_LED[1]) ^ temp_Crossway_LED_Die[1]), \
					 	 ~((~temp_Current_Crossway_LED[2]) ^ temp_Crossway_LED_Die[2]), \
					 	 ~((~temp_Current_Crossway_LED[3]) ^ temp_Crossway_LED_Die[3]), \
					 	 ~((~temp_Current_Crossway_LED[4]) ^ temp_Crossway_LED_Die[4]), \
					 	 ~((~temp_Current_Crossway_LED[5]) ^ temp_Crossway_LED_Die[5]));
	/*
	if(Current_CutDown == 0){
		Phase_FLAG_Start = 1;  //��λ��ʼ��־
		//�ж���ҹ-һ��λ���һ��
		Time_JudgeDayOrNight();

		//����Ƿ�������ڣ���-�������������ڣ���-���ʱ���Ƿ�Ҫ����
		if(Current_PhaseTable < (my_Schedule.Phase_OneTable[Current_TimeTable].Phase_Number-1)){
			Current_PhaseTable ++;
		}
		else{  //Current_PhaseTable == (my_Schedule.Phase_OneTable[Current_TimeTable].Phase_Number-1)
			Cycle_FLAG_Start = 1;	//���ڿ�ʼ��־
			Current_PhaseTable = 0;

			//ʹ��RTC_FLAG_ALRSET���ı�ʱ�Σ�����stm32f10x_it.c���޸�
			/ *	
			if(RTC_FLAG_ALRSET == 1 || Time_FLAG_TimeTableChanged == 1){
				RTC_FLAG_ALRSET = 0;
				Time_FLAG_TimeTableChanged = 0;
				/ *
				if(Current_TimeTable == my_Schedule.TimeTable_Number-1){
					Current_TimeTable = 0;
				}
				else{
					Current_TimeTable += 1;
				}
				* /
				Current_TimeTable = TimeTable_Next;
			}			
			* /
			
			//���ڿ�ʼ������Ƿ������һ��ʱ��
			//�ս���ʱ������жϣ�
			Time_JudgeTimeTable();
		}

		Current_CutDown = my_Schedule.Phase_OneTable[Current_TimeTable].Last_Time[Current_PhaseTable];

		LED_Interpretation(Current_TimeTable, Current_PhaseTable);
	}
	else{
		Current_CutDown --;
		//printf("Current_CutDown:%d\n", Current_CutDown);
	}
	*/
}	
