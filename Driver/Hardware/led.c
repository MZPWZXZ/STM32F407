#include "led.h"



void my_led_config()
{
    GPIO_InitTypeDef    gpio;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
    gpio.GPIO_Mode = GPIO_Mode_OUT;
    gpio.GPIO_OType = GPIO_OType_PP;
    gpio.GPIO_Pin = GPIO_Pin_13;
    gpio.GPIO_PuPd = GPIO_PuPd_UP;
    gpio.GPIO_Speed = GPIO_High_Speed;
    GPIO_Init(GPIOE, &gpio);

    LED1_OFF();//默认关闭
}

void my_led_toggle()
{
    GPIO_ToggleBits(GPIOE, GPIO_Pin_13);
}
