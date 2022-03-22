/*******************************************************************************

                   Usart实例模块实现

*******************************************************************************/
#include "Usart.h"
#include "IoCtrl.h"
#include <string.h>

struct _Usart Usart; //实例

/*******************************************************************************
		                      相关函数实现
*******************************************************************************/

//----------------------------------初始化-------------------------------------
//需在UsartDevCfg_Init()后调用
void Usart_Init(void)
{
  memset(&Usart, 0, sizeof(struct _Usart));
  CfgUsart(); //IoCtrl.h:   IO口初始化
  //各底层分别初始化
  for(unsigned char UsartId = 0; UsartId < USART_COUNT; UsartId++){
    UsartDev_Init(&Usart.Dev[UsartId], //底层硬件通讯初始化
                  Usart_cbHw[UsartId]); 
    Usart_cbCfgInt[UsartId]();  //最后配置中断
  }
}

//----------------------------------由指针得到ID-------------------------------
//负值未找到
signed char Usart_GetUsartId(const struct _UsartDev *pDev)
{
  for(unsigned char UsartId = 0; UsartId < USART_COUNT; UsartId++){
    if(pDev == &Usart.Dev[UsartId]) return UsartId;
  }
  return -1;//未找到
}


