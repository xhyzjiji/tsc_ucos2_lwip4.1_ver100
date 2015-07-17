/*
*********************************************************************************************************
*                                               uC/OS-II
*                                         The Real-Time Kernel
*
*
*                                (c) Copyright 2006, Micrium, Weston, FL
*                                          All Rights Reserved
*
*                                           ARM Cortex-M3 Port
*
* File      : OS_CPU.H
* Version   : V2.89
* By        : Jean J. Labrosse
*             Brian Nagel
*
* For       : ARMv7M Cortex-M3
* Mode      : Thumb2
* Toolchain : RealView Development Suite
*             RealView Microcontroller Development Kit (MDK)
*             ARM Developer Suite (ADS)
*             Keil uVision
*********************************************************************************************************
*/

#ifndef  OS_CPU_H
#define  OS_CPU_H


#ifdef   OS_CPU_GLOBALS
#define  OS_CPU_EXT
#else
#define  OS_CPU_EXT  extern
#endif

#ifndef  OS_CPU_EXCEPT_STK_SIZE
#define  OS_CPU_EXCEPT_STK_SIZE    128u          /* Default exception stack size is 128 OS_STK entries */
#endif

/*
*********************************************************************************************************
*                                              DATA TYPES
*                                         (Compiler Specific)
*********************************************************************************************************
*/

typedef unsigned char  BOOLEAN;
typedef unsigned char  INT8U;                    /* Unsigned  8 bit quantity                           */
typedef signed   char  INT8S;                    /* Signed    8 bit quantity                           */
typedef unsigned short INT16U;                   /* Unsigned 16 bit quantity                           */
typedef signed   short INT16S;                   /* Signed   16 bit quantity                           */
typedef unsigned int   INT32U;                   /* Unsigned 32 bit quantity                           */
typedef signed   int   INT32S;                   /* Signed   32 bit quantity                           */
typedef float          FP32;                     /* Single precision floating point                    */
typedef double         FP64;                     /* Double precision floating point                    */

typedef unsigned int   OS_STK;                   /* Each stack entry is 32-bit wide                    */
typedef unsigned int   OS_CPU_SR;                /* Define size of CPU status register (PSR = 32 bits) */

typedef unsigned char 	UBYTE;
typedef signed char 	BYTE;
typedef signed short 	HALFWORD;
typedef unsigned short 	UHALFWORD;
typedef signed int 		WORD;
typedef unsigned int 	UWORD;


//typedef unsigned char  CPU_BOOLEAN;
//typedef unsigned char  CPU_INT8U;                    /* Unsigned  8 bit quantity                           */
//typedef signed   char  CPU_INT8S;                    /* Signed    8 bit quantity                           */
//typedef unsigned short CPU_INT16U;                   /* Unsigned 16 bit quantity                           */
//typedef signed   short CPU_INT16S;                   /* Signed   16 bit quantity                           */
//typedef unsigned int   CPU_INT32U;                   /* Unsigned 32 bit quantity                           */
//typedef signed   int   CPU_INT32S;                   /* Signed   32 bit quantity                           */
//typedef float          CPU_FP32;                     /* Single precision floating point                    */
//typedef double         CPU_FP64;                     /* Double precision floating point                    */

//typedef signed int  CPU_SIZE_T;

/*
*********************************************************************************************************
*                                              Cortex-M3
*                                      Critical Section Management
*
* Method #1:  Disable/Enable interrupts using simple instructions.  After critical section, interrupts
*             will be enabled even if they were disabled before entering the critical section.
*             NOT IMPLEMENTED
*
* Method #2:  Disable/Enable interrupts by preserving the state of interrupts.  In other words, if
*             interrupts were disabled before entering the critical section, they will be disabled when
*             leaving the critical section.
*             NOT IMPLEMENTED
*
* Method #3:  Disable/Enable interrupts by preserving the state of interrupts.  Generally speaking you
*             would store the state of the interrupt disable flag in the local variable 'cpu_sr' and then
*             disable interrupts.  'cpu_sr' is allocated in all of uC/OS-II's functions that need to
*             disable interrupts.  You would restore the interrupt disable state by copying back 'cpu_sr'
*             into the CPU's status register.
*********************************************************************************************************
*/

#define  OS_CRITICAL_METHOD   3u

#if OS_CRITICAL_METHOD == 3u
#define  OS_ENTER_CRITICAL()  {cpu_sr = OS_CPU_SR_Save();}  //���û����룬method3�������ٽ���ǰ�������ж������֣�Ȼ���ֹ�����жϣ��ٽ���ִ����Ϻ󣬻ָ��ж�������
#define  OS_EXIT_CRITICAL()   {OS_CPU_SR_Restore(cpu_sr);}
#endif

/*
*********************************************************************************************************
*                                        Cortex-M3 Miscellaneous
*********************************************************************************************************
*/

#define  OS_STK_GROWTH        1u                  /* Stack grows from HIGH to LOW memory on ARM        */

#define  OS_TASK_SW()         OSCtxSw()

/*
*********************************************************************************************************
*                                            GLOBAL VARIABLES
*********************************************************************************************************
*/

OS_CPU_EXT  OS_STK   OS_CPU_ExceptStk[OS_CPU_EXCEPT_STK_SIZE];
OS_CPU_EXT  OS_STK  *OS_CPU_ExceptStkBase;

/*
*********************************************************************************************************
*                                              PROTOTYPES
*********************************************************************************************************
*/

#if OS_CRITICAL_METHOD == 3u                      /* See OS_CPU_A.ASM                                  */
OS_CPU_SR  OS_CPU_SR_Save(void);
void       OS_CPU_SR_Restore(OS_CPU_SR cpu_sr);
#endif

//��os_cpu_a.asm��ʵ��
void       OSCtxSw(void);
void       OSIntCtxSw(void);
void       OSStartHighRdy(void);

//void       OS_CPU_PendSVHandler(void);
void	   PendSV_Handler(void);
//void	   OSPendSV(void);

void	   CPU_IntDis(void);
void	   CPU_IntEn(void);
void	   CPU_WaitForInt(void);
void 	   CPU_WaitForExcept(void);

                                                  /* See OS_CPU_C.C                                    */
//void       OS_CPU_SysTickHandler(void);
void	   SysTick_Handler(void);
void       OS_CPU_SysTickInit(INT32U  cnts);
#endif
