#include "stm32f10x.h"
#include "bsp_usart.h"
#include "tcrt/bsp_tcrt.h"

extern __IO uint16_t ADC_ConvertedValue;


// �����ʱ
void Delay(__IO uint32_t nCount)
{
  for(; nCount != 0; nCount--);
} 

/**
  * @brief  ������
  * @param  ��
  * @retval ��
  */
int main(void)
{	
	
	/* ��ʼ��USART1 ����ģʽΪ 115200 8-N-1 */
	USART_Config();
	
	/* ѭ��ģ���ʼ�� */
	TCRT_Init();
	
	printf("\r\n ----����һ��ѭ��ģ���ȡ����ʵ��----\r\n");
	
	while (1)
	{	
        printf("ѭ��(�����Ժ���)ģ�� ������Ϊ��%d\n",GPIO_ReadInputDataBit(TCRT_PORT,TCRT_PIN)); 
        printf("ѭ��(�����Ժ���)ģ�� ��⵽��ģ����Ϊ��%d\n\n",ADC_ConvertedValue);
        
        if(GPIO_ReadInputDataBit(TCRT_PORT,TCRT_PIN) == 0)
        {
            printf("��⵽����\n");
        }
        else
        {
            printf("��⵽���߻���û�м�⵽����\n");
        }
        
        printf("\r\n\r\n");
        Delay(0x1fffee);
	}
}

