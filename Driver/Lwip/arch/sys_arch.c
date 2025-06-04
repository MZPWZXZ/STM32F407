#include "sys_arch.h"




extern uint32_t g_lwip_localtime;
uint32_t sys_now(void)
{
    return g_lwip_localtime;
}

