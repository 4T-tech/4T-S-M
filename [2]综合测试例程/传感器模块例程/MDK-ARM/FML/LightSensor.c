#include "LightSensor.h"



//光敏初始化
void LightSensor_Init(void)
{
	// 初始化GPIO
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	
	// 使能GPIO时钟
	__HAL_RCC_GPIOA_CLK_ENABLE();
	
	// 配置PA5为输入（光敏传感器）
	GPIO_InitStruct.Pin = GPIO_PIN_5;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	printf("光敏传感器测试开始\r\n");
	HAL_Delay(1000); // 短暂延时
}
/*
VCC--3.3
GND--GND
DO--PA5
*/
//光敏传感器测试
void LightSensor_Test(void)
{
	LightSensor_Init();
	printf("开始检测光线强度...\r\n");

	while (1)
	{
		if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_5) == GPIO_PIN_SET)
		{
			printf("光线不足\r\n");
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET); // 光线充足时熄灭LED
		}
		else
		{	
			printf("光线充足\r\n");
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET); // 光线不足时点亮LED
		}
		HAL_Delay(500); // 500ms检测间隔
	}
}
