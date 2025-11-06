/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include "stm32g4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "core_cm4.h"
#include "tim.h"

#include "iic.h"
#include "stdio.h"

//心率传感器
#include "max30102.h"
#include "blood.h"
#include "algorithm.h"

//姿态传感器
#include "mpu6050.h"//MPU6050驱动库
#include "mpuiic.h"
#include "inv_mpu.h"//陀螺仪驱动库
#include "inv_mpu_dmp_motion_driver.h" //DMP姿态解读库
//温度传感器
#include "ds18b20.h"
//气压传感器
#include "bmp388.h"
//人体热释电	
#include "as312.h"
//压力传感器
#include "hx711.h"
//温湿度传感器
#include "aht20.h"
//火焰传感器
#include "FlameSensor.h"
//红外循迹传感器
#include "InfraredTrack.h"
//光敏传感器
#include "LightSensor.h"
//声音传感器
#include "SoundSensor.h"
//超声波传感器
#include "Ultrasonic.h"
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
#define SOFT_I2C_SDA_Pin GPIO_PIN_6
#define SOFT_I2C_SDA_GPIO_Port GPIOA
#define SOFT_I2C_SCL_Pin GPIO_PIN_7
#define SOFT_I2C_SCL_GPIO_Port GPIOA

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
