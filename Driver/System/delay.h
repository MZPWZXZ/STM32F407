#ifndef __DELAY_H__
#define __DELAY_H__



#include "stm32f4xx.h"



void delay_init(u8 SYSCLK);
void delay_us(u32 nus);
void delay_ms(u32 nms);
void delay_xms(u32 nms);



#endif
