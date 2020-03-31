/*
  ___          _____     __  __
   _/    _  _ (_  | |\/|  _)  _)
  /__|_||||(_)__) | |  | __) /__

 Fachhochschule Suedwestfalen
 Mechatronik/Mikrocomputer
 Prof. Dr.-Ing. Tobias Ellermeyer

 File: motors.c
 Path:  zumolib/src/zumo

 Created:	   11.09.2019
 Author:       Tobias Ellermeyer


 28.03.2020 (TE):
 	 Fixed sign of PWM for driving both motors forward

*/

/**
 @file motors.c
 @code #include <zumo/motors.h> @endcode

 @brief Basic functions to control motor pwm and read out encoders

 Motors use Timer 2, Channel 1&2 for PWM generation
 Encoders are implemented in timer 3 (left) and 4 (right)

 @author Tobias Ellermeyer
 */

#include "main.h"

// Timer handles
/// Handle für Timer2
extern TIM_HandleTypeDef htim2;
/// Handle für Timer3
extern TIM_HandleTypeDef htim3;
/// Handle für Timer4
extern TIM_HandleTypeDef htim4;


static volatile int32_t enc_left_10ms = 0;
static volatile int32_t enc_right_10ms = 0;

static int32_t enc_delta(uint16_t, uint16_t);

/**
 * @brief Initialize motors and encoders
 *
 * Motors will be initialized with PWM=0 (=off)
 * Encoders will be set to 0
 *
 * @params none
 * @return none
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
	__HAL_TIM_SET_COUNTER(&htim3,0);
	__HAL_TIM_SET_COUNTER(&htim4,0);
	HAL_TIM_Encoder_Start_IT(&htim3,TIM_CHANNEL_1);
	HAL_TIM_Encoder_Start_IT(&htim4,TIM_CHANNEL_1);
}

/**
 * @brief: Internal routine to calculate delta based on actual and old value
 *
 * Over-/underflow will be taken into account.
 *
 */
static int32_t enc_delta(uint16_t enc_act, uint16_t enc_old)
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
 * @brief periodically read out encoders and check for over-/underflow
 *
 * Routine is called every 10ms from SysTick (see zumo/systick.c)
 *
 */
void motors_encoder_callback()
{
	uint32_t tmp;
	static uint16_t enc_left_old;
	static uint16_t enc_right_old;
	uint16_t enc_left, enc_right;

	// read out values
	enc_left  = __HAL_TIM_GET_COUNTER(&htim3);
	enc_right = __HAL_TIM_GET_COUNTER(&htim4);

	// Handle overflow
	enc_left_10ms  += enc_delta(enc_left,  enc_left_old);
	enc_right_10ms += enc_delta(enc_right, enc_right_old);

	// Keep old values values
	enc_left_old = enc_left;
	enc_right_old = enc_right;
}

/**
 * @brief Return current value of left encoder
 *
 * Update: Changed to int32-value
 *
 * Possible improvement: Currently only the value of the last 10ms interval is returned
 * 						 could be improved to add also the remaining steps until then
 *
 * @param none
 * @return int32 (signed) of encoder steps
 *
 */
int32_t motors_encoder_left()
{
//	return __HAL_TIM_GET_COUNTER(&htim3);
	return enc_left_10ms;
}

/**
 * @brief Return current value of right encoder
 *
 * Update: Changed to int32-value
 *
 * Possible improvement: Currently only the value of the last 10ms interval is returned
 * 						 could be improved to add also the remaining steps until then
 *
 * @param none
 * @return int32 (signed) of encoder steps
 *
 */
int32_t motors_encoder_right()
{
	return enc_right_10ms;
//	return __HAL_TIM_GET_COUNTER(&htim4);
}

/**
 * @brief set PWM value for left motor
 *
 * Positive values will drive forward, negative backwards; Range -100 ... 100
 * If value is outside these limits no changes will take place...
 *
 * @param pwm: Value for PWM (-100 ... 100)
 *
 * @return none
 *
 */
void motors_set_left_pwm(int16_t pwm)
{
	// Check for direction
	if ( (pwm<0) && (pwm>=-100) )
	{
		// Change direction
		HAL_GPIO_WritePin(MOT_L_DIR_GPIO_Port, MOT_L_DIR_Pin, GPIO_PIN_SET);
		// set pwm value (minus sign needed as pwm is <0 in this case)
		__HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_1,-pwm);
	}
	else if (pwm<=100)
	{
		HAL_GPIO_WritePin(MOT_L_DIR_GPIO_Port, MOT_L_DIR_Pin, GPIO_PIN_RESET);
		__HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_1,pwm);
	}
}

/**
 * @brief set PWM value for left motor
 *
 * Positive values will drive forward, negative backwards; Range -100 ... 100
 * If value is outside these limits no changes will take place...
 *
 * @param pwm: Value for PWM (-100 ... 100)
 *
 * @return none
 *
 */
void motors_set_right_pwm(int16_t pwm)
{
	if ( (pwm<0) && (pwm>=-100) )
	{
		// Check for direction
		HAL_GPIO_WritePin(MOT_R_DIR_GPIO_Port, MOT_R_DIR_Pin, GPIO_PIN_RESET);
		// set pwm value (minus sign needed as pwm is <0 in this case)
		__HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_2,-pwm);
	}
	else if (pwm<=100)
	{
		HAL_GPIO_WritePin(MOT_R_DIR_GPIO_Port, MOT_R_DIR_Pin, GPIO_PIN_SET);
		__HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_2,pwm);
	}
}

/**
 * @brief One routine to set bot PWMs
 *
 * For more details, see motors_set_left_pwm() or motors_set_right_pwm()
 *
 * @param
 * 		pwm_left, pwm_right: Values for PWM of both motors (Range: -100 ... 100)
 *
 * @return none
 */
void motors_set_both_pwm(int16_t pwm_left, int16_t pwm_right)
{
	motors_set_left_pwm(pwm_left);
	motors_set_right_pwm(pwm_right);
}

/**
 * @brief Stop both motors
 *
 * (no breaking, just setting pwm to zero)
 *
 * @param none
 * @return none
 *
 */
void motors_stop()
{
	motors_set_left_pwm(0);
	motors_set_right_pwm(0);
}
