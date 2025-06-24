#ifndef __MAIN_H__
#define __MAIN_H__


#include "stm32f4xx.h"
#include "stdio.h"


#define MY_ASSERT_PARAM(expr) ((expr) ? (void)0 : my_assert_failed((uint8_t *)__FILE__, __LINE__))


void my_assert_failed(uint8_t* file, uint32_t line);



#endif

