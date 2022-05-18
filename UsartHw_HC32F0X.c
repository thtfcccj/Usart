/******************************************************************************

                   Usart设备硬件在HC32F0X(如F030,F050)的实现
//本模块负责串口相关的设置,暂不支持校验,需使用对应定时器
//无校验时为模式1，有校验时为模式3(此时B8位作校验位，硬件自动实现)
******************************************************************************/
#include "UsartHw_HC32.h"
#include "UsartDevCfg.h"
#include "CMSIS.h"

#define USART_TypeDef  M0P_UART_TypeDef

/******************************************************************************
                        相关实现
******************************************************************************/

//-----------------------得到定时器值函数------------------------------
static unsigned short _GetSCNT(unsigned long Bandrate,
                                 unsigned long Clk)        //当前串口使用的时钟
{
  //资料公式: Bandrate = (倍率取1*FpClk) / (over取32 * SCNT)
  //fosc为FpClk时钟
  Clk /= Bandrate; // = over * SCNT
  if(Clk & 0x01) Clk++; //四舍五入
  return Clk >> 4; //Mode1/3： 0：16采样分频
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
  pHw->SCNT = _GetSCNT(UsartDevCfg_GetBuad(pCfg), Clk);
  //停止位个数控制  
	if(pCfg->Cfg  & USART_DEV_CFG_2_STOP)//两个停止位时
    pHw->SCON_f.STOPBIT = 2;
  else pHw->SCON_f.STOPBIT = 0;//1个停止位
  
  //校验控制
	if(pCfg->Cfg & USART_DEV_CFG_PAR_EN){//校验打开时
    pHw->SCON_f.SM =  3; //工作模式3
    if(pCfg->Cfg  & USART_DEV_CFG_ODD)
      pHw->SCON_f.B8CONT = 2; //硬件奇校验
    else pHw->SCON_f.B8CONT = 1;//硬件偶校验
  }
  else{//无校验
    pHw->SCON_f.SM =  1; //工作模式1
    pHw->SCON_f.B8CONT = 0;//关闭校验
  }
}


