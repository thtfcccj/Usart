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

#include "EepromInner.h"

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
{3,    5,1,2};

static const unsigned char _Para16Super[] = //Para低4bit为1个参数时超级用户
{4,    0, 3, 4, 7};
static const unsigned char _Para16Admin[] = //Para低4bit为1个参数时
{3,    3, 4, 7};

static const unsigned char _Para13Super[] = //Para低4bit为1+3bit模式时
{5,    0, 3, 4, 8,9};
static const unsigned char _Para13Admin[] = //Para低4bit为1+3bit模式时
{4,    3, 4, 8,9};

//由工作模式查找状态：1主机+双参数， 2主机+单参数, 主机1+3bit模式, 0或其它从机
extern unsigned char UsartDevCfgU0_cbGetModeType(unsigned char Mode);

//----------------------------------回调函数------------------------------- 
//描述
static const struct _MNumDesc _Desc[] = {
  {MNUM_TYPE_DEC, 0, 15}, //0工作模式
  {MNUM_TYPE_DEC, 0, 3},  //1工作模式下的通讯协议(低4bit为2+2模式时高2bit)
  {MNUM_TYPE_DEC, 0, 3},  //2工作模式与协议下的配置(低4bit为2+2模式时低2bit)
  {MNUM_TYPE_DEC, 1, 255},//3(主机模式)帧间隔时间
  {MNUM_TYPE_DEC, 10,255},//4(主机模式)接收等待时间
  {MNUM_TYPE_DEC, 1, 255},//5(从机模式)从机地址
  {MNUM_TYPE_DEC, 1, 255},//6(从机模式)帧间隔时间
  {MNUM_TYPE_DEC, 0, 15}, //7Para低4bit为1个参数时
  {MNUM_TYPE_DEC, 0, 1},  //8Para低4bit为1+3bit模式时
  {MNUM_TYPE_DEC, 0, 7},  //9Para低4bit为1+3bit模式时  
};

static void _Notify(unsigned char Type,//通报类型
                    //根据菜单类型输入的输入输出数据区
                    //该区域结构在控件里定义,直接指针转换即可
                    void *pv)
{
  struct _UsartDevCfg *pCfg = &UsartDevCfg[UsartDevCfgTMenu_cbGetId()];
  struct _MNumAdjUser *pUser = (struct _MNumAdjUser *)pv;
  
  //准备查找表
  const unsigned char *pLUT;
  unsigned char ModeType = UsartDevCfgU0_cbGetModeType(pCfg->U.M.Cfg >> 4);
  if(ModeType == 1){ //1主机+双参数
    if(Power_IsAdminMoreUp()) pLUT = _MasterSuper;
    else pLUT = _MasterAdmin;
  }
  else if(ModeType == 2){ //主机+单参数
    if(Power_IsAdminMoreUp()) pLUT = _Para16Super;
    else pLUT = _Para16Admin;
  } 
  else if(ModeType == 3){ //主机1+3bit模式
    if(Power_IsAdminMoreUp()) pLUT = _Para13Super;
    else pLUT = _Para13Admin;
  }   
  else{//从机
    if(Power_IsAdminMoreUp()) pLUT = _SlaveSuper;
    else pLUT = _SlaveAdmin;
  }
  
  switch(Type){
  case TM_NOTIFY_GET_DATA:{ //将当前值装入
    unsigned char LutCount = *pLUT++;
    for(unsigned char Pos = 0; Pos < LutCount; Pos++){
      unsigned char Data = 0; //获得当前值
      switch(pLUT[Pos]){
        case 0: Data  = pCfg->U.M.Cfg >> 4;  break; //工作模式
        case 1: Data = (pCfg->U.M.Cfg >> 2) & 0x03; break;//工作模式下的通讯协议
        case 2: Data = pCfg->U.M.Cfg & 0x03;  break; //工作模式与协议下的配置(主从相同)
        
        case 3: Data = pCfg->U.M.SpaceT;  break;//(主机模式)帧间隔时间
        case 4: Data = pCfg->U.M.WaitT;  break;//(主机模式)接收等待时间
        case 5: Data = pCfg->U.S.Adr;  break;//(从机模式)从机地址
        case 6: Data = pCfg->U.S.SpaceT;  break;//(从机模式)数据间隔
        case 7: Data = pCfg->U.M.Cfg & 0x0F;  break;//Para低4bit为1个参数时
        case 8: Data = (pCfg->U.M.Cfg & 0x08)? 1 :0 ;  break;//1+3bit模式时
        case 9: Data = pCfg->U.M.Cfg & 0x07;  break;//Para低4bit为1+3bit模式时        
      }
      pUser->Value[Pos] = Data;
    }
    break;
  }
  case TM_NOTIFY_SET_DATA:{ //保存设定值
    unsigned char PrvUMode = pCfg->U.M.Cfg & USART_DEV_UMODE_MASK;
    unsigned char LutCount = *pLUT++;
    for(unsigned char Pos = 0; Pos < LutCount; Pos++){
      unsigned char Data = pUser->Value[Pos]; //获得当前值
      switch(pLUT[Pos]){
        case 0: //工作模式        
          pCfg->U.M.Cfg &= ~USART_DEV_UMODE_MASK;
          pCfg->U.M.Cfg |= Data << 4; break;        
        case 1://工作模式下的通讯协议
          pCfg->U.M.Cfg &= ~USART_DEV_UPROTOCOL_MASK;
          pCfg->U.M.Cfg |= Data << 2; break;
        case 2://工作模式与协议下的配置
          pCfg->U.M.Cfg &= ~USART_DEV_UPARA_MASK;
          pCfg->U.M.Cfg |= Data; break;          
        case 3: pCfg->U.M.SpaceT = Data;  break;//(主机模式)帧间隔时间
        case 4: pCfg->U.M.WaitT = Data;  break;//(主机模式)接收等待时间
        case 5: pCfg->U.S.Adr = Data;  break;//(从机模式)从机地址
        case 6: pCfg->U.S.SpaceT = Data;  break;//(从机模式)数据间隔  
        case 7: //Para低4bit为1个参数时 
          pCfg->U.M.Cfg &= ~0x0F;  
          pCfg->U.M.Cfg |= Data;           
          break;
        case 8: //低4bit为1+3bit模式时
          if(Data) pCfg->U.M.Cfg |= 0x08;
          else pCfg->U.M.Cfg &= ~0x08;
          break;          
        case 9: //低4bit为1+3bit模式时
          pCfg->U.M.Cfg &= ~0x07;  
          pCfg->U.M.Cfg |= Data;           
          break;
      }
    }
    UsartDevCfg_Save(UsartDevCfgTMenu_cbGetId());
    UiTips_UpdateS(ls_OpFinal); //提示成功
    //模式的转换，需要重新分配类型分别挂接，故需要回写保存
    if(PrvUMode != (pCfg->U.M.Cfg & USART_DEV_UMODE_MASK)){
      Eeprom_ForceWrBufAndRestart();
    }
    break; 
  }
  case TM_NOTIFY_MNUM_GET_DESC:{ //得到数值描述
    memcpy(&pUser->Desc,  &_Desc[pLUT[pUser->CurItem + 1]], 
           sizeof(struct _MNumDesc));
    break;
  }
  case TM_NOTIFY_USER_GET_LUT:{ //得到查找表
    memcpy(pv, pLUT, *pLUT + 1);
    break;
  }
  default:break;
  }
}

//对应的字符
const LanCode_t* const lsAry_U0[] = {
  ls_WorkMode, ls_Protocol, ls_ProtocolPara, 
  ls_SpaceT, ls_WaitT, ls_SlaveAdr, ls_WaitRouter, 
  ls_UsartPara16, ls_UsartPara1, ls_UsartPara7};

//----------------------U0模式的菜单结构-------------------------
const TMenu_t UsartDevCfgU0_TMenu = {//菜单结构
  TMTYPE_MNUMADJ | TM_MNUMADJ_WRITE, //菜单类型为多值调整模式模式与用户区标志
  0x80 | 10,                          //由菜单类型决定的相关数据大小
  ls_WorkParaSet,                    //菜单头,为NULL时从回调里读取
  USART_DEV_CFG_TMENU_PARENT,        //自已的父菜单
  lsAry_U0,                         //存放自已的子菜单阵列连接头
  _Notify,                           //与用户空间交互的通报函数
};






