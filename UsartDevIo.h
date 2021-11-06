/******************************************************************************

                   Usart�豸����ͨ�ýӿ�-����IOʵ��ʱ
�˽ӿ�����ʵ����IOģ��UARTͨѶ��ͬʱ��ʹ��MCU�ڲ�UARTӲ�������ʱʹ��
//    �˽ӿڽ�����UsartDevIo������IO��׺��������ɲ���ʹ�ã���ϵͳ��û��ʹ��--
//UsartDevIo.h�ӿڣ�����ֱ��ʹ�øýӿ����ṩ�������ԡ�
//���豸������Եײ�Ӳ��������,�����CPU���Ͳ�ͬ����ʵ�ֻ�ֱ�����

//ע����ģ���е���Ҫ����������UsartDev.h�е���ͬ(ֻ�Ǽ��˺�׺IO)�������ڶ�̬
******************************************************************************/
#ifndef __USART_DEV_IO_H
#define __USART_DEV_IO_H
#ifdef SUPPORT_EX_PREINCLUDE//��֧��Preinlude�r
  #include "Preinclude.h"
#endif

#include "UsartDev.h" //ʹ����ͬ�����ݽṹ

/******************************************************************************
                             ��غ���
******************************************************************************/

//-------------------------��ʼ������---------------------------------------
void UsartDevIo_Init(struct _UsartDev *pDev,
                   void * pUsartHw  //�ҽӵ�Ӳ��
                   /*,void *pVoid*/);      //�ص�����������Ҫ��ָ��
          
//-------------------------�ƴ�������ú���-------------------------------
//�������ò���,ͬʱ����ʵ���ϵĲ�����
//�˺�������λ����Usart�ĵ�ǰ����
//void UsartDevIo_ReCfg(struct _UsartDev *pDev,
//                   struct _UsartDevCfg *pCfg);

//---------------------Usart��ʼ�������ݺ���--------------------------------
//����USART��������,�����Ƿ������ɹ�
//���������Ƿ�ɹ�,0:�ɹ�,����ʧ��
//ע:���ջ�������С���λ��1��ʾ���Զ�ģʽ
signed char UsartDevIo_RcvStart(struct _UsartDev *pDev,  //�����豸
                              unsigned char *pBuf,     //���ջ�����
                              unsigned short Size,     //���ջ�������С
                              UsartDevInt_t RcvEndInt);//���ջص�����

//--------------------Usart��������ǿ��ֹͣ---------------------------
//ֹͣ���ڶ������е����ݻ�ѽ�����Ϊֹͣ״̬
//�����ѽ��յ������ݸ���
unsigned short UsartDevIo_RcvStop(struct _UsartDev *pDev);

//--------------------Usar����������������--------------------------------
//����USART�н�������,�����Ƿ������ɹ�
//���������Ƿ�ɹ�,0:�ɹ�,����ʧ��
//ע:���ͻ�������С���λ��1��ʾ���Զ�ģʽ
signed char UsartDevIo_SendStart(struct _UsartDev *pDev,      //�����豸
                               const unsigned char *pBuf,   //���ͻ�����
                               unsigned short Size,         //���ͻ�������С
                               UsartDevInt_t SendEndInt);    //���ͻص�����

//--------------------Usart��������ֹͣ����---------------------------
//ֹͣ����д�����е����ݻ�ѷ�����Ϊֹͣ״̬
//�����ѷ��͵������ݸ���
unsigned short UsartDevIo_SendStop(struct _UsartDev *pDev);

//--------------------�õ��ѽ������ݴ�С---------------------------
//unsigned short UsartDevIo_GetRcvLen(struct _UsartDev *pDev);
#define UsartDevIo_GetRcvLen(pdev) ((pdev)->RcvLen)

//--------------------�õ��ѷ������ݴ�С---------------------------
//unsigned short UsartDevIo_GetSendLen(struct _UsartDev *pDev);
#define UsartDevIo_GetSendLen(pdev) ((pdev)->SendLen)

//---------------------USARTӲ�������жϴ�����--------------------
void UsartDevIo_RcvIRQ(struct _UsartDev *pDev);



#endif

