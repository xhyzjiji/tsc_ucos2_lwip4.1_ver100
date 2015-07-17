#include "stm32f10x.h"
#include "Sys_Config.h"
#include "USART_OP.h"
#include "TIME_OP.h"
#include "stdio.h"
#include "time.h"
#include "Traffic_Param.h"
#include "EX_Var.h"
#include "at24c64_op.h"

void Get_Schedule(void)
{
	//从e2prom中下载时段相位方案
	//读取并存储整个方案，这里作为测试，代码设置表

	//各灯组中灯位对应灯色，0x0000表示该灯色并没有被使用
	//开机设置？E2PROM表头存储一次即可，修改后一秒后生效
	/*
							//灯组		   //灯色
	my_Schedule.Crossway_LED_Sn[0].LEDColor_Sn[0] = 1;
	my_Schedule.Crossway_LED_Sn[0].LEDColor_Sn[1] = 2;
	my_Schedule.Crossway_LED_Sn[0].LEDColor_Sn[2] = 3;
	my_Schedule.Crossway_LED_Sn[0].LEDColor_Sn[3] = 4;
	my_Schedule.Crossway_LED_Sn[0].LEDColor_Sn[4] = 5;
	my_Schedule.Crossway_LED_Sn[0].LEDColor_Sn[5] = 6;
	my_Schedule.Crossway_LED_Sn[0].LEDColor_Sn[6] = 7;
	my_Schedule.Crossway_LED_Sn[0].LEDColor_Sn[7] = 8;
	my_Schedule.Crossway_LED_Sn[0].LEDColor_Sn[8] = 9;
	my_Schedule.Crossway_LED_Sn[0].LEDColor_Sn[9] = 10;
	my_Schedule.Crossway_LED_Sn[0].LEDColor_Sn[10] = 11;
	my_Schedule.Crossway_LED_Sn[0].LEDColor_Sn[11] = 12;
	my_Schedule.Crossway_LED_Sn[0].LEDColor_Sn[12] = 13;
	my_Schedule.Crossway_LED_Sn[0].LEDColor_Sn[13] = 14;
	my_Schedule.Crossway_LED_Sn[0].LEDColor_Sn[14] = 15;
	my_Schedule.Crossway_LED_Sn[0].LEDColor_Sn[15] = 16;

	my_Schedule.Crossway_LED_Sn[1].LEDColor_Sn[0] = 17;
	my_Schedule.Crossway_LED_Sn[1].LEDColor_Sn[1] = 18;
	my_Schedule.Crossway_LED_Sn[1].LEDColor_Sn[2] = 19;
	my_Schedule.Crossway_LED_Sn[1].LEDColor_Sn[3] = 20;
	my_Schedule.Crossway_LED_Sn[1].LEDColor_Sn[4] = 21;
	my_Schedule.Crossway_LED_Sn[1].LEDColor_Sn[5] = 22;
	my_Schedule.Crossway_LED_Sn[1].LEDColor_Sn[6] = 23;
	my_Schedule.Crossway_LED_Sn[1].LEDColor_Sn[7] = 24;
	my_Schedule.Crossway_LED_Sn[1].LEDColor_Sn[8] = 25;
	my_Schedule.Crossway_LED_Sn[1].LEDColor_Sn[9] = 26;
	my_Schedule.Crossway_LED_Sn[1].LEDColor_Sn[10] = 27;
	my_Schedule.Crossway_LED_Sn[1].LEDColor_Sn[11] = 28;
	my_Schedule.Crossway_LED_Sn[1].LEDColor_Sn[12] = 29;
	my_Schedule.Crossway_LED_Sn[1].LEDColor_Sn[13] = 30;
	my_Schedule.Crossway_LED_Sn[1].LEDColor_Sn[14] = 31;
	my_Schedule.Crossway_LED_Sn[1].LEDColor_Sn[15] = 32;

	my_Schedule.Crossway_LED_Sn[2].LEDColor_Sn[0] = 33;
	my_Schedule.Crossway_LED_Sn[2].LEDColor_Sn[1] = 34;
	my_Schedule.Crossway_LED_Sn[2].LEDColor_Sn[2] = 35;
	my_Schedule.Crossway_LED_Sn[2].LEDColor_Sn[3] = 36;
	my_Schedule.Crossway_LED_Sn[2].LEDColor_Sn[4] = 37;
	my_Schedule.Crossway_LED_Sn[2].LEDColor_Sn[5] = 38;
	my_Schedule.Crossway_LED_Sn[2].LEDColor_Sn[6] = 39;
	my_Schedule.Crossway_LED_Sn[2].LEDColor_Sn[7] = 40;
	my_Schedule.Crossway_LED_Sn[2].LEDColor_Sn[8] = 41;
	my_Schedule.Crossway_LED_Sn[2].LEDColor_Sn[9] = 42;
	my_Schedule.Crossway_LED_Sn[2].LEDColor_Sn[10] = 43;
	my_Schedule.Crossway_LED_Sn[2].LEDColor_Sn[11] = 44;
	my_Schedule.Crossway_LED_Sn[2].LEDColor_Sn[12] = 45;
	my_Schedule.Crossway_LED_Sn[2].LEDColor_Sn[13] = 46;
	my_Schedule.Crossway_LED_Sn[2].LEDColor_Sn[14] = 47;
	my_Schedule.Crossway_LED_Sn[2].LEDColor_Sn[15] = 48;

	my_Schedule.Crossway_LED_Sn[3].LEDColor_Sn[0] = 49;
	my_Schedule.Crossway_LED_Sn[3].LEDColor_Sn[1] = 50;
	my_Schedule.Crossway_LED_Sn[3].LEDColor_Sn[2] = 51;
	my_Schedule.Crossway_LED_Sn[3].LEDColor_Sn[3] = 52;
	my_Schedule.Crossway_LED_Sn[3].LEDColor_Sn[4] = 53;
	my_Schedule.Crossway_LED_Sn[3].LEDColor_Sn[5] = 54;
	my_Schedule.Crossway_LED_Sn[3].LEDColor_Sn[6] = 55;
	my_Schedule.Crossway_LED_Sn[3].LEDColor_Sn[7] = 56;
	my_Schedule.Crossway_LED_Sn[3].LEDColor_Sn[8] = 57;
	my_Schedule.Crossway_LED_Sn[3].LEDColor_Sn[9] = 58;
	my_Schedule.Crossway_LED_Sn[3].LEDColor_Sn[10] = 59;
	my_Schedule.Crossway_LED_Sn[3].LEDColor_Sn[11] = 60;
	my_Schedule.Crossway_LED_Sn[3].LEDColor_Sn[12] = 61;
	my_Schedule.Crossway_LED_Sn[3].LEDColor_Sn[13] = 62;
	my_Schedule.Crossway_LED_Sn[3].LEDColor_Sn[14] = 63;
	my_Schedule.Crossway_LED_Sn[3].LEDColor_Sn[15] = 64;

	my_Schedule.Crossway_LED_Sn[4].LEDColor_Sn[0] = 65;
	my_Schedule.Crossway_LED_Sn[4].LEDColor_Sn[1] = 66;
	my_Schedule.Crossway_LED_Sn[4].LEDColor_Sn[2] = 67;
	my_Schedule.Crossway_LED_Sn[4].LEDColor_Sn[3] = 68;
	my_Schedule.Crossway_LED_Sn[4].LEDColor_Sn[4] = 69;
	my_Schedule.Crossway_LED_Sn[4].LEDColor_Sn[5] = 70;
	my_Schedule.Crossway_LED_Sn[4].LEDColor_Sn[6] = 71;
	my_Schedule.Crossway_LED_Sn[4].LEDColor_Sn[7] = 72;
	my_Schedule.Crossway_LED_Sn[4].LEDColor_Sn[8] = 73;
	my_Schedule.Crossway_LED_Sn[4].LEDColor_Sn[9] = 74;
	my_Schedule.Crossway_LED_Sn[4].LEDColor_Sn[10] = 75;
	my_Schedule.Crossway_LED_Sn[4].LEDColor_Sn[11] = 76;
	my_Schedule.Crossway_LED_Sn[4].LEDColor_Sn[12] = 77;
	my_Schedule.Crossway_LED_Sn[4].LEDColor_Sn[13] = 78;
	my_Schedule.Crossway_LED_Sn[4].LEDColor_Sn[14] = 79;
	my_Schedule.Crossway_LED_Sn[4].LEDColor_Sn[15] = 80;

	my_Schedule.Crossway_LED_Sn[5].LEDColor_Sn[0] = 81;
	my_Schedule.Crossway_LED_Sn[5].LEDColor_Sn[1] = 82;
	my_Schedule.Crossway_LED_Sn[5].LEDColor_Sn[2] = 83;
	my_Schedule.Crossway_LED_Sn[5].LEDColor_Sn[3] = 84;
	my_Schedule.Crossway_LED_Sn[5].LEDColor_Sn[4] = 85;
	my_Schedule.Crossway_LED_Sn[5].LEDColor_Sn[5] = 86;
	my_Schedule.Crossway_LED_Sn[5].LEDColor_Sn[6] = 87;
	my_Schedule.Crossway_LED_Sn[5].LEDColor_Sn[7] = 88;
	my_Schedule.Crossway_LED_Sn[5].LEDColor_Sn[8] = 89;
	my_Schedule.Crossway_LED_Sn[5].LEDColor_Sn[9] = 90;
	my_Schedule.Crossway_LED_Sn[5].LEDColor_Sn[10] = 91;
	my_Schedule.Crossway_LED_Sn[5].LEDColor_Sn[11] = 92;
	my_Schedule.Crossway_LED_Sn[5].LEDColor_Sn[12] = 93;
	my_Schedule.Crossway_LED_Sn[5].LEDColor_Sn[13] = 94;
	my_Schedule.Crossway_LED_Sn[5].LEDColor_Sn[14] = 95;
	my_Schedule.Crossway_LED_Sn[5].LEDColor_Sn[15] = 96;
	*/
	/*
	my_Schedule.Crossway_LED_Sn[0].LEDColor_Sn[0] = 1;
	my_Schedule.Crossway_LED_Sn[0].LEDColor_Sn[1] = 2;
	my_Schedule.Crossway_LED_Sn[0].LEDColor_Sn[2] = 3;
	my_Schedule.Crossway_LED_Sn[0].LEDColor_Sn[3] = 4;
	my_Schedule.Crossway_LED_Sn[0].LEDColor_Sn[4] = 5;
	my_Schedule.Crossway_LED_Sn[0].LEDColor_Sn[5] = 6;
	my_Schedule.Crossway_LED_Sn[0].LEDColor_Sn[6] = 7;
	my_Schedule.Crossway_LED_Sn[0].LEDColor_Sn[7] = 8;
	my_Schedule.Crossway_LED_Sn[0].LEDColor_Sn[8] = 0;
	my_Schedule.Crossway_LED_Sn[0].LEDColor_Sn[9] = 0;
	my_Schedule.Crossway_LED_Sn[0].LEDColor_Sn[10] = 0;
	my_Schedule.Crossway_LED_Sn[0].LEDColor_Sn[11] = 0;
	my_Schedule.Crossway_LED_Sn[0].LEDColor_Sn[12] = 0;
	my_Schedule.Crossway_LED_Sn[0].LEDColor_Sn[13] = 0;
	my_Schedule.Crossway_LED_Sn[0].LEDColor_Sn[14] = 0;
	my_Schedule.Crossway_LED_Sn[0].LEDColor_Sn[15] = 0;

	my_Schedule.Crossway_LED_Sn[1].LEDColor_Sn[0] = 9;
	my_Schedule.Crossway_LED_Sn[1].LEDColor_Sn[1] = 10;
	my_Schedule.Crossway_LED_Sn[1].LEDColor_Sn[2] = 11;
	my_Schedule.Crossway_LED_Sn[1].LEDColor_Sn[3] = 12;
	my_Schedule.Crossway_LED_Sn[1].LEDColor_Sn[4] = 13;
	my_Schedule.Crossway_LED_Sn[1].LEDColor_Sn[5] = 14;
	my_Schedule.Crossway_LED_Sn[1].LEDColor_Sn[6] = 15;
	my_Schedule.Crossway_LED_Sn[1].LEDColor_Sn[7] = 16;
	my_Schedule.Crossway_LED_Sn[1].LEDColor_Sn[8] = 0;
	my_Schedule.Crossway_LED_Sn[1].LEDColor_Sn[9] = 0;
	my_Schedule.Crossway_LED_Sn[1].LEDColor_Sn[10] = 0;
	my_Schedule.Crossway_LED_Sn[1].LEDColor_Sn[11] = 0;
	my_Schedule.Crossway_LED_Sn[1].LEDColor_Sn[12] = 0;
	my_Schedule.Crossway_LED_Sn[1].LEDColor_Sn[13] = 0;
	my_Schedule.Crossway_LED_Sn[1].LEDColor_Sn[14] = 0;
	my_Schedule.Crossway_LED_Sn[1].LEDColor_Sn[15] = 0;

	my_Schedule.Crossway_LED_Sn[2].LEDColor_Sn[0] = 17;
	my_Schedule.Crossway_LED_Sn[2].LEDColor_Sn[1] = 18;
	my_Schedule.Crossway_LED_Sn[2].LEDColor_Sn[2] = 19;
	my_Schedule.Crossway_LED_Sn[2].LEDColor_Sn[3] = 20;
	my_Schedule.Crossway_LED_Sn[2].LEDColor_Sn[4] = 21;
	my_Schedule.Crossway_LED_Sn[2].LEDColor_Sn[5] = 22;
	my_Schedule.Crossway_LED_Sn[2].LEDColor_Sn[6] = 23;
	my_Schedule.Crossway_LED_Sn[2].LEDColor_Sn[7] = 24;
	my_Schedule.Crossway_LED_Sn[2].LEDColor_Sn[8] = 0;
	my_Schedule.Crossway_LED_Sn[2].LEDColor_Sn[9] = 0;
	my_Schedule.Crossway_LED_Sn[2].LEDColor_Sn[10] = 0;
	my_Schedule.Crossway_LED_Sn[2].LEDColor_Sn[11] = 0;
	my_Schedule.Crossway_LED_Sn[2].LEDColor_Sn[12] = 0;
	my_Schedule.Crossway_LED_Sn[2].LEDColor_Sn[13] = 0;
	my_Schedule.Crossway_LED_Sn[2].LEDColor_Sn[14] = 0;
	my_Schedule.Crossway_LED_Sn[2].LEDColor_Sn[15] = 0;

	my_Schedule.Crossway_LED_Sn[3].LEDColor_Sn[0] = 25;
	my_Schedule.Crossway_LED_Sn[3].LEDColor_Sn[1] = 26;
	my_Schedule.Crossway_LED_Sn[3].LEDColor_Sn[2] = 27;
	my_Schedule.Crossway_LED_Sn[3].LEDColor_Sn[3] = 28;
	my_Schedule.Crossway_LED_Sn[3].LEDColor_Sn[4] = 29;
	my_Schedule.Crossway_LED_Sn[3].LEDColor_Sn[5] = 30;
	my_Schedule.Crossway_LED_Sn[3].LEDColor_Sn[6] = 31;
	my_Schedule.Crossway_LED_Sn[3].LEDColor_Sn[7] = 32;
	my_Schedule.Crossway_LED_Sn[3].LEDColor_Sn[8] = 0;
	my_Schedule.Crossway_LED_Sn[3].LEDColor_Sn[9] = 0;
	my_Schedule.Crossway_LED_Sn[3].LEDColor_Sn[10] = 0;
	my_Schedule.Crossway_LED_Sn[3].LEDColor_Sn[11] = 0;
	my_Schedule.Crossway_LED_Sn[3].LEDColor_Sn[12] = 0;
	my_Schedule.Crossway_LED_Sn[3].LEDColor_Sn[13] = 0;
	my_Schedule.Crossway_LED_Sn[3].LEDColor_Sn[14] = 0;
	my_Schedule.Crossway_LED_Sn[3].LEDColor_Sn[15] = 0;

	my_Schedule.Crossway_LED_Sn[4].LEDColor_Sn[0] = 33;
	my_Schedule.Crossway_LED_Sn[4].LEDColor_Sn[1] = 34;
	my_Schedule.Crossway_LED_Sn[4].LEDColor_Sn[2] = 35;
	my_Schedule.Crossway_LED_Sn[4].LEDColor_Sn[3] = 36;
	my_Schedule.Crossway_LED_Sn[4].LEDColor_Sn[4] = 37;
	my_Schedule.Crossway_LED_Sn[4].LEDColor_Sn[5] = 38;
	my_Schedule.Crossway_LED_Sn[4].LEDColor_Sn[6] = 39;
	my_Schedule.Crossway_LED_Sn[4].LEDColor_Sn[7] = 40;
	my_Schedule.Crossway_LED_Sn[4].LEDColor_Sn[8] = 0;
	my_Schedule.Crossway_LED_Sn[4].LEDColor_Sn[9] = 0;
	my_Schedule.Crossway_LED_Sn[4].LEDColor_Sn[10] = 0;
	my_Schedule.Crossway_LED_Sn[4].LEDColor_Sn[11] = 0;
	my_Schedule.Crossway_LED_Sn[4].LEDColor_Sn[12] = 0;
	my_Schedule.Crossway_LED_Sn[4].LEDColor_Sn[13] = 0;
	my_Schedule.Crossway_LED_Sn[4].LEDColor_Sn[14] = 0;
	my_Schedule.Crossway_LED_Sn[4].LEDColor_Sn[15] = 0;

	my_Schedule.Crossway_LED_Sn[5].LEDColor_Sn[0] = 41;
	my_Schedule.Crossway_LED_Sn[5].LEDColor_Sn[1] = 42;
	my_Schedule.Crossway_LED_Sn[5].LEDColor_Sn[2] = 43;
	my_Schedule.Crossway_LED_Sn[5].LEDColor_Sn[3] = 44;
	my_Schedule.Crossway_LED_Sn[5].LEDColor_Sn[4] = 45;
	my_Schedule.Crossway_LED_Sn[5].LEDColor_Sn[5] = 46;
	my_Schedule.Crossway_LED_Sn[5].LEDColor_Sn[6] = 47;
	my_Schedule.Crossway_LED_Sn[5].LEDColor_Sn[7] = 48;
	my_Schedule.Crossway_LED_Sn[5].LEDColor_Sn[8] = 0;
	my_Schedule.Crossway_LED_Sn[5].LEDColor_Sn[9] = 0;
	my_Schedule.Crossway_LED_Sn[5].LEDColor_Sn[10] = 0;
	my_Schedule.Crossway_LED_Sn[5].LEDColor_Sn[11] = 0;
	my_Schedule.Crossway_LED_Sn[5].LEDColor_Sn[12] = 0;
	my_Schedule.Crossway_LED_Sn[5].LEDColor_Sn[13] = 0;
	my_Schedule.Crossway_LED_Sn[5].LEDColor_Sn[14] = 0;
	my_Schedule.Crossway_LED_Sn[5].LEDColor_Sn[15] = 0;
	*/
	uint8_t crossway, i, temp;

	for(crossway=0; crossway<6; crossway++)
		e2prom_read_bytes(0x0000+16*crossway, 16, my_Schedule.Crossway_LED_Sn[crossway].LEDColor_Sn);

	for(crossway=0; crossway<6; crossway++){
		for(i=0; i<16; i++){
			temp = my_Schedule.Crossway_LED_Sn[crossway].LEDColor_Sn[i];
			if(temp>96){
				my_Schedule.Crossway_LED_Sn[crossway].LEDColor_Sn[i] = 0;
				continue;
			}
			else if(temp!=0x00){
				if(ledgpio_map_check_used(temp) == SUCCESS)
					used_if_record[(temp-1)/16] |= 0x0001<<((temp-1)%16);
				else
					my_Schedule.Crossway_LED_Sn[crossway].LEDColor_Sn[i] = 0;
			}
		}
	}//检测重复项并改为0
	/*
	my_Schedule.Crossway_LED_Sn[0].LEDColor_Sn[0] = 1;
	my_Schedule.Crossway_LED_Sn[0].LEDColor_Sn[1] = 5;
	my_Schedule.Crossway_LED_Sn[0].LEDColor_Sn[2] = 9;
	my_Schedule.Crossway_LED_Sn[0].LEDColor_Sn[3] = 0;
	my_Schedule.Crossway_LED_Sn[0].LEDColor_Sn[4] = 0;
	my_Schedule.Crossway_LED_Sn[0].LEDColor_Sn[5] = 0;
	my_Schedule.Crossway_LED_Sn[0].LEDColor_Sn[6] = 2;
	my_Schedule.Crossway_LED_Sn[0].LEDColor_Sn[7] = 6;
	my_Schedule.Crossway_LED_Sn[0].LEDColor_Sn[8] = 10;
	my_Schedule.Crossway_LED_Sn[0].LEDColor_Sn[9] = 0;
	my_Schedule.Crossway_LED_Sn[0].LEDColor_Sn[10] = 4;
	my_Schedule.Crossway_LED_Sn[0].LEDColor_Sn[11] = 7;
	my_Schedule.Crossway_LED_Sn[0].LEDColor_Sn[12] = 12;
	my_Schedule.Crossway_LED_Sn[0].LEDColor_Sn[13] = 0;
	my_Schedule.Crossway_LED_Sn[0].LEDColor_Sn[14] = 0;
	my_Schedule.Crossway_LED_Sn[0].LEDColor_Sn[15] = 0;

	my_Schedule.Crossway_LED_Sn[1].LEDColor_Sn[0] = 13;
	my_Schedule.Crossway_LED_Sn[1].LEDColor_Sn[1] = 17;
	my_Schedule.Crossway_LED_Sn[1].LEDColor_Sn[2] = 21;
	my_Schedule.Crossway_LED_Sn[1].LEDColor_Sn[3] = 0;
	my_Schedule.Crossway_LED_Sn[1].LEDColor_Sn[4] = 0;
	my_Schedule.Crossway_LED_Sn[1].LEDColor_Sn[5] = 0;
	my_Schedule.Crossway_LED_Sn[1].LEDColor_Sn[6] = 14;
	my_Schedule.Crossway_LED_Sn[1].LEDColor_Sn[7] = 18;
	my_Schedule.Crossway_LED_Sn[1].LEDColor_Sn[8] = 22;
	my_Schedule.Crossway_LED_Sn[1].LEDColor_Sn[9] = 0;
	my_Schedule.Crossway_LED_Sn[1].LEDColor_Sn[10] = 15;
	my_Schedule.Crossway_LED_Sn[1].LEDColor_Sn[11] = 19;
	my_Schedule.Crossway_LED_Sn[1].LEDColor_Sn[12] = 23;
	my_Schedule.Crossway_LED_Sn[1].LEDColor_Sn[13] = 0;
	my_Schedule.Crossway_LED_Sn[1].LEDColor_Sn[14] = 0;
	my_Schedule.Crossway_LED_Sn[1].LEDColor_Sn[15] = 0;

	my_Schedule.Crossway_LED_Sn[2].LEDColor_Sn[0] = 25;
	my_Schedule.Crossway_LED_Sn[2].LEDColor_Sn[1] = 29;
	my_Schedule.Crossway_LED_Sn[2].LEDColor_Sn[2] = 33;
	my_Schedule.Crossway_LED_Sn[2].LEDColor_Sn[3] = 0;
	my_Schedule.Crossway_LED_Sn[2].LEDColor_Sn[4] = 0;
	my_Schedule.Crossway_LED_Sn[2].LEDColor_Sn[5] = 0;
	my_Schedule.Crossway_LED_Sn[2].LEDColor_Sn[6] = 26;
	my_Schedule.Crossway_LED_Sn[2].LEDColor_Sn[7] = 30;
	my_Schedule.Crossway_LED_Sn[2].LEDColor_Sn[8] = 34;
	my_Schedule.Crossway_LED_Sn[2].LEDColor_Sn[9] = 0;
	my_Schedule.Crossway_LED_Sn[2].LEDColor_Sn[10] = 27;
	my_Schedule.Crossway_LED_Sn[2].LEDColor_Sn[11] = 31;
	my_Schedule.Crossway_LED_Sn[2].LEDColor_Sn[12] = 36;
	my_Schedule.Crossway_LED_Sn[2].LEDColor_Sn[13] = 0;
	my_Schedule.Crossway_LED_Sn[2].LEDColor_Sn[14] = 0;
	my_Schedule.Crossway_LED_Sn[2].LEDColor_Sn[15] = 0;

	my_Schedule.Crossway_LED_Sn[3].LEDColor_Sn[0] = 37;
	my_Schedule.Crossway_LED_Sn[3].LEDColor_Sn[1] = 41;
	my_Schedule.Crossway_LED_Sn[3].LEDColor_Sn[2] = 45;
	my_Schedule.Crossway_LED_Sn[3].LEDColor_Sn[3] = 0;
	my_Schedule.Crossway_LED_Sn[3].LEDColor_Sn[4] = 0;
	my_Schedule.Crossway_LED_Sn[3].LEDColor_Sn[5] = 0;
	my_Schedule.Crossway_LED_Sn[3].LEDColor_Sn[6] = 38;
	my_Schedule.Crossway_LED_Sn[3].LEDColor_Sn[7] = 42;
	my_Schedule.Crossway_LED_Sn[3].LEDColor_Sn[8] = 46;
	my_Schedule.Crossway_LED_Sn[3].LEDColor_Sn[9] = 0;
	my_Schedule.Crossway_LED_Sn[3].LEDColor_Sn[10] = 39;
	my_Schedule.Crossway_LED_Sn[3].LEDColor_Sn[11] = 43;
	my_Schedule.Crossway_LED_Sn[3].LEDColor_Sn[12] = 47;
	my_Schedule.Crossway_LED_Sn[3].LEDColor_Sn[13] = 0;
	my_Schedule.Crossway_LED_Sn[3].LEDColor_Sn[14] = 0;
	my_Schedule.Crossway_LED_Sn[3].LEDColor_Sn[15] = 0;

	my_Schedule.Crossway_LED_Sn[4].LEDColor_Sn[0] = 0;
	my_Schedule.Crossway_LED_Sn[4].LEDColor_Sn[1] = 0;
	my_Schedule.Crossway_LED_Sn[4].LEDColor_Sn[2] = 0;
	my_Schedule.Crossway_LED_Sn[4].LEDColor_Sn[3] = 0;
	my_Schedule.Crossway_LED_Sn[4].LEDColor_Sn[4] = 0;
	my_Schedule.Crossway_LED_Sn[4].LEDColor_Sn[5] = 0;
	my_Schedule.Crossway_LED_Sn[4].LEDColor_Sn[6] = 0;
	my_Schedule.Crossway_LED_Sn[4].LEDColor_Sn[7] = 0;
	my_Schedule.Crossway_LED_Sn[4].LEDColor_Sn[8] = 0;
	my_Schedule.Crossway_LED_Sn[4].LEDColor_Sn[9] = 0;
	my_Schedule.Crossway_LED_Sn[4].LEDColor_Sn[10] = 0;
	my_Schedule.Crossway_LED_Sn[4].LEDColor_Sn[11] = 0;
	my_Schedule.Crossway_LED_Sn[4].LEDColor_Sn[12] = 0;
	my_Schedule.Crossway_LED_Sn[4].LEDColor_Sn[13] = 0;
	my_Schedule.Crossway_LED_Sn[4].LEDColor_Sn[14] = 0;
	my_Schedule.Crossway_LED_Sn[4].LEDColor_Sn[15] = 0;

	my_Schedule.Crossway_LED_Sn[5].LEDColor_Sn[0] = 0;
	my_Schedule.Crossway_LED_Sn[5].LEDColor_Sn[1] = 0;
	my_Schedule.Crossway_LED_Sn[5].LEDColor_Sn[2] = 0;
	my_Schedule.Crossway_LED_Sn[5].LEDColor_Sn[3] = 0;
	my_Schedule.Crossway_LED_Sn[5].LEDColor_Sn[4] = 0;
	my_Schedule.Crossway_LED_Sn[5].LEDColor_Sn[5] = 0;
	my_Schedule.Crossway_LED_Sn[5].LEDColor_Sn[6] = 0;
	my_Schedule.Crossway_LED_Sn[5].LEDColor_Sn[7] = 0;
	my_Schedule.Crossway_LED_Sn[5].LEDColor_Sn[8] = 0;
	my_Schedule.Crossway_LED_Sn[5].LEDColor_Sn[9] = 0;
	my_Schedule.Crossway_LED_Sn[5].LEDColor_Sn[10] = 0;
	my_Schedule.Crossway_LED_Sn[5].LEDColor_Sn[11] = 0;
	my_Schedule.Crossway_LED_Sn[5].LEDColor_Sn[12] = 0;
	my_Schedule.Crossway_LED_Sn[5].LEDColor_Sn[13] = 0;
	my_Schedule.Crossway_LED_Sn[5].LEDColor_Sn[14] = 0;
	my_Schedule.Crossway_LED_Sn[5].LEDColor_Sn[15] = 0;
	*/

	//早晚分割时间
	//每秒检测一次
	my_Schedule.DayNight_Mode.DayNight_hour = 18;
	my_Schedule.DayNight_Mode.DayNight_min = 30;
	my_Schedule.DayNight_Mode.DayNight_sec = 30;

	//各灯组的可闪灯设置，以及早上夜间的常闪灯位设置
	//修改后下一秒开始生效
	
//////这里存储的是按照指定灯色顺序的
	//LG SG RG BG LY SY RY BY LR SR RR BR W1G W1R W2G W2R
	//这是因为E2PROM里面存储的知识一个16位数据，当灯色灯位发生变化，系统是无法得知，所以需要对这个16位数据进行解析，翻译成相应的灯位

						   //灯组号
	my_Schedule.Crossway_Flash[0].FLASH_Allowence = 0x03C0;
	my_Schedule.Crossway_Flash[0].Day_FLASH_ON = 0x0000;
	my_Schedule.Crossway_Flash[0].Night_FLASH_ON = None;

	my_Schedule.Crossway_Flash[1].FLASH_Allowence = 0x0000;
	my_Schedule.Crossway_Flash[1].Day_FLASH_ON = 0x0000;
	my_Schedule.Crossway_Flash[1].Night_FLASH_ON = None;

	my_Schedule.Crossway_Flash[2].FLASH_Allowence = 0x0000;
	my_Schedule.Crossway_Flash[2].Day_FLASH_ON = 0x0000;
	my_Schedule.Crossway_Flash[2].Night_FLASH_ON = None;

	my_Schedule.Crossway_Flash[3].FLASH_Allowence = 0x0000;
	my_Schedule.Crossway_Flash[3].Day_FLASH_ON = 0x0000;
	my_Schedule.Crossway_Flash[3].Night_FLASH_ON = None;

	my_Schedule.Crossway_Flash[4].FLASH_Allowence = 0x0000;
	my_Schedule.Crossway_Flash[4].Day_FLASH_ON = 0x0000;
	my_Schedule.Crossway_Flash[4].Night_FLASH_ON = None;

	my_Schedule.Crossway_Flash[5].FLASH_Allowence = 0x0000;
	my_Schedule.Crossway_Flash[5].Day_FLASH_ON = 0x0000;
	my_Schedule.Crossway_Flash[5].Night_FLASH_ON = None;

	//时段表的读取记录
	//增减时段数，下个周期生效
	//修改相位灯位，下个相位生效
	my_Schedule.TimeTable_Number = 2;  //两个时段

	//第一个时段有4个相位，时长分别为8、9、10、11s
	my_Schedule.TimeTable[0].hour = 12;	  //这个存时间还是秒数会比较好
	my_Schedule.TimeTable[0].min = 58;
	my_Schedule.TimeTable[0].sec = 0;

	my_Schedule.PhaseTable[0].Phase_Number = 4;
	my_Schedule.PhaseTable[0].Last_Time[0] = 8;
	my_Schedule.PhaseTable[0].Last_Time[1] = 9;
	my_Schedule.PhaseTable[0].Last_Time[2] = 10;
	my_Schedule.PhaseTable[0].Last_Time[3] = 11;
	//每个相位亮灯选择：每个方向亮绿灯还是红灯，不选则不亮
					   //时段		  //灯组 相位
    my_Schedule.PhaseTable[0].Crossway_LED[0][0] = 0x6027;//LeftGreen | StraightGreen | RightRed | BackRed | SideWalk1Red | SideWalk2Red;
	my_Schedule.PhaseTable[0].Crossway_LED[0][1] = 0xB604;//LeftRed | StraightRed | RightGreen | BackRed | SideWalk1Red | SideWalk2Green;
	my_Schedule.PhaseTable[0].Crossway_LED[0][2] = 0x9708;//LeftRed | StraightRed | RightRed | BackGreen | SideWalk1Green | SideWalk2Red;
	my_Schedule.PhaseTable[0].Crossway_LED[0][3] = 0xAF00;//LeftRed | StraightRed | RightRed | BackRed | SideWalk1Red | SideWalk2Red;

	my_Schedule.PhaseTable[0].Crossway_LED[1][0] = 0x6027;
	my_Schedule.PhaseTable[0].Crossway_LED[1][1] = 0xB604;
	my_Schedule.PhaseTable[0].Crossway_LED[1][2] = 0x9708;
	my_Schedule.PhaseTable[0].Crossway_LED[1][3] = 0xAF00;

	my_Schedule.PhaseTable[0].Crossway_LED[2][0] = 0x6027;
	my_Schedule.PhaseTable[0].Crossway_LED[2][1] = 0xB604;
	my_Schedule.PhaseTable[0].Crossway_LED[2][2] = 0x9708;
	my_Schedule.PhaseTable[0].Crossway_LED[2][3] = 0xAF00;

	my_Schedule.PhaseTable[0].Crossway_LED[3][0] = 0x6027;
	my_Schedule.PhaseTable[0].Crossway_LED[3][1] = 0xB604;
	my_Schedule.PhaseTable[0].Crossway_LED[3][2] = 0x9708;
	my_Schedule.PhaseTable[0].Crossway_LED[3][3] = 0xAF00;

	my_Schedule.PhaseTable[0].Crossway_LED[4][0] = 0x6027;
	my_Schedule.PhaseTable[0].Crossway_LED[4][1] = 0xB604;
	my_Schedule.PhaseTable[0].Crossway_LED[4][2] = 0x9708;
	my_Schedule.PhaseTable[0].Crossway_LED[4][3] = 0xAF00;

	my_Schedule.PhaseTable[0].Crossway_LED[5][0] = 0x6027;
	my_Schedule.PhaseTable[0].Crossway_LED[5][1] = 0xB604;
	my_Schedule.PhaseTable[0].Crossway_LED[5][2] = 0x9708;
	my_Schedule.PhaseTable[0].Crossway_LED[5][3] = 0xAF00;

	//第二个时段只有2个相位
	my_Schedule.TimeTable[1].hour = 14;
	my_Schedule.TimeTable[1].min = 0;
	my_Schedule.TimeTable[1].sec = 0;

	my_Schedule.PhaseTable[1].Phase_Number = 2;
	my_Schedule.PhaseTable[1].Last_Time[0] = 20;
	my_Schedule.PhaseTable[1].Last_Time[1] = 12;
	//每个相位亮灯选择：每个方向亮绿灯还是红灯，不选则不亮
	
	my_Schedule.PhaseTable[1].Crossway_LED[0][0] = 0x0406;//LeftGreen | StraightGreen | RightRed | BackRed | SideWalk1Red | SideWalk2Red;
	my_Schedule.PhaseTable[1].Crossway_LED[0][1] = 0x6825;//LeftRed | StraightRed | RightGreen | BackRed | SideWalk1Red | SideWalk2Green;

	my_Schedule.PhaseTable[1].Crossway_LED[1][0] = 0x0C04;
	my_Schedule.PhaseTable[1].Crossway_LED[1][1] = 0x6825;

	my_Schedule.PhaseTable[1].Crossway_LED[2][0] = 0x1006;
	my_Schedule.PhaseTable[1].Crossway_LED[2][1] = 0x6825;

	my_Schedule.PhaseTable[1].Crossway_LED[3][0] = 0x1C00;
	my_Schedule.PhaseTable[1].Crossway_LED[3][1] = 0x6825;

	my_Schedule.PhaseTable[1].Crossway_LED[4][0] = 0x0000;
	my_Schedule.PhaseTable[1].Crossway_LED[4][1] = 0x0000;

	my_Schedule.PhaseTable[1].Crossway_LED[5][0] = 0x0000;
	my_Schedule.PhaseTable[1].Crossway_LED[5][1] = 0x0000;
}
