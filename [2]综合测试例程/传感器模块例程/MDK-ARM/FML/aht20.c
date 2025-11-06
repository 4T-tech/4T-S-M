#include "aht20.h"
#include "iic.h"

#define AHT20_ADDRESS 0x70

// 使用软件I2C重新实现的AHT20驱动

void AHT20_Init(void)
{
    uint8_t status = 0;
    
    // 初始化软件I2C
    SOFT_I2C_Init();
    HAL_Delay(40);
    
    // 读取状态寄存器
    if(SOFT_I2C_Read_Reg(AHT20_ADDRESS, 0x00, &status))
    {
        // 检查状态字的bit[3]（校准使能位），如果为0需要初始化
        if((status & 0x08) == 0x00)
        {
            // 发送初始化命令：0xBE 0x08 0x00
            SOFT_I2C_Start();

					SOFT_I2C_Send_Byte(AHT20_ADDRESS ); // 写地址
            if(SOFT_I2C_Wait_Ack() == 0)
            {
                SOFT_I2C_Send_Byte(0xBE); // 初始化命令
                if(SOFT_I2C_Wait_Ack() == 0)
                {
                    SOFT_I2C_Send_Byte(0x08); // 参数1
                    if(SOFT_I2C_Wait_Ack() == 0)
                    {
                        SOFT_I2C_Send_Byte(0x00); // 参数2
                        SOFT_I2C_Wait_Ack();
                    }
                }
            }
            SOFT_I2C_Stop();
            
            // 等待初始化完成
            HAL_Delay(10);
        }
    }
}

// 简化的初始化函数（如果上面的太复杂）
void AHT20_Init_Simple(void)
{
    SOFT_I2C_Init();
    HAL_Delay(40);
    
    // 直接发送初始化命令
    uint8_t init_cmd[3] = {0xBE, 0x08, 0x00};
    
    SOFT_I2C_Start();

		SOFT_I2C_Send_Byte(AHT20_ADDRESS);
    SOFT_I2C_Wait_Ack();
    
    for(int i = 0; i < 3; i++)
    {
        SOFT_I2C_Send_Byte(init_cmd[i]);
        SOFT_I2C_Wait_Ack();
    }
    
    SOFT_I2C_Stop();
    HAL_Delay(10);
}

uint8_t AHT20_Read_Status(void)
{
    uint8_t status = 0;
    if(SOFT_I2C_Read_Reg(AHT20_ADDRESS, 0x00, &status))
    {
        return status;
    }
    return 0xFF; // 读取失败
}

uint8_t AHT20_Start_Measurement(void)
{
    uint8_t cmd[3] = {0xAC, 0x33, 0x00};
    
    SOFT_I2C_Start();

    SOFT_I2C_Send_Byte(AHT20_ADDRESS); // 写地址
    if(SOFT_I2C_Wait_Ack())
        return 0;
    
    for(int i = 0; i < 3; i++)
    {
        SOFT_I2C_Send_Byte(cmd[i]);
        if(SOFT_I2C_Wait_Ack())
            return 0;
    }
    
    SOFT_I2C_Stop();
    return 1;
}

uint8_t AHT20_Read_Data(uint8_t *data, uint8_t len)
{
    SOFT_I2C_Start();

    SOFT_I2C_Send_Byte(AHT20_ADDRESS | 0x01); // 读地址
    if(SOFT_I2C_Wait_Ack())
        return 0;
    
    for(int i = 0; i < len; i++)
    {
        data[i] = SOFT_I2C_Read_Byte();
        if(i == len - 1)
            SOFT_I2C_NAck(); // 最后一个字节发送非应答
        else
            SOFT_I2C_Ack();  // 其他字节发送应答
    }
    
    SOFT_I2C_Stop();
    return 1;
}

void AHT20_Read(float *Temperature, float *Humidity)
{
    uint8_t readBuffer[6] = {0};
    
    // 启动测量
    if(!AHT20_Start_Measurement())
    {
        *Temperature = 0;
        *Humidity = 0;
        return;
    }
    
    // 等待测量完成
    HAL_Delay(80);
    
    // 读取数据
    if(!AHT20_Read_Data(readBuffer, 6))
    {
        *Temperature = 0;
        *Humidity = 0;
        return;
    }
    
    // 检查状态位，确保数据就绪且有效
    if((readBuffer[0] & 0x80) == 0x00) // bit[7]为0表示数据就绪
    {
        uint32_t humidity_data = 0;
        uint32_t temperature_data = 0;
        
        // 解析湿度数据（20位）
        humidity_data = ((uint32_t)readBuffer[1] << 12) | 
                       ((uint32_t)readBuffer[2] << 4) | 
                       ((uint32_t)readBuffer[3] >> 4);
        
        // 解析温度数据（20位）  
        temperature_data = (((uint32_t)readBuffer[3] & 0x0F) << 16) | 
                          ((uint32_t)readBuffer[4] << 8) | 
                          (uint32_t)readBuffer[5];
        
        // 转换为实际值
        *Humidity = (float)humidity_data * 100.0f / 1048576.0f; // 2^20 = 1048576
        *Temperature = (float)temperature_data * 200.0f / 1048576.0f - 50.0f;
    }
    else
    {
        // 数据未就绪
        *Temperature = 0;
        *Humidity = 0;
    }
}

// 增强版的读取函数，带重试机制
uint8_t AHT20_Read_Enhanced(float *Temperature, float *Humidity, uint8_t retry_count)
{
    uint8_t status;
    
    for(uint8_t i = 0; i < retry_count; i++)
    {
        status = AHT20_Read_Status();
        
        // 如果设备忙，等待
        if(status & 0x80)
        {
            HAL_Delay(10);
            continue;
        }
        
        AHT20_Read(Temperature, Humidity);
        
        // 检查读取的数据是否合理
        if(*Temperature >= -40.0f && *Temperature <= 85.0f && 
           *Humidity >= 0.0f && *Humidity <= 100.0f)
        {
            return 1; // 读取成功
        }
        
        HAL_Delay(10);
    }
    
    return 0; // 读取失败
}

// 测试AHT20设备
uint8_t AHT20_Test_Device(void)
{
    return SOFT_I2C_Test_Device(AHT20_ADDRESS);
}

/*
VCC--3.3
GND--GND
SDA--PA7
SCL--PA6
*/
//温湿度传感器测试
void AHT20_Test(void)
{	
	// 初始化
	AHT20_Init();

	// 测试设备
	if(AHT20_Test_Device())
	{
		printf("AHT20 设备找到，开始测试!\n");
		SOFT_I2C_Scan_Bus();
	}

	// 读取数据
	float temp, humi;
	while(1)
	{
		AHT20_Read(&temp, &humi);
		printf("温度: %.2f C, 湿度: %.2f %%\n", temp, humi);
		HAL_Delay(500);
	}

}


