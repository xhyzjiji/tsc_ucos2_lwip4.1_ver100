#include "stm32f10x.h"

void delay_100ns(uint16_t tns)
{
	uint8_t ns_100;
	for(; tns>0; tns--)
		for(ns_100=7; ns_100>0; ns_100--);
}

void delay_us(uint16_t tus)
{
	uint8_t us;
	for(; tus>0; tus--)
		for(us=72; us>0; us--);
}

void delay(uint16_t time)
{
	for(; time>0; time--);
}
