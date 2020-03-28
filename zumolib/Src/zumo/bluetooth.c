/*
 * bluetooth.c
 *
 *  Created on: 02.03.2020
 *      Author: Tobias Ellermeyer
 */

#include "main.h"
#include <zumo/uart.h>
#include <zumo/bluetooth.h>

#include <stdio.h>
#include <string.h>

//#define BT_DEBUG

/**
 @file bluetooth.c

 @brief Ansteuerung des HC-05 Bluetooth-Modul

 @author Tobias Ellermeyer
 */


static char bt_name[16];
static uint8_t bt_initialized = BT_NOT_INITIALIZED;

static uint8_t bt_program();		// Change settings of HC-05 module

/**
 *  @brief Starten der Bluetooth-Verbindung
 *
 *  Falls erforderlich, werden die Parameter des HC-05 gesetzt
 *
 *  @param none
 *  @return status -> 0 kein Fehler
 */
uint8_t bt_init()
{
	uint8_t len_recv, len_sufx;	// Text length of received data and name suffix
	char recbuf[20];	// Receive Buffer
	uint8_t status;


#ifdef BT_DEBUG
	printf("   ... power up bluetooth module\r\n");
#endif
	// Temporarily set bt_initialized to ongoing

	bt_initialized = BT_INIT_ONGOING;

	// Power up BT Module in normal mode (Way 1 in Data Sheet)
	// i.e. BT_Key=Low
	HAL_GPIO_WritePin(BT_KEY_GPIO_Port, BT_KEY_Pin, GPIO_PIN_RESET);
	HAL_Delay(100);
	HAL_GPIO_WritePin(BT_PWR_GPIO_Port, BT_PWR_Pin, GPIO_PIN_SET);

	uart2_baudrate(BT_BAUDRATE);

	HAL_Delay(1000);
	// Temporaly switch to command mode
	HAL_GPIO_WritePin(BT_KEY_GPIO_Port, BT_KEY_Pin, GPIO_PIN_SET);
	HAL_Delay(100);
	bt_send("AT+NAME?\r\n");
	HAL_Delay(100);
	while (!uart2_tx_done());
	status = bt_receive(recbuf);
#ifdef BT_DEBUG
	if (status!=0)
	{
		printf("   ... Timeout Bluetooth receive\n");
	}
#endif
	if (status==0)  // bt_receive was ok
	{ 	// Check if Name Suffix matches
		len_recv = strlen(recbuf);
		len_sufx  = strlen(BT_SUFFIX);
		if (len_recv < (len_sufx+5))	// Wrong length of answer
		{
			status = 1;
		}
		else
		{
			status = (strncmp(&recbuf[len_recv-len_sufx-2],BT_SUFFIX,len_sufx)!=0);
		}
	}
	// If status != 0, BT needs reprogramming
	if (status!=0)
	{
		if (bt_program())	// Reprogramming of BT module needed
		{	// There was an Error while reprogramming
			bt_initialized = BT_NOT_INITIALIZED;
			return 1;
		}
	}
	else	// OK
	{
		// Remove /r/n
		recbuf[len_recv-2]=0;
		sprintf(bt_name,"%s", &recbuf[6]);
	}

//#ifdef BT_DEBUG
	printf("   ... bluetooth name: %s\r\n   ... baudrate: %i,8,N,1\r\n",
		bt_name, BT_BAUDRATE);
//#endif
	// Back to normal mode
	HAL_GPIO_WritePin(BT_KEY_GPIO_Port, BT_KEY_Pin, GPIO_PIN_RESET);
	bt_initialized = BT_INITIALIZED;

	return 0;
}


/**
 *  @brief Senden eines Strings über die Bluetooth-Verbindung
 *
 *  @param  buf: Nullterminierter String, der übertragen wird
 *  @return status -> 0 kein Fehler; 1 sonst
 */
uint8_t bt_send(char* buf)
{
	uint16_t i;

	if (bt_initialized == BT_NOT_INITIALIZED)
		{
		printf("ERROR: BT not initialized ...\r\n");
		return 1;
		}
	i=0;
	while ((buf[i]) && (i<255))
	{
		uart2_putc(buf[i]);
		i++;
	}

	return 0;
}

/**
 *  @brief Empfangen eines Strings über die Bluetooth-Verbindung
 *
 *  @param  recv: Empfangender String
 *  @return status
 *  				0 kein Fehler
 *  				1 BT nicht initialisiert
 *  				2 Timeout
 */
uint8_t bt_receive(char* buf)
{
	uint8_t c,i;
	uint16_t timeout;

	if (bt_initialized == BT_NOT_INITIALIZED)
		{
		printf("ERROR: BT not initialized ...\r\n");
		return 1;
		}

	c=0;
	i=0;
	buf[0]=0;

	while(c!='\n')
	{
		timeout = 1000;  // Wait max 1 sec
		do
		{
			c=uart2_getc();
			if (c==UART_EMPTY)
			{
				timeout--;
				HAL_Delay(5);
			}
		}
		while ((c==UART_EMPTY) && (timeout!=0) );
		if (timeout == 0)
			{
			buf[0]=0;
			return 2;
			}
		buf[i++]=c;
		buf[i]=0;
	}
	return 0;
}

/**
 *  @brief Interne Routine zum Umprogrammieren
 *
 *  @param  none
 *  @return 0 = success
 *  		1 = fail
 */
static uint8_t bt_program()
{
	char buf[40];

#ifdef BT_DEBUG
	printf("   ... bluetooth needs parameter update\r\n");
#endif

	// Power down module to enter command mode
	HAL_GPIO_WritePin(BT_PWR_GPIO_Port, BT_PWR_Pin, GPIO_PIN_RESET);
	HAL_Delay(200);
	// Power up BT Module in Command mode
	// i.e. BT_Key=High
	HAL_GPIO_WritePin(BT_KEY_GPIO_Port, BT_KEY_Pin, GPIO_PIN_SET);
	HAL_Delay(100);
	HAL_GPIO_WritePin(BT_PWR_GPIO_Port, BT_PWR_Pin, GPIO_PIN_SET);

	uart2_baudrate(38400);		// This is the Baudrate in Command mode

	HAL_Delay(1000);		// Wait until BT Module boots up

	// Check if Module responds
	bt_send("AT\r\n");
	HAL_Delay(100);
	bt_receive(buf);
	if (strncmp(buf,"OK",2))
	{
		printf("ERROR - Bluetooth module not responding\r\n");
	    return 1;
		//Error_Handler();
	}

	// Programm name
	sprintf(bt_name,"Speedy_%s",BT_SUFFIX);
	sprintf(buf,"AT+NAME=%s\r\n",bt_name);
	bt_send(buf);
	bt_receive(buf);
	if (strncmp(buf,"OK",2))
	{
		printf("ERROR - Setting bluetooth name failed\r\n");
	    return 1;
		//Error_Handler();
	}

	// Programm baud rate (0,0 -> 1 Stopp / no parity)
	sprintf(buf,"AT+UART=%i,0,0\r\n",BT_BAUDRATE);
	bt_send(buf);
	bt_receive(buf);
	if (strncmp(buf,"OK",2))
	{
		printf("ERROR - Setting bluetooth baudrate\r\n");
	    return 1;
		//Error_Handler();
	}

	// Revert to normal mode
	HAL_GPIO_WritePin(BT_PWR_GPIO_Port, BT_PWR_Pin, GPIO_PIN_RESET);
	HAL_Delay(200);
	HAL_GPIO_WritePin(BT_KEY_GPIO_Port, BT_KEY_Pin, GPIO_PIN_RESET);
	HAL_Delay(100);
	HAL_GPIO_WritePin(BT_PWR_GPIO_Port, BT_PWR_Pin, GPIO_PIN_SET);

	uart2_baudrate(BT_BAUDRATE);		// This is the Baudrate in Normal mode

	HAL_Delay(1000);		// Wait until BT Module boots up

	return 0;
}

