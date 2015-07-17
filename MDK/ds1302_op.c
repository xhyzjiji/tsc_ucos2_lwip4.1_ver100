#include "stm32f10x.h"
#include "ds1302_op.h"
#include "stm32f10x_conf.h"
/*
void delay_us(uint16_t us){
	uint16_t time_1us;
	for(;us>0;us--)
		for(time_1us=72; time_1us>0; time_1us--);
}*/

/*模拟时序驱动*/
void DS1302_WriteByte(uint8_t addr, uint8_t dat, uint8_t RAM_CK)
{
	uint8_t command_byte;
	uint8_t i;
	
	if(RAM_CK == DS1302_RAM) command_byte = 0x80|DS1302_RAM|(addr<<DS1302_ADDR_leftshift)&DS1302_WRITE;
	else command_byte = 0x80|(addr<<DS1302_ADDR_leftshift)&DS1302_WRITE&DS1302_CK;	

	ds1302_rst_l;
	delay_us(1);
	ds1302_sck_l;
	delay_us(1);
	ds1302_rst_h;

	for(i=0; i<8; i++){
		if((command_byte>>i)&0x01) ds1302_io_h;
		else ds1302_io_l;
		delay_us(1);
		ds1302_sck_h;
		delay_us(1);
		ds1302_sck_l;
	}

	for(i=0; i<8; i++){
		if((dat>>i)&0x01) ds1302_io_h;
		else ds1302_io_l;
		delay_us(1);
		ds1302_sck_h;
		delay_us(1);
		ds1302_sck_l;
	}

	delay_us(1);
	ds1302_rst_l;
}

uint8_t DS1302_ReadByte(uint8_t addr, uint8_t RAM_CK)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	uint8_t command_byte;
	uint8_t data;
	uint8_t i;
	
	if(RAM_CK == DS1302_RAM) command_byte = 0x80|DS1302_RAM|(addr<<DS1302_ADDR_leftshift)|DS1302_READ;
	else command_byte = 0x80|(addr<<DS1302_ADDR_leftshift)|DS1302_READ;	

	ds1302_rst_l;
	delay_us(1);
	ds1302_sck_l;
	delay_us(1);
	ds1302_rst_h;

	for(i=0; i<8; i++){
		if((command_byte>>i)&0x01) ds1302_io_h;
		else ds1302_io_l;
		delay_us(1);
		ds1302_sck_h;
		delay_us(1);
		ds1302_sck_l;
	}

	GPIO_InitStructure.GPIO_Pin = DS1302_IO;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(DS1302_port, &GPIO_InitStructure);

	//ds1302_sck_h;
	data = 0x00;

	for(i=0; i<8; i++){
		delay_us(1);
		if(GPIO_ReadInputDataBit(DS1302_port, DS1302_IO) == Bit_SET) data = (data>>1)|0x80;
		//data = data|((0x01<<i)&(GPIO_ReadInputDataBit(DS1302_port, DS1302_IO)<<i));
		else data = data>>1;
		delay_us(1);
		ds1302_sck_h;
		delay_us(1);
		ds1302_sck_l;
	}

	delay_us(1);
	ds1302_rst_l;

	GPIO_InitStructure.GPIO_Pin = DS1302_IO;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(DS1302_port, &GPIO_InitStructure);

	return data;
}

void DS1302_SetSec(uint8_t sec, FunctionalState stop_flag) //0-59
{
	uint8_t temp;
	
	if(sec>59) sec = 0;

	temp = ((sec/10)<<4)|(sec%10);

	if(stop_flag == ENABLE) temp |= 0x80; 
	DS1302_WriteByte(DS1302_SEC, temp, DS1302_CK);
}

uint8_t DS1302_ReadSec(void)
{
	uint8_t sec;

	sec = DS1302_ReadByte(DS1302_SEC, DS1302_CK);
	sec = (sec&0x0F)+((sec&0x70)>>3)+((sec&0x70)>>1);

	return sec;
}

void DS1302_SetMin(uint8_t min)	//0-59
{
	if(min==0 || min>59) min = 1;
	DS1302_WriteByte(DS1302_MIN, ((min/10)<<4)|(min%10), DS1302_CK);
}

uint8_t DS1302_ReadMin(void){
	uint8_t min;

	min = DS1302_ReadByte(DS1302_MIN, DS1302_CK);
	min = (min&0x0F)+((min&0x70)>>3)+((min&0x70)>>1);

	return min;
}

void DS1302_SetHour(uint8_t hour, FunctionalState ampm) //1-12 or 0-23
{
	uint8_t temp;
	
	if(ampm == ENABLE){  //12小时制
		if(hour>24) temp = 0xA1;
		else if(hour>12) temp = 0xA0|((hour-12)/10<<4)|((hour-12)%10);
		else if(hour>0) temp = 0x80|(hour/10)|(hour%10);
		else temp = 0x81;
	}
	else{  //24小时制
		if(hour > 23) temp = 0x00;
		else temp = ((hour/10)<<4)|hour%10;
	}

	DS1302_WriteByte(DS1302_HR, temp, DS1302_CK);
}

uint8_t DS1302_ReadHour(void)
{
	uint8_t hour;

	hour = DS1302_ReadByte(DS1302_HR, DS1302_CK);
	if(hour&0x80){  //12小时制
		hour = ((hour&0x10)>>3)+((hour&0x10)>>1)+(hour&0x0F);
	}
	else hour = ((hour&0x30)>>3)+((hour&0x30)>>1)+(hour&0x0F);

	return hour;
}

void DS1302_SetDate(uint8_t date){ //1-31
	if(date==0 || date>31) date = 1;
	DS1302_WriteByte(DS1302_DATE, (date/10)<<4|date%10, DS1302_CK);
}

uint8_t DS1302_ReadDate(void){
	uint8_t date;

	date = DS1302_ReadByte(DS1302_DATE, DS1302_CK);
	date = ((date&0x30)>>3)+((date&0x30)>>1)+(date&0x0F);

	return date;
}

void DS1302_SetMon(uint8_t mon){
	if(mon==0 || mon>12) mon = 1;
	DS1302_WriteByte(DS1302_MON, (mon/10)<<4|mon%10, DS1302_CK);
}

uint8_t DS1302_ReadMon(void){
	uint8_t mon;

	mon = DS1302_ReadByte(DS1302_MON, DS1302_CK);
	mon = ((mon&0x10)>>3)+((mon&0x10)>>1)+(mon&0x0F);

	return mon;
}

void DS1302_SetYear(uint8_t year){	//0-99，DS1302不支持跨世纪闰年检测，2099到2100的时候需要手动判断
	DS1302_WriteByte(DS1302_YEAR, (year/10)<<4|year%10, DS1302_CK);
}

uint8_t DS1302_ReadYear(void){
	uint8_t year;

	year = DS1302_ReadByte(DS1302_YEAR, DS1302_CK);
	year = ((year&0xF0)>>3)+((year&0xF0)>>1)+(year&0x0F);

	return year;
}

void DS1302_WriteProtect(FunctionalState wp){
	if(wp == ENABLE)  DS1302_WriteByte(DS1302_CTRL, 0x80, DS1302_CK);
}

void DS1302_SetCharger(FunctionalState charge, uint8_t ds, uint8_t rs){
	if(charge == ENABLE){
		DS1302_WriteByte(DS1302_CHG, 0xA0|ds|rs, DS1302_CK);
	}
	else DS1302_WriteByte(DS1302_CHG, 0x00, DS1302_CK);
}
