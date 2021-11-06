/******************************************************************************

                Usart设备硬件底层实现

******************************************************************************/
#include "UsartHwIo.h"
#include <string.h>

/*********************************************************************
                        系统调用相关函数实现
*********************************************************************/

//---------------由配置得到收发位函数--------------------
//表, 0b校验允许, 1b两停止位 ，2b7个数据位
static const unsigned char _Cfg2MaxLen[] = {
   10, //0,1起+8数+0校+1停
   11, //1,1起+8数+1校+1停 
   11, //2,1起+8数+0校+2停
   12, //3,1起+8数+1校+2停
   9,  //4,1起+7数+0校+1停 
   10, //5,1起+7数+1校+1停 
   10, //6,1起+7数+0校+2停 
   11, //7,1起+7数+1校+2停   
};
#define _GetMaxLen(hw) _Cfg2MaxLen[((hw)->UartCfg & 0x0f) >> 1]

//-----------------------得到校验位函数-----------------------------
//返回数据中1的个数
static unsigned char _GetPar(unsigned char SBUF)
{
  unsigned char BitHiCount = 0;
  for(unsigned char Mask = 0x01; Mask > 0; Mask <<= 1){
    if(SBUF & Mask) BitHiCount++;
  }
  return BitHiCount;
}

//-----------------------接收完成处理函数-----------------------------
static void _RcvPro(struct _UsartHwIo *pHwIo)
{
  unsigned short Data = pHwIo->BitData;
  if(Data & 0x01){//起始错误
    pHwIo->ISR = USART_HW_IO_ISR_FE | USART_HW_IO_ISR_ESTART;
    return;
  }
  Data >>= 1;//数据了
  unsigned char SBUF = Data; //预取数据
  if(pHwIo->UartCfg & USART_DEV_CFG_7_BIT){
    SBUF &= ~0x80;//去最高位
    Data >>= 7; //数据以后了
  }
  else Data >>= 8; //数据以后了
  pHwIo->SBUF = SBUF; //读取的数据
  //校验位
  if(pHwIo->UartCfg & USART_DEV_CFG_PAR_EN){
    //寄偶校验
    unsigned char Par = ((_GetPar(SBUF) & 0x01) ^ (Data & 0x01));
    if(pHwIo->UartCfg & USART_DEV_CFG_ODD){//奇校验
      if(Par) pHwIo->ISR = USART_HW_IO_ISR_FE | USART_HW_IO_ISR_EPAR;
    }
    else{//偶校验：当实际数据中“1”的个数为偶数的时候，这个校验位就是“0”
      if(!Par) pHwIo->ISR = USART_HW_IO_ISR_FE | USART_HW_IO_ISR_EPAR;
    }
    Data >>= 1;//寄偶校验后了
  }
  //双停止位
  if(pHwIo->UartCfg & USART_DEV_CFG_2_STOP){
    if(!(Data & 0x01)) //停止位应为高电平
      pHwIo->ISR = USART_HW_IO_ISR_FE | USART_HW_IO_ISR_ESTOP;
    Data >>= 1;//到最后一停止位了    
  }
  if(!(Data & 0x01)) //停止位应为高电平
    pHwIo->ISR = USART_HW_IO_ISR_FE | USART_HW_IO_ISR_ESTOP;   
}

//-----------------------得到发送位函数-----------------------------
static unsigned short  _GetSendBitData(const struct _UsartHwIo *pHwIo)
{
  unsigned short Data = (unsigned short)pHwIo->SBUF << 1;//停止位为0
  unsigned short Mask;
  if(pHwIo->UartCfg & USART_DEV_CFG_7_BIT){
    Data &= ~0x100;//取消最高位
    Mask = (1 << 8);
  }
  else Mask = (1 << 9);
  //校验位
  if(pHwIo->UartCfg & USART_DEV_CFG_PAR_EN){
    unsigned char Par = _GetPar(pHwIo->SBUF) & 0x01;
    if(pHwIo->UartCfg & USART_DEV_CFG_ODD){//奇校验
      if(!Par) Data |= Mask;
    }
    else{//偶校验
      if(Par) Data |= Mask;
    }
    Mask <<= 1; //至停止位了
  }
  //1个或两个停止位及以上全部为1;
  Data |= ~(Mask - 1);
  return Data;
}

//--------------------------初始化-----------------------------
//调用此函数前确保Tx,Rx初始化完成
void UsartHwIo_Init(struct _UsartHwIo *pHwIo,
                    unsigned char Id,//此模拟的ID号
                    void *pHwTimer) //挂接的硬件定时器
{
  memset(pHwIo, 0, sizeof(struct _UsartHwIo));
  pHwIo->Id = Id;
  pHwIo->pHwTimer = pHwTimer;  
}

//-------------------硬件定时器中断处理函数--------------------
//放入对应的硬件定时器中断中
void UsartHwIo_TimerIRQ(struct _UsartHwIo *pHwIo)
{
  //===================接收时处理=======================
  if(pHwIo->SCON & USART_HW_IO_SCON_RE){
    //未在接收状态且未待接收启始位时，状态机错误,这里丢弃
    if(pHwIo->BitPos < 1) return;
    //无条件接收当前位
    if(UsartHwIo_cbIsRx(pHwIo->Id)){
      pHwIo->BitData |= (unsigned short)1 << pHwIo->BitPos;
    }
    pHwIo->BitPos++;//下一位 

    //接收完成
    if(pHwIo->BitPos > _GetMaxLen(pHwIo)){
      UsartHwIo_cbTimerStop(pHwIo->pHwTimer);//停止定时器
      pHwIo->ISR |= USART_HW_IO_ISR_RX_FINAL;//接收完成待处理
    }
  }
  //===================发送时处理=======================
  if(pHwIo->SCON & USART_HW_IO_SCON_TCIE){
    //发送完成
    if(pHwIo->BitPos > _GetMaxLen(pHwIo)){
      UsartHwIo_cbTimerStop(pHwIo->pHwTimer);//停止定时器
      pHwIo->ISR |= USART_HW_IO_ISR_TX_FINAL;//接收完成待处理
    }
    //发送数据
    if(pHwIo->BitData & ((unsigned short)1 << pHwIo->BitPos))
      UsartHwIo_cbSetTx(pHwIo->Id);
    else UsartHwIo_cbClrTx(pHwIo->Id);
    pHwIo->BitPos++;//下一位  
  }
}

//---------------Rx引脚收到起始沿中断处理函数--------------------
//放入Rx引脚中断中
void UsartHwIo_RxIRQ(struct _UsartHwIo *pHwIo)
{
  //未在接收状态且未接收启始位时，状态机错误,这里丢弃
  if(pHwIo->BitPos || !(pHwIo->SCON & USART_HW_IO_SCON_RE))
    return;
  UsartHwIo_cbRxStop(pHwIo->Id);//停止接收中断
  pHwIo->BitPos = 1; //收到起始条件了
  //预启动定时器以接收首个数
  UsartHwIo_cbTimerStartRdy(pHwIo->pHwTimer);
}

//-------------------------快速任务函数-----------------------
//放入系统快速任务中查询
void UsartHwIo_FastTask(struct _UsartHwIo *pHwIo)
{
  unsigned char IsFinal = pHwIo->ISR & USART_HW_IO_ISR_TR_FINAL;
  if(!IsFinal) return;//没有待处理的数据 
  pHwIo->ISR &= ~USART_HW_IO_ISR_TR_FINAL;//先取消
  
  //数据发送后续处理:
  if(IsFinal & USART_HW_IO_ISR_TR_FINAL){
    pHwIo->ISR |= USART_HW_IO_ISR_TI;//发送完成需中断
    UsartHwIo_cbUsartIRQ(pHwIo);//发送中断
    return;
  }
  //数据接收后续处理
  /*if(IsFinal & USART_HW_IO_ISR_RX_FINAL)*/{  
    _RcvPro(pHwIo); //:
    pHwIo->ISR |= USART_HW_IO_ISR_RI;//按收完成需中断
    UsartHwIo_cbUsartIRQ(pHwIo);//接收中断
  }
}

/*********************************************************************
                      UART硬件配置与收发相关函数
*********************************************************************/
//------------------------------启动发送-----------------------------
//写完控制位SCON与SBUF后，调用此函数启动发送
void UsartHwIo_SendStart(struct _UsartHwIo *pHwIo)
{
  UsartHwIo_RcvStop(pHwIo);//先强制停止接收
  pHwIo->BitPos = 0;
  pHwIo->BitData = _GetSendBitData(pHwIo); 
  pHwIo->ISR = 0;
  
  __disable_irq(); //临界区操作
  UsartHwIo_cbClrTx(pHwIo->Id);//低电平开始启动
  UsartHwIo_cbTimerStart(pHwIo->pHwTimer); //启动定时器
	__enable_irq();
}

//-----------------------------停止发送------------------------------
//调用强制停止发送数据
void UsartHwIo_SendStop(struct _UsartHwIo *pHwIo)
{
  UsartHwIo_cbTimerStop(pHwIo->pHwTimer);//先停止定时器
  pHwIo->SCON &= ~USART_HW_IO_SCON_TCIE;//停止发送,关发送中断
  pHwIo->ISR &= ~USART_HW_IO_ISR_TI;//清发送中断标志
  UsartHwIo_cbSetTx(pHwIo->Id);//恢复为默认电平
}

//------------------------------启动接收-----------------------------
//写完控制位SCON后，调用此函数启动接收
void UsartHwIo_RcvStart(struct _UsartHwIo *pHwIo)
{
  UsartHwIo_RcvStart(pHwIo);//先强制停止发送
  pHwIo->BitPos = 0; //从起始位开始
  pHwIo->BitData = 0;//待读取填充1
  pHwIo->ISR = 0;
  UsartHwIo_cbRxStart(pHwIo->Id); //启动Rx接收中断
}

//-----------------------------停止接收------------------------------
//调用强制停止接收数据
void UsartHwIo_RcvStop(struct _UsartHwIo *pHwIo)
{
  UsartHwIo_cbRxStop(pHwIo->Id); //停止Rx接收中断
  UsartHwIo_cbTimerStop(pHwIo->pHwTimer);//停止定时器
  //停止接收，关闭接收中断
  pHwIo->SCON &= ~(USART_HW_IO_SCON_RE | USART_HW_IO_SCON_RCIE);
  pHwIo->ISR &= ~USART_HW_IO_ISR_RI;  //清除接收中断标志。
}

//---------------------------标准配置实现-------------------------
//只管输入串口的配置字，不参与串口的使能和其他寄存器操作
void UsartHwIo_Cfg(const struct _UsartDevCfg *pCfg,//串口配置结构体
                   void * pUsartHw,           //硬件设备指针
                   unsigned long Clk)        //当前串口使用的时钟
{
  //假定其它位在开机后一直保持默认值  
  struct _UsartHwIo *pHw = pUsartHw; 
  //类型控制位执行中判断
  pHw->UartCfg = pCfg->Cfg & USART_DEV_CFG_TYPE_MASK;
  //设置波特率
  UsartHwIo_cbCfgTimer(pHw->pHwTimer, UsartDevCfg_GetBuad(pCfg));
}



