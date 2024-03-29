/******************************************************************************

                   Usart设备硬件定义及相关配置模块在HC32中的定义

******************************************************************************/
#ifndef __USART_HW_HC32_H
#define __USART_HW_HC32_H

/******************************************************************************
                             全局定义说明
******************************************************************************/

//支持USART内部数字滤波时需定义
//#define SUPPORT_USART_HW_FLITER 


/******************************************************************************
                        相关实现
******************************************************************************/
#include "UsartDevCfg.h"

//----------------------------HC32串口标准配置实现----------------------------
//只管输入串口的配置字，不参与串口的使能和其他寄存器操作
void UsartHw_HC32(struct _UsartDevCfg *pCfg,//串口配置结构体
                   void * pUsartHw,           //硬件设备指针
                   unsigned long Clk);        //当前串口使用的时钟

/******************************************************************************
                        回调实现
******************************************************************************/
//------------------------由当前波特率更新至对应定时器---------------------------
//F00x时需实现: TM = 56635 - SCNT;
void UsartHw_cbUpdateTimer(void * pUsartHw, unsigned short SCNT);

#endif

