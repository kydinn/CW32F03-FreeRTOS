// FreeRTOS 头文件
#include "FreeRTOS.h"
#include "task.h"

// 开发板硬件 bsp 头文件
#include "bsp.h"
#include "uart.h"

#define MAX_TASK 2

static TaskHandle_t AppTask_Handle[MAX_TASK]; // 任务句柄
static BaseType_t xReturn[MAX_TASK];          // 创建信息返回值

static void AppTask1(void *parameter)
{
    char data[] = "task1 running";
    while (1)
    {
        uart_send((uint8_t *)data, sizeof(data));
        vTaskDelay(300);
    }
}

static void AppTask2(void *parameter)
{
    char data[] = "task2 running";
    while (1)
    {
        uart_send((uint8_t *)data, sizeof(data));
        vTaskDelay(500);
    }
}

int main(void)
{
    /* 开发板硬件初始化 */
    bsp_init();

    uint8_t msg[] = "hello world";
    uart_send(msg, sizeof(msg));

    /* 创建两个任务 */
    xReturn[0] = xTaskCreate((TaskFunction_t)AppTask1,            // 任务入口函数
                             (const char *)"AppTask1",            // 任务名字
                             (uint16_t)128,                       // 任务栈大小
                             (void *)NULL,                        // 任务入口函数参数
                             (UBaseType_t)2,                      // 任务的优先级 (0~最大值-1 有效)
                             (TaskHandle_t *)&AppTask_Handle[0]); // 任务控制块指针

    xReturn[1] = xTaskCreate((TaskFunction_t)AppTask2,            // 任务入口函数
                             (const char *)"AppTask2",            // 任务名字
                             (uint16_t)128,                       // 任务栈大小
                             (void *)NULL,                        // 任务入口函数参数
                             (UBaseType_t)2,                      // 任务的优先级 (0~最大值-1 有效)
                             (TaskHandle_t *)&AppTask_Handle[1]); // 任务控制块指针

    /* 启动任务调度 */
    for (int i = 0; i < MAX_TASK; i++)
    {
        if (xReturn[i] != pdPASS)
        {
            return -1; // 如果有创建失败的任务，就结束程序
        }
    }
    vTaskStartScheduler(); // 启动任务，开启调度

    while (1)
        ; // 正常不会执行到这里
}
