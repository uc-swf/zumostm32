/*
  ___          _____     __  __
   _/    _  _ (_  | |\/|  _)  _)
  /__|_||||(_)__) | |  | __) /__

 Fachhochschule Südwestfalen
 Mechatronik/Mikrocomputer
 Prof. Dr.-Ing. Tobias Ellermeyer

 Datei: uart_printf.c
 Pfad:  zumolib/src/zumo

 Erstellt:    02.03.2020
 Autor:       Tobias Ellermeyer

 see the following web page for details of IRQ handling:
 https://stackoverflow.com/questions/37336527/why-does-uart-transmit-interrupt-fail-to-work-in-this-case
*/

#ifndef INC_ZUMO_UART_H_
#define INC_ZUMO_UART_H_

// Define Buffer Sizes
#define RXBUF_LEN            128 // must be power of 2
#define TXBUF_LEN            128 // must be power of 2
#define RXBUF_MSK            (RXBUF_LEN-1)
#define TXBUF_MSK            (TXBUF_LEN-1)

#define UART_EMPTY 		0

void uart_init();
void uart_putc(uint8_t);
uint8_t uart_getc();

void uart2_putc(uint8_t);
uint8_t uart2_getc();
void uart2_baudrate(uint32_t);
uint8_t uart2_tx_done();

#endif /* INC_ZUMO_UART_H_ */
