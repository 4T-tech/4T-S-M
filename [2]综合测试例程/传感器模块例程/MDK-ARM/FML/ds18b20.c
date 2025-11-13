#include "ds18b20.h"
#include <stdio.h>

// 微秒延时函数（基于170MHz系统时钟的软件延时）
void DS18B20_DelayUs(uint16_t us)
{
    // 根据80MHz系统时钟调整这个值
    // 这个值需要通过实验校准
    volatile uint32_t delay_count = us * 13; 
    
    while(delay_count--)
    {
        __NOP(); // 空操作指令
    }
}

// DS18B20初始化
void DS18B20_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    // 使能GPIOA时钟
    __HAL_RCC_GPIOA_CLK_ENABLE();
    
    // 配置PA5为推挽输出
    GPIO_InitStruct.Pin = DS18B20_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(DS18B20_PORT, &GPIO_InitStruct);
    
    // 初始状态设置为高电平
    HAL_GPIO_WritePin(DS18B20_PORT, DS18B20_PIN, GPIO_PIN_SET);
}

// 开始信号
uint8_t DS18B20_Start(void)
{
    uint8_t presence = 0;
    
    // 设置为输出模式
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = DS18B20_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(DS18B20_PORT, &GPIO_InitStruct);
    
    // 拉低总线480us
    HAL_GPIO_WritePin(DS18B20_PORT, DS18B20_PIN, GPIO_PIN_RESET);
    DS18B20_DelayUs(480);
    
    // 释放总线，设置为输入模式
    HAL_GPIO_WritePin(DS18B20_PORT, DS18B20_PIN, GPIO_PIN_SET);
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(DS18B20_PORT, &GPIO_InitStruct);
    
    // 等待15-60us
    DS18B20_DelayUs(60);
    
    // 检测应答信号
    if (HAL_GPIO_ReadPin(DS18B20_PORT, DS18B20_PIN) == GPIO_PIN_RESET)
    {
        presence = 1;  // 设备存在
    }
    
    // 等待完成复位周期（至少480us）
    DS18B20_DelayUs(420);
    
    return presence;
}

// 写一位数据
void DS18B20_WriteBit(uint8_t bit)
{
    // 设置为输出模式
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = DS18B20_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(DS18B20_PORT, &GPIO_InitStruct);
    
    if (bit)
    {
        // 写"1"
        HAL_GPIO_WritePin(DS18B20_PORT, DS18B20_PIN, GPIO_PIN_RESET);
        DS18B20_DelayUs(6);
        HAL_GPIO_WritePin(DS18B20_PORT, DS18B20_PIN, GPIO_PIN_SET);
        DS18B20_DelayUs(64);
    }
    else
    {
        // 写"0"
        HAL_GPIO_WritePin(DS18B20_PORT, DS18B20_PIN, GPIO_PIN_RESET);
        DS18B20_DelayUs(60);
        HAL_GPIO_WritePin(DS18B20_PORT, DS18B20_PIN, GPIO_PIN_SET);
        DS18B20_DelayUs(10);
    }
}

// 读一位数据
uint8_t DS18B20_ReadBit(void)
{
    uint8_t bit = 0;
    
    // 设置为输出模式，先拉低
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = DS18B20_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(DS18B20_PORT, &GPIO_InitStruct);
    
    // 拉低总线1us
    HAL_GPIO_WritePin(DS18B20_PORT, DS18B20_PIN, GPIO_PIN_RESET);
    DS18B20_DelayUs(2);
    
    // 释放总线，设置为输入模式
    HAL_GPIO_WritePin(DS18B20_PORT, DS18B20_PIN, GPIO_PIN_SET);
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(DS18B20_PORT, &GPIO_InitStruct);
    
    // 等待15us后读取
    DS18B20_DelayUs(15);
    
    // 读取总线状态
    if (HAL_GPIO_ReadPin(DS18B20_PORT, DS18B20_PIN) == GPIO_PIN_SET)
    {
        bit = 1;
    }
    
    // 等待完成读时序
    DS18B20_DelayUs(45);
    
    return bit;
}

// 写一个字节
void DS18B20_WriteByte(uint8_t data)
{
    uint8_t i;
    for (i = 0; i < 8; i++)
    {
        DS18B20_WriteBit(data & 0x01);
        data >>= 1;
    }
}

// 读一个字节
uint8_t DS18B20_ReadByte(void)
{
    uint8_t i, data = 0;
    for (i = 0; i < 8; i++)
    {
        data >>= 1;
        if (DS18B20_ReadBit())
        {
            data |= 0x80;
        }
    }
    return data;
}

// 读取温度值
float DS18B20_ReadTemp(void)
{
    uint8_t temp_l, temp_h;
    int16_t temp;
    float temperature;
    
    // 启动温度转换
    if (DS18B20_Start())
    {
        DS18B20_WriteByte(DS18B20_CMD_SKIP_ROM);
        DS18B20_WriteByte(DS18B20_CMD_CONVERT_T);
        
        // 等待转换完成（750ms for 12-bit resolution）
        HAL_Delay(750);
        
        // 读取温度值
        DS18B20_Start();
        DS18B20_WriteByte(DS18B20_CMD_SKIP_ROM);
        DS18B20_WriteByte(DS18B20_CMD_READ_SCRATCHPAD);
        
        temp_l = DS18B20_ReadByte();
        temp_h = DS18B20_ReadByte();
        
        // 组合温度值
        temp = (temp_h << 8) | temp_l;
        
        // 转换为实际温度值
        temperature = temp * 0.0625f;
        
        return temperature;
    }
    
    return -999.0f;  // 错误值
}
/*
VCC--3.3
GND--GND
DO--PA5
*/

//温度传感器测试
void DS18B20_Test(void)
{
	DS18B20_Init();
	printf("DS18B20 温度检测\r\n");
	while(1)
	{
		float temperature = DS18B20_ReadTemp();

		if (temperature > -100.0f)  // 有效的温度值
		{
				printf("Temperature: %.2f C\r\n", temperature);
		}
		else
		{
				printf("DS18B20 read error!\r\n");
		}

		HAL_Delay(500);  // 每秒读取一次
	}

}
