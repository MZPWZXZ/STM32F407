#ifndef __MY_LED_H__
#define __MY_LED_H__


#include "stm32f4xx.h"


#define     LED1_ON()       do{ GPIO_ResetBits(GPIOE, GPIO_Pin_13); }while(0)
#define     LED1_OFF()      do{ GPIO_SetBits(GPIOE, GPIO_Pin_13); }while(0)
#define     LED1_TOGGLE()   do{ GPIO_ToggleBits(GPIOE, GPIO_Pin_13); }while(0)


void my_led_config(void);
void my_led_toggle(void);

#endif
