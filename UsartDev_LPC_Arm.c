/******************************************************************************

                   Usart设备抽像在LPC arm系列(arm7,m3,m0)中的实现
//此模块支持接口提供的所有功能
******************************************************************************/

#include "UsartDev.h"
#include <stm32f10x.h>   
//#include <string.h>
//#include "LPC12xx.h"//不同芯片需更改//需将LPC_UART_TypeDef定义为struct _USART_HW
//#include "LPC12xxbit.h"//不同芯片需更改
extern void UsartDev_IRQ(struct _UsartDev *pDev); //放入中断函数内

//-------------------------初始化函数---------------------------------------
void UsartDev_Init(struct _UsartDev *pDev,
                   void * pUsartHw//,挂接的硬件
                   /*void *pVoid*/) //回调函数可能需要的指针
{
	struct USART_TypeDef *pUartHw = (struct _USART_HW *)pUsartHw;
  memset(pDev, 0, sizeof(struct _UsartDev));
  pDev->pUsartHw = pUsartHw;
  //pDev->pVoid = pVoid;  //回调函数可能需要的指针
  //关闭串口,及中断使能
  //停止并复位相关资源
  pUartHw->CR1 = 1<<13;//禁止串口
  pUartHw->FCR = LPC_FIFO_EN | LPC_RXFIFO_RST | LPC_TXFIFO_RST;
  pUartHw->IER = 0;
}

//---------------------Usart开始接收数据函数--------------------------------
//启动USART接收数据,返回是否启动成功
//返回启动是否成功,0:成功,其它失败
//注:接收缓冲区大小最高位置1表示在自动模式
signed char UsartDev_RcvStart(struct _UsartDev *pDev,  //所带设备
                              unsigned char *pBuf,     //接收缓冲区
                              unsigned short Size,     //接收缓冲区大小
                              UsartDevInt_t RcvEndInt)//接收回调函数
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

  struct _USART_HW *pUartHw = pDev->pUsartHw;
  //复位并停止接收，置为接收状态,配置FIFO接收
  if(pDev->Flag & USART_DEV_RCV_AUTO)
    pUartHw->FCR |= LPC_FIFO_EN | LPC_RXFIFO_RST | LPC_RX_TRI_LEV8;
  else
    pUartHw->FCR |= LPC_FIFO_EN | LPC_RXFIFO_RST | LPC_RX_TRI_LEV1;
  //接收超时或接收缓冲区满均产生中断
  pUartHw->IER |= LPC_RBR_IE;
  //接收超时定时开始
  return 0;
}

//--------------------Usart接收数据强制停止---------------------------
//停止正在读过程中的数据或把接收置为停止状态
//返回已接收到的数据个数
unsigned short UsartDev_RcvStop(struct _UsartDev *pDev)
{
  struct _USART_HW *pUartHw = pDev->pUsartHw;
  //复位并停止接收
  pUartHw->FCR |= LPC_RXFIFO_RST;
  //接收中断禁止
  pUartHw->IER &= ~LPC_RBR_IE;
  pDev->Flag &= ~USART_DEV_RCV_BUSY;//最后置停止标志
  return pDev->RcvLen;
}

//--------------------Usar发送数据启动函数--------------------------------
//启动USART中接收数据,返回是否启动成功
//返回启动是否成功,0:成功,其它失败
//注:发送缓冲区大小最高位置1表示在自动模式
signed char UsartDev_SendStart(struct _UsartDev *pDev,      //所带设备
                               unsigned char *pBuf,         //发送缓冲区
                               unsigned short Size,         //发送大小
                               UsartDevInt_t SendEndInt) //发送回调函数
{
  //if(pDev->Flag & USART_DEV_SEND_BUSY) return -1;  //正在用发送呢！
  pDev->Flag |= USART_DEV_SEND_BUSY;
  pDev->Flag &= ~(USART_DEV_SEND_ERR | //相关发送标志初始化
                  USART_DEV_SEND_AUTO);
  pDev->pSendBuf = pBuf;
  pDev->SendEndInt = SendEndInt;
  if(Size & 0x8000){//自动模式
    pDev->Flag |= USART_DEV_SEND_AUTO;
    Size &= ~0x8000;
  }
  pDev->SendCount = Size;

  struct _USART_HW *pUartHw = pDev->pUsartHw;
  //复位并停止发送，置为发送状态
  pUartHw->FCR |= LPC_FIFO_EN | LPC_TXFIFO_RST;
  //发送完成产生中断
  pUartHw->IER |= LPC_THRE_IE;

  //发送时间间隔无法实现

  //写FIFO
  if(pDev->Flag & USART_DEV_SEND_AUTO){
    unsigned char CurSize;
    if(Size > 14) CurSize = 14; //首个数最多14个以保证中断即发完了
    else CurSize = Size;
    pDev->SendLen =  CurSize;//已发送个数
    //启动发送
    while(CurSize--) pUartHw->THR = *pBuf++;
  }
  else{ //写FIFO仅一字
    pUartHw->THR = *pBuf;
    pDev->SendLen = 1;
  }
  //启动发送
  pUartHw->TER = LPC_TXE;

  return 0;
}

//--------------------Usart发送数据停止函数---------------------------
//停止正在写过程中的数据或把发送置为停止状态
//返回已发送到的数据个数
unsigned short UsartDev_SendStop(struct _UsartDev *pDev)
{
  struct _USART_HW *pUartHw = pDev->pUsartHw;
  //复位并停止发送
  pUartHw->FCR |= LPC_TXFIFO_RST;
  //发送中断禁止
  pUartHw->IER &= ~LPC_THRE_IE;
  //PDC时已发送数据个数(无法判断具体发送个数)
  pDev->Flag &= ~USART_DEV_SEND_BUSY;//最后置停止标志
  return pDev->SendLen;
}

//---------------------USART硬件接收中断处理函数----------------------------
void UsartDev_IRQ(struct _UsartDev *pDev)
{
  struct _USART_HW *pUartHw = pDev->pUsartHw;//临时指针
  if(pUartHw == NULL) return; //未初始化完成

  unsigned char Ident = pUartHw->IIR & LPC_INTIDENT;
  switch(Ident){//检查识别码
  //=====================接收数据中断================
  case LPC_INT_RDA://FIFO满,直接收数据
  case LPC_INT_CTI:{//FIFO超时
    if(!(pDev->Flag & USART_DEV_RCV_BUSY)) break;//不在接收状态
    if(pDev->Flag & USART_DEV_RCV_BUF_OV) break; //缓冲区溢出
    //得到允许接收个数
    unsigned char EnLen = pDev->RcvCount - pDev->RcvLen;
    unsigned char CurRcvLen;
    if(pDev->Flag & USART_DEV_RCV_AUTO) CurRcvLen = 8;
    else  CurRcvLen = 1;
    if(CurRcvLen > EnLen) CurRcvLen = EnLen;
    unsigned char *pRcvPos = pDev->pRcvBuf + pDev->RcvLen;
    if(Ident == LPC_INT_RDA){//FIFO满
      pDev->RcvLen += CurRcvLen;
      while(CurRcvLen){ *pRcvPos++= pUartHw->RBR; CurRcvLen--;}
    }
    else{//收到一个数(如余7个数时,将中断7次)
      *pRcvPos= pUartHw->RBR;
      pDev->RcvLen++;
    }
    pDev->RcvData = *pRcvPos;//缓冲最后一次接收的数据
    //缓冲区溢出检查,溢出时后面的再也不收了！
    if(pDev->RcvLen >= pDev->RcvCount){
      UsartDev_RcvStop(pDev);//先结束
      pDev->Flag |= USART_DEV_RCV_BUF_OV; //缓冲区满结束
      pDev->RcvEndInt(pDev);//中断结束处理
    }

    #ifdef USART_DEV_SUPPORT_RCV_OV_STOP
    //单个数据接收模式,正自动模式接收超时时中止
    else if(!(pDev->Flag & USART_DEV_RCV_AUTO) || (Ident ==LPC_INT_CTI)){
    #else//单个数据接收模式,自动模式接收超时时可认为由数据不连续引起,即允许间隔
    else if(!(pDev->Flag & USART_DEV_RCV_AUTO)){
    #endif
      if(pDev->RcvEndInt(pDev))//手工停止了
        UsartDev_RcvStop(pDev);
    }
    break;
  }
   //======================发送数据中断===================
  case LPC_INT_THER:{
    if(!(pDev->Flag & USART_DEV_SEND_BUSY)) break;//不在发送状态
    //注:SendLen已在发送前更新了,发送完成不面更新
    unsigned char EnLen = pDev->SendCount - pDev->SendLen;
    if(!EnLen){//数据发送完成(最后一组数据软件保证全部发送完成了)
      UsartDev_SendStop(pDev);//强制完成
      pDev->SendEndInt(pDev);
      break;
    }
    else if(!(pDev->Flag & USART_DEV_SEND_AUTO)){//单次模式时发送前处理
      if(pDev->SendEndInt(pDev)){//手工停止了
        UsartDev_SendStop(pDev);
        break;
      }
    }

    //继续发送未完成的数据
    unsigned char CurSendLen;
    if(pDev->Flag & USART_DEV_SEND_AUTO){
      CurSendLen = 14; //14个时产生的中断,最后2个数可能还在缓冲区里
      if(CurSendLen > EnLen){//最后一次能发送完
        if(EnLen > 12) CurSendLen = 12;//在13,14个时,为保证最后一组数发完中断,少发
        else CurSendLen = EnLen;
      }
    }
    else CurSendLen = 1;//单次发送模式一次一个

    //送FIFO数据，并发出
    unsigned char *pSendPos = pDev->pSendBuf + pDev->SendLen;
    pDev->SendLen += CurSendLen; //发送前计数
    while(CurSendLen--) pUartHw->THR = *pSendPos++;


    break;
  }
  case LPC_INT_RLS:	 //Rx 线状态/错误 OE, PE, FE, 或BI,最高级,忽略
  default:break;
  }
}