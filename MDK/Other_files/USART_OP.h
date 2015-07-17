#ifndef _USART_OP_H_
#define _USART_OP_H_

#include "stm32f10x.h"
#include "stdio.h"
#include "time.h"

//使用printf必须使用microLIB
int fputc(int ch, FILE *f);
int GetKey (void);
int fgetc(FILE *f);
//重定义fputc fgetc后可以使用stdio.h中的printf和scanf函数了

#endif
