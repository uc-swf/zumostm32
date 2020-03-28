/*
  ___          _____     __  __
   _/    _  _ (_  | |\/|  _)  _)
  /__|_||||(_)__) | |  | __) /__

 Fachhochschule Südwestfalen
 Mechatronik/Mikrocomputer
 Prof. Dr.-Ing. Tobias Ellermeyer

 Datei: gpio_sx1508.c
 Pfad:  zumolib/src/gpio_sx1508.c

 Erstellt:    23.09.2019
 Autor:       Tobias Ellermeyer
*/

/**
 @file gpio_sx1508.c
 @code #include <zumo/gpio_sx1508.h> @endcode

 @brief Ansteuerung des I/O-Expanders, benutzt für ToF-Sensor-Initialisierung

 @author Tobias Ellermeyer
 */

#include "main.h"
#include <zumo/gpio_sx1508.h>

#define DEBUG_SX1508

#ifdef DEBUG_SX1508
#include <stdio.h>
#endif

extern I2C_HandleTypeDef hi2c1;		// Handle for I2C #1

#define SX1508_I2C_ADDR (0x20<<1)

#define SX1508_REG_INPUT_DISABLE 0x00
#define SX1508_REG_LONG_SLEW	 0x01
#define SX1508_REG_LOW_DRIVE     0x02
#define SX1508_REG_PULL_UP		 0x03
#define SX1508_REG_PULL_DOWN	 0x04
#define SX1508_REG_OPEN_DRAIN	 0x05
#define SX1508_REG_POLARITY		 0x06
#define SX1508_REG_DIR			 0x07
#define SX1508_REG_DATA			 0x08
#define SX1508_REG_RESET		 0x7D

// ... to be continued if needed


#define SX1508_MASK              ( TOF_ENABLE_FF | TOF_ENABLE_FR | TOF_ENABLE_FL | TOF_ENABLE_LL | TOF_ENABLE_RR)

/**
 * @brief Initialize the SX1508 I2C I/O Expander
 *
 *  @param none
 *  @return
 *     0 - no error
 *     1 - error
 */
uint8_t gpio_sx1508_init()
{
	uint8_t status;
	uint8_t data;
	//printf("SX1508 INIT\r\n");
	// Soft Reset
	data = 0x12;
	status = HAL_I2C_Mem_Write(&hi2c1, SX1508_I2C_ADDR, SX1508_REG_RESET, I2C_MEMADD_SIZE_8BIT, &data, 1, 100);
	data = 0x34;
	status += HAL_I2C_Mem_Write(&hi2c1, SX1508_I2C_ADDR, SX1508_REG_RESET, I2C_MEMADD_SIZE_8BIT, &data, 1, 100);
	if (status) {
		printf("SX1508 ERROR: RESET\r\n");
		return 1;
	}

	HAL_Delay(100);

	// Disable Input Buffer
	data = SX1508_MASK;
	status = HAL_I2C_Mem_Write(&hi2c1, SX1508_I2C_ADDR, SX1508_REG_INPUT_DISABLE, I2C_MEMADD_SIZE_8BIT, &data, 1, 100);
	if (status) {
		printf("SX1508 ERROR: INIT 1\r\n");
		return 1;
	}

	data = 0;
	status = HAL_I2C_Mem_Write(&hi2c1, SX1508_I2C_ADDR, SX1508_REG_DATA, I2C_MEMADD_SIZE_8BIT, &data, 1, 100);;
	if (status) {
		printf("SX1508 ERROR: INIT 1\r\n");
		return 1;
	}

	// Set open drain

	data = SX1508_MASK;
	status = HAL_I2C_Mem_Write(&hi2c1, SX1508_I2C_ADDR, SX1508_REG_OPEN_DRAIN, I2C_MEMADD_SIZE_8BIT, &data, 1, 100);
	if (status) {
		printf("SX1508 ERROR: INIT 2\r\n");
		return 1;
	}

	// Set data direction to output
	data = SX1508_MASK;
	status = HAL_I2C_Mem_Read(&hi2c1, SX1508_I2C_ADDR, SX1508_REG_DIR, I2C_MEMADD_SIZE_8BIT, &data, 1, 100);
	if (status) {
		printf("SX1508 ERROR: INIT 3\r\n");
		return 1;
	}
	return 0;
}

/**
 * @brief Set outputs on SX1508 I/O Expander
 *
 * @param val: Which outputs to set; Predefined values (may be or'ed)
 * TOF_ENABLE_FL: Front left sensor
 * TOF_ENABLE_FF: Front sensor
 * TOF_ENABLE_FR: Front right sensor
 * TOF_ENABLE_RR: Right sensor
 * TOF_ENABLE_LL: Left sensor
 *
 * @return
 *   0 - no error
 *   1 - error
 */
uint8_t gpio_sx1508_out(uint8_t val)
{
	uint8_t status;
	uint8_t data;

	data = val & SX1508_MASK;
	//printf("SX data: 0x%02x\r\n",data);
	status = HAL_I2C_Mem_Write(&hi2c1, SX1508_I2C_ADDR, SX1508_REG_DATA, I2C_MEMADD_SIZE_8BIT, &data, 1, 100);
	if (status) {
		printf("SX1508 ERROR: OUT 1\r\n");
		return 1;
	}
	return 0;
}


