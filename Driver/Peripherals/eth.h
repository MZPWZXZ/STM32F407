#ifndef __MY_ETH_H__
#define __MY_ETH_H__


#include "stm32f4xx.h"
#include "stm32f4x7_eth.h"



extern ETH_DMADESCTypeDef *pg_dma_rx_dscr_tab;
extern ETH_DMADESCTypeDef *pg_dma_tx_dscr_tab;
extern uint8_t *pg_rx_buff;
extern uint8_t *pg_tx_buff;


extern ETH_DMADESCTypeDef  *DMATxDescToSet;
extern ETH_DMADESCTypeDef  *DMARxDescToGet;



void my_eth_config(uint16_t phy_address);
void my_eth_mem_free(void);
uint8_t my_eth_mem_malloc(void);
uint32_t eth_get_current_tx_buffer(void);
FrameTypeDef eth_rx_packet(void);
uint8_t eth_tx_packet(uint16_t FrameLength);
    

#endif
