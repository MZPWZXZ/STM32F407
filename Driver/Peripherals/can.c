#include "can.h"


CanRxMsg RxMessage;
/**
  * @brief  CAN1 中断服务函数	
  * @param  None
  * @retval None
  */
void CAN1_RX0_IRQHandler(void)
{
    CAN_Receive(CAN1, 0, &RxMessage);
}

/**
  * @brief  CAN2 中断服务函数	
  * @param  None
  * @retval None
  */
void CAN2_RX0_IRQHandler(void)
{
    CAN_Receive(CAN2, 0, &RxMessage);
}

/**
  * @brief  CAN 发送数据
  * @param  None
  * @retval None
  */
u8 can_1_send_data(u8* msg, u8 len)
{	
    u8 mbox;
    u16 i = 0;
    CanTxMsg tx_msg;
    
    tx_msg.StdId = 0x12;	        // 标准标识符为0
    tx_msg.ExtId = 0x12;	        // 设置扩展标示符（29位）
    tx_msg.IDE = CAN_Id_Standard;   // 使用扩展标识符
    tx_msg.RTR = CAN_RTR_Data;		// 消息类型为数据帧，一帧8位
    tx_msg.DLC = len;	            // 发送两帧信息
    
    for(i=0; i<len; i++)
    {
        tx_msg.Data[i] = msg[i];				 // 第一帧信息     
    }
         
    mbox = CAN_Transmit(CAN1, &tx_msg);   
    i=0;
    while(CAN_TransmitStatus(CAN1, mbox) == CAN_TxStatus_Failed)
    {
        i++;	
        if(i >= 0XFFF) //发送超时
        {
            return 1;
        }
    }
    
    return 0;		
}

/**
  * @brief  CAN 发送数据
  * @param  None
  * @retval None
  */
u8 can_2_send_data(u8* msg, u8 len)
{	
    u8 mbox;
    u16 i = 0;
    CanTxMsg tx_msg;
    
    tx_msg.StdId = 0x22;	        // 标准标识符为0
    tx_msg.ExtId = 0x22;	        // 设置扩展标示符（29位）
    tx_msg.IDE = CAN_Id_Standard;   // 使用扩展标识符
    tx_msg.RTR = CAN_RTR_Data;		// 消息类型为数据帧，一帧8位
    tx_msg.DLC = len;	            // 发送两帧信息
    
    for(i=0; i<len; i++)
    {
        tx_msg.Data[i] = msg[i];				 // 第一帧信息     
    }
         
    mbox = CAN_Transmit(CAN2, &tx_msg);   
    i=0;
    while(CAN_TransmitStatus(CAN2, mbox) == CAN_TxStatus_Failed)
    {
        i++;	
        if(i >= 0XFFF) //发送超时
        {
            return 1;
        }
    }
    
    return 0;		
}

/**
  * @brief  CAN1 驱动初始化
  * @param  None
  * @retval None
  */
void can_1_driver_config(u16 brp, u8 tsjw, u8 tbs1, u8 tbs2)
{
    GPIO_InitTypeDef        can_gpio; 
    CAN_InitTypeDef         can;
    CAN_FilterInitTypeDef   can_filter;
    NVIC_InitTypeDef        nvic;
    
    //使能相关时钟
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);//使能PORTA时钟	                   											 
    //初始化GPIO
    can_gpio.GPIO_Mode = GPIO_Mode_AF;//复用功能
    can_gpio.GPIO_OType = GPIO_OType_PP;//推挽输出
    can_gpio.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
    can_gpio.GPIO_PuPd = GPIO_PuPd_UP;//上拉
    
    //CAN RX
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource0, GPIO_AF_CAN1); //GPIOA11复用为CAN1
    can_gpio.GPIO_Pin = GPIO_Pin_0;
    GPIO_Init(GPIOD, &can_gpio);//初始化PA11
    
    //CAN TX
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource1, GPIO_AF_CAN1); //GPIOA12复用为CAN1
    can_gpio.GPIO_Pin = GPIO_Pin_1;
    GPIO_Init(GPIOD, &can_gpio);//初始化PA12
      
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);//使能CAN1时钟	
    //CAN单元设置
    can.CAN_TTCM = DISABLE;	//非时间触发通信模式   
    can.CAN_ABOM = DISABLE;	//软件自动离线管理	  
    can.CAN_AWUM = DISABLE; //睡眠模式通过软件唤醒(清除CAN->MCR的SLEEP位)
    can.CAN_NART = ENABLE;	//禁止报文自动传送 
    can.CAN_RFLM = DISABLE;	//报文不锁定,新的覆盖旧的  
    can.CAN_TXFP = DISABLE;	//优先级由报文标识符决定 
    can.CAN_Mode = CAN_Mode_Normal;	    //模式设置 
    can.CAN_Prescaler = brp;  //分频系数(Fdiv)为brp+1	
    can.CAN_SJW = tsjw;	    //重新同步跳跃宽度(Tsjw)为tsjw+1个时间单位 CAN_SJW_1tq~CAN_SJW_4tq
    can.CAN_BS1 = tbs1;     //Tbs1范围CAN_BS1_1tq ~CAN_BS1_16tq
    can.CAN_BS2 = tbs2;     //Tbs2范围CAN_BS2_1tq ~	CAN_BS2_8tq
    CAN_Init(CAN1, &can);   // 初始化CAN1 

    //配置过滤器
    can_filter.CAN_FilterNumber = 0;	                    //过滤器0
    can_filter.CAN_FilterMode = CAN_FilterMode_IdMask; 
    can_filter.CAN_FilterScale = CAN_FilterScale_32bit;   //32位 
    can_filter.CAN_FilterIdHigh = 0x0000;                 //32位ID
    can_filter.CAN_FilterIdLow = 0x0000;
    can_filter.CAN_FilterMaskIdHigh = 0x0000;             //32位MASK
    can_filter.CAN_FilterMaskIdLow = 0x0000;
    can_filter.CAN_FilterFIFOAssignment = CAN_Filter_FIFO0;//过滤器0关联到FIFO0
    can_filter.CAN_FilterActivation = ENABLE;             //激活过滤器0
    CAN_FilterInit(&can_filter);                        //滤波器初始化
        
    CAN_ITConfig(CAN1, CAN_IT_FMP0,ENABLE);//FIFO0消息挂号中断允许.		    
    nvic.NVIC_IRQChannel = CAN1_RX0_IRQn;
    nvic.NVIC_IRQChannelPreemptionPriority = 1;     // 主优先级为1
    nvic.NVIC_IRQChannelSubPriority = 0;            // 次优先级为0
    nvic.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvic);
}



/**
  * @brief  CAN2 驱动初始化
  * @param  None
  * @retval None
  */
void can_2_driver_config(u16 brp, u8 tsjw, u8 tbs1, u8 tbs2)
{
    GPIO_InitTypeDef        can_gpio; 
    CAN_InitTypeDef         can;
    CAN_FilterInitTypeDef   can_filter;
    NVIC_InitTypeDef        nvic;
    
    //使能相关时钟
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);//使能PORTA时钟	                   											 
    //初始化GPIO
    can_gpio.GPIO_Mode = GPIO_Mode_AF;//复用功能
    can_gpio.GPIO_OType = GPIO_OType_PP;//推挽输出
    can_gpio.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
    can_gpio.GPIO_PuPd = GPIO_PuPd_UP;//上拉
    //CAN RX
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource5, GPIO_AF_CAN2); //GPIOB5复用为CAN2
    can_gpio.GPIO_Pin = GPIO_Pin_5;
    GPIO_Init(GPIOB, &can_gpio);
    //CAN TX
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_CAN2); //GPIOB6复用为CAN2
    can_gpio.GPIO_Pin = GPIO_Pin_6;
    GPIO_Init(GPIOB, &can_gpio);
      
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN2, ENABLE);//使能CAN1时钟	
    //CAN单元设置
    can.CAN_TTCM = DISABLE;	//非时间触发通信模式   
    can.CAN_ABOM = DISABLE;	//软件自动离线管理	  
    can.CAN_AWUM = DISABLE; //睡眠模式通过软件唤醒(清除CAN->MCR的SLEEP位)
    can.CAN_NART = ENABLE;	//禁止报文自动传送 
    can.CAN_RFLM = DISABLE;	//报文不锁定,新的覆盖旧的  
    can.CAN_TXFP = DISABLE;	//优先级由报文标识符决定 
    can.CAN_Mode = CAN_Mode_Normal;	    //模式设置 
    can.CAN_Prescaler = brp;  //分频系数(Fdiv)为brp+1	
    can.CAN_SJW = tsjw;	    //重新同步跳跃宽度(Tsjw)为tsjw+1个时间单位 CAN_SJW_1tq~CAN_SJW_4tq
    can.CAN_BS1 = tbs1;     //Tbs1范围CAN_BS1_1tq ~CAN_BS1_16tq
    can.CAN_BS2 = tbs2;     //Tbs2范围CAN_BS2_1tq ~	CAN_BS2_8tq
    CAN_Init(CAN2, &can);   // 初始化CAN1 

    //配置过滤器
    can_filter.CAN_FilterNumber = 0;	                    //过滤器0
    can_filter.CAN_FilterMode = CAN_FilterMode_IdMask; 
    can_filter.CAN_FilterScale = CAN_FilterScale_32bit;   //32位 
    can_filter.CAN_FilterIdHigh = 0x0000;                 //32位ID
    can_filter.CAN_FilterIdLow = 0x0000;
    can_filter.CAN_FilterMaskIdHigh = 0x0000;             //32位MASK
    can_filter.CAN_FilterMaskIdLow = 0x0000;
    can_filter.CAN_FilterFIFOAssignment = CAN_Filter_FIFO0;//过滤器0关联到FIFO0
    can_filter.CAN_FilterActivation = ENABLE;             //激活过滤器0
    CAN_FilterInit(&can_filter);                        //滤波器初始化
        
    CAN_ITConfig(CAN2, CAN_IT_FMP0, ENABLE);//FIFO0消息挂号中断允许.		    
    nvic.NVIC_IRQChannel = CAN2_RX0_IRQn;
    nvic.NVIC_IRQChannelPreemptionPriority = 1;     // 主优先级为1
    nvic.NVIC_IRQChannelSubPriority = 0;            // 次优先级为0
    nvic.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvic);
}


