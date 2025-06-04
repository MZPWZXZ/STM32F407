#include "lwip_comm.h"
#include "malloc.h"
#include "netif/etharp.h"
#include "phy.h"
#include "lwip/init.h"
#include "ethernetif.h"
#include "lwip/dhcp.h"



struct stru_lwip_dev g_lwip_dev; /* lwip 控制结构体 */					
struct netif g_lwip_netif;				


uint32_t  memp_get_memorysize(void);	
uint8_t  *pg_memp_memory;				
uint8_t  *pg_ram_heap;		


uint32_t g_tcp_timer=0;			
uint32_t g_arp_timer=0;			
uint32_t g_lwip_localtime;		


#if LWIP_DHCP
    uint32_t g_dhcp_fine_time=0;	
    uint32_t g_dhcp_coarse_time=0;	
#endif


void lwip_comm_mem_free(void)
{ 	
	my_free(SRAM_IN, pg_memp_memory);
	my_free(SRAM_IN, pg_ram_heap);
}


uint8_t lwip_comm_mem_malloc(void)
{
    uint32_t mempsize;
    uint32_t ramheapsize; 
    mempsize = memp_get_memorysize(); //得到 memp_memory 数组大小
    pg_memp_memory = my_malloc(SRAM_IN, mempsize); //为 memp_memory 申请内存
    ramheapsize=LWIP_MEM_ALIGN_SIZE(MEM_SIZE)+2*\
    LWIP_MEM_ALIGN_SIZE(4*3)+MEM_ALIGNMENT;//得到 ram heap 大小
    pg_ram_heap = my_malloc(SRAM_IN, ramheapsize); //为 ram_heap 申请内存
    
    if(!pg_memp_memory || !pg_ram_heap)//有申请失败的
    {
        lwip_comm_mem_free();
        return 1;
    }
    
    return 0;
}



uint8_t lwip_comm_init(void)
{

    /* 调用 netif_add()函数时的返回值,用于判断网络初始化是否成功 */
    struct netif *netif_init_flag;
    ip_addr_t ipaddr; /* ip 地址 */
    ip_addr_t netmask; /* 子网掩码 */
    ip_addr_t gw; /* 默认网关 */
    if (my_eth_mem_malloc())
    {
        return 1; /* 内存申请失败*/
    }
    lwip_comm_default_ip_set(&g_lwip_dev); /* 设置默认 IP 等信息 */
    while (!dp83848c_config()) /* 初始化以太网芯片,如果失败的话就重试 5 次 */
    {

    }
    lwip_init(); /* 初始化 LWIP 内核 */
    
#if LWIP_DHCP /* 使用动态 IP */
    ipaddr.addr = 0;
    netmask.addr = 0;
    gw.addr = 0;
#else /* 使用静态 IP */
    IP4_ADDR(&ipaddr, g_lwip_dev.ip[0], g_lwip_dev.ip[1],
    g_lwip_dev.ip[2], g_lwip_dev.ip[3]);
    IP4_ADDR(&netmask, g_lwip_dev.netmask[0], g_lwip_dev.netmask[1],
    g_lwip_dev.netmask[2], g_lwip_dev.netmask[3]);
    IP4_ADDR(&gw, g_lwip_dev.gateway[0], g_lwip_dev.gateway[1],
    g_lwip_dev.gateway[2], g_lwip_dev.gateway[3]);
#endif /* 向网卡列表中添加一个网口 */
    
    netif_init_flag = netif_add(&g_lwip_netif, (const ip_addr_t *)&ipaddr, (const ip_addr_t *)&netmask, (const ip_addr_t *)&gw, NULL, &ethernetif_init, &ethernet_input);
   
    if (netif_init_flag == NULL)
    {
        return 4; /* 网卡添加失败 */
    }
    else /* 网口添加成功后,设置 netif 为默认值,并且打开 netif 网口 */
    { 
        netif_set_default(&g_lwip_netif); /* 设置 netif 为默认网口 */
        if (netif_is_link_up(&g_lwip_netif))
        {
            netif_set_up(&g_lwip_netif); /* 打开 netif 网口 */
        }
        else
        {
            netif_set_down(&g_lwip_netif);
        }
    }
    
#if LWIP_DHCP /* 如果使用 DHCP 的话 */
    g_lwip_dev.dhcpstatus = 0; /* DHCP 标记为 0 */
    dhcp_start(&g_lwip_netif); /* 开启 DHCP 服务 */
#endif
    
    return 0; /* 操作 OK. */
}



void lwip_comm_default_ip_set(struct stru_lwip_dev *lwipx)
{
    /* 默认远端 IP 为:192.168.1.134 */
    lwipx->remoteip[0] = 192;
    lwipx->remoteip[1] = 168;
    lwipx->remoteip[2] = 8;
    lwipx->remoteip[3] = 88;

    /* MAC 地址设置*/
    lwipx->mac[0] = 0xB8; 
    lwipx->mac[1] = 0xAE;
    lwipx->mac[2] = 0x1D;
    lwipx->mac[3] = 0x00;
    lwipx->mac[4] = 0x07;
    lwipx->mac[5] = 0x00;

    /* 默认本地 IP 为:192.168.1.30 */
    lwipx->ip[0] = 192;
    lwipx->ip[1] = 168;
    lwipx->ip[2] = 8;
    lwipx->ip[3] = 100;
    /* 默认子网掩码:255.255.255.0 */
    lwipx->netmask[0] = 255;
    lwipx->netmask[1] = 255;
    lwipx->netmask[2] = 255;
    lwipx->netmask[3] = 0;
 
    /* 默认网关:192.168.1.1 */
    lwipx->gateway[0] = 192;
    lwipx->gateway[1] = 168;
    lwipx->gateway[2] = 8;
    lwipx->gateway[3] = 1;
    lwipx->dhcpstatus = 0; /* 没有 DHCP */
}


void lwip_pkt_handle(void)
{
 //从网络缓冲区中读取接收到的数据包并将其发送给 LWIP 处理
   ethernetif_input(&g_lwip_netif);
} 


void lwip_periodic_handle()
{
#if LWIP_DHCP /* 如果使用 DHCP */
    /* 每 500ms 调用一次 dhcp_fine_tmr() */
    /* DHCP_FINE_TIMER_MSECS(500)单位 */
    if (sys_now() - g_dhcp_fine_time >= DHCP_FINE_TIMER_MSECS) 
    {
        g_dhcp_fine_time = sys_now();
        dhcp_fine_tmr();
        
        if ((g_lwip_dev.dhcpstatus != 2) && (g_lwip_dev.dhcpstatus != 0XFF))
        {
            lwip_dhcp_process_handle(); /* DHCP 处理 */
        }
    }
    /* 每 60s 执行一次 DHCP 粗糙处理 */
    if (sys_now() - g_dhcp_coarse_time >= DHCP_COARSE_TIMER_MSECS)
    {
        g_dhcp_coarse_time = sys_now();
        dhcp_coarse_tmr();
    }
#endif
}



/* 如果使能 DHCP */
#if LWIP_DHCP
struct dhcp gdhcp;
/**
* @breif DHCP 处理任务
* @param 无
* @retval 无
*/
void lwip_dhcp_process_handle(void)
{
    uint32_t ip = 0, netmask = 0, gw = 0;
    
    switch (g_lwip_dev.dhcpstatus)
    {
        case 0: /* 开启 DHCP */
        {
            dhcp_start(&g_lwip_netif);
            g_lwip_dev.dhcpstatus = 1; /* 等待通过 DHCP 获取到的地址 */
            printf("正在查找 DHCP 服务器,请稍等...........\r\n");
        }
        break;
    
        case 1: /* 等待获取到 IP 地址 */
        {
            ip = g_lwip_netif.ip_addr.addr; /* 读取新 IP 地址 */
            netmask = g_lwip_netif.netmask.addr; /* 读取子网掩码 */
            gw = g_lwip_netif.gw.addr; /* 读取默认网关 */
            
            if (ip != 0) /*正确获取到 IP 地址的时候*/
            {
                g_lwip_dev.dhcpstatus = 2; /* DHCP 成功 */

                /* 解析出通过 DHCP 获取到的 IP 地址 */
                g_lwip_dev.ip[3] = (uint8_t)(ip >> 24);
                g_lwip_dev.ip[2] = (uint8_t)(ip >> 16);
                g_lwip_dev.ip[1] = (uint8_t)(ip >> 8);
                g_lwip_dev.ip[0] = (uint8_t)(ip);

                /* 解析通过 DHCP 获取到的子网掩码地址 */
                g_lwip_dev.netmask[3] = (uint8_t)(netmask >> 24);
                g_lwip_dev.netmask[2] = (uint8_t)(netmask >> 16);
                g_lwip_dev.netmask[1] = (uint8_t)(netmask >> 8);
                g_lwip_dev.netmask[0] = (uint8_t)(netmask);

            }/* 通过 DHCP 服务获取 IP 地址失败,且超过最大尝试次数 */
            else if (gdhcp.tries > LWIP_MAX_DHCP_TRIES) 
            {
                g_lwip_dev.dhcpstatus = 0XFF; /* DHCP 超时失败. */
                /* 使用静态 IP 地址 */
                IP4_ADDR(&(g_lwip_netif.ip_addr), g_lwip_dev.ip[0],
                g_lwip_dev.ip[1],
                g_lwip_dev.ip[2],
                g_lwip_dev.ip[3]);
                IP4_ADDR(&(g_lwip_netif.netmask), g_lwip_dev.netmask[0],
                g_lwip_dev.netmask[1],
                g_lwip_dev.netmask[2],
                g_lwip_dev.netmask[3]);
                IP4_ADDR(&(g_lwip_netif.gw), g_lwip_dev.gateway[0],
                g_lwip_dev.gateway[1],
                g_lwip_dev.gateway[2],
                g_lwip_dev.gateway[3]);
            }
        }
        break;
        
        default :
        break;
    }
}
#endif
