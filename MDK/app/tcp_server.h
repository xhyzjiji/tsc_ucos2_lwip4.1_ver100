#ifndef _TCP_SERVER_H_
#define _TCP_SERVER_H_

#include "include_global_val.h"

#include "lwip/tcp.h"
#include "lwip/err.h"
#include <string.h>
#include <stdio.h>

void print_server_pcb_state(struct tcp_pcb *pcb);
void tcp_server_errf(void *arg, err_t err);
err_t tcp_server_sent(void *arg, struct tcp_pcb *pcb, u16_t len);
err_t tcp_server_poll(void *arg, struct tcp_pcb *pcb);
err_t tcp_server_accept(void *arg, struct tcp_pcb *pcb, err_t err);  //当监听状态的pcb与一个新来的连接建立连接则调用此函数;
err_t tcp_server_recv(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err);
void tcp_server_init(void);

#endif
