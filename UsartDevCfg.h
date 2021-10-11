/******************************************************************************

           UsartDev中的struct _UsartDevCfg,接口操作函数
//此模块仅通过struct _UsartDevCfg指针操作结构成员变量，但不负责保存EEPROM
******************************************************************************/
#ifndef __USART_DEV_CFG_H
#define __USART_DEV_CFG_H
#ifdef SUPPORT_EX_PREINCLUDE//不支持Preinlude時
  #include "Preinclude.h"
#endif

/******************************************************************************
                             相关配置
******************************************************************************/

//精简模式时，struct _UsartDevCfg占1Byte, 波特率为枚举型
//#define SUPPORT_USART_DEV_CFG_TINY    

//定义托管的数据，如通讯地址，默认不支持
//#define USART_DEV_CFG_USER  //示例:   unsigned char Adr; unsigned char Para;


//---------------------------------为独立模块时定义---------------------------
//定义支持保存，支持时为独立模块
#define SUPPORT_USART_DEV_CFG_SAVE
//支持保存，为多例化，否则为单例化
//#define SUPPORT_USART_DEV_CFG_MUTITON

/****************************************************************************
                             相关结构
******************************************************************************/

struct _UsartDevCfg{
  #ifndef SUPPORT_USART_DEV_CFG_TINY    //精简模式时，可用Cfg高位组成枚举型组成
    unsigned short BuadL;   //波特率低16bit
    unsigned char  BuadH;   //波特率高8bit
  #endif  
  unsigned char  Cfg;    //相关配置,见定义
  #ifdef USART_DEV_CFG_USER
    USART_DEV_CFG_USER
  #endif
};

//相关配置定义为:
#define USART_DEV_CFG_ODD       0x01//奇校验,否则为偶校验
#define USART_DEV_CFG_PAR_EN    0x02//校验允许,否则无校验
#define USART_DEV_CFG_2_STOP    0x04//2个停止位,否则为1个
#define USART_DEV_CFG_7_BIT     0x08//7个数据位,否则为8个

#ifdef SUPPORT_USART_DEV_CFG_TINY    //精简模式时，Cfg高位为波特率枚举型
  #define USART_DEV_BUAD_SHIFT  4
  #define USART_DEV_BUAD_MASK   (0x07 << USART_DEV_BUAD_SHIFT) //具体配置为:
  #define USART_DEV_BUAD_9600    0  //默认
  #define USART_DEV_BUAD_4800    1
  #define USART_DEV_BUAD_2400    2
  #define USART_DEV_BUAD_1200    3
  #define USART_DEV_BUAD_19200   4
  #define USART_DEV_BUAD_38400   5
  #define USART_DEV_BUAD_57600   6
  #define USART_DEV_BUAD_115200  7
#endif

/******************************************************************************
                   同时支持单例化与多例化时的宏定义
******************************************************************************/

//1-1. 函数传递时, 可读写结构声明
#ifdef SUPPORT_USART_DEV_CFG_MUTITON  //多例化时
  #define USART_DEV_CFG_P    struct _UsartDevCfg *pUsartDevCfg,
#else
  #define USART_DEV_CFG_P
#endif

//1-2. 函数传递时用的可读写指针,后不跟参数,也可用于局部变量声明
#ifdef SUPPORT_USART_DEV_CFG_MUTITON  //多例化时
  #define USART_DEV_CFG_PV    struct _UsartDevCfg *pUsartDevCfg
#else
  #define USART_DEV_CFG_PV    void
#endif

//2-1. 函数传递时用的只写指针,后跟参数
#ifdef SUPPORT_USART_DEV_CFG_MUTITON  //多例化时
  #define USART_DEV_CFG_CP    const struct _UsartDevCfg *pUsartDevCfg, 
#else
  #define USART_DEV_CFG_CP
#endif  
  
//2-2. 函数传递时用的只读指针,后不跟参数,也可用于局部变量声明
#ifdef SUPPORT_USART_DEV_CFG_MUTITON  //多例化时
  #define USART_DEV_CFG_CPV    const struct _UsartDevCfg *pUsartDevCfg 
#else
  #define USART_DEV_CFG_CPV    void
#endif   

//3-1. 内部操作时的this指针
#ifdef SUPPORT_USART_DEV_CFG_MUTITON  //多例化时
  #define USART_DEV_CFG_THIS_  pUsartDevCfg->
#else
  extern struct _UsartDevCfg UsartDevCfg;
  #define USART_DEV_CFG_THIS_  UsartDevCfg.
#endif

//3-2. 声明或形参传递时的this指针
#ifdef SUPPORT_USART_DEV_CFG_MUTITON  //多例化时
  #define USART_DEV_CFG_FTHIS  pUsartDevCfg
#else
  extern struct _UsartDevCfg UsartDevCfg;   //直接定义
  #define USART_DEV_CFG_FTHIS  
#endif
  
//3-3. 内部操作时取基址
#ifdef SUPPORT_USART_DEV_CFG_MUTITON  //多例化时
  #define USART_DEV_CFG_BASE  pUsartDevCfg
#else
  extern struct _UsartDevCfg UsartDevCfg;
  #define USART_DEV_CFG_BASE  (&UsartDevCfg)
#endif

/******************************************************************************
                             行为函数
******************************************************************************/
#ifdef  SUPPORT_USART_DEV_CFG_SAVE   //支持保存时

//----------------------------初始化函数----------------------------------
void UsartDevCfg_Init(USART_DEV_CFG_P signed char IsInited);

//----------------------------保存函数----------------------------------
//结构变化时调用此函数保存至EEPROM
void UsartDevCfg_Save(USART_DEV_CFG_CPV);

//----------------------------默认结构----------------------------------
//回调结构，多例化时需由外部实现
#ifdef SUPPORT_USART_DEV_CFG_MUTITON  //多例化时
  const struct _UsartDevCfg *UsartDevCfg_cbpGetDefault(USART_DEV_CFG_CPV);
#else
  extern const struct _UsartDevCfg UsartDevCfg_cbDefault;
  #define UsartDevCfg_cbpGetDefault(a)  &UsartDevCfg_cbDefault
#endif

#endif //SUPPORT_USART_DEV_CFG_SAVE
/*****************************************************************************
                             成员操作函数
多例化宏操作
******************************************************************************/

//----------------------------波特率相关----------------------------------
#ifdef SUPPORT_USART_DEV_CFG_TINY    //精简模式时，Cfg高位为波特率枚举型
  extern const unsigned long UsartDevCfg_Buad[];
  #define UsartDevCfg_GetBuad(pcfg) \
    UsartDevCfg_Buad[((pcfg)->Cfg & USART_DEV_BUAD_MASK) >> USART_DEV_BUAD_SHIFT]
    //#define UsartDevCfg_SetBuadc(pcfg, buad) //使用UsartDevCfg_SetUserCfg()
#else
  #define UsartDevCfg_GetBuad(pcfg)  \
    (((unsigned long)((pcfg)->BuadH) << 16) + (pcfg)->BuadL)
  #define UsartDevCfg_SetBuad(pcfg, buad) \
      do{(pcfg)->BuadH = buad >> 16; (pcfg)->BuadL = buad & 0xffff;}while(0)      
#endif

//-------------------------------配置位整体操作相关--------------------------
#define UsartDevCfg_GetCfg(pcfg) ((pcfg)->Cfg)
#define UsartDevCfg_SetCfg(pcfg, cfg) do{(pcfg)->Cfg = cfg;}while(0)

//-----------------------------通讯部分位定义--------------------------------
#define UsartDevCfg_GetCommCfg(pcfg) ((pcfg)->Cfg & 0x0f)
#define UsartDevCfg_SetCommCfg(pcfg,commcfg) do{\
  (pcfg)->Cfg = (commcfg & 0x0f) | ((pcfg)->Cfg & 0xf0);}while(0)
    
//----------------------------用户托管部分位定义-------------------------------
#define UsartDevCfg_GetUserCfg(pcfg) ((pcfg)->Cfg & 0xf0)
#define UsartDevCfg_SetUserCfg(pcfg,userCfg) do{\
  (pcfg)->Cfg = (userCfg & 0xf0) | ((pcfg)->Cfg & 0x0f);}while(0)
    
//----------------------------奇偶校验位------------------------------------
//奇校验,否则为偶校验  
#define UsartDevCfg_IsOdd(pcfg)  ((pcfg)->Cfg & USART_DEV_CFG_ODD)
#define UsartDevCfg_SetOdd(pcfg) do{(pcfg)->Cfg |= USART_DEV_CFG_ODD;}while(0)  
#define UsartDevCfg_ClrOdd(pcfg) do{(pcfg)->Cfg &= ~USART_DEV_CFG_ODD;}while(0)
    
//--------------------------------校验允许位------------------------------------
//校验允许,否则无校验
#define UsartDevCfg_IsParEn(pcfg)  ((pcfg)->Cfg & USART_DEV_CFG_PAR_EN)
#define UsartDevCfg_SetParEn(pcfg) do{(pcfg)->Cfg |= USART_DEV_CFG_PAR_EN;}while(0)  
#define UsartDevCfg_ClrParEn(pcfg) do{(pcfg)->Cfg &= ~USART_DEV_CFG_PAR_EN;}while(0)  
    
//------------------------------------停止位------------------------------------
//2个停止位,否则为1个
#define UsartDevCfg_Is2Stop(pcfg)  ((pcfg)->Cfg & USART_DEV_CFG_2_STOP)
#define UsartDevCfg_Set2Stop(pcfg) do{(pcfg)->Cfg |= USART_DEV_CFG_2_STOP;}while(0)  
#define UsartDevCfg_Clr2Stop(pcfg) do{(pcfg)->Cfg &= ~USART_DEV_CFG_2_STOP;}while(0)    
    
//------------------------------------数据位------------------------------------
//7个数据位,否则为8个
#define UsartDevCfg_Is7Bit(pcfg)  ((pcfg)->Cfg & USART_DEV_CFG_7_BIT)
#define UsartDevCfg_Set7Bit(pcfg) do{(pcfg)->Cfg |= USART_DEV_CFG_7_BIT;}while(0)  
#define UsartDevCfg_Clr7Bit(pcfg) do{(pcfg)->Cfg &= ~USART_DEV_CFG_7_BIT;}while(0)     

#endif

