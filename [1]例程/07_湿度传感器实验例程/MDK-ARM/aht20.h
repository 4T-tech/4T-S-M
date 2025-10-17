#ifndef _AHT20_H_
#define _AHT20_H_

#include "i2c.h"

void AHT20_Init();
void AHT20_Read(float *Temperature,float *Humidity);

#endif
