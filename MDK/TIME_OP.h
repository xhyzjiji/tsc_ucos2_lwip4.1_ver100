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
       int tm_sec;   // 秒 seconds after the minute, 0 to 60
                        (0 - 60 allows for the occasional leap second)
       int tm_min;   // 分 minutes after the hour, 0 to 59
        int tm_hour;  // 时 hours since midnight, 0 to 23
        int tm_mday;  // 日 day of the month, 1 to 31
        int tm_mon;   // 月 months since January, 0 to 11
        int tm_year;  // 年 years since 1900
        int tm_wday;  // 星期 days since Sunday, 0 to 6
        int tm_yday;  // 从元旦起的天数 days since January 1, 0 to 365
        int tm_isdst; // 夏令时？？Daylight Savings Time flag
}
*/

uint32_t Time_ConvCalendarToUnix(struct tm t);  //日历时间(struct tm型数据)转换为Unix时间戳(距离1970年1月0时0分的秒数，uint32_t型数据)
struct tm Time_ConvUnixToCalendar(uint32_t t);  //Unix时间戳(距离1970年1月0时0分的秒数，uint32_t型数据)转换为日历时间(struct tm型数据)
void Time_SetCalendarTime(struct tm t);  //设置日历时间，调用Time_ConvCalendarToUnix函数
void Time_SetUnixTime(uint32_t t);  //设置Unix时间戳
uint32_t Time_GetUnixTime(void);  //获取Unix时间戳
void Get_CurrentTime(void);
void Time_JudgeDayOrNight(void);
void Time_JudgeTimeTable(void);

#endif
