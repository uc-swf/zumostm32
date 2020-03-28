/*
  ___          _____     __  __
   _/    _  _ (_  | |\/|  _)  _)
  /__|_||||(_)__) | |  | __) /__

 Fachhochschule Südwestfalen
 Mechatronik/Mikrocomputer
 Prof. Dr.-Ing. Tobias Ellermeyer

 Datei: gpio_sx1508.h
 Pfad:  zumolib/inc/gpio_sx1508.h

 Erstellt:    23.09.2019
 Autor:       Tobias Ellermeyer
*/


#ifndef INC_ZUMO_GPIO_SX1508_H_
#define INC_ZUMO_GPIO_SX1508_H_



uint8_t gpio_sx1508_init();
uint8_t gpio_sx1508_out(uint8_t val);

#define TOF_ENABLE_FL			    0x01
#define TOF_ENABLE_LL			    0x02
#define TOF_ENABLE_RR			    0x20
#define TOF_ENABLE_FR			    0x40
#define TOF_ENABLE_FF			    0x80
#define TOF_ENABLE_ALL			    0xE3

#endif /* INC_ZUMO_GPIO_SX1508_H_ */
