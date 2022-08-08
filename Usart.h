/*******************************************************************************

                   Usart实例模块接口
此模块实现实例化UsartDev模块，并通过ID号提供接口
此模块接口独立于应用与硬件，内部通过回调独立于硬件
*******************************************************************************/
#ifndef __USART_H
#define	__USART_H
#ifdef SUPPORT_EX_PREINCLUDE//不支持Preinlude時
  #include "Preinclude.h"
#endif

/******************************************************************************
		                      相关配置
******************************************************************************/

//禁止内部配置中断时定义，定义后，中断的配置由本模块实现改由外部手工实现。
//#define USART_DIS_CFG_INT   

#ifndef USART_COUNT
  #define USART_COUNT  1//USART总数
#endif


/******************************************************************************
		                      相关结构
******************************************************************************/
#include "UsartDev.h"

typedef void(*RtsFun_t)(void);    //控制RTS的函数结构

//主结构
struct _Usart{
  struct _UsartDev Dev[USART_COUNT];   //实例化
};

extern struct _Usart Usart; //单例模式

/******************************************************************************
		                          对外函数-普通
******************************************************************************/

//----------------------------------初始化-------------------------------------
//需在UsartDevCfg_Init()后调用
void  Usart_Init(void);

//------------------------------由ID得到指针-----------------------------------
//struct _UsartDev *Usart_GetDev(unsigned char UsartId);
#define Usart_GetDev(usartId) (&Usart.Dev[usartId])  //直接实现未检查下标

//-------------------------------由指针得到ID----------------------------------
//负值未找到
signed char Usart_GetUsartId(const struct _UsartDev *pDev);

//-------------------------RTS置低电平函数接口------------------------------
//回调方式实现,应用层调用，下标为UsartId
extern const RtsFun_t Usart_cbClrRTS[USART_COUNT];

//---------------------------RTS置高电平函数接口------------------------------
//回调方式实现,应用层调用，下标为UsartId
extern const RtsFun_t Usart_cbSetRTS[USART_COUNT];

/******************************************************************************
		                           回调函数
******************************************************************************/

//---------------------------挂接的底层硬件指针--------------------------------
extern void* const Usart_cbHw[USART_COUNT];

//---------------------------------中断配置接口--------------------------------
#ifndef USART_DIS_CFG_INT //没有禁止本模块实现时
extern const RtsFun_t Usart_cbCfgInt[USART_COUNT];
#endif

#endif //#define	__USART_H









