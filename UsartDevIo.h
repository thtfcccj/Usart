/******************************************************************************

                   Usart�豸����ͨ�ýӿ�-����IOʵ��ʱ
�˽ӿ�����ʵ����IOģ��UARTͨѶ��ͬʱ��ʹ��MCU�ڲ�UARTӲ�������ʱʹ��
//    �˽ӿڽ�����UsartDevIo������IO��׺��������ɲ���ʹ�ã���ϵͳ��û��ʹ��--
//UsartDevIo.h�ӿڣ�����ֱ��ʹ�øýӿ����ṩ�������ԡ�
//���豸������Եײ�Ӳ��������,�����CPU���Ͳ�ͬ����ʵ�ֻ�ֱ�����
******************************************************************************/
#ifndef __USART_DEV_IO_H
#define __USART_DEV_IO_H
#ifdef SUPPORT_EX_PREINCLUDE//��֧��Preinlude�r
  #include "Preinclude.h"
#endif

/******************************************************************************
                             �������
******************************************************************************/


/******************************************************************************
                             ��ؽṹ
******************************************************************************/

//-----------------------�����жϴ���������--------------------------------
//�β�Ϊ��struct _UsartDevIoָ��
//��״̬����Ϊ:0:�����շ�,����:ֹͣ�շ�
typedef signed char (*UsartDevIoInt_t)(void*);

//---------------------------Usart�豸����-------------------------------------
//Spi�豸��ʾһ������SpiӲ���ӿڣ�һ��SpiӲ���ӿ���������Spi�豸(��Ƭѡ����),
struct _UsartDevIo{
  void *pUsartHw;                      //�ҽӵ�Ӳ���豸
  void *pVoid;                         //�ص�����������Ҫ��ָ��
  //���ջ�����
  unsigned char *pRcvBuf;              //���ջ�����
  unsigned short RcvCount;             //���ջ�������С
  unsigned short RcvLen;               //�ѽ��ո���
  UsartDevIoInt_t  RcvEndInt;            //���ʹ�����
  //���ͻ�����
  const unsigned char *pSendBuf;       //���ͻ�����
  unsigned short SendCount;            //���ͻ�������С
  unsigned short SenLen;               //�ѷ��͸���
  UsartDevIoInt_t  SendEndInt;           //���ʹ�����
  //����:
  unsigned char UsartId;              //�ص�����������Ҫ���豸ID
  volatile unsigned char RcvData;      //�������һ�ν��յ�����
	volatile unsigned char Flag;         //��ر�־��������
};

//��ر�־����Ϊ��
#define USART_DEV_SEND_BUSY      0x01  //����æ����ʾ���ڷ��͹�����
#define USART_DEV_RCV_BUSY       0x02  //����æ����ʾ���ڽ��չ�����
#define USART_DEV_RCV_BUF_OV     0x04  //���ջ���������־

#define USART_DEV_SEND_ERR       0x10  //���͹������д���
#define USART_DEV_RCV_ERR        0x20  //���չ������д���
#define USART_DEV_SEND_AUTO      0x40 //�Զ�ģʽ��ʾ������ɵ����жϻص�,���򵥸����ݷ��ͼ��ж�
#define USART_DEV_RCV_AUTO       0x80 //�Զ�ģʽ��ʾ������ɵ����жϻص�,���򵥸����ݽ��ռ��ж�


/******************************************************************************
                             ��غ���
******************************************************************************/

//-------------------------��ʼ������---------------------------------------
void UsartDevIo_Init(struct _UsartDevIo *pDev,
                   void * pUsartHw  //�ҽӵ�Ӳ��
                   /*,void *pVoid*/);      //�ص�����������Ҫ��ָ��
          
//-------------------------�ƴ�������ú���-------------------------------
//�������ò���,ͬʱ����ʵ���ϵĲ�����
//�˺�������λ����Usart�ĵ�ǰ����
//void UsartDevIo_ReCfg(struct _UsartDevIo *pDev,
//                   struct _UsartDevIoCfg *pCfg);

//---------------------Usart��ʼ�������ݺ���--------------------------------
//����USART��������,�����Ƿ������ɹ�
//���������Ƿ�ɹ�,0:�ɹ�,����ʧ��
//ע:���ջ�������С���λ��1��ʾ���Զ�ģʽ
signed char UsartDevIo_RcvStart(struct _UsartDevIo *pDev,  //�����豸
                              unsigned char *pBuf,     //���ջ�����
                              unsigned short Size,     //���ջ�������С
                              UsartDevIoInt_t RcvEndInt);//���ջص�����

//--------------------Usart��������ǿ��ֹͣ---------------------------
//ֹͣ���ڶ������е����ݻ�ѽ�����Ϊֹͣ״̬
//�����ѽ��յ������ݸ���
unsigned short UsartDevIo_RcvStop(struct _UsartDevIo *pDev);

//--------------------Usar����������������--------------------------------
//����USART�н�������,�����Ƿ������ɹ�
//���������Ƿ�ɹ�,0:�ɹ�,����ʧ��
//ע:���ͻ�������С���λ��1��ʾ���Զ�ģʽ
signed char UsartDevIo_SendStart(struct _UsartDevIo *pDev,      //�����豸
                               const unsigned char *pBuf,   //���ͻ�����
                               unsigned short Size,         //���ͻ�������С
                               UsartDevIoInt_t SendEndInt);    //���ͻص�����

//--------------------Usart��������ֹͣ����---------------------------
//ֹͣ����д�����е����ݻ�ѷ�����Ϊֹͣ״̬
//�����ѷ��͵������ݸ���
unsigned short UsartDevIo_SendStop(struct _UsartDevIo *pDev);

//--------------------�õ��ѽ������ݴ�С---------------------------
//unsigned short UsartDevIo_GetRcvLen(struct _UsartDevIo *pDev);
#define UsartDevIo_GetRcvLen(pdev) ((pdev)->RcvLen)

//--------------------�õ��ѷ������ݴ�С---------------------------
//unsigned short UsartDevIo_GetSendLen(struct _UsartDevIo *pDev);
#define UsartDevIo_GetSendLen(pdev) ((pdev)->SendLen)

//---------------------USARTӲ�������жϴ�����--------------------
void UsartDevIo_RcvIRQ(struct _UsartDevIo *pDev);



#endif

