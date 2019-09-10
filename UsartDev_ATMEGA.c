/******************************************************************************

                   Usart设备抽像在ATMEGA中的实现
//此模块支持接口提供的所有功能
******************************************************************************/

#include "UsartDev.h"
#include <string.h>
#include "UsartHw_ATMEGA.h"

//-------------------------初始化函数---------------------------------------
void UsartDev_Init(struct _UsartDev *pDev,
                   void * pUsartHw)  //挂接的硬件
{
  memset(pDev, 0, sizeof(struct _UsartDev));
  pDev->pUsartHw = pUsartHw;
  struct _USART_HW *pHw = (struct _USART_HW *)pUsartHw;
  //关闭串口,及中断使能
  pHw->UCSRB &= ~((1 << TXEN) | (1 << RXEN) |
                 (1 << RXCIE) | (1 << TXCIE) | (1 << UDRIE));
}

//---------------------Usart开始接收数据函数--------------------------------
//启动USART接收数据,返回是否启动成功
//返回启动是否成功,0:成功,其它失败
//注:接收缓冲区大小最高位置1表示在自动模式
signed char UsartDev_RcvStart(struct _UsartDev *pDev,  //所带设备
                              unsigned char *pBuf,     //接收缓冲区
                              UsartSize_t Size,     //接收缓冲区大小
                              UsartDevInt_t RcvEndInt)//接收回调函数
{
  //if(pDev->Flag & USART_DEV_RCV_BUSY) return -1;  //正在用接收呢！
  pDev->Flag |= USART_DEV_RCV_BUSY;
  pDev->Flag &= ~(USART_DEV_RCV_BUF_OV | //相关发送标志初始化
                  USART_DEV_RCV_ERR | USART_DEV_RCV_AUTO);
  pDev->pRcvBuf = pBuf;
  pDev->RcvEndInt = RcvEndInt;
  if(Size & USART_DEV_SIZE_AUTO)
    pDev->Flag |= USART_DEV_RCV_AUTO;
  pDev->RcvCount = Size & ~USART_DEV_SIZE_AUTO;
  pDev->RcvLen = 0;

  struct _USART_HW *pUsartHw = pDev->pUsartHw;
  //打开接收使能
  pUsartHw->UCSRB |= (1 << RXEN);
  //打开接收中断
  pUsartHw->UCSRB |= (1 << RXCIE);
  return 0;
}

//--------------------Usart接收数据强制停止---------------------------
//停止正在读过程中的数据或把接收置为停止状态
//返回已接收到的数据个数
UsartSize_t UsartDev_RcvStop(struct _UsartDev *pDev)
{
  struct _USART_HW *pUsartHw = pDev->pUsartHw;
  //停止接收，关闭接收中断
  pUsartHw->UCSRB &= ~((1 << RXEN) | (1 << RXCIE));
  //执行数据读取操作，来清除接收中断标志。
  unsigned char i = pUsartHw->UDR;
  pDev->Flag &= ~USART_DEV_RCV_BUSY;//最后置停止标志
  return pDev->RcvLen;
}

//--------------------Usar发送单个数据准备--------------------------------
//启动USART中接收数据,返回是否启动成功
//返回启动是否成功,0:成功,其它失败
//注:发送缓冲区大小最高位置1表示在自动模式
signed char UsartDev_SendRdy(struct _UsartDev *pDev,      //所带设备
                          unsigned char *pBuf,         //发送缓冲区
                          UsartSize_t Size,         //发送缓冲区大小
                          UsartDevInt_t SendRcvEndInt);//发送回调函数


//--------------------Usar发送数据启动函数--------------------------------
//启动USART中接收数据,返回是否启动成功
//返回启动是否成功,0:成功,其它失败
//注:发送缓冲区大小最高位置1表示在自动模式
signed char UsartDev_SendStart(struct _UsartDev *pDev,      //所带设备
                               unsigned char *pBuf,         //发送缓冲区
                               UsartSize_t Size,         //发送缓冲区大小
                               UsartDevInt_t SendEndInt) //发送回调函数
{
  //if(pDev->Flag & USART_DEV_SEND_BUSY) return -1;  //正在用发送呢！
  pDev->Flag |= USART_DEV_SEND_BUSY;
  pDev->Flag &= ~(USART_DEV_SEND_ERR | //相关发送标志初始化
                  USART_DEV_SEND_AUTO);
  pDev->pSendBuf = pBuf;
  pDev->SendEndInt = SendEndInt;
  if(Size & USART_DEV_SIZE_AUTO)//自动模式
    pDev->Flag |= USART_DEV_SEND_AUTO;
  pDev->SendCount = Size & ~USART_DEV_SIZE_AUTO;
  pDev->SenLen = 0;

  struct _USART_HW *pUsartHw = pDev->pUsartHw;
  //打开发送使能
  pUsartHw->UCSRA |= (1 << TXC);//清除发送标志
  pUsartHw->UCSRB |= (1 << TXEN);
  //预送一个数产生中断时送第二个数
  pUsartHw->UDR = *pBuf;//装载数据准备发送
  //最后使能空中断，开始发送
  pUsartHw->UCSRB |= (1 << UDRIE);
  return 0;
}

//--------------------Usart发送数据停止函数---------------------------
//停止正在写过程中的数据或把发送置为停止状态
//返回已发送到的数据个数
UsartSize_t UsartDev_SendStop(struct _UsartDev *pDev)
{
  struct _USART_HW *pUsartHw = pDev->pUsartHw;
  //停止发送//关发送中断
  pUsartHw->UCSRB &= ~((1 << TXEN) | (1 << TXCIE) | (1 << UDRIE));
  //清发送中断标志
  pUsartHw->UCSRA |= (1 << TXC) | (1 << UDRE);//清除发送标志
  pDev->Flag &= ~USART_DEV_SEND_BUSY;//最后置停止标志
  return pDev->SenLen;
}

//---------------------USART硬件接收中断处理函数----------------------------
void UsartDev_RcvIRQ(struct _UsartDev *pDev)
{
  struct _USART_HW *pUsartHw = pDev->pUsartHw;//临时指针
  unsigned char State = pUsartHw->UCSRA;
  unsigned char RcvData = pUsartHw->UDR;//无条件接收数据
  pDev->RcvData = RcvData;
  //判断帧错误，溢出错误,奇偶校验错误
  if(State & ((1 << FE) | (1 << DOR) | (1 << PE))){
    pDev->Flag |= USART_DEV_RCV_ERR;
    return;
  }

  UsartSize_t RcvLen = pDev->RcvLen;
  //缓冲区溢出检查,溢出时后面的再也不收了！
  if(RcvLen >= pDev->RcvCount){
    //收完数据强制完成
    pDev->Flag |= USART_DEV_RCV_ERR;
    pDev->RcvEndInt(pDev);
    UsartDev_RcvStop(pDev);
    return;
  }
  //收到缓冲区中
  *(pDev->pRcvBuf + RcvLen) = RcvData;
  pDev->RcvLen++;
  if(!(pDev->Flag & USART_DEV_RCV_AUTO)){
    if(pDev->RcvEndInt(pDev))//手工停止了
      UsartDev_RcvStop(pDev);
  }
}

//---------------------USART硬件发送中断处理函数----------------------------
void UsartDev_SendIRQ(struct _UsartDev *pDev)
{
  struct _USART_HW *pUsartHw = pDev->pUsartHw;//临时指针
  pDev->SenLen++;//已经发送一个了

  //未发送完成时
  if(pDev->SenLen < pDev->SendCount){
    if(!(pDev->Flag & USART_DEV_SEND_AUTO)){//中断模式时
      if(pDev->SendEndInt(pDev)){//手工停止了
        UsartDev_SendStop(pDev);
        return;
      }
    }
    pUsartHw->UDR = *(pDev->pSendBuf + pDev->SenLen);//发送下个数
  }
  //最后一个数被移出了,转换为数据发送中断完成
  //此功能也可用置
  else if(pDev->SenLen == pDev->SendCount){
    pUsartHw->UCSRB &= ~(1 << UDRIE);
    pUsartHw->UCSRB |= (1 << TXCIE);
  }
  else{//发送完成
    pDev->SendEndInt(pDev);//强制完成
    UsartDev_SendStop(pDev);
    return;
  }
}



