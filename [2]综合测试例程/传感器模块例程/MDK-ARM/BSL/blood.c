#include "blood.h"

// 全局变量定义
uint16_t g_fft_index = 0;
struct compx s1[FFT_N+16];
struct compx s2[FFT_N+16];
BloodData g_blooddata = {0};

// 心率检测相关变量
static uint32_t last_peak_time = 0;
static uint32_t peak_intervals[8] = {0};
static uint8_t peak_index = 0;
static uint32_t last_ir_value = 0;
static uint8_t was_rising = 0;
static uint32_t ir_max = 0;
static uint32_t ir_min = 0xFFFFFFFF;
static uint32_t valid_peak_count = 0;

// 血氧计算相关变量
static uint32_t red_max = 0, red_min = 0xFFFFFFFF;
static uint32_t ir_max_spo2 = 0, ir_min_spo2 = 0xFFFFFFFF;
static uint32_t last_spo2_time = 0;

// 血液检测信息更新
void blood_data_update(void)
{
    g_fft_index = 0;
    uint32_t timeout = 0;
    
    // 快速采集100个数据点
    while(g_fft_index < 100 && timeout < 1000)
    {
        if(HAL_GPIO_ReadPin(MAX30102_INTPin_PORT, MAX30102_INTPin_PIN) == GPIO_PIN_RESET)
        {
            MAX30102_Read_FIFO();
            
            // 实时心率计算
            HeartRate_Calculate();
            
            g_fft_index++;
            timeout = 0;
        }
        else
        {
            HAL_Delay(1);
            timeout++;
        }
    }
}

// 实时心率计算函数
void HeartRate_Calculate(void)
{
    uint32_t current_time = HAL_GetTick();
    
    // 更新IR信号的最大最小值
    if(fifo_ir > ir_max) ir_max = fifo_ir;
    if(fifo_ir < ir_min) ir_min = fifo_ir;
    
    // 动态阈值计算
    uint32_t threshold = 0;
    if(ir_max > ir_min + 1000)
    {
        threshold = ir_min + (ir_max - ir_min) * 2 / 3;
    }
    else
    {
        threshold = 50000;
    }
    
    // 峰值检测逻辑
    if(fifo_ir > threshold && last_ir_value <= threshold && !was_rising)
    {
        was_rising = 1;
    }
    else if(fifo_ir < threshold && last_ir_value >= threshold && was_rising)
    {
        was_rising = 0;
        
        if(last_peak_time > 0)
        {
            uint32_t interval = current_time - last_peak_time;
            
            if(interval > 300 && interval < 1500)
            {
                peak_intervals[peak_index] = interval;
                peak_index = (peak_index + 1) % 8;
                valid_peak_count++;
                
                if(valid_peak_count >= 2)
                {
                    uint32_t sum_intervals = 0;
                    uint8_t count = 0;
                    
                    for(int i = 0; i < 8; i++)
                    {
                        if(peak_intervals[i] > 0)
                        {
                            sum_intervals += peak_intervals[i];
                            count++;
                        }
                    }
                    
                    if(count > 0)
                    {
                        uint32_t avg_interval = sum_intervals / count;
                        float heart_rate = 60000.0f / avg_interval;
                        
                        if(heart_rate >= 40.0f && heart_rate <= 180.0f)
                        {
                            g_blooddata.heart = (int)heart_rate;
                        }
                        else
                        {
                            g_blooddata.heart = 0;
                        }
                    }
                }
            }
        }
        last_peak_time = current_time;
        
        // 定期重置最大值最小值
        if(valid_peak_count % 10 == 0)
        {
            ir_max = 0;
            ir_min = 0xFFFFFFFF;
        }
    }
    
    last_ir_value = fifo_ir;
}

// 实时血氧计算函数
void SPO2_Calculate(void)
{
    uint32_t current_time = HAL_GetTick();
    
    // 更新血氧相关的最大值最小值
    if(fifo_red > red_max) red_max = fifo_red;
    if(fifo_red < red_min) red_min = fifo_red;
    if(fifo_ir > ir_max_spo2) ir_max_spo2 = fifo_ir;
    if(fifo_ir < ir_min_spo2) ir_min_spo2 = fifo_ir;
    
    // 每2秒计算一次血氧
    if(current_time - last_spo2_time >= 2000)
    {
        last_spo2_time = current_time;
        
        // 计算信号幅度
        uint32_t red_amplitude = red_max - red_min;
        uint32_t ir_amplitude = ir_max_spo2 - ir_min_spo2;
        
        // 幅度检查：如果信号幅度太小，则认为没有有效信号
        if(red_amplitude < 1000 || ir_amplitude < 1000)
        {
            g_blooddata.SpO2 = 0.0f;
        }
        else
        {
            // 计算AC分量
            float red_ac = (float)(red_max - red_min);
            float ir_ac = (float)(ir_max_spo2 - ir_min_spo2);
            
            // 计算DC分量
            float red_dc = (float)(red_max + red_min) / 2.0f;
            float ir_dc = (float)(ir_max_spo2 + ir_min_spo2) / 2.0f;
            
            // 计算R值
            if(ir_ac > 0 && ir_dc > 0)
            {
                float R = (red_ac / red_dc) / (ir_ac / ir_dc);
                
                // 使用经验公式计算血氧
                float spo2 = 110.0f - 25.0f * R;
                
                // 限制血氧范围
                if(spo2 >= 70.0f && spo2 <= 100.0f)
                {
                    g_blooddata.SpO2 = spo2;
                }
                else
                {
                    g_blooddata.SpO2 = 0.0f;
                }
            }
            else
            {
                g_blooddata.SpO2 = 0.0f;
            }
        }
        
        // 重置最大值最小值
        red_max = 0;
        red_min = 0xFFFFFFFF;
        ir_max_spo2 = 0;
        ir_min_spo2 = 0xFFFFFFFF;
    }
}

// 快速心率和血氧计算
void Quick_HeartRate_SPO2_Calculate(void)
{
    // 实时计算血氧
    SPO2_Calculate();
		// 实时计算心率
		HeartRate_Calculate();
	
    printf("RED: %lu, IR: %lu  ", fifo_red, fifo_ir);
    // 直接输出结果
    //printf("心率: %3d BPM, 血氧: %5.1f%%\r\n", g_blooddata.heart, g_blooddata.SpO2);
}

// 简单心率计算


// 血液检测主循环
//void blood_Loop(void)
//{
//    // 采集数据并实时计算心率
//    blood_data_update();
//    
//    // 计算血氧
//    SPO2_Calculate();
//    
//    // 直接输出结果
//    printf("心率: %3d BPM, 血氧: %5.1f%%\r\n", g_blooddata.heart, g_blooddata.SpO2);
//}

// 血液检测初始化
void Blood_Init(void)
{
    last_peak_time = 0;
    peak_index = 0;
    was_rising = 0;
    valid_peak_count = 0;
    ir_max = 0;
    ir_min = 0xFFFFFFFF;
    red_max = 0;
    red_min = 0xFFFFFFFF;
    ir_max_spo2 = 0;
    ir_min_spo2 = 0xFFFFFFFF;
    last_spo2_time = 0;
    
    for(int i = 0; i < 8; i++)
    {
        peak_intervals[i] = 0;
    }
    
    printf("血液检测系统初始化完成\r\n");
}

