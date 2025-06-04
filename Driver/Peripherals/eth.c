#include "eth.h"
#include "malloc.h"


ETH_DMADESCTypeDef *pg_dma_rx_dscr_tab = NULL;
ETH_DMADESCTypeDef *pg_dma_tx_dscr_tab = NULL;
uint8_t *pg_rx_buff = NULL;
uint8_t *pg_tx_buff = NULL;



void my_eth_mem_free(void)
{ 
	my_free(SRAM_IN, pg_dma_rx_dscr_tab);
    my_free(SRAM_IN, pg_dma_tx_dscr_tab);
    my_free(SRAM_IN, pg_rx_buff);		
    my_free(SRAM_IN, pg_tx_buff);		
}     



uint8_t my_eth_mem_malloc(void)
{ 
    uint8_t err = 0;
    
    if(!pg_dma_rx_dscr_tab && !pg_dma_tx_dscr_tab && !pg_rx_buff && !pg_tx_buff)
    {
        pg_dma_rx_dscr_tab = my_malloc(SRAM_IN, ETH_RXBUFNB * sizeof(ETH_DMADESCTypeDef));
        pg_dma_tx_dscr_tab = my_malloc(SRAM_IN, ETH_TXBUFNB * sizeof(ETH_DMADESCTypeDef));
        pg_rx_buff = my_malloc(SRAM_IN, ETH_RX_BUF_SIZE * ETH_RXBUFNB); //申请内存
        pg_tx_buff = my_malloc(SRAM_IN, ETH_TX_BUF_SIZE * ETH_TXBUFNB); //申请内存
        if(!pg_dma_rx_dscr_tab || !pg_dma_tx_dscr_tab || !pg_rx_buff || !pg_tx_buff)
        {
            my_eth_mem_free();
            err = 1; 
        }
    }
    else
    {
        my_eth_mem_free();
        err = 1;
    }

    return err; 
}



FrameTypeDef eth_rx_packet(void)
{ 
    uint32_t framelength = 0;
    FrameTypeDef frame = {0,0}; 
    
    //检查当前描述符,是否属于 ETHERNET DMA(设置的时候)/CPU(复位的时候)
    if((DMARxDescToGet->Status&ETH_DMARxDesc_OWN) != (uint32_t)RESET)
    {
        frame.length = ETH_ERROR; 
        if ((ETH->DMASR&ETH_DMASR_RBUS) != (uint32_t)RESET) 
        { 
            ETH->DMASR = ETH_DMASR_RBUS;//清除 ETH DMA 的 RBUS 位
            ETH->DMARPDR = 0;//恢复 DMA 接收
        }
        return frame;//错误,OWN 位被设置了
    } 
    
    if(((DMARxDescToGet->Status & ETH_DMARxDesc_ES) == (uint32_t)RESET) &&
    ((DMARxDescToGet->Status & ETH_DMARxDesc_LS) != (uint32_t)RESET) &&
    ((DMARxDescToGet->Status & ETH_DMARxDesc_FS) != (uint32_t)RESET)) 
    { 
        //得到接收包帧长度(不包含 4 字节 CRC)
        framelength = ((DMARxDescToGet->Status & ETH_DMARxDesc_FL) >> ETH_DMARxDesc_FrameLengthShift) - 4; 
        frame.buffer = DMARxDescToGet->Buffer1Addr;//得到包数据所在的位置
    }
    else 
    {
        framelength = ETH_ERROR;//错误
    }
    
    frame.length = framelength; 
    frame.descriptor = DMARxDescToGet; 
    //更新 ETH DMA 全局 Rx 描述符为下一个 Rx 描述符
    //为下一次 buffer 读取设置下一个 DMA Rx 描述符
    DMARxDescToGet = (ETH_DMADESCTypeDef*)(DMARxDescToGet->Buffer2NextDescAddr); 
    
    return frame; 
}


u32 eth_get_current_tx_buffer(void)
{  
  return DMATxDescToSet->Buffer1Addr;//·µ»ؔx bufferµؖ·  
}


uint8_t eth_tx_packet(uint16_t FrameLength)
{ 
    //检查当前描述符,是否属于 ETHERNET DMA(设置的时候)/CPU(复位的时候)
    if((DMATxDescToSet->Status & ETH_DMATxDesc_OWN) != (uint32_t)RESET)
    {
        return ETH_ERROR;//错误,OWN 位被设置了
    }
    //设置帧长度,bits[12:0]
    DMATxDescToSet->ControlBufferSize = (FrameLength & ETH_DMATxDesc_TBS1);
    //设置最后一个和第一个位段置位(1 个描述符传输一帧)
    DMATxDescToSet->Status |= ETH_DMATxDesc_LS | ETH_DMATxDesc_FS; 
    //设置 Tx 描述符的 OWN 位,buffer 重归 ETH DMA
    DMATxDescToSet->Status |= ETH_DMATxDesc_OWN;
    //当 Tx Buffer 不可用位(TBUS)被设置的时候,重置它.恢复传输
    if((ETH->DMASR & ETH_DMASR_TBUS) != (uint32_t)RESET)
    { 
        ETH->DMASR = ETH_DMASR_TBUS;//重置 ETH DMA TBUS 位
        ETH->DMATPDR = 0;//恢复 DMA 发送
    } 
    //更新 ETH DMA 全局 Tx 描述符为下一个 Tx 描述符
    //为下一次 buffer 发送设置下一个 DMA Tx 描述符
    DMATxDescToSet = (ETH_DMADESCTypeDef*)(DMATxDescToSet->Buffer2NextDescAddr);
    
    return ETH_SUCCESS; 
}



static void eth_gpio_config()
{
    GPIO_InitTypeDef    gpio;

    gpio.GPIO_Mode = GPIO_Mode_AF;
    gpio.GPIO_OType = GPIO_OType_PP;
    gpio.GPIO_PuPd = GPIO_PuPd_NOPULL;
    gpio.GPIO_Speed = GPIO_High_Speed;

    //RMII_RX_CLK
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    gpio.GPIO_Pin = GPIO_Pin_1;
    GPIO_Init(GPIOA, &gpio);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource1, GPIO_AF_ETH);

    //RMII_MDIO
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    gpio.GPIO_Pin = GPIO_Pin_2;
    GPIO_Init(GPIOA, &gpio);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_ETH);

    //RMII_CRX_DV
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    gpio.GPIO_Pin = GPIO_Pin_7;
    GPIO_Init(GPIOA, &gpio);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_ETH);

    //RMII_TX_EN
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    gpio.GPIO_Pin = GPIO_Pin_11;
    GPIO_Init(GPIOB, &gpio);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_ETH);

    //RMII_TXD0
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    gpio.GPIO_Pin = GPIO_Pin_12;
    GPIO_Init(GPIOB, &gpio);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource12, GPIO_AF_ETH);

    //RMII_TXD1
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    gpio.GPIO_Pin = GPIO_Pin_13;
    GPIO_Init(GPIOB, &gpio);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource13, GPIO_AF_ETH);

    //RMII_MDC
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
    gpio.GPIO_Pin = GPIO_Pin_1;
    GPIO_Init(GPIOC, &gpio);
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource1, GPIO_AF_ETH);

    //RMII_RXD0
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
    gpio.GPIO_Pin = GPIO_Pin_4;
    GPIO_Init(GPIOC, &gpio);
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource4, GPIO_AF_ETH);

    //RMII_RXD1
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
    gpio.GPIO_Pin = GPIO_Pin_5;
    GPIO_Init(GPIOC, &gpio);
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource5, GPIO_AF_ETH);

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
    SYSCFG_ETH_MediaInterfaceConfig(SYSCFG_ETH_MediaInterface_RMII);
}


static void eth_mac_dma_config(uint16_t phy_address)
{
    ETH_InitTypeDef     eth;

    /* Enable ETHERNET clock  */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_ETH_MAC | RCC_AHB1Periph_ETH_MAC_Tx | RCC_AHB1Periph_ETH_MAC_Rx, ENABLE);                                             
    ETH_DeInit();
    ETH_SoftwareReset();
    while (ETH_GetSoftwareResetStatus() == SET);
    ETH_StructInit(&eth);

    eth.ETH_AutoNegotiation = ETH_AutoNegotiation_Enable;
    //ETH_InitStructure.ETH_AutoNegotiation = ETH_AutoNegotiation_Disable; 
    //ETH_InitStructure.ETH_Speed = ETH_Speed_10M;
    //ETH_InitStructure.ETH_Mode = ETH_Mode_FullDuplex;   

    eth.ETH_LoopbackMode = ETH_LoopbackMode_Disable;
    eth.ETH_RetryTransmission = ETH_RetryTransmission_Disable;
    eth.ETH_AutomaticPadCRCStrip = ETH_AutomaticPadCRCStrip_Disable;
    eth.ETH_ReceiveAll = ETH_ReceiveAll_Disable;
    eth.ETH_BroadcastFramesReception = ETH_BroadcastFramesReception_Enable;
    eth.ETH_PromiscuousMode = ETH_PromiscuousMode_Disable;
    eth.ETH_MulticastFramesFilter = ETH_MulticastFramesFilter_Perfect;
    eth.ETH_UnicastFramesFilter = ETH_UnicastFramesFilter_Perfect;

#ifdef CHECKSUM_BY_HARDWARE
    eth.ETH_ChecksumOffload = ETH_ChecksumOffload_Enable;
#endif

    eth.ETH_DropTCPIPChecksumErrorFrame = ETH_DropTCPIPChecksumErrorFrame_Enable; 
    eth.ETH_ReceiveStoreForward = ETH_ReceiveStoreForward_Enable;         
    eth.ETH_TransmitStoreForward = ETH_TransmitStoreForward_Enable;     

    eth.ETH_ForwardErrorFrames = ETH_ForwardErrorFrames_Disable;       
    eth.ETH_ForwardUndersizedGoodFrames = ETH_ForwardUndersizedGoodFrames_Disable;   
    eth.ETH_SecondFrameOperate = ETH_SecondFrameOperate_Enable;
    eth.ETH_AddressAlignedBeats = ETH_AddressAlignedBeats_Enable;      
    eth.ETH_FixedBurst = ETH_FixedBurst_Enable;                
    eth.ETH_RxDMABurstLength = ETH_RxDMABurstLength_32Beat;          
    eth.ETH_TxDMABurstLength = ETH_TxDMABurstLength_32Beat;
    eth.ETH_DMAArbitration = ETH_DMAArbitration_RoundRobin_RxTx_2_1;
    ETH_Init(&eth, phy_address);
    
    ETH_DMAITConfig(ETH_DMA_IT_NIS | ETH_DMA_IT_R, ENABLE);
}


static void eth_nvic_config()
{
    NVIC_InitTypeDef    nvic;

    nvic.NVIC_IRQChannel = ETH_IRQn;
    nvic.NVIC_IRQChannelPreemptionPriority = 0;
    nvic.NVIC_IRQChannelSubPriority = 0;
    nvic.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvic);    
}


void my_eth_config(uint16_t phy_address)
{
    eth_gpio_config();
    eth_mac_dma_config(phy_address);
    eth_nvic_config();
}
