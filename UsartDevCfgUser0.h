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
    unsigned char WaitT;    //发送数据后的等待时间,ms为单位
    unsigned char SpaceT;   //数据帧间隔时间,ms为单位
    unsigned char Cfg;      //主机时配置，如通讯载议等
  }M; //主机模式时
  struct{
    unsigned char Adr;        //从机地址
    unsigned char WaitRoute;  //设备做中转合用时，等待下级设备返回时间
    unsigned char Cfg;        //从机时配置，如通讯载议等
  }S; //从机模式时
}UsartDevCfgUser0_t;


#endif

