/*
 * timer_elapsed.c
 *
 *  Created on: 08.11.2019
 *      Author: Tobias Ellermeyer
 */

#include "main.h"
extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim4;

volatile uint16_t overflow_cnt = 0;

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{


	   overflow_cnt+=5;

   if(htim == &htim3)
   {
	   overflow_cnt++;
   }



}

