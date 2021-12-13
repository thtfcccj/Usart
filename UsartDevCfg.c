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
  
struct _UsartDevCfg UsartDevCfg[USART_DEV_CFG_COUNT];

//----------------------------��ʼ������----------------------------------
void UsartDevCfg_Init(signed char IsInited)
{
  for(unsigned char Ch = 0; Ch < USART_DEV_CFG_COUNT; Ch++){
    if(!IsInited){//װ��Ĭ��
      memcpy(&UsartDevCfg[Ch], 
             &UsartDevCfg_cbDefault[Ch],
             sizeof(struct _UsartDevCfg));
      UsartDevCfg_Save(Ch);
    }
    else{
      Eeprom_Rd(UsartDevCfg_GetInfoBase(Ch),
                &UsartDevCfg[Ch],
                sizeof(struct _UsartDevCfg));
    }
  }
}

//----------------------------���溯��----------------------------------
//�ṹ�仯ʱ���ô˺���������EEPROM
void UsartDevCfg_Save(unsigned char Ch)
{
    Eeprom_Wr(UsartDevCfg_GetInfoBase(Ch),
              &UsartDevCfg[Ch],
              sizeof(struct _UsartDevCfg));
  
    UsartDevCfg_cbSaveNotify(Ch);//ͨ����ʵ�ֲ������µ��ײ�
}

#endif //SUPPORT_USART_DEV_CFG_SAVE
  
