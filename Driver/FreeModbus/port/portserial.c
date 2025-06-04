/*
 * FreeModbus Libary: MSP430 Port
 * Copyright (C) 2006 Christian Walter <wolti@sil.at>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * File: $Id$
 */

/* ----------------------- Platform includes --------------------------------*/
#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"
#include "485.h"

/* ----------------------- Defines ------------------------------------------*/
#define U0_CHAR                 ( 0x10 )        /* Data 0:7-bits / 1:8-bits */

#define DEBUG_PERFORMANCE       ( 1 )

#if DEBUG_PERFORMANCE == 1
#define DEBUG_PIN_RX            ( 0 )
#define DEBUG_PIN_TX            ( 1 )
#define DEBUG_PORT_DIR          ( P1DIR )
#define DEBUG_PORT_OUT          ( P1OUT )
#define DEBUG_INIT( )           \
  do \
  { \
    DEBUG_PORT_DIR |= ( 1 << DEBUG_PIN_RX ) | ( 1 << DEBUG_PIN_TX ); \
    DEBUG_PORT_OUT &= ~( ( 1 << DEBUG_PIN_RX ) | ( 1 << DEBUG_PIN_TX ) ); \
  } while( 0 ); 
#define DEBUG_TOGGLE_RX( ) DEBUG_PORT_OUT ^= ( 1 << DEBUG_PIN_RX )
#define DEBUG_TOGGLE_TX( ) DEBUG_PORT_OUT ^= ( 1 << DEBUG_PIN_TX )

#else

#define DEBUG_INIT( )
#define DEBUG_TOGGLE_RX( )
#define DEBUG_TOGGLE_TX( )
#endif

/* ----------------------- Static variables ---------------------------------*/
UCHAR           ucGIEWasEnabled = FALSE;
UCHAR           ucCriticalNesting = 0x00;

/* ----------------------- Start implementation -----------------------------*/
void
vMBPortSerialEnable( BOOL xRxEnable, BOOL xTxEnable )
{
    if(xRxEnable)
        USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
    else
        USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);
    
    if(xTxEnable)
        USART_ITConfig(USART2, USART_IT_TC, ENABLE);
    else
        USART_ITConfig(USART2, USART_IT_TC, DISABLE);
}

BOOL
xMBPortSerialInit( UCHAR ucPort, ULONG ulBaudRate, UCHAR ucDataBits, eMBParity eParity )
{
    BOOL            bInitialized = TRUE;
    
    rs485_config(ulBaudRate);
    
    return bInitialized;
}

BOOL
xMBPortSerialPutByte( CHAR ucByte )
{
//    rs485_send_byte(ucByte);
    USART_SendData(USART2, ucByte);
    return TRUE;
}

BOOL
xMBPortSerialGetByte( CHAR * pucByte )
{
    *pucByte = (uint8_t)USART_ReceiveData(USART2);
    return TRUE;
}

void
prvvMBSerialRXIRQHandler( void ) 
{
    pxMBFrameCBByteReceived(  );
}

void
prvvMBSerialTXIRQHandler( void ) 
{
    pxMBFrameCBTransmitterEmpty(  );
}

/**
  * @brief  
  * @param  None
  * @retval None
  */
void USART2_IRQHandler(void)  
{
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
	{	
		prvvMBSerialRXIRQHandler();
        USART_ClearITPendingBit(USART2, USART_IT_RXNE);
	}
    
    if(USART_GetITStatus(USART2, USART_IT_ORE) != RESET)
	{	
        prvvMBSerialRXIRQHandler();
        USART_ClearITPendingBit(USART2, USART_IT_ORE);
	}
    
    if(USART_GetITStatus(USART2, USART_IT_TC) != RESET)
	{	
		prvvMBSerialTXIRQHandler();
        USART_ClearITPendingBit(USART2, USART_IT_TC);
	}
}
