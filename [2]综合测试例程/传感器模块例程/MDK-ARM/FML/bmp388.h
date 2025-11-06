

#ifndef __BMP388_H_
#define __BMP388_H_

/*接线方式
CS------VCC
SDO-----GND
INT-----NC
SCL-----PA6
SDA-----PA7
*/

#include "main.h"
#include "stm32g4xx_hal.h"
#include "iic.h"  // 包含软件I2C驱动头文件

#define Concat_Bytes(msb, lsb) (((uint16_t)msb << 8) | (uint16_t)lsb)

// IIC地址，SDO接地为0x76，接VCC地址为0x77
#define BMP388_Addr 0x76

// 相关结构体定义，分别存放修正系数，未修正数据，已经修正的数据
struct bmp3_calib_data{
    uint16_t    par_t1;
    uint16_t    par_t2;
    int8_t      par_t3;
    int16_t     par_p1;
    int16_t     par_p2;
    int8_t      par_p3;
    int8_t      par_p4;
    uint16_t    par_p5;
    uint16_t    par_p6;
    int8_t      par_p7;
    int8_t      par_p8;
    int16_t     par_p9;
    int8_t      par_p10;
    int8_t      par_p11;
    int64_t     t_lin;
};

struct bmp3_uncomp_data {
    uint32_t pressure;
    uint32_t temperature;
};

struct bmp3_data {
    int64_t temperature;
    uint64_t pressure;
};

// 函数定义
uint8_t BMP388_Init(void);     			// IO初始化，读取ID，读取修正系数，配置寄存器
void BMP388_Get_PaT(void);  		// 读取温度和气压
void BMP388_ON(void);						// 开始采样
void BMP388_OFF(void);					// 停止采样


void BMP388_Test(void);
//void delay_us(uint16_t us);

#endif
