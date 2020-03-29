/*
 * bluetooth.h
 *
 *  Created on: 02.03.2020
 *      Author: Tobias Ellermeyer
 */

#ifndef INC_ZUMO_BLUETOOTH_H_
#define INC_ZUMO_BLUETOOTH_H_

#define BT_BAUDRATE  115200
#define BT_SUFFIX    "Zumo"

#define BT_NOT_INITIALIZED    	0
#define BT_INIT_ONGOING 	  	2
#define BT_INITIALIZED   		1

uint8_t bt_init();
uint8_t bt_send(char* buf);
uint8_t bt_receive(char *recv);
uint8_t bt_program(char *name);
char *bt_getname();



#endif /* INC_ZUMO_BLUETOOTH_H_ */
