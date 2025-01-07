#include "myiic.h"
#include "delay.h"
#include "stm32l4xx.h"
void IIC_Delay(void)
{
    delay_us(500);
}

/**
 * @brief  IIC start signal 
 * @param  i2c_driver_inst IIC driver instance
 * @return - none       
 * @note   - none
 */
void IIC_Start(i2c_driver_t * i2c_driver)
{
    i2c_driver->pf_i2c_interface_inst->pf_i2c_sda_out(); 
    i2c_driver->pf_i2c_interface_inst->pf_set_sda(1);
    i2c_driver->pf_i2c_interface_inst->pf_set_scl(1);
    i2c_driver->pf_delay_interface_inst->pf_delay_us(100);
    i2c_driver->pf_i2c_interface_inst->pf_set_sda(0);
    i2c_driver->pf_delay_interface_inst->pf_delay_us(100);
    i2c_driver->pf_i2c_interface_inst->pf_set_scl(0);
}
/**
 * @brief  IIC stop signal 
 * @param  i2c_driver_inst IIC driver instance
 * @return - none       
 * @note   - none
 */
void IIC_Stop(i2c_driver_t * i2c_driver)
{
    i2c_driver->pf_i2c_interface_inst->pf_i2c_sda_out();
    i2c_driver->pf_i2c_interface_inst->pf_set_sda(0);
    i2c_driver->pf_i2c_interface_inst->pf_set_scl(1);
    i2c_driver->pf_delay_interface_inst->pf_delay_us(100);
    i2c_driver->pf_i2c_interface_inst->pf_set_sda(1);
    i2c_driver->pf_delay_interface_inst->pf_delay_us(100);
    i2c_driver->pf_i2c_interface_inst->pf_set_scl(0);
}

/**
 * @brief  IIC wait for ack signal 
 * @param  i2c_driver_inst IIC driver instance
 * @return - none       
 * @note   - none
 */
uint8_t IIC_Wait_Ack(i2c_driver_t * i2c_driver)
{
    
    uint16_t ucErrTime = 0;
    i2c_driver->pf_i2c_interface_inst->pf_i2c_sda_in();
    i2c_driver->pf_i2c_interface_inst->pf_set_sda(1);
    i2c_driver->pf_delay_interface_inst->pf_delay_us(100);
    i2c_driver->pf_i2c_interface_inst->pf_set_scl(1);
    i2c_driver->pf_delay_interface_inst->pf_delay_us(100);

    while (i2c_driver->pf_i2c_interface_inst->pf_get_sda())
    {
        ucErrTime++;

        if (ucErrTime > 2000)
        {
            IIC_Stop(i2c_driver);
            return 1;
        }
    }
    i2c_driver->pf_i2c_interface_inst->pf_set_scl(0);
    return 0;
}
/**
 * @brief  IIC ack signal 
 * @param  i2c_driver_inst IIC driver instance
 * @return - none       
 * @note   - none
 */
void IIC_Ack(i2c_driver_t * i2c_driver)
{
    
    i2c_driver->pf_i2c_interface_inst->pf_set_scl(0);
    i2c_driver->pf_i2c_interface_inst->pf_i2c_sda_out();
    i2c_driver->pf_i2c_interface_inst->pf_set_sda(0);
    i2c_driver->pf_delay_interface_inst->pf_delay_us(100);
    i2c_driver->pf_i2c_interface_inst->pf_set_scl(1);
    i2c_driver->pf_delay_interface_inst->pf_delay_us(100);
    i2c_driver->pf_i2c_interface_inst->pf_set_scl(0);
}
/**
 * @brief  IIC nack signal 
 * @param  i2c_driver_inst IIC driver instance
 * @return - none       
 * @note   - none
 */
void IIC_NAck(i2c_driver_t * i2c_driver)
{
    
    i2c_driver->pf_i2c_interface_inst->pf_set_scl(0);
    i2c_driver->pf_i2c_interface_inst->pf_i2c_sda_out();
    i2c_driver->pf_i2c_interface_inst->pf_set_sda(1);
    i2c_driver->pf_delay_interface_inst->pf_delay_us(100);
    i2c_driver->pf_i2c_interface_inst->pf_set_scl(1);
    i2c_driver->pf_delay_interface_inst->pf_delay_us(100);
    i2c_driver->pf_i2c_interface_inst->pf_set_scl(0);
}
/**
 * @brief  IIC send byte 
 * @param  i2c_driver_inst IIC driver instance  txd - data to be sent
 * @return - none       
 * @note   - none
 */
void IIC_Send_Byte(i2c_driver_t * i2c_driver,uint8_t txd)
{
    
    uint8_t t;
    i2c_driver->pf_i2c_interface_inst->pf_i2c_sda_out();
    i2c_driver->pf_i2c_interface_inst->pf_set_scl(0);
    for (t = 0; t < 8; t++)
    {
        i2c_driver->pf_i2c_interface_inst->pf_set_sda((txd & 0x80) >> 7);
        txd <<= 1;
        i2c_driver->pf_i2c_interface_inst->pf_set_scl(1);
        i2c_driver->pf_delay_interface_inst->pf_delay_us(100);
        i2c_driver->pf_i2c_interface_inst->pf_set_scl(0);
        i2c_driver->pf_delay_interface_inst->pf_delay_us(100);
    }

}
/**
 * @brief  IIC read byte 
 * @param  i2c_driver_inst IIC driver instance  ack - ack signal
 * @return - none       
 * @note   - none
 */
uint8_t IIC_Read_Byte(i2c_driver_t * i2c_driver,unsigned char ack)
{
    
    unsigned char i, receive = 0;
    i2c_driver->pf_i2c_interface_inst->pf_i2c_sda_in();
    for (i = 0; i < 8; i++)
    {
        i2c_driver->pf_i2c_interface_inst->pf_set_scl(0);
        i2c_driver->pf_delay_interface_inst->pf_delay_us(100);
        i2c_driver->pf_i2c_interface_inst->pf_set_scl(1);
        receive <<= 1;
        if (i2c_driver->pf_i2c_interface_inst->pf_get_sda())
            receive++;
        i2c_driver->pf_delay_interface_inst->pf_delay_us(100);
    }

    if (!ack)
        IIC_NAck(i2c_driver); 
    else
        IIC_Ack(i2c_driver); 
    return receive;
}
/**
 * @brief  IIC write data 
 * @param  i2c_driver_inst  -IIC driver instance  
 *         addr             - device address
 *         cmd              - command
 *         data             - data to be sent
 *         len              - data length
 * @return -       
 * @note   - no
 */
uint8_t IIC_Write_Data(i2c_driver_t * i2c_driver,
                            uint8_t       addr,
                            uint8_t        cmd,
                            uint8_t      *data,
                            uint8_t        len
                        )
{
    
    IIC_Start(i2c_driver);
    IIC_Send_Byte(i2c_driver,(addr << 1) | 0);

    if (IIC_Wait_Ack(i2c_driver))
    {
        IIC_Stop(i2c_driver);
        return 1;
    }

    IIC_Send_Byte(i2c_driver,cmd);
    IIC_Wait_Ack(i2c_driver);
    for (uint8_t i = 0; i < len; i++)
    {
        IIC_Send_Byte(i2c_driver,data[i]);
        IIC_Wait_Ack(i2c_driver);
    }

    IIC_Stop(i2c_driver);
    return 0;
}

uint8_t IIC_Read_Data(i2c_driver_t * i2c_driver,
                           uint8_t       addr,
                           uint8_t      *data,
                           uint8_t        len
                           )
{
    
    IIC_Start(i2c_driver);
    IIC_Send_Byte(i2c_driver,(addr << 1) | 0x01);
    if (IIC_Wait_Ack(i2c_driver))
    {
        IIC_Stop(i2c_driver);
        return 1;

    }
    for (uint8_t i = 0; i < len; i++)
    {
        if (i == (len - 1))
            data[i] = IIC_Read_Byte(i2c_driver,0);

        else
            data[i] = IIC_Read_Byte(i2c_driver,1);
    }

    IIC_Stop(i2c_driver);
    return 0;
}
iic_status_t iic_driver_init(i2c_driver_t * i2c_driver_inst)
{
    i2c_driver_inst->pf_i2c_interface_inst->pf_i2c_init();
    return IIC_OK;
}

iic_status_t iic_driver_inst(
                         i2c_driver_t           *        i2c_driver_inst,
                         i2c_driver_interface_t *  pf_i2c_interface_inst,
                         delay_interface_t      * pf_delay_interface_inst
)
{
    if ( NULL == i2c_driver_inst || NULL == pf_i2c_interface_inst || NULL == pf_delay_interface_inst)
    {
        return IIC_ERROR;
    }
    i2c_driver_inst->pf_i2c_interface_inst = pf_i2c_interface_inst;
    i2c_driver_inst->pf_delay_interface_inst = pf_delay_interface_inst;
    if(IIC_OK != iic_driver_init(i2c_driver_inst))
    {
        return IIC_ERROR;
    }
    i2c_driver_inst->pf_i2c_interface_inst->pf_set_sda(1);
    i2c_driver_inst->pf_i2c_interface_inst->pf_set_scl(1);
    return IIC_OK;
}