#ifndef _BLOOD_H
#define _BLOOD_H

#include "main.h"
#include "MAX30102.h"
#include "algorithm.h"
#include "math.h"

typedef enum
{
    BLD_NORMAL,     // 正常
    BLD_ERROR,      // 侦测错误
    
} BloodState;       // 血液状态

typedef struct
{
    int     heart;      // 心率数据
    float   SpO2;       // 血氧数据
} BloodData;

// 函数声明
void blood_data_translate(void);
void blood_data_update(void);
void blood_Loop(void);
void Blood_Init(void);
void Quick_HeartRate_SPO2_Calculate(void);
void Simple_HeartRate_Calculate(void);
void HeartRate_Calculate(void);  // 新增心率计算函数

// 外部变量声明
extern BloodData g_blooddata;

#endif
