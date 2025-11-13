#ifndef HX711_H
#define HX711_H

#include "stm32g4xx_hal.h"
#include "main.h"
#include "stdbool.h"

/* 用于保存HX711的增益 */
typedef enum {
    GAIN_128 = 1,
    GAIN_32,
    GAIN_64,
} ENUM_HX711_GAIN_TYPEDEF;

/* 结构体 用于实现HX711本身的属性 */
typedef struct {
    ENUM_HX711_GAIN_TYPEDEF gain; /* 增益 */
    bool isTare;    /* 判断传感器数据是否是皮重 true-是 false-否 */
    float k;        /* 比例系数 */
    float b;        /* 补偿值 */
    float tare;     /* 皮重 */
    float actual;   /* 实重 */
} STRUCT_HX711_TYPEDEF;

/* HX711传感器引脚定义 - 请根据实际硬件连接修改 */
#define HX711_DOUT_GPIO_PORT   GPIOA
#define HX711_DOUT_GPIO_PIN    GPIO_PIN_5

#define HX711_SCK_GPIO_PORT    GPIOA
#define HX711_SCK_GPIO_PIN     GPIO_PIN_4

/* ---------------- 函数清单 --------------- */
void hx711_init(void);                       /* HX711初始化 */
void hx711_power_off(void);                  /* HX711断电 */
void hx711_reset(void);                      /* HX711断电后复位 */
void hx711_set_convert_ratio(float ratio);   /* 设置HX711重量比例系数 */
void hx711_set_offset_value(float offset);   /* 设置HX711重量补偿值 */
void hx711_set_gain(ENUM_HX711_GAIN_TYPEDEF gain); /* 设置HX711增益系数 */
float hx711_get_tare_weight(void);           /* 获取皮重 */
float hx711_get_actual_weight(void);         /* 获取实重 */

/* 微秒延时函数声明 */
void delay_us(uint16_t us);

#endif
