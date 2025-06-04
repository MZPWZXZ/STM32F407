#include "usart.h"



void my_uart_init()
{
    GPIO_InitTypeDef    gpio;

    gpio.GPIO_Mode = GPIO_Mode_AF;
    gpio.GPIO_OType = GPIO_OType_PP;
    gpio.GPIO_PuPd = GPIO_PuPd_UP;
    gpio.GPIO_Speed = GPIO_High_Speed;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
    gpio.GPIO_Pin = GPIO_Pin_13;
    GPIO_Init(GPIOE, &gpio);

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
    gpio.GPIO_Pin = GPIO_Pin_13;
    GPIO_Init(GPIOE, &gpio);

}

