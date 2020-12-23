/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
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
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <string.h>
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

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define MEMS_SCK5_Pin GPIO_PIN_7
#define MEMS_SCK5_GPIO_Port GPIOF
#define MEMS_MISO5_Pin GPIO_PIN_8
#define MEMS_MISO5_GPIO_Port GPIOF
#define MEMS_MOSI5_Pin GPIO_PIN_9
#define MEMS_MOSI5_GPIO_Port GPIOF
#define MEMS_CS_Pin GPIO_PIN_1
#define MEMS_CS_GPIO_Port GPIOC
#define USER_INT_Pin GPIO_PIN_0
#define USER_INT_GPIO_Port GPIOA
#define USER_INT_EXTI_IRQn EXTI0_IRQn
#define MEMS_INT1_Pin GPIO_PIN_1
#define MEMS_INT1_GPIO_Port GPIOA
#define MEMS_INT1_EXTI_IRQn EXTI1_IRQn
#define MEMS_INT2_Pin GPIO_PIN_2
#define MEMS_INT2_GPIO_Port GPIOA
#define MEMS_INT2_EXTI_IRQn EXTI2_IRQn
#define NEXTION_RX_Pin GPIO_PIN_3
#define NEXTION_RX_GPIO_Port GPIOA
#define SPI1_CS_Pin GPIO_PIN_4
#define SPI1_CS_GPIO_Port GPIOA
#define SD_CD_Pin GPIO_PIN_4
#define SD_CD_GPIO_Port GPIOC
#define PWM_OUT_Pin GPIO_PIN_10
#define PWM_OUT_GPIO_Port GPIOB
#define TP_SDA3_Pin GPIO_PIN_9
#define TP_SDA3_GPIO_Port GPIOC
#define TP_SCL3_Pin GPIO_PIN_8
#define TP_SCL3_GPIO_Port GPIOA
#define TP_INT_Pin GPIO_PIN_15
#define TP_INT_GPIO_Port GPIOA
#define TP_INT_EXTI_IRQn EXTI15_10_IRQn
#define NEXTION_TX_Pin GPIO_PIN_5
#define NEXTION_TX_GPIO_Port GPIOD
#define LED3_Pin GPIO_PIN_13
#define LED3_GPIO_Port GPIOG
#define LED4_Pin GPIO_PIN_14
#define LED4_GPIO_Port GPIOG
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
