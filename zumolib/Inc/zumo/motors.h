/*
  ___          _____     __  __
   _/    _  _ (_  | |\/|  _)  _)
  /__|_||||(_)__) | |  | __) /__

 Fachhochschule Suedwestfalen
 Mechatronik/Mikrocomputer
 Prof. Dr.-Ing. Tobias Ellermeyer

 File: motors.h
 Path:  zumolib/inc/zumo

 Created:    23.09.2019
 Author:     Tobias Ellermeyer
*/


#ifndef INC_ZUMO_MOTORS_H_
#define INC_ZUMO_MOTORS_H_

#include <stdint.h>

void motors_init();
void motors_set_left_pwm(int16_t pwm);
void motors_set_right_pwm(int16_t pwm);
void motors_set_both_pwm(int16_t pwm_left, int16_t pwm_right);
void motors_stop();
int32_t motors_encoder_left();
int32_t motors_encoder_right();

#endif /* INC_ZUMO_MOTORS_H_ */
