/******************************************************************************

                UsartDevģ��-��̬����(polymorphic type)ͳһ�ӿ�
//�˽ӿ���Ҫ����ͬһ�����Ŀ�У�������MCU�ڲ�UsartDev��IOģ��UsartDevIO����ʱ
//Ϊ�ϲ�Ӧ���ṩͳһ�ӿڵĴ���ʽ
******************************************************************************/
#ifndef __USART_DEV_PT_H
#define __USART_DEV_PT_H
#ifdef SUPPORT_EX_PREINCLUDE//��֧��Preinlude�r
  #include "Preinclude.h"
#endif

/******************************************************************************
                             ��ؽṹ����
******************************************************************************/

//��̬�����ڲ��������Ͷ��壬�Է��㲻ͬ��ִ�ж�������ǿ��ת��:
//������������
typedef signed char(*UsartDevPt_RcvStart_t)(void *pDev,  //ִ�ж���
                    unsigned char *pBuf,     //���ջ�����
                    unsigned short Size,     //���ջ�������С
                    signed char (*UsartDevInt_t)(void*));//���ջص�����
//����ֹͣ
typedef unsigned short (*UsartDevPt_RcvStop_t)(void *pDev);  //ִ�ж���
//��������    
typedef signed char (*UsartDevPt_SendStart_t)(void *pDev,  //ִ�ж���
                     const unsigned char *pBuf,   //���ͻ�����
                     unsigned short Size,         //���ͻ�������С
                     signed char (*UsartDevInt_t)(void*));//���ͻص�����
//����ֹͣ
typedef unsigned short (*UsartDevPt_SendStop_t)(void *pDev);  //ִ�ж���

//��̬���Ͷ��壬����ֻ���շ�
struct _UsartDevPt{
  UsartDevPt_RcvStart_t RcvStart;
  UsartDevPt_RcvStop_t RcvStop;
  UsartDevPt_SendStart_t SendStart;
  UsartDevPt_SendStop_t SendStop;
};

/******************************************************************************
                             ���ʵ������
******************************************************************************/
extern const struct _UsartDevPt  UsartDevPt_Hw; //UsartDev��̬����Ӳ���ӿ�

#ifdef SUPPORT_USART_DEV_IO    //����IOʵ��ʱ
  extern const struct _UsartDevPt  UsartDevPt_Io; //UsartDev��̬����IOģ��ӿ�
#endif

#endif

