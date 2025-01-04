#ifndef __MYIIC_H
#define __MYIIC_H

#include "stdint.h"
#define AHT10_IIC_ADDR	0x38			

#define AHT10_CALIBRATION_CMD 	0xE1 	
#define AHT10_NORMAL_CMD 		0xA8 	
#define AHT10_GET_DATA 			0xAC 	
/********************* GPIO Configuration Macros *********************/
/* Configure SDA pin as input */
#define SDA_IN()         do {                                                  \
                             GPIOC->MODER &= ~(3<<(1*2));                      \
                             GPIOC->MODER |= 0<<(1*2);                         \
                            } while(0)

/* Configure SDA pin as output */
#define SDA_OUT()        do {                                                  \
                             GPIOC->MODER &= ~(3<<(1*2));                      \
                             GPIOC->MODER |= 1<<(1*2);                         \
                            } while(0)

/* SCL pin control (PD6) */
#define IIC_SCL(n)  (n ? HAL_GPIO_WritePin(GPIOD,GPIO_PIN_6,GPIO_PIN_SET) :    \
                         HAL_GPIO_WritePin(GPIOD,GPIO_PIN_6,GPIO_PIN_RESET))

/* SDA pin control (PC1) */
#define IIC_SDA(n)  (n ? HAL_GPIO_WritePin(GPIOC,GPIO_PIN_1,GPIO_PIN_SET) :    \
                         HAL_GPIO_WritePin(GPIOC,GPIO_PIN_1,GPIO_PIN_RESET))

/* Read SDA pin state */
#define READ_SDA    HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_1)

/************************ Function Prototypes ************************/
 void     IIC_Init(void);                    /* Initialize IIC 	        */


uint8_t IIC_Write_Data( uint8_t addr,uint8_t cmd, uint8_t *data, uint8_t len);
uint8_t IIC_Read_Data(uint8_t addr,uint8_t *data, uint8_t len)	;

#endif /* __MYIIC_H */