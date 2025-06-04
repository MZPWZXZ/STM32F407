#ifndef __LWIP_COMM_H__
#define __LWIP_COMM_H__


#include "stm32f4xx.h"
#include "phy.h"


#define LWIP_MAX_DHCP_TRIES 4 /* DHCP 服务器最大重试次数 */


/*lwip 控制结构体 */
struct stru_lwip_dev{

    uint8_t mac[6];         /* MAC 地址 */
    
    uint8_t remoteip[4];    /* 远端主机 IP 地址 */
    
    uint8_t ip[4];          /* 本机 IP 地址 */
    
    uint8_t netmask[4];     /* 子网掩码 */
    
    uint8_t gateway[4];     /* 默认网关的 IP 地址 */
    
    uint8_t dhcpstatus;     /* dhcp 状态 */
                            /* 0, 未获取 DHCP 地址;*/
                            /* 1, 进入 DHCP 获取状态*/
                            /* 2, 成功获取 DHCP 地址*/
                            /* 0XFF,获取失败 */
};
extern struct stru_lwip_dev g_lwip_dev; /* lwip 控制结构体 */


extern uint32_t  memp_get_memorysize(void);	
extern uint8_t  *pg_memp_memory;				
extern uint8_t  *pg_ram_heap;	



uint8_t lwip_comm_mem_malloc(void);/* 内存堆 ram_heap 和内存池 memp_memory 的内存分配 */
void lwip_comm_mem_free(void); /* lwip 中 mem 和 memp 内存释放 */
void lwip_comm_default_ip_set(struct stru_lwip_dev *lwipx); /* lwip 默认 IP 设置 */
uint8_t lwip_comm_init(void);/* lwIP 初始化(lwIP 启动的时候使用) */
void lwip_pkt_handle(void); /* 当接收到数据后调用 */
void lwip_periodic_handle(void); /* lwip_periodic_handle */
void lwip_dhcp_process_handle(void); /* DHCP 处理任务 */


#endif





