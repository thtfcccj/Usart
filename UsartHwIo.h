/******************************************************************************

                Usart设备硬件底层实现
1. 此模块模模拟MCU内部的USART硬件结构实现通讯
2. 支持多例化，但只支持半双工，每个实例需1个硬件定器支持及Rx中断支持
3. 因UART为异步通讯，建议波特率在4800及以下且MCU足够快
   且定时器中断设为中断嵌套方式，最高优化级。
此实现独立于硬件
******************************************************************************/
#ifndef __USART_HW_IO_H
#define __USART_HW_IO_H

/******************************************************************************
                             相关结构 
******************************************************************************/

struct _UsartHwIo{
  //public: 外部可直接访问变量
  volatile unsigned char SCON;   //控制位，位定义见定义
  volatile unsigned char ISR;    //状态寄存器，位定义见定义 
  unsigned char SBUF;            //接收完成，或待发送的数据  
  //private: 内部变量
  unsigned char UartCfg;        //同UsartDevCfg->Cfg低8位
  unsigned short BitData;        //收到，或正在发送的数据
  //接收FIFO缓冲
  unsigned short CurBitData;    //正在接收的数据
  volatile unsigned char BitPos;  //正在收发的位
                         //2~9为数据，10及以后由配置决定
  //硬件拥有相关:
  unsigned char Id;     //此模拟的ID号,用于TX，Rx引脚 
  void *pHwTimer;       //拥有的硬件定时器
};

//控制位SCON位定义为:
#define USART_HW_IO_SCON_TX_SPACE   0x40 //发送数据间隔(内部使用)
#define USART_HW_IO_SCON_RX_WAIT    0x20 //接收数据时等待启动标志(内部使用)

#define USART_HW_IO_SCON_RE         0x10 //接收使能
#define USART_HW_IO_SCON_RCIE       0x01 //使能接收完成中断
#define USART_HW_IO_SCON_TCIE       0x02 //使能发送完成中断

//状态寄存器ISR位定义为:
#define USART_HW_IO_ISR_TR_FINAL    0xC0 //收发完成待处理,具体为：
#define USART_HW_IO_ISR_TX_FINAL    0x80 //发送完成待处理
#define USART_HW_IO_ISR_RX_FINAL    0x40 //接收完成待处理

#define USART_HW_IO_ISR_ESTART      0x20 //起始帧错误
#define USART_HW_IO_ISR_EPAR        0x10 //寄偶校验错误
#define USART_HW_IO_ISR_ESTOP       0x08 //停止位错误

#define USART_HW_IO_ISR_FE          0x04 //接收帧错误标志位
#define USART_HW_IO_ISR_TI          0x02 //发送完成中断
#define USART_HW_IO_ISR_RI          0x01 //接收完成中断

/*********************************************************************
                        系统调用相关函数
*********************************************************************/
//--------------------------初始化-----------------------------
//调用此函数前确保Tx,Rx初始化完成
void UsartHwIo_Init(struct _UsartHwIo *pHwIo,
                    unsigned char Id,//此模拟的ID号
                    void *pHwTimer); //挂接的硬件定时器

//-------------------硬件定时器中断处理函数--------------------
//放入对应的硬件定时器中断中,注意调用前应清除中断
void UsartHwIo_TimerIRQ(struct _UsartHwIo *pHwIo);

//---------------Rx引脚收到起始沿中断处理函数--------------------
//放入Rx引脚中断中,注意调用前应清除中断
void UsartHwIo_RxIRQ(struct _UsartHwIo *pHwIo);

//-------------------------快速任务函数-----------------------
//放入系统快速任务中查询
void UsartHwIo_FastTask(struct _UsartHwIo *pHwIo);

/*********************************************************************
                      UART硬件配置与收发相关函数
*********************************************************************/

//------------------------------启动发送-----------------------------
//写完控制位SCON与SBUF后，调用此函数启动发送
void UsartHwIo_SendStart(struct _UsartHwIo *pHwIo);

//-----------------------------停止发送------------------------------
//调用强制停止发送数据
void UsartHwIo_SendStop(struct _UsartHwIo *pHwIo);

//------------------------------启动接收-----------------------------
//写完控制位SCON后，调用此函数启动接收
void UsartHwIo_RcvStart(struct _UsartHwIo *pHwIo);

//-----------------------------停止接收------------------------------
//调用强制停止接收数据
void UsartHwIo_RcvStop(struct _UsartHwIo *pHwIo);

//---------------------------标准配置实现-------------------------
#include "UsartDevCfg.h"
//只管输入串口的配置字，不参与串口的使能和其他寄存器操作
void UsartHwIo_Cfg(const struct _UsartDevCfg *pCfg,//串口配置结构体
                   void * pUsartHw,           //硬件设备指针
                   unsigned long Clk);        //当前串口使用的时钟

/*********************************************************************
                    硬件层回调函数
*********************************************************************/            
#include "IoCtrl.h" //部分驱动可直接实现

//----------------------硬件中断回调函数------------------------
//此函数在收发完一个数据后调用
void UsartHwIo_cbUsartIRQ(struct _UsartHwIo *pHwIo);

//-------------------------定时器操作----------------------------
//通过波特率配置定时器,需实现为定时器预置为对应波特率定时器溢出
void UsartHwIo_cbCfgTimer(void *pHwTimer, unsigned long Buad);  
          
//预启动定时器，主要为接收到起始沿时，定时器加快1倍以在中间采样。
void UsartHwIo_cbTimerStartRdy(void *pHwTimer);                        
//正常启动定时器
void UsartHwIo_cbTimerStart(void *pHwTimer);
//正常启动定时器并发送间隔帧(两帧数据间的间隔)
void UsartHwIo_cbTimerStartSpace(void *pHwTimer);
//->程序延时发送后也够了：
//#define UsartHwIo_cbTimerStartSpace(hwTimer) UsartHwIo_cbTimerStart(hwTimer)

//停止定时器,建议停止同时清一下此定时器中断
#ifndef UsartHwIo_cbTimerStop                
  void UsartHwIo_cbTimerStop(void *pHwTimer); 
#endif

//-------------------------Rx中断操作----------------------------
#ifndef UsartHwIo_cbRxStart
  void UsartHwIo_cbRxStart(unsigned char Id); //启动并清Rx接收中断
#endif
#ifndef UsartHwIo_cbRxStop
  void UsartHwIo_cbRxStop(unsigned char Id); //停止并清Rx接收中断
#endif

//----------------------Tx，Rx引脚操作----------------------------
#ifndef UsartHwIo_cbIsRx
  unsigned short UsartHwIo_cbIsRx(unsigned char Id); //Rx是否为高电平
#endif
#ifndef UsartHwIo_cbSetTx
  void UsartHwIo_cbSetTx(unsigned char Id); //Tx置为高电平
#endif  
#ifndef UsartHwIo_cbClrTx
  void UsartHwIo_cbClrTx(unsigned char Id); //Tx置为低电平
#endif 

#endif

