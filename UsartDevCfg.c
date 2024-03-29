/******************************************************************************

               UsartDevCfg相关实现

******************************************************************************/
#include "UsartDevCfg.h"

/******************************************************************************
                           属性相关实现
******************************************************************************/
#ifdef SUPPORT_USART_DEV_CFG_TINY    //精简模式时，Cfg高位为波特率枚举型
  const unsigned long UsartDevCfg_Buad[] = {
    9600,//   0  //默认
    #ifndef SUPPORT_USART_DEV_TINY_DIS_LOW //没有禁止低波特率时     
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
                             行为函数实现
******************************************************************************/
#ifdef  SUPPORT_USART_DEV_CFG_SAVE   //支持保存时
#include <string.h>
#include "Eeprom.h"
#include "InfoBase.h"
  
struct _UsartDevCfg UsartDevCfg[USART_DEV_CFG_COUNT];

//----------------------------初始化函数----------------------------------
void UsartDevCfg_Init(signed char IsInited)
{
  for(unsigned char Ch = 0; Ch < USART_DEV_CFG_COUNT; Ch++){
    if(!IsInited){//装载默认
      memcpy(&UsartDevCfg[Ch], 
             &UsartDevCfg_cbDefault[Ch],
             sizeof(struct _UsartDevCfg));
      UsartDevCfg_Save(Ch);
    }
    else{
      Eeprom_Rd(UsartDevCfg_GetInfoBase(Ch),
                &UsartDevCfg[Ch],
                sizeof(struct _UsartDevCfg));
      UsartDevCfg_cbSaveNotify(Ch);//通报以实现参数更新到底层
    }
  }
}

//----------------------------保存函数----------------------------------
//结构变化时调用此函数保存至EEPROM
void UsartDevCfg_Save(unsigned char Ch)
{
    Eeprom_Wr(UsartDevCfg_GetInfoBase(Ch),
              &UsartDevCfg[Ch],
              sizeof(struct _UsartDevCfg));
  
    UsartDevCfg_cbSaveNotify(Ch);//通报以实现参数更新到底层
}

#endif //SUPPORT_USART_DEV_CFG_SAVE
  
