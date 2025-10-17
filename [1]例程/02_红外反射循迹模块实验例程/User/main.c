#include "stm32f10x.h"
#include "bsp_usart.h"
#include "tcrt/bsp_tcrt.h"

extern __IO uint16_t ADC_ConvertedValue;


// 软件延时
void Delay(__IO uint32_t nCount)
{
  for(; nCount != 0; nCount--);
} 

/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void)
{	
	
	/* 初始化USART1 配置模式为 115200 8-N-1 */
	USART_Config();
	
	/* 循迹模块初始化 */
	TCRT_Init();
	
	printf("\r\n ----这是一个循迹模块读取数据实验----\r\n");
	
	while (1)
	{	
        printf("循迹(反射性红外)模块 数字量为：%d\n",GPIO_ReadInputDataBit(TCRT_PORT,TCRT_PIN)); 
        printf("循迹(反射性红外)模块 检测到的模拟量为：%d\n\n",ADC_ConvertedValue);
        
        if(GPIO_ReadInputDataBit(TCRT_PORT,TCRT_PIN) == 0)
        {
            printf("检测到物体\n");
        }
        else
        {
            printf("检测到黑线或者没有检测到物体\n");
        }
        
        printf("\r\n\r\n");
        Delay(0x1fffee);
	}
}

