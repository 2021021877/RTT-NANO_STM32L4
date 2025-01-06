#ifndef __AHT10_H
#define __AHT10_H
#include "stdint.h"
#include "myiic.h"

#define AHT10_IIC_ADDR 0x38
#define AHT10_CALIBRATION_CMD 0xE1
#define AHT10_NORMAL_CMD 0xA8
#define AHT10_GET_DATA 0xAC

#define SOFT_IIC 1

#ifdef HARD_IIC
#define I2C_DRIVER_TO_HAL(driver) ((I2C_HandleTypeDef *)driver)
#endif
#ifdef SOFT_IIC
#define I2C_DRIVER_TO_HAL(driver) ((i2c_driver_t)driver)
#endif
uint8_t AHT10_Init(i2c_driver_t  i2c_driver);
float AHT10_Read_Temperature(i2c_driver_t  i2c_driver);
float AHT10_Read_Humidity(i2c_driver_t  i2c_driver);
// typedef struct
// {

//     uint8_t (*Write)(void driver, uint8_t address, uint8_t cmd, uint8_t *data, uint8_t len);
//     uint8_t (*Read)(void driver, uint8_t address, uint8_t *data, uint8_t len);
// } I2C_Driver;

// typedef struct
// {
//     I2C_Driver *i2c_driver; // 硬件接口驱动
// } aht10_t;
// void aht10_driver_inst(aht10_t *aht10, I2C_Driver *i2c_driver);
//  float AHT10_Read_Temperature( aht10_t* aht10);
//  float AHT10_Read_Humidity( aht10_t* aht10);
#endif
