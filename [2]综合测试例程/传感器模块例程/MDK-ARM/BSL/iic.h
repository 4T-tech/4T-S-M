#ifndef __IIC_H
#define __IIC_H

#include "main.h"
#include "stdbool.h"

// 软件I2C引脚配置 - 根据实际硬件连接修改
#define SOFT_I2C_SCL_PORT         GPIOA
#define SOFT_I2C_SCL_PIN          GPIO_PIN_6
#define SOFT_I2C_SDA_PORT         GPIOA
#define SOFT_I2C_SDA_PIN          GPIO_PIN_7

// 引脚电平控制宏
#define IIC_SCL_1                 HAL_GPIO_WritePin(SOFT_I2C_SCL_PORT, SOFT_I2C_SCL_PIN, GPIO_PIN_SET)
#define IIC_SCL_0                 HAL_GPIO_WritePin(SOFT_I2C_SCL_PORT, SOFT_I2C_SCL_PIN, GPIO_PIN_RESET)
#define IIC_SDA_1                 HAL_GPIO_WritePin(SOFT_I2C_SDA_PORT, SOFT_I2C_SDA_PIN, GPIO_PIN_SET)
#define IIC_SDA_0                 HAL_GPIO_WritePin(SOFT_I2C_SDA_PORT, SOFT_I2C_SDA_PIN, GPIO_PIN_RESET)
#define IIC_SDA_READ              HAL_GPIO_ReadPin(SOFT_I2C_SDA_PORT, SOFT_I2C_SDA_PIN)

// 函数声明
void SOFT_I2C_Init(void);
void SOFT_I2C_Start(void);
void SOFT_I2C_Stop(void);
uint8_t SOFT_I2C_Wait_Ack(void);
void SOFT_I2C_Ack(void);
void SOFT_I2C_NAck(void);
void SOFT_I2C_Send_Byte(uint8_t data);
uint8_t SOFT_I2C_Read_Byte(void);
uint8_t SOFT_I2C_Write_Reg(uint8_t dev_addr, uint8_t reg_addr, uint8_t data);
uint8_t SOFT_I2C_Read_Reg(uint8_t dev_addr, uint8_t reg_addr, uint8_t *data);
uint8_t SOFT_I2C_Read_Multi_Reg(uint8_t dev_addr, uint8_t reg_addr, uint8_t *data, uint16_t len);
void SOFT_I2C_Scan_Bus(void);
uint8_t SOFT_I2C_Test_Device(uint8_t dev_addr);
void SOFT_I2C_Debug_Pins(void);
void IIC_Delay_us(uint32_t us);
uint8_t MPU_IIC_Read_Byte(unsigned char ack);

#endif
