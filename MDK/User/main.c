//#include "include_h_files.h"
//ϵͳ�ڲ��豸ͷ�ļ�
#include "stm32f10x.h"
#include "stm32f10x_conf.h"
#include "system_conf.h"
#include "stm32f10x_it.h"

//ϵͳ�ⲿ�ӿ�ʹ�ú���
#include "USART_OP.h"
#include "SPI_OP.h"

//��Χ����ʹ�ú���
#include "ENC28J60.h"
#include "netconf.h"
#include "ethernetif.h"
#include "stdio.h"

//Lwip
#include "lwip/err.h"
#include "lwip/dhcp.h"
#include "etharp.h"

//����Ӧ�ò�
#include "tcp_server.h"
#include "tcp_client.h"
#include "arch/cc.h"

#include "global_val.h"

#ifndef OLD
#define OLD 0
#endif

extern struct netif enc28j60;
//������ʹ�þ�̬IP������ԣ����ﲻ����DHCPЭ��
//����Ӧ�ó���Ϊtcp�ͻ��˺�tcp��������tcp_client, tcp_serve
struct app_arg{
	uint8_t app_state;
	uint8_t textlen;
	uint8_t* dataptr;
};

int main(void){
	//uint8_t init_flag = 0;

	Project_Config();

	printf("LWIP TEST Start!\n");

	GPIOs_Init();

	LwIP_Init();  //��ʼ��lwipЭ��ջ ����оƬ���� lwip��ײ������Ĺ���

	tcp_server_init();
	//tcp_client_request();

	//��ЧuIPЭ��ջ����������enc28j60���ݰ�����0��������Ѱ�����ethernetif_input�����ɴ˺�������low_level_input����ײ����ģ��
	while(1){
		lwip_eth_poll(LocalTime);           //����Ƿ����������룬����ѯ�����ӵ�����״̬
		/*
		if(enc28j60.dhcp->state == DHCP_BOUND && init_flag == 0){
			tcp_server_init();
			tcp_client_request();
			init_flag = 1;
		}*/
	}

	//return 0;
}
