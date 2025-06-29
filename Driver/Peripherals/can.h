#ifndef __CAN_H__
#define __CAN_H__


#include "stm32f4xx.h"



u8 can_2_send_data(u8* msg, u8 len);
u8 can_1_send_data(u8* msg, u8 len);
void can_2_driver_config(u16 brp, u8 tsjw, u8 tbs1, u8 tbs2);
void can_1_driver_config(u16 brp, u8 tsjw, u8 tbs1, u8 tbs2);





#endif
