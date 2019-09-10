/******************************************************************************

                   Usart设备抽像在Stm32中的实现
//此模块支持接口提供的所有功能
******************************************************************************/

#include "UsartDev.h"
#include <string.h>

#include "CMSIS.h"

//-------------------------初始化函数---------------------------------------
void UsartDev_Init(struct _UsartDev *pDev,
                   void * pUsartHw)  //挂接的硬件
{
  memset(pDev, 0, sizeof(struct _UsartDev));
  pDev->pUsartHw = pUsartHw;
  USART_TypeDef *pHw = (USART_TypeDef *)pUsartHw;
  //关闭串口,及中断使能
  pHw->CR1 &= ~(USART_CR1_RE | USART_CR1_RXNEIE | USART_CR1_TXEIE);
  pHw->CR1 |= USART_CR1_UE;//打开串口 
	pHw->SR  = 0;//清除状态寄存器值	
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

  USART_TypeDef *pUsartHw = pDev->pUsartHw;
  //打开接收使能
  pUsartHw->CR1 |= USART_CR1_RE;
  //打开接收中断
  pUsartHw->CR1 |= USART_CR1_RXNEIE;
  return 0;
}

//--------------------Usart接收数据强制停止---------------------------
//停止正在读过程中的数据或把接收置为停止状态
//返回已接收到的数据个数
unsigned short UsartDev_RcvStop(struct _UsartDev *pDev)
{
  USART_TypeDef *pUsartHw = pDev->pUsartHw;
  //停止接收，关闭接收中断
  pUsartHw->CR1 &= ~(USART_CR1_RE | USART_CR1_RXNEIE);
  //执清除接收中断标志。

  pDev->Flag &= ~USART_DEV_RCV_BUSY;//最后置停止标志
  return pDev->RcvLen;
}

//--------------------Usar发送数据启动函数--------------------------------
//启动USART中接收数据,返回是否启动成功
//返回启动是否成功,0:成功,其它失败
//注:发送缓冲区大小最高位置1表示在自动模式
signed char UsartDev_SendStart(struct _UsartDev *pDev,      //所带设备
                               unsigned char *pBuf,         //发送缓冲区
                               unsigned short Size,         //发送缓冲区大小
                               UsartDevInt_t SendEndInt) //发送回调函数
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
  //打开发送使能
  pUsartHw->CR1 |= USART_CR1_TE;
  //先清中断
  //pUsartHw->SR &= ~(USART_SR_TC | USART_SR_TXE);
  //发送完成一个数产生中断
  pUsartHw->DR = *pBuf;//装载数据准备发送
  pUsartHw->CR1 |= USART_CR1_TXEIE;//发送完成中断使能
  
  return 0;
}

//--------------------Usart发送数据停止函数---------------------------
//停止正在写过程中的数据或把发送置为停止状态
//返回已发送到的数据个数
unsigned short UsartDev_SendStop(struct _UsartDev *pDev)
{
  USART_TypeDef *pUsartHw = pDev->pUsartHw;
  //停止发送//关发送中断,注TE不能清,因关了再开，会先发一个前导字符
  pUsartHw->CR1 &= ~(/*USART_CR1_TE | */USART_CR1_TCIE | USART_CR1_TXEIE);
  //清发送中断标志
  pUsartHw->SR &= ~USART_SR_TC;//清除发送完成中断标志
  pDev->Flag &= ~USART_DEV_SEND_BUSY;//最后置停止标志
  return pDev->SenLen;
}

//---------------------USART硬件接收中断处理函数----------------------------
void UsartDev_RcvIRQ(struct _UsartDev *pDev)
{
  USART_TypeDef *pUsartHw = pDev->pUsartHw;//临时指针
   unsigned char RcvData;
	 unsigned short RcvLen;
  
	//接收中断部分
	if((pUsartHw->CR1 & USART_CR1_RXNEIE) && 
     (pUsartHw->SR & (USART_SR_RXNE | USART_SR_ORE))){//过载时不会置位RXNE位
		RcvData = pUsartHw->DR;//无条件接收数据
    pUsartHw->SR &= ~(USART_SR_RXNE | USART_SR_ORE);//去除ORE标志  
		pDev->RcvData = RcvData;
	   //判断帧错误，上溢错误,奇偶校验错误
		if(pUsartHw->SR & (USART_SR_PE/* | USART_SR_FE | USART_SR_NE*/)){
			pDev->Flag |= USART_DEV_RCV_ERR;
			return;
		}
		RcvLen = pDev->RcvLen;
		//缓冲区溢出检查,溢出时后面的再也不收了！
		if(RcvLen >= pDev->RcvCount){
			//收完数据强制完成
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
 
	//发送未完成
  if((pUsartHw->CR1 & USART_CR1_TXEIE) && (pUsartHw->SR & USART_SR_TXE)){
    pDev->SenLen++; //已写入缓冲区一个数了
    if(pDev->SenLen < pDev->SendCount){
      if(!(pDev->Flag & USART_DEV_SEND_AUTO)){//发送完每个数通报
        if(pDev->SendEndInt(pDev)){//用户让结束了
          UsartDev_SendStop(pDev);
          return;
        }
      }
      pUsartHw->DR = pDev->pSendBuf[pDev->SenLen];//继续发送数
    }
    //最后一个数移入，但没发出,切换为完成中断
    else{
      if(pUsartHw->SR & USART_SR_TC){//已经完成了
        UsartDev_SendStop(pDev);//关闭发送完成中断
        pDev->SendEndInt(pDev);//发送完成回调处理
      }
      else{
        pUsartHw->CR1 &= ~USART_CR1_TXEIE;
        pUsartHw->CR1 |= USART_CR1_TCIE;
      }
    }
	}
  //最后一位发送完成,关闭发送相关中断
  else if((pUsartHw->CR1 & USART_CR1_TCIE) && (pUsartHw->SR & USART_SR_TC)){
    UsartDev_SendStop(pDev);//关闭发送完成中断
    pDev->SendEndInt(pDev);//发送完成回调处理
  }
}



