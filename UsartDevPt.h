/******************************************************************************

                UsartDev模块-多态类型(polymorphic type)统一接口
//此接口主要用于同一软件项目中，存在如MCU内部UsartDev与IO模拟UsartDevIO并存时
//为上层应用提供统一接口的处理方式
******************************************************************************/
#ifndef __USART_DEV_PT_H
#define __USART_DEV_PT_H
#ifdef SUPPORT_EX_PREINCLUDE//不支持Preinlude時
  #include "Preinclude.h"
#endif

/******************************************************************************
                             相关结构定义
******************************************************************************/

//多态类型内部函数类型定义，以方便不同的执有对像向下强制转换:
//接收启动函数
typedef signed char(*UsartDevPt_RcvStart_t)(void *pDev,  //执有对像
                    unsigned char *pBuf,     //接收缓冲区
                    unsigned short Size,     //接收缓冲区大小
                    signed char (*UsartDevInt_t)(void*));//接收回调函数
//接收停止
typedef unsigned short (*UsartDevPt_RcvStop_t)(void *pDev);  //执有对像
//发送启动    
typedef signed char (*UsartDevPt_SendStart_t)(void *pDev,  //执有对像
                     const unsigned char *pBuf,   //发送缓冲区
                     unsigned short Size,         //发送缓冲区大小
                     signed char (*UsartDevInt_t)(void*));//发送回调函数
//发送停止
typedef unsigned short (*UsartDevPt_SendStop_t)(void *pDev);  //执有对像

//多态类型定义，这里只管收发
struct _UsartDevPt{
  UsartDevPt_RcvStart_t RcvStart;
  UsartDevPt_RcvStop_t RcvStop;
  UsartDevPt_SendStart_t SendStart;
  UsartDevPt_SendStop_t SendStop;
};

/******************************************************************************
                             相关实例声明
******************************************************************************/
extern const struct _UsartDevPt  UsartDevPt_Hw; //UsartDev多态类型硬件接口

#ifdef SUPPORT_USART_DEV_IO    //独立IO实现时
  extern const struct _UsartDevPt  UsartDevPt_Io; //UsartDev多态类型IO模拟接口
#endif

#endif

