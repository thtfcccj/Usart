/******************************************************************************

           UsartDev中的struct _UsartDevCfg,接口操作函数
//此模块仅通过struct _UsartDevCfg指针操作结构成员变量，但不负责保存EEPROM
******************************************************************************/
#ifndef __USART_DEV_CFG_M_H
#define __USART_DEV_CFG_M_H


#include "UsartDev.h"  //仅负责struct _UsartDevCfg操作

/******************************************************************************
                             相关函数
******************************************************************************/


//----------------------------波特率相关----------------------------------
#define UsartDevCfg_GetBuad(pcfg)  \
  (((unsigned long)((pcfg)->BuadH) << 16) + (pcfg)->BuadL)
#define UsartDevCfg_SetBuad(pcfg, buad) \
    do{(pcfg)->BuadH = buad >> 16; (pcfg)->BuadL = buad & 0xffff;}while(0)

//-------------------------------配置位整体操作相关--------------------------
#define UsartDevCfg_GetCfg(pcfg) ((pcfg)->Cfg)
#define UsartDevCfg_SetCfg(pcfg, cfg) do{(pcfg)->Cfg = cfg;}while(0)

//-----------------------------通讯部分位定义--------------------------------
#define UsartDevCfg_GetCommCfg(pcfg) ((pcfg)->Cfg & 0x0f)
#define UsartDevCfg_SetCommCfg(pcfg,commcfg) do{\
  (pcfg)->Cfg = (commcfg & 0x0f) | ((pcfg)->Cfg & 0xf0);}while(0)
    
//----------------------------用户托管部分位定义-------------------------------
#define UsartDevCfg_GetUserCfg(pcfg) ((pcfg)->Cfg & 0xf0)
#define UsartDevCfg_SetUserCfg(pcfg,userCfg) do{\
  (pcfg)->Cfg = (userCfg & 0xf0) | ((pcfg)->Cfg & 0x0f);}while(0)
    
//----------------------------奇偶校验位------------------------------------
//奇校验,否则为偶校验  
#define UsartDevCfg_IsOdd(pcfg)  ((pcfg)->Cfg & USART_DEV_CFG_ODD)
#define UsartDevCfg_SetOdd(pcfg) do{(pcfg)->Cfg |= USART_DEV_CFG_ODD;}while(0)  
#define UsartDevCfg_ClrOdd(pcfg) do{(pcfg)->Cfg &= ~USART_DEV_CFG_ODD;}while(0)
    
//--------------------------------校验允许位------------------------------------
//校验允许,否则无校验
#define UsartDevCfg_IsParEn(pcfg)  ((pcfg)->Cfg & USART_DEV_CFG_PAR_EN)
#define UsartDevCfg_SetParEn(pcfg) do{(pcfg)->Cfg |= USART_DEV_CFG_PAR_EN;}while(0)  
#define UsartDevCfg_ClrParEn(pcfg) do{(pcfg)->Cfg &= ~USART_DEV_CFG_PAR_EN;}while(0)  
    
//------------------------------------停止位------------------------------------
//2个停止位,否则为1个
#define UsartDevCfg_Is2Stop(pcfg)  ((pcfg)->Cfg & USART_DEV_CFG_2_STOP)
#define UsartDevCfg_Set2Stop(pcfg) do{(pcfg)->Cfg |= USART_DEV_CFG_2_STOP;}while(0)  
#define UsartDevCfg_Clr2Stop(pcfg) do{(pcfg)->Cfg &= ~USART_DEV_CFG_2_STOP;}while(0)    
    
//------------------------------------数据位------------------------------------
//7个数据位,否则为8个
#define UsartDevCfg_Is7Bit(pcfg)  ((pcfg)->Cfg & USART_DEV_CFG_7_BIT)
#define UsartDevCfg_Set7Bit(pcfg) do{(pcfg)->Cfg |= USART_DEV_CFG_7_BIT;}while(0)  
#define UsartDevCfg_Clr7Bit(pcfg) do{(pcfg)->Cfg &= ~USART_DEV_CFG_7_BIT;}while(0)     

#endif

