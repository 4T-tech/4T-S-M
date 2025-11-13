#ifndef __AHT20_H
#define __AHT20_H

#include "main.h"

void AHT20_Init(void);
void AHT20_Init_Simple(void);
uint8_t AHT20_Read_Status(void);
void AHT20_Read(float *Temperature, float *Humidity);
uint8_t AHT20_Read_Enhanced(float *Temperature, float *Humidity, uint8_t retry_count);
uint8_t AHT20_Test_Device(void);

#endif

