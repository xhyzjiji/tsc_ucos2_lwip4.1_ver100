#ifndef _GLOBAL_VAL_H_
#define _GLOBAL_VAL_H_

#define MAX_RX_SIZE 1536
#define MAX_TX_SIZE 1500*2

uint8_t ETH_INT=0;
uint32_t LocalTime;
uint8_t MACaddr[6];
uint8_t eth_buf1[MAX_RX_SIZE];
uint8_t eth_buf2[MAX_TX_SIZE];

uint32_t system_time;

#endif
