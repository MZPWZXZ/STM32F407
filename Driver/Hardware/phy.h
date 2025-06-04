#ifndef __DP83848C_H__
#define __DP83848C_H__


#include "stm32f4xx.h"
#include "stm32f4x7_eth.h"
#include "eth.h"

#define     DP83848C_ADDR       ((uint16_t)0x0001)


uint8_t dp83848c_config(void);


#endif
