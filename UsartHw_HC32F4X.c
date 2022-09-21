/******************************************************************************

                   Usart设备硬件在HC32F4X的实现
//本模块负责串口相关的设置,暂不支持校验,需使用对应定时器
//无校验时为模式1，有校验时为模式3(此时B8位作校验位，硬件自动实现)
******************************************************************************/
#include "UsartHw_HC32.h"
#include "UsartDevCfg.h"
#include "CMSIS.h"

#define USART_TypeDef  M4_USART_TypeDef

/******************************************************************************
                        相关实现
******************************************************************************/

static const unsigned char _PrShiftLut[] = {0,2,4,6};
//-----------------------得到定时器值函数------------------------------
//返回B14-15为分频值，B8~0为整数值,经计算，不用小数部分也能达到较高精度
static unsigned short _GetSCNT(unsigned long Bandrate,
                                 unsigned long Clk)        //当前串口使用的时钟
{
  //资料公式: B = C / (8*(2-OV8) × (DIV_Integer+1))
  //OV8取1即16, B可分频，输出需<= 256; 则(DIV_Integer+1) = C / B / (8*1)
  unsigned char Pr = 0;
  unsigned short Div;
  for(; Pr < 4; Pr++){
    unsigned long B = Clk >> _PrShiftLut[Pr];
    B /= Bandrate;   //C / B
    if(B & 0x04) B+=0x04; //四舍五入
    Div = B >> 3; // (8*1)
    if(Div <= 256) break;
  }
  return Div | (unsigned short)Pr << 14;
}

//----------------------------HC32串口标准配置实现----------------------------
//只管输入串口的配置字，不参与串口的使能和其他寄存器操作
void UsartHw_HC32(struct _UsartDevCfg *pCfg,//串口配置结构体
                   void * pUsartHw,           //硬件设备指针
                   unsigned long Clk)        //当前串口使用的时钟
{  
  USART_TypeDef *pHw = pUsartHw;
  //先强制关闭发送与接收(在TE=0&RE=0)，设置才有效
  //保存收发使能与当前状态(0XE0)现场,高16bit置0即可
  unsigned long Cr1 = pHw->CR1 & ((1 << 2) | (1 << 3) | 0xE0);
  pHw->CR1 = 0; //关闭
  unsigned short SCNT = _GetSCNT(UsartDevCfg_GetBuad(pCfg), Clk);
  pHw->PR = SCNT >> 14;
  pHw->BRR = ((SCNT & 0x1ff) - 1) << 8;
  pHw->CR3 = 0;  //不使用其它功能
  //停止位个数控制,CF2,其它默认为0
	if(pCfg->Cfg & USART_DEV_CFG_2_STOP)//两个停止位时
    pHw->CR2 = (1 << 13);
  else  pHw->CR2 = 0;    
  
  //此MCU不支持7个数据位(仅支持8~9位)
  #ifdef SUPPORT_USART_HW_FLITER //支持数字滤波时
    Cr1 |= (1 << 15) | (1 << 30); //B15:OVER0,B30:使能滤波功能,其它默认为0
  #else //不支持时
    Cr1 |= (1 << 15); //B15:OVER0,其它默认为0
  #endif
  //校验控制
	if(pCfg->Cfg & USART_DEV_CFG_PAR_EN){//校验打开时
    Cr1 |= (1 << 10); //使能校验位
    if(pCfg->Cfg & USART_DEV_CFG_ODD)
      Cr1 |= (1 << 9); //使能校奇校验
  }

  pHw->CR1 = Cr1; //恢复现场，设置完成
}


