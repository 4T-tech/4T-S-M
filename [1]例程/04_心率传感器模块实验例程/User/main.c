#include "stm32f10x.h"                // Device header
#include "Delay.h"
#include "OLED.h"
#include "usart.h"
#include "max30102.h"
#include "algorithm.h"
#include "xiic.h"
#include "blood.h"


int main(void)
{
	int i=0;
	Delay_init(72);			//设置外部晶振和单片机时钟频率为72m
	OLED_Init();				//OLED初始化
	OLED_ShowString(1,1,"hello");
	Usart1_Init(115200);		//调试串口初始化
	Usart2_Init(115200);		//串口初始
	
	UsartPrintf(USART_DEBUG," 你好\r\n");
	IIC_GPIO_INIT();		
	MAX30102_GPIO();
	Max30102_reset();
	MAX30102_Config();
	for(i = 0;i < 128;i++) 
	{
		while(MAX30102_INTPin_Read()==0)
		{
			//读取FIFO
			max30102_read_fifo();
		}
	}
	while (1)
	{
		blood_Loop();				//获取血氧和心率
		
	}
}
