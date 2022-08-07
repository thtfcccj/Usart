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
    #ifndef SUPPORT_USART_DEV_TINY_DIS_LOW //û�н�ֹ�Ͳ�����ʱ     
      4800,//   1
      2400,//   2
      1200,//   3
    #endif
    19200,//  4or1
    38400,//  5or2
    57600,//  6or3
    115200,// 7or4
  };

//unsigned long UsartDevCfg_GetBuad(const struct _UsartDevCfg *pCfg)
//{
//  return UsartDevCfg_Buad[(pCfg->Cfg & USART_DEV_BUAD_MASK) >> USART_DEV_BUAD_SHIFT];
//}

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
      UsartDevCfg_cbSaveNotify(Ch);//ͨ����ʵ�ֲ������µ��ײ�
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
  
