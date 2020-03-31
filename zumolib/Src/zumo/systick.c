/*
  ___          _____     __  __
   _/    _  _ (_  | |\/|  _)  _)
  /__|_||||(_)__) | |  | __) /__

 Fachhochschule Suedwestfalen
 Mechatronik/Mikrocomputer
 Prof. Dr.-Ing. Tobias Ellermeyer

 File:  systick.c
 Path:  zumolib/src/zumo

 Created:	   30.03.2020
 Author:       Tobias Ellermeyer

*/

/**
 @file systick.c

 @brief Reassembles SysTick interrupt to call own routines periodically

 Important:
 	 - There are no routines in this file, which should be called from
     - Check in STM32CubeMX that in "Generate IRQ handler" is disabled
       for "Time base: System tick timer" in NVIC setting -> Code generation

 The SysTick interrupt is issued every millisecond (i.e. 1000 times per second)

 Currently this is used for encoder overflow detection (every 10ms)
 In future, playing sounds may be added to this...

 @author Tobias Ellermeyer
*/

#include "main.h"

void motors_encoder_callback();		// see zumolib/src/zumo/motors.c


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
