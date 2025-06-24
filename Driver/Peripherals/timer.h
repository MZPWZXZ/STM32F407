#ifndef __MY_TIMER_H__
#define __MY_TIMER_H__


#include "stm32f4xx.h"




void delay_timer_config(uint32_t psc, uint32_t arr);
void modbus_timer_config(uint32_t psc, uint32_t arr);
void delay_update(void);
void delay_ms(uint32_t ms);
void ethernet_timer_config(uint32_t psc, uint32_t arr);

#endif
