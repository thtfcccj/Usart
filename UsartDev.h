/******************************************************************************

                   Usart设备抽像通用接口
//此设备支持双向通讯
//此设备实现时,若为自动模式,则在带DMA及PDC控制器中将启用该功能,否则禁止
//此设备不负责对底层硬件的配置,需根据CPU类型不同单独实现或直接填充
//注:在接收中,若底层硬件支持DMA及PDC,则可在无数据时直接结束,否则将在数据区满时结束
******************************************************************************/
#ifndef __USART_DEV_H
#define __USART_DEV_H

/******************************************************************************
                             相关配置
******************************************************************************/


/******************************************************************************
                             相关结构
******************************************************************************/

//-----------------------结束中断处理函数定义--------------------------------
//形参为返struct _UsartDev指针
//回状态定义为:0:继续收发,其它:停止收发
typedef signed char (*UsartDevInt_t)(void*);

//---------------------------Usart设备定义-------------------------------------
//Spi设备表示一个物理Spi硬件接口，一个Spi硬件接口允许带多个Spi设备(以片选区分),
struct _UsartDev{
  void *pUsartHw;                      //挂接的硬件设备
  void *pVoid;                         //回调函数可能需要的指针
  //接收缓冲区
  unsigned char *pRcvBuf;              //接收缓冲区
  unsigned short RcvCount;             //接收缓冲区大小
  unsigned short RcvLen;               //已接收个数
  UsartDevInt_t  RcvEndInt;            //发送处理函数
  //发送缓冲区
  const unsigned char *pSendBuf;       //发送缓冲区
  unsigned short SendCount;            //发送缓冲区大小
  unsigned short SenLen;               //已发送个数
  UsartDevInt_t  SendEndInt;           //发送处理函数
  //其它:
  volatile unsigned char RcvData;      //缓冲最后一次接收的数据
	volatile unsigned char Flag;         //相关标志，见定义
};

//相关标志定义为：
#define USART_DEV_SEND_BUSY      0x01  //发送忙，表示正在发送过程中
#define USART_DEV_RCV_BUSY       0x02  //接收忙，表示正在接收过程中
#define USART_DEV_RCV_BUF_OV     0x04  //接收缓冲区满标志

#define USART_DEV_SEND_ERR       0x10  //发送过程中有错误
#define USART_DEV_RCV_ERR        0x20  //接收过程中有错误
#define USART_DEV_SEND_AUTO      0x40 //自动模式表示发送完成调用中断回调,否则单个数据发送即中断
#define USART_DEV_RCV_AUTO       0x80 //自动模式表示按收完成调用中断回调,否则单个数据接收即中断


/******************************************************************************
                             相关函数
******************************************************************************/

//-------------------------初始化函数---------------------------------------
void UsartDev_Init(struct _UsartDev *pDev,
                   void * pUsartHw  //挂接的硬件
                   /*,void *pVoid*/);      //回调函数可能需要的指针

//-------------------------推存的重配置函数-------------------------------
//送入配置参数,同时返回实际上的波特率
//此函数将复位整个Usart的当前操作
//void UsartDev_ReCfg(struct _UsartDev *pDev,
//                   struct _UsartDevCfg *pCfg);

//---------------------Usart开始接收数据函数--------------------------------
//启动USART接收数据,返回是否启动成功
//返回启动是否成功,0:成功,其它失败
//注:接收缓冲区大小最高位置1表示在自动模式
signed char UsartDev_RcvStart(struct _UsartDev *pDev,  //所带设备
                              unsigned char *pBuf,     //接收缓冲区
                              unsigned short Size,     //接收缓冲区大小
                              UsartDevInt_t RcvEndInt);//接收回调函数

//--------------------Usart接收数据强制停止---------------------------
//停止正在读过程中的数据或把接收置为停止状态
//返回已接收到的数据个数
unsigned short UsartDev_RcvStop(struct _UsartDev *pDev);

//--------------------Usar发送数据启动函数--------------------------------
//启动USART中接收数据,返回是否启动成功
//返回启动是否成功,0:成功,其它失败
//注:发送缓冲区大小最高位置1表示在自动模式
signed char UsartDev_SendStart(struct _UsartDev *pDev,      //所带设备
                               const unsigned char *pBuf,   //发送缓冲区
                               unsigned short Size,         //发送缓冲区大小
                               UsartDevInt_t SendEndInt);    //发送回调函数

//--------------------Usart发送数据停止函数---------------------------
//停止正在写过程中的数据或把发送置为停止状态
//返回已发送到的数据个数
unsigned short UsartDev_SendStop(struct _UsartDev *pDev);

//--------------------得到已接收数据大小---------------------------
//unsigned short UsartDev_GetRcvLen(struct _UsartDev *pDev);
#define UsartDev_GetRcvLen(pdev) ((pdev)->RcvLen)

//--------------------得到已发送数据大小---------------------------
//unsigned short UsartDev_GetSendLen(struct _UsartDev *pDev);
#define UsartDev_GetSendLen(pdev) ((pdev)->SendLen)

//---------------------USART硬件接收中断处理函数----------------------------
void UsartDev_RcvIRQ(struct _UsartDev *pDev);



#endif

