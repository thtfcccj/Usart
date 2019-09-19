/******************************************************************************

                   Usart设备抽像在PC平台qextseroal中的实现
//此模块支持接口提供的所有功能
******************************************************************************/

#include "UsartDev.h"
#include <string.h>

//------------------------接口函数声明---------------------------------------
//打开串口函数,返回打开是否成功
//此函数同时启动读数据
extern int Win_QextSerialPort_open(struct Win_QextSerialPort *pSerial);

//关闭串口函数
 extern void Win_QextSerialPort_close(struct Win_QextSerialPort *pSerial);

//读若干数据函数
//返回读数据个数或错误-1;
 extern int Win_QextSerialPort_readData(struct Win_QextSerialPort *pSerial,
                                              char *data, int maxSize);

//写若干数据函数
//返回写数据个数或错误-1;
 extern int Win_QextSerialPort_writeData(struct Win_QextSerialPort *pSerial,
                                              char *data, int maxSize);

//-------------------------初始化函数---------------------------------------
void UsartDev_Init(struct _UsartDev *pDev,
                   void * pUsartHw,  //挂接的硬件
                   void *pVoid)      //回调函数可能需要的指针
{
  memset(pDev, 0, sizeof(struct _UsartDev));
  pDev->pUsartHw = pUsartHw;
   pDev->pVoid = pVoid;
  //关闭串口
  //Win_QextSerialPort_close(pUsartHw);
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
  return 0;
}

//--------------------Usart接收数据强制停止---------------------------
//停止正在读过程中的数据或把接收置为停止状态
//返回已接收到的数据个数
unsigned short UsartDev_RcvStop(struct _UsartDev *pDev)
{
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
  int Resume;
  //if(pDev->Flag & USART_DEV_SEND_BUSY) return -1;  //正在用发送呢！
  pDev->Flag |= USART_DEV_SEND_BUSY;
  pDev->Flag &= ~(USART_DEV_SEND_ERR | //相关发送标志初始化
                  USART_DEV_SEND_AUTO);
  pDev->pSendBuf = pBuf;
  pDev->SendEndInt = SendEndInt;
  if(Size & 0x8000)//自动模式
    pDev->Flag |= USART_DEV_SEND_AUTO;
  pDev->SendCount = Size & ~0x8000;
  

  //开始发送,这里为阻塞方式运行
  Resume = Win_QextSerialPort_writeData(pDev->pUsartHw,pBuf,pDev->SendCount);
  //if(Resume < 0) return -1;//写错误
  pDev->SenLen = Resume;
  pDev->SendEndInt(pDev);   //通报完成

  return Resume;
}

//--------------------Usart发送数据停止函数---------------------------
//停止正在写过程中的数据或把发送置为停止状态
//返回已发送到的数据个数
unsigned short UsartDev_SendStop(struct _UsartDev *pDev)
{
  //停止前已结束,不需停止
  return pDev->SenLen;
}

//---------------------USART硬件接收中断处理函数----------------------------
void UsartDev_RcvIRQ(struct _UsartDev *pDev)
{
  int Resume;
  unsigned short RcvLen = pDev->RcvLen;
  unsigned short RcvCount;

  //不在接收过程中时，将收到的数据直接丢弃
  if(!(pDev->Flag & USART_DEV_RCV_BUSY)){
    unsigned char DumpBuf[16];
    int i = 0;
    for(; i < 100; i++){
        if(Win_QextSerialPort_readData(pDev->pUsartHw,DumpBuf,16) <= 0) break;
    };
    return;
  }


  //自动接收启动时收完全部缓冲区的数据
  //if(pDev->Flag & USART_DEV_RCV_AUTO)
    RcvCount = pDev->RcvCount - RcvLen;
  //else RcvCount = 1;//一个个收

  Resume = Win_QextSerialPort_readData(pDev->pUsartHw,
                                           pDev->pRcvBuf + RcvLen,
                                           RcvCount);
  if(Resume < 0){//异常处理
      pDev->Flag |=  USART_DEV_RCV_ERR;
      RcvCount = 0;
  }
  else{//接收总数
      RcvLen += Resume;
      RcvCount = Resume;
  }

  //缓冲区溢出检查,溢出时后面的再也不收了！
  if((RcvLen >= pDev->RcvCount) || (pDev->Flag & USART_DEV_RCV_ERR)){
    //收完数据强制完成
    pDev->Flag |= USART_DEV_RCV_ERR;
    if(pDev->RcvEndInt(pDev))//收完数据强制完成(可重启动)
        UsartDev_RcvStop(pDev);
    return;
  }

  //自动时
  if(pDev->Flag & USART_DEV_RCV_AUTO){
    pDev->RcvLen = RcvLen;
    if(pDev->RcvEndInt(pDev))//手工停止了
      UsartDev_RcvStop(pDev);
  }
  else{//手工收时模拟单个数据接收
    for( ; RcvCount > 0; RcvCount--){
      pDev->RcvLen++;
      if(pDev->RcvEndInt(pDev)){//手工停止了
        UsartDev_RcvStop(pDev);
      }
    }
  } //end for

}



