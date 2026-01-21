#include "cw32f030_rcc.h"

#include "FreeRTOS.h"
#include "task.h"

extern void xPortSysTickHandler(void);

/**
 * @brief This function handles System tick timer.
 */
void SysTick_Handler(void)
{
#if (INCLUDE_xTaskGetSchedulerState == 1)
	if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
	{
#endif
		xPortSysTickHandler();
#if (INCLUDE_xTaskGetSchedulerState == 1)
	}
#endif
}

void clock_init(void)
{
	// 时钟初始化
	__RCC_FLASH_CLK_ENABLE();		   // 打开 FLASH 时钟
	RCC_HSI_Enable(RCC_HSIOSC_DIV2);   // 设置系统时钟 HSI = 24M
	RCC_HCLKPRS_Config(RCC_HCLK_DIV1); // HCLK = HSI
	RCC_PCLKPRS_Config(RCC_PCLK_DIV1); // PCLK = HSI
	SystemInit();
	RCC_SYSCLKSRC_Config(RCC_SYSCLKSRC_HSI);
}
