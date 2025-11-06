#include "iic.h"

// 全局变量
uint8_t ack = 0;

// 更精确的微秒级延时函数
void IIC_Delay_us(uint32_t us)
{

    volatile uint32_t cycles = us * 13;
    while(cycles--);
}

// SDA设置为输入模式
static void SOFT_I2C_SDA_IN(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = SOFT_I2C_SDA_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(SOFT_I2C_SDA_PORT, &GPIO_InitStruct);
}

// SDA设置为输出模式
static void SOFT_I2C_SDA_OUT(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = SOFT_I2C_SDA_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(SOFT_I2C_SDA_PORT, &GPIO_InitStruct);
}


// 初始化软件I2C
void SOFT_I2C_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    //printf("=== Software I2C Initialization ===\r\n");
    
    // 确保GPIOA时钟使能
    __HAL_RCC_GPIOA_CLK_ENABLE();
    //printf("GPIOA clock: ENABLED\r\n");
    
    // 配置SCL引脚 - 开漏输出，上拉
    GPIO_InitStruct.Pin = SOFT_I2C_SCL_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(SOFT_I2C_SCL_PORT, &GPIO_InitStruct);
    
    // 配置SDA引脚 - 开漏输出，上拉
    GPIO_InitStruct.Pin = SOFT_I2C_SDA_PIN;
    HAL_GPIO_Init(SOFT_I2C_SDA_PORT, &GPIO_InitStruct);
    
    // 设置总线空闲状态为高电平
    IIC_SCL_1;
    IIC_SDA_1;
    
    
    // 延时确保总线稳定
    HAL_Delay(50);
    
    // 检查初始引脚状态
    GPIO_PinState scl_state = HAL_GPIO_ReadPin(SOFT_I2C_SCL_PORT, SOFT_I2C_SCL_PIN);
    GPIO_PinState sda_state = HAL_GPIO_ReadPin(SOFT_I2C_SDA_PORT, SOFT_I2C_SDA_PIN);
    
   //printf("Initial pin states - SCL: %d, SDA: %d\r\n", scl_state, sda_state);
    
    if(scl_state == GPIO_PIN_RESET || sda_state == GPIO_PIN_RESET)
    {
       // printf("WARNING: SCL or SDA is LOW! Check pull-up resistors.\r\n");
    }
    else
    {
       // printf("SCL and SDA are HIGH (good)\r\n");
    }
    
//printf("I2C initialization complete\r\n");
}

// 产生起始信号
void SOFT_I2C_Start(void)
{
	SOFT_I2C_SDA_OUT();
	IIC_SDA_1;
	IIC_SCL_1;
	IIC_Delay_us(5);  // 起始条件建立时间 >4.7μs
	IIC_SDA_0;
	IIC_Delay_us(5);  // 起始条件保持时间 >4.0μs
	IIC_SCL_0;
	IIC_Delay_us(2);  // 时钟低电平时间
}

// 产生停止信号
void SOFT_I2C_Stop(void)
{
	SOFT_I2C_SDA_OUT();
	IIC_SCL_1;
	IIC_SDA_0;
	IIC_Delay_us(2);
	IIC_SCL_1;
	IIC_Delay_us(5);  // 停止条件建立时间 >4.7μs
	IIC_SDA_1;
	IIC_Delay_us(5);  // 总线空闲时间
}

// 等待应答信号
// 返回0: 收到应答
// 返回1: 未收到应答
uint8_t SOFT_I2C_Wait_Ack(void)
{
	uint8_t timeout = 0;
	SOFT_I2C_SDA_IN();
	
	IIC_SDA_1; // 释放SDA线
	IIC_Delay_us(2);
	IIC_SCL_1;
	IIC_Delay_us(5);
	// 等待从设备拉低SDA（应答信号）
	while(IIC_SDA_READ)
	{
			timeout++;
			if(timeout > 200)  // 约200*5μs=1ms超时
			{
					SOFT_I2C_Stop();
					SOFT_I2C_SDA_OUT();
					return 1; // 超时，无应答
			}
			IIC_Delay_us(5);
	}
	
	IIC_SCL_0;
	IIC_Delay_us(2);
	SOFT_I2C_SDA_OUT();
	return 0; // 收到应答
}

// 发送应答信号
void SOFT_I2C_Ack(void)
{
	SOFT_I2C_SDA_OUT();
	IIC_SCL_0;
	IIC_Delay_us(2);
	IIC_SDA_0; // 拉低表示应答
	IIC_Delay_us(2);
	IIC_SCL_1;
	IIC_Delay_us(5);
	IIC_SCL_0;
	IIC_Delay_us(2);
	IIC_SDA_1; // 释放SDA线
}

// 发送非应答信号
void SOFT_I2C_NAck(void)
{
    SOFT_I2C_SDA_OUT();
    IIC_SCL_0;
    IIC_Delay_us(2);
    IIC_SDA_1; // 拉高表示非应答
    IIC_Delay_us(2);
    IIC_SCL_1;
    IIC_Delay_us(5);
    IIC_SCL_0;
    IIC_Delay_us(2);
}

// 发送一个字节
void SOFT_I2C_Send_Byte(uint8_t data)
{
    uint8_t i;
    SOFT_I2C_SDA_OUT();
    IIC_SCL_0;
    for(i = 0; i < 8; i++)
    {
        // 在SCL低电平期间准备数据
        if(data & 0x80)
        {
            IIC_SDA_1;
        }
        else
        {
            IIC_SDA_0;
        }
        IIC_Delay_us(5);
        
        // SCL上升沿，数据被采样
        IIC_SCL_1;
        IIC_Delay_us(20);
        
        // SCL下降沿，准备下一位
        IIC_SCL_0;
        IIC_Delay_us(5);
        
        data <<= 1;
    }
    
    // 释放SDA线，等待应答
    IIC_SDA_1;
    IIC_Delay_us(5);
}

// 读取一个字节
uint8_t SOFT_I2C_Read_Byte(void)
{
	uint8_t i, receive = 0;
	SOFT_I2C_SDA_IN();
	for(i = 0; i < 8; i++)
	{
		receive <<= 1;
		IIC_SCL_0;
		IIC_Delay_us(2);
		// SCL上升沿，读取数据
		IIC_SCL_1;
		IIC_Delay_us(5);
		if(IIC_SDA_READ)
		{
			receive |= 0x01; // 读数据位
		}
		IIC_SCL_0;
		IIC_Delay_us(2);
	}
	SOFT_I2C_SDA_OUT();
	return receive;
}

// 写寄存器
uint8_t SOFT_I2C_Write_Reg(uint8_t dev_addr, uint8_t reg_addr, uint8_t data)
{
    SOFT_I2C_Start();
    
    SOFT_I2C_Send_Byte(dev_addr & 0xFE); // 写命令
    if(SOFT_I2C_Wait_Ack())
        return 0;
    
    SOFT_I2C_Send_Byte(reg_addr);
    if(SOFT_I2C_Wait_Ack())
        return 0;
    
    SOFT_I2C_Send_Byte(data);
    if(SOFT_I2C_Wait_Ack())
        return 0;
    
    SOFT_I2C_Stop();
    
    return 1;
}

// 读寄存器
uint8_t SOFT_I2C_Read_Reg(uint8_t dev_addr, uint8_t reg_addr, uint8_t *data)
{
    SOFT_I2C_Start();
    
    SOFT_I2C_Send_Byte(dev_addr & 0xFE); // 写命令
    if(SOFT_I2C_Wait_Ack())
        return 0;
    
    SOFT_I2C_Send_Byte(reg_addr);
    if(SOFT_I2C_Wait_Ack())
        return 0;
    
    SOFT_I2C_Start(); // 重复起始条件
    
    SOFT_I2C_Send_Byte(dev_addr | 0x01); // 读命令
    if(SOFT_I2C_Wait_Ack())
        return 0;
    
    *data = SOFT_I2C_Read_Byte();
    SOFT_I2C_NAck();
    
    SOFT_I2C_Stop();
    
    return 1;
}

// 连续读多个寄存器
uint8_t SOFT_I2C_Read_Multi_Reg(uint8_t dev_addr, uint8_t reg_addr, uint8_t *data, uint16_t len)
{
    uint16_t i;
    
    SOFT_I2C_Start();
    
    SOFT_I2C_Send_Byte(dev_addr & 0xFE); // 写命令
    if(SOFT_I2C_Wait_Ack())
        return 0;
    
    SOFT_I2C_Send_Byte(reg_addr);
    if(SOFT_I2C_Wait_Ack())
        return 0;
    
    SOFT_I2C_Start(); // 重复起始条件
    
    SOFT_I2C_Send_Byte(dev_addr | 0x01); // 读命令
    if(SOFT_I2C_Wait_Ack())
        return 0;
    
    for(i = 0; i < len; i++)
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


// 调试引脚状态
void SOFT_I2C_Debug_Pins(void)
{
    GPIO_PinState scl_state = HAL_GPIO_ReadPin(SOFT_I2C_SCL_PORT, SOFT_I2C_SCL_PIN);
    GPIO_PinState sda_state = HAL_GPIO_ReadPin(SOFT_I2C_SDA_PORT, SOFT_I2C_SDA_PIN);
    
    printf("I2C Pin Status: SCL=%d, SDA=%d\r\n", scl_state, sda_state);
}

// I2C总线扫描
void SOFT_I2C_Scan_Bus(void)
{
    uint8_t i, ret;
    printf("=== I2C Bus Scan (0x08-0x77) ===\r\n");
    
    uint8_t found_devices = 0;
    for(i = 8; i < 120; i++)
    {
        SOFT_I2C_Start();
        SOFT_I2C_Send_Byte(i); // 写地址
        ret = SOFT_I2C_Wait_Ack();
        SOFT_I2C_Stop();
        if(ret == 0)
        {
            printf("找到设备地址: 0x%02X\r\n", i);
            found_devices++;
        }
        
        HAL_Delay(1);
    }

}

// 测试特定设备
uint8_t SOFT_I2C_Test_Device(uint8_t dev_addr)
{
    uint8_t ret;
    
    printf("测试设备 0x%02X: ", dev_addr);
    
    SOFT_I2C_Start();
    SOFT_I2C_Send_Byte(dev_addr); // 写命令
    ret = SOFT_I2C_Wait_Ack();
    SOFT_I2C_Stop();
    
    if(ret == 0)
    {
        printf("确认收到 - 设备存在!\r\n");
        return 1;
    }
    else
    {
        printf("没有收到 - 设备不存在！\r\n");
        return 0;
    }
}


