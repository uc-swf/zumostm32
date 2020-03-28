/*
  ___          _____     __  __
   _/    _  _ (_  | |\/|  _)  _)
  /__|_||||(_)__) | |  | __) /__

 Fachhochschule Südwestfalen
 Mechatronik/Mikrocomputer
 Prof. Dr.-Ing. Tobias Ellermeyer

 Datei: distance.h
 Pfad:  zumolib/inc/zumo

 Erstellt:    02.12.2019
 Autor:       Tobias Ellermeyer
*/


#ifndef ZUMOLIB_INC_ZUMO_DISTANCE_H_
#define ZUMOLIB_INC_ZUMO_DISTANCE_H_


#define TOF_I2CADDR_FF 		0x30
#define TOF_I2CADDR_FL 		0x31
#define TOF_I2CADDR_FR 		0x32
#define TOF_I2CADDR_LL 		0x33
#define TOF_I2CADDR_RR 		0x34


uint16_t distance_single_mm(uint8_t sensor);
uint8_t distance_init();
uint8_t distance_init_scan();
uint8_t distance_start_scan();
uint8_t distance_get_scan_values(uint16_t *values);


#endif /* ZUMOLIB_INC_ZUMO_DISTANCE_H_ */
