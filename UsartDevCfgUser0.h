/******************************************************************************

           UsartDevCfg��չ�û�����->��ʽ0
��Ҫ�������ӻ�ʱ�����Ƶ�ַ����ʱ���
******************************************************************************/
#ifndef __USART_DEV_CFG_USER0_H
#define __USART_DEV_CFG_USER0_H
#ifdef SUPPORT_EX_PREINCLUDE//��֧��Preinlude�r
  #include "Preinclude.h"
#endif

//----------------------------�û��ṹ����--------------------------------------
typedef union _UsartDevCfgUser0{
  struct{
    unsigned char WaitT;    //�������ݺ�ĵȴ�ʱ��,msΪ��λ
    unsigned char SpaceT;   //����֡���ʱ��,msΪ��λ
    unsigned char Cfg;      //����ʱ���ã���ͨѶ�����
  }M; //����ģʽʱ
  struct{
    unsigned char Adr;        //�ӻ���ַ
    unsigned char WaitRoute;  //�豸����ת����ʱ���ȴ��¼��豸����ʱ��
    unsigned char Cfg;        //�ӻ�ʱ���ã���ͨѶ�����
  }S; //�ӻ�ģʽʱ
}UsartDevCfgUser0_t;


#endif

