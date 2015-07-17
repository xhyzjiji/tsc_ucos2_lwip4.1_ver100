#ifndef _AT24C64_OP_H_
#define _AT24C64_OP_H_

#define e2prom_scl GPIO_Pin_6
#define e2prom_sda GPIO_Pin_7
#define e2prom_port GPIOB

#define i2c_simulation

#define set_scl GPIO_SetBits(e2prom_port, e2prom_scl)
#define set_sda GPIO_SetBits(e2prom_port, e2prom_sda)
#define reset_scl GPIO_ResetBits(e2prom_port, e2prom_scl)
#define reset_sda GPIO_ResetBits(e2prom_port, e2prom_sda)
#define set_scl_sda GPIO_SetBits(e2prom_port, e2prom_scl|e2prom_sda)
#define reset_scl_sda GPIO_ResetBits(e2prom_port, e2prom_scl|e2prom_sda)

void e2prom_reset(void);
void e2prom_write_byte(uint16_t add, uint8_t data);
uint8_t e2prom_read_currentbyte_simulate(void);
uint8_t e2prom_read_byte(uint16_t add);

void e2prom_write_bytes(uint16_t start_addr, uint16_t bytenum, uint8_t* dat);
void e2prom_read_bytes(uint16_t start_addr, uint16_t bytenum, uint8_t* buf);

#endif
