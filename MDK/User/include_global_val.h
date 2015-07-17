#ifndef _INCLUDE_GLOBAL_VAL_H_
#define _INCLUDE_GLOBAL_VAL_H_

#include "stm32f10x.h"
#include "time_op.h"

#define eth_rx_buf eth_buf1
#define eth_tx_buf eth_buf2

#ifndef MAX_RX_SIZE
#define MAX_RX_SIZE 1536
#endif

#ifndef MAX_TX_SIZE
#define MAX_TX_SIZE	1500*2
#endif

extern uint8_t ETH_INT;
extern uint32_t LocalTime;
extern uint8_t MACaddr[6];
extern uint8_t eth_buf1[MAX_RX_SIZE];
extern uint8_t eth_buf2[MAX_TX_SIZE];
extern struct netif enc28j60;

extern time_syn time_update_status;

extern uint32_t system_time;

#endif
