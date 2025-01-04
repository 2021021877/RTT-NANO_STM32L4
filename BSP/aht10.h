#ifndef __AHT10_H
#define __AHT10_H
#include "stdint.h"




uint8_t AHT10_Init(void);
float AHT10_Read_Temperature(void);
float AHT10_Read_Humidity(void);

#endif



