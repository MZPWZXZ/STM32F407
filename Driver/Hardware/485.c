#include "485.h"
#include "stdio.h"
#include "string.h"



void rs485_config(uint32_t baud)
{
    GPIO_InitTypeDef    GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;    
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource5, GPIO_AF_USART2);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5; 
	GPIO_Init(GPIOD, &GPIO_InitStructure);
    
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource6, GPIO_AF_USART2);        
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;        
	GPIO_Init(GPIOD, &GPIO_InitStructure);                  
	
    USART_InitTypeDef   USART_InitStructure;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2 , ENABLE);
	USART_InitStructure.USART_BaudRate = baud; 
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No; 
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART2,&USART_InitStructure); 
//    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);   
//    USART_ClearITPendingBit(USART2, USART_IT_TC);

    NVIC_InitTypeDef   NVIC_InitStructure;									
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;	  
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;	
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	USART_Cmd(USART2, ENABLE);
    
    rs485_send_byte(0xFF);
}


void rs485_send_byte(uint8_t byte)
{
    USART2->DR = byte;
    while((USART2->SR & 0X40)==0);	
}



void rs485_send_data(uint8_t *data, uint16_t len)
{
    unsigned int i = 0;

	for(i = 0;i < len; i++)
	{			
		USART2->DR = data[i];
		while((USART2->SR & 0X40)==0);	
	}
}
