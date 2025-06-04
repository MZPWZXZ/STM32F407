#ifndef __ETHERNETIF_H__
#define __ETHERNETIF_H__


#include "stm32f4x7_eth.h"
#include "lwip/err.h"
#include "lwip/netif.h"



#define     IFNAME0     's'
#define     IFNAME1     't'

static err_t low_level_init(struct netif *netif);
err_t ethernetif_init(struct netif *netif); /* 网卡初始化函数 */
err_t  ethernetif_input(struct netif *netif); /* 数据包输入函数 */
u32_t sys_now(void);


#endif
