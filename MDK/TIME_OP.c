#include "stm32f10x.h"
#include "time.h"
#include "ds1302_op.h"
#include "EX_Var.h"
#include "Traffic_Param.h"

uint32_t Time_ConvCalendarToUnix(struct tm Calendar_t)
{
	Calendar_t.tm_year -= 1900;
	return mktime(&Calendar_t);
}

struct tm Time_ConvUnixToCalendar(uint32_t Unix_t)
{
	struct tm* Calendar_t;
	Calendar_t = localtime(&Unix_t);
	Calendar_t->tm_year += 1900;
	return (*Calendar_t);
}

void Time_SetUnixTime(uint32_t Unix_t)
{
	RTC_WaitForLastTask();
	RTC_SetCounter(Unix_t);
	RTC_WaitForLastTask();
	return;
}

void Time_SetCalendarTime(struct tm Calendar_t)
{
	Time_SetUnixTime(Time_ConvCalendarToUnix(Calendar_t));
}

uint32_t Time_GetUnixTime(void)
{
	return (uint32_t)RTC_GetCounter();
}

//���ⲿRTC-DS1302��ȡ��ǰʱ��
void Get_CurrentTime(void)
{
	Current_Time.tm_year = DS1302_ReadYear()+2000;//2012;
	Current_Time.tm_mon = DS1302_ReadMon()-1;//8-1;
	Current_Time.tm_mday = DS1302_ReadDate();//11;
	Current_Time.tm_hour = DS1302_ReadHour();//17;
	Current_Time.tm_min = DS1302_ReadMin();//44;
	Current_Time.tm_sec = DS1302_ReadSec();//30;
}

void Time_JudgeDayOrNight(void)
{
	if(Current_Time.tm_hour > my_Schedule.DayNight_Mode.DayNight_hour){
		Time_FLAG_DayOrNight = 1;
	}
	else if(Current_Time.tm_hour < my_Schedule.DayNight_Mode.DayNight_hour){
		Time_FLAG_DayOrNight = 0;
	}
	else{
		if(Current_Time.tm_min > my_Schedule.DayNight_Mode.DayNight_min){
			Time_FLAG_DayOrNight = 1;
			}
		else if(Current_Time.tm_min < my_Schedule.DayNight_Mode.DayNight_min){
			Time_FLAG_DayOrNight = 0;
			}
		else{
			if(Current_Time.tm_sec > my_Schedule.DayNight_Mode.DayNight_sec){
				Time_FLAG_DayOrNight = 1;
			}
			else{
				Time_FLAG_DayOrNight = 0;  //0�����ϣ�1������
			}
		}
	}
}

void Time_JudgeTimeTable(void)
{
	uint8_t i;
	
	for(i=my_Schedule.TimeTable_Number, Current_TimeTable = my_Schedule.TimeTable_Number-1; i>0; i--){  //����ʱ��ΰ�˳��洢�����
		//ʱ�������洢ʱ�������ȶԶ����ķ����������������������ʱ�����бȽ�
		//�����д��ʱ��ȽϺ������������رȽ����
		if(Current_Time.tm_hour < my_Schedule.TimeTable[i-1].hour)	 continue;
		else if(Current_Time.tm_hour > my_Schedule.TimeTable[i-1].hour){
		    Current_TimeTable = i-1;
			break;
		}
		else{
			if(Current_Time.tm_min < my_Schedule.TimeTable[i-1].min)    continue;
			else if(Current_Time.tm_min > my_Schedule.TimeTable[i-1].min){
			    Current_TimeTable = i-1;
				break;
			}
			else{
				if(Current_Time.tm_sec < my_Schedule.TimeTable[i-1].sec)    continue;
				else{
					Current_TimeTable = i-1;
					break;  //�����ʱ��αȵ�ǰʱ�����ʹ�ø�ʱ��Σ������ǰʱ�������ʱ��ζ����С����ʹ�����һ��ʱ���
					//���������������ѭ����
				}
			} 
		}
	}
}
