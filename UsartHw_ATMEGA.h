/******************************************************************************

                   Usart设备硬件定义及相关配置模块
//此模块定义UsartHw设备,同时辅助UsartDev完成对ATMEGA相关芯片Usart的配置
******************************************************************************/
#ifndef __USART_HW_ATMEGA_H
#define __USART_HW_ATMEGA_H

/******************************************************************************
                         相关配置
******************************************************************************/

//定义CPU类型
#define   USART_HW_CPU_TYPE   0
//可定义为类型有:
//0 关闭
//1 ATmega32或ATmega16
//2 ATmega64或ATmega128


/******************************************************************************
                  Usart设备硬件定义
******************************************************************************/
//------------------------------寄存器类型定义---------------------------
typedef volatile unsigned char AVR_REG;

//-------------------USART寄存器结构体--------------------
//该结构体只使用于Mega64的USART，但没有包含UCSRC寄存器
struct _USART_HW{
  //AVR_REG UBRRH;    //波特率寄存器高位
  AVR_REG UBRRL;    //波特率寄存器低位
  AVR_REG UCSRB;    //控制状态寄存器B
  AVR_REG UCSRA;    //控制状态寄存器A
  AVR_REG UDR;      //I/O数据寄存器
};

//---------------------------串口实例化------------------------------
//#include <avr/IO.h>  //AVRGCC环境时
#include <ioavr.h>//IAR环境时

#define Mega_BASE_USART1      ((struct _USART_HW*)&UBRR1L)//MEGA64或128时
#define Mega_BASE_USART0      ((struct _USART_HW*)&UBRR0L)//MEGA64或128时
#define Mega_BASE_USART       ((struct _USART_HW*)&UBRRL)//MEGA32时

/******************************************************************************
                        相关实现
******************************************************************************/
#include "UsartDev.h"
#include "UsartDevCfg.h"

//-----------------------Atmega32或16的标准配置实现--------------------------
//送入配置参数,同时返回实际上的波特率
//此函数将复位整个Usart的当前操作
void UsartHw_Mega32Cfg(struct _UsartDevCfg *pCfg);

//-----------------------Atmega64或128配置Usart0实现------------------------------
//送入配置参数,同时返回实际上的波特率
//此函数将复位整个Usart的当前操作
void UsartHw_Mega64Cfg0(struct _UsartDevCfg *pCfg);

//-----------------------Atmega64或128配置Usart1实现------------------------------
//送入配置参数,同时返回实际上的波特率
//此函数将复位整个Usart的当前操作
void UsartHw_Mega64Cfg0(struct _UsartDevCfg *pCfg);

/******************************************************************************
                    UsartDev之ATMEGA实现里的中断函数声明
******************************************************************************/

//---------------------USART硬件接收中断处理函数----------------------------
void UsartDev_RcvIRQ(struct _UsartDev *pDev);

//---------------------USART硬件发送中断处理函数----------------------------
void UsartDev_SendIRQ(struct _UsartDev *pDev);

/******************************************************************************
                        底层实现相关回调函数-ATMEGA系列
//此回调函数根据底层实现不同可能有区别
******************************************************************************/

//--------------------------得到系统主时钟函数-----------------------------
//unsigned long UsartHw_cbGetMck(void);
#define UsartHw_cbGetMck() (12000000)

#endif

