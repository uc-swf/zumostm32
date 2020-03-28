/*
 * hw_basics.c
 *
 *  Created on: 20.08.2019
 *      Author: Tobias Ellermeyer
 */

#include "main.h"

void 		motors_encoder_callback();


// SysTick Timer; this is called every 1 ms
void SysTick_Handler(void)
{
	static int mscnt=0;

	// Must be called, otherwise HAL_Delay() fails
	HAL_IncTick();

	mscnt++;
	if (mscnt==10)	// Backup/Calculate Encoder values every 10 ms to handle over-/underflows
	{
		mscnt=0;
		motors_encoder_callback();
	}
}
