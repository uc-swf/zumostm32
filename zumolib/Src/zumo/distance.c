/*
  ___          _____     __  __
   _/    _  _ (_  | |\/|  _)  _)
  /__|_||||(_)__) | |  | __) /__

 Fachhochschule Südwestfalen
 Mechatronik/Mikrocomputer
 Prof. Dr.-Ing. Tobias Ellermeyer

 Datei: distance.c
 Pfad:  zumolib/src/zumo

 Erstellt:    02.12.2019
 Autor:       Tobias Ellermeyer
*/

#include <zumo/vl53l0x.h>
#include <zumo/gpio_sx1508.h>
#include <zumo/distance.h>
//#include <stdio.h>
#include "main.h"

//void scan_i2c();

/**
 @file distance.c
 @code #include <zumo/distance.h> @endcode

 @brief High-Level Time-of-Flight Entfernungsmessungs-Funktionen

		Einfacher Ablauf:

		uint16_t ranges[5];
		distance_init();
		distance_start_scan();	// Erste Messung starten

		while(1)
		{
		   distance_get_scan_values(ranges);  // Werte lesen
		   distance_start_scan();			  // sofort neue Messung starten
		   ...
		   Weitere Berechnungen
		   ...
		}


 @author Tobias Ellermeyer
 */


/**
 * @brief Alle ToF-Sensors initialisieren
 *
 * @return
 *
 *    0  - ok \n
 *   <>0 - Fehler
 */
uint8_t distance_init()
{
	// Initialize the GPIO Expander IC
    gpio_sx1508_init();

	// Assign Front sensor to new I2C-Address
	VL53L0X_address=0x29;
	gpio_sx1508_out(TOF_ENABLE_FF);
	HAL_Delay(20); // Wait for Sensor Power up
	//if (VL53L0X_pingAddress(VL53L0X_address)) return 1; // Check if sensor present
	VL53L0X_setAddress(TOF_I2CADDR_FF);	// New address

	// Assign Front/Left sensor to new I2C-Address
	VL53L0X_address=0x29;
	// Already moved sensors must be kept enabled
	// otherwise they are resetted to default address
	gpio_sx1508_out(TOF_ENABLE_FL|TOF_ENABLE_FF);
	HAL_Delay(20);
	//if (VL53L0X_pingAddress(VL53L0X_address)) return 2;
	VL53L0X_setAddress(TOF_I2CADDR_FL);

	// Assign Front/Right sensor to new I2C-Address
	VL53L0X_address=0x29;
	gpio_sx1508_out(TOF_ENABLE_FR|TOF_ENABLE_FL|TOF_ENABLE_FF);
	HAL_Delay(20);
	//if (VL53L0X_pingAddress(VL53L0X_address)) return 3;
	VL53L0X_setAddress(TOF_I2CADDR_FR);

	// Assign Right sensor to new I2C-Address
	VL53L0X_address=0x29;
	gpio_sx1508_out(TOF_ENABLE_FR|TOF_ENABLE_FL|TOF_ENABLE_FF|TOF_ENABLE_RR);
	HAL_Delay(20);
	//if (VL53L0X_pingAddress(VL53L0X_address)) return 4;
	VL53L0X_setAddress(TOF_I2CADDR_RR);

	// Assign Left sensor to new I2C-Address
	VL53L0X_address=0x29;
	gpio_sx1508_out(TOF_ENABLE_ALL);
	HAL_Delay(20);
	//if (VL53L0X_pingAddress(VL53L0X_address)) return 5;
	VL53L0X_setAddress(TOF_I2CADDR_LL);
	//scan_i2c();

	// Initalize all Sensors
	VL53L0Xm_init(TOF_I2CADDR_FF,1);
	VL53L0X_setSignalRateLimit(2.5);
	VL53L0X_setMeasurementTimingBudget(30000);
	VL53L0Xm_init(TOF_I2CADDR_FL,1);
	VL53L0X_setSignalRateLimit(2.5);
	VL53L0X_setMeasurementTimingBudget(30000);
	VL53L0Xm_init(TOF_I2CADDR_FR,1);
	VL53L0X_setSignalRateLimit(2.5);
	VL53L0X_setMeasurementTimingBudget(30000);
	VL53L0Xm_init(TOF_I2CADDR_RR,1);
	VL53L0X_setSignalRateLimit(2.5);
	VL53L0X_setMeasurementTimingBudget(30000);
	VL53L0Xm_init(TOF_I2CADDR_LL,1);
	VL53L0X_setSignalRateLimit(2.5);
	VL53L0X_setMeasurementTimingBudget(30000);
	return 0;
}

/**
 * @brief Eine Einzelmessung starten
 *
 * @param sensor\n
 *    `TOF_I2CADDR_FF`	Vorderer Sensor geradeaus\n
 *    `TOF_I2CADDR_FR`	Vorderer Sensor 45 Grad rechts\n
 *    `TOF_I2CADDR_FL`	Vorderer Sensor 45 Grad links\n
 *    `TOF_I2CADDR_RR`	Seitlicher Sensor rechts\n
 *    `TOF_I2CADDR_LL`	Seitlicher Sensor links\n
 *
 *
 * @return
 *   Abstandswert in Millimeter (0=Fehler)
 */
uint16_t distance_single_mm(uint8_t sensor)
{
	if ( (sensor==TOF_I2CADDR_FF) ||
		(sensor==TOF_I2CADDR_FR) ||
		(sensor==TOF_I2CADDR_FL) ||
		(sensor==TOF_I2CADDR_RR) ||
		(sensor==TOF_I2CADDR_LL) )
	{
	VL53L0X_address = sensor;
	}
	else
	{
		return 0;
	}

	return VL53L0X_readRangeSingleMillimeters();
}


/**
 * @brief Alle fuenf Sensorwerte gleichzeitig lesen
 *
 * @param values\n
 *        Zeiger auf ein uint16_t-Feld mit 5 Eintraegen
 *        values[0] -> vorderer Sensor
 *        values[1] -> vorderer linker 45 Grad Sensor
 *        values[2] -> vorderer rechter 45 Grad Sensor
 *        values[3] -> linker Sensor
 *        values[4] -> rechter Sensor
 *
 *
 * @return
 *   0 -> ok  /  !=0 -> kein Fehler
 */
uint8_t distance_get_scan_values(uint16_t *values)
{
	values[0] = VL53L0Xm_readRangeContinuousMillimeters(TOF_I2CADDR_FF);
	values[1] = VL53L0Xm_readRangeContinuousMillimeters(TOF_I2CADDR_FL);
	values[2] = VL53L0Xm_readRangeContinuousMillimeters(TOF_I2CADDR_FR);
	values[3] = VL53L0Xm_readRangeContinuousMillimeters(TOF_I2CADDR_LL);
	values[4] = VL53L0Xm_readRangeContinuousMillimeters(TOF_I2CADDR_RR);
	return 0;
}

/**
 * @brief Auf allen Sensoren gleichzeitig eine Einzelmessung starten
 *
 * @return
 *   0 -> ok  /  !=0 -> kein Fehler
 */
uint8_t distance_start_scan()
{
	VL53L0X_startSingle(TOF_I2CADDR_FF);
	VL53L0X_startSingle(TOF_I2CADDR_FL);
	VL53L0X_startSingle(TOF_I2CADDR_FR);
	VL53L0X_startSingle(TOF_I2CADDR_LL);
	VL53L0X_startSingle(TOF_I2CADDR_RR);
	return 0;
}
/*
uint8_t distance_start_scan()
{
	VL53L0Xm_startContinuous(TOF_I2CADDR_FF,0);
	VL53L0Xm_startContinuous(TOF_I2CADDR_FL,0);
	VL53L0Xm_startContinuous(TOF_I2CADDR_FR,0);
	VL53L0Xm_startContinuous(TOF_I2CADDR_LL,0);
	VL53L0Xm_startContinuous(TOF_I2CADDR_RR,0);
	return 0;
}
*/
