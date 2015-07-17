#ifndef _USART_OP_H_
#define _USART_OP_H_

#include "stm32f10x.h"
#include "stdio.h"
#include "time.h"

//ʹ��printf����ʹ��microLIB
int fputc(int ch, FILE *f);
int GetKey (void);
int fgetc(FILE *f);
//�ض���fputc fgetc�����ʹ��stdio.h�е�printf��scanf������

void USART_SendWords(USART_TypeDef* USARTx, uint8_t* WordsPoint);  //�Լ��õ�С�ɴ��ڷ����ַ�������
void USART_DisplayTime(struct tm t);

#endif
