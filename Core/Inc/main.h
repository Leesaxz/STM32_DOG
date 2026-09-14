/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "cmsis_os2.h"

#include "DogActions.h"
#include "MyIIC.h"
#include "OLED.h"
#include "MPU6050.h"
#include "MPU6050Type.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */
extern osMessageQueueId_t action0QueueHandle;
extern osMessageQueueId_t voiceQueueHandle;
extern osMessageQueueId_t sensorQueueHandle;
extern osMessageQueueId_t BluetoothQueueHandle;
/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LED_PC13_Pin GPIO_PIN_13
#define LED_PC13_GPIO_Port GPIOC
#define BLE_TX_Pin GPIO_PIN_2
#define BLE_TX_GPIO_Port GPIOA
#define BLE_RX_Pin GPIO_PIN_3
#define BLE_RX_GPIO_Port GPIOA
#define Servo_FL_Pin GPIO_PIN_6
#define Servo_FL_GPIO_Port GPIOA
#define Servo_FR_Pin GPIO_PIN_7
#define Servo_FR_GPIO_Port GPIOA
#define Servo_RL_Pin GPIO_PIN_0
#define Servo_RL_GPIO_Port GPIOB
#define Servo_RR_Pin GPIO_PIN_1
#define Servo_RR_GPIO_Port GPIOB
#define MOU6050_SCL_Pin GPIO_PIN_12
#define MOU6050_SCL_GPIO_Port GPIOB
#define MPU6050_SDA_Pin GPIO_PIN_13
#define MPU6050_SDA_GPIO_Port GPIOB
#define Data_L_Pin GPIO_PIN_14
#define Data_L_GPIO_Port GPIOB
#define Data_M_Pin GPIO_PIN_15
#define Data_M_GPIO_Port GPIOB
#define Data_H_Pin GPIO_PIN_8
#define Data_H_GPIO_Port GPIOA
#define OLED_SCL_Pin GPIO_PIN_6
#define OLED_SCL_GPIO_Port GPIOB
#define OLED_SDA_Pin GPIO_PIN_7
#define OLED_SDA_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
