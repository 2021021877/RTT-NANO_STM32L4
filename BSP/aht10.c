#include "aht10.h"
#include "delay.h"
#include "rtthread.h"

float AHT10_Read_Temperature(
#ifdef SOFT_IIC
    i2c_driver_t *i2c_driver
#endif
#ifdef HARD_IIC
    void
#endif
)
{
    uint8_t res = 0;
    uint8_t cmd[2] = {0, 0};
    uint8_t temp[6];
    float cur_temp;

#ifdef SOFT_IIC
    res = IIC_Write_Data(i2c_driver, AHT10_IIC_ADDR, AHT10_GET_DATA, cmd, 2);
#endif
#ifdef HARD_IIC
#error "HARD_IIC not support"
#endif
    if (res)
        return 1;
#ifdef SOFT_IIC
    res = IIC_Read_Data(i2c_driver, AHT10_IIC_ADDR, temp, 6);
#endif
#ifdef HARD_IIC
#error "HARD_IIC not support"
#endif
    if (res)
        return 1;

    cur_temp = ((temp[3] & 0xf) << 16 | temp[4] << 8 | temp[5]) * 200.0 / (1 << 20) - 50;

    return cur_temp;
}

float AHT10_Read_Humidity(
#ifdef SOFT_IIC
    i2c_driver_t *i2c_driver
#endif
#ifdef HARD_IIC
    void
#endif
)
{
    uint8_t res = 0;
    uint8_t cmd[2] = {0, 0};
    uint8_t humi[6];
    float cur_humi;
#ifdef SOFT_IIC
    res = IIC_Write_Data(i2c_driver, AHT10_IIC_ADDR, AHT10_GET_DATA, cmd, 2);
#endif
#ifdef HARD_IIC
#error "HARD_IIC not support"
#endif
    if (res)
        return 1;
#ifdef SOFT_IIC
    res = IIC_Read_Data(i2c_driver, AHT10_IIC_ADDR, humi, 6);
#endif
#ifdef HARD_IIC
#error "HARD_IIC not support"
#endif
    if (res)
        return 1;

    cur_humi = ((humi[1]) << 12 | humi[2] << 4 | (humi[3] & 0xF0)) * 100.0 / (1 << 20);

    return cur_humi;
}

uint8_t AHT10_Init(
#ifdef SOFT_IIC
    i2c_driver_t *i2c_driver
#endif
#ifdef HARD_IIC
    void
#endif
)
{
    uint8_t res;
    uint8_t temp[2] = {0, 0};

#ifdef SOFT_IIC
    res = IIC_Write_Data(i2c_driver, AHT10_IIC_ADDR, AHT10_NORMAL_CMD, temp, 2);
#endif
#ifdef HARD_IIC
#error "HARD_IIC not support"
#endif
    if (res != 0)
        return 1;
    delay_ms(300);
    temp[0] = 0x08;
    temp[1] = 0x00;
#ifdef SOFT_IIC
    res = IIC_Write_Data(i2c_driver, AHT10_IIC_ADDR, AHT10_CALIBRATION_CMD, temp, 2);
#endif
#ifdef HARD_IIC
#error "HARD_IIC not support"
#endif
    if (res != 0)
        return 1;

    delay_ms(300);

    return 0;
}
