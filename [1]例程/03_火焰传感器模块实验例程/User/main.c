
#include "stm32f10x.h"
#include "bsp_usart.h"
#include "flame/bsp_flame.h"

//ADC的转换值
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
	
	/* 火焰检测模块初始化 */
	Flame_Init();
	
    printf("\r\n ----这是一个火焰检测模块读取数据实验----\r\n");
	
	while (1)
	{	
        printf("火焰检测模块 数字量为：%d\n",GPIO_ReadInputDataBit(Flame_PORT,Flame_PIN)); 
        printf("火焰检测模块检测到的模拟量为：%d\n\n",ADC_ConvertedValue);
        
        if(GPIO_ReadInputDataBit(Flame_PORT,Flame_PIN) == 0)
        {
            printf("火焰强度达到阈值\n");
        }
        else
        {
            printf("火焰强度未达到阈值\n");
        }
        
        printf("\r\n\r\n");
        Delay(0x1fffee);
	}
}
/*********************************************END OF FILE**********************/
