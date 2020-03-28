/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define BTN_DN_Pin GPIO_PIN_13
#define BTN_DN_GPIO_Port GPIOC
#define BTN_OK_Pin GPIO_PIN_14
#define BTN_OK_GPIO_Port GPIOC
#define BTN_USR_Pin GPIO_PIN_15
#define BTN_USR_GPIO_Port GPIOC
#define BNO_INT_Pin GPIO_PIN_0
#define BNO_INT_GPIO_Port GPIOC
#define BNO_NRST_Pin GPIO_PIN_1
#define BNO_NRST_GPIO_Port GPIOC
#define GPIO_PINHDR0_Pin GPIO_PIN_2
#define GPIO_PINHDR0_GPIO_Port GPIOC
#define TOF_LL_INT_Pin GPIO_PIN_3
#define TOF_LL_INT_GPIO_Port GPIOC
#define DISP_CS_Pin GPIO_PIN_4
#define DISP_CS_GPIO_Port GPIOA
#define DISP_RST_Pin GPIO_PIN_4
#define DISP_RST_GPIO_Port GPIOC
#define DISP_DC_Pin GPIO_PIN_5
#define DISP_DC_GPIO_Port GPIOC
#define LED_USR0_Pin GPIO_PIN_0
#define LED_USR0_GPIO_Port GPIOB
#define LED_USR1_Pin GPIO_PIN_1
#define LED_USR1_GPIO_Port GPIOB
#define BOOT1_Pin GPIO_PIN_2
#define BOOT1_GPIO_Port GPIOB
#define PINHDR_PWM0_Pin GPIO_PIN_10
#define PINHDR_PWM0_GPIO_Port GPIOB
#define LED_FRONT_Pin GPIO_PIN_11
#define LED_FRONT_GPIO_Port GPIOB
#define MOT_L_DIR_Pin GPIO_PIN_12
#define MOT_L_DIR_GPIO_Port GPIOB
#define MOT_R_DIR_Pin GPIO_PIN_13
#define MOT_R_DIR_GPIO_Port GPIOB
#define LED_BACK_R_Pin GPIO_PIN_14
#define LED_BACK_R_GPIO_Port GPIOB
#define LED_BACK_L_Pin GPIO_PIN_15
#define LED_BACK_L_GPIO_Port GPIOB
#define MOT_L_ENCA_Pin GPIO_PIN_6
#define MOT_L_ENCA_GPIO_Port GPIOC
#define MOT_L_ENCB_Pin GPIO_PIN_7
#define MOT_L_ENCB_GPIO_Port GPIOC
#define BT_PWR_Pin GPIO_PIN_8
#define BT_PWR_GPIO_Port GPIOC
#define BT_KEY_Pin GPIO_PIN_9
#define BT_KEY_GPIO_Port GPIOC
#define PINHDR_PWM1_Pin GPIO_PIN_8
#define PINHDR_PWM1_GPIO_Port GPIOA
#define UART_TX_Pin GPIO_PIN_9
#define UART_TX_GPIO_Port GPIOA
#define UART_RX_Pin GPIO_PIN_10
#define UART_RX_GPIO_Port GPIOA
#define BUZ_PWM_Pin GPIO_PIN_11
#define BUZ_PWM_GPIO_Port GPIOA
#define TOF_RR_INT_Pin GPIO_PIN_12
#define TOF_RR_INT_GPIO_Port GPIOA
#define MOT_L_PWM_Pin GPIO_PIN_15
#define MOT_L_PWM_GPIO_Port GPIOA
#define GPIO_PINHDR1_Pin GPIO_PIN_10
#define GPIO_PINHDR1_GPIO_Port GPIOC
#define GPIO_PINHDR2_Pin GPIO_PIN_11
#define GPIO_PINHDR2_GPIO_Port GPIOC
#define BTN_UP_Pin GPIO_PIN_12
#define BTN_UP_GPIO_Port GPIOC
#define TOF_FR_INT_Pin GPIO_PIN_2
#define TOF_FR_INT_GPIO_Port GPIOD
#define MOT_R_PWM_Pin GPIO_PIN_3
#define MOT_R_PWM_GPIO_Port GPIOB
#define TOF_FF_INT_Pin GPIO_PIN_4
#define TOF_FF_INT_GPIO_Port GPIOB
#define TOF_FL_INT_Pin GPIO_PIN_5
#define TOF_FL_INT_GPIO_Port GPIOB
#define MOT_R_ENCA_Pin GPIO_PIN_6
#define MOT_R_ENCA_GPIO_Port GPIOB
#define MOT_R_ENCB_Pin GPIO_PIN_7
#define MOT_R_ENCB_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
