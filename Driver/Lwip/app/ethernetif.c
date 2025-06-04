#include "ethernetif.h"
#include "stdio.h"
#include "string.h"
#include "malloc.h"
#include "lwip_comm.h"
#include "netif/etharp.h"

static err_t low_level_init(struct netif *netif);
static struct pbuf * low_level_input(struct netif *netif);
static err_t low_level_output(struct netif *netif, struct pbuf *p);

err_t ethernetif_init(struct netif *netif)
{
    LWIP_ASSERT("netif!=NULL",(netif!=NULL));
#if LWIP_NETIF_HOSTNAME //LWIP_NETIF_HOSTNAME 
netif->hostname="lwip"; //初始化名称
#endif 
netif->name[0]=IFNAME0; //初始化变量 netif 的 name 字段
netif->name[1]=IFNAME1; //在文件外定义这里不用关心具体值
netif->output=etharp_output;//IP 层发送数据包函数
netif->linkoutput=low_level_output;//ARP 模块发送数据包函数
low_level_init(netif); //底层硬件初始化函数
return ERR_OK;
}



static err_t low_level_init(struct netif *netif)
{
#ifdef CHECKSUM_BY_HARDWARE
    int i; 
#endif
    netif->hwaddr_len = ETHARP_HWADDR_LEN; /*设置 MAC 地址长度,为 6 个字节*/
    /*初始化 MAC 地址,设置什么地址由用户自己设置,但是不能与网络中其他设备 MAC 地址重复*/
    netif->hwaddr[0]= g_lwip_dev.mac[0];
    netif->hwaddr[1]= g_lwip_dev.mac[1];
    netif->hwaddr[2]= g_lwip_dev.mac[2];
    netif->hwaddr[3]= g_lwip_dev.mac[3]; 
    netif->hwaddr[4]= g_lwip_dev.mac[4];
    netif->hwaddr[5]= g_lwip_dev.mac[5];

    netif->mtu=1500; /* 最大允许传输单元,允许该网卡广播和 ARP 功能 */
    /* 网卡状态信息标志位，是很重要的控制字段，它包括网卡功能使能、广播*/
    /* 使能、 ARP 使能等等重要控制位*//*广播 ARP 协议 链接检测*/
    netif->flags = NETIF_FLAG_BROADCAST|NETIF_FLAG_ETHARP|NETIF_FLAG_LINK_UP; 

    /*初始化发送描述符*/
    ETH_MACAddressConfig(ETH_MAC_Address0,netif->hwaddr);
    ETH_DMATxDescChainInit(pg_dma_tx_dscr_tab, pg_tx_buff, ETH_TXBUFNB);
    ETH_DMARxDescChainInit(pg_dma_rx_dscr_tab, pg_rx_buff, ETH_RXBUFNB);
    
#ifdef CHECKSUM_BY_HARDWARE //使用硬件帧校验
    for(i=0;i<ETH_TXBUFNB;i++)
    {
        //使能 TCP,UDP 和 ICMP 的发送帧校验,TCP,UDP 和 ICMP 的接收帧校验在 DMA 中配置了
        ETH_DMATxDescChecksumInsertionConfig(&DMATxDscrTab[i],\
        ETH_DMATxDesc_ChecksumTCPUDPICMPFull);
    }
#endif

    ETH_Start(); //开启 MAC 和 DMA 
    return ERR_OK;    
}



static err_t low_level_output(struct netif *netif, struct pbuf *p)
{
    u8 res;
    struct pbuf *q;
    int l = 0;
    //获取当前要发送的 DMA 描述符中的缓冲区地址
    u8 *buffer=(u8 *)eth_get_current_tx_buffer(); 
    for(q=p;q!=NULL;q=q->next) 
    {
    memcpy((u8_t*)&buffer[l], q->payload, q->len);
    l=l+q->len;
    } 
    res=eth_tx_packet(l); //调用 ETH_Tx_Packet 函数发送数据
    if(res==ETH_ERROR)return ERR_MEM;//返回错误状态
    return ERR_OK;
}


//网卡接收数据(lwip 直接调用)
//netif:网卡结构体指针
//返回值:ERR_OK,发送正常
// ERR_MEM,发送失败
err_t ethernetif_input(struct netif *netif)
{
err_t err;
struct pbuf *p;
p=low_level_input(netif); //调用 low_level_input 函数接收数据
if(p==NULL) return ERR_MEM;
err=netif->input(p, netif);//调用 netif 结构体中的 input 字段(一个函数)来处理数据包
if(err!=ERR_OK)
{
LWIP_DEBUGF(NETIF_DEBUG,("ethernetif_input: IP input error\n"));
pbuf_free(p);
p = NULL;
} 
return err;
}



static struct pbuf * low_level_input(struct netif *netif)
{ 
 struct pbuf *p, *q;
u16_t len;
int l =0;
FrameTypeDef frame;
u8 *buffer;
p = NULL;
frame=eth_rx_packet();
len=frame.length;//得到包大小
buffer=(u8 *)frame.buffer;//得到包数据地址
p=pbuf_alloc(PBUF_RAW,len,PBUF_POOL);//pbufs 内存池分配 pbuf
if(p!=NULL)
{for(q=p;q!=NULL;q=q->next)
{
memcpy((u8_t*)q->payload,(u8_t*)&buffer[l], q->len);
l=l+q->len;
} 
}
//设置 Rx 描述符 OWN 位,buffer 重归 ETH DMA 
frame.descriptor->Status=ETH_DMARxDesc_OWN;
//当 Rx Buffer 不可用位(RBUS)被设置的时候,重置它.恢复传输
if((ETH->DMASR&ETH_DMASR_RBUS)!=(u32)RESET)
{ 
ETH->DMASR=ETH_DMASR_RBUS;//重置 ETH DMA RBUS 位
ETH->DMARPDR=0;//恢复 DMA 接收
}
return p;
}

