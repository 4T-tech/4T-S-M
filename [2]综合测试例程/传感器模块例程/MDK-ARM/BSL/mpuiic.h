#ifndef __MPUIIC_H
#define __MPUIIC_H
#include "main.h"

//正点原子MPU6050通讯线驱动
	   		   
//IO方向设置
#define MPU_SDA_IN()  do { \
    GPIO_InitTypeDef GPIO_InitStruct = {0}; \
    GPIO_InitStruct.Pin = GPIO_PIN_7; \
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT; \
    GPIO_InitStruct.Pull = GPIO_NOPULL; \
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct); \
} while(0)

#define MPU_SDA_OUT() do { \
    GPIO_InitTypeDef GPIO_InitStruct = {0}; \
    GPIO_InitStruct.Pin = GPIO_PIN_7; \
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP; \
    GPIO_InitStruct.Pull = GPIO_NOPULL; \
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH; \
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct); \
} while(0)

//IO操作函数	 
#define MPU_IIC_SCL(x)    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, (GPIO_PinState)(x))	//SCL 
#define MPU_IIC_SDA(x)    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, (GPIO_PinState)(x))	//SDA	 
#define MPU_READ_SDA      HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_7)		//输入SDA

//IIC所有操作函数
void MPU_IIC_Delay(uint32_t us);
void MPU_IIC_Init(void);                //初始化IIC的IO口				 
void MPU_IIC_Start(void);				//发送IIC开始信号
void MPU_IIC_Stop(void);	  			//发送IIC停止信号
void MPU_IIC_Send_Byte(uint8_t txd);			//IIC发送一个字节
uint8_t MPU_IIC_Read_Byte(unsigned char ack);//IIC读取一个字节
uint8_t MPU_IIC_Wait_Ack(void); 				//IIC等待ACK信号
void MPU_IIC_Ack(void);					//IIC发送ACK信号
void MPU_IIC_NAck(void);				//IIC不发送ACK信号

void IMPU_IC_Write_One_Byte(uint8_t daddr, uint8_t addr, uint8_t data);
uint8_t MPU_IIC_Read_One_Byte(uint8_t daddr, uint8_t addr);	  
#endif

