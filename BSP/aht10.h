#ifndef __AHT10_H
#define __AHT10_H
#include "stdint.h"
#include "myiic.h"

#define AHT10_IIC_ADDR 0x38
#define AHT10_CALIBRATION_CMD 0xE1
#define AHT10_NORMAL_CMD 0xA8
#define AHT10_GET_DATA 0xAC
/*note
   if you are using a hardware iic
   1.#define HARD_IIC
    2.  #error "HARD_IIC not support" replace with like this
    ==IIC_Write_Data(i2c_driver, AHT10_IIC_ADDR, AHT10_GET_DATA, cmd, 2)==
*/

#define SOFT_IIC
// #define HARD_IIC

#ifdef SOFT_IIC
#if defined(HARD_IIC)
#error "SOFT_IIC and HARD_IIC cannot be defined at the same time!"
#endif
#else
#ifdef HARD_IIC
// 继续执行
#endif
#endif

uint8_t AHT10_Init(
#ifdef SOFT_IIC
    i2c_driver_t *i2c_driver
#endif
#ifdef HARD_IIC
    void
#endif
);
float AHT10_Read_Temperature(
#ifdef SOFT_IIC
    i2c_driver_t *i2c_driver
#endif
#ifdef HARD_IIC
    void
#endif
);
float AHT10_Read_Humidity(
#ifdef SOFT_IIC
    i2c_driver_t *i2c_driver
#endif
#ifdef HARD_IIC
    void
#endif
);
#endif