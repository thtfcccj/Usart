/******************************************************************************

               UsartDevCfg���ʵ��

******************************************************************************/
#include "UsartDevCfg.h"

/******************************************************************************
                           �������ʵ��
******************************************************************************/
#ifdef SUPPORT_USART_DEV_CFG_TINY    //����ģʽʱ��Cfg��λΪ������ö����
  const unsigned long UsartDevCfg_Buad[] = {
    9600,//   0  //Ĭ��
    4800,//   1
    2400,//   2
    1200,//   3
    19200,//  4
    38400,//  5
    57600,//  6
    115200,// 7
  };
#endif
  
/******************************************************************************
                             ��Ϊ����ʵ��
******************************************************************************/
#ifdef  SUPPORT_USART_DEV_CFG_SAVE   //֧�ֱ���ʱ
#include <string.h>
#include "Eeprom.h"
#include "InfoBase.h"
  
struct _UsartDevCfg UsartDevCfg; //��֧�ֵ���

//----------------------------��ʼ������----------------------------------
void UsartDevCfg_Init(USART_DEV_CFG_P signed char IsInited)
{
  if(!IsInited){//װ��Ĭ��
    USART_DEV_CFG_THIS_ Cfg = 0; //�β�ʾ��
    memcpy(USART_DEV_CFG_BASE, 
           UsartDevCfg_cbpGetDefault(USART_DEV_CFG_FTHIS),
           sizeof(struct _UsartDevCfg));
    UsartDevCfg_Save(USART_DEV_CFG_FTHIS);
  }
  else{
    Eeprom_Rd(UsartDevCfg_GetInfoBase(USART_DEV_CFG_FTHIS),
              USART_DEV_CFG_BASE,
              sizeof(struct _UsartDevCfg));
  }
}

//----------------------------���溯��----------------------------------
//�ṹ�仯ʱ���ô˺���������EEPROM
void UsartDevCfg_Save(USART_DEV_CFG_CPV)
{
    Eeprom_Wr(UsartDevCfg_GetInfoBase(USART_DEV_CFG_FTHIS),
              USART_DEV_CFG_BASE,
              sizeof(struct _UsartDevCfg));
}

#endif //SUPPORT_USART_DEV_CFG_SAVE
  
