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
    unsigned char Cfg;        //�������,������
    unsigned char SpaceT;   //����֡���ʱ��,msΪ��λ    
    unsigned char WaitT;    //�������ݺ�ĵȴ�ʱ��,msΪ��λ
  }M; //����ģʽʱ
  struct{
    unsigned char Cfg;        //������ã�������
    unsigned char Adr;        //�ӻ���ַ
    unsigned char WaitRoute;  //�豸����ת����ʱ���ȴ��¼��豸����ʱ��
  }S; //�ӻ�ģʽʱ
}UsartDevCfgUser0_t;

//-------------------------���ӻ�ģʽ��Cfg����----------------------------------
//����ģʽ��
#define USART_DEV_UMODE_SHIRT      4     
#define USART_DEV_UMODE_MASK       0xf0
#define USART_DEV_UMODE_SLAVE      0     //�ӻ�ģʽ
#define USART_DEV_UMODE_MASTER     1     //����ģʽ
                                     //����������,�ӻ�,��ӡ����WIFI��,���û�����

//����ģʽ��ӦͨѶЭ��,��Modbus,͸����,�������û�����
#define USART_DEV_UPROTOCOL_SHIRT  2
#define USART_DEV_UPROTOCOL_MASK   0xC0 

//����ģʽ��Э���µ�����(��MODBUS�µ�RTU��ASC)
#define USART_DEV_UPARA_MASK       0x03 

#endif

