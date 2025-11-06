#include "FlameSensor.h"



//初始化
void FlameSensor_Init(void)
{
	 // 初始化GPIO
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    // 使能GPIO时钟
    __HAL_RCC_GPIOA_CLK_ENABLE();
    // 配置PA5为输入
    GPIO_InitStruct.Pin = GPIO_PIN_5;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
		
    printf("火焰传感器初始化完成\r\n");
    HAL_Delay(1000); // 短暂延时
}
/*
VCC--3.3
GND--GND
DO--PA5
*/
//火焰传感器测试
void FlameSensor_Test(void)
{
	FlameSensor_Init();
	printf("开始检测火焰...\r\n");
	while (1)
	{
		if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_5) == GPIO_PIN_SET)
		{
			printf("未检测到火焰\r\n");
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET); // 无火焰时熄灭LED
		}
		else
		{		
			printf("检测到火焰！\r\n");
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET); // 检测到火焰时点亮LED
		}
		HAL_Delay(500); // 500ms检测间隔
	}
}

