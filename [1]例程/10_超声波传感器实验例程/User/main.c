

#include "stm32f10x.h"
#include "stm32f10x_it.h"
#include "systick/bsp_SysTick.h"
#include "led/bsp_led.h"
#include "usart/bsp_usart.h"
#include "bsp_cs100a.h"

void Soft_Delay(__IO uint32_t nCount);

/**
  * @brief  主函数
  * @param  无  
  * @retval 无
  */
int main(void)
{
	uint32_t time;
	float distance;
    /* 启动系统滴答定时器 */
    SysTick_Init(); 
    
    /* LED 端口初始化 */
    //LED_GPIO_Config();
    
    /* 串口1通信初始化 */
    USART_Config();
printf("\r\n ----这是一个超声波测距模块读取数据实验----\r\n");
	/* 超声波测距模块初始化 */
	CS100A_Init();
    
    printf("\r\n 超声波测距模块前方处尽量空旷，避免周围小物体的回波信号造成错误\r\n");
    printf("\r\n 待测目标面积尽量不小于0.5平方米，平面尽量平整且垂直于超声波探头，否则影响测量结果\r\n");    

    while(1)
    {
	/* 触发CS100A发出超声波脉冲 */
	CS100A_TRIG();
    /* 完成测量高电平脉宽 */
    if(TIM_ICUserValueStructure.ucFinishFlag == 1 )
		{
      /* 计算高电平计数值 */
			time = TIM_ICUserValueStructure .usPeriod * GENERAL_TIM_PERIOD + TIM_ICUserValueStructure .usCtr;
			/* 根据时间计算距离，单位m */
			distance = time*340/200000.00;
            if(distance>5.6)
            printf ( "没有收到超声波或超出测量范围\n" );
            else
			printf ( "目标距离：%f m\n",distance );
			TIM_ICUserValueStructure .ucFinishFlag = 0;	        
        }
        /* 调整打印速度 */
		Soft_Delay(300000);
    }
	
}
void Soft_Delay(__IO uint32_t nCount) // 简单的延时函数
{
    for (; nCount != 0; nCount--)
        ;
}
/*********************************************END OF FILE**********************/
