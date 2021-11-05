/***************************************************************************

      UsartDev中的struct _UsartDevCfg,-在使用SMenu菜单系统时的实现
                         
****************************************************************************/
#ifndef _USART_DEV_CFG_SMENU_H
#define _USART_DEV_CFG_SMENU_H
#ifdef SUPPORT_EX_PREINCLUDE//不支持Preinlude時
  #include "Preinclude.h"
#endif

#include "SMenuUser.h"
#include "UsartDevCfg.h"

/***************************************************************************
                          相关函数
****************************************************************************/

//-------------------------------Cfg操作菜单------------------------------
extern const struct _SMenuFun SMenu_UsartDevCfgCfg;

//-----------------------------波特效率操作菜单------------------------------
#ifndef SUPPORT_USART_DEV_CFG_TINY    //非精简模式时
extern const struct _SMenuFun SMenu_UsartDevCfgBuad;
#endif


/***************************************************************************
                          回调函数
****************************************************************************/

//-----------------------由结构得到正在调整的------------------------------
#if USART_DEV_CFG_COUNT >  1  //多个时
  const struct _UsartDevCfg *SMenu_UsartDevCfg_cbGet(struct _SMenuUser *pUser);
#else
  #define SMenu_UsartDevCfg_cbGet(user) &UsartDevCfg[0]
#endif  

#endif //_USART_DEV_CFG_H
