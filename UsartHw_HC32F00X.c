/******************************************************************************

                   Usart设备硬件在HC32F00X（F003,F005等）中的实现
//本模块负责串口相关的设置,暂不支持校验,需使用对应定时器
//一个停止位时，为模式1，两个停止位时，为模式3(此时B8位用作停止位)
******************************************************************************/
#include "UsartHw_HC32.h"
#include "UsartDevCfg.h"
#include "CMSIS.h"

#define USART_TypeDef  M0P_UART_TypeDef

/******************************************************************************
                        相关实现
******************************************************************************/

//在HC32F005中，倍率取1以与F190兼容
//在HC32F190中，over取32以与F005兼容
#ifdef SUPPORT_HC32F190
  #define   _PRE_SHIFT   4  //取16分频
#else
  #define   _PRE_SHIFT   5  //固定 32分频
#endif

//-----------------------得到定时器值函数------------------------------
static unsigned short _GetSCNT(unsigned long Bandrate,
                                 unsigned long Clk)        //当前串口使用的时钟
{
  //资料公式: Bandrate = (倍率取1*FpClk) / (over取32 * SCNT)
  //fosc为FpClk时钟
  Clk /= Bandrate; // = over * SCNT
  if(Clk & 0x01) Clk ++; //四舍五入
  return Clk >> _PRE_SHIFT;
}

//----------------------------HC32串口标准配置实现----------------------------
//只管输入串口的配置字，不参与串口的使能和其他寄存器操作
void UsartHw_HC32(struct _UsartDevCfg *pCfg,//串口配置结构体
                   void * pUsartHw,           //硬件设备指针
                   unsigned long Clk)        //当前串口使用的时钟
{
  //假定其它位在开机后一直保持默认值  
  USART_TypeDef *pHw = pUsartHw; 
  //设置所需波特率
  unsigned short SCNT = _GetSCNT(UsartDevCfg_GetBuad(pCfg), Clk);
  UsartHw_cbUpdateTimer(pUsartHw, SCNT); //F005时TM = 56635 - SCNT;
  
  //停止位个数控制
	if(pCfg->Cfg  & USART_DEV_CFG_2_STOP)//两个停止位时
    pHw->SCON = (3 << 6) | (0 << 3); //工作模式3(b6),TB8用作停止位0(b3)
  else
    pHw->SCON = (1 << 6) | (0 << 3); //工作模式1
}


