/***************************************************************************

      UsartDev�е�struct _UsartDevCfg,-��ʹ��TMenu�˵�ϵͳʱ��ʵ��
                         
****************************************************************************/

#include "UsartDevCfg_TMenu.h"
#include "TMenu_MNumAdj.h"
#include "UiTips.h"

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
  static const char ls_Baudrate[]      = {"������"};
  static const char ls_ParityMode[]    = {"У��λ[0��][1��][2ż]"};
  static const char ls_StopBit[]       = {"ֹͣλ[0:1λ][1:2λ]"};
  static const char ls_DataBit[]       = {"����λ[0:8λ][1:7λ]"};
  static const char ls_ComParaSet[]    = {"ͨѶ��������"};  
  //��ʾ
  extern const char ls_OpFinal[]       = {"�����ɹ�!"};
#endif

//--------------------------����ģʽʱ,�ɵ�ǰ����ת������-----------------------
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

//----------------------------------�ص�����------------------------------- 
//����
static const struct _MNumDesc _UsartDevCfgDesc[] = {
  {MNUM_TYPE_DEC, 1200, 115200},//������
  {MNUM_TYPE_DEC, 0, 2},//У��λ
  {MNUM_TYPE_DEC, 0, 1},//ֹͣλ
  {MNUM_TYPE_DEC, 0, 1},//����λ
};

static void _Notify(unsigned char Type,//ͨ������
                    //���ݲ˵�����������������������
                    //������ṹ�ڿؼ��ﶨ��,ֱ��ָ��ת������
                    void *pv)
{
  struct _MNumAdjUser *pUser = (struct _MNumAdjUser *)pv;
  struct _UsartDevCfg *pCfg = &UsartDevCfg[UsartDevCfgTMenu_cbGetId()];
  switch(Type){
  case TM_NOTIFY_GET_DATA:{ //����ǰֵװ��
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
  case TM_NOTIFY_SET_DATA:{ //�����趨ֵ
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
    UiTips_UpdateS(ls_OpFinal); //��ʾ�ɹ�
    break; 
  }
  case TM_NOTIFY_MNUM_GET_DESC:{ //�õ���ֵ����
    memcpy(&pUser->Desc,  &_UsartDevCfgDesc[pUser->CurItem], 
           sizeof(struct _MNumDesc));
    break;
  }
  default:break;
  }
}

//��Ӧ���ַ�
const LanCode_t* const lsAry_UsartDevCfg[] = {
  ls_Baudrate, ls_ParityMode, ls_StopBit, ls_DataBit};

//--------------------------����˵��ṹ----------------------------

const TMenu_t UsartDevCfg_TMenu = {//�˵��ṹ
  TMTYPE_MNUMADJ | TM_MNUMADJ_WRITE, //�˵�����Ϊ��ֵ����ģʽģʽ���û�����־
  4,                                 //�ɲ˵����;�����������ݴ�С
  ls_ComParaSet,                     //�˵�ͷ,ΪNULLʱ�ӻص����ȡ
  USART_DEV_CFG_TMENU_PARENT,        //���ѵĸ��˵�
  lsAry_UsartDevCfg,                 //������ѵ��Ӳ˵���������ͷ
  _Notify,                           //���û��ռ佻����ͨ������
};


