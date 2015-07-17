#include "include_global_val.h"

#include "lwip/tcp.h"
#include "lwip/err.h"
#include <string.h>
#include <stdio.h>

#include "ds1302_op.h"
#include "includes.h"

enum tcp_client_state{
	CLIENT_CONNECTED = 0x00U,
	CLIENT_WAITING_FOR_CMD,
	CLIENT_BUSY,
	CLIENT_SENT,
	CLIENT_ERROR,
	CLIENT_CLOSE,
	CLIENT_WAITING_FOR_CONNECTION,
	CLIENT_WELCOME
};

struct tcp_client_app_arg{
	uint8_t app_state;
	uint8_t textlen;
	char* dataptr;
}; //根据不同的应用程序，订造自己的应用程序函数

static struct ip_addr destip;
//destip.ip_addr = (uint32_t)192+(168<<8)+(1<<16)+(20<<24);
void print_client_pcb_state(struct tcp_pcb *pcb){
	printf("client state:%s\n", tcp_debug_state_str(pcb->state));
}

void tcp_client_errf(void *arg, err_t err)
{
	struct tcp_client_app_arg* pro_arg = (struct tcp_client_app_arg*)arg;

	printf("tcp client err\n");
	
	pro_arg->app_state = CLIENT_ERROR;

	switch(err){
		case ERR_MEM: printf("Out of memory error\n"); break;
		case ERR_BUF: printf("Buffer error\n"); break;
		case ERR_TIMEOUT: printf("Timeout\n"); break;
		case ERR_RTE: printf("Routing problem\n"); break;
		case ERR_ABRT: printf("Connection aborted\n"); break;
		case ERR_RST: printf("Connection reset\n"); break;
		case ERR_CLSD: printf("Connection closed\n"); break;
		case ERR_CONN: printf("Not connected\n"); break;
		case ERR_VAL: printf("Illegal value\n"); break;
		case ERR_ARG: printf("Illegal argument\n"); break;
		case ERR_USE: printf("Address in use\n"); break;
		case ERR_IF: printf("Low-level netif error\n"); break;
		case ERR_ISCONN: printf("Already connected\n"); break;
		case ERR_INPROGRESS: printf("Operation in progress\n"); break;
		default: printf("Unknown error\n");
	}

	if(time_update_status==DS1302_SYNING||time_update_status==DS1302_SYN_FAIL) time_update_status = DS1302_NEED_SYN;
}

err_t tcp_client_recv(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err)
{
	struct pbuf *q;
	struct tcp_client_app_arg *appcmd = (struct tcp_client_app_arg *)arg;
	uint16_t i = 0;
	uint8_t ip[4];
	char* str;
	struct tm mytime;

	const char* cmd1 = "hello";
	const char* cmd2 = "echo name";
	const char* cmd3 = "echo log";
	const char* cmd4 = "echo IP";
	const char* cmd5 = "bye";
	const char* cmd6 = "time calibration"; //命令格式：time calibration:YY MM DD HH MM SS

	//printf("tcp client recv\n");
	print_client_pcb_state(pcb);
		
	/* We perform here any necessary processing on the pbuf */
	if (p != NULL) 
	{        
		/* We call this function to tell the LwIp that we have processed the data */
		/* This lets the stack advertise a larger window, so more data can be received*/
		tcp_recved(pcb, p->tot_len);
		
		/* Check the name if NULL, no data passed, return withh illegal argument error */
		if(appcmd == NULL) 
		{
		  pbuf_free(p);
		  return ERR_ARG;
		}
		/*
		for(q=p; q != NULL; q = q->next) 
		{
			
			c = q->payload;
			for(i=0; i<q->len && !done; i++) 
			{
				done = ((c[i] == '\r') || (c[i] == '\n'));
				if(name->length < MAX_NAME_SIZE) 
				{
				  name->bytes[name->length++] = c[i];
				}
			}
			
		}
		*/
		appcmd->dataptr = (char*)mem_calloc(sizeof(uint8_t), p->tot_len);
		if(appcmd->dataptr == NULL){
			printf("Memory error\n");
			return ERR_MEM;
		}
		appcmd->textlen = p->tot_len;
		for(q=p; q!=NULL; q=q->next){
			memcpy((uint8_t*)&appcmd->dataptr[i], q->payload, q->len);  //同一个数据包会存在一个pbuf链表中
			i = i + q->len;
		}
		//应用层代码
		switch(appcmd->app_state){
			case CLIENT_CONNECTED:
			case CLIENT_WAITING_FOR_CMD: {
				if(memcmp(appcmd->dataptr, cmd1, strlen(cmd1)) == 0){
					str = "hello\n";
					//tcp_write(pcb, (uint8_t*)str, strlen(str), 1);
				}
				else if(memcmp(appcmd->dataptr, cmd2, strlen(cmd2)) == 0){
					str = "lwip\n";
				}
				else if(memcmp(appcmd->dataptr, cmd3, strlen(cmd3)) == 0){
					str = "log is ...\n";
				}
				else if(memcmp(appcmd->dataptr, cmd4, strlen(cmd4)) == 0){
					ip[0] = pcb->remote_ip.addr>>24;
					ip[1] = pcb->remote_ip.addr>>16;
					ip[2] = pcb->remote_ip.addr>>8;
					ip[3] = pcb->remote_ip.addr;
					str = mem_calloc(sizeof(uint8_t), 30);
					sprintf((char*)str, "ipaddr:%d,%d,%d,%d\n", ip[3], ip[2], ip[1], ip[0]);
					tcp_write(pcb, (uint8_t *)str, strlen(str), 1);
					mem_free(str);
					break;
				}
				else if(memcmp(appcmd->dataptr, cmd5, strlen(cmd5)) == 0){
					appcmd->app_state = CLIENT_CLOSE;
					//goto get_close;
					break;
				}
				else if(memcmp(appcmd->dataptr, cmd6, strlen(cmd6)) == 0){
					sscanf(appcmd->dataptr, "%*[^:]:%d%d%d%d%d%d", &mytime.tm_year, &mytime.tm_mon, &mytime.tm_mday, &mytime.tm_hour, &mytime.tm_min, &mytime.tm_sec);
					if(mytime.tm_year<100&&mytime.tm_mon-1<12&&mytime.tm_mday<32&&mytime.tm_hour<24&&mytime.tm_min<60&&mytime.tm_sec<60&&mytime.tm_mon>0){
						str = "time calibration success\n";
						DS1302_SetSec(mytime.tm_sec, DISABLE);
						DS1302_SetMin(mytime.tm_min);
						DS1302_SetHour(mytime.tm_hour, DISABLE);
						DS1302_SetDate(mytime.tm_mday);
						DS1302_SetMon(mytime.tm_mon-1);
						DS1302_SetYear(mytime.tm_year);
						
						mytime.tm_year += 2000; 
						mytime.tm_mon --;
						RTC_WaitForLastTask();
						RTC_ITConfig(RTC_IT_SEC, DISABLE);  
						Time_SetCalendarTime(mytime);  
						RTC_ITConfig(RTC_IT_SEC, ENABLE);

						time_update_status = DS1302_SYN_SUL;
						  
						appcmd->app_state = CLIENT_CLOSE;
					}
					else{
						str = "time calibratioin failed\n";
						time_update_status = DS1302_SYN_FAIL;
					}
					tcp_write(pcb, (uint8_t *)str, strlen(str), 1);
					break;
				}
				else{
					str = "unknown cmd\n";
					//tcp_write(pcb, (uint8_t*)str, strlen(str), 1);
				}
				tcp_write(pcb, (uint8_t *)str, strlen(str), 1);
				break;
			}
			default: {
				str = "lwip down\n";
				tcp_write(pcb, (uint8_t *)str, strlen(str), 1);
				break;
			}
		}
		mem_free(appcmd->dataptr);
		//取得应用层数据后，先释放pbuf队列相应数据链，再对数据进行解析与执行操作
		/* End of processing, we free the pbuf */
		pbuf_free(p);
	}  
	else if (err == ERR_OK) 
	{
		/* When the pbuf is NULL and the err is ERR_OK, the remote end 
		                                    is closing the connection. */
		/* We free the allocated memory and we close the connection */
		mem_free(appcmd);
		return tcp_close(pcb);
	}
	return ERR_OK;
}

//该函数用于设定远程主机成功接收到数据后调用的回调函数，即远程主机对接收的数据进行应答后调用
err_t tcp_client_sent(void *arg, struct tcp_pcb *pcb, u16_t len)
{
	struct tcp_client_app_arg* pro_arg = (struct tcp_client_app_arg*)arg;

	//printf("tcp client sent\n");
	print_client_pcb_state(pcb);

	switch(pro_arg->app_state){
		case CLIENT_WAITING_FOR_CMD:{
			printf("acked!\n");
			break;
		}
		case CLIENT_CLOSE:{
			mem_free(pro_arg);
			return tcp_close(pcb);
		}
		default:{}
	}

	return ERR_OK;
}

err_t tcp_client_poll(void *arg, struct tcp_pcb *pcb)
{
	struct tcp_client_app_arg* pro_arg = (struct tcp_client_app_arg*)arg;
	const char* str;// = "I love you\n";

	//printf("tcp client poll\n");
	print_client_pcb_state(pcb);

	//可以主动发送数据给客户端或者服务器
	switch(pro_arg->app_state){
		case CLIENT_WAITING_FOR_CMD:{
			if(time_update_status==DS1302_SYNING||time_update_status==DS1302_SYN_FAIL) str = "time update\n";
			else str = "I love you\n"; 
			tcp_write(pcb, (char*)str, strlen(str), 1); 
			break;
		}
		case CLIENT_WELCOME: str = "Welcome to tcp client\n"; tcp_write(pcb, (uint8_t *)str, strlen(str), 1); pro_arg->app_state = CLIENT_WAITING_FOR_CMD; break;
		default: break;
	}
	//tcp_write(pcb, (char*)str, strlen(str), 1);

	return ERR_OK;
}

extern OS_EVENT *mbox_client_connected;
INT8U client_signal;
err_t tcp_client_connected(void *arg, struct tcp_pcb *pcb, err_t err)
{
	//INT8U uc_err;
	const char* str; 
	
	struct tcp_client_app_arg* app_arg = (struct tcp_client_app_arg*)arg;

	str = "Welcome to tcp client\n";
	
	//printf("tcp client connected\n");
	print_client_pcb_state(pcb);
	
	tcp_err(pcb, tcp_client_errf);
  	tcp_recv(pcb, tcp_client_recv);
	tcp_sent(pcb, tcp_client_sent);
	tcp_poll(pcb, tcp_client_poll, 4);

	//tcp_write(pcb, (uint8_t *)str, strlen(str), 1);
	//app_arg->app_state = CLIENT_WAITING_FOR_CMD;
	printf("space of client send buffer is %d\n", tcp_sndbuf(pcb));
	tcp_write(pcb, (uint8_t *)str, strlen(str), 1);
	printf("space of client send buffer is %d\n", tcp_sndbuf(pcb));

	app_arg->app_state = CLIENT_WELCOME;

	time_update_status = DS1302_SYNING;
	
	client_signal = 1;
	//uc_err = OSMboxPost(mbox_client_connected, &client_signal);
	OSMboxPost(mbox_client_connected, &client_signal);
	
	return ERR_OK;
}

extern void task5_client_reconnect(void *arg);
extern INT8U global_err;
extern OS_STK task5_stk[128];
void tcp_client_request(BOOLEAN create_task)
{
	struct tcp_pcb* client_pcb;
	struct tcp_client_app_arg* app_arg;

	printf("tcp client inti\n");

	destip.addr = (uint32_t)192+(168<<8)+(1<<16)+(120<<24);

	client_pcb = tcp_new();
	print_client_pcb_state(client_pcb);
	if(client_pcb != NULL){
		tcp_arg(client_pcb, mem_calloc(sizeof(struct tcp_client_app_arg), 1));   //为pcb对应的app数据创建存储空间，并把指针存于pcb->callback_arg中	
		app_arg = client_pcb->callback_arg;
		app_arg->app_state = CLIENT_WAITING_FOR_CONNECTION;
		tcp_connect(client_pcb, &destip, 2200, tcp_client_connected);
		if(create_task){
			global_err = OSTaskCreate(task5_client_reconnect, (void *)0, &task5_stk[128-1], 30);
		}
	}
	else{
		//重新申请pcb空间
		printf("tcp alloc failed\n");
	}
}
