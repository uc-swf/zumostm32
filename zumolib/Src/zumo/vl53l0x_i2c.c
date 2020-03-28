/*
 * vl53l0x_i2c.c
 *
 *  Created on: 09.11.2019
 *      Author: Tobias Ellermeyer
 *
 *
 *  ATTENTION: STM32 uses little endian, while
 *             VL53L0X expects big endian.
 *             ==> Byte order needs to be adjusted when
 *                 transmitting word / long word
 */


#include "main.h"
#include <stdio.h>

extern I2C_HandleTypeDef hi2c1;		// Handle for I2C #1
extern uint32_t VL53L0X_address;

/**
 * @brief Check if Address is already in use
 *
 * @param none
 * @return
 *    0 - Device responds
 *    1 - Address not in use
 */
uint8_t VL53L0X_pingAddress(uint8_t addr)
{
	return (HAL_I2C_IsDeviceReady(&hi2c1, addr<<1,1,100)==0);
}


// VL53L0X Error
void VL53L0X_error(char* msg, uint8_t reg)
{
	printf("%s (addr 0x%02X@reg 0x%02X)\n",msg,VL53L0X_address, reg);
	Error_Handler();
}
// Write an 8-bit register


uint8_t VL53L0X_writeReg(uint8_t reg, uint8_t value)
{
	uint8_t status;
	status = HAL_I2C_Mem_Write(&hi2c1, VL53L0X_address<<1, reg, I2C_MEMADD_SIZE_8BIT, &value, 1, 100);
	if (status) VL53L0X_error("I2C Error WriteReg",reg);
	return status;
}

// Write a 16-bit register

uint8_t VL53L0X_writeReg16Bit(uint8_t reg, uint16_t value)
{
	uint8_t status;
	uint8_t data[2];
	uint8_t *p;
	// Rearrange from little to big endianess
	p = (uint8_t*)&value;
	data[0] = p[1];
	data[1] = p[0];
	status = HAL_I2C_Mem_Write(&hi2c1, VL53L0X_address<<1, reg, I2C_MEMADD_SIZE_8BIT,data, 2, 100);
	if (status) VL53L0X_error("I2C Error Write16",reg);

	return status;
}

// Write a 32-bit register

uint8_t VL53L0X_writeReg32Bit(uint8_t reg, uint32_t value)
{
	uint8_t status;
	uint8_t data[4];
	uint8_t *p;
	// Rearrange from little to big endianess
	p = (uint8_t*)&value;
	data[0] = p[3];
	data[1] = p[2];
	data[2] = p[1];
	data[3] = p[0];
	status = HAL_I2C_Mem_Write(&hi2c1, VL53L0X_address<<1, reg, I2C_MEMADD_SIZE_8BIT, data, 4, 100);
	if (status) VL53L0X_error("I2C Error Write32",reg);
	return status;
}

// Read an 8-bit register

uint8_t VL53L0X_readReg(uint8_t reg)
{
	uint8_t status;
	uint8_t value;

	status = HAL_I2C_Mem_Read(&hi2c1, VL53L0X_address<<1, reg, I2C_MEMADD_SIZE_8BIT, (uint8_t*) &value, 1, 100);
	if (status) VL53L0X_error("I2C Error ReadReg",reg);

	return value;
}

// Read a 16-bit register

uint16_t VL53L0X_readReg16Bit(uint8_t reg)
{
	uint8_t status;
	uint8_t data[2];
	uint16_t value;
	uint8_t  *p;

	status = HAL_I2C_Mem_Read(&hi2c1, VL53L0X_address<<1, reg, I2C_MEMADD_SIZE_8BIT, data, 2, 100);
	if (status) VL53L0X_error("I2C Error Read16",reg);
	// Rearrange from big to little endianess
	p = (uint8_t*)&value;
	p[0] = data[1];
	p[1] = data[0];
	return value;
}

// Read a 32-bit register

uint32_t VL53L0X_readReg32Bit(uint8_t reg)
{
	uint8_t status;
	uint8_t data[4];
	uint32_t value;
	uint8_t  *p;
	status = HAL_I2C_Mem_Read(&hi2c1, VL53L0X_address<<1, reg, I2C_MEMADD_SIZE_8BIT, data, 4, 100);
	if (status) VL53L0X_error("I2C Error Read32",reg);
	// Rearrange from big to little endianess
	p = (uint8_t*)&value;
	p[0] = data[3];
	p[1] = data[2];
	p[2] = data[1];
	p[3] = data[0];
	printf("Read32: 0x%08x 0x%08x\r\n",(uint32_t)(*data),value);

	return value;
}

// Write an arbitrary number of bytes from the given array to the sensor,
// starting at the given register

void VL53L0X_writeMulti(uint8_t reg, uint8_t const * src, uint8_t count)
{
	uint8_t status;

	status = HAL_I2C_Mem_Write(&hi2c1, VL53L0X_address<<1, reg, I2C_MEMADD_SIZE_8BIT, (uint8_t*) src, count, 100);
	if (status) VL53L0X_error("I2C Error WriteMulti",reg);

}

// Read an arbitrary number of bytes from the sensor, starting at the given
// register, into the given array

void VL53L0X_readMulti(uint8_t reg, uint8_t * dst, uint8_t count)
{
	uint8_t status;

	status = HAL_I2C_Mem_Read(&hi2c1, VL53L0X_address<<1, reg, I2C_MEMADD_SIZE_8BIT, dst, count, 100);
	if (status) VL53L0X_error("I2C Error ReadMulti",reg);

}
