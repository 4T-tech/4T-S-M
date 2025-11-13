#ifndef _MAX30102_H
#define _MAX30102_H

#include "main.h"           // HAL库主头文件
#include "iic.h"           // I2C驱动头文件
#include "stdbool.h"       // 布尔类型支持

// MAX30102设备地址
#define MAX30102_Device_address         0xAE

// 寄存器地址定义
#define REG_INTR_STATUS_1       0x00
#define REG_INTR_STATUS_2       0x01
#define REG_INTR_ENABLE_1       0x02
#define REG_INTR_ENABLE_2       0x03
#define REG_FIFO_WR_PTR         0x04
#define REG_OVF_COUNTER         0x05
#define REG_FIFO_RD_PTR         0x06
#define REG_FIFO_DATA           0x07
#define REG_FIFO_CONFIG         0x08
#define REG_MODE_CONFIG         0x09
#define REG_SPO2_CONFIG         0x0A
#define REG_LED1_PA             0x0C
#define REG_LED2_PA             0x0D
#define REG_PILOT_PA            0x10
#define REG_MULTI_LED_CTRL1     0x11
#define REG_MULTI_LED_CTRL2     0x12
#define REG_TEMP_INTR           0x1F
#define REG_TEMP_FRAC           0x20
#define REG_TEMP_CONFIG         0x21
#define REG_PROX_INT_THRESH     0x30
#define REG_REV_ID              0xFE
#define REG_PART_ID             0xFF

// 中断引脚配置（根据实际硬件连接修改）
#define MAX30102_INTPin_PORT            GPIOA
#define MAX30102_INTPin_PIN             GPIO_PIN_5
#define MAX30102_INTPin_Read()          HAL_GPIO_ReadPin(MAX30102_INTPin_PORT, MAX30102_INTPin_PIN)

// LED指示灯配置（可选，用于调试）
#define Led_PORT                         GPIOC
#define Led_PIN                          GPIO_PIN_9
#define LED_ON()                         HAL_GPIO_WritePin(Led_PORT, Led_PIN, GPIO_PIN_SET)
#define LED_OFF()                        HAL_GPIO_WritePin(Led_PORT, Led_PIN, GPIO_PIN_RESET)
#define LED_TOGGLE()                     HAL_GPIO_TogglePin(Led_PORT, Led_PIN)

// 采样频率
#define SAMPLES_PER_SECOND               100

// 外部变量声明
extern uint32_t fifo_red;    // 改为uint32_t以支持18位数据
extern uint32_t fifo_ir;     // 改为uint32_t以支持18位数据

// 函数声明
void MAX30102_GPIO_Init(void);
uint8_t MAX30102_Reset(void);
void MAX30102_Config(void);
void MAX30102_Read_FIFO(void);
uint8_t MAX30102_Init(void);
uint8_t MAX30102_Read_Reg(uint8_t reg_addr, uint8_t *data);
uint8_t MAX30102_Write_Reg(uint8_t reg_addr, uint8_t data);
uint8_t MAX30102_Read_FIFO_Data(uint32_t *red_value, uint32_t *ir_value);
void MAX30102_HeartRate_SPO2_Calculate(float *heart_rate, float *spo2);

#endif
