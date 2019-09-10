/******************************************************************************

                   Usart设备硬件之Atmega相关实现

******************************************************************************/
#include "UsartHw_ATMEGA.h"

/******************************************************************************
                        相关实现
******************************************************************************/

//-----------------------得到UBRR函数------------------------------
//返回-1表示计算失败，原因为主频太高或波特效率太低了
//波特率计算：UBRR = fosc/(16 * BAUD) - 1;
//返回值；-1，波特率设置错误；0，
#if USART_HW_CPU_TYPE != 0

static unsigned short _GetUbrr(unsigned long Bandrate,
                               unsigned long Mck)
{
  unsigned long MckDiv;
  //判断最大波特率
  if(Bandrate > (Mck >> 4)) return (unsigned short)-1;
  MckDiv = (Mck << 2) / (Bandrate << 0);//放大32倍进行运算
  MckDiv >>= 5;
  if(MckDiv & 1) MckDiv++;
  MckDiv >>= 1;
  MckDiv -= 1;
  return MckDiv;
}
#endif //#if USART_HW_CPU_TYPE != 0

//-----------------------Atmega32或16的标准配置实现------------------------------
//送入配置参数,同时返回实际上的波特率
//此函数将复位整个Usart的当前操作

#if USART_HW_CPU_TYPE == 1
void UsartHw_Mega32Cfg(struct _UsartDevCfg *pCfg)
{
  //因Atmega32仅一个串口,故这里直接实现
  //关闭串口,中断使能
  UCSRB &= ~((1 << TXEN) | (1 << RXEN) |
                  (1 << RXCIE) | (1 << TXCIE) | (1 << UDRIE));

  //硬件寄存器初始化
  unsigned char Cfg = pCfg->Cfg;
  unsigned char Ucsrc;
  if(Cfg & USART_DEV_CFG_PAR_EN){
    if(Cfg & USART_DEV_CFG_ODD)
      Ucsrc = (1 << UPM1) | (1 << UPM0) | (1 << URSEL);
    else Ucsrc = (1 << UPM1) | (1 << URSEL);
  }
  else Ucsrc = (1 << URSEL);

  if(Cfg & USART_DEV_CFG_2_STOP)
    Ucsrc |= (1 << USBS);

  if(Cfg & USART_DEV_CFG_7_BIT)
    Ucsrc |= (1 << UCSZ1);
  else
    Ucsrc |= (1 << UCSZ1) | (1 << UCSZ0);
  UCSRC = Ucsrc;

  //配置波特效率产生器
  unsigned long Buad = ((unsigned long)pCfg->BuadH << 16) | pCfg->BuadL;
  unsigned short Ubrr  = _GetUbrr(Buad, UsartHw_cbGetMck());
  UCSRA &= ~(1 << MPCM);
  if(Ubrr == (unsigned short)-1){
    //16倍重采样不能满足要求，采用8倍相当于降低主频一半，再试试
    Ubrr  = _GetUbrr(Buad, UsartHw_cbGetMck() >> 1);
    if(Ubrr == (unsigned short)-1) return;//波特率设的太低了
    UCSRA |= 1 << U2X;//倍频
  }
  //设置波特率
  UBRRL = (unsigned char)Ubrr;
  UBRRH = (unsigned char)(Ubrr >> 8);

}

#endif // USART_HW_CPU_TYPE == 0


