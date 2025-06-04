#ifndef __485_h__
#define __485_h__

#include "stm32f4xx.h"

#define RS485_RECV_MAX  100

extern uint16_t g_recv_len;
extern uint8_t g_recv_485_buf[RS485_RECV_MAX];


void rs485_config(uint32_t baud);
void rs485_send_byte(uint8_t byte);
void rs485_send_data(uint8_t *data, uint16_t len);

#endif

