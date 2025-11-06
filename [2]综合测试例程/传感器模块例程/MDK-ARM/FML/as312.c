#include "as312.h"
#include <stdio.h>
// 初始化函数
void AS312_Init(void)
{
// 初始化GPIO
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    // 使能GPIO时钟
    __HAL_RCC_GPIOA_CLK_ENABLE();
    
    // 配置PA5为输入（AS312传感器）
    GPIO_InitStruct.Pin = GPIO_PIN_5;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    

    printf("AS312人体传感器测试开始\r\n");
}
/*
VCC--3.3
GND--GND
OUT--PA5
*/

//人体热释电传感器测试
void AS312_Test(void)
{
    AS312_Init();
    HAL_Delay(5000);
    printf("开始检测人体移动...\r\n");

    while (1)
    {
				
			if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_5) == GPIO_PIN_SET)
			{
				printf("检测到人体！\r\n");
				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET); // 点亮LED
			}
			else
			{
				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET); // 熄灭LED
			}
			HAL_Delay(500);
    }
}

