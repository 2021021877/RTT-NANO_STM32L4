#include "aht10.h"
#include "delay.h"
#include "myiic.h"




float AHT10_Read_Temperature(void)
{
    uint8_t res = 0;
    uint8_t cmd[2] = {0, 0};
    uint8_t temp[6];
    float cur_temp;

    res = IIC_Write_Data(AHT10_IIC_ADDR,AHT10_GET_DATA, cmd, 2); 

    if(res)	return 1;

    res = IIC_Read_Data(AHT10_IIC_ADDR,temp, 6);				

    if(res)	return 1;

    cur_temp = ((temp[3] & 0xf) << 16 | temp[4] << 8 | temp[5]) * 200.0 / (1 << 20) - 50;

    return cur_temp;
}

float AHT10_Read_Humidity(void)
{
    uint8_t res = 0;
    uint8_t cmd[2] = {0, 0};
    uint8_t humi[6];
    float cur_humi;

    res = IIC_Write_Data(AHT10_IIC_ADDR,AHT10_GET_DATA, cmd, 2); 

    if(res)	return 1;

    res = IIC_Read_Data(AHT10_IIC_ADDR,humi, 6);				

    if(res)	return 1;

    cur_humi = ((humi[1]) << 12 | humi[2] << 4 | (humi[3] & 0xF0)) * 100.0 / (1 << 20);

    return cur_humi;
}


uint8_t AHT10_Init(void)
{
    uint8_t res;
    uint8_t temp[2] = {0, 0};

    IIC_Init();		

    res = IIC_Write_Data(AHT10_IIC_ADDR,AHT10_NORMAL_CMD, temp, 2);

    if(res != 0)	return 1;

    delay_ms(300);

    temp[0] = 0x08;
    temp[1] = 0x00;
    res = IIC_Write_Data(AHT10_IIC_ADDR,AHT10_CALIBRATION_CMD, temp, 2);

    if(res != 0)	return 1;

    delay_ms(300);

    return 0;
}


