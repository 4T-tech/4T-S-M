#include "Ultrasonic.h"



int upEdge = 0;
int downEdge = 0;
float distance = 0;

uint32_t measurement_count = 0;
uint32_t error_count = 0;
uint8_t measurement_in_progress = 0;  // 测量进行中标志
uint32_t measurement_start_time = 0;  // 测量开始时间
uint32_t last_ultrasonic_time = 0;    // 

//超声波传感器初始化
void Ultrasonic_Init(void)
{
	HAL_TIM_Base_Start(&htim2);
	HAL_TIM_IC_Start(&htim2,TIM_CHANNEL_3);
	HAL_TIM_IC_Start_IT(&htim2,TIM_CHANNEL_4);
// 初始化GPIO
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	
	// 使能GPIO时钟
	__HAL_RCC_GPIOA_CLK_ENABLE();
	
	// 配置PA5为输入
	GPIO_InitStruct.Pin = GPIO_PIN_5;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;  // 输出模式
	GPIO_InitStruct.Pull = GPIO_NOPULL;          // 无上下拉
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH; // 高速
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	// 初始化为低电平
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
	printf("超声波传感器初始化完成\r\n");
}
/*
VCC--3.3
GND--GND
TRIG--PA5
ECHO--PA2
*/	
//超声波传感器测试
void Ultrasonic_Test(void)
{
	Ultrasonic_Init();
	
	while(1)
	{
		uint32_t current_time = HAL_GetTick();
	// 每500ms触发一次超声波测量
		if(current_time - last_ultrasonic_time >= 500)
		{
			last_ultrasonic_time = current_time;
			
			// 检查上次测量是否已完成
			if(measurement_in_progress)
			{
				printf("警告: 上次测量未完成，跳过本次测量\r\n");
			}
			else
			{
				// 开始新的测量
				measurement_in_progress = 1;
				measurement_start_time = current_time;
				
				// 重置计数器（关键步骤！）
				__HAL_TIM_SET_COUNTER(&htim2, 0);
				
				// 发送10us触发脉冲
				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
				delay_us(10);
				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
				
				printf("发送触发脉冲，开始测量...\r\n");
				}
				
				// 检查测量是否超时（50ms超时）
				if(measurement_in_progress && (current_time - measurement_start_time > 50))
				{
					printf("测量超时，未收到回波\r\n");
					distance = -4.0f; // 用-4表示超时
					measurement_in_progress = 0;
				}
				
				// 根据距离值进行判断和输出
				if(distance == -1.0f)
				{
					printf("测量错误: 定时器计数顺序异常\r\n");
				}
				else if(distance == -2.0f)
				{
					printf("测量错误: 回波时间超出范围\r\n");
				}
				else if(distance == -3.0f)
				{
					printf("测量错误: 计算距离异常\r\n");
				}
				else if(distance == -4.0f)
				{
					printf("测量错误: 测量超时\r\n");
				}
				else
				{
					printf("正常距离: %.2fcm\r\n", distance);
					HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);
					HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_RESET);
				}
							
				// 主LED指示
				HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
		}
		
		HAL_Delay(10);

	}
	
}
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	// 只处理定时器2的通道4中断（下降沿）
	if(htim == &htim2 && htim->Channel == HAL_TIM_ACTIVE_CHANNEL_4)
	{
		// 检查是否在测量过程中
		if(!measurement_in_progress)
		{
			printf("警告: 收到回波但未开始测量\r\n");
			return;
		}
		
		// 读取捕获值
		upEdge = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_3);    // 上升沿
		downEdge = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_4);  // 下降沿
		measurement_count++;
		
		// 调试信息
//        printf("调试: upEdge=%d, downEdge=%d, 计数器=%ld\r\n", 
//               upEdge, downEdge, __HAL_TIM_GET_COUNTER(&htim2));
		
		// 完整的数据有效性检查
		if(downEdge <= upEdge)
		{
			error_count++;
			distance = -1.0f; // 用-1表示顺序错误
			printf("错误: 下降沿(%d) <= 上升沿(%d)\r\n", downEdge, upEdge);
			measurement_in_progress = 0; // 结束测量
			return;
		}
		
		uint32_t time_diff = downEdge - upEdge;
		
		// 检查时间差是否在合理范围内 (2cm - 500cm)
		// 最小: 2cm ≈ 117us, 最大: 500cm ≈ 29411us
		if(time_diff < 100 || time_diff > 30000)
		{
			error_count++;
			distance = -2.0f; // 用-2表示超范围
			printf("错误: 时间差超出范围: %dus\r\n", time_diff);
			measurement_in_progress = 0; // 结束测量
			return;
		}
		
		// 计算有效距离
		distance = (time_diff * 0.034f) / 2.0f;
		// 最终距离范围检查
		if(distance < 0.1f || distance > 500.0f)
		{
			error_count++;
			distance = -3.0f; // 用-3表示距离异常
			printf("错误: 计算距离异常: %.2fcm\r\n", distance);
		}
			measurement_in_progress = 0;
	}
}
