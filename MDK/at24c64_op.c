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
* Description    : �ȴ���ȡI2C���߿���Ȩ �ж�æ״̬
* Input          : - I2Cx:I2C�Ĵ�����ַ
*                  - I2C_Addr:��������ַ
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
/*Once the internally-timed write cycle has started and the EEPROM inputs are disabled, acknowledge polling can be initiated.
 This involves sending a start condition followed by the device address word.
 The read/write bit is representative of the operation desired.
 Only if the internal write cycle has completed will the EEPROM respond with a zero, allowing the read or write sequence to continue.
 ��e2prom�ڲ�д���ڿ�ʼʱ�����뱻��ֹ����ʱ��ʹ�á���ѯӦ�𡱣�������һ����ʼ������Ȼ������豸��ַ�Ͷ�д����λ�������ڲ�д���ڽ�����e2prom����Ӧһ���͵�ƽ����ʾ��дʱ��ļ����� 
*/
#ifdef i2c_simulation

//IICģ��ʱ������
void e2prom_start_condition(void){  //ͨ����ʼʱ��scl�ź���Ϊ�ͣ�sda�ź���Ϊ�ߣ���ʼ�����������Ϊ��
	//��ʼ����
	reset_scl; set_sda;
	delay_100ns(2);

	set_scl;
	delay_100ns(6);  //tsu.sta   min:0.6us in 1.8Vcc, min:0.25us in 5.0Vcc
	reset_sda;
	delay_100ns(6);  //thd.sta   min:0.6us in 1.8Vcc, min:0.25us in 5.0Vcc
	reset_scl;
	delay_100ns(13); //tlow      min:1.3us in 1.8Vcc, min:0.4us in 5.0Vcc
}

void e2prom_stop_condition(void){	//����ǰ��scl sda��Ϊ�ͣ���������������scl�ź���Ϊ�ͣ�sda�ź���Ϊ��
	reset_scl_sda; delay_100ns(2);
	
	set_scl;
	delay_100ns(6);	 //tsu.sto   min:0.6us in 1.8Vcc, min:0.25us in 5.0Vcc
	set_sda;
	delay_100ns(13); //tbuf      min:1.3us in 1.8Vcc, min:0.5us in 5.0Vcc
	reset_scl;
	delay_100ns(6);

	//delay_us(50);
}

void e2prom_reset(void){  //���ݴ�����ɺ�sclΪ�ͣ�sdaΪ��
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
	uint16_t i; //ͳ�Ƶȴ�����

	state = ERROR;
	i = 0;

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7; //PB6--SCL  PB7--SDA �ͷ�SDA�ź���
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

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7; //PB6--SCL  PB7--SDA �ͷ�SDA�ź���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	while(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_7) == (uint8_t)Bit_RESET);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

void e2prom_waitfor_ack(void){
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7; //PB6--SCL  PB7--SDA �ͷ�SDA�ź���
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

void e2prom_send_data(uint8_t dat){	 //����˺���ʱ��sclȷ��Ϊ��
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

		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7; //PB6--SCL  PB7--SDA �ͷ�SDA�ź���
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

	e2prom_send_data(E2PROM_ADDR&0xFE);  //д��������ַ
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

	e2prom_send_data((E2PROM_ADDR&0xFE)|0x01);  //д��������ַ
	e2prom_waitfor_ack();

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7; //PB6--SCL  PB7--SDA �ͷ�SDA�ź���
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

	e2prom_send_data((E2PROM_ADDR&0xFE));  //д��������ַ
	e2prom_waitfor_ack();
	e2prom_send_data(0xFF&(add>>8));
	e2prom_waitfor_ack();
	e2prom_send_data(0xFF&add);
	e2prom_waitfor_ack();

	e2prom_start_condition();

	e2prom_send_data((E2PROM_ADDR&0xFE)|0x01);  //д��������ַ
	e2prom_waitfor_ack();

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7; //PB6--SCL  PB7--SDA �ͷ�SDA�ź���
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

			e2prom_send_data(E2PROM_ADDR&0xFE);  //д��������ַ
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

			//I2C_AcknowledgePolling(E2PROM_ADDR);  //�ȴ�д�������
			delay_us(50); //acknowledge_poll();
		}
		else{
			e2prom_check_busy();			

			e2prom_start_condition();

			e2prom_send_data(E2PROM_ADDR&0xFE);  //д��������ַ
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

	e2prom_send_data(E2PROM_ADDR&0xFE);  //д��������ַ
	e2prom_waitfor_ack();
	e2prom_send_data(0xFF&(start_addr>>8));
	e2prom_waitfor_ack();
	e2prom_send_data(0xFF&start_addr);
	e2prom_waitfor_ack();

	e2prom_start_condition();

	e2prom_send_data((E2PROM_ADDR&0xFE)|0x01);  //д��������ַ
	e2prom_waitfor_ack();

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7; //PB6--SCL  PB7--SDA �ͷ�SDA�ź���
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

//IIC��STM32Ӳ���ӿ�����
//I2CЭ��ͨ��С��ʶ��ACK���ڵ�9��SCL���ڣ���SCLΪ��ʱ���ɽ����豸��SDA������ΪACKӦ���źţ���ʾ�����ɹ�����NACK�򱣳�SDAΪ�ߣ���Ʋ�����Ӧ����Ϊ��
static void I2C_AcknowledgePolling(uint8_t I2C_Addr)
{
  vu16 SR1_Tmp;
  do
  {   
    I2C_GenerateSTART(I2C1, ENABLE); /*��ʼλ*/
    /*��SR1*/
    SR1_Tmp = I2C_ReadRegister(I2C1, I2C_Register_SR1);
    /*������ַ(д)*/
//#ifdef AT24C01A

	I2C_Send7bitAddress(I2C1, I2C_Addr, I2C_Direction_Transmitter);
//#else

//	I2C_Send7bitAddress(I2Cx, 0, I2C_Direction_Transmitter);
//#endif

  }while(!(I2C_ReadRegister(I2C1, I2C_Register_SR1) & 0x0002));	 //����Ƿ񷵻�Ӧ���ź�
  
  I2C_ClearFlag(I2C1, I2C_FLAG_AF);
    
  I2C_GenerateSTOP(I2C1, ENABLE);  /*ֹͣλ*/  
}

void e2prom_write_byte(uint16_t add, uint8_t data)
{
	//I2C_AcknowledgePolling(E2PROM_ADDR);
	
	//���俪ʼǰ��������Ƿ���У������SR2�ϵ�BUSYλ
	/*while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY) == SET);*/
	
	/*
	//������ʼ����
	I2C_GenerateSTART(I2C1, ENABLE);

	//���BUSY MSL SBλ���ж��Ƿ����EV5�¼�����EV5 ��SB=1����SR1 Ȼ�󽫵�ַд��DR�Ĵ�����������¼�
	//SB:�����ͳ���ʼ����ʱ��λ���á�1��;�����ȡSR1 �Ĵ�����д���ݼĴ����Ĳ����������λ����PE=0ʱ��Ӳ�������λ(I2C_Cmd����ʹ��ʧ��I2C�豸�����޸�PEλ) 
	//BUSY:�ڼ�⵽SDA��SClΪ�͵�ƽʱ��Ӳ������λ�á�1��;����⵽һ��ֹͣ����ʱ��Ӳ������λ���
	//MSL:���ӿڴ�����ģʽ(SB=1)ʱ��Ӳ������λ��λ;�������ϼ�⵽һ��ֹͣ�������ٲö�ʧ(ARLO=1ʱ)����PE=0ʱ��Ӳ�������λ
	while(I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT) == ERROR);  //��ȡSR1 SR2״̬�Ĵ����ж�BUSY MSL SBλ�Ƿ���λ��BUSY MSLλ�ڼ�⵽ֹͣ����ʱ��Ӳ������

	I2C_Send7bitAddress(I2C1, E2PROM_ADDR, I2C_Direction_Transmitter);	//��DRд��������ַ�ʹ��䷽�����ڶ�ȡSR1��дDR��SB��Ӳ������
	*/
	//���BUSY, MSL, ADDR, TXE and TRA��־λ
	//ADDR:��stm32����I2C���豸λ�ã�ADDR�����ַ�ѱ�����(��ģʽ):7λ��ַģʽʱ�����յ���ַ��ACK���λ���á�1�������յ�NACK��ADDRλ���ᱻ��λ���������ȡSR1�Ĵ����󣬶�SR2�Ĵ����Ķ������������λ����PE=0ʱ����Ӳ�������λ 
	//TXE:(DR������Ϊ�գ����ܵ�ACKӦ�����ô�λ)�ڷ�������ʱ�����ݼĴ���Ϊ��ʱ��λ���á�1�����ڷ��͵�ַ�׶β����ø�λ;���д���ݵ�DR�Ĵ����������λ�����ڷ���һ����ʼ��ֹͣ�����󣬻�PE=0ʱ��Ӳ���Զ����;
	//	  ����յ�һ��NACK������һ��Ҫ���͵��ֽ���PEC(PEC=1)����λ������λ����д���1��Ҫ���͵����ݺ󣬻�������BTFʱд�����ݣ����������TxEλ��������Ϊ���ݼĴ�����ȻΪ��
	//TRA:�����ѷ���,��������ַ����׶εĽ�β����λ���ݵ�ַ�ֽڵ�R/Wλ���趨;�ڼ�⵽ֹͣ����(STOPF=1)���ظ�����ʼ�����������ٲö�ʧ(ARLO=1)�󣬻�PE=0ʱ��Ӳ���������
	//delay_100ns(10);
	I2C1->CR1 |= 0x0100;   //��ʼ����
    I2C1->DR = E2PROM_ADDR&0xFFFE;	  //������ַ
	while(I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) == ERROR);		 
	
	I2C_SendData(I2C1, 0xFF&(add>>8));

	//���TRA(�������), BUSY(����ͨ��), MSL(����Ϊ���豸), TXE(DRΪ��) and BTF(�ֽڷ��ͽ���)��־λ
	//BTF:�ֽڷ��ͽ���,��NOSTRETCH=0ʱ,�ڷ���ʱ����һ�������ݽ������������ݼĴ�����δ��д���µ�����(TxE=1);�������ȡSR1 �Ĵ����󣬶����ݼĴ����Ķ���д�����������λ�����ڴ����з���һ����ʼ��ֹͣ�����󣬻�PE=0ʱ����Ӳ�������λ
	//	  ���յ�һ��NACK��BTFλ���ᱻ��λ;�����һ��Ҫ������ֽ���PEC(I2C_SR2�Ĵ�����TRAΪ��1����ͬʱI2C_CR1�Ĵ�����PECΪ��1��)��BTF λ���ᱻ��λ
	while(I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED) == ERROR);  	
	
	I2C_SendData(I2C1, 0xFF&add);
	while(I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED) == ERROR);

	I2C_SendData(I2C1, data);
	while(I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED) == ERROR);

	I2C_GenerateSTOP(I2C1, ENABLE);
	//����ֹͣ�����������¼����һ�������ֱ�־λ
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
	
	//���²�����ʼ�ź�
	I2C_GenerateSTART(I2C1, ENABLE);
	while(I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT) == ERROR);
	
	I2C_Send7bitAddress(I2C1, E2PROM_ADDR, I2C_Direction_Receiver);
	//���BUSY, MSL and ADDR(���ACKӦ���ź�)λ
	while(I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED) == ERROR); 

	I2C_AcknowledgeConfig(I2C1, DISABLE);

	//���BUSY, MSL and RxNE(DR�������ǿ�)��־λ
	while(I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED) == ERROR);
	data = I2C_ReceiveData(I2C1);

	I2C_GenerateSTOP(I2C1, ENABLE);

	delay_us(50);

	//I2C_AcknowledgeConfig(I2C1, ENABLE);

	return data;
}

//�ڲ����ݵ�ַ�ĵ�5λ��ÿ����һ���ֽں�����������λ�������������ָ�ҳ�ڴ洢���еĵ�ַ������г���32���ֽڱ�д�룬��ҳ��ַ��ع���ҳͷ������ǰ������ݡ�
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
			I2C1->CR1 |= 0x0100;   //��ʼ����
    		I2C1->DR = E2PROM_ADDR&0xFFFE;	  //������ַ
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
			I2C1->CR1 |= 0x0100;   //��ʼ����
			while(I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT) == ERROR);
    		I2C1->DR = E2PROM_ADDR&0xFFFE;	  //������ַ
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
	
	//���²�����ʼ�ź�
	I2C_GenerateSTART(I2C1, ENABLE);
	while(I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT) == ERROR);
	
	I2C_Send7bitAddress(I2C1, E2PROM_ADDR, I2C_Direction_Receiver);
	//���BUSY, MSL and ADDR(���ACKӦ���ź�)λ
	while(I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED) == ERROR); 

	for(i=bytenum-1; i>0; i--){
	//���BUSY, MSL and RxNE(DR�������ǿ�)��־λ
		while(I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED) == ERROR);
		*buf++ = I2C_ReceiveData(I2C1);
	}

	I2C_AcknowledgeConfig(I2C1, DISABLE);	//���һλ������Ӧ���źţ���ʾ��ȡ����

	while(I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED) == ERROR);
	*buf = I2C_ReceiveData(I2C1);

	I2C_GenerateSTOP(I2C1, ENABLE);
	delay_us(50);
	//I2C_AcknowledgeConfig(I2C1, ENABLE);
}
#endif
