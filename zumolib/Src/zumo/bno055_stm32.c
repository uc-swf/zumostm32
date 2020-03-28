/*
  ___          _____     __  __
   _/    _  _ (_  | |\/|  _)  _)
  /__|_||||(_)__) | |  | __) /__

 Fachhochschule Südwestfalen
 Mechatronik/Mikrocomputer
 Prof. Dr.-Ing. Tobias Ellermeyer

 Datei: bno055_stm32.c
 Pfad:  zumolib/src/zumo

 Erstellt:    20.09.2019
 Autor:       Tobias Ellermeyer
*/

#ifndef BNO055_STM32_H_
#define BNO055_STM32_H_
#include <zumo/bno055.h>
#include "main.h"

/**
 @file bno055_stm32.c

 @brief Low-Level I2C Funktionen, STM32 spezifisch

 @author Tobias Ellermeyer
 */


#ifdef __cplusplus
  extern "C" {
#endif

#ifdef BNO_DEBUG
#include <stdio.h>
#endif


//#include "stm32f1xx_hal_i2c.h"

#ifdef FREERTOS_ENABLED
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"
#endif


extern I2C_HandleTypeDef hi2c1;

#define BNO055_I2C_ADDR BNO055_I2C_ADDR_LO

void bno055_delay(int time) {
#ifdef FREERTOS_ENABLED
  osDelay(time);
#else
  HAL_Delay(time);
#endif
}

#ifdef BNO_DEBUG
void bno_i2c_error(uint8_t status)
{
	  uint32_t error;
	  HAL_I2C_StateTypeDef state;

	  if (status == HAL_ERROR) {
	    printf("BNO_I2C: HAL_I2C_Master_Transmit HAL_ERROR\r\n");
	  } else if (status == HAL_TIMEOUT) {
		  printf("BNO_I2C: HAL_I2C_Master_Transmit HAL_TIMEOUT\r\n");
	  } else if (status == HAL_BUSY) {
		  printf("BNO_I2C: HAL_I2C_Master_Transmit HAL_BUSY\r\n");
	  } else {
		  printf("BNO_I2C: Unknown status data %d", status);
	  }

	  error = HAL_I2C_GetError(&hi2c1);
	  if (error == HAL_I2C_ERROR_NONE) {
	    return;
	  } else if (error == HAL_I2C_ERROR_BERR) {
		  printf("BNO_I2C: HAL_I2C_ERROR_BERR\r\n");
	  } else if (error == HAL_I2C_ERROR_ARLO) {
		  printf("BNO_I2C: HAL_I2C_ERROR_ARLO\r\n");
	  } else if (error == HAL_I2C_ERROR_AF) {
		  printf("BNO_I2C: HAL_I2C_ERROR_AF\r\n");
	  } else if (error == HAL_I2C_ERROR_OVR) {
		  printf("BNO_I2C: HAL_I2C_ERROR_OVR\r\n");
	  } else if (error == HAL_I2C_ERROR_DMA) {
		  printf("BNO_I2C: HAL_I2C_ERROR_DMA\r\n");
	  } else if (error == HAL_I2C_ERROR_TIMEOUT) {
		  printf("BNO_I2C: HAL_I2C_ERROR_TIMEOUT\r\n");
	  }

	  state = HAL_I2C_GetState(&hi2c1);
	  if (state == HAL_I2C_STATE_RESET) {
		  printf("BNO_I2C: HAL_I2C_STATE_RESET\r\n");
	  } else if (state == HAL_I2C_STATE_READY) {
		  printf("BNO_I2C: HAL_I2C_STATE_RESET\r\n");
	  } else if (state == HAL_I2C_STATE_BUSY) {
		  printf("BNO_I2C: HAL_I2C_STATE_BUSY\r\n");
	  } else if (state == HAL_I2C_STATE_BUSY_TX) {
		  printf("BNO_I2C: HAL_I2C_STATE_BUSY_TX\r\n");
	  } else if (state == HAL_I2C_STATE_BUSY_RX) {
		  printf("BNO_I2C: HAL_I2C_STATE_BUSY_RX\r\n");
	  } else if (state == HAL_I2C_STATE_LISTEN) {
		  printf("BNO_I2C: HAL_I2C_STATE_LISTEN\r\n");
	  } else if (state == HAL_I2C_STATE_BUSY_TX_LISTEN) {
		  printf("BNO_I2C: HAL_I2C_STATE_BUSY_TX_LISTEN\r\n");
	  } else if (state == HAL_I2C_STATE_BUSY_RX_LISTEN) {
		  printf("BNO_I2C: HAL_I2C_STATE_BUSY_RX_LISTEN\r\n");
	  } else if (state == HAL_I2C_STATE_ABORT) {
		  printf("BNO_I2C: HAL_I2C_STATE_ABORT\r\n");
	  } else if (state == HAL_I2C_STATE_TIMEOUT) {
		  printf("BNO_I2C: HAL_I2C_STATE_TIMEOUT\r\n");
	  } else if (state == HAL_I2C_STATE_ERROR) {
		  printf("BNO_I2C: HAL_I2C_STATE_ERROR\r\n");
	  }

}
#endif

void bno055_writeData(uint8_t reg, uint8_t data) {
  uint8_t txdata[2] = {reg, data};
  uint8_t status;
  status = HAL_I2C_Master_Transmit(&hi2c1, BNO055_I2C_ADDR << 1, txdata, sizeof(txdata), 10);
  if (status != HAL_OK) {
#ifdef BNO_DEBUG
	  bno_i2c_error(status);
#endif
	  return;
  }

}

void bno055_readData(uint8_t reg, uint8_t *data, uint8_t len) {

  uint8_t status;

/*  status = HAL_I2C_Master_Transmit(hi2c1, BNO055_I2C_ADDR << 1, &reg, 1, 100);
  printf("Master Transmit \r\n");
  if (status) bno_i2c_error(status);

  HAL_I2C_Master_Receive(hi2c1, BNO055_I2C_ADDR << 1, data, len,  100);
  printf("Master Receive \r\n");
  if (status) bno_i2c_error(status);
*/
  status =  HAL_I2C_Mem_Read(&hi2c1, BNO055_I2C_ADDR<<1, reg, I2C_MEMADD_SIZE_8BIT, data, len, 100);
  //printf("MemREAD reg 0x%02x len=%i data[0]=0x%02x\r\n",reg,len,data[0]);
  if (status) bno_i2c_error(status);

}

#ifdef __cplusplus
  }
#endif

#endif  // BNO055_STM32_H_
