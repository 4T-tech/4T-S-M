#include "MAX30102.h"
#include "main.h"  // HAL库主头文件



// 全局变量定义
uint32_t fifo_red = 0;  // 改为32位以存储18位数据
uint32_t fifo_ir = 0;   // 改为32位以存储18位数据

uint32_t last_detect_time = 0;
uint32_t sample_count = 0;
uint32_t last_heart_rate = 0;
float last_spo2 = 0.0f;

// GPIO初始化函数
void MAX30102_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    // 使能GPIOB时钟（中断引脚）
    __HAL_RCC_GPIOB_CLK_ENABLE();
    
    // 配置中断引脚为输入模式
    GPIO_InitStruct.Pin = MAX30102_INTPin_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;  // 外部已加上拉电阻
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(MAX30102_INTPin_PORT, &GPIO_InitStruct);
    
    // 使能GPIOC时钟（LED指示灯）
    __HAL_RCC_GPIOC_CLK_ENABLE();
    
    // 配置LED引脚为输出模式
    GPIO_InitStruct.Pin = Led_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(Led_PORT, &GPIO_InitStruct);
    
    LED_OFF(); // 初始关闭LED
}

// 复位MAX30102
uint8_t MAX30102_Reset(void)
{
    if(SOFT_I2C_Write_Reg(MAX30102_Device_address, REG_MODE_CONFIG, 0x40))
        return 1;  // 复位成功
    else
        return 0;  // 复位失败
}

// 配置MAX30102
void MAX30102_Config(void)
{
    // 延时确保设备就绪
    HAL_Delay(10);
    
    // 中断设置
    SOFT_I2C_Write_Reg(MAX30102_Device_address, REG_INTR_ENABLE_1, 0xC0);
    SOFT_I2C_Write_Reg(MAX30102_Device_address, REG_INTR_ENABLE_2, 0x00);
    
    // FIFO配置 - 减少平均采样次数，提高响应速度
    SOFT_I2C_Write_Reg(MAX30102_Device_address, REG_FIFO_CONFIG, 0x40); // 平均采样1次，FIFO几乎满值0
    
    // 模式配置
    SOFT_I2C_Write_Reg(MAX30102_Device_address, REG_MODE_CONFIG, 0x03); // SpO2模式
    
    // SpO2配置 - 提高采样率到400Hz
    SOFT_I2C_Write_Reg(MAX30102_Device_address, REG_SPO2_CONFIG, 0x2F); // 400Hz, 4096nA, 411μs
    
    // LED电流设置 - 适当提高电流以获得更好信号
    SOFT_I2C_Write_Reg(MAX30102_Device_address, REG_LED1_PA, 0x24);   // RED LED ~7mA
    SOFT_I2C_Write_Reg(MAX30102_Device_address, REG_LED2_PA, 0x24);   // IR LED ~7mA
    
    // 多LED模式控制
    SOFT_I2C_Write_Reg(MAX30102_Device_address, REG_MULTI_LED_CTRL1, 0x21);
    
    HAL_Delay(10);
    
//    printf("MAX30102快速模式配置完成\r\n");
}

// 读取FIFO数据
void MAX30102_Read_FIFO(void)
{
    uint8_t i2c_data[6];
    uint32_t temp;
    
    // 清零数据
    fifo_red = 0;
    fifo_ir = 0;
    
    // 读取和清除状态寄存器（可选）
    uint8_t status1, status2;
    SOFT_I2C_Read_Reg(MAX30102_Device_address, REG_INTR_STATUS_1, &status1);
    SOFT_I2C_Read_Reg(MAX30102_Device_address, REG_INTR_STATUS_2, &status2);
    
    // 连续读取6字节FIFO数据
    SOFT_I2C_Read_Multi_Reg(MAX30102_Device_address, REG_FIFO_DATA, i2c_data, 6);
    
    // 组合RED数据（18位）
    temp = ((uint32_t)i2c_data[0] << 16) | ((uint32_t)i2c_data[1] << 8) | i2c_data[2];
    fifo_red = temp & 0x3FFFF;  // 取18位有效数据
    
    // 组合IR数据（18位）
    temp = ((uint32_t)i2c_data[3] << 16) | ((uint32_t)i2c_data[4] << 8) | i2c_data[5];
    fifo_ir = temp & 0x3FFFF;   // 取18位有效数据
    
    // 数据有效性检查
    if(fifo_ir <= 10000)
    {
        fifo_ir = 0;
    }
    if(fifo_red <= 10000)
    {
        fifo_red = 0;
    }
}

// 初始化MAX30102
uint8_t MAX30102_Init(void)
{
    uint8_t part_id = 0;
    
    // 初始化GPIO
    MAX30102_GPIO_Init();
    
    // 初始化软件I2C
    SOFT_I2C_Init();
    
    // 检查器件ID
    HAL_Delay(10);
    if(SOFT_I2C_Read_Reg(MAX30102_Device_address, REG_PART_ID, &part_id))
    {
        if(part_id != 0x15)
        {
            return 0; // 器件ID不匹配
        }
    }
    else
    {
        return 0; // 读取器件ID失败
    }
    
    // 复位器件
    if(!MAX30102_Reset())
    {
        return 0; // 复位失败
    }
    
    HAL_Delay(10);
    
    // 配置器件
    MAX30102_Config();
    
    HAL_Delay(10);
    
    return 1; // 初始化成功
}

// 读取单个寄存器
uint8_t MAX30102_Read_Reg(uint8_t reg_addr, uint8_t *data)
{
    return SOFT_I2C_Read_Reg(MAX30102_Device_address, reg_addr, data);
}

// 写入单个寄存器
uint8_t MAX30102_Write_Reg(uint8_t reg_addr, uint8_t data)
{
    return SOFT_I2C_Write_Reg(MAX30102_Device_address, reg_addr, data);
}

// 读取FIFO数据到指定变量
uint8_t MAX30102_Read_FIFO_Data(uint32_t *red_value, uint32_t *ir_value)
{
    uint8_t i2c_data[6];
    
    // 连续读取6字节FIFO数据
    if(SOFT_I2C_Read_Multi_Reg(MAX30102_Device_address, REG_FIFO_DATA, i2c_data, 6))
    {
        return 0; // 读取失败
    }
    
    // 组合RED数据（18位）
    *red_value = ((uint32_t)i2c_data[0] << 16) | ((uint32_t)i2c_data[1] << 8) | i2c_data[2];
    *red_value &= 0x3FFFF;  // 取18位有效数据
    
    // 组合IR数据（18位）
    *ir_value = ((uint32_t)i2c_data[3] << 16) | ((uint32_t)i2c_data[4] << 8) | i2c_data[5];
    *ir_value &= 0x3FFFF;   // 取18位有效数据
    
    return 1; // 读取成功
}

// 简单的心率和血氧计算（示例算法）
void MAX30102_HeartRate_SPO2_Calculate(float *heart_rate, float *spo2)
{
    static uint32_t last_ir_value = 0;
    static uint32_t peak_count = 0;
    static uint32_t last_peak_time = 0;
    uint32_t current_time = HAL_GetTick();
    
    // 简单的心率计算（基于IR信号的峰值检测）
    if(fifo_ir > last_ir_value && last_ir_value > 0)
    {
        // 检测到上升沿
        if(current_time - last_peak_time > 300) // 最小间隔300ms
        {
            peak_count++;
            if(peak_count >= 2)
            {
                uint32_t interval = current_time - last_peak_time;
                *heart_rate = 60000.0f / interval; // 转换为BPM
                
                // 限制心率范围
                if(*heart_rate < 40) *heart_rate = 0;
                if(*heart_rate > 180) *heart_rate = 0;
            }
            last_peak_time = current_time;
        }
    }
    last_ir_value = fifo_ir;
    
    // 简单的血氧计算（基于RED/IR比率）
    if(fifo_red > 0 && fifo_ir > 0)
    {
        float ratio = (float)fifo_red / fifo_ir;
        *spo2 = 110.0f - 25.0f * ratio;
        
        // 限制血氧范围
        if(*spo2 < 70) *spo2 = 0;
        if(*spo2 > 100) *spo2 = 100;
    }
    else
    {
        *spo2 = 0;
    }
}
/*
VCC--3.3
GND--GND
SDA--PA7
SCL--PA6
INT--PA5
*/
//心率传感器测试
void MAX30102_Test(void)
{
	printf("MAX30102 心率血氧检测\r\n");
	if(MAX30102_Init())
	{
		printf("\r\nMAX30102传感器初始化成功\r\n");
	}
	else
	{
		printf("\r\nMAX30102传感器初始化失败\r\n");
		while(1);
	}
		for(int i = 0; i < 32; i++) 
	{
		if(MAX30102_INTPin_Read() == 0)
		{
			MAX30102_Read_FIFO();
		}
		HAL_Delay(1);
	}
	printf("\r\nFIFO缓冲区已清空\r\n");
	uint32_t current_time = HAL_GetTick();
	// 每1秒进行一次快速检测
	if(current_time - last_detect_time >= 1000)
	{
		last_detect_time = current_time;
		
		// 使用快速计算方法
		Quick_HeartRate_SPO2_Calculate();
		
		// LED指示
		HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
	}
	
	// 持续采集数据，不等待
	if(MAX30102_INTPin_Read() == 0)
	{
		MAX30102_Read_FIFO();
		sample_count++;
		
		// 每采集一定数量的样本就进行一次简单计算
		if(sample_count >= 50)  // 50个样本约0.5秒（100Hz采样率）
		{
			
			sample_count = 0;
		}
	}
}



