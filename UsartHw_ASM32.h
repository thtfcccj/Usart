/******************************************************************************

                   Usart设备硬件定义及相关配置模块在ASM32中的定义

******************************************************************************/
#ifndef __USART_HW_ASM32_H
#define __USART_HW_ASM32_H

#ifdef SUPPORT_EX_PREINCLUDE//不支持Preinlude時
  #include "Preinclude.h"
#endif

/******************************************************************************
                        相关实现
******************************************************************************/
#include "UsartDevCfg.h"

//----------------------------ASM32串口标准配置实现----------------------------
//只管输入串口的配置字，不参与串口的使能和其他寄存器操作
void UsartHw_ASM32(const struct _UsartDevCfg *pCfg,//串口配置结构体
                   void * pUsartHw,           //硬件设备指针
                   unsigned long Clk);        //当前串口使用的时钟

#endif

