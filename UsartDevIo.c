/******************************************************************************

                   Usart设备抽像在使用IO模拟时的实现
//此模块由HC32Fork出，模块结构基本一致
******************************************************************************/

#ifdef SUPPORT_EX_PREINCLUDE//不支持Preinlude時
  #include "Preinclude.h"
#endif

#ifdef SUPPORT_USART_DEV_IO    //独立IO实现时
  #include "UsartDevIo.h"
#else
  #include "UsartDev.h"
#endif

#include <string.h>

#include "UsartHwIo.h"

//---------------------------内部定义-----------------------------------------
#define USART_TypeDef  struct _UsartHwIo


//-------------------------初始化函数---------------------------------------
#ifdef SUPPORT_USART_DEV_IO    //独立IO实现时
void UsartDevIo_Init(struct _UsartDev *pDev,
                   void * pUsartHw)  //挂接的硬件
#else
void UsartDev_Init(struct _UsartDev *pDev,
                   void * pUsartHw)  //挂接的硬件
#endif
{
  memset(pDev, 0, sizeof(struct _UsartDev));
  pDev->pUsartHw = pUsartHw;
  USART_TypeDef *pHw = (USART_TypeDef *)pUsartHw;
}

//---------------------Usart开始接收数据函数--------------------------------
//启动USART接收数据,返回是否启动成功
//返回启动是否成功,0:成功,其它失败
//注:接收缓冲区大小最高位置1表示在自动模式
#ifdef SUPPORT_USART_DEV_IO    //独立IO实现时
signed char UsartDevIo_RcvStart(struct _UsartDev *pDev,  //所带设备
                              unsigned char *pBuf,     //接收缓冲区
                              unsigned short Size,     //接收缓冲区大小
                              UsartDevInt_t RcvEndInt)//接收回调函数
#else
signed char UsartDev_RcvStart(struct _UsartDev *pDev,  //所带设备
                              unsigned char *pBuf,     //接收缓冲区
                              unsigned short Size,     //接收缓冲区大小
                              UsartDevInt_t RcvEndInt)//接收回调函数
#endif
{
  //if(pDev->Flag & USART_DEV_RCV_BUSY) return -1;  //正在用接收呢！
  pDev->Flag |= USART_DEV_RCV_BUSY;
  pDev->Flag &= ~(USART_DEV_RCV_BUF_OV | //相关发送标志初始化
                  USART_DEV_RCV_ERR | USART_DEV_RCV_AUTO);
  pDev->pRcvBuf = pBuf;
  pDev->RcvEndInt = RcvEndInt;
  if(Size & 0x8000)
    pDev->Flag |= USART_DEV_RCV_AUTO;
  pDev->RcvCount = Size & ~0x8000;
  pDev->RcvLen = 0;

  USART_TypeDef *pUsartHw = pDev->pUsartHw;
  //打开接收使能与中断
  pUsartHw->SCON |= USART_HW_IO_SCON_RE | USART_HW_IO_SCON_RCIE;
  UsartHwIo_SendStop(pUsartHw);//仅支持半双工，停止可能的发送
  UsartHwIo_RcvStart(pUsartHw); //启动接收
  return 0;
}

//--------------------Usart接收数据强制停止---------------------------
//停止正在读过程中的数据或把接收置为停止状态
//返回已接收到的数据个数
#ifdef SUPPORT_USART_DEV_IO    //独立IO实现时
unsigned short UsartDevIo_RcvStop(struct _UsartDev *pDev)
#else
unsigned short UsartDev_RcvStop(struct _UsartDev *pDev)
#endif
{
  USART_TypeDef *pUsartHw = pDev->pUsartHw;
  UsartHwIo_RcvStop(pUsartHw);
  pDev->Flag &= ~USART_DEV_RCV_BUSY;//最后置停止标志
  return pDev->RcvLen;
}

//--------------------Usar发送数据启动函数--------------------------------
//启动USART中接收数据,返回是否启动成功
//返回启动是否成功,0:成功,其它失败
//注:发送缓冲区大小最高位置1表示在自动模式
#ifdef SUPPORT_USART_DEV_IO    //独立IO实现时
signed char UsartDevIo_SendStart(struct _UsartDev *pDev,      //所带设备
                               const unsigned char *pBuf,         //发送缓冲区
                               unsigned short Size,         //发送缓冲区大小
                               UsartDevInt_t SendEndInt) //发送回调函数
#else
signed char UsartDev_SendStart(struct _UsartDev *pDev,      //所带设备
                               const unsigned char *pBuf,         //发送缓冲区
                               unsigned short Size,         //发送缓冲区大小
                               UsartDevInt_t SendEndInt) //发送回调函数
#endif

{
  //if(pDev->Flag & USART_DEV_SEND_BUSY) return -1;  //正在用发送呢！
  pDev->Flag |= USART_DEV_SEND_BUSY;
  pDev->Flag &= ~(USART_DEV_SEND_ERR | //相关发送标志初始化
                  USART_DEV_SEND_AUTO);
  pDev->pSendBuf = pBuf;
  pDev->SendEndInt = SendEndInt;
  if(Size & 0x8000)//自动模式
    pDev->Flag |= USART_DEV_SEND_AUTO;
  pDev->SendCount = Size & ~0x8000;
  pDev->SenLen = 0;

  USART_TypeDef *pUsartHw = pDev->pUsartHw;
  pUsartHw->SBUF = *pBuf;//装载数据准备发送
  pUsartHw->SCON |= USART_HW_IO_SCON_TCIE;//发送完成中断使能
  UsartHwIo_RcvStop(pUsartHw);//仅支持半双工，停止可能的接收
  UsartHwIo_SendStart(pUsartHw);
  return 0;
}

//--------------------Usart发送数据停止函数---------------------------
//停止正在写过程中的数据或把发送置为停止状态
//返回已发送到的数据个数
#ifdef SUPPORT_USART_DEV_IO    //独立IO实现时
unsigned short UsartDevIo_SendStop(struct _UsartDev *pDev)
#else
unsigned short UsartDev_SendStop(struct _UsartDev *pDev)
#endif
{
  USART_TypeDef *pUsartHw = pDev->pUsartHw;
  UsartHwIo_SendStop(pUsartHw);
  pDev->Flag &= ~USART_DEV_SEND_BUSY;//最后置停止标志
  return pDev->SenLen;
}

//---------------------内部收发停止宏----------------------------
#ifdef SUPPORT_USART_DEV_IO    //独立IO实现时
  #define _RcvStop(dev) UsartDevIo_RcvStop(dev)
  #define _SendStop(dev) UsartDevIo_SendStop(dev)
#else
  #define _RcvStop(dev) UsartDev_RcvStop(dev)
  #define _SendStop(dev) UsartDev_SendStop(dev)
#endif

//---------------------USART硬件接收中断处理函数----------------------------
#ifdef SUPPORT_USART_DEV_IO    //独立IO实现时
void UsartDevIo_RcvIRQ(struct _UsartDev *pDev)
#else
void UsartDev_RcvIRQ(struct _UsartDev *pDev)
#endif
{
  USART_TypeDef *pUsartHw = pDev->pUsartHw;//临时指针
   unsigned char RcvData;
	 unsigned short RcvLen;
  
	//接收完成中断
	if(pUsartHw->ISR & USART_HW_IO_ISR_RI){
	  RcvData = pUsartHw->SBUF;//无条件接收数据
    pUsartHw->ISR |= USART_HW_IO_ISR_RI;//清接收中断
    if(pUsartHw->SCON & USART_HW_IO_SCON_RCIE){//收数据过程中时
      pDev->RcvData = RcvData;
       //判断帧错误
      if(pUsartHw->ISR & USART_HW_IO_ISR_FE){
        pUsartHw->ISR &= ~USART_HW_IO_ISR_FE; //清错误标志
        pDev->Flag |= USART_DEV_RCV_ERR;
        return;
      }
      RcvLen = pDev->RcvLen;
      //缓冲区溢出检查,溢出时后面的再也不收了！
      if(RcvLen >= pDev->RcvCount){
        if(pDev->RcvEndInt(pDev))//收完数据强制完成(可重启动)
          _RcvStop(pDev);
        return;
      }
      //收到缓冲区中
      *(pDev->pRcvBuf + RcvLen) = RcvData;
      pDev->RcvLen++;
      if(!(pDev->Flag & USART_DEV_RCV_AUTO)){
        if(pDev->RcvEndInt(pDev))//手工停止了
          _RcvStop(pDev);
      }
    }//end 收数据过程中时
	}
 
	//发送完成中断
  if(pUsartHw->ISR & USART_HW_IO_ISR_TI){
    pUsartHw->ISR |= USART_HW_IO_ISR_TI;//清中断
    if(pUsartHw->SCON & USART_HW_IO_SCON_TCIE){//发数据过程中时
      pDev->SenLen++; //已写入缓冲区一个数了
      if(pDev->SenLen < pDev->SendCount){
        if(!(pDev->Flag & USART_DEV_SEND_AUTO)){//发送完每个数通报
          if(pDev->SendEndInt(pDev)){//用户让结束了
            _SendStop(pDev);
            return;
          }
        }
        pUsartHw->SBUF = pDev->pSendBuf[pDev->SenLen];//继续发送数
      }
      //最后一个数移入，但没发出,切换为完成中断
      else{
        _SendStop(pDev);//关闭发送完成中断
        pDev->SendEndInt(pDev);//发送完成回调处理
        pUsartHw->SCON &= ~USART_HW_IO_SCON_TCIE;
      }
	  }//end 发数据过程中时
  }
}



