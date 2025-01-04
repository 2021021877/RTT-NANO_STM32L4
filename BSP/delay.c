#include "delay.h"

#include "stm32l4xx.h"
/*********************************************************************************
			  ___   _     _____  _____  _   _  _____  _____  _   __
			 / _ \ | |   |_   _||  ___|| \ | ||_   _||  ___|| | / /
			/ /_\ \| |     | |  | |__  |  \| |  | |  | |__  | |/ /
			|  _  || |     | |  |  __| | . ` |  | |  |  __| |    \
			| | | || |_____| |_ | |___ | |\  |  | |  | |___ | |\  \
			\_| |_/\_____/\___/ \____/ \_| \_/  \_/  \____/ \_| \_/

 *	******************************************************************************
 *	������ֻ��ѧϰʹ�ã�δ���������ɣ��������������κ���;
 *	ALIENTEK Pandora STM32L475 IOT������
 *	ʹ��SysTick����ͨ����ģʽ���ӳٽ��й���
 *	����delay_us,delay_ms
 *	����ԭ��@ALIENTEK
 *	������̳:www.openedv.com
 *	��������:2015/6/10
 *	�汾��V1.1
 *	��Ȩ���У�����ؾ���
 *	Copyright(C) �������������ӿƼ����޹�˾ 2014-2024
 *	All rights reserved
 *	******************************************************************************
 *	�޸�˵��
 *	******************************************************************************/

//���ʹ��ucos,����������ͷ�ļ�����.
#if SYSTEM_SUPPORT_OS
#include "includes.h"					//ucos ʹ��	  
#endif

static uint32_t fac_us = 0;							//us��ʱ������

#if SYSTEM_SUPPORT_OS
static uint16_t fac_ms = 0;				      //ms��ʱ������,��os��,����ÿ�����ĵ�ms��

#if SYSTEM_SUPPORT_OS							//���SYSTEM_SUPPORT_OS������,˵��Ҫ֧��OS��(������UCOS).
//��delay_us/delay_ms��Ҫ֧��OS��ʱ����Ҫ������OS��صĺ궨��ͺ�����֧��
//������3���궨��:
//delay_osrunning:���ڱ�ʾOS��ǰ�Ƿ���������,�Ծ����Ƿ����ʹ����غ���
//delay_ostickspersec:���ڱ�ʾOS�趨��ʱ�ӽ���,delay_init�����������������ʼ��systick
//delay_osintnesting:���ڱ�ʾOS�ж�Ƕ�׼���,��Ϊ�ж����治���Ե���,delay_msʹ�øò����������������
//Ȼ����3������:
//delay_osschedlock:��������OS�������,��ֹ����
//delay_osschedunlock:���ڽ���OS�������,���¿�������
//delay_ostimedly:����OS��ʱ,���������������.

//�����̽���UCOSII��UCOSIII��֧��,����OS,�����вο�����ֲ
//֧��UCOSII
#ifdef 	OS_CRITICAL_METHOD						//OS_CRITICAL_METHOD������,˵��Ҫ֧��UCOSII				
#define delay_osrunning		OSRunning			//OS�Ƿ����б��,0,������;1,������
#define delay_ostickspersec	OS_TICKS_PER_SEC	//OSʱ�ӽ���,��ÿ����ȴ���
#define delay_osintnesting 	OSIntNesting		//�ж�Ƕ�׼���,���ж�Ƕ�״���
#endif

//֧��UCOSIII
#ifdef 	CPU_CFG_CRITICAL_METHOD					//CPU_CFG_CRITICAL_METHOD������,˵��Ҫ֧��UCOSIII	
#define delay_osrunning		OSRunning			//OS�Ƿ����б��,0,������;1,������
#define delay_ostickspersec	OSCfg_TickRate_Hz	//OSʱ�ӽ���,��ÿ����ȴ���
#define delay_osintnesting 	OSIntNestingCtr		//�ж�Ƕ�׼���,���ж�Ƕ�״���
#endif


/**
 * @brief	us����ʱʱ,�ر��������(��ֹ���us���ӳ�)
 *
 * @param   SYSCLK	ϵͳʱ��Ƶ��
 *
 * @return  void
 */
void delay_osschedlock(void)
{
#ifdef CPU_CFG_CRITICAL_METHOD   			//ʹ��UCOSIII
    OS_ERR err;
    OSSchedLock(&err);						//UCOSIII�ķ�ʽ,��ֹ���ȣ���ֹ���us��ʱ
#else										//����UCOSII
    OSSchedLock();							//UCOSII�ķ�ʽ,��ֹ���ȣ���ֹ���us��ʱ
#endif
}
/**
 * @brief	us����ʱʱ,�ָ��������
 *
 * @param   SYSCLK	ϵͳʱ��Ƶ��
 *
 * @return  void
 */
void delay_osschedunlock(void)
{
#ifdef CPU_CFG_CRITICAL_METHOD   			//ʹ��UCOSIII
    OS_ERR err;
    OSSchedUnlock(&err);					//UCOSIII�ķ�ʽ,�ָ�����
#else										//����UCOSII
    OSSchedUnlock();						//UCOSII�ķ�ʽ,�ָ�����
#endif
}
/**
 * @brief	����OS�Դ�����ʱ������ʱ
 *
 * @param   ticks	��ʱ�Ľ�����
 *
 * @return  void
 */
void delay_ostimedly(uint32_t ticks)
{
#ifdef CPU_CFG_CRITICAL_METHOD
    OS_ERR err;
    OSTimeDly(ticks, OS_OPT_TIME_PERIODIC, &err); //UCOSIII��ʱ��������ģʽ
#else
    OSTimeDly(ticks);						    //UCOSII��ʱ
#endif
}
/**
* @brief	systick�жϷ�����,ʹ��OSʱ�õ�
*
* @param   ticks	��ʱ�Ľ�����
*
* @return  void
*/
void SysTick_Handler(void)
{
    HAL_IncTick();

    if(delay_osrunning == 1)					//OS��ʼ����,��ִ�������ĵ��ȴ���
    {
        OSIntEnter();						//�����ж�
        OSTimeTick();       				//����ucos��ʱ�ӷ������
        OSIntExit();       	 				//���������л����ж�
    }
}
#endif



#endif
/**
 * @brief	��ʼ���ӳٺ���,SYSTICK��ʱ�ӹ̶�ΪAHBʱ��
 *
 * @param   SYSCLK	ϵͳʱ��Ƶ��
 *
 * @return  void
 */
void delay_init(uint8_t SYSCLK)
{
#if SYSTEM_SUPPORT_OS 						//�����Ҫ֧��OS.
    uint32_t reload;
#endif
    HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);//SysTickƵ��ΪHCLK
    fac_us = SYSCLK;						//�����Ƿ�ʹ��OS,fac_us����Ҫʹ��

#if SYSTEM_SUPPORT_OS 						//�����Ҫ֧��OS.
    reload = SYSCLK;					  //ÿ���ӵļ������� ��λΪK
    reload *= 1000000 / delay_ostickspersec;	//����delay_ostickspersec�趨���ʱ��
    //reloadΪ24λ�Ĵ���,���ֵ:16777216,��80M�£�Լ209.7ms����
    fac_ms = 1000 / delay_ostickspersec;		//����OS������ʱ�����ٵ�λ
    SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk; //����SYSTICK�ж�
    SysTick->LOAD = reload; 					//ÿ1/OS_TICKS_PER_SEC���ж�һ��
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk; //����SYSTICK
#else
#endif
}


/**
 * @brief	��ʱ΢��(us)����
 *
 * @remark	nus:0~190887435(���ֵ��2^32/fac_us@fac_us=22.5)
 *
 * @param   nus		��Ҫ��ʱ����΢��
 *
 * @return  void
 */
void delay_us(uint32_t nus)
{
    uint32_t ticks;
    uint32_t told, tnow, tcnt = 0;
    uint32_t reload = SysTick->LOAD;				//LOAD��ֵ
    ticks = nus * fac_us; 						//��Ҫ�Ľ�����
    told = SysTick->VAL;        				//�ս���ʱ�ļ�����ֵ

    while(1)
    {
        tnow = SysTick->VAL;

        if(tnow != told)
        {
            if(tnow < told)tcnt += told - tnow;	//����ע��һ��SYSTICK��һ���ݼ��ļ������Ϳ�����.
            else tcnt += reload - tnow + told;
			
            told = tnow;
            if(tcnt >= ticks)break;			//ʱ�䳬��/����Ҫ�ӳٵ�ʱ��,���˳�.
        }
    }
}


/**
 * @brief	��ʱ����(ms)����
 *
 * @param   nms		��Ҫ��ʱ���ٺ���
 *
 * @return  void
 */
void delay_ms(uint16_t nms)
{
    uint32_t i;

    for(i = 0; i < nms; i++) delay_us(1000);
}







