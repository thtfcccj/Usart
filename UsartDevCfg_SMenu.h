/***************************************************************************

      UsartDev�е�struct _UsartDevCfg,-��ʹ��SMenu�˵�ϵͳʱ��ʵ��
                         
****************************************************************************/
#ifndef _USART_DEV_CFG_SMENU_H
#define _USART_DEV_CFG_SMENU_H
#ifdef SUPPORT_EX_PREINCLUDE//��֧��Preinlude�r
  #include "Preinclude.h"
#endif

#include "SMenuUser.h"
#include "UsartDevCfg.h"

/***************************************************************************
                          ��غ���
****************************************************************************/

//-------------------------------Cfg�����˵�------------------------------
extern const struct _SMenuFun SMenu_UsartDevCfgCfg;

//-----------------------------����Ч�ʲ����˵�------------------------------
#ifndef SUPPORT_USART_DEV_CFG_TINY    //�Ǿ���ģʽʱ
extern const struct _SMenuFun SMenu_UsartDevCfgBuad;
#endif

/***************************************************************************
                          �ص�����
****************************************************************************/

//------------------��User�ṹ�õ����ڵ�����UsartDevCfg�ṹ--------------------
#if USART_DEV_CFG_COUNT >  1  //���ʱ
  const struct _UsartDevCfg *SMenu_UsartDevCfg_cbGet(const struct _SMenuUser *pUser);
#else
  #define SMenu_UsartDevCfg_cbGet(user) &UsartDevCfg[0]
#endif  

//------------------��User�ṹ�õ����ڵ�����UsartDevCfg ID--------------------
#if USART_DEV_CFG_COUNT >  1  //���ʱ
  unsigned char SMenu_UsartDevCfg_cbGetId(const struct _SMenuUser *pUser);
#else
  #define SMenu_UsartDevCfg_cbGetId(user) (0)
#endif  




#endif //_USART_DEV_CFG_H
