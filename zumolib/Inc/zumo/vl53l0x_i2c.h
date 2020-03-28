/*
  ___          _____     __  __
   _/    _  _ (_  | |\/|  _)  _)
  /__|_||||(_)__) | |  | __) /__

 Fachhochschule Südwestfalen
 Mechatronik/Mikrocomputer
 Prof. Dr.-Ing. Tobias Ellermeyer

 Datei: vl53l0x_i2c.h
 Pfad:  zumolib/inc/zumo

 Erstellt:    09.11.2019
 Autor:       Tobias Ellermeyer
*/

#ifndef INC_ZUMO_VL53L0X_I2C_H_
#define INC_ZUMO_VL53L0X_I2C_H_

uint8_t VL53L0X_writeReg(uint8_t reg, uint8_t value);
uint8_t VL53L0X_writeReg16Bit(uint8_t reg, uint16_t value);
uint8_t VL53L0X_writeReg32Bit(uint8_t reg, uint32_t value);
uint8_t VL53L0X_readReg(uint8_t reg);
uint16_t VL53L0X_readReg16Bit(uint8_t reg);
uint32_t VL53L0X_readReg32Bit(uint8_t reg);
void VL53L0X_writeMulti(uint8_t reg, uint8_t const * src, uint8_t count);
void VL53L0X_readMulti(uint8_t reg, uint8_t * dst, uint8_t count);









#endif /* INC_ZUMO_VL53L0X_I2C_H_ */
