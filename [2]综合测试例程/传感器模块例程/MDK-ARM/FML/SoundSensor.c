#include "SoundSensor.h"
/*
VCC--3.3
GND--GND
DO--PA5
*/
//声音传感器初始化
void SoundSensor_Init(void)
{
	// 初始化GPIO
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	
	// 使能GPIO时钟
	__HAL_RCC_GPIOA_CLK_ENABLE();
	
	// 配置PA5为输入（声音传感器）
	GPIO_InitStruct.Pin = GPIO_PIN_5;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	printf("声音传感器初始化完成\r\n");
	HAL_Delay(1000); // 短暂延时
}
//声音传感器测试
void SoundSensor_Test(void)
{
	SoundSensor_Init();
	printf("开始检测声音...\r\n");

	while (1)
	{
		if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_5) == GPIO_PIN_SET)
		{
			printf("未检测到声音\r\n");
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET); // 无声音时熄灭LED
		}
		else
		{		
			printf("检测到声音！\r\n");
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET); // 检测到声音时点亮LED
		}

		HAL_Delay(500); // 500ms检测间隔
	}
}

