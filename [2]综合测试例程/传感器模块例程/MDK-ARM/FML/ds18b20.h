#ifndef __DS18B20_H
#define __DS18B20_H

#include "main.h"

// DS18B20 命令定义
#define DS18B20_CMD_SKIP_ROM        0xCC
#define DS18B20_CMD_CONVERT_T       0x44
#define DS18B20_CMD_READ_SCRATCHPAD 0xBE

// 引脚定义
#define DS18B20_PORT GPIOA
#define DS18B20_PIN  GPIO_PIN_5

// 函数声明
void DS18B20_Init(void);
uint8_t DS18B20_Start(void);
void DS18B20_WriteBit(uint8_t bit);
uint8_t DS18B20_ReadBit(void);
void DS18B20_WriteByte(uint8_t data);
uint8_t DS18B20_ReadByte(void);
float DS18B20_ReadTemp(void);
void DS18B20_DelayUs(uint16_t us);

#endif /* __DS18B20_H */
