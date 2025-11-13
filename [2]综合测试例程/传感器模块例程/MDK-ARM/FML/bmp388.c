#include "bmp388.h"
#include "main.h"
#include "stdio.h"

// 全局变量定义
struct bmp3_calib_data  calib_data;
struct bmp3_uncomp_data uncomp_data;
struct bmp3_data        comp_data;

// 微秒延时函数
//void delay_us(uint16_t us)
//{
//    // 使用I2C驱动中的延时函数
//    IIC_Delay_us(us);
//}

// 毫秒延时函数


// 写一个寄存器
void BMP388_Send_Byte(uint8_t addr, uint8_t data)
{
	SOFT_I2C_Start();
	SOFT_I2C_Send_Byte(BMP388_Addr << 1);  // 写地址
	SOFT_I2C_Wait_Ack();
	SOFT_I2C_Send_Byte(addr);              // 寄存器地址
	SOFT_I2C_Wait_Ack();
	SOFT_I2C_Send_Byte(data);              // 数据
	SOFT_I2C_Wait_Ack();
	SOFT_I2C_Stop();
}

// 读一个寄存器
uint8_t BMP388_Read_Byte(uint8_t addr)
{
	uint8_t data;
	SOFT_I2C_Start();
	SOFT_I2C_Send_Byte(BMP388_Addr << 1);      // 写地址
	SOFT_I2C_Wait_Ack();
	SOFT_I2C_Send_Byte(addr);                  // 寄存器地址
	SOFT_I2C_Wait_Ack();
	SOFT_I2C_Start();                          // 重复起始条件
	SOFT_I2C_Send_Byte(BMP388_Addr << 1 | 0x01); // 读地址
	SOFT_I2C_Wait_Ack();
	data = SOFT_I2C_Read_Byte();      // 读取数据
	SOFT_I2C_NAck();                           // 发送非应答
	SOFT_I2C_Stop();
	return data;
}

// 读取未修正的气压和温度信息
void get_PandT(void)
{
    uint8_t reg_data[6] = {0};
    uint8_t status;
    uint8_t i = 0;
    status = BMP388_Read_Byte(0x03);
    while(!((status & 0x40) && (status & 0x20)))  // 查询数据是否准备完毕
    {
        status = BMP388_Read_Byte(0x03);
        HAL_Delay(1);
    }
    
    
		SOFT_I2C_Start();
		SOFT_I2C_Send_Byte(BMP388_Addr << 1); 
		SOFT_I2C_Wait_Ack();
		SOFT_I2C_Send_Byte(0x04); 
		SOFT_I2C_Wait_Ack();
		SOFT_I2C_Start();
		SOFT_I2C_Send_Byte((BMP388_Addr << 1)|0x01); 
		SOFT_I2C_Wait_Ack();
    //SOFT_I2C_Read_Multi_Reg(BMP388_Addr, 0x04, reg_data, 6);
    for(i=0;i<5;i++)												//连续读6个寄存器，读完最后一个数据发送不应答信号
    {
        reg_data[i] = SOFT_I2C_Read_Byte();
        SOFT_I2C_Ack();
    }
		reg_data[5] = SOFT_I2C_Read_Byte();
		SOFT_I2C_NAck();
    SOFT_I2C_Stop();
    // 数据合成
    uncomp_data.pressure    = (uint32_t)reg_data[2] << 16 | (uint32_t)reg_data[1] << 8 | (uint32_t)reg_data[0];
    uncomp_data.temperature = (uint32_t)reg_data[5] << 16 | (uint32_t)reg_data[4] << 8 | (uint32_t)reg_data[3];
}

// 读取修正系数
void get_calib_data(void)
{
    uint8_t reg_data[21] = {0};
    uint8_t i = 0;

    SOFT_I2C_Start();
		SOFT_I2C_Send_Byte(BMP388_Addr << 1); 
		SOFT_I2C_Wait_Ack();
		SOFT_I2C_Send_Byte(0x31); 
		SOFT_I2C_Wait_Ack();
		SOFT_I2C_Start();
		SOFT_I2C_Send_Byte((BMP388_Addr << 1)|0x01); 
		SOFT_I2C_Wait_Ack();
		
		for(i=0;i<20;i++)                //连续读21个数据
    {
			reg_data[i] = SOFT_I2C_Read_Byte();
			SOFT_I2C_Ack();
    }
		reg_data[20] = SOFT_I2C_Read_Byte();
		SOFT_I2C_NAck();
    SOFT_I2C_Stop();
		
		
    // 根据数据手册的数据类型转换
    calib_data.par_t1  = Concat_Bytes(reg_data[1], reg_data[0]);
    calib_data.par_t2  = Concat_Bytes(reg_data[3], reg_data[2]);
    calib_data.par_t3  = (int8_t)reg_data[4];
    calib_data.par_p1  = (int16_t)Concat_Bytes(reg_data[6], reg_data[5]);
    calib_data.par_p2  = (int16_t)Concat_Bytes(reg_data[8], reg_data[7]);
    calib_data.par_p3  = (int8_t)reg_data[9];
    calib_data.par_p4  = (int8_t)reg_data[10];
    calib_data.par_p5  = Concat_Bytes(reg_data[12], reg_data[11]);
    calib_data.par_p6  = Concat_Bytes(reg_data[14], reg_data[13]);
    calib_data.par_p7  = (int8_t)reg_data[15];
    calib_data.par_p8  = (int8_t)reg_data[16];
    calib_data.par_p9  = (int16_t)Concat_Bytes(reg_data[18], reg_data[17]);
    calib_data.par_p10 = (int8_t)reg_data[19];
    calib_data.par_p11 = (int8_t)reg_data[20];
}

// 修正温度（保持不变）
void compensate_temperature(void)
{
    int64_t partial_data1;
    int64_t partial_data2;
    int64_t partial_data3;
    int64_t partial_data4;
    int64_t partial_data5;
    int64_t partial_data6;
    int64_t comp_temp;

    partial_data1 = (int64_t)uncomp_data.temperature - (256 * (int64_t)calib_data.par_t1);
    partial_data2 = (int64_t)calib_data.par_t2 * partial_data1;
    partial_data3 = partial_data1 * partial_data1;
    partial_data4 = (int64_t)partial_data3 * (int64_t)calib_data.par_t3;
    partial_data5 = ((int64_t)(partial_data2 * 262144) + partial_data4);
    partial_data6 = partial_data5 / 4294967296;
    calib_data.t_lin = partial_data6;  // 存储这个data6为t_lin因为计算气压要用到
    comp_temp = (int64_t)((partial_data6 * 25) / 16384);
    comp_data.temperature = comp_temp;
}

// 修正气压（保持不变）
void compensate_pressure(void)
{
    int64_t partial_data1;
    int64_t partial_data2;
    int64_t partial_data3;
    int64_t partial_data4;
    int64_t partial_data5;
    int64_t partial_data6;
    int64_t offset;
    int64_t sensitivity;
    uint64_t comp_press;

    partial_data1 = calib_data.t_lin * calib_data.t_lin;
    partial_data2 = partial_data1 / 64;
    partial_data3 = (partial_data2 * calib_data.t_lin) / 256;
    partial_data4 = (calib_data.par_p8 * partial_data3) / 32;
    partial_data5 = (calib_data.par_p7 * partial_data1) * 16;
    partial_data6 = (calib_data.par_p6 * calib_data.t_lin) * 4194304;
    offset = ((int64_t)calib_data.par_p5 * 140737488355328) + partial_data4 + partial_data5 + partial_data6;

    partial_data2 = (calib_data.par_p4 * partial_data3) / 32;
    partial_data4 = (calib_data.par_p3 * partial_data1) * 4;
    partial_data5 = ((int64_t)calib_data.par_p2 - 16384) * calib_data.t_lin * 2097152;
    sensitivity = (((int64_t)calib_data.par_p1 - 16384) * 70368744177664) + partial_data2 + partial_data4 + partial_data5;

    partial_data1 = (sensitivity / 16777216) * uncomp_data.pressure;
    partial_data2 = calib_data.par_p10 * calib_data.t_lin;
    partial_data3 = partial_data2 + (65536 * (int64_t)calib_data.par_p9);
    partial_data4 = (partial_data3 * uncomp_data.pressure) / 8192;
    partial_data5 = (partial_data4 * uncomp_data.pressure) / 512;
    partial_data6 = (int64_t)((uint64_t)uncomp_data.pressure * (uint64_t)uncomp_data.pressure);
    partial_data2 = (calib_data.par_p11 * partial_data6) / 65536;
    partial_data3 = (partial_data2 * uncomp_data.pressure) / 128;
    partial_data4 = (offset / 4) + partial_data1 + partial_data5 + partial_data3;
    comp_press = (((uint64_t)partial_data4 * 25) / (uint64_t)1099511627776);
    comp_data.pressure = comp_press;
}

// BMP388初始化
uint8_t BMP388_Init(void)
{
   
    SOFT_I2C_Init();
    HAL_Delay(10);
    
    // 读取芯片ID
    if(BMP388_Read_Byte(0x00) != 0x50)
    {
        printf("BMP388 ID Read Failed: 0x%02X\r\n", BMP388_Read_Byte(0x00));
        return 1;
    }
    
    printf("BMP388 ID 检查通过: 0x50\r\n");
    get_calib_data();  // 读取补偿系数
    
    // 查询能否写指令
    while(!(BMP388_Read_Byte(0x03) & 0x10))
        HAL_Delay(3);
        
    BMP388_Send_Byte(0x7e, 0xb6);  // 写重置指令，重置全部寄存器
    HAL_Delay(5);
    
    // 查看错误指示寄存器，有错误返回1
    if(BMP388_Read_Byte(0x02) & 0x07)
    {
        printf("BMP388 错误寄存器: 0x%02X\r\n", BMP388_Read_Byte(0x02));
        return 2;
    }
        
    BMP388_Send_Byte(0x1c, 0x00);  // 设置温度过采样*1 气压过采样*1
    BMP388_Send_Byte(0x1f, 0x04);  // 设置滤波系数2
    BMP388_Send_Byte(0x1D, 0x01);  // 设置输出分频系数
    BMP388_Send_Byte(0x1b, 0x03);  // 使能气压和温度采样但是先不启动采样
    
    printf("BMP388 初始化成功\r\n");
    return 0;
}

// 获得正确的值
void BMP388_Get_PaT(void)
{
    get_PandT();
    compensate_temperature();
    compensate_pressure();
}

void BMP388_ON(void)
{
    BMP388_Send_Byte(0x1b, 0x33);  // 启动传输
    printf("BMP388 开始采样\r\n");
}

void BMP388_OFF(void)
{
    BMP388_Send_Byte(0x1b, 0x03);  // 进入睡眠
    printf("BMP388 进入睡眠模式\r\n");
}
/*
VCC--3.3
GND--GND
SDA--PA7
SCL--PA6
*/
// 测试函数
void BMP388_Test(void)
{
    // 先测试I2C总线
    printf("=== BMP388 Test ===\r\n");
    SOFT_I2C_Scan_Bus();
    
    uint8_t init_status = BMP388_Init();
    if(init_status == 0)
    {
        printf("BMP388 初始化成功!\r\n");
    }
    else
    {
        printf("BMP388 初始化失败! 错误代码: %d\r\n", init_status);
        return;
    }
    
    // 启动采样
    BMP388_ON();
    
    while(1)
    {
        // 读取温度和气压
        BMP388_Get_PaT();
        
        // 输出结果
        printf("温度: %.2f C, 气压: %.2f Pa\r\n", 
                     (float)comp_data.temperature / 100.0f, 
                     (float)comp_data.pressure/100.0f);
        
        HAL_Delay(1000);
    }
}
