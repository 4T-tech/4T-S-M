
#include "stm32f10x.h"
#include "bsp_usart.h"
#include "photoresistor/bsp_photoresistor.h"




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
	// 配置串口
	USART_Config();
	
	// 光敏模块初始化
	PhotoResistor_Init();
	
	printf("\r\n ----这是一个声音模块读取数据实验----\r\n");
	
	while (1)
	{	
        printf("声音模块 数据量为：%d\n",GPIO_ReadInputDataBit(PhotoResistor_PORT,PhotoResistor_PIN)); 

        
        if(GPIO_ReadInputDataBit(PhotoResistor_PORT,PhotoResistor_PIN) == 0)
        {
            printf("声音达到阈值\n");
        }
        else
        {
            printf("声音未达到阈值\n");
        }
        
        printf("\r\n\r\n");
        Delay(0x1fffee);
	}
}
/*********************************************END OF FILE**********************/
