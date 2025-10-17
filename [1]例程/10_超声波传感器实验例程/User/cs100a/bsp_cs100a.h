#ifndef __BSP_GENERALTIME_H
#define __BSP_GENERALTIME_H

#include "stm32f10x.h"

// ��ʱ��Ԥ��Ƶ
#define GENERAL_TIM_PRESCALER               71

// ��ʱ������
#define GENERAL_TIM_PERIOD                  0xFFFF

/************ͨ�ö�ʱ��TIM�������壬ֻ��TIM2��3��4��5************/
// ��ʹ�ò�ͬ�Ķ�ʱ����ʱ�򣬶�Ӧ��GPIO�ǲ�һ���ģ����Ҫע��

#define GENERAL_TIMx                        TIM2
#define GENERAL_TIM_APBxClock_FUN           RCC_APB1PeriphClockCmd
#define GENERAL_TIM_CLK                     RCC_APB1Periph_TIM2
#define GENERAL_TIM_CHANNELx                TIM_Channel_2
#define GENERAL_TIM_IT_CCx                  TIM_IT_CC2


#define GENERAL_TIM_IRQn                    TIM2_IRQn
#define GENERAL_TIM_IRQHANDLER              TIM2_IRQHandler

#define ECHO_RCC_GPIO_CLK                   RCC_APB2Periph_GPIOA
#define ECHO_GPIO_PIN                       GPIO_Pin_0
#define ECHO_GPIO_PORT                      GPIOA


#define TRIG_RCC_GPIO_CLK                   RCC_APB2Periph_GPIOA
#define TRIG_GPIO_PIN                       GPIO_Pin_1
#define TRIG_GPIO_PORT                      GPIOA


//----------------------------------------------------------------
// ��ȡ����Ĵ���ֵ�����궨��
#define            GENERAL_TIM_GetCapturex_FUN                 TIM_GetCapture2
// �����źż��Ժ����궨��
#define            GENERAL_TIM_OCxPolarityConfig_FUN           TIM_OC2PolarityConfig

// ��������ʼ����
#define            GENERAL_TIM_STRAT_ICPolarity        TIM_ICPolarity_Rising 
// �����Ľ�������
#define            GENERAL_TIM_END_ICPolarity          TIM_ICPolarity_Falling 


//----------------------------------------------------------------
// ��ʱ�����벶���û��Զ�������ṹ������
typedef struct              
{   
	uint8_t   ucFinishFlag;   // ���������־λ
	uint8_t   ucStartFlag;    // ����ʼ��־λ
	uint16_t  usCtr;          // ����Ĵ�����ֵ
	uint16_t  usPeriod;       // �Զ���װ�ؼĴ������±�־ 
}STRUCT_CAPTURE; 

//----------------------------------------------------------------
extern STRUCT_CAPTURE TIM_ICUserValueStructure;
//----------------------------------------------------------------

/**************************��������********************************/
void CS100A_TRIG(void);
void CS100A_Init(void);


#endif	/* __BSP_GENERALTIME_H */


