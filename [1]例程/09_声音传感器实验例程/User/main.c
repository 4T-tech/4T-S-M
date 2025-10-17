
#include "stm32f10x.h"
#include "bsp_usart.h"
#include "photoresistor/bsp_photoresistor.h"




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
	// ���ô���
	USART_Config();
	
	// ����ģ���ʼ��
	PhotoResistor_Init();
	
	printf("\r\n ----����һ������ģ���ȡ����ʵ��----\r\n");
	
	while (1)
	{	
        printf("����ģ�� ������Ϊ��%d\n",GPIO_ReadInputDataBit(PhotoResistor_PORT,PhotoResistor_PIN)); 

        
        if(GPIO_ReadInputDataBit(PhotoResistor_PORT,PhotoResistor_PIN) == 0)
        {
            printf("�����ﵽ��ֵ\n");
        }
        else
        {
            printf("����δ�ﵽ��ֵ\n");
        }
        
        printf("\r\n\r\n");
        Delay(0x1fffee);
	}
}
/*********************************************END OF FILE**********************/
