#include "myiic.h"
#include "delay.h"
#include "stm32l4xx.h"
void IIC_Delay(void)
{
	delay_us(500);
}

/**
 * @brief	IIC��ʼ������
 *
 * @param   void
 *
 * @return  void
 */
void IIC_Init(void)
{
    GPIO_InitTypeDef GPIO_Initure;

    __HAL_RCC_GPIOC_CLK_ENABLE();   //ʹ��GPIOCʱ��
	__HAL_RCC_GPIOD_CLK_ENABLE();   //ʹ��GPIODʱ��

    /*
		SCL - PD6		SDA-PC1
	*/
    GPIO_Initure.Pin 	= GPIO_PIN_1;
    GPIO_Initure.Mode 	= GPIO_MODE_OUTPUT_OD;	//�������
    GPIO_Initure.Pull 	= GPIO_PULLUP;        	//����
    GPIO_Initure.Speed 	= GPIO_SPEED_FAST;   	//����
    HAL_GPIO_Init(GPIOC, &GPIO_Initure);
	
	
	GPIO_Initure.Pin = GPIO_PIN_6;
    GPIO_Initure.Mode = GPIO_MODE_OUTPUT_OD; 	//�������
    GPIO_Initure.Pull = GPIO_PULLUP;        	//����
    GPIO_Initure.Speed = GPIO_SPEED_FAST;   	//����
    HAL_GPIO_Init(GPIOD, &GPIO_Initure);

    IIC_SDA(1);
    IIC_SCL(1);
}

/**
 * @brief	����IIC��ʼ�ź�
 *
 * @param   void
 *
 * @return  void
 */
void IIC_Start(void)
{
    SDA_OUT();     //sda�����
    IIC_SDA(1);
    IIC_SCL(1);
    IIC_Delay();
    IIC_SDA(0);//START:when CLK is high,DATA change form high to low
	 IIC_Delay();
    IIC_SCL(0);//ǯסI2C���ߣ�׼�����ͻ��������
}
/**
 * @brief	����IICֹͣ�ź�
 *
 * @param   void
 *
 * @return  void
 */
void IIC_Stop(void)
{
    SDA_OUT();//sda�����
	IIC_SDA(0);
    IIC_SCL(1);
     IIC_Delay();
	IIC_SDA(1);//STOP:when CLK is high DATA change form low to high
	 IIC_Delay();
	IIC_SCL(0);//����I2C���߽����ź�
}

/**
 * @brief	�ȴ�Ӧ���źŵ���
 *
 * @param   void
 *
 * @return  uint8_t		1������Ӧ��ʧ��
 *					0������Ӧ��ɹ�
 */
uint8_t IIC_Wait_Ack(void)
{
    uint16_t ucErrTime = 0;
    SDA_IN();      //SDA����Ϊ����
    IIC_SDA(1);
     IIC_Delay();
    IIC_SCL(1);
     IIC_Delay();

    while(READ_SDA)
    {
        ucErrTime++;
		
        if(ucErrTime > 2000)
        {
            IIC_Stop();
            return 1;
        }
    }

    IIC_SCL(0);//ʱ�����0
    return 0;
}
/**
 * @brief	����ACKӦ��
 *
 * @param   void
 *
 * @return  void
 */
void IIC_Ack(void)
{
    IIC_SCL(0);
    SDA_OUT();
    IIC_SDA(0);
    IIC_Delay();
    IIC_SCL(1);
     IIC_Delay();
    IIC_SCL(0);
}
/**
 * @brief	������ACKӦ��
 *
 * @param   void
 *
 * @return  void
 */
void IIC_NAck(void)
{
    IIC_SCL(0);
    SDA_OUT();
    IIC_SDA(1);
    IIC_Delay();
    IIC_SCL(1);
     IIC_Delay();
    IIC_SCL(0);
}
/**
 * @brief	IIC����һ���ֽ�
 *
 * @param   txd		��Ҫ���͵�����
 *
 * @return  void
 */
void IIC_Send_Byte(uint8_t txd)
{
    uint8_t t;
    SDA_OUT();
    IIC_SCL(0);//����ʱ�ӿ�ʼ���ݴ���

    for(t = 0; t < 8; t++)
    {
        IIC_SDA((txd & 0x80) >> 7);
        txd <<= 1;
        IIC_SCL(1);
         IIC_Delay();
        IIC_SCL(0);
         IIC_Delay();
    }
}
/**
 * @brief	��1���ֽ�����
 *
 * @param   ack		1������ACK		0������nACK
 *
 * @return  uint8_t		���ض�ȡ����
 */
uint8_t IIC_Read_Byte(unsigned char ack)
{
    unsigned char i, receive = 0;
    SDA_IN();

    for(i = 0; i < 8; i++)
    {
        IIC_SCL(0);
         IIC_Delay();
        IIC_SCL(1);
        receive <<= 1;
        if(READ_SDA)receive++;
        IIC_Delay();
    }

    if(!ack)
        IIC_NAck();//����nACK
    else
        IIC_Ack(); //����ACK

    return receive;
}
uint8_t IIC_Write_Data( uint8_t addr,uint8_t cmd, uint8_t *data, uint8_t len)
{
    IIC_Start();
    IIC_Send_Byte((addr << 1) | 0); 

    if(IIC_Wait_Ack())          
    {
        IIC_Stop();
        return 1;
    }

    IIC_Send_Byte(cmd);         
    IIC_Wait_Ack();             

    for(uint8_t i = 0; i < len; i++)
    {
        IIC_Send_Byte(data[i]);     
        IIC_Wait_Ack();				
    }

    IIC_Stop();
    return 0;
}



uint8_t IIC_Read_Data(uint8_t addr,uint8_t *data, uint8_t len)
{
    IIC_Start();
    IIC_Send_Byte((addr << 1) | 0x01); 
    if(IIC_Wait_Ack())         
    {
        IIC_Stop();
        return 1;
    }

    for(uint8_t i = 0; i < len; i++)
    {
        if(i == (len - 1))
            data[i] = IIC_Read_Byte(0);		

        else
            data[i] = IIC_Read_Byte(1);		
    }

    IIC_Stop();
    return 0;
}

