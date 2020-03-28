/*
  ___          _____     __  __
   _/    _  _ (_  | |\/|  _)  _)
  /__|_||||(_)__) | |  | __) /__

 Fachhochschule Südwestfalen
 Mechatronik/Mikrocomputer
 Prof. Dr.-Ing. Tobias Ellermeyer

 Datei: motors.h
 Pfad:  zumolib/inc/zumo

 Erstellt:    23.09.2019
 Autor:       Tobias Ellermeyer
*/


#ifndef INC_ZUMO_MOTORS_H_
#define INC_ZUMO_MOTORS_H_

void motors_init();
void motors_set_left_pwm(int16_t pwm);
void motors_set_right_pwm(int16_t pwm);
void motors_set_both_pwm(int16_t pwm_left, int16_t pwm_right);
void motors_stop();
int motors_encoder_left();
int motors_encoder_right();

#endif /* INC_ZUMO_MOTORS_H_ */
