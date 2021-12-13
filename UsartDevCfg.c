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
  
