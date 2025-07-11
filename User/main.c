#include "main.h"
#include "delay.h"
#include "led.h"
#include "timer.h"
#include "phy.h"
#include "lwip_comm.h"
#include "malloc.h"
#include "485.h"
#include "can.h"
#include "mb.h"
#include "FreeRTOS.h"
#include "task.h"

//stast_task
void start_task(void * pvParameters);
#define START_TASK_STACK_SIZE   128
#define START_TASK_PRIO         1
TaskHandle_t start_task_handler;

//task1
void task1(void * pvParameters);
#define TASK1_STACK_SIZE        128
#define TASK1_PRIO              2
TaskHandle_t task1_handler;

//task2
void task2(void * pvParameters);
#define TASK2_STACK_SIZE        128
#define TASK2_PRIO              3
TaskHandle_t task2_handler;

//task3
void task3(void * pvParameters);
#define TASK3_STACK_SIZE        128
#define TASK3_PRIO              4
TaskHandle_t task3_handler;


void start_task(void * pvParameters)
{
    taskENTER_CRITICAL();
    
    xTaskCreate((TaskFunction_t         ) task1,
                (char *                 ) "task1",
                (configSTACK_DEPTH_TYPE ) TASK1_STACK_SIZE,
                (void *                 ) NULL,
                (UBaseType_t            ) TASK1_PRIO,
                (TaskHandle_t *         ) &task1_handler );

    xTaskCreate((TaskFunction_t         ) task2,
                (char *                 ) "task2",
                (configSTACK_DEPTH_TYPE ) TASK2_STACK_SIZE,
                (void *                 ) NULL,
                (UBaseType_t            ) TASK2_PRIO,
                (TaskHandle_t *         ) &task2_handler );
                
    xTaskCreate((TaskFunction_t         ) task3,
                (char *                 ) "task3",
                (configSTACK_DEPTH_TYPE ) TASK3_STACK_SIZE,
                (void *                 ) NULL,
                (UBaseType_t            ) TASK3_PRIO,
                (TaskHandle_t *         ) &task3_handler );
                
    vTaskDelete(start_task_handler);
                
    taskEXIT_CRITICAL();
}

void task1 (void * pvParameters)
{
    while(1)
    {
        LED1_TOGGLE();
        vTaskDelay(500);
    }
}


void task2(void * pvParameters)
{
    while(1)
    {
        rs485_send_byte(0xAA);
        vTaskDelay(500);
    }
}

uint8_t data[8] = {1,2,3,4,5,6,7,8};
void task3(void * pvParameters)
{
    while(1)
    {
        can_1_send_data(&data[0], 8);
        vTaskDelay(500);
        can_2_send_data(&data[0], 8);
        vTaskDelay(500);
    }
}



int main()
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

    delay_init(168);
    
    my_led_config();
    
    rs485_config(115200);
    
    can_1_driver_config(6, CAN_SJW_1tq, CAN_BS1_7tq, CAN_BS2_6tq);
    can_2_driver_config(6, CAN_SJW_1tq, CAN_BS1_7tq, CAN_BS2_6tq);
    
    xTaskCreate((TaskFunction_t         ) start_task,
                (char *                 ) "start_task",
                (configSTACK_DEPTH_TYPE ) START_TASK_STACK_SIZE,
                (void *                 ) NULL,
                (UBaseType_t            ) START_TASK_PRIO,
                (TaskHandle_t *         ) &start_task_handler );
    vTaskStartScheduler();
    

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
        

    }
    
}






