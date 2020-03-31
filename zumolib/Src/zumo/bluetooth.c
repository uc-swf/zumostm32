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

//#define BT_DEBUG		// uncomment to enable additional debugging output

/**
 @file bluetooth.c

 @brief driver to control the HC-05 bluetooth modul

 @author Tobias Ellermeyer
 */


static char bt_name[16];
static uint8_t bt_initialized = BT_NOT_INITIALIZED;


/**
 *  @brief start the bluetooth connection
 *
 *  This routine checks if HC-05 bluetooth modul can be addressed properly.
 *  If not, it tries to reprogram it.
 *
 *  Startup takes approx 2sec (w/o reprogramming)
 *
 *  @param none
 *  @return
 *  	status: 0 - no error
 *  	        1 - error
 */
uint8_t bt_init()
{
	uint8_t len_recv, len_sufx;	// Text length of received data and name suffix
	char recbuf[30];	// Receive Buffer
	uint8_t status;


#ifdef BT_DEBUG
	printf("   ... power up bluetooth module\r\n");
#endif
	// Temporarily set bt_initialized to ongoing

	bt_initialized = BT_INIT_ONGOING;

	// Power up BT Module in normal mode (Way 1 in Data Sheet to check correct baud rate)
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
	status = bt_receive(recbuf,sizeof(recbuf));
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
		sprintf(bt_name,"UNNAMD");
		if (bt_program(bt_name))	// Reprogramming of BT module needed
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
	HAL_GPIO_WritePin(BT_KEY_GPIO_Port, BT_PWR_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(BT_KEY_GPIO_Port, BT_KEY_Pin, GPIO_PIN_RESET);
	HAL_Delay(200);
	HAL_GPIO_WritePin(BT_KEY_GPIO_Port, BT_PWR_Pin, GPIO_PIN_SET);
	bt_initialized = BT_INITIALIZED;

	return 0;
}


/**
 *  @brief Send a string via bluetooth
 *
 *  @param  buf: string to be transmitted
 *  @return
 *  	status: 0 - no error
 *				1 - error
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
 *  @brief Receive a Newline (\n) terminated string via bluetooth
 *
 *	The routine has a build in timeout and will terminate if no more
 *	characters are received within this timeout period.
 *
 *  @param
 *  	buf:    pointer to memory for received string
 *  	maxlen:	maximum number of chars to receive (incl. \0 string terminator)
 *  			(if max. is reached, last char will be overwritten until \n is received)
 *
 *  @return
 *  	status:		0 - no error
 *  				1 - BT not initialized
 *  				2 - Timeout
 */
uint8_t bt_receive(char* buf, uint8_t maxlen)
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
		buf[i]=c;
		if (i<(maxlen-1)) i++;
		buf[i]=0;
	}
	return 0;
}

/**
 *  @brief Internal routine to set bt parameters and bt name
 *
 *  @param
 *  	newname: Name to use for bluetooth; a suffix ("_Zumo") is added to this name
 *
 *  @return
 *  	status: 	0 - success
 *  				1 - fail
 */
uint8_t bt_program(char *newname)
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

	uart2_baudrate(38400);	// This is the Baudrate in Command mode

	HAL_Delay(1000);		// Wait until BT Module boots up

	// Check if Module responds
	bt_send("AT\r\n");
	HAL_Delay(100);
	bt_receive(buf,sizeof(buf));
	if (strncmp(buf,"OK",2))
	{
		printf("ERROR - Bluetooth module not responding\r\n");
	    return 1;
	}

	// Programm name
	sprintf(bt_name,"%s_%s",newname,BT_SUFFIX);
	sprintf(buf,"AT+NAME=%s\r\n",bt_name);
	bt_send(buf);
	bt_receive(buf,sizeof(buf));
	if (strncmp(buf,"OK",2))
	{
		printf("ERROR - Setting bluetooth name failed\r\n");
	    return 1;
	}

	// Programm baud rate (0,0 -> 1 Stopp / no parity)
	sprintf(buf,"AT+UART=%i,0,0\r\n",BT_BAUDRATE);
	bt_send(buf);
	bt_receive(buf,sizeof(buf));
	if (strncmp(buf,"OK",2))
	{
		printf("ERROR - Setting bluetooth baudrate\r\n");
	    return 1;
	}

	// Revert to normal mode
	HAL_GPIO_WritePin(BT_PWR_GPIO_Port, BT_PWR_Pin, GPIO_PIN_RESET);
	HAL_Delay(200);
	HAL_GPIO_WritePin(BT_KEY_GPIO_Port, BT_KEY_Pin, GPIO_PIN_RESET);
	HAL_Delay(100);
	HAL_GPIO_WritePin(BT_PWR_GPIO_Port, BT_PWR_Pin, GPIO_PIN_SET);

	uart2_baudrate(BT_BAUDRATE);		// This is the baudrate in Normal mode

	HAL_Delay(1000);		// Wait until BT Module boots up

	return 0;
}

<<<<<<< HEAD
/**
 *  @brief Get name of bluetooth device
 *
 *  @param  	none
 *
 *  @return
 *  	name: 	char* pointing to name
 */

=======
>>>>>>> refs/remotes/origin/master
char* bt_getname()
{
	return bt_name;
}
