#include "phy.h"
#include "stdio.h"
#include "main.h"


uint16_t g_phy_bcr = 0;
uint16_t g_phy_bsr = 0;



uint16_t read_phy_reg(uint16_t phy_addr, uint16_t reg)
{
    uint16_t reg_val = 0;

    reg_val = ETH_ReadPHYRegister(phy_addr, reg);

    return reg_val;
}



uint16_t write_phy_reg(uint16_t phy_addr, uint16_t reg, uint16_t reg_val)
{
    uint16_t err = 0;

    err = ETH_WritePHYRegister(phy_addr, reg, reg_val);

    return err;
}



uint8_t get_phy_link_status()
{
    uint8_t link_status = 0;

    link_status = (read_phy_reg(DP83848C_ADDR, PHY_BSR) & 0x0040);
    
    return link_status;
}






uint8_t dp83848c_config()
{
    uint8_t err = 0;

    my_eth_config(DP83848C_ADDR);

    g_phy_bcr = read_phy_reg(DP83848C_ADDR, PHY_BCR);
    g_phy_bsr = read_phy_reg(DP83848C_ADDR, PHY_BSR);

    if(get_phy_link_status())
    {
        err = 1;
    }

    return err;
}


