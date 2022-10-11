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

#include "EepromInner.h"

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
{3,    5,1,2};

static const unsigned char _Para16Super[] = //Para��4bitΪ1������ʱ�����û�
{4,    0, 3, 4, 7};
static const unsigned char _Para16Admin[] = //Para��4bitΪ1������ʱ
{3,    3, 4, 7};

static const unsigned char _Para13Super[] = //Para��4bitΪ1+3bitģʽʱ
{5,    0, 3, 4, 8,9};
static const unsigned char _Para13Admin[] = //Para��4bitΪ1+3bitģʽʱ
{4,    3, 4, 8,9};

//�ɹ���ģʽ����״̬��1����+˫������ 2����+������, ����1+3bitģʽ, 0�������ӻ�
extern unsigned char UsartDevCfgU0_cbGetModeType(unsigned char Mode);

//----------------------------------�ص�����------------------------------- 
//����
static const struct _MNumDesc _Desc[] = {
  {MNUM_TYPE_DEC, 0, 15}, //0����ģʽ
  {MNUM_TYPE_DEC, 0, 3},  //1����ģʽ�µ�ͨѶЭ��(��4bitΪ2+2ģʽʱ��2bit)
  {MNUM_TYPE_DEC, 0, 3},  //2����ģʽ��Э���µ�����(��4bitΪ2+2ģʽʱ��2bit)
  {MNUM_TYPE_DEC, 1, 255},//3(����ģʽ)֡���ʱ��
  {MNUM_TYPE_DEC, 10,255},//4(����ģʽ)���յȴ�ʱ��
  {MNUM_TYPE_DEC, 1, 255},//5(�ӻ�ģʽ)�ӻ���ַ
  {MNUM_TYPE_DEC, 1, 255},//6(�ӻ�ģʽ)֡���ʱ��
  {MNUM_TYPE_DEC, 0, 15}, //7Para��4bitΪ1������ʱ
  {MNUM_TYPE_DEC, 0, 1},  //8Para��4bitΪ1+3bitģʽʱ
  {MNUM_TYPE_DEC, 0, 7},  //9Para��4bitΪ1+3bitģʽʱ  
};

static void _Notify(unsigned char Type,//ͨ������
                    //���ݲ˵�����������������������
                    //������ṹ�ڿؼ��ﶨ��,ֱ��ָ��ת������
                    void *pv)
{
  struct _UsartDevCfg *pCfg = &UsartDevCfg[UsartDevCfgTMenu_cbGetId()];
  struct _MNumAdjUser *pUser = (struct _MNumAdjUser *)pv;
  
  //׼�����ұ�
  const unsigned char *pLUT;
  unsigned char ModeType = UsartDevCfgU0_cbGetModeType(pCfg->U.M.Cfg >> 4);
  if(ModeType == 1){ //1����+˫����
    if(Power_IsAdminMoreUp()) pLUT = _MasterSuper;
    else pLUT = _MasterAdmin;
  }
  else if(ModeType == 2){ //����+������
    if(Power_IsAdminMoreUp()) pLUT = _Para16Super;
    else pLUT = _Para16Admin;
  } 
  else if(ModeType == 3){ //����1+3bitģʽ
    if(Power_IsAdminMoreUp()) pLUT = _Para13Super;
    else pLUT = _Para13Admin;
  }   
  else{//�ӻ�
    if(Power_IsAdminMoreUp()) pLUT = _SlaveSuper;
    else pLUT = _SlaveAdmin;
  }
  
  switch(Type){
  case TM_NOTIFY_GET_DATA:{ //����ǰֵװ��
    unsigned char LutCount = *pLUT++;
    for(unsigned char Pos = 0; Pos < LutCount; Pos++){
      unsigned char Data = 0; //��õ�ǰֵ
      switch(pLUT[Pos]){
        case 0: Data  = pCfg->U.M.Cfg >> 4;  break; //����ģʽ
        case 1: Data = (pCfg->U.M.Cfg >> 2) & 0x03; break;//����ģʽ�µ�ͨѶЭ��
        case 2: Data = pCfg->U.M.Cfg & 0x03;  break; //����ģʽ��Э���µ�����(������ͬ)
        
        case 3: Data = pCfg->U.M.SpaceT;  break;//(����ģʽ)֡���ʱ��
        case 4: Data = pCfg->U.M.WaitT;  break;//(����ģʽ)���յȴ�ʱ��
        case 5: Data = pCfg->U.S.Adr;  break;//(�ӻ�ģʽ)�ӻ���ַ
        case 6: Data = pCfg->U.S.SpaceT;  break;//(�ӻ�ģʽ)���ݼ��
        case 7: Data = pCfg->U.M.Cfg & 0x0F;  break;//Para��4bitΪ1������ʱ
        case 8: Data = (pCfg->U.M.Cfg & 0x08)? 1 :0 ;  break;//1+3bitģʽʱ
        case 9: Data = pCfg->U.M.Cfg & 0x07;  break;//Para��4bitΪ1+3bitģʽʱ        
      }
      pUser->Value[Pos] = Data;
    }
    break;
  }
  case TM_NOTIFY_SET_DATA:{ //�����趨ֵ
    unsigned char PrvUMode = pCfg->U.M.Cfg & USART_DEV_UMODE_MASK;
    unsigned char LutCount = *pLUT++;
    for(unsigned char Pos = 0; Pos < LutCount; Pos++){
      unsigned char Data = pUser->Value[Pos]; //��õ�ǰֵ
      switch(pLUT[Pos]){
        case 0: //����ģʽ        
          pCfg->U.M.Cfg &= ~USART_DEV_UMODE_MASK;
          pCfg->U.M.Cfg |= Data << 4; break;        
        case 1://����ģʽ�µ�ͨѶЭ��
          pCfg->U.M.Cfg &= ~USART_DEV_UPROTOCOL_MASK;
          pCfg->U.M.Cfg |= Data << 2; break;
        case 2://����ģʽ��Э���µ�����
          pCfg->U.M.Cfg &= ~USART_DEV_UPARA_MASK;
          pCfg->U.M.Cfg |= Data; break;          
        case 3: pCfg->U.M.SpaceT = Data;  break;//(����ģʽ)֡���ʱ��
        case 4: pCfg->U.M.WaitT = Data;  break;//(����ģʽ)���յȴ�ʱ��
        case 5: pCfg->U.S.Adr = Data;  break;//(�ӻ�ģʽ)�ӻ���ַ
        case 6: pCfg->U.S.SpaceT = Data;  break;//(�ӻ�ģʽ)���ݼ��  
        case 7: //Para��4bitΪ1������ʱ 
          pCfg->U.M.Cfg &= ~0x0F;  
          pCfg->U.M.Cfg |= Data;           
          break;
        case 8: //��4bitΪ1+3bitģʽʱ
          if(Data) pCfg->U.M.Cfg |= 0x08;
          else pCfg->U.M.Cfg &= ~0x08;
          break;          
        case 9: //��4bitΪ1+3bitģʽʱ
          pCfg->U.M.Cfg &= ~0x07;  
          pCfg->U.M.Cfg |= Data;           
          break;
      }
    }
    UsartDevCfg_Save(UsartDevCfgTMenu_cbGetId());
    UiTips_UpdateS(ls_OpFinal); //��ʾ�ɹ�
    //ģʽ��ת������Ҫ���·������ͷֱ�ҽӣ�����Ҫ��д����
    if(PrvUMode != (pCfg->U.M.Cfg & USART_DEV_UMODE_MASK)){
      Eeprom_ForceWrBufAndRestart();
    }
    break; 
  }
  case TM_NOTIFY_MNUM_GET_DESC:{ //�õ���ֵ����
    memcpy(&pUser->Desc,  &_Desc[pLUT[pUser->CurItem + 1]], 
           sizeof(struct _MNumDesc));
    break;
  }
  case TM_NOTIFY_USER_GET_LUT:{ //�õ����ұ�
    memcpy(pv, pLUT, *pLUT + 1);
    break;
  }
  default:break;
  }
}

//��Ӧ���ַ�
const LanCode_t* const lsAry_U0[] = {
  ls_WorkMode, ls_Protocol, ls_ProtocolPara, 
  ls_SpaceT, ls_WaitT, ls_SlaveAdr, ls_WaitRouter, 
  ls_UsartPara16, ls_UsartPara1, ls_UsartPara7};

//----------------------U0ģʽ�Ĳ˵��ṹ-------------------------
const TMenu_t UsartDevCfgU0_TMenu = {//�˵��ṹ
  TMTYPE_MNUMADJ | TM_MNUMADJ_WRITE, //�˵�����Ϊ��ֵ����ģʽģʽ���û�����־
  0x80 | 10,                          //�ɲ˵����;�����������ݴ�С
  ls_WorkParaSet,                    //�˵�ͷ,ΪNULLʱ�ӻص����ȡ
  USART_DEV_CFG_TMENU_PARENT,        //���ѵĸ��˵�
  lsAry_U0,                         //������ѵ��Ӳ˵���������ͷ
  _Notify,                           //���û��ռ佻����ͨ������
};






