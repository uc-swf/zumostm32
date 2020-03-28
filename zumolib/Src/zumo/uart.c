/*
  ___          _____     __  __
   _/    _  _ (_  | |\/|  _)  _)
  /__|_||||(_)__) | |  | __) /__

 Fachhochschule Südwestfalen
 Mechatronik/Mikrocomputer
 Prof. Dr.-Ing. Tobias Ellermeyer

 Datei: uart_printf.c
 Pfad:  zumolib/src/zumo

 Erstellt:    23.09.2019
 Autor:       Tobias Ellermeyer

 see the following web page for details of IRQ handling:
 https://stackoverflow.com/questions/37336527/why-does-uart-transmit-interrupt-fail-to-work-in-this-case
*/

#ifndef SRC_UART_C_
#define SRC_UART_C_

#include "main.h"
#include "zumo/uart.h"
#include <stdio.h>


static uint8_t rx_buf[RXBUF_LEN];		// Receive Buffer UART1
static uint8_t tx_buf[TXBUF_LEN];		// Transmit Buffer UART1
static volatile uint16_t rx_i, tx_o;
static uint16_t rx_e, tx_i;
static volatile uint8_t tx_busy;

static uint8_t rx2_buf[RXBUF_LEN];		// Receive Buffer UART2
static uint8_t tx2_buf[TXBUF_LEN];		// Transmit Buffer UART2
static volatile uint16_t rx2_i, tx2_o;
static uint16_t rx2_e, tx2_i;
static volatile uint8_t tx2_busy;

static volatile uint8_t tx_cnt;

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;


/**
 @file uart_printf.c

 @brief printf auf serielle/USB-Schnittstelle

 Wenn diese Datei mit im Projekt ist, wird die Ausgabe von printf auf die
 serielle Schnittstelle bzw. USB umgeleitet.

 @author Tobias Ellermeyer
 */



#ifdef __GNUC__
/* With GCC, small printf (option LD Linker->Libraries->Small printf
   set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART1 and Loop until the end of transmission */
  //HAL_UART_Transmit(&huart1, (uint8_t *)&ch,1,0xffff);
  uart_putc(ch);
  return ch;
}

void uart_init()
{
	tx_i  = 0;
	tx2_i = 0;
	rx_i  = 0;
	rx2_i = 0;
	rx_e  = 0;
	rx2_e = 0;
	tx_busy = 0;
	tx2_busy = 0;
	tx_cnt = 0;
	__HAL_UART_ENABLE_IT(&huart1, UART_IT_RXNE);
	__HAL_UART_ENABLE_IT(&huart2, UART_IT_RXNE);
	//printf("UART init done");
}

void uart_putc(uint8_t byte)
{

	// Wait until buffer has free space
	while (tx_cnt > (TXBUF_LEN-1));

    tx_buf[TXBUF_MSK & tx_i] = byte;
    tx_i++;
    tx_cnt++;
    tx_busy = 1;
    __HAL_UART_ENABLE_IT(&huart1, UART_IT_TXE);
}

uint8_t uart_getc()
{
	uint8_t c;
	if (rx_i == rx_e)  // No chars received
	{
		c = UART_EMPTY;
	}
	else		// There are chars in ringbuffer
	{
		c = rx_buf[RXBUF_MSK & rx_e];
		rx_e++;
	}
	return c;
}

void uart2_putc(uint8_t byte)
{

    tx2_buf[TXBUF_MSK & tx2_i] = byte;
    tx2_i++;
    tx2_busy = 1;
    __HAL_UART_ENABLE_IT(&huart2, UART_IT_TXE);
}

uint8_t uart2_tx_done()
{
	return (tx2_busy == 0);
}
uint8_t uart2_getc()
{
	uint8_t c;
	if (rx2_i == rx2_e)  // No chars received
	{
		c = UART_EMPTY;
	}
	else		// There are chars in ringbuffer
	{
		c = rx2_buf[RXBUF_MSK & rx2_e];
		rx2_e++;
	}
	return c;
}

void USART1_IRQHandler(void)
{
    /* USER CODE BEGIN USART1_IRQn 0 */
    if((__HAL_UART_GET_FLAG(&huart1, UART_FLAG_RXNE) != RESET) &&
       (__HAL_UART_GET_IT_SOURCE(&huart1, UART_IT_RXNE) != RESET))
    {
        rx_buf[rx_i & RXBUF_MSK] = (uint8_t)(huart1.Instance->DR & 0x00FF);
        rx_i++;
        /* Clear RXNE interrupt flag */
        //__HAL_UART_SEND_REQ(&huart1, UART_RXDATA_FLUSH_REQUEST);
        __HAL_UART_CLEAR_NEFLAG(&huart1);
    }
    if((__HAL_UART_GET_FLAG(&huart1, UART_FLAG_TXE) != RESET) &&
       (__HAL_UART_GET_IT_SOURCE(&huart1, UART_IT_TXE) != RESET))
    {
        if (tx_i == tx_o) {
            __HAL_UART_DISABLE_IT(&huart1, UART_IT_TXE);
            __HAL_UART_ENABLE_IT(&huart1, UART_IT_TC);
            tx_cnt = 0;
        } else {
            huart1.Instance->DR = (uint8_t)(tx_buf[TXBUF_MSK & tx_o] & (uint8_t)0xFF);
            tx_o++;
            tx_cnt--;
        }
    }
    if((__HAL_UART_GET_FLAG(&huart1, UART_FLAG_TC) != RESET) &&
       (__HAL_UART_GET_IT_SOURCE(&huart1, UART_IT_TC) != RESET))
    {
        tx_busy = 0;
        __HAL_UART_DISABLE_IT(&huart1, UART_IT_TC);
    }
    /* And never call default handler */
    return;
    /* USER CODE END USART1_IRQn 0 */

    HAL_UART_IRQHandler(&huart1);

    /* USER CODE BEGIN USART1_IRQn 1 */
    /* USER CODE END USART1_IRQn 1 */
}

void USART2_IRQHandler(void)
{
    /* USER CODE BEGIN USART2_IRQn 0 */
    if((__HAL_UART_GET_FLAG(&huart2, UART_FLAG_RXNE) != RESET) &&
       (__HAL_UART_GET_IT_SOURCE(&huart2, UART_IT_RXNE) != RESET))
    {
        rx2_buf[rx2_i & RXBUF_MSK] = (uint8_t)(huart2.Instance->DR & 0x00FF);
        rx2_i++;
        /* Clear RXNE interrupt flag */
        //__HAL_UART_SEND_REQ(&huart1, UART_RXDATA_FLUSH_REQUEST);
        __HAL_UART_CLEAR_NEFLAG(&huart2);
    }
    if((__HAL_UART_GET_FLAG(&huart2, UART_FLAG_TXE) != RESET) &&
       (__HAL_UART_GET_IT_SOURCE(&huart2, UART_IT_TXE) != RESET))
    {
        if (tx2_i == tx2_o) {
            __HAL_UART_DISABLE_IT(&huart2, UART_IT_TXE);
            __HAL_UART_ENABLE_IT(&huart2, UART_IT_TC);
        } else {
            huart2.Instance->DR = (uint8_t)(tx2_buf[TXBUF_MSK & tx2_o] & (uint8_t)0xFF);
            tx2_o++;
        }
    }
    if((__HAL_UART_GET_FLAG(&huart2, UART_FLAG_TC) != RESET) &&
       (__HAL_UART_GET_IT_SOURCE(&huart2, UART_IT_TC) != RESET))
    {
        tx2_busy = 0;
        __HAL_UART_DISABLE_IT(&huart2, UART_IT_TC);
    }
    /* And never call default handler */
    return;
    /* USER CODE END USART1_IRQn 0 */

    HAL_UART_IRQHandler(&huart2);

    /* USER CODE BEGIN USART1_IRQn 1 */
    /* USER CODE END USART1_IRQn 1 */
}

void uart2_baudrate(uint32_t br)
{
	huart2.Init.BaudRate=br;
	if (HAL_UART_Init(&huart2) != HAL_OK)
	  {
	    Error_Handler();
	  }
}

#endif /* SRC_UART_C_ */
