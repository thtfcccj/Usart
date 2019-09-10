/******************************************************************************
                   Usart设备硬件在STM32的实现
//本模块负责串口相关的设置（波特率、奇偶校验）									 
******************************************************************************/
#include "UsartHw_STM32.h"
#include "CMSIS.h"

/******************************************************************************
                        相关实现
******************************************************************************/

//-----------------------得到UBRR函数------------------------------
//波特率实际含义为波特率的Q4数
static unsigned short _GetBandQ4(unsigned long Bandrate,
                                   unsigned long Clk)        //当前串口使用的时钟
{
  //波特率计算：UBRR = fosc/(16 * BAUD);
  //fosc为外设的时钟(PCLK1用于USART2、3、4、5，PCLK2用于USART1)
  Clk <<= 4; //最大允许Q4不超限
  Clk /= Bandrate; //Q4 * 16 = Q8
  if((Clk & 0x0f) >= 0x08) Clk += 0x10; //四舍五入
  return Clk >> 4;
}

//----------------------------STM32串口标准配置实现----------------------------
//只管输入串口的配置字，不参与串口的使能和其他寄存器操作
void UsartHw_STM32(struct _UsartDevCfg *pCfg,//串口配置结构体
                   void * pUsartHw,           //硬件设备指针
                   unsigned long Clk)        //当前串口使用的时钟
{
  //假定其它位在开机后一直保持默认值  
  USART_TypeDef *Hw = pUsartHw; 
  //波特率
	Hw->BRR = _GetBandQ4(((unsigned long)(pCfg->BuadH) << 16) + pCfg->BuadL, Clk);
  unsigned char Cfg = pCfg->Cfg;
  //奇偶校验位使能
  Hw->CR1 &= ~USART_CR1_M;
	if(Cfg & USART_DEV_CFG_PAR_EN){
	  Hw->CR1 |= USART_CR1_PCE;
    if(!(Cfg & USART_DEV_CFG_7_BIT)) //8个数据位时,第9位用作校验位
      Hw->CR1 |= USART_CR1_M;
  }
  else{//无校验时,没有第9位
    Hw->CR1 &= ~USART_CR1_PCE;
  }
  //奇校验使能
	if(Cfg & USART_DEV_CFG_ODD)
	  Hw->CR1 |= USART_CR1_PS;
  else Hw->CR1 &= ~USART_CR1_PS;
  //停止位个数控制
  Hw->CR2 &= ~USART_CR2_STOP;
	if(Cfg & USART_DEV_CFG_2_STOP){
    //8个数据位,或7个数据位,但有校验位时,开启两个停止位
    if(!(Cfg & USART_DEV_CFG_7_BIT) || (Cfg & USART_DEV_CFG_PAR_EN))
      Hw->CR2 |= USART_CR2_STOP_1;
  }
  //注:1个停止位时+7个数据位无校验,硬件不支持(全部只有8bit,硬件最小9个)
}


