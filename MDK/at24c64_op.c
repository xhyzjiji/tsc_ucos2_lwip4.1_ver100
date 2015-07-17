#include "stm32f10x.h"
#include "Delay.h"
#include "at24c64_op.h"

#define E2PROM_PageSize 32
#define E2PROM_ADDR	 	0xA0
#define E2PROM_R 		0x01
#define E2PROM_W 		0xFE
#define E2PROM_WAITTIME	0x20
/*
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
*/
/*******************************************************************************
* Function Name  : I2C_AcknowledgePolling
* Description    : 等待获取I2C总线控制权 判断忙状态
* Input          : - I2Cx:I2C寄存器基址
*                  - I2C_Addr:从器件地址
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
/*Once the internally-timed write cycle has started and the EEPROM inputs are disabled, acknowledge polling can be initiated.
 This involves sending a start condition followed by the device address word.
 The read/write bit is representative of the operation desired.
 Only if the internal write cycle has completed will the EEPROM respond with a zero, allowing the read or write sequence to continue.
 当e2prom内部写周期开始时，输入被禁止，这时候，使用“查询应答”（即发送一个起始条件，然后紧跟设备地址和读写操作位），当内部写周期结束，e2prom将回应一个低电平来表示读写时序的继续。 
*/
#ifdef i2c_simulation

//IIC模拟时序驱动
void e2prom_start_condition(void){  //通信起始时，scl信号线为低，sda信号线为高，起始条件产生后均为低
	//起始条件
	reset_scl; set_sda;
	delay_100ns(2);

	set_scl;
	delay_100ns(6);  //tsu.sta   min:0.6us in 1.8Vcc, min:0.25us in 5.0Vcc
	reset_sda;
	delay_100ns(6);  //thd.sta   min:0.6us in 1.8Vcc, min:0.25us in 5.0Vcc
	reset_scl;
	delay_100ns(13); //tlow      min:1.3us in 1.8Vcc, min:0.4us in 5.0Vcc
}

void e2prom_stop_condition(void){	//结束前，scl sda均为低，结束条件产生后scl信号线为低，sda信号线为高
	reset_scl_sda; delay_100ns(2);
	
	set_scl;
	delay_100ns(6);	 //tsu.sto   min:0.6us in 1.8Vcc, min:0.25us in 5.0Vcc
	set_sda;
	delay_100ns(13); //tbuf      min:1.3us in 1.8Vcc, min:0.5us in 5.0Vcc
	reset_scl;
	delay_100ns(6);

	//delay_us(50);
}

void e2prom_reset(void){  //数据传输完成后，scl为低，sda为低
	uint8_t i;
	e2prom_start_condition();
	//scl sda = 0	

	set_sda;
	delay_100ns(2);  //tsu.dat   min:100ns
	for(i=0; i<9; i++){
		set_scl;
		delay_100ns(6);  //thigh   min:0.6us in 1.8Vcc, min:0.4us in 5.0Vcc
		reset_scl;
		delay_100ns(13);
	}

	e2prom_start_condition();
	e2prom_stop_condition();
}
/*
ErrorStatus e2prom_check_ack(void){
	ErrorStatus state;
	GPIO_InitTypeDef GPIO_InitStructure;
	uint16_t i; //统计等待次数

	state = ERROR;
	i = 0;

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7; //PB6--SCL  PB7--SDA 释放SDA信号线
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	for(i=0; i<1000; i++){
		if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_7) == (uint8_t)Bit_RESET){
			state = SUCCESS;
			break;
		}
		else continue;
	}

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	return state;
}
*/
void e2prom_check_busy(void){
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7; //PB6--SCL  PB7--SDA 释放SDA信号线
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	while(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_7) == (uint8_t)Bit_RESET);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

void e2prom_waitfor_ack(void){
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7; //PB6--SCL  PB7--SDA 释放SDA信号线
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	set_scl;
	delay_100ns(2);
	while(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_7) == (uint8_t)Bit_SET);
	delay_100ns(10);
	reset_scl;

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

void e2prom_send_data(uint8_t dat){	 //进入此函数时，scl确定为低
	uint8_t i;

	reset_scl;
	for(i=0; i<8; i++){
		//reset_scl;
		if(dat&0x80) set_sda;
		else reset_sda;
		delay_100ns(9);
		set_scl;
		delay_100ns(6);
		reset_scl;
		delay_100ns(13);
		dat <<= 1;
	}
}

void acknowledge_poll(void){
	GPIO_InitTypeDef GPIO_InitStructure;
	uint8_t i;

	do{
		//delay_100ns(100);

		e2prom_start_condition();
		delay_100ns(5);
		e2prom_send_data(E2PROM_ADDR&0xFE);

		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7; //PB6--SCL  PB7--SDA 释放SDA信号线
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
		GPIO_Init(GPIOB, &GPIO_InitStructure);
	
		delay_100ns(5);
		set_scl;
		delay_100ns(8);
		if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_7) == (uint8_t)Bit_RESET) i=0;
		else continue;
		delay_100ns(8);//(10);
		reset_scl;
		delay_100ns(13);
		//if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_7) == (uint8_t)Bit_RESET) i=0;
		//else i = 0xFF;
		while(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_7) == (uint8_t)Bit_RESET);
	
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
		GPIO_Init(GPIOB, &GPIO_InitStructure);
	}while(i);
}

void e2prom_write_byte(uint16_t add, uint8_t data){
	//acknowledge_poll();
	
	e2prom_check_busy();

	e2prom_start_condition();

	e2prom_send_data(E2PROM_ADDR&0xFE);  //写入器件地址
	e2prom_waitfor_ack();
	e2prom_send_data(0xFF&(add>>8));
	e2prom_waitfor_ack();
	e2prom_send_data(0xFF&add);
	e2prom_waitfor_ack();
	e2prom_send_data(data);
	e2prom_waitfor_ack();

	e2prom_stop_condition();

	delay_us(50);

	//acknowledge_poll();
}

uint8_t e2prom_read_currentbyte_simulate(void){
	GPIO_InitTypeDef GPIO_InitStructure;
	uint8_t data;
	uint8_t i;

	data = 0;
	
	e2prom_check_busy();

	e2prom_start_condition();

	e2prom_send_data((E2PROM_ADDR&0xFE)|0x01);  //写入器件地址
	e2prom_waitfor_ack();

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7; //PB6--SCL  PB7--SDA 释放SDA信号线
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	for(i=0; i<8; i++){
		set_scl;
		delay_100ns(5);
		if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_7) == (uint8_t)Bit_SET) data = (data<<1) | 0x01;
		else data = data<<1;
	}
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	e2prom_stop_condition();

	//acknowledge_poll();
	delay_us(50);

	return data;
}

uint8_t e2prom_read_byte(uint16_t add){
	GPIO_InitTypeDef GPIO_InitStructure;
	uint8_t data;
	uint8_t i;

	data = 0;
	
	//acknowledge_poll();

	e2prom_check_busy();

	e2prom_start_condition();

	e2prom_send_data((E2PROM_ADDR&0xFE));  //写入器件地址
	e2prom_waitfor_ack();
	e2prom_send_data(0xFF&(add>>8));
	e2prom_waitfor_ack();
	e2prom_send_data(0xFF&add);
	e2prom_waitfor_ack();

	e2prom_start_condition();

	e2prom_send_data((E2PROM_ADDR&0xFE)|0x01);  //写入器件地址
	e2prom_waitfor_ack();

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7; //PB6--SCL  PB7--SDA 释放SDA信号线
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	for(i=0; i<8; i++){
		set_scl;
		delay_100ns(5);
		if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_7) == (uint8_t)Bit_SET) data = (data<<1) | 0x01;
		else data = data<<1;
		delay_100ns(5);
		reset_scl;
		delay_100ns(13);
	}
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	e2prom_stop_condition();

	//acknowledge_poll();
	delay_us(50);

	return data;
}

//void e2prom_write_page(uint16_t start_addr, uint16_t bytenum, uint8_t* dat){
void e2prom_write_bytes(uint16_t start_addr, uint16_t bytenum, uint8_t* dat){
	uint16_t addr_temp = start_addr;
	uint8_t i;

	while(bytenum>0){
		if(bytenum>=E2PROM_PageSize){
			bytenum -= E2PROM_PageSize;

			e2prom_check_busy();

			e2prom_start_condition();

			e2prom_send_data(E2PROM_ADDR&0xFE);  //写入器件地址
			e2prom_waitfor_ack();
			e2prom_send_data(0xFF&(addr_temp>>8));
			e2prom_waitfor_ack();
			e2prom_send_data(0xFF&addr_temp);
			e2prom_waitfor_ack();

			for(i=0; i<E2PROM_PageSize; i++){
				e2prom_send_data(*dat++);
				e2prom_waitfor_ack();
			}
			addr_temp += E2PROM_PageSize;

			e2prom_stop_condition();

			//I2C_AcknowledgePolling(E2PROM_ADDR);  //等待写周期完成
			delay_us(50); //acknowledge_poll();
		}
		else{
			e2prom_check_busy();			

			e2prom_start_condition();

			e2prom_send_data(E2PROM_ADDR&0xFE);  //写入器件地址
			e2prom_waitfor_ack();
			e2prom_send_data(0xFF&(addr_temp>>8));
			e2prom_waitfor_ack();
			e2prom_send_data(0xFF&addr_temp);
			e2prom_waitfor_ack();

			for(i=0; i<bytenum; i++){
				e2prom_send_data(*dat++);
				e2prom_waitfor_ack();
			}
			bytenum = 0;

			e2prom_stop_condition();

			delay_us(50);  //acknowledge_poll(); 
		}
	}
}

//void e2prom_read_page(uint16_t start_addr, uint16_t bytenum, uint8_t* buf){
void e2prom_read_bytes(uint16_t start_addr, uint16_t bytenum, uint8_t* buf){
	uint16_t i,k;
	uint8_t dat;
	GPIO_InitTypeDef GPIO_InitStructure;

	e2prom_check_busy();
	
	e2prom_start_condition();

	e2prom_send_data(E2PROM_ADDR&0xFE);  //写入器件地址
	e2prom_waitfor_ack();
	e2prom_send_data(0xFF&(start_addr>>8));
	e2prom_waitfor_ack();
	e2prom_send_data(0xFF&start_addr);
	e2prom_waitfor_ack();

	e2prom_start_condition();

	e2prom_send_data((E2PROM_ADDR&0xFE)|0x01);  //写入器件地址
	e2prom_waitfor_ack();

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7; //PB6--SCL  PB7--SDA 释放SDA信号线
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	for(i=bytenum-1,dat=0x00; i>0; i--){
		for(k=0; k<8; k++){
			set_scl;
			delay_100ns(2);
			if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_7) == (uint8_t)Bit_SET) dat = (dat<<1) | 0x01;
			else dat = dat<<1;
			delay_100ns(5);
			reset_scl;
			delay_100ns(13);		
		}
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
		GPIO_Init(GPIOB, &GPIO_InitStructure);
		reset_sda;
		delay_100ns(5);

		set_scl;
		delay_100ns(6);
		reset_scl;
		delay_100ns(13);
		set_sda;

		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
		GPIO_Init(GPIOB, &GPIO_InitStructure);

		*buf = dat;
		dat = 0x00;
		buf++;
	}

	for(k=0; k<8; k++){
		set_scl;
		delay_100ns(5);
		if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_7) == (uint8_t)Bit_SET) *buf = (*buf<<1) | 0x01;
		else *buf = *buf<<1;
		delay_100ns(5);
		reset_scl;
		delay_100ns(13);
	}

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	e2prom_stop_condition();

	delay_us(50);
	//acknowledge_poll();
}

#else

//IIC的STM32硬件接口驱动
//I2C协议通信小常识：ACK：在第9个SCL周期，且SCL为高时，由接收设备将SDA拉低作为ACK应答信号，表示操作成功，而NACK则保持SDA为高，或称不做出应答行为。
static void I2C_AcknowledgePolling(uint8_t I2C_Addr)
{
  vu16 SR1_Tmp;
  do
  {   
    I2C_GenerateSTART(I2C1, ENABLE); /*起始位*/
    /*读SR1*/
    SR1_Tmp = I2C_ReadRegister(I2C1, I2C_Register_SR1);
    /*器件地址(写)*/
//#ifdef AT24C01A

	I2C_Send7bitAddress(I2C1, I2C_Addr, I2C_Direction_Transmitter);
//#else

//	I2C_Send7bitAddress(I2Cx, 0, I2C_Direction_Transmitter);
//#endif

  }while(!(I2C_ReadRegister(I2C1, I2C_Register_SR1) & 0x0002));	 //检查是否返回应答信号
  
  I2C_ClearFlag(I2C1, I2C_FLAG_AF);
    
  I2C_GenerateSTOP(I2C1, ENABLE);  /*停止位*/  
}

void e2prom_write_byte(uint16_t add, uint8_t data)
{
	//I2C_AcknowledgePolling(E2PROM_ADDR);
	
	//传输开始前检查总线是否空闲，即检查SR2上的BUSY位
	/*while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY) == SET);*/
	
	/*
	//产生起始条件
	I2C_GenerateSTART(I2C1, ENABLE);

	//检查BUSY MSL SB位，判断是否产生EV5事件，即EV5 ：SB=1，读SR1 然后将地址写入DR寄存器将清除该事件
	//SB:当发送出起始条件时该位被置’1’;软件读取SR1 寄存器后，写数据寄存器的操作将清除该位，或当PE=0时，硬件清除该位(I2C_Cmd用于使能失能I2C设备，即修改PE位) 
	//BUSY:在检测到SDA或SCl为低电平时，硬件将该位置’1’;当检测到一个停止条件时，硬件将该位清除
	//MSL:当接口处于主模式(SB=1)时，硬件将该位置位;当总线上检测到一个停止条件、仲裁丢失(ARLO=1时)、或当PE=0时，硬件清除该位
	while(I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT) == ERROR);  //读取SR1 SR2状态寄存器判断BUSY MSL SB位是否置位，BUSY MSL位在检测到停止条件时，硬件清零

	I2C_Send7bitAddress(I2C1, E2PROM_ADDR, I2C_Direction_Transmitter);	//向DR写入器件地址和传输方向，由于读取SR1和写DR，SB被硬件清零
	*/
	//检查BUSY, MSL, ADDR, TXE and TRA标志位
	//ADDR:当stm32处于I2C主设备位置，ADDR代表地址已被发送(主模式):7位地址模式时，当收到地址的ACK后该位被置’1’，在收到NACK后，ADDR位不会被置位，在软件读取SR1寄存器后，对SR2寄存器的读操作将清除该位，或当PE=0时，由硬件清除该位 
	//TXE:(DR发送器为空，且受到ACK应答则置此位)在发送数据时，数据寄存器为空时该位被置’1’，在发送地址阶段不设置该位;软件写数据到DR寄存器可清除该位；或在发生一个起始或停止条件后，或当PE=0时由硬件自动清除;
	//	  如果收到一个NACK，或下一个要发送的字节是PEC(PEC=1)，该位不被置位；在写入第1个要发送的数据后，或设置了BTF时写入数据，都不能清除TxE位，这是因为数据寄存器仍然为空
	//TRA:数据已发送,在整个地址传输阶段的结尾，该位根据地址字节的R/W位来设定;在检测到停止条件(STOPF=1)、重复的起始条件或总线仲裁丢失(ARLO=1)后，或当PE=0时，硬件将其清除
	//delay_100ns(10);
	I2C1->CR1 |= 0x0100;   //起始条件
    I2C1->DR = E2PROM_ADDR&0xFFFE;	  //器件地址
	while(I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) == ERROR);		 
	
	I2C_SendData(I2C1, 0xFF&(add>>8));

	//检查TRA(发送完成), BUSY(正在通信), MSL(本机为主设备), TXE(DR为空) and BTF(字节发送结束)标志位
	//BTF:字节发送结束,当NOSTRETCH=0时,在发送时，当一个新数据将被发送且数据寄存器还未被写入新的数据(TxE=1);在软件读取SR1 寄存器后，对数据寄存器的读或写操作将清除该位；或在传输中发送一个起始或停止条件后，或当PE=0时，由硬件清除该位
	//	  在收到一个NACK后，BTF位不会被置位;如果下一个要传输的字节是PEC(I2C_SR2寄存器中TRA为’1’，同时I2C_CR1寄存器中PEC为’1’)，BTF 位不会被置位
	while(I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED) == ERROR);  	
	
	I2C_SendData(I2C1, 0xFF&add);
	while(I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED) == ERROR);

	I2C_SendData(I2C1, data);
	while(I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED) == ERROR);

	I2C_GenerateSTOP(I2C1, ENABLE);
	//产生停止条件不产生事件，且会清除部分标志位
	delay_us(50);
	//I2C_AcknowledgePolling(E2PROM_ADDR);
}

uint8_t e2prom_read_byte(uint16_t add)
{
	uint8_t data;
	
	while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY) == SET);
	
	I2C_GenerateSTART(I2C1, ENABLE);
	while(I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT) == ERROR);

	I2C_Send7bitAddress(I2C1, E2PROM_ADDR, I2C_Direction_Transmitter);
	while(I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) == ERROR);
	
	I2C_SendData(I2C1, 0xFF&(add>>8));
	while(I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED) == ERROR);
	I2C_SendData(I2C1, 0xFF&add);
	while(I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED) == ERROR);
	
	//重新产生起始信号
	I2C_GenerateSTART(I2C1, ENABLE);
	while(I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT) == ERROR);
	
	I2C_Send7bitAddress(I2C1, E2PROM_ADDR, I2C_Direction_Receiver);
	//检查BUSY, MSL and ADDR(检测ACK应答信号)位
	while(I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED) == ERROR); 

	I2C_AcknowledgeConfig(I2C1, DISABLE);

	//检查BUSY, MSL and RxNE(DR接收器非空)标志位
	while(I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED) == ERROR);
	data = I2C_ReceiveData(I2C1);

	I2C_GenerateSTOP(I2C1, ENABLE);

	delay_us(50);

	//I2C_AcknowledgeConfig(I2C1, ENABLE);

	return data;
}

//内部数据地址的低5位在每接收一个字节后自增，而高位不会自增，保持该页在存储器中的地址。如果有超过32个字节被写入，则页地址会回滚到页头，覆盖前面的数据。
void e2prom_write_bytes(uint16_t start_addr, uint16_t bytenum, uint8_t* dat)
{
	//uint16_t num_temp = bytenum;
	uint16_t addr_temp = start_addr;
	uint8_t i;
	//uint8_t k = 0;

	while(bytenum>0){
		if(bytenum>=E2PROM_PageSize){
			bytenum -= E2PROM_PageSize;

			//while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY) == SET);
			/*
			I2C_GenerateSTART(I2C1, ENABLE);
			while(I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT) == ERROR);

			I2C_Send7bitAddress(I2C1, E2PROM_ADDR, I2C_Direction_Transmitter);
			while(I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) == ERROR);*/
			I2C1->CR1 |= 0x0100;   //起始条件
    		I2C1->DR = E2PROM_ADDR&0xFFFE;	  //器件地址
			while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

			I2C_SendData(I2C1, 0xFF&(addr_temp>>8));
			while(I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED) == ERROR);
			I2C_SendData(I2C1, 0xFF&addr_temp);
			while(I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED) == ERROR);

			for(i=0; i<E2PROM_PageSize; i++){
				I2C_SendData(I2C1, *dat++);
				while(I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED) == ERROR);
			}
			addr_temp += E2PROM_PageSize;

			I2C_GenerateSTOP(I2C1, ENABLE);

			delay_us(50);

			//I2C_AcknowledgePolling(E2PROM_ADDR);
		}
		else{
			//while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY) == SET);			
			//delay_100ns(10);
//i2c_writebytes_restart:
/*
			I2C_GenerateSTART(I2C1, ENABLE);
			while(I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT) == ERROR);

			I2C_Send7bitAddress(I2C1, E2PROM_ADDR, I2C_Direction_Transmitter);
			while(I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) == ERROR);
			//if(I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) == ERROR) goto i2c_writebytes_restart;
*/
			I2C1->CR1 |= 0x0100;   //起始条件
			while(I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT) == ERROR);
    		I2C1->DR = E2PROM_ADDR&0xFFFE;	  //器件地址
			while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

			I2C_SendData(I2C1, 0xFF&(addr_temp>>8));
			while(I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED) == ERROR);
			I2C_SendData(I2C1, 0xFF&addr_temp);
			while(I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED) == ERROR);

			for(i=0; i<bytenum; i++){
				I2C_SendData(I2C1, *dat++);
				while(I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED) == ERROR);
			}
			bytenum = 0;

			I2C_GenerateSTOP(I2C1, ENABLE);
			delay_us(50);
			//I2C_AcknowledgePolling(E2PROM_ADDR);
		}
	}
}

void e2prom_read_bytes(uint16_t start_addr, uint16_t bytenum, uint8_t* buf)
{
	uint16_t i;

	while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY) == SET);
	
	I2C_GenerateSTART(I2C1, ENABLE);
	while(I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT) == ERROR);

	I2C_Send7bitAddress(I2C1, E2PROM_ADDR, I2C_Direction_Transmitter);
	while(I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) == ERROR);
	
	I2C_SendData(I2C1, 0xFF&(start_addr>>8));
	while(I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED) == ERROR);
	I2C_SendData(I2C1, 0xFF&start_addr);
	while(I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED) == ERROR);
	
	//重新产生起始信号
	I2C_GenerateSTART(I2C1, ENABLE);
	while(I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT) == ERROR);
	
	I2C_Send7bitAddress(I2C1, E2PROM_ADDR, I2C_Direction_Receiver);
	//检查BUSY, MSL and ADDR(检测ACK应答信号)位
	while(I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED) == ERROR); 

	for(i=bytenum-1; i>0; i--){
	//检查BUSY, MSL and RxNE(DR接收器非空)标志位
		while(I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED) == ERROR);
		*buf++ = I2C_ReceiveData(I2C1);
	}

	I2C_AcknowledgeConfig(I2C1, DISABLE);	//最后一位不产生应答信号，表示读取结束

	while(I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED) == ERROR);
	*buf = I2C_ReceiveData(I2C1);

	I2C_GenerateSTOP(I2C1, ENABLE);
	delay_us(50);
	//I2C_AcknowledgeConfig(I2C1, ENABLE);
}
#endif
