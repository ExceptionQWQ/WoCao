/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
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
#include "stm32f4xx_hal.h"

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

extern SPI_HandleTypeDef hspi1;
extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim6;
extern UART_HandleTypeDef huart5;
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart3;
extern UART_HandleTypeDef huart4;

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LCD_CS_Pin GPIO_PIN_6
#define LCD_CS_GPIO_Port GPIOE
#define MOTOR_DRIVER_EN_Pin GPIO_PIN_0
#define MOTOR_DRIVER_EN_GPIO_Port GPIOF
#define LED0_Pin GPIO_PIN_9
#define LED0_GPIO_Port GPIOF
#define WHEEL_BACK_RIGHT_OUTPUT_B_Pin GPIO_PIN_1
#define WHEEL_BACK_RIGHT_OUTPUT_B_GPIO_Port GPIOA
#define WHEEL_FRONT_LEFT_OUTPUT_B_Pin GPIO_PIN_4
#define WHEEL_FRONT_LEFT_OUTPUT_B_GPIO_Port GPIOC
#define WHEEL_BACK_RIGHT_OUTPUT_A_Pin GPIO_PIN_9
#define WHEEL_BACK_RIGHT_OUTPUT_A_GPIO_Port GPIOE
#define LCD_RST_Pin GPIO_PIN_10
#define LCD_RST_GPIO_Port GPIOE
#define WHEEL_FRONT_LEFT_OUTPUT_A_Pin GPIO_PIN_11
#define WHEEL_FRONT_LEFT_OUTPUT_A_GPIO_Port GPIOE
#define LCD_RS_Pin GPIO_PIN_12
#define LCD_RS_GPIO_Port GPIOE
#define WHEEL_BACK_LEFT_OUTPUT_A_Pin GPIO_PIN_13
#define WHEEL_BACK_LEFT_OUTPUT_A_GPIO_Port GPIOE
#define WHEEL_FRONT_RIGHT_OUTPUT_A_Pin GPIO_PIN_14
#define WHEEL_FRONT_RIGHT_OUTPUT_A_GPIO_Port GPIOE
#define WHEEL_BACK_LEFT_INPUT_A_Pin GPIO_PIN_12
#define WHEEL_BACK_LEFT_INPUT_A_GPIO_Port GPIOD
#define WHEEL_BACK_LEFT_INPUT_A_EXTI_IRQn EXTI15_10_IRQn
#define WHEEL_BACK_LEFT_INPUT_B_Pin GPIO_PIN_13
#define WHEEL_BACK_LEFT_INPUT_B_GPIO_Port GPIOD
#define WHEEL_BACK_LEFT_INPUT_B_EXTI_IRQn EXTI15_10_IRQn
#define WHEEL_BACK_LEFT_OUTPUT_B_Pin GPIO_PIN_14
#define WHEEL_BACK_LEFT_OUTPUT_B_GPIO_Port GPIOD
#define WHEEL_FRONT_RIGHT_OUTPUT_B_Pin GPIO_PIN_15
#define WHEEL_FRONT_RIGHT_OUTPUT_B_GPIO_Port GPIOD
#define WHEEL_FRONT_RIGHT_INPUT_A_Pin GPIO_PIN_6
#define WHEEL_FRONT_RIGHT_INPUT_A_GPIO_Port GPIOC
#define WHEEL_FRONT_RIGHT_INPUT_A_EXTI_IRQn EXTI9_5_IRQn
#define WHEEL_FRONT_RIGHT_INPUT_B_Pin GPIO_PIN_7
#define WHEEL_FRONT_RIGHT_INPUT_B_GPIO_Port GPIOC
#define WHEEL_FRONT_RIGHT_INPUT_B_EXTI_IRQn EXTI9_5_IRQn
#define WHEEL_BACK_RIGHT_INPUT_A_Pin GPIO_PIN_15
#define WHEEL_BACK_RIGHT_INPUT_A_GPIO_Port GPIOA
#define WHEEL_BACK_RIGHT_INPUT_A_EXTI_IRQn EXTI15_10_IRQn
#define WHEEL_BACK_RIGHT_INPUT_B_Pin GPIO_PIN_3
#define WHEEL_BACK_RIGHT_INPUT_B_GPIO_Port GPIOB
#define WHEEL_BACK_RIGHT_INPUT_B_EXTI_IRQn EXTI3_IRQn
#define WHEEL_FRONT_LEFT_INPUT_A_Pin GPIO_PIN_4
#define WHEEL_FRONT_LEFT_INPUT_A_GPIO_Port GPIOB
#define WHEEL_FRONT_LEFT_INPUT_A_EXTI_IRQn EXTI4_IRQn
#define WHEEL_FRONT_LEFT_INPUT_B_Pin GPIO_PIN_5
#define WHEEL_FRONT_LEFT_INPUT_B_GPIO_Port GPIOB
#define WHEEL_FRONT_LEFT_INPUT_B_EXTI_IRQn EXTI9_5_IRQn
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
