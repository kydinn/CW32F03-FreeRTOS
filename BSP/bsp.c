#include "clock.h"
#include "uart.h"

void bsp_init(void)
{
    clock_init(); // 初始化系统时钟
    uart_init(115200); // 初始化串口，用于打印调试信息
}
