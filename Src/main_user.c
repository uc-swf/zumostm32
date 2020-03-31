/*
  ___          _____     __  __
   _/    _  _ (_  | |\/|  _)  _)
  /__|_||||(_)__) | |  | __) /__

 Fachhochschule Südwestfalen
 Mechatronik/Mikrocomputer
 Prof. Dr.-Ing. Tobias Ellermeyer

 file: main_user.c
 path:  src

 created:    10.09.2019
 author:       Tobias Ellermeyer
*/


#include "main.h"
#include <stdio.h>
#include <zumo/display.h>
#include <zumo/motors.h>
#include <zumo/bno055.h>
#include <zumo/distance.h>
#include <zumo/vl53l0x.h>
#include <zumo/uart.h>
#include <zumo/bluetooth.h>

#include "testing.h"

#define WITH_TESTING		// Uncomment to remove test menu option (saves approx. 10k flash memory)

void main_user()
{
	uint8_t status;
	uint8_t *p;
	uint16_t range[5];  // FF, FL, FR, LL, RR
	char buf[80];
	uint8_t dir = 0,stop=0;
	uint8_t cnt =0;

	display_init();		// initialize display driver
#ifdef WITH_TESTING
	display_gotoxy(20,7);
	display_println("Hold <USR> to enter Test");
#endif

	motors_init();		// initialize motor driver
	char c;

	uart_init();		// switch printf to usb/serial
	HAL_Delay(100);
	printf("\r\n****   HERE we GO  ****\r\n\r\n");

	// initialize bluetooth
	status = bt_init();
	if (status)
	{
		printf("***ERROR*** bt_init() returned %i\r\n",status);
		Error_Handler();	// Do SOS blinking
	}

	// initialize distance measurement
	status=distance_init();
	if (status)
	{
		printf("***ERROR*** distance_init() returned %i\r\n",status);
		Error_Handler();	// Do SOS blinking
	}

	// BNO055 IMU initialization
    bno055_setup();
    bno055_setOperationModeNDOF();

    HAL_Delay(100);


#ifdef WITH_TESTING
	// Check if Button <USR> is pressed -> Enter test menu
	// --> see Src/testing.c for details how to control most of the peripherals
    if (HAL_GPIO_ReadPin(BTN_USR_GPIO_Port,BTN_USR_Pin)==0)
    	{
    	test_menu(0);
    	}
#endif

    //**********************************************************************
    // ADD OWN ROUTINES BELOW THIS LINE ...
    //**********************************************************************


    // TEST ROUTINE for transmitting ToF and Encoder data via BT
    // and also remote control of Robot via keyboard

	display_clear();
	display_println("Sending Distance");
	display_println("-Data via BT");

	// start initial distance scan; this takes approx. 20 ms
	distance_start_scan();
	HAL_Delay(20);

	while(1)
	{
		distance_get_scan_values(range);	// read out array of measured values (order FF,FL,FR,LL,RR)
		distance_start_scan();				// and immediatly start a new measurement (which again takes 20ms)

		// format string for bluetooth transmission
		sprintf(buf,"FF %3i; FL %3i; FR %3i; LL %3i; RR %3i; ER  %9li; EL %9li\r\n",
				range[0], range[1], range[2], range[3], range[4],
				motors_encoder_left(), motors_encoder_right());
		bt_send(buf); 		// send bluetooth string

		c = uart2_getc();	// receive a char from BT for controlling movement
		sprintf(buf,"KEY: %c",c);
		display_gotoxy(3,6);
		display_println(buf);

		// Check for obstacle
		stop = (range[0]<60) || (range[1]<70) || (range[2]<70);
		if ( (dir==1) && stop)
			motors_set_both_pwm(0,0);

		#define SPEED 35

		switch(c)
		{
		case ' ': bt_send("***** MARK *****\r\n");		// Add a marker in BT Data
				  break;
		case 'w': if (!stop) motors_set_both_pwm(SPEED,SPEED);	// Go forward
				  dir = 1;
				  break;
		case 'a': motors_set_both_pwm(-SPEED,SPEED);		// turn left
				  if (dir) dir = 2;
				  cnt = 0;
				  break;
		case 's': motors_set_both_pwm( SPEED,-SPEED);		// turn right
				  if (dir) dir = 2;
				  cnt = 0;
				  break;
		case 'y': if (dir)									// stop / drive backwards
					{
					dir = 0;
					motors_set_both_pwm(0,0);
					}
				  else
				  {
					  motors_set_both_pwm(-SPEED,-SPEED);
					  cnt = 0;
				  }
				  break;
		default:  break;
		}
    	HAL_Delay(100);

    	// this control logic ensures small movements for turning left/right
    	// if robot was driving forward, this will be enabled again after turning
    	if ( (cnt++)%2 == 1 )
    	{
    		if (dir==0)
    		{
    			motors_set_both_pwm(0,0);
    		}
    		if (dir==2)
    		{
    			motors_set_both_pwm(SPEED,SPEED);
    			dir = 1;
    		}
    	}
	}

}
