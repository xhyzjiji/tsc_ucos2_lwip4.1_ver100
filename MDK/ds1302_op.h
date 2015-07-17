#ifndef _DS1302_OP_H_
#define _DS1302_OP_H_

#include "stm32f10x.h"
#include "stm32f10x_conf.h"

#define DS1302_sck GPIO_Pin_13
#define DS1302_IO  GPIO_Pin_15
#define DS1302_nrst GPIO_Pin_12
#define DS1302_port GPIOB

#define ds1302_sck_l GPIO_ResetBits(DS1302_port, DS1302_sck)
#define ds1302_sck_h GPIO_SetBits(DS1302_port, DS1302_sck)
#define ds1302_io_l GPIO_ResetBits(DS1302_port, DS1302_IO)
#define ds1302_io_h GPIO_SetBits(DS1302_port, DS1302_IO)
#define ds1302_rst_l GPIO_ResetBits(DS1302_port, DS1302_nrst)
#define ds1302_rst_h GPIO_SetBits(DS1302_port, DS1302_nrst)

//上电时需要确保RST端为低电平，RST置高可以中止传输过程，只能在SCK为低时，才能把RST置高
//控制字命令格式，串行输出由低位开始
//bit7									   bit0
//  1   RAM/nCK   A4   A3   A2   A1   A0   R/nW

//寄存器区以及读写
#define DS1302_WRITE 0xFE
#define DS1302_READ  0x01
#define DS1302_RAM   0x40
#define DS1302_CK    0xBF

//时钟、日历寄存器地址
#define DS1302_ADDR_leftshift 1
#define DS1302_SEC 	0x00	   //CH(1)  10SEC(3)  SEC(4)
#define DS1302_MIN	0x01	   // 0(1)  10MIN(3)  MIN(4)
#define DS1302_HR	0x02	   //12/24(1) 0(1) 1/0orAM/PM(1) HR(1) HR(4)
#define DS1302_DATE 0x03	   //0 0  10DATE(2)  DATE(4)
#define DS1302_MON  0x04	   //0 0 0	10MON(1)  MON(4)
#define DS1302_DAY  0x05	   //0 0 0 0 0  DAY(3)
#define DS1302_YEAR 0x06	   //10YEAR(4)  YEAR(4)
#define DS1302_CTRL 0x07	   //WP 0 0 0 0 0 0 0
#define DS1302_CHG  0x08	   //TCS(4) DS(2) RS(2)
#define DS1302_CHG_1DS 0x04
#define DS1302_CHG_2DS 0x08
#define DS1302_CHG_2K 0x01
#define DS1302_CHG_4K 0x02
#define DS1302_CHG_8K 0x03

void delay_us(uint16_t us);
void DS1302_WriteByte(uint8_t addr, uint8_t dat, uint8_t RAM_CK);
uint8_t DS1302_ReadByte(uint8_t addr, uint8_t RAM_CK);

void DS1302_SetSec(uint8_t sec, FunctionalState stop_flag); //0-59
uint8_t DS1302_ReadSec(void);
void DS1302_SetMin(uint8_t min);
uint8_t DS1302_ReadMin(void);
void DS1302_SetHour(uint8_t hour, FunctionalState ampm); //1-12 or 0-23
uint8_t DS1302_ReadHour(void);
void DS1302_SetDate(uint8_t date); //1-31
uint8_t DS1302_ReadDate(void);
void DS1302_SetMon(uint8_t mon);
uint8_t DS1302_ReadMon(void);
void DS1302_SetYear(uint8_t year);
uint8_t DS1302_ReadYear(void);
void DS1302_WriteProtect(FunctionalState wp);
void DS1302_SetCharger(FunctionalState charge, uint8_t ds, uint8_t rs);

#endif
