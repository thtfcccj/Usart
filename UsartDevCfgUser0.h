/******************************************************************************

                     UsartDevCfg扩展用户配置->方式0
主要用于主从机时，控制地址与间隔时间等
******************************************************************************/
#ifndef __USART_DEV_CFG_USER0_H
#define __USART_DEV_CFG_USER0_H
#ifdef SUPPORT_EX_PREINCLUDE//不支持Preinlude時
  #include "Preinclude.h"
#endif

//----------------------------用户结构配置--------------------------------------
typedef union _UsartDevCfgUser0{
  struct{
    unsigned char Cfg;        //相关配置,见定义
    unsigned char SpaceT;     //数据帧间隔时间,ms为单位    
    unsigned char WaitT;      //发送数据后的等待时间,ms为单位
  }M; //主机模式时
  struct{
    unsigned char Cfg;        //相关配置，见定义
    unsigned char SpaceT;     //数据帧间隔时间,超过此时间认为一帧数据完成    
    unsigned char Adr;        //从机地址
  }S; //从机模式时
}UsartDevCfgUser0_t;

//-------------------------主从机模式下Cfg配置----------------------------------
//工作模式：
#define USART_DEV_UMODE_SHIRT      4     
#define USART_DEV_UMODE_MASK       0xf0
#define USART_DEV_UMODE_SLAVE      0     //从机模式
#define USART_DEV_UMODE_MASTER     1     //主机模式
                               //2~15: 其它如打印机，WIFI等,由用户决定
#ifndef USART_DEV_UMODE_COUNT
  #define USART_DEV_UMODE_COUNT      2   //现支持模式总数
#endif

//工作模式对应通讯协议,如Modbus,透传等,具体由用户决定
#define USART_DEV_UPROTOCOL_SHIRT  2
#define USART_DEV_UPROTOCOL_MASK   0x0C 

//工作模式与协议下的配置(如MODBUS下的RTU与ASC)
#define USART_DEV_UPARA_MASK       0x03 

#endif

