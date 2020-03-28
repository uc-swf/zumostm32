/*
  ___          _____     __  __
   _/    _  _ (_  | |\/|  _)  _)
  /__|_||||(_)__) | |  | __) /__

 Fachhochschule Südwestfalen
 Mechatronik/Mikrocomputer
 Prof. Dr.-Ing. Tobias Ellermeyer

 Datei: motors.c
 Pfad:  zumolib/src/zumo

 Erstellt:    11.09.2019
 Autor:       Tobias Ellermeyer


 28.03.2020 (TE):
 	 PWM positiv ist nun bei beiden Motoren "vorwaerts"

*/

/**
 @file motors.c
 @code #include <zumo/motors.h> @endcode

 @brief Grundlegende Funktionen, um die Motoren mittels PWM anzusteuern

 @author Tobias Ellermeyer
 */

#include "main.h"

// Timer handles
//extern TIM_HandleTypeDef htim1;
/// Handle für Timer2
extern TIM_HandleTypeDef htim2;
/// Handle für Timer3
extern TIM_HandleTypeDef htim3;
/// Handle für Timer4
extern TIM_HandleTypeDef htim4;


static volatile int32_t enc_left_10ms = 0;
static volatile int32_t enc_right_10ms = 0;

static int32_t enc_over_underflow(uint16_t enc_act, uint16_t enc_old);

/**
 * @brief Initialisieren der Motoren und Encoder
 *
 * Für die Motoren wird Timer 2, Kanal 1 und 2 verwendet;
 * die Encoder nutzen Timer 3 und 4
 *
 * Die Routine initialisiert die PWM mit dem Wert 0 (=Stillstand)
 *
 */
void motors_init()
{
	/* PWM Setup */
	HAL_TIM_Base_Start(&htim2);
	HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_2);

	/* Set Speed to Zero at beginning */
	__HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_1,0);
	__HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_2,0);

	/* Encoder Setup */
	HAL_TIM_Encoder_Start_IT(&htim3,TIM_CHANNEL_1);
	HAL_TIM_Encoder_Start_IT(&htim4,TIM_CHANNEL_1);

}

static int32_t enc_over_underflow(uint16_t enc_act, uint16_t enc_old)
{
	int32_t tmp;

	tmp = (int32_t)enc_act - (int32_t)enc_old;

	// Check for overflow	(0x4000 = 16384; 0xC000=49152)
	if ( (enc_act < 0x4000) && (enc_old > 0xC000) )
	{
		tmp+=65536;
	}
	// Check for underflow
	if ( (enc_act > 0xC000) && (enc_old < 0x4000) )
	{
		tmp-=65536;
	}
	return (tmp);
}

/**
 * @brief Encoder-Zählstande auswerten; wird über SysTick-ISR alle 10ms aufgerufen
 *
 */
void motors_encoder_callback()
{
	uint32_t tmp;
	static uint16_t enc_left_old;
	static uint16_t enc_right_old;
	uint16_t enc_left, enc_right;

	enc_left  = __HAL_TIM_GET_COUNTER(&htim3);
	enc_right = __HAL_TIM_GET_COUNTER(&htim4);

	// Handle overflow
	enc_left_10ms  += enc_over_underflow(enc_left,  enc_left_old);
	enc_right_10ms += enc_over_underflow(enc_right, enc_right_old);

	// Store values
	enc_left_old = enc_left;
	enc_right_old = enc_right;

}

/**
 * @brief Encoder-Zählstand linker Motor auslesen
 *
 * (Inzwischen als in32 Wert...)
 *
 */
int32_t motors_encoder_left()
{
//	return __HAL_TIM_GET_COUNTER(&htim3);
	return enc_left_10ms;
}

/**
 * @brief Encoder-Zählstand rechter Motor auslesen
 *
 * (Inzwischen als in32 Wert...)
 *
 */
int32_t motors_encoder_right()
{
	return enc_right_10ms;
//	return __HAL_TIM_GET_COUNTER(&htim4);
}

/**
 * @brief Setzen des PWM-Wertes für linken Motor
 *
 * @param pwm Wert für die PWM (Wertebereich: -100 ... 100)
 *
 * Positive Werte -> vorwärts
 *
 */
void motors_set_left_pwm(int16_t pwm)
{
	if ( (pwm<0) && (pwm>=-100) )
	{
		// Drehrichtung setzen
		HAL_GPIO_WritePin(MOT_L_DIR_GPIO_Port, MOT_L_DIR_Pin, GPIO_PIN_SET);
		// PWM Setzen
		__HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_1,-pwm);
	}
	else if (pwm<=100)
	{
		HAL_GPIO_WritePin(MOT_L_DIR_GPIO_Port, MOT_L_DIR_Pin, GPIO_PIN_RESET);
		__HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_1,pwm);
	}
}

/**
 * @brief Setzen des PWM-Wertes für rechten Motor
 *
 * @param pwm Wert für die PWM (Wertebereich: -100 ... 100)
 *
 * Positive Werte -> vorwärts
 *
 */
void motors_set_right_pwm(int16_t pwm)
{
	if ( (pwm<0) && (pwm>=-100) )
	{
		// Drehrichtung setzen
		HAL_GPIO_WritePin(MOT_R_DIR_GPIO_Port, MOT_R_DIR_Pin, GPIO_PIN_RESET);
		// PWM Setzen
		__HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_2,-pwm);
	}
	else if (pwm<=100)
	{
		HAL_GPIO_WritePin(MOT_R_DIR_GPIO_Port, MOT_R_DIR_Pin, GPIO_PIN_SET);
		__HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_2,pwm);
	}
}

/**
 * @brief Setzen des PWM-Wertes fuer beide Motorn
 *
 * @param pwm_left, pwm_right Wert2 für die PWM (Wertebereich: -100 ... 100)
 * Positive Werte -> vorwärts
 */
void motors_set_both_pwm(int16_t pwm_left, int16_t pwm_right)
{
	motors_set_left_pwm(pwm_left);
	motors_set_right_pwm(pwm_right);
}

/**
 * @brief Beide Motoren anhalten
 *
 */
void motors_stop()
{
	motors_set_left_pwm(0);
	motors_set_right_pwm(0);
}
