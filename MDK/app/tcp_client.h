#ifndef _TCP_CLIENT_H_
#define _TCP_CLIENT_H_

#include "include_global_val.h"

#include "lwip/tcp.h"
#include "lwip/err.h"
#include <string.h>
#include <stdio.h>
#include "includes.h"

void print_client_pcb_state(struct tcp_pcb *pcb);
void tcp_client_errf(void *arg, err_t err);
err_t tcp_client_recv(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err);
err_t tcp_client_sent(void *arg, struct tcp_pcb *pcb, u16_t len);
err_t tcp_client_poll(void *arg, struct tcp_pcb *pcb);
err_t tcp_client_connected(void *arg, struct tcp_pcb *pcb, err_t err);
void tcp_client_request(BOOLEAN create_task);

#endif
