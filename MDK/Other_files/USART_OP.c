#include "stm32f10x.h"
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
