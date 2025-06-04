#include "main.h"
#include "systick.h"
#include "led.h"
#include "timer.h"
#include "phy.h"
#include "lwip_comm.h"
#include "malloc.h"
#include "485.h"
#include "mb.h"



int main()
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

    delay_timer_config(168, 1000);
    
    my_led_config();
    
    eMBInit(MB_RTU, 1, 0, 9600, MB_PAR_NONE);
    eMBEnable();
  
//    my_mem_init(SRAM_IN);

//    ethernet_timer_config(84, 10000);//84000000/84/10000 = 100Hz  10ms

//    while(lwip_comm_init() != 0)
//    {
//        
//    }

//#if LWIP_DHCP
//    while((lwipdev.dhcpstatus != 2)&&(lwipdev.dhcpstatus != 0XFF))/* 等待 DHCP 获取成功/超时溢出 */
//    {
//        lwip_periodic_handle();
//    }
//#endif
//    
//    while(1)
//    {
//        lwip_periodic_handle(); /* LWIP 轮询任务 */

//    }

    while(1)
    {
        eMBPoll();
    }
}






