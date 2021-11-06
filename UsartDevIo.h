/******************************************************************************

                   Usart设备抽像通用接口-独立IO实现时
此接口用于实现用IO模拟UART通讯，同时还使用MCU内部UART硬件的情况时使用
//    此接口仅简单在UsartDevIo后增加IO后缀，以与其可并行使用，若系统中没有使用--
//UsartDevIo.h接口，则建议直接使用该接口以提供最大兼容性。
//此设备不负责对底层硬件的配置,需根据CPU类型不同单独实现或直接填充

//注：此模块中的主要函数必须与UsartDev.h中的相同(只是加了后缀IO)，以用于多态
******************************************************************************/
#ifndef __USART_DEV_IO_H
#define __USART_DEV_IO_H
#ifdef SUPPORT_EX_PREINCLUDE//不支持Preinlude時
  #include "Preinclude.h"
#endif

#include "UsartDev.h" //使用相同的数据结构

/******************************************************************************
                             相关函数
******************************************************************************/

//-------------------------初始化函数---------------------------------------
void UsartDevIo_Init(struct _UsartDev *pDev,
                   void * pUsartHw  //挂接的硬件
                   /*,void *pVoid*/);      //回调函数可能需要的指针
          
//-------------------------推存的重配置函数-------------------------------
//送入配置参数,同时返回实际上的波特率
//此函数将复位整个Usart的当前操作
//void UsartDevIo_ReCfg(struct _UsartDev *pDev,
//                   struct _UsartDevCfg *pCfg);

//---------------------Usart开始接收数据函数--------------------------------
//启动USART接收数据,返回是否启动成功
//返回启动是否成功,0:成功,其它失败
//注:接收缓冲区大小最高位置1表示在自动模式
signed char UsartDevIo_RcvStart(struct _UsartDev *pDev,  //所带设备
                              unsigned char *pBuf,     //接收缓冲区
                              unsigned short Size,     //接收缓冲区大小
                              UsartDevInt_t RcvEndInt);//接收回调函数

//--------------------Usart接收数据强制停止---------------------------
//停止正在读过程中的数据或把接收置为停止状态
//返回已接收到的数据个数
unsigned short UsartDevIo_RcvStop(struct _UsartDev *pDev);

//--------------------Usar发送数据启动函数--------------------------------
//启动USART中接收数据,返回是否启动成功
//返回启动是否成功,0:成功,其它失败
//注:发送缓冲区大小最高位置1表示在自动模式
signed char UsartDevIo_SendStart(struct _UsartDev *pDev,      //所带设备
                               const unsigned char *pBuf,   //发送缓冲区
                               unsigned short Size,         //发送缓冲区大小
                               UsartDevInt_t SendEndInt);    //发送回调函数

//--------------------Usart发送数据停止函数---------------------------
//停止正在写过程中的数据或把发送置为停止状态
//返回已发送到的数据个数
unsigned short UsartDevIo_SendStop(struct _UsartDev *pDev);

//--------------------得到已接收数据大小---------------------------
//unsigned short UsartDevIo_GetRcvLen(struct _UsartDev *pDev);
#define UsartDevIo_GetRcvLen(pdev) ((pdev)->RcvLen)

//--------------------得到已发送数据大小---------------------------
//unsigned short UsartDevIo_GetSendLen(struct _UsartDev *pDev);
#define UsartDevIo_GetSendLen(pdev) ((pdev)->SendLen)

//---------------------USART硬件接收中断处理函数--------------------
void UsartDevIo_RcvIRQ(struct _UsartDev *pDev);



#endif

