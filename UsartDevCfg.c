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
  
struct _UsartDevCfg UsartDevCfg; //仅支持单例

//----------------------------初始化函数----------------------------------
void UsartDevCfg_Init(USART_DEV_CFG_P signed char IsInited)
{
  if(!IsInited){//装载默认
    USART_DEV_CFG_THIS_ Cfg = 0; //形参示例
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

//----------------------------保存函数----------------------------------
//结构变化时调用此函数保存至EEPROM
void UsartDevCfg_Save(USART_DEV_CFG_CPV)
{
    Eeprom_Wr(UsartDevCfg_GetInfoBase(USART_DEV_CFG_FTHIS),
              USART_DEV_CFG_BASE,
              sizeof(struct _UsartDevCfg));
}

#endif //SUPPORT_USART_DEV_CFG_SAVE
  
