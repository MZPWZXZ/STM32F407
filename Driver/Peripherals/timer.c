#include "timer.h"

static __IO uint32_t g_delay_time = 0;


void delay_timer_config(uint32_t psc, uint32_t arr)
{
    TIM_TimeBaseInitTypeDef     tim_base;
    NVIC_InitTypeDef            nvic;

    //开启时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
    
    //
    tim_base.TIM_ClockDivision = TIM_CKD_DIV1;
    tim_base.TIM_Prescaler = psc - 1;
    tim_base.TIM_Period = arr - 1;
    tim_base.TIM_CounterMode = TIM_CounterMode_Up;
    tim_base.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM1, &tim_base);
    
    TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);

    nvic.NVIC_IRQChannel = TIM1_UP_TIM10_IRQn;
    nvic.NVIC_IRQChannelCmd = ENABLE;
    nvic.NVIC_IRQChannelPreemptionPriority = 3;
    nvic.NVIC_IRQChannelSubPriority = 0;
    NVIC_Init(&nvic);
    
    TIM_Cmd(TIM1, ENABLE);
}

/**
  * @brief  延时计时，每ms自减1
  * @param  None
  * @retval None
  */
void delay_update()
{
    if(g_delay_time)
    {
        g_delay_time--;
    }
}

/**
  * @brief  毫秒级延时
  * @param  None
  * @retval None
  */
void delay_ms(uint32_t ms)
{
    g_delay_time = ms;
    while(g_delay_time)
    {
    
    }
}


void modbus_timer_config(uint32_t psc, uint32_t arr)
{
    TIM_TimeBaseInitTypeDef     tim_base;
    NVIC_InitTypeDef            nvic;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    tim_base.TIM_ClockDivision = TIM_CKD_DIV1;
    tim_base.TIM_Prescaler = psc - 1;
    tim_base.TIM_Period = arr - 1;
    tim_base.TIM_CounterMode = TIM_CounterMode_Up;
    tim_base.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM2, &tim_base);
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
    TIM_Cmd(TIM2, ENABLE);

    nvic.NVIC_IRQChannel = TIM2_IRQn;
    nvic.NVIC_IRQChannelCmd = ENABLE;
    nvic.NVIC_IRQChannelPreemptionPriority = 3;
    nvic.NVIC_IRQChannelSubPriority = 0;
    NVIC_Init(&nvic);
}


void ethernet_timer_config(uint32_t psc, uint32_t arr)
{
    TIM_TimeBaseInitTypeDef     tim_base;
    NVIC_InitTypeDef            nvic;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);
    tim_base.TIM_ClockDivision = TIM_CKD_DIV1;
    tim_base.TIM_Prescaler = psc - 1;
    tim_base.TIM_Period = arr - 1;
    tim_base.TIM_CounterMode = TIM_CounterMode_Up;
    tim_base.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM6, &tim_base);
    TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE);
    TIM_Cmd(TIM6, ENABLE);

    nvic.NVIC_IRQChannel = TIM6_DAC_IRQn;
    nvic.NVIC_IRQChannelCmd = ENABLE;
    nvic.NVIC_IRQChannelPreemptionPriority = 3;
    nvic.NVIC_IRQChannelSubPriority = 0;
    NVIC_Init(&nvic);
}
