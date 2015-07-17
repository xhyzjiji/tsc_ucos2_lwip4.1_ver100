#include "stm32f10x.h"
#include "time.h"
#include "stdio.h"

int fputc(int ch, FILE *f)
{
    USART_SendData(USART1, (unsigned char) ch);  //USART1可以换成USART2等
    while (!(USART1->SR & USART_FLAG_TXE));
	//while(!(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == SET))
	return (ch);
}

int GetKey(void)  
{ 
    while (!(USART1->SR & USART_FLAG_RXNE));
	return ((int)(USART1->DR & 0x1FF));
}

int fgetc(FILE *f)
{
  /* Loop until received a char */
  while(!(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == SET))
  {
  }
  
    /* Read a character from the USART and RETURN */
  return (USART_ReceiveData(USART1));
}

void USART_SendWords(USART_TypeDef* USARTx, uint8_t* WordsPoint)
{
	uint8_t* Temp_WordsPoint;
	for(Temp_WordsPoint = WordsPoint; *Temp_WordsPoint != '\0'; Temp_WordsPoint++){
		USART_SendData(USARTx, *Temp_WordsPoint);
		while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
		USART_ClearFlag(USART1, USART_FLAG_TC);
	}
}

void USART_ConvDataToChar()
{
	//暂时用printf代替
}

void USART_DisplayTime(struct tm t)
{
	printf("Current time is:%d-%d-%d-%d-%d-%d\n", t.tm_year, t.tm_mon, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec);
}
