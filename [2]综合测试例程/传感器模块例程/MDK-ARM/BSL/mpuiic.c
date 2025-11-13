#include "mpuiic.h"

//正点原子MPU6050通讯线驱动 

//MPU IIC 延时函数
void MPU_IIC_Delay(uint32_t us)
{
    volatile uint32_t cycles = us * 13;
    while(cycles--);
}

//初始化IIC
void MPU_IIC_Init(void)
{					     
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    __HAL_RCC_GPIOA_CLK_ENABLE();
    
    //SCL配置
    GPIO_InitStruct.Pin = GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    
    //SDA配置
    GPIO_InitStruct.Pin = GPIO_PIN_7;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    
    //初始化为高电平
    MPU_IIC_SCL(1);
    MPU_IIC_SDA(1);
}

//产生IIC起始信号
void MPU_IIC_Start(void)
{
    MPU_SDA_OUT();     //sda线输出
    MPU_IIC_SDA(1);	  
    MPU_IIC_SCL(1);
    MPU_IIC_Delay(5);
    MPU_IIC_SDA(0); //START:when CLK is high,DATA change form high to low 
    MPU_IIC_Delay(5);
    MPU_IIC_SCL(0); //钳住I2C总线，准备发送或接收数据 
}	  

//产生IIC停止信号
void MPU_IIC_Stop(void)
{
    MPU_SDA_OUT(); //sda线输出
    MPU_IIC_SCL(0);
    MPU_IIC_SDA(0); //STOP:when CLK is high DATA change form low to high
    MPU_IIC_Delay(5);
    MPU_IIC_SCL(1);  
    MPU_IIC_SDA(1); //发送I2C总线结束信号
    MPU_IIC_Delay(5);							   	
}

//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
uint8_t MPU_IIC_Wait_Ack(void)
{
    uint8_t ucErrTime = 0;
    MPU_SDA_IN();      //SDA设置为输入  
    MPU_IIC_SDA(1); 
    MPU_IIC_Delay(5);	   
    MPU_IIC_SCL(1);
    MPU_IIC_Delay(5);	 
    
    while(MPU_READ_SDA)
    {
        ucErrTime++;
        if(ucErrTime > 254)
        {
            MPU_IIC_Stop();
            return 41;
        }
    }
    MPU_IIC_SCL(0); //时钟输出0 	   
    return 0;  
} 

//产生ACK应答
void MPU_IIC_Ack(void)
{
    MPU_IIC_SCL(0);
    MPU_SDA_OUT();
    MPU_IIC_SDA(0);
    MPU_IIC_Delay(5);
    MPU_IIC_SCL(1);
    MPU_IIC_Delay(5);
    MPU_IIC_SCL(0);
}

//不产生ACK应答		    
void MPU_IIC_NAck(void)
{
    MPU_IIC_SCL(0);
    MPU_SDA_OUT();
    MPU_IIC_SDA(1);
    MPU_IIC_Delay(5);
    MPU_IIC_SCL(1);
    MPU_IIC_Delay(5);
    MPU_IIC_SCL(0);
}					 				     

//IIC发送一个字节
void MPU_IIC_Send_Byte(uint8_t txd)
{                        
    uint8_t t;   
    MPU_SDA_OUT(); 	    
    MPU_IIC_SCL(0); //拉低时钟开始数据传输
    for(t = 0; t < 8; t++)
    {              
        MPU_IIC_SDA((txd & 0x80) >> 7);
        txd <<= 1; 	  
        MPU_IIC_SCL(1);
        MPU_IIC_Delay(5); 
        MPU_IIC_SCL(0);	
        MPU_IIC_Delay(5);
    }	 
} 	    

//读1个字节，ack=1时，发送ACK，ack=0，发送nACK   
uint8_t MPU_IIC_Read_Byte(unsigned char ack)
{
    unsigned char i, receive = 0;
    MPU_SDA_IN(); //SDA设置为输入
    for(i = 0; i < 8; i++)
    {
        MPU_IIC_SCL(0); 
        MPU_IIC_Delay(5);
        MPU_IIC_SCL(1);
        receive <<= 1;
        if(MPU_READ_SDA) 
            receive++;   
        MPU_IIC_Delay(5); 
    }					 
    if (!ack)
        MPU_IIC_NAck(); //发送nACK
    else
        MPU_IIC_Ack();  //发送ACK   
    return receive;
}

//写入一个字节
void IMPU_IC_Write_One_Byte(uint8_t daddr, uint8_t addr, uint8_t data)
{
    MPU_IIC_Start();
    MPU_IIC_Send_Byte(daddr);     //发送设备地址+写信号
    MPU_IIC_Wait_Ack();
    MPU_IIC_Send_Byte(addr);      //发送寄存器地址
    MPU_IIC_Wait_Ack();
    MPU_IIC_Send_Byte(data);      //发送数据
    MPU_IIC_Wait_Ack();
    MPU_IIC_Stop();
}

//读取一个字节
uint8_t MPU_IIC_Read_One_Byte(uint8_t daddr, uint8_t addr)
{
    uint8_t data;
    MPU_IIC_Start();
    MPU_IIC_Send_Byte(daddr);     //发送设备地址+写信号
    MPU_IIC_Wait_Ack();
    MPU_IIC_Send_Byte(addr);      //发送寄存器地址
    MPU_IIC_Wait_Ack();
    
    MPU_IIC_Start();
    MPU_IIC_Send_Byte(daddr | 0x01); //发送设备地址+读信号
    MPU_IIC_Wait_Ack();
    data = MPU_IIC_Read_Byte(0);     //读取数据，发送nACK
    MPU_IIC_Stop();
    return data;
}

