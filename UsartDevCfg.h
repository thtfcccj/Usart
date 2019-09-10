/******************************************************************************

           UsartDev�е�struct _UsartDevCfg,�ӿڲ�������
//��ģ���ͨ��struct _UsartDevCfgָ������ṹ��Ա�������������𱣴�EEPROM
******************************************************************************/
#ifndef __USART_DEV_CFG_M_H
#define __USART_DEV_CFG_M_H


#include "UsartDev.h"  //������struct _UsartDevCfg����

/******************************************************************************
                             ��غ���
******************************************************************************/


//----------------------------���������----------------------------------
#define UsartDevCfg_GetBuad(pcfg)  \
  (((unsigned long)((pcfg)->BuadH) << 16) + (pcfg)->BuadL)
#define UsartDevCfg_SetBuad(pcfg, buad) \
    do{(pcfg)->BuadH = buad >> 16; (pcfg)->BuadL = buad & 0xffff;}while(0)

//-------------------------------����λ����������--------------------------
#define UsartDevCfg_GetCfg(pcfg) ((pcfg)->Cfg)
#define UsartDevCfg_SetCfg(pcfg, cfg) do{(pcfg)->Cfg = cfg;}while(0)

//-----------------------------ͨѶ����λ����--------------------------------
#define UsartDevCfg_GetCommCfg(pcfg) ((pcfg)->Cfg & 0x0f)
#define UsartDevCfg_SetCommCfg(pcfg,commcfg) do{\
  (pcfg)->Cfg = (commcfg & 0x0f) | ((pcfg)->Cfg & 0xf0);}while(0)
    
//----------------------------�û��йܲ���λ����-------------------------------
#define UsartDevCfg_GetUserCfg(pcfg) ((pcfg)->Cfg & 0xf0)
#define UsartDevCfg_SetUserCfg(pcfg,userCfg) do{\
  (pcfg)->Cfg = (userCfg & 0xf0) | ((pcfg)->Cfg & 0x0f);}while(0)
    
//----------------------------��żУ��λ------------------------------------
//��У��,����ΪżУ��  
#define UsartDevCfg_IsOdd(pcfg)  ((pcfg)->Cfg & USART_DEV_CFG_ODD)
#define UsartDevCfg_SetOdd(pcfg) do{(pcfg)->Cfg |= USART_DEV_CFG_ODD;}while(0)  
#define UsartDevCfg_ClrOdd(pcfg) do{(pcfg)->Cfg &= ~USART_DEV_CFG_ODD;}while(0)
    
//--------------------------------У������λ------------------------------------
//У������,������У��
#define UsartDevCfg_IsParEn(pcfg)  ((pcfg)->Cfg & USART_DEV_CFG_PAR_EN)
#define UsartDevCfg_SetParEn(pcfg) do{(pcfg)->Cfg |= USART_DEV_CFG_PAR_EN;}while(0)  
#define UsartDevCfg_ClrParEn(pcfg) do{(pcfg)->Cfg &= ~USART_DEV_CFG_PAR_EN;}while(0)  
    
//------------------------------------ֹͣλ------------------------------------
//2��ֹͣλ,����Ϊ1��
#define UsartDevCfg_Is2Stop(pcfg)  ((pcfg)->Cfg & USART_DEV_CFG_2_STOP)
#define UsartDevCfg_Set2Stop(pcfg) do{(pcfg)->Cfg |= USART_DEV_CFG_2_STOP;}while(0)  
#define UsartDevCfg_Clr2Stop(pcfg) do{(pcfg)->Cfg &= ~USART_DEV_CFG_2_STOP;}while(0)    
    
//------------------------------------����λ------------------------------------
//7������λ,����Ϊ8��
#define UsartDevCfg_Is7Bit(pcfg)  ((pcfg)->Cfg & USART_DEV_CFG_7_BIT)
#define UsartDevCfg_Set7Bit(pcfg) do{(pcfg)->Cfg |= USART_DEV_CFG_7_BIT;}while(0)  
#define UsartDevCfg_Clr7Bit(pcfg) do{(pcfg)->Cfg &= ~USART_DEV_CFG_7_BIT;}while(0)     

#endif

