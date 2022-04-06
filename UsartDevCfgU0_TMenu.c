/***************************************************************************

     UsartDevCfg扩展用户配置->方式0,-在使用TMenu菜单系统时的实现
                         
****************************************************************************/

#include "UsartDevCfg_TMenu.h"
#include "TMenu_MNumAdj.h"
#include "UiTips.h"
#include "Power.h"

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
  extern const char ls_WorkMode[]      = {"工作模式"};
  extern const char ls_Protocol[]      = {"通讯协议"};
  extern const char ls_ProtocolPara[]  = {"协议参数"};
  extern const char ls_SpaceT[]        = {"帧间隔时间"};
  extern const char ls_WaitT[]         = {"接收等待时间"};
  extern const char ls_SlaveAdr[]      = {"从机地址"};
  extern const char ls_WaitRouter[]    = {"等待路由时间"}; 
  
  extern const char ls_WorkParaSet[]   = {"工作参数设置"};  
  
  //提示
  extern const char ls_OpFinal[]       = {"操作成功!"};
#endif

//-----------------------------------查找表--------------------------------
//子菜单查找表结构
static const unsigned char _MasterSuper[] = //主机模式超级用户
{5,    0,1,2,3,4};
static const unsigned char _SlaveSuper[] = //从机模式超级用户
{5,    5,0,1,2,6};

static const unsigned char _MasterAdmin[] = //主机模式管理员
{2,    3,4};
static const unsigned char _SlaveAdmin[] = //从机模式管理员
{1,    5};

//工作模式对应是否为主机
extern unsigned short UsartDevCfgU0_cbIsMasterMask;

//----------------------------------回调函数------------------------------- 
//描述
static const struct _MNumDesc _Desc[] = {
  {MNUM_TYPE_DEC, 0, 15},//工作模式
  {MNUM_TYPE_DEC, 0, 3},//通讯协议
  {MNUM_TYPE_DEC, 0, 3},//参数配置
  {MNUM_TYPE_DEC, 0, 255},//帧间隔时间
  {MNUM_TYPE_DEC, 10,255},//接收等待时间
  {MNUM_TYPE_DEC, 1, 255},//从机地址
  {MNUM_TYPE_DEC, 20,255},//路由等待时间
};

static void _Notify(unsigned char Type,//通报类型
                    //根据菜单类型输入的输入输出数据区
                    //该区域结构在控件里定义,直接指针转换即可
                    void *pv)
{
  struct _UsartDevCfg *pCfg = &UsartDevCfg[UsartDevCfgTMenu_cbGetId()];
  struct _MNumAdjUser *pUser = (struct _MNumAdjUser *)pv;
  
  switch(Type){
  case TM_NOTIFY_GET_DATA:{ //将当前值装入
    pUser->Value[0] = pCfg->U.M.Cfg >> 4;
    pUser->Value[1] = (pCfg->U.M.Cfg >> 2) & 0x03;  
    pUser->Value[2] = pCfg->U.M.Cfg & 0x03;   
    pUser->Value[3] = pCfg->U.M.SpaceT;//帧间隔时间
    pUser->Value[4] = pCfg->U.M.WaitT; //接收等待时间
    pUser->Value[5] = pCfg->U.S.Adr;//从机地址
    pUser->Value[6] = pCfg->U.S.WaitRoute; //路由等待时间
    break;
  }
  case TM_NOTIFY_SET_DATA:{ //保存设定值
    pCfg->U.M.Cfg = (pUser->Value[0] << 4) |
                    (pUser->Value[1] << 2) | 
                    pUser->Value[2];
    if(pCfg->U.M.SpaceT != pUser->Value[3])
      pCfg->U.M.SpaceT = pUser->Value[3];//帧间隔时间
    else pCfg->U.M.SpaceT = pUser->Value[5];//从机地址
    
    if(pCfg->U.M.WaitT != pUser->Value[4])   
      pCfg->U.M.WaitT = pUser->Value[4]; //接收等待时间
    else pCfg->U.S.WaitRoute = pUser->Value[6];//路由等待时间 
    
    UsartDevCfg_Save(UsartDevCfgTMenu_cbGetId());
    UiTips_UpdateS(ls_OpFinal); //提示成功
    break; 
  }
  case TM_NOTIFY_MNUM_GET_DESC:{ //得到数值描述
    memcpy(&pUser->Desc,  &_Desc[pUser->CurItem], 
           sizeof(struct _MNumDesc));
    break;
  }
  case TM_NOTIFY_USER_GET_LUT:{ //得到查找表
    const unsigned char *pLUT;
    if(UsartDevCfgU0_cbIsMasterMask & (1 << (pCfg->U.M.Cfg >> 4))){
      if(Power_IsSuper()) pLUT = _MasterSuper;
      else pLUT = _MasterAdmin;
    }
    else{
      if(Power_IsSuper()) pLUT = _SlaveSuper;
      else pLUT = _SlaveAdmin;
    }
    memcpy(pv, pLUT, *pLUT + 1);
    break;
  }
  default:break;
  }
}

//对应的字符
const LanCode_t* const lsAry_U0[] = {
  ls_WorkMode, ls_Protocol, ls_ProtocolPara, 
  ls_SpaceT, ls_WaitT, ls_SlaveAdr, ls_WaitRouter};

//----------------------U0模式的菜单结构-------------------------
const TMenu_t UsartDevCfgU0_TMenu = {//菜单结构
  TMTYPE_MNUMADJ | TM_MNUMADJ_WRITE, //菜单类型为多值调整模式模式与用户区标志
  0x80 | 5,                          //由菜单类型决定的相关数据大小
  ls_WorkParaSet,                    //菜单头,为NULL时从回调里读取
  USART_DEV_CFG_TMENU_PARENT,        //自已的父菜单
  lsAry_U0,                         //存放自已的子菜单阵列连接头
  _Notify,                           //与用户空间交互的通报函数
};






