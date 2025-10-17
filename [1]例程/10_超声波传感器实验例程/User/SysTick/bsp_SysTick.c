
//SysTick_Config��Ҫ���������ж�����������STK_VAL�Ĵ���������SysTickʱ��ΪAHB 
#include "./systick/bsp_SysTick.h"
 
 static __IO uint32_t TimingDelay;
 
/**
  * @brief  ����ϵͳ�δ�ʱ�� SysTick
  * @param  ��
  * @retval ��
  */
void SysTick_Init(void)
{
	/* SystemFrequency / 1000    1ms�ж�һ��
	 * SystemFrequency / 100000	 10us�ж�һ��
	 * SystemFrequency / 1000000 1us�ж�һ��
	 */
//	if (SysTick_Config(SystemFrequency / 100000))	// ST3.0.0��汾
	if (SysTick_Config(SystemCoreClock / 100000))	// ST3.5.0��汾
	{ 
		/* Capture error */ 
		while (1);
	}

  // �رյδ�ʱ��  
	//SysTick->CTRL &= ~ SysTick_CTRL_ENABLE_Msk;
	
  // ʹ�ܵδ�ʱ��  1ms�ж�һ��
	SysTick->CTRL |=  SysTick_CTRL_ENABLE_Msk;
}

/**
  * @brief  Inserts a delay time.
  * @param  nTime: specifies the delay time length, in milliseconds.
  * @retval None
  */
void Delay_us(__IO uint32_t nTime)
{ 
  TimingDelay = nTime;

  while(TimingDelay != 0);
}

/**
  * @brief  Decrements the TimingDelay variable.
  * @param  None
  * @retval None
  * @attention  �� SysTick �жϺ��� SysTick_Handler()����
  */
void TimingDelay_Decrement(void)
{
  if (TimingDelay != 0x00)
  { 
    TimingDelay--;
  }
}

/*********************************************END OF FILE**********************/
