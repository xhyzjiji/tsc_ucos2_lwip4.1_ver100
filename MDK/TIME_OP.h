#ifndef _TIME_OP_H_
#define _TIME_OP_H_

#include "stm32f10x.h"
#include "time.h"
#include "Ex_Var.h"

enum time_synchronization {DS1302_NEED_SYN, DS1302_SYNING, DS1302_SYN_SUL, DS1302_SYN_FAIL, RTC_NEED_SYN, RTC_SYN_SUL, RTC_SYN_FAIL};
typedef enum time_synchronization time_syn; 

// typedef unsigned int time_t;
/*
struct tm {
       int tm_sec;   // �� seconds after the minute, 0 to 60
                        (0 - 60 allows for the occasional leap second)
       int tm_min;   // �� minutes after the hour, 0 to 59
        int tm_hour;  // ʱ hours since midnight, 0 to 23
        int tm_mday;  // �� day of the month, 1 to 31
        int tm_mon;   // �� months since January, 0 to 11
        int tm_year;  // �� years since 1900
        int tm_wday;  // ���� days since Sunday, 0 to 6
        int tm_yday;  // ��Ԫ��������� days since January 1, 0 to 365
        int tm_isdst; // ����ʱ����Daylight Savings Time flag
}
*/

uint32_t Time_ConvCalendarToUnix(struct tm t);  //����ʱ��(struct tm������)ת��ΪUnixʱ���(����1970��1��0ʱ0�ֵ�������uint32_t������)
struct tm Time_ConvUnixToCalendar(uint32_t t);  //Unixʱ���(����1970��1��0ʱ0�ֵ�������uint32_t������)ת��Ϊ����ʱ��(struct tm������)
void Time_SetCalendarTime(struct tm t);  //��������ʱ�䣬����Time_ConvCalendarToUnix����
void Time_SetUnixTime(uint32_t t);  //����Unixʱ���
uint32_t Time_GetUnixTime(void);  //��ȡUnixʱ���
void Get_CurrentTime(void);
void Time_JudgeDayOrNight(void);
void Time_JudgeTimeTable(void);

#endif
