/***************************************************************************

      UsartDev中的struct _UsartDevCfg,-在使用TMenu菜单系统时的实现
                         
****************************************************************************/

#include "UsartDevCfg_TMenu.h"
#include "TMenu_MNumAdj.h"
#include "UiTips.h"

#include "stringEx.h"
#include <string.h>
#include "TMenuBuf.h"

//--------------------------------内部字符资源--------------------------------- 
#ifdef TM_EN_MUTI_LAN            //常量选择型多国语言时
  //暂不支持常量选择型字符串
#elif defined(TM_RESOURCE_LAN)  //由资源文件获取多国语言时
  //菜单
  #include "lsAry.h"
  //提示
  #include "Rid.h"
  #include "RidNote.h"
  #define ls_OpFinal   RID_pGeNote(RID_NOTE_OP_FINAL)
#else  //单一语言字符串时
  //菜单
  static const char ls_Baudrate[]      = {"波特率"};
  static const char ls_ParityMode[]    = {"校验位[0无][1奇][2偶]"};
  static const char ls_StopBit[]       = {"停止位[0:1位][1:2位]"};
  static const char ls_DataBit[]       = {"数据位[0:8位][1:7位]"};
  static const char ls_ComParaSet[]    = {"通讯参数设置"};  
  //提示
  extern const char ls_OpFinal[]       = {"操作成功!"};
#endif

//--------------------------精简模式时,由当前设置转波特率-----------------------
#ifdef SUPPORT_USART_DEV_CFG_TINY    
unsigned char _Baud2Id(unsigned long Baud){
  if(Baud >= 115200) return USART_DEV_BUAD_115200;
  if(Baud >= 57600) return USART_DEV_BUAD_57600; 
  if(Baud >= 38400) return USART_DEV_BUAD_38400;
  if(Baud >= 19200) return USART_DEV_BUAD_19200;   
  if(Baud >= 9600) return USART_DEV_BUAD_9600;
  if(Baud >= 4800) return USART_DEV_BUAD_4800; 
  if(Baud >= 2400) return USART_DEV_BUAD_2400;
  return USART_DEV_BUAD_1200;   
}
#endif

//----------------------------------回调函数------------------------------- 
//描述
static const struct _MNumDesc _UsartDevCfgDesc[] = {
  {MNUM_TYPE_DEC, 1200, 115200},//波特率
  {MNUM_TYPE_DEC, 0, 2},//校验位
  {MNUM_TYPE_DEC, 0, 1},//停止位
  {MNUM_TYPE_DEC, 0, 1},//数据位
};

static void _Notify(unsigned char Type,//通报类型
                    //根据菜单类型输入的输入输出数据区
                    //该区域结构在控件里定义,直接指针转换即可
                    void *pv)
{
  struct _MNumAdjUser *pUser = (struct _MNumAdjUser *)pv;
  struct _UsartDevCfg *pCfg = &UsartDevCfg[UsartDevCfgTMenu_cbGetId()];
  switch(Type){
  case TM_NOTIFY_GET_DATA:{ //将当前值装入
    pUser->Value[0] = UsartDevCfg_GetBuad(pCfg);
    if(UsartDevCfg_IsParEn(pCfg)){
      if(UsartDevCfg_IsOdd(pCfg)) pUser->Value[1] = 1;
      else pUser->Value[1] = 2;
    }
    else pUser->Value[1] = 0;
    if(UsartDevCfg_Is2Stop(pCfg)) pUser->Value[2] = 1;
    else pUser->Value[2] = 0;
    if(UsartDevCfg_Is7Bit(pCfg)) pUser->Value[3] = 1;    
    else pUser->Value[3] = 0;
    break;
  }
  case TM_NOTIFY_SET_DATA:{ //保存设定值
    #ifdef SUPPORT_USART_DEV_CFG_TINY
      UsartDevCfg_SetBuadId(pCfg, _Baud2Id(pUser->Value[0]));    
    #else
      UsartDevCfg_SetBuad(pCfg, pUser->Value[0]);
    #endif
    if(pUser->Value[1] == 0) UsartDevCfg_ClrParEn(pCfg);
    else{
      UsartDevCfg_SetParEn(pCfg);
      if(pUser->Value[1] == 1) UsartDevCfg_SetOdd(pCfg);
      else UsartDevCfg_ClrOdd(pCfg);
    }
    if(pUser->Value[2] == 1) UsartDevCfg_Set2Stop(pCfg);
    else UsartDevCfg_Clr2Stop(pCfg); 
    if(pUser->Value[3] == 1) UsartDevCfg_Set7Bit(pCfg);
    else UsartDevCfg_Clr7Bit(pCfg);     
    
    UsartDevCfg_Save(UsartDevCfgTMenu_cbGetId());
    UiTips_UpdateS(ls_OpFinal); //提示成功
    break; 
  }
  case TM_NOTIFY_MNUM_GET_DESC:{ //得到数值描述
    memcpy(&pUser->Desc,  &_UsartDevCfgDesc[pUser->CurItem], 
           sizeof(struct _MNumDesc));
    break;
  }
  default:break;
  }
}

//对应的字符
const LanCode_t* const lsAry_UsartDevCfg[] = {
  ls_Baudrate, ls_ParityMode, ls_StopBit, ls_DataBit};

//--------------------------顶层菜单结构----------------------------

const TMenu_t UsartDevCfg_TMenu = {//菜单结构
  TMTYPE_MNUMADJ | TM_MNUMADJ_WRITE, //菜单类型为多值调整模式模式与用户区标志
  4,                                 //由菜单类型决定的相关数据大小
  ls_ComParaSet,                     //菜单头,为NULL时从回调里读取
  USART_DEV_CFG_TMENU_PARENT,        //自已的父菜单
  lsAry_UsartDevCfg,                 //存放自已的子菜单阵列连接头
  _Notify,                           //与用户空间交互的通报函数
};


