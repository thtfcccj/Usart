/***************************************************************************

     UsartDevCfg��չ�û�����->��ʽ0,-��ʹ��TMenu�˵�ϵͳʱ��ʵ��
                         
****************************************************************************/

#include "UsartDevCfg_TMenu.h"
#include "TMenu_MNumAdj.h"
#include "UiTips.h"
#include "Power.h"

#include "stringEx.h"
#include <string.h>
#include "TMenuBuf.h"

//--------------------------------�ڲ��ַ���Դ--------------------------------- 
#ifdef TM_EN_MUTI_LAN            //����ѡ���Ͷ������ʱ
  //�ݲ�֧�ֳ���ѡ�����ַ���
#elif defined(TM_RESOURCE_LAN)  //����Դ�ļ���ȡ�������ʱ
  //�˵�
  #include "lsAry.h"
  //��ʾ
  #include "Rid.h"
  #include "RidNote.h"
  #define ls_OpFinal   RID_pGeNote(RID_NOTE_OP_FINAL)
#else  //��һ�����ַ���ʱ
  //�˵�
  extern const char ls_WorkMode[]      = {"����ģʽ"};
  extern const char ls_Protocol[]      = {"ͨѶЭ��"};
  extern const char ls_ProtocolPara[]  = {"Э�����"};
  extern const char ls_SpaceT[]        = {"֡���ʱ��"};
  extern const char ls_WaitT[]         = {"���յȴ�ʱ��"};
  extern const char ls_SlaveAdr[]      = {"�ӻ���ַ"};
  extern const char ls_WaitRouter[]    = {"�ȴ�·��ʱ��"}; 
  
  extern const char ls_WorkParaSet[]   = {"������������"};  
  
  //��ʾ
  extern const char ls_OpFinal[]       = {"�����ɹ�!"};
#endif

//-----------------------------------���ұ�--------------------------------
//�Ӳ˵����ұ�ṹ
static const unsigned char _MasterSuper[] = //����ģʽ�����û�
{5,    0,1,2,3,4};
static const unsigned char _SlaveSuper[] = //�ӻ�ģʽ�����û�
{5,    5,0,1,2,6};

static const unsigned char _MasterAdmin[] = //����ģʽ����Ա
{2,    3,4};
static const unsigned char _SlaveAdmin[] = //�ӻ�ģʽ����Ա
{1,    5};

//����ģʽ��Ӧ�Ƿ�Ϊ����
extern unsigned short UsartDevCfgU0_cbIsMasterMask;

//----------------------------------�ص�����------------------------------- 
//����
static const struct _MNumDesc _Desc[] = {
  {MNUM_TYPE_DEC, 0, 15},//����ģʽ
  {MNUM_TYPE_DEC, 0, 3},//ͨѶЭ��
  {MNUM_TYPE_DEC, 0, 3},//��������
  {MNUM_TYPE_DEC, 0, 255},//֡���ʱ��
  {MNUM_TYPE_DEC, 10,255},//���յȴ�ʱ��
  {MNUM_TYPE_DEC, 1, 255},//�ӻ���ַ
  {MNUM_TYPE_DEC, 20,255},//·�ɵȴ�ʱ��
};

static void _Notify(unsigned char Type,//ͨ������
                    //���ݲ˵�����������������������
                    //������ṹ�ڿؼ��ﶨ��,ֱ��ָ��ת������
                    void *pv)
{
  struct _UsartDevCfg *pCfg = &UsartDevCfg[UsartDevCfgTMenu_cbGetId()];
  struct _MNumAdjUser *pUser = (struct _MNumAdjUser *)pv;
  
  switch(Type){
  case TM_NOTIFY_GET_DATA:{ //����ǰֵװ��
    pUser->Value[0] = pCfg->U.M.Cfg >> 4;
    pUser->Value[1] = (pCfg->U.M.Cfg >> 2) & 0x03;  
    pUser->Value[2] = pCfg->U.M.Cfg & 0x03;   
    pUser->Value[3] = pCfg->U.M.SpaceT;//֡���ʱ��
    pUser->Value[4] = pCfg->U.M.WaitT; //���յȴ�ʱ��
    pUser->Value[5] = pCfg->U.S.Adr;//�ӻ���ַ
    pUser->Value[6] = pCfg->U.S.WaitRoute; //·�ɵȴ�ʱ��
    break;
  }
  case TM_NOTIFY_SET_DATA:{ //�����趨ֵ
    pCfg->U.M.Cfg = (pUser->Value[0] << 4) |
                    (pUser->Value[1] << 2) | 
                    pUser->Value[2];
    if(pCfg->U.M.SpaceT != pUser->Value[3])
      pCfg->U.M.SpaceT = pUser->Value[3];//֡���ʱ��
    else pCfg->U.M.SpaceT = pUser->Value[5];//�ӻ���ַ
    
    if(pCfg->U.M.WaitT != pUser->Value[4])   
      pCfg->U.M.WaitT = pUser->Value[4]; //���յȴ�ʱ��
    else pCfg->U.S.WaitRoute = pUser->Value[6];//·�ɵȴ�ʱ�� 
    
    UsartDevCfg_Save(UsartDevCfgTMenu_cbGetId());
    UiTips_UpdateS(ls_OpFinal); //��ʾ�ɹ�
    break; 
  }
  case TM_NOTIFY_MNUM_GET_DESC:{ //�õ���ֵ����
    memcpy(&pUser->Desc,  &_Desc[pUser->CurItem], 
           sizeof(struct _MNumDesc));
    break;
  }
  case TM_NOTIFY_USER_GET_LUT:{ //�õ����ұ�
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

//��Ӧ���ַ�
const LanCode_t* const lsAry_U0[] = {
  ls_WorkMode, ls_Protocol, ls_ProtocolPara, 
  ls_SpaceT, ls_WaitT, ls_SlaveAdr, ls_WaitRouter};

//----------------------U0ģʽ�Ĳ˵��ṹ-------------------------
const TMenu_t UsartDevCfgU0_TMenu = {//�˵��ṹ
  TMTYPE_MNUMADJ | TM_MNUMADJ_WRITE, //�˵�����Ϊ��ֵ����ģʽģʽ���û�����־
  0x80 | 5,                          //�ɲ˵����;�����������ݴ�С
  ls_WorkParaSet,                    //�˵�ͷ,ΪNULLʱ�ӻص����ȡ
  USART_DEV_CFG_TMENU_PARENT,        //���ѵĸ��˵�
  lsAry_U0,                         //������ѵ��Ӳ˵���������ͷ
  _Notify,                           //���û��ռ佻����ͨ������
};






