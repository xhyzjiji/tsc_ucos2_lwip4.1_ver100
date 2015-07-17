#ifndef __APP_H__
#define __APP_H__

//#define OS_LOWEST_PRIO           63    /* Defines the lowest priority that can be assigned ...         */
#include "ucos_ii.h"

#define START_TASK_PRI0 10
#define TASK1_PRI  5
#define TASK2_PRI  6

//task parameters
#define START_TASK_STK 64
#define TASK1_STK 	   64
#define TASK2_STK      64

void start_task(void *pdata);
void task1(void *pdata);
void task2(void *pdata);

#endif
