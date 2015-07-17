#include "include_global_val.h"

#include "lwip/tcp.h"
#include "lwip/err.h"
#include "tcp_server.h"
#include <string.h>
#include <stdio.h>

extern union tcp_listen_pcbs_t tcp_listen_pcbs;

enum tcp_server_state{
	SERVER_CONNECTED = 0x00U,
	SERVER_WAITING_FOR_CMD,
	SERVER_BUSY,
	SERVER_SENT,
	SERVER_ERROR,
	SERVER_CLOSE
};

struct tcp_server_app_arg{
	uint8_t app_state;
	uint8_t textlen;
	uint8_t* dataptr;
};

void print_server_pcb_state(struct tcp_pcb *pcb){
	printf("tcp server state:%s\n", tcp_debug_state_str(pcb->state));
}

void tcp_server_errf(void *arg, err_t err)
{
	//uint8_t i;
	//struct tcp_pcb_listen* pcb;

	struct tcp_server_app_arg* pro_arg = (struct tcp_server_app_arg*)arg;

	printf("tcp server err\n");
	
	pro_arg->app_state = SERVER_ERROR;

	switch(err){
		case ERR_MEM: printf("Out of memory error\n"); break;
		case ERR_BUF: printf("Buffer error\n"); break;
		case ERR_TIMEOUT: printf("Timeout\n"); break;
		case ERR_RTE: printf("Routing problem\n"); break;
		case ERR_ABRT:{ 
			printf("Connection aborted\n"); 
			//tcp_server_init();
			mem_free(pro_arg);
			/*
			for(pcb = (struct tcp_pcb*)tcp_listen_pcbs.listen_pcbs; pcb != NULL; pcb = pcb->next){
				if(pcb->local_port == 1200){
					tcp_accept((struct tcp_pcb*)pcb, tcp_server_accept);
				}
			}
			*/
			break;
		}
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
}

err_t tcp_server_recv(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err)
{
	struct pbuf *q;
	struct tcp_server_app_arg *appcmd = (struct tcp_server_app_arg *)arg;
	uint16_t i = 0;
	uint8_t ip[4];
	uint8_t* str;

	const char* cmd1 = "hello";
	const char* cmd2 = "echo name";
	const char* cmd3 = "echo log";
	const char* cmd4 = "echo IP";
	const char* cmd5 = "bye";

	//printf("tcp server recv\n");
	print_server_pcb_state(pcb);
		
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
		appcmd->dataptr = (uint8_t*)mem_calloc(sizeof(uint8_t), p->tot_len);
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
			case SERVER_CONNECTED:
			case SERVER_WAITING_FOR_CMD: {
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
					appcmd->app_state = SERVER_CLOSE;
					//goto get_close;
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
err_t tcp_server_sent(void *arg, struct tcp_pcb *pcb, u16_t len)
{
	struct tcp_server_app_arg* pro_arg = (struct tcp_server_app_arg*)arg;

	//printf("tcp server sent\n");
	print_server_pcb_state(pcb);

	switch(pro_arg->app_state){
		case SERVER_WAITING_FOR_CMD:{
			printf("acked!\n");
			break;
		}
		/*
		case SERVER_CLOSE:{
			mem_free(pro_arg);
			return tcp_close(pcb);
		}*/
		default:{}
	}

	return ERR_OK;
}

err_t tcp_server_poll(void *arg, struct tcp_pcb *pcb)
{
	struct tcp_server_app_arg* pro_arg = (struct tcp_server_app_arg*)arg;
	//struct tcp_server_app_arg* pro_arg = (struct app_arg*)arg;
	const char* str = "I love you\n";

	//printf("tcp server poll\n");
	print_server_pcb_state(pcb);

	//可以主动发送数据给客户端或者服务器
	switch(pro_arg->app_state){
		case SERVER_WAITING_FOR_CMD: tcp_write(pcb, (char*)str, strlen(str), 1); break;
		case SERVER_CLOSE:{
			mem_free(pro_arg);
			return tcp_close(pcb);
		}
	}

	return ERR_OK;
}

//void *arg属于APP状态字
err_t tcp_server_accept(void *arg, struct tcp_pcb *pcb, err_t err)  //当监听状态的pcb与一个新来的连接建立连接则调用此函数
{
	struct tcp_server_app_arg* pro_arg;
	
	char* str = "Welcome to tcp server\n";
	
	//printf("tcp server accept\n");
	print_server_pcb_state(pcb);

	tcp_arg(pcb, mem_calloc(sizeof(struct tcp_server_app_arg), 1));   //为pcb对应的app数据创建存储空间，并把指针存于pcb->callback_arg中

	tcp_err(pcb, tcp_server_errf);
  	tcp_recv(pcb, tcp_server_recv);
	tcp_sent(pcb, tcp_server_sent);
	tcp_poll(pcb, tcp_server_poll, 4);	  //调用顺序：lwip_eth_poll(LocalTime)》tcp_tmr()》(由pcb结构体的成员poll的注释可以知道poll在tcp_slowtmr()中被调用)tcp_slowtmr()》pcb->polltmr自增(tcp_new时对其初始化为0)》比较polltmr与pollinterval(pollinterval就是这里我们要用tcp_poll设的值)》TCP_EVENT_POLL(tcp,err)》如果err为ERR_OK:tcp_output(pcb)
	//2s调用一次tcp_poll

	tcp_write(pcb, (uint8_t *)str, strlen(str), 1);
	pro_arg = pcb->callback_arg;
	pro_arg->app_state = SERVER_WAITING_FOR_CMD;

	return ERR_OK;
}

void tcp_server_init(void)
{
	struct tcp_pcb *pcb;

	printf("tcp server init\n");
	
	pcb = tcp_new();

	print_server_pcb_state(pcb);
	tcp_bind(pcb, IP_ADDR_ANY, 1200);
	//tcp_bind(pcb, &enc28j60.ip_addr, 2200);
	pcb = tcp_listen(pcb);
	print_server_pcb_state(pcb);
		            		
    tcp_accept(pcb, tcp_server_accept);	 //初始化pcb->accept
}

