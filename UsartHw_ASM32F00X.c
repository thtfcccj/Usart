/******************************************************************************

                   Usart设备硬件在ASM32F00X（F003,F005等）中的实现
//本模块负责串口相关的设置,暂不支持校验,使用使用自身波特率生成模式
//一个停止位时，为模式1，两个停止位时，为模式3(此时B8位用作停止位)
////此模块由HC32Fork出，模块结构基本一致
******************************************************************************/
#include "UsartHw_ASM32.h"
#include "UsartDevCfg.h"
#include "CMSIS.h"

#define USART_TypeDef  UART_TypeDef

/******************************************************************************
                        相关实现
******************************************************************************/


//-----------------------得到定时器值函数------------------------------
static unsigned short _GetSCNT(unsigned long Bandrate,
                                 unsigned long Clk)        //当前串口使用的时钟
{
  //页P311: 配置UARTx_BAUDCR. SELF_BRG=1,使用自身波特率生成模式
  //资料公式: Bandrate = ((UARTx_SCON.BAUD=0 + 1)*FpClk) / (32 * (UARTx_BAUDCR.BRG + 1))
  //精简后为： UARTx_BAUDCR.BRG = ((FpClk / Bandrate) >> 5) - 1;
  Clk /= Bandrate;
  if(Clk & 0x01) Clk ++; //四舍五入
  return (Clk >> 5) - 1;
}

//----------------------------ASM32串口标准配置实现----------------------------
//只管输入串口的配置字，不参与串口的使能和其他寄存器操作
void UsartHw_ASM32(const struct _UsartDevCfg *pCfg,//串口配置结构体
                   void * pUsartHw,           //硬件设备指针
                   unsigned long Clk)        //当前串口使用的时钟
{
  //假定其它位在开机后一直保持默认值  
  USART_TypeDef *pHw = pUsartHw; 
  //设置所需波特率： b16为UARTx_BAUDCR. SELF_BRG=1
  pHw->BRG = (1 << 16) | _GetSCNT(UsartDevCfg_GetBuad(pCfg), Clk);
  
  //停止位个数控制
	if(pHw->SCON  & USART_DEV_CFG_2_STOP)//两个停止位时
    pHw->SCON = (3 << 6) | (0 << 3); //工作模式3(b6),TB8用作停止位0(b3)
  else
    pHw->SCON = (1 << 6) | (0 << 3); //工作模式1
}


