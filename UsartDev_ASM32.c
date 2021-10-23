/******************************************************************************

                   Usart设备抽像在ASM32中的实现
//此模块由HC32Fork出，模块结构基本一致
******************************************************************************/

#include "UsartDev.h"
#include <string.h>

#include "CMSIS.h"

//---------------------------内部定义-----------------------------------------
#define USART_TypeDef  UART_TypeDef

#define USART_SCON_RE         0x10 //接收使能
#define USART_SCON_RCIE       0x01 //使能接收完成中断
#define USART_SCON_TCIE       0x02 //使能发送完成中断

#define USART_ISR_FE          0x04 //接收帧错误标志位
#define USART_ISR_TI          0x02 //发送完成中断
#define USART_ISR_RI          0x01 //接收完成中断

//#ifdef SUPPORT_USART_TX_BUF  //支持发送缓冲时(低端不支持)
//  #define USART_SCON_TXEIE     0x100   //带发送缓冲时定义
//#else
//  #define USART_SCON_TXEIE     0   //无定义
//#endif


//-------------------------初始化函数---------------------------------------
void UsartDev_Init(struct _UsartDev *pDev,
                   void * pUsartHw)  //挂接的硬件
{
  memset(pDev, 0, sizeof(struct _UsartDev));
  pDev->pUsartHw = pUsartHw;
  USART_TypeDef *pHw = (USART_TypeDef *)pUsartHw;
  //关闭串口,及中断使能
  pHw->SCON &= ~(USART_SCON_RE | USART_SCON_RCIE | USART_SCON_TCIE);
  //pHw->SCON |= USART_SCON_UE;//打开串口 
	pHw->ISR  = 0;//清除状态寄存器值	
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
  pUsartHw->SCON |= USART_SCON_RE;
  //打开接收中断
  pUsartHw->SCON |= USART_SCON_RCIE;
  return 0;
}

//--------------------Usart接收数据强制停止---------------------------
//停止正在读过程中的数据或把接收置为停止状态
//返回已接收到的数据个数
unsigned short UsartDev_RcvStop(struct _UsartDev *pDev)
{
  USART_TypeDef *pUsartHw = pDev->pUsartHw;
  //停止接收，关闭接收中断
  pUsartHw->SCON &= ~(USART_SCON_RE | USART_SCON_RCIE);
  //执清除接收中断标志。

  pDev->Flag &= ~USART_DEV_RCV_BUSY;//最后置停止标志
  return pDev->RcvLen;
}

//--------------------Usar发送数据启动函数--------------------------------
//启动USART中接收数据,返回是否启动成功
//返回启动是否成功,0:成功,其它失败
//注:发送缓冲区大小最高位置1表示在自动模式
signed char UsartDev_SendStart(struct _UsartDev *pDev,      //所带设备
                               const unsigned char *pBuf,         //发送缓冲区
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
  //pUsartHw->SCON |= USART_SCON_TE;
  //先清中断
  //pUsartHw->ISR &= ~(USART_ISR_TI | USART_ISR_TI);
  //发送完成一个数产生中断
  pUsartHw->SBUF = *pBuf;//装载数据准备发送
  pUsartHw->SCON |= USART_SCON_TCIE;//发送完成中断使能
  
  return 0;
}

//--------------------Usart发送数据停止函数---------------------------
//停止正在写过程中的数据或把发送置为停止状态
//返回已发送到的数据个数
unsigned short UsartDev_SendStop(struct _UsartDev *pDev)
{
  USART_TypeDef *pUsartHw = pDev->pUsartHw;
  //停止发送//关发送中断
  pUsartHw->SCON &= ~USART_SCON_TCIE;
  //清发送中断标志
  pUsartHw->ISR &= ~USART_ISR_TI;//清除发送完成中断标志
  pDev->Flag &= ~USART_DEV_SEND_BUSY;//最后置停止标志
  return pDev->SenLen;
}

//---------------------USART硬件接收中断处理函数----------------------------
void UsartDev_RcvIRQ(struct _UsartDev *pDev)
{
  USART_TypeDef *pUsartHw = pDev->pUsartHw;//临时指针
   unsigned char RcvData;
	 unsigned short RcvLen;
  
	//接收完成中断
	if((pUsartHw->SCON & USART_SCON_RCIE) && (pUsartHw->ISR & USART_ISR_RI)){
		RcvData = pUsartHw->SBUF;//无条件接收数据
    pUsartHw->ICR |= USART_ISR_RI;
		pDev->RcvData = RcvData;
	   //判断帧错误
		if(pUsartHw->ISR & USART_ISR_FE){
      pUsartHw->ICR |= USART_ISR_FE;
			pDev->Flag |= USART_DEV_RCV_ERR;
			return;
		}
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
 
	//发送完成中断
  if((pUsartHw->SCON & USART_SCON_TCIE) && (pUsartHw->ISR & USART_ISR_TI)){
    pUsartHw->ICR |= USART_ISR_TI;
    pDev->SenLen++; //已写入缓冲区一个数了
    if(pDev->SenLen < pDev->SendCount){
      if(!(pDev->Flag & USART_DEV_SEND_AUTO)){//发送完每个数通报
        if(pDev->SendEndInt(pDev)){//用户让结束了
          UsartDev_SendStop(pDev);
          return;
        }
      }
      pUsartHw->SBUF = pDev->pSendBuf[pDev->SenLen];//继续发送数
    }
    //最后一个数移入，但没发出,切换为完成中断
    else{
      UsartDev_SendStop(pDev);//关闭发送完成中断
      pDev->SendEndInt(pDev);//发送完成回调处理
      pUsartHw->SCON &= ~USART_SCON_TCIE;
    }
	}
}



