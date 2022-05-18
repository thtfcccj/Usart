/***************************************************************************

      UsartDev中的struct _UsartDevCfg,-在使用SMenu菜单系统时的实现
                         
****************************************************************************/

#include "UsartDevCfg_SMenu.h"
#include "LedSign.h"
#include "SMenuKey.h"

/***************************************************************************
                          相关函数实现
****************************************************************************/

//-------------------------------Cfg操作菜单------------------------------
static void _GetUsartDevCfgCfg(struct _SMenuUser *pUser, unsigned char Type)
{
  pUser->Cfg = SMENU_USR_ADJ_LOGIC;
  pUser->Adj = UsartDevCfg_GetCfg(SMenu_UsartDevCfg_cbGet(pUser));
  pUser->Max =  0xff;
}

//参数更新,恢复恢复到默认值0
static void _SetUsartDevCfgCfg(struct _SMenuUser *pUser, unsigned char Type)
{
  UsartDevCfg_SetCfg(SMenu_UsartDevCfg_cbGet(pUser), pUser->Adj);
  UsartDevCfg_Save(SMenu_UsartDevCfg_cbGetId(pUser));
}

//主结结构
const struct _SMenuFun SMenu_UsartDevCfgCfg = {
  {LED_SIGN_G,LED_SIGN_F, LED_SIGN_C, LED_SIGN_4 | LED_SIGN_DOT,}, //5.CFG
  _GetUsartDevCfgCfg, _SetUsartDevCfgCfg,
};

//-----------------------------波特效率操作菜单------------------------------
#ifndef SUPPORT_USART_DEV_CFG_TINY    //非精简模式时
static void _GetUsartDevCfgBuad(struct _SMenuUser *pUser, unsigned char Type)
{
  pUser->Cfg = SMENU_USR_ADJ_BIT;
  pUser->Adj = UsartDevCfg_GetBuad(SMenu_UsartDevCfg_cbGet(pUser)) / 100;
}

//参数更新
static void _SetUsartDevCfgBuad(struct _SMenuUser *pUser, unsigned char Type)
{
  UsartDevCfg_SetBuad(SMenu_UsartDevCfg_cbGet(pUser), pUser->Adj * 100);
}

//主结结构
const struct _SMenuFun SMenu_UsartDevCfgBuad = {
  {LED_SIGN_D,LED_SIGN_U, LED_SIGN_B, LED_SIGN_4 | LED_SIGN_DOT,}, //4.BUD
  _GetUsartDevCfgBuad, _SetUsartDevCfgBuad,
};
#endif //#ifndef SUPPORT_USART_DEV_CFG_TINY    //非精简模式时





