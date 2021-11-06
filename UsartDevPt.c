/******************************************************************************

                UsartDev模块-多态类型(polymorphic type)实例实现

******************************************************************************/
#include "UsartDevPt.h"

/******************************************************************************
                             相关实例实现
******************************************************************************/
//---------------------------UsartDev多态类型硬件接口-------------------------
#include "UsartDev.h"
const struct _UsartDevPt  UsartDevPt_Hw = {
  (UsartDevPt_RcvStart_t)  UsartDev_RcvStart, //接收启动
  (UsartDevPt_RcvStop_t)   UsartDev_RcvStop,//接收停止
  (UsartDevPt_SendStart_t) UsartDev_SendStart,//发送启动    
  (UsartDevPt_SendStop_t)  UsartDev_SendStop,//发送停止
};

//---------------------------UsartDev多态类型IO模拟接口-------------------------
#ifdef SUPPORT_USART_DEV_IO    //独立IO实现时
#include "UsartDevIo.h"
const struct _UsartDevPt UsartDevPt_Io = {
  (UsartDevPt_RcvStart_t)  UsartDevIo_RcvStart, //接收启动
  (UsartDevPt_RcvStop_t)   UsartDevIo_RcvStop,//接收停止
  (UsartDevPt_SendStart_t) UsartDevIo_SendStart,//发送启动    
  (UsartDevPt_SendStop_t)  UsartDevIo_SendStop,//发送停止
};

#endif

