//#include "include_h_files.h"
//系统内部设备头文件
#include "stm32f10x.h"
#include "stm32f10x_conf.h"
#include "system_conf.h"
#include "stm32f10x_it.h"

//系统外部接口使用函数
#include "USART_OP.h"
#include "SPI_OP.h"

//外围器件使用函数
#include "ENC28J60.h"
#include "netconf.h"
#include "ethernetif.h"
#include "stdio.h"

//Lwip
#include "lwip/err.h"
#include "lwip/dhcp.h"
#include "etharp.h"

//网络应用层
#include "tcp_server.h"
#include "tcp_client.h"
#include "arch/cc.h"

#include "global_val.h"

#ifndef OLD
#define OLD 0
#endif

extern struct netif enc28j60;
//局域网使用静态IP分配策略，这里不适用DHCP协议
//测试应用程序为tcp客户端和tcp服务器：tcp_client, tcp_serve
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

	LwIP_Init();  //初始化lwip协议栈 网络芯片配置 lwip与底层驱动的关联

	tcp_server_init();
	//tcp_client_request();

	//仿效uIP协议栈的做法，当enc28j60数据包大于0，则继续把包送入ethernetif_input，并由此函数调用low_level_input这个底层接收模块
	while(1){
		lwip_eth_poll(LocalTime);           //检查是否有数据输入，并轮询已连接的链接状态
		/*
		if(enc28j60.dhcp->state == DHCP_BOUND && init_flag == 0){
			tcp_server_init();
			tcp_client_request();
			init_flag = 1;
		}*/
	}

	//return 0;
}
