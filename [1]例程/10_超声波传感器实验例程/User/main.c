

#include "stm32f10x.h"
#include "stm32f10x_it.h"
#include "systick/bsp_SysTick.h"
#include "led/bsp_led.h"
#include "usart/bsp_usart.h"
#include "bsp_cs100a.h"

void Soft_Delay(__IO uint32_t nCount);

/**
  * @brief  ������
  * @param  ��  
  * @retval ��
  */
int main(void)
{
	uint32_t time;
	float distance;
    /* ����ϵͳ�δ�ʱ�� */
    SysTick_Init(); 
    
    /* LED �˿ڳ�ʼ�� */
    //LED_GPIO_Config();
    
    /* ����1ͨ�ų�ʼ�� */
    USART_Config();
printf("\r\n ----����һ�����������ģ���ȡ����ʵ��----\r\n");
	/* ���������ģ���ʼ�� */
	CS100A_Init();
    
    printf("\r\n ���������ģ��ǰ���������տ���������ΧС����Ļز��ź���ɴ���\r\n");
    printf("\r\n ����Ŀ�����������С��0.5ƽ���ף�ƽ�澡��ƽ���Ҵ�ֱ�ڳ�����̽ͷ������Ӱ��������\r\n");    

    while(1)
    {
	/* ����CS100A�������������� */
	CS100A_TRIG();
    /* ��ɲ����ߵ�ƽ���� */
    if(TIM_ICUserValueStructure.ucFinishFlag == 1 )
		{
      /* ����ߵ�ƽ����ֵ */
			time = TIM_ICUserValueStructure .usPeriod * GENERAL_TIM_PERIOD + TIM_ICUserValueStructure .usCtr;
			/* ����ʱ�������룬��λm */
			distance = time*340/200000.00;
            if(distance>5.6)
            printf ( "û���յ��������򳬳�������Χ\n" );
            else
			printf ( "Ŀ����룺%f m\n",distance );
			TIM_ICUserValueStructure .ucFinishFlag = 0;	        
        }
        /* ������ӡ�ٶ� */
		Soft_Delay(300000);
    }
	
}
void Soft_Delay(__IO uint32_t nCount) // �򵥵���ʱ����
{
    for (; nCount != 0; nCount--)
        ;
}
/*********************************************END OF FILE**********************/
