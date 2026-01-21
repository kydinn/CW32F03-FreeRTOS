#include "cw32f030_gpio.h"
#include "cw32f030_rcc.h"
#include "cw32f030_uart.h"

#include "uart.h"

static volatile unsigned char g_uart_recv_buf[UART_RECVBUF_SIZE] = { 0 }; // 接收数据缓冲区
static volatile unsigned int g_index_write = 0;
static volatile unsigned int g_index_read = 0;

/******************************************************************************/

/**
 * uart 接收一个 byte
 * @param
 *      data : 数据接收指针
 * @return
 *      成功 : 返回 1，表示接收到 1byte 数据
 *      失败 : 返回 0，表示数据缓冲区为空
*/
char uart_recv_byte(unsigned char *ch)
{
	if (g_index_read == g_index_write) {
		return 0;
	}

	*ch = g_uart_recv_buf[g_index_read];

	g_index_read++;
	if (g_index_read == UART_RECVBUF_SIZE) {
		g_index_read = 0;
	}

	return 1;
}

void uart_send_byte(unsigned char ch)
{
	USART_SendData_8bit(CW_UART2, ch);

	while (USART_GetFlagStatus(CW_UART2, USART_FLAG_TXE) == RESET)
		;
	while (USART_GetFlagStatus(CW_UART2, USART_FLAG_TXBUSY) == SET)
		;
}

/******************************************************************************/

void uart_recv(uint8_t *buf, uint16_t size)
{
	while (size--) {
		while (!uart_recv_byte(buf))
			;
		buf++;
	}
}

/**
 * @brief 带超时的串口接收函数
 * @param buf 接收缓冲区
 * @param size 要接收的字节数
 * @param timeout_ms 超时时间（毫秒），0 表示不等待
 * @return 1:成功接收所有数据，0:超时
 */
uint8_t uart_recv_timeout(uint8_t *buf, uint16_t size, uint32_t timeout_ms)
{
	uint32_t timeout_count;
	const uint32_t LOOP_PER_MS = 1000; // 根据 MCU 频率调整，这里假设每毫秒 1000 次循环
	
	while (size--) {
		timeout_count = timeout_ms * LOOP_PER_MS;
		while (!uart_recv_byte(buf)) {
			if (timeout_ms > 0 && timeout_count-- == 0) {
				return 0; // 超时
			}
		}
		buf++;
	}
	return 1; // 成功
}

void uart_send(uint8_t *buf, uint16_t size)
{
	while (size--) {
		uart_send_byte(*buf);
		buf++;
	}
}

/******************************************************************************/

size_t __write(int handle, const uint8_t *buffer, size_t size)
{
	size_t nChars = 0;
	if (buffer == 0) {
		return 0;
	}
	for (/* Empty */; size != 0; --size) {
		uart_send_byte(*buffer++);
		++nChars;
	}
	return nChars;
}

// printf 需要实现 fputc
int fputc(int ch, FILE *f)
{
	uart_send_byte(ch);
	return ch;
}

void uart_init(uint32_t BaudRate)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	// 外设时钟使能
	RCC_AHBPeriphClk_Enable(DEBUG_USART_GPIO_CLK | RCC_AHB_PERIPH_DMA, ENABLE);
	DEBUG_USART_APBClkENx(DEBUG_USART_CLK, ENABLE);

	// UART TX RX 复用
	DEBUG_USART_AFTX;
	DEBUG_USART_AFRX;

	GPIO_InitStructure.Pins = DEBUG_USART_TX_GPIO_PIN;
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
	GPIO_Init(DEBUG_USART_TX_GPIO_PORT, &GPIO_InitStructure);
	GPIO_InitStructure.Pins = DEBUG_USART_RX_GPIO_PIN;
	GPIO_InitStructure.Mode = GPIO_MODE_INPUT_PULLUP;
	GPIO_Init(DEBUG_USART_RX_GPIO_PORT, &GPIO_InitStructure);

	USART_InitStructure.USART_BaudRate = BaudRate;
	USART_InitStructure.USART_Over = USART_Over_16;
	USART_InitStructure.USART_Source = USART_Source_PCLK;
	USART_InitStructure.USART_UclkFreq = DEBUG_USART_UclkFreq;
	USART_InitStructure.USART_StartBit = USART_StartBit_FE;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(DEBUG_USARTx, &USART_InitStructure);

	NVIC_SetPriority(UART2_IRQn, 1); // 优先级
	NVIC_EnableIRQ(UART2_IRQn); // UARTx 中断使能

	USART_ITConfig(DEBUG_USARTx, USART_IT_RC, ENABLE); // 开启串口接收中断
}

void UART2_IRQHandler(void)
{
	// 检查并清除错误标志 (帧错误 FE, 校验错误 PE)
	// 如果不清除这些错误，UART 可能会停止接收新的数据
	if (USART_GetFlagStatus(DEBUG_USARTx, USART_FLAG_FE) != RESET) {
		USART_ClearFlag(DEBUG_USARTx, USART_FLAG_FE);
	}
	if (USART_GetFlagStatus(DEBUG_USARTx, USART_FLAG_PE) != RESET) {
		USART_ClearFlag(DEBUG_USARTx, USART_FLAG_PE);
	}

	// 处理接收中断
	if (USART_GetITStatus(DEBUG_USARTx, USART_IT_RC) != RESET) {
		// 先清除标志位，再读取数据
		USART_ClearITPendingBit(DEBUG_USARTx, USART_IT_RC);

		g_uart_recv_buf[g_index_write] = USART_ReceiveData_8bit(DEBUG_USARTx);

		g_index_write++;
		if (g_index_write == UART_RECVBUF_SIZE) {
			g_index_write = 0;
		}

		// 简单的环形缓冲区溢出保护
		if (g_index_write == g_index_read) {
			g_index_read++;
			if (g_index_read == UART_RECVBUF_SIZE) {
				g_index_read = 0;
			}
		}
	}
}
