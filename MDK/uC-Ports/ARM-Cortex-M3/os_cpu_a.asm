;********************************************************************************************************
;                                               uC/OS-II
;                                         The Real-Time Kernel
;
;                               (c) Copyright 1992-2006, Micrium, Weston, FL
;                                          All Rights Reserved
;
;                                           ARM Cortex-M3 Port
;
; File      : OS_CPU_A.ASM
; Version   : V2.89
; By        : Jean J. Labrosse
;             Brian Nagel
;
; For       : ARMv7M Cortex-M3
; Mode      : Thumb2
; Toolchain : RealView Development Suite
;             RealView Microcontroller Development Kit (MDK)
;             ARM Developer Suite (ADS)
;             Keil uVision
;********************************************************************************************************

;********************************************************************************************************
;                                           PUBLIC FUNCTIONS
;********************************************************************************************************

    EXTERN  OSRunning                                           ; External references
    EXTERN  OSPrioCur
    EXTERN  OSPrioHighRdy
    EXTERN  OSTCBCur
    EXTERN  OSTCBHighRdy
    EXTERN  OSIntExit
    EXTERN  OSTaskSwHook
	EXTERN  OS_CPU_ExceptStkBase


    EXPORT  OS_CPU_SR_Save                                      ; Functions declared in this file
    EXPORT  OS_CPU_SR_Restore
    EXPORT  OSStartHighRdy
    EXPORT  OSCtxSw
    EXPORT  OSIntCtxSw
    ;EXPORT  OS_CPU_PendSVHandler
	EXPORT  PendSV_Handler
	;EXPORT  OSPendSV
	
;可以参考core_cm3.h，均有以下函数的实现	
	EXPORT  CPU_IntDis
	EXPORT  CPU_IntEn
		
	EXPORT  CPU_WaitForInt  	; CPU进入低功耗模式，中断唤醒
    EXPORT  CPU_WaitForExcept  	; CPU进入低功耗模式，异常唤醒
	EXPORT  CPU_RevBits

;********************************************************************************************************
;                                                EQUATES
;********************************************************************************************************

NVIC_INT_CTRL   EQU     0xE000ED04                              ; Interrupt control state register.
NVIC_SYSPRI14   EQU     0xE000ED22                              ; System priority register (priority 14).  //PendSV优先级
NVIC_PENDSV_PRI EQU           0xFF                              ; PendSV priority value (lowest).  //设置PendSV中断优先级为最低
NVIC_PENDSVSET  EQU     0x10000000                              ; Value to trigger PendSV exception. //对中断控制及状态寄存器ICSR(ucos中改名为NVIC_INT_CTRL，地址0xE000ED04)写入0x10000000即触发PendSV异常

NVIC_SYSPRI12   EQU     0xE000ED20  							; 调试监视器的优先级寄存器
NVIC_SYSPRI11   EQU     0xE000ED1F								; SVC优先级
NVIC_SYSPRI15   EQU     0xE000ED23								; SysTick优先级
;********************************************************************************************************
;                                      CODE GENERATION DIRECTIVES
;********************************************************************************************************

    AREA |.text|, CODE, READONLY, ALIGN=2
    THUMB
    REQUIRE8  ;指定当前文件要求堆栈八字节对齐
    PRESERVE8  ;指定当前文件保持堆栈八字节对齐

;********************************************************************************************************
;                                    DISABLE and ENABLE INTERRUPTS
;
; Description : Disable/Enable interrupts.
;
; Prototypes  : void  CPU_IntDis(void);
;               void  CPU_IntEn (void);
;********************************************************************************************************

CPU_IntDis
        CPSID   I
        BX      LR


CPU_IntEn
        CPSIE   I
        BX      LR
		
;********************************************************************************************************
;                                         WAIT FOR INTERRUPT
;
; Description : Enters sleep state, which will be exited when an interrupt is received.
;
; Prototypes  : void  CPU_WaitForInt (void)
;
; Argument(s) : none.
;********************************************************************************************************

CPU_WaitForInt
        WFI                                     ; Wait for interrupt
        BX      LR
		
;********************************************************************************************************
;                                         WAIT FOR EXCEPTION
;
; Description : Enters sleep state, which will be exited when an exception is received.
;
; Prototypes  : void  CPU_WaitForExcept (void)
;
; Argument(s) : none.
;********************************************************************************************************

CPU_WaitForExcept
        WFE                                     ; Wait for exception
        BX      LR
		
;********************************************************************************************************
;                                            CPU_RevBits()
;                                            REVERSE BITS
;
; Description : Reverses the bits in a data value.
;
; Prototypes  : CPU_DATA  CPU_RevBits(CPU_DATA  val);
;
; Argument(s) : val         Data value to reverse bits.
;
; Return(s)   : Value with all bits in 'val' reversed (see Note #1).
;
; Caller(s)   : Application.
;
;               This function is an INTERNAL CPU module function but MAY be called by application function(s).
;
; Note(s)     : (1) The final, reversed data value for 'val' is such that :
;
;                       'val's final bit  0       =  'val's original bit  N
;                       'val's final bit  1       =  'val's original bit (N - 1)
;                       'val's final bit  2       =  'val's original bit (N - 2)
;
;                               ...                           ...
;
;                       'val's final bit (N - 2)  =  'val's original bit  2
;                       'val's final bit (N - 1)  =  'val's original bit  1
;                       'val's final bit  N       =  'val's original bit  0
;********************************************************************************************************

CPU_RevBits
        RBIT    R0, R0                          ; Reverse bits
        BX      LR
		
;********************************************************************************************************
;                                   CRITICAL SECTION METHOD 3 FUNCTIONS
;
; Description: Disable/Enable interrupts by preserving the state of interrupts.  Generally speaking you
;              would store the state of the interrupt disable flag in the local variable 'cpu_sr' and then
;              disable interrupts.  'cpu_sr' is allocated in all of uC/OS-II's functions that need to
;              disable interrupts.  You would restore the interrupt disable state by copying back 'cpu_sr'
;              into the CPU's status register.
;
; Prototypes :     OS_CPU_SR  OS_CPU_SR_Save(void);
;                  void       OS_CPU_SR_Restore(OS_CPU_SR cpu_sr);
;
;
; Note(s)    : 1) These functions are used in general like this:
;
;                 void Task (void *p_arg)
;                 {
;                 #if OS_CRITICAL_METHOD == 3          /* Allocate storage for CPU status register */
;                     OS_CPU_SR  cpu_sr;
;                 #endif
;
;                          :
;                          :
;                     OS_ENTER_CRITICAL();             /* cpu_sr = OS_CPU_SaveSR();                */
;                          :
;                          :
;                     OS_EXIT_CRITICAL();              /* OS_CPU_RestoreSR(cpu_sr);                */
;                          :
;                          :
;                 }
;********************************************************************************************************

OS_CPU_SR_Save
    MRS     R0, PRIMASK                                         ; Set prio int mask to mask all (except faults)
    CPSID   I
    BX      LR

OS_CPU_SR_Restore
    MSR     PRIMASK, R0
    BX      LR

;********************************************************************************************************
;                                          START MULTITASKING
;                                       void OSStartHighRdy(void)
;
; Note(s) : 1) This function triggers a PendSV exception (essentially, causes a context switch) to cause
;              the first task to start.
;
;           2) OSStartHighRdy() MUST:
;              a) Setup PendSV exception priority to lowest;
;              b) Set initial PSP to 0, to tell context switcher this is first run;
;              c) Set the main stack to OS_CPU_ExceptStkBase;
;              d) Set OSRunning to TRUE;
;              e) Trigger PendSV exception;
;              f) Enable interrupts (tasks will run with interrupts enabled).
;********************************************************************************************************

OSStartHighRdy  												; 设置PendSV异常优先级并触发PendSV创建第一个任务
    LDR     R0, =NVIC_SYSPRI14                                  ; Set the PendSV exception priority
    LDR     R1, =NVIC_PENDSV_PRI								; the lowest priority for PendSV exception
    STRB    R1, [R0]

    MOVS    R0, #0                                              ; Set the PSP to 0 for initial context switch call
    MSR     PSP, R0

    LDR     R0, =OS_CPU_ExceptStkBase                           ; Initialize the MSP to the OS_CPU_ExceptStkBase
    LDR     R1, [R0]
    MSR     MSP, R1    

    LDR     R0, =OSRunning                                      ; OSRunning = TRUE
    MOVS    R1, #1
    STRB    R1, [R0]

    LDR     R0, =NVIC_INT_CTRL                                  ; Trigger the PendSV exception (causes context switch)
    LDR     R1, =NVIC_PENDSVSET
    STR     R1, [R0]

    CPSIE   I                                                   ; Enable interrupts at processor level

OSStartHang
    B       OSStartHang                                         ; Should never get here


;********************************************************************************************************
;                               PERFORM A CONTEXT SWITCH (From task level)
;                                           void OSCtxSw(void)
;
; Note(s) : 1) OSCtxSw() is called when OS wants to perform a task context switch.  This function
;              triggers the PendSV exception which is where the real work is done.
;********************************************************************************************************

OSCtxSw
    LDR     R0, =NVIC_INT_CTRL                                  ; Trigger the PendSV exception (causes context switch)
    LDR     R1, =NVIC_PENDSVSET
    STR     R1, [R0]
    BX      LR

;********************************************************************************************************
;                             PERFORM A CONTEXT SWITCH (From interrupt level)
;                                         void OSIntCtxSw(void)
;
; Notes:    1) OSIntCtxSw() is called by OSIntExit() when it determines a context switch is needed as
;              the result of an interrupt.  This function simply triggers a PendSV exception which will
;              be handled when there are no more interrupts active and interrupts are enabled.
;********************************************************************************************************

OSIntCtxSw
    LDR     R0, =NVIC_INT_CTRL                                  ; Trigger the PendSV exception (causes context switch)
    LDR     R1, =NVIC_PENDSVSET
    STR     R1, [R0]
    BX      LR

;********************************************************************************************************
;                                         HANDLE PendSV EXCEPTION
;                                     void OS_CPU_PendSVHandler(void)
;
; Note(s) : 1) PendSV is used to cause a context switch.  This is a recommended method for performing
;              context switches with Cortex-M3.  This is because the Cortex-M3 auto-saves half of the
;              processor context on any exception, and restores same on return from exception.  So only
;              saving of R4-R11 is required and fixing up the stack pointers.  Using the PendSV exception
;              this way means that context saving and restoring is identical whether it is initiated from
;              a thread or occurs due to an interrupt or exception.
;
;           2) Pseudo-code is:
;              a) Get the process SP, if 0 then skip (goto d) the saving part (first context switch);
;              b) Save remaining regs r4-r11 on process stack;
;              c) Save the process SP in its TCB, OSTCBCur->OSTCBStkPtr = SP;
;              d) Call OSTaskSwHook();
;              e) Get current high priority, OSPrioCur = OSPrioHighRdy;
;              f) Get current ready thread TCB, OSTCBCur = OSTCBHighRdy;
;              g) Get new process SP from TCB, SP = OSTCBHighRdy->OSTCBStkPtr;
;              h) Restore R4-R11 from new process stack;
;              i) Perform exception return which will restore remaining context.
;
;           3) On entry into PendSV handler:
;              a) The following have been saved on the process stack (by processor):
;                 xPSR, PC, LR, R12, R0-R3
;              b) Processor mode is switched to Handler mode (from Thread mode)
;              c) Stack is Main stack (switched from Process stack)
;              d) OSTCBCur      points to the OS_TCB of the task to suspend
;                 OSTCBHighRdy  points to the OS_TCB of the task to resume
;
;           4) Since PendSV is set to lowest priority in the system (by OSStartHighRdy() above), we
;              know that it will only be run when no other exception or interrupt is active, and
;              therefore safe to assume that context being switched out was using the process stack (PSP).
;********************************************************************************************************
;OS_CPU_PendSVHandler

PendSV_Handler                                                  ; 异常服务中可能会使用MSP，所以不能手动PUSH压寄存器
    CPSID   I                                                   ; Prevent interruption during context switch   使用双堆栈时，是否不需要关闭中断开关？
    MRS     R0, PSP                                             ; PSP is process stack pointer
    CBZ     R0, OS_CPU_PendSVHandler_nosave                     ; Skip register save the first time

    SUBS    R0, R0, #0x20                                       ; Save remaining regs r4-11 on process stack 8*4bytes=32bytes=0x20bytes
    STM     R0, {R4-R11}

    LDR     R1, =OSTCBCur                                       ; OSTCBCur->OSTCBStkPtr = SP;
    LDR     R1, [R1]
    STR     R0, [R1]                                            ; R0 is SP of process being switched out

                                                                ; At this point, entire context of process has been saved
OS_CPU_PendSVHandler_nosave
    PUSH    {R14}                                               ; Save LR exc_return value
    LDR     R0, =OSTaskSwHook                                   ; OSTaskSwHook(); here do nothing
    BLX     R0
    POP     {R14}

    LDR     R0, =OSPrioCur                                      ; OSPrioCur = OSPrioHighRdy;
    LDR     R1, =OSPrioHighRdy
    LDRB    R2, [R1]
    STRB    R2, [R0]

    LDR     R0, =OSTCBCur                                       ; OSTCBCur  = OSTCBHighRdy;
    LDR     R1, =OSTCBHighRdy
    LDR     R2, [R1]
    STR     R2, [R0]

    LDR     R0, [R2]                                            ; R0 is new process SP; SP = OSTCBHighRdy->OSTCBStkPtr;
    LDM     R0, {R4-R11}                                        ; Restore r4-11 from new process stack
    ADDS    R0, R0, #0x20
    MSR     PSP, R0                                             ; Load PSP with new process SP
    ORR     LR, LR, #0x04                                       ; Ensure exception return uses process stack
    CPSIE   I
    BX      LR                                                  ; Exception return will restore remaining context
		
	END