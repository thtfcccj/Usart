/******************************************************************************

                   Usart设备抽像在HC32中的实现
//此模块支持接口提供的所有功能
******************************************************************************/

#include "UsartDev.h"
#include <string.h>

#include "CMSIS.h"


//---------------------------内部定义-----------------------------------------
#define USART_TypeDef  M4_USART_TypeDef

//CR1中使用的位:
#define USART_CR1_CORE    (1 <<19)//b19 ORE标志清零位
#define USART_CR1_CFE     (1 <<17)//b17 FE标志清零位 FE标志清零位
#define USART_CR1_CPE     (1 <<16)//b16 PE标志清零位
#define USART_CR1_CERR    (USART_CR1_CORE | USART_CR1_CFE | USART_CR1_CPE)//所有故障位

#define USART_CR1_TXEIE   (1 << 7)//b7 发送数据寄存器空中断使能位
#define USART_CR1_TCIE    (1 << 6)//b6 发送完成中断使 能位
#define USART_CR1_RIE     (1 << 5)//b5 接收中断使能位
#define USART_CR1_TE      (1 << 3)//b3 发送器使能位
#define USART_CR1_RE      (1 << 2)//b2 接收器使能位 接收器使能位
  

//SR中使用的位:
#define USART_SR_TXE   (1 << 7)//b7 发送数据寄存器空
#define USART_SR_TC    (1 << 6)//b6 发送完成标志
#define USART_SR_RXNE  (1 << 5)//b5 接收数据寄存器不为空

#define USART_SR_ORE  (1 << 3)//b3 接收数据寄存器未被读取的情况下，又接收到一帧新的数据

#define USART_SR_FE    (1 << 1)//b1 接收帧错误
#define USART_SR_PE    (1 << 1)//b0 接收数据校验错误标志

//-------------------------初始化函数---------------------------------------
void UsartDev_Init(struct _UsartDev *pDev,
                   void * pUsartHw)  //挂接的硬件
{
  memset(pDev, 0, sizeof(struct _UsartDev));
  pDev->pUsartHw = pUsartHw;
  USART_TypeDef *pHw = (USART_TypeDef *)pUsartHw;
  //关闭串口,及中断使能
  pHw->CR1 &= ~(USART_CR1_RE | USART_CR1_TE | 
                USART_CR1_RIE | USART_CR1_TCIE | USART_CR1_TXEIE);
  pHw->CR1 |=  USART_CR1_CFE | USART_CR1_CPE;//清除故障状态值	
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
  pUsartHw->CR1 |= USART_CR1_RIE;
  return 0;
}

//--------------------Usart接收数据强制停止---------------------------
//停止正在读过程中的数据或把接收置为停止状态
//返回已接收到的数据个数
unsigned short UsartDev_RcvStop(struct _UsartDev *pDev)
{
  USART_TypeDef *pUsartHw = pDev->pUsartHw;
  //停止接收，关闭接收中断
  pUsartHw->CR1 &= ~(USART_CR1_RE | USART_CR1_RIE);
  //清除故障状态值	
  pUsartHw->CR1 |=  USART_CR1_CFE | USART_CR1_CPE | USART_CR1_CORE;
  
  if(pUsartHw->SR & USART_SR_RXNE){//读数清接收中断
    volatile unsigned char dump = pUsartHw->DR;
  }

  pDev->Flag &= ~USART_DEV_RCV_BUSY;//最后置停止标志
  return pDev->RcvLen;
}

//--------------------Usar发送数据启动函数--------------------------------
//启动USART中接收数据,返回是否启动成功
//返回启动是否成功,0:成功,其它失败
//注:发送缓冲区大小最高位置1表示在自动模式
signed char UsartDev_SendStart(struct _UsartDev *pDev,      //所带设备
                               const unsigned char *pBuf,   //发送缓冲区
                               unsigned short Size,         //发送缓冲区大小
                               UsartDevInt_t SendEndInt)    //发送回调函数
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
  pUsartHw->DR = *pBuf;//装载数据准备发送
  //发送完成中断使能
  if(pDev->SendCount > 1)  pUsartHw->CR1 |= USART_CR1_TXEIE;
  else pUsartHw->CR1 |= USART_CR1_TCIE;//一个数直接完成
  return 0;
}

//--------------------Usart发送数据停止函数---------------------------
//停止正在写过程中的数据或把发送置为停止状态
//返回已发送到的数据个数
unsigned short UsartDev_SendStop(struct _UsartDev *pDev)
{
  USART_TypeDef *pUsartHw = pDev->pUsartHw;
  //停止发送,关发送中断
  pUsartHw->CR1 &= ~(USART_CR1_TCIE | USART_CR1_TXEIE | USART_CR1_TE);

  //pUsartHw->CR1 |=  USART_CR1_CORE;//清故障值
  //if(pUsartHw->SR & USART_SR_TXE)//写数清发送中断
  //  pUsartHw->DR = 0;
  
  pDev->Flag &= ~USART_DEV_SEND_BUSY;//最后置停止标志
  return pDev->SenLen;
}

//---------------------USART硬件接收中断处理函数----------------------------
void UsartDev_RcvIRQ(struct _UsartDev *pDev)
{
   USART_TypeDef *pUsartHw = pDev->pUsartHw;//临时指针
   unsigned char RcvData;
	 unsigned short RcvLen;
  
   //接收数据帧错误，或接收数据有丢失时
   if(pUsartHw->SR & (USART_SR_ORE | USART_SR_FE | USART_SR_PE)){
     //ORE=1后不能继续接收数据，时钟同步模式下也不能发送数据
     //FE=1时收到的数据会保留但是RI中断不会发生，FE=1后不能继续接收数据
     //PE=1时收到的数据会保留但是RI中断不会发生，PE=1后不能继续接收数据
     pUsartHw->CR1 |= USART_CR1_CORE | USART_CR1_CFE | USART_CR1_CPE;//强制清除
     pDev->Flag |= USART_DEV_RCV_ERR; //置接收错误
   }
   
	//接收完成中断
	if((pUsartHw->CR1 & USART_CR1_RIE) && (pUsartHw->SR & USART_SR_RXNE)){
		RcvData = pUsartHw->DR >> 16;//无条件接收数据,自动清接收空中断
		pDev->RcvData = RcvData;
		RcvLen = pDev->RcvLen;
		//缓冲区溢出检查,溢出时后面的再也不收了！
		if(RcvLen >= pDev->RcvCount){
      if(pDev->RcvEndInt(pDev))//收完数据强制完成(可重启动)
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
 
	//发送过程及全部完成中断
  else if((pUsartHw->CR1 & USART_CR1_TXEIE) && (pUsartHw->SR & USART_SR_TXE) ||
     (pUsartHw->CR1 & USART_CR1_TCIE) && (pUsartHw->SR & USART_SR_TC)){
    pDev->SenLen++; //已写入缓冲区一个数了
    if(pDev->SenLen < pDev->SendCount){
      if(!(pDev->Flag & USART_DEV_SEND_AUTO)){//发送完每个数通报
        if(pDev->SendEndInt(pDev)){//用户让结束了
          UsartDev_SendStop(pDev);
          return;
        }
      }
      pUsartHw->DR = pDev->pSendBuf[pDev->SenLen];//继续发送数并清中断
      //最后一个数移入了，切换中断类型
      if((pDev->SenLen + 1) >= pDev->SendCount){
        pUsartHw->CR1 &= ~USART_CR1_TXEIE;
        pUsartHw->CR1 |= USART_CR1_TCIE;       
      }
    }
    //最后一个数完成了
    else{
      UsartDev_SendStop(pDev);//关闭发送完成中断
      pDev->SendEndInt(pDev);//发送完成回调处理
    }
	}
  else{//工作异常了
    pDev->Flag |= USART_DEV_SEND_ERR | USART_DEV_RCV_ERR;
    UsartDev_RcvStop(pDev);
    UsartDev_SendStop(pDev);
  }
}



