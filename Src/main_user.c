/*
  ___          _____     __  __
   _/    _  _ (_  | |\/|  _)  _)
  /__|_||||(_)__) | |  | __) /__

 Fachhochschule Südwestfalen
 Mechatronik/Mikrocomputer
 Prof. Dr.-Ing. Tobias Ellermeyer

 Datei: main_user.c
 Pfad:  src

 Erstellt:    10.09.2019
 Autor:       Tobias Ellermeyer
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


void main_user()
{
	uint8_t status;
	uint8_t *p;
	uint16_t range[5];  // FF, FL, FR, LL, RR
	char buf[80];

	display_init();
	motors_init();
	char c;
	uart_init();
	HAL_Delay(100);
	// Ausgabe auf USB/seriell
	printf("\r\n");
	printf("**************************************\r\n");
	printf("*  HERE we GO \r\n");
	printf("**************************************\r\n\r\n");

	// Initialisiere Bluetooth
	status = bt_init();
	if (status)
	{
		printf("***ERROR*** bt_init() returned %i\r\n",status);
		Error_Handler();	// Do SOS blinking
	}

	// Initialisiere Entfernungsmessung
	status=distance_init();
	if (status)
	{
		printf("***ERROR*** distance_init() returned %i\r\n",status);
		Error_Handler();	// Do SOS blinking
	}

	// BNO055 IMU initialisieren
    bno055_setup();
    bno055_setOperationModeNDOF();

    HAL_Delay(100);


	// Auswahl-Menu anzeigen
	// --> in Src/testing.c finden Sie weitere Routinen zur Ansteuerung
	test_menu(0);

	display_clear();    // Display löschen
	display_println("Sending Distance");
	display_println("-Data via BT");

	// Einen ersten Distance-Scann starten; dieser läuft ca. 20 ms
	distance_start_scan();
	HAL_Delay(20);

	while(1)
	{
		distance_get_scan_values(range);	// Gemessene Werte auslesen
		distance_start_scan();				// und sofort neue Messung starten

		// Bluetooth-String zusammenbauen
		sprintf(buf,"FF %3i; FL %3i; FR %3i; LL %3i; RR %3i; ER  0; EL 0\r\n",
				range[0], range[1], range[2], range[3], range[4]);
		// und senden
		bt_send(buf);
    	HAL_Delay(200);
	}

}
