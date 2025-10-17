
#include "bsp_cs100a.h"
#include "systick/bsp_SysTick.h"

// ��ʱ�����벶���û��Զ�������ṹ�嶨��
STRUCT_CAPTURE TIM_ICUserValueStructure = {0,0,0,0};

// �ж����ȼ�����
static void GENERAL_TIM_NVIC_Config(void)
{
    NVIC_InitTypeDef NVIC_InitStructure; 
    // �����ж���Ϊ0
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);		
	// �����ж���Դ
    NVIC_InitStructure.NVIC_IRQChannel = GENERAL_TIM_IRQn ;	
	// ���������ȼ�Ϊ 0
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	 
	// ������ռ���ȼ�Ϊ3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;	
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

static void GENERAL_TIM_GPIO_Config(void) 
{
    GPIO_InitTypeDef GPIO_InitStructure;

    // ���벶��ͨ�� GPIO ��ʼ��
    RCC_APB2PeriphClockCmd(ECHO_RCC_GPIO_CLK, ENABLE);
    /* ��������ʱ��ʹ�� */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    
    GPIO_InitStructure.GPIO_Pin =  ECHO_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(ECHO_GPIO_PORT, &GPIO_InitStructure);	
    
    GPIO_PinRemapConfig(GPIO_FullRemap_TIM2, ENABLE); 
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable , ENABLE); 
    
}

///*
// * ע�⣺TIM_TimeBaseInitTypeDef�ṹ��������5����Ա��TIM6��TIM7�ļĴ�������ֻ��
// * TIM_Prescaler��TIM_Period������ʹ��TIM6��TIM7��ʱ��ֻ���ʼ����������Ա���ɣ�
// * ����������Ա��ͨ�ö�ʱ���͸߼���ʱ������.
// *-----------------------------------------------------------------------------
// *typedef struct
// *{ TIM_Prescaler            ����
// *	TIM_CounterMode			     TIMx,x[6,7]û�У���������
// *  TIM_Period               ����
// *  TIM_ClockDivision        TIMx,x[6,7]û�У���������
// *  TIM_RepetitionCounter    TIMx,x[1,8,15,16,17]����
// *}TIM_TimeBaseInitTypeDef; 
// *-----------------------------------------------------------------------------
// */
/* ----------------   PWM�ź� ���ں�ռ�ձȵļ���--------------- */
// ARR ���Զ���װ�ؼĴ�����ֵ
// CLK_cnt����������ʱ�ӣ����� Fck_int / (psc+1) = 72M/(psc+1)
// PWM �źŵ����� T = ARR * (1/CLK_cnt) = ARR*(PSC+1) / 72M
// ռ�ձ�P=CCR/(ARR+1)
static void GENERAL_TIM_Mode_Config(void)
{
    // ������ʱ��ʱ��,���ڲ�ʱ��CK_INT=72M
    GENERAL_TIM_APBxClock_FUN(GENERAL_TIM_CLK,ENABLE);
    
/*--------------------ʱ���ṹ���ʼ��-------------------------*/	
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	// �Զ���װ�ؼĴ�����ֵ���ۼ�TIM_Period+1��Ƶ�ʺ����һ�����»����ж�
	TIM_TimeBaseStructure.TIM_Period=GENERAL_TIM_PERIOD;	
	// ����CNT��������ʱ�� = Fck_int/(psc+1)
	TIM_TimeBaseStructure.TIM_Prescaler= GENERAL_TIM_PRESCALER;	
	// ʱ�ӷ�Ƶ���� ����������ʱ��ʱ��Ҫ�õ�
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;		
	// ����������ģʽ������Ϊ���ϼ���
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;		
	// �ظ���������ֵ��û�õ����ù�
	TIM_TimeBaseStructure.TIM_RepetitionCounter=0;	
	// ��ʼ����ʱ��
	TIM_TimeBaseInit(GENERAL_TIMx, &TIM_TimeBaseStructure);

	/*--------------------���벶��ṹ���ʼ��-------------------*/	
	TIM_ICInitTypeDef TIM_ICInitStructure;
	// �������벶���ͨ������Ҫ���ݾ����GPIO������
	TIM_ICInitStructure.TIM_Channel = GENERAL_TIM_CHANNELx;
	// ���벶���źŵļ�������
	TIM_ICInitStructure.TIM_ICPolarity = GENERAL_TIM_STRAT_ICPolarity;
	// ����ͨ���Ͳ���ͨ����ӳ���ϵ����ֱ���ͷ�ֱ������
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
	// �������Ҫ��������źŵķ�Ƶϵ��
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
	// �������Ҫ��������źŵ��˲�ϵ��
	TIM_ICInitStructure.TIM_ICFilter = 0;
	// ��ʱ�����벶���ʼ��
	TIM_ICInit(GENERAL_TIMx, &TIM_ICInitStructure);
	
	// ������ºͲ����жϱ�־λ
    TIM_ClearFlag(GENERAL_TIMx, TIM_FLAG_Update|GENERAL_TIM_IT_CCx);	
    // �������ºͲ����ж�  
	TIM_ITConfig (GENERAL_TIMx, TIM_IT_Update | GENERAL_TIM_IT_CCx, ENABLE );
	
	// ʹ�ܼ�����
	TIM_Cmd(GENERAL_TIMx, ENABLE);
    
}
/**
  * @brief  TRIG�ŵ�GPIO����
  * @param  ��
  * @retval ��
  */
static void CS100A_TRIG_GPIO_Config(void)
{		
		
    /*����һ��GPIO_InitTypeDef���͵Ľṹ��*/
    GPIO_InitTypeDef  GPIO_InitStruct;

    /*����GPIO����ʱ��*/
    RCC_APB2PeriphClockCmd(TRIG_RCC_GPIO_CLK, ENABLE);
  
    /*ѡ��Ҫ���Ƶ�GPIO����*/															   
    GPIO_InitStruct.GPIO_Pin = TRIG_GPIO_PIN;	

    /*�������ŵ��������Ϊ�������*/
    GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_Out_PP;  

    /*������������Ϊ���� */   
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;

    /*���ÿ⺯����ʹ���������õ�GPIO_InitStructure��ʼ��GPIO*/
    GPIO_Init(TRIG_GPIO_PORT, &GPIO_InitStruct);	
    
}

/**
  * @brief  ���һ������ 10us �ĸߵ�ƽ�������
  * @param  ��
  * @retval ��
  */
void CS100A_TRIG(void)
{
	GPIO_SetBits(TRIG_GPIO_PORT,TRIG_GPIO_PIN);
	Delay_us(30);
	GPIO_ResetBits(TRIG_GPIO_PORT,TRIG_GPIO_PIN);
}

/**
  * @brief  ���������ģ���ʼ��
  * @param  ��
  * @retval ��
  */
void CS100A_Init(void)
{
    
    GENERAL_TIM_GPIO_Config();
    GENERAL_TIM_NVIC_Config();
    CS100A_TRIG_GPIO_Config();
    GENERAL_TIM_Mode_Config();
}

/*********************************************END OF FILE**********************/
