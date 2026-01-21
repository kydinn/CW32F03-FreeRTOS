#ifndef UART_H
#define UART_H

#include "base_types.h"
#include "cw32f030_uart.h"

// UARTx
#define DEBUG_USARTx                   CW_UART2
#define DEBUG_USART_CLK                RCC_APB1_PERIPH_UART2
#define DEBUG_USART_APBClkENx          RCC_APBPeriphClk_Enable1
#define DEBUG_USART_UclkFreq           24000000 // 时钟频率 24M

// UARTx GPIO
#define DEBUG_USART_GPIO_CLK           RCC_AHB_PERIPH_GPIOA
#define DEBUG_USART_TX_GPIO_PORT       CW_GPIOA
#define DEBUG_USART_TX_GPIO_PIN        GPIO_PIN_2
#define DEBUG_USART_RX_GPIO_PORT       CW_GPIOA
#define DEBUG_USART_RX_GPIO_PIN        GPIO_PIN_3

// GPIO AF
#define DEBUG_USART_AFTX               PA02_AFx_UART2TXD()
#define DEBUG_USART_AFRX               PA03_AFx_UART2RXD()

#define UART_RECVBUF_SIZE  128
#define UART_TX_BUF_SIZE  128

char uart_recv_byte(unsigned char *ch);
void uart_send_byte(unsigned char ch);
void uart_recv(uint8_t *buf, uint16_t size);
uint8_t uart_recv_timeout(uint8_t *buf, uint16_t size, uint32_t timeout_ms);
void uart_send(uint8_t *buf, uint16_t size);
void uart_init(uint32_t BaudRate);

extern uint8_t g_tx_buffer[UART_TX_BUF_SIZE];

#endif // UART_H
