/******************************************************************************

                   Usart设备硬件之LPC_Arm相关实现

******************************************************************************/
#include "UsartHw_LPC_Arm.h"
#include "LPC12XXBit.h"

/******************************************************************************
                        相关实现
******************************************************************************/

//=========================设置UART0波特率函数============================	
//返回:
//0-3:DIVADDVAL,细分值除位值
//4-7:MULVAL,,细分值剩位值
//12-27:分频值
//－1表示计算失败，原因为主频太高或波特效率太低了
static unsigned int _GetBandrater(unsigned int Bandrate,//设置的波特率，
                           unsigned int Mck)            //模块时钟
{
  //当波特率出现问题时初始化为9600
  if((Bandrate < 1000) && (Bandrate > 172800)) Bandrate = 9600;

  //得到除数值,Q12制小数(固定16分频Q4+Q8)
  unsigned int ulBaundDiv = (Mck << 6) / Bandrate;
  ulBaundDiv <<= 2;

  //与系统分频数成整倍(或接近整倍)关系统时,直接分频,无小数部分
  unsigned int ulBaundMod = ulBaundDiv & 0x00000fff;
  if((ulBaundMod < 0x0000000f) || (ulBaundMod > 0x00000ff0)){
    if(ulBaundMod > 0x00000ff0) ulBaundDiv += (1 << 12);
    ulBaundDiv &= ~0xfff;
    return ulBaundDiv | 0x10; //MULVAL至少为1
  }

  //当不成整倍关系时,使用小数,具体算法是:
  //  整数部分为2的次方数,此值用2的次方数逼近直到商为1-2之间
  //  小数部分为商-1,即0-1之间此值用分子分母去逼近,直到误差最小
  unsigned int ulDifNum = 0x1000;//细分除数值,Q12
  while((ulBaundDiv /  ulDifNum) != 1) ulDifNum <<= 1;

  ulBaundMod = ulBaundDiv / (ulDifNum >> 12);//得到Q12制小数校正值,为1-2之间
  ulBaundMod -= 0x1000;		//得到小数部分0-1之间,Q12制小数
  ulBaundDiv =  ulDifNum;	//得到整数部分,2的次方数

  //采用逼近法得到小数的分子分母
  ulDifNum = 0xffffffff;	//置差值为最大
  unsigned char uDiv,uMui;
  //j为分母值,i为分子值
  for(unsigned char j = 1; j <= 15; j++){
    for(unsigned char i = j + 1; i <= 15; i++){
      //得到Q12制小数值,4舍5入
      unsigned int ulDifNum2 = (j << 13) / i;
      if(ulDifNum2 & 0x1) ulDifNum2++;
      ulDifNum2 >>= 1;

      //若差值更小时记住当前值及差值
      if(ulDifNum2 >= ulBaundMod) ulDifNum2 -=  ulBaundMod;
	  else ulDifNum2 = ulBaundMod - ulDifNum2;
      if(ulDifNum2 < ulDifNum){
        uDiv = j;
        uMui = i;
        ulDifNum = ulDifNum2;
      }
    }
  }
  ulBaundDiv &= ~0xfff;
  return ulBaundDiv |= uDiv | (uMui << 4);
}

//---------------------------------配置函数---------------------------------
//使用Usart推荐的配置结构,返回是否成功
signed char UsartHw_Cfg(struct _USART_HW *pUartHw,//需配置的硬件
                        const struct _UsartDevCfg *pCfg)
{
  unsigned long Bandrate = ((unsigned long)pCfg->BuadH << 16) + pCfg->BuadL;
  //得到配置波特率产生器
  unsigned int Bandrater = _GetBandrater(Bandrate,UsartHw_cbGetMck());

  //得到其它配置
  unsigned int HwCfg;//pUartHw->LCR中的Cfg;
  unsigned char Cfg = pCfg->Cfg;
  if(Cfg & USART_DEV_CFG_PAR_EN){
    if(Cfg & USART_DEV_CFG_ODD) HwCfg = LPC_PARITY_ODD;
    else HwCfg = LPC_PARITY_EVEN;
  }
  else{
    if(Cfg & USART_DEV_CFG_ODD) HwCfg = LPC_PARITY_FORCE1;
    else HwCfg = LPC_PARITY_FORCE0;
  }
  if(Cfg & USART_DEV_CFG_2_STOP) HwCfg |= LPC_STOPBIT2;
  if(Cfg & USART_DEV_CFG_7_BIT) HwCfg |= LPC_WORD_LEN7;
  else  HwCfg |= LPC_WORD_LEN8;

  //配置LSR开始
  pUartHw->LCR = LPC_DLAB;
  pUartHw->LCR = HwCfg | LPC_DLAB;
  pUartHw->FDR = Bandrater & 0xff;
  //计算分频数
  Bandrater >>= 12;//分频数
  if(Bandrater < 0x100){//直接分频
    pUartHw->DLL = (unsigned char)Bandrater;
    pUartHw->DLM = 0;
  }
  else if(Bandrater < 0x1000){//使用DLM分频，为16倍关系
    pUartHw->DLL = (unsigned char)(Bandrater & 0x0f);
    pUartHw->DLM = (unsigned char)(Bandrater >> 4);
  }
  else{//分不过来了!
    pUartHw->DLL = 0xff;
    pUartHw->DLM = 0xff;
  }
  //配置完成
  pUartHw->LCR &= ~LPC_DLAB;
  //停止并复位相关资源
  pUartHw->FCR = LPC_FIFO_EN;
  pUartHw->FCR = LPC_FIFO_EN | LPC_RXFIFO_RST | LPC_TXFIFO_RST;
  pUartHw->IER = 0;
  return 0;
}