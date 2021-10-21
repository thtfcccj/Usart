/******************************************************************************

                   Usart�豸����ͨ�ýӿ�
//���豸֧��˫��ͨѶ
//���豸ʵ��ʱ,��Ϊ�Զ�ģʽ,���ڴ�DMA��PDC�������н����øù���,�����ֹ
//���豸������Եײ�Ӳ��������,�����CPU���Ͳ�ͬ����ʵ�ֻ�ֱ�����
//ע:�ڽ�����,���ײ�Ӳ��֧��DMA��PDC,�����������ʱֱ�ӽ���,��������������ʱ����
******************************************************************************/
#ifndef __USART_DEV_H
#define __USART_DEV_H

/******************************************************************************
                             �������
******************************************************************************/


/******************************************************************************
                             ��ؽṹ
******************************************************************************/

//-----------------------�����жϴ���������--------------------------------
//�β�Ϊ��struct _UsartDevָ��
//��״̬����Ϊ:0:�����շ�,����:ֹͣ�շ�
typedef signed char (*UsartDevInt_t)(void*);

//---------------------------Usart�豸����-------------------------------------
//Spi�豸��ʾһ������SpiӲ���ӿڣ�һ��SpiӲ���ӿ���������Spi�豸(��Ƭѡ����),
struct _UsartDev{
  void *pUsartHw;                      //�ҽӵ�Ӳ���豸
  void *pVoid;                         //�ص�����������Ҫ��ָ��
  //���ջ�����
  unsigned char *pRcvBuf;              //���ջ�����
  unsigned short RcvCount;             //���ջ�������С
  unsigned short RcvLen;               //�ѽ��ո���
  UsartDevInt_t  RcvEndInt;            //���ʹ�����
  //���ͻ�����
  const unsigned char *pSendBuf;       //���ͻ�����
  unsigned short SendCount;            //���ͻ�������С
  unsigned short SenLen;               //�ѷ��͸���
  UsartDevInt_t  SendEndInt;           //���ʹ�����
  //����:
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
void UsartDev_Init(struct _UsartDev *pDev,
                   void * pUsartHw  //�ҽӵ�Ӳ��
                   /*,void *pVoid*/);      //�ص�����������Ҫ��ָ��

//-------------------------�ƴ�������ú���-------------------------------
//�������ò���,ͬʱ����ʵ���ϵĲ�����
//�˺�������λ����Usart�ĵ�ǰ����
//void UsartDev_ReCfg(struct _UsartDev *pDev,
//                   struct _UsartDevCfg *pCfg);

//---------------------Usart��ʼ�������ݺ���--------------------------------
//����USART��������,�����Ƿ������ɹ�
//���������Ƿ�ɹ�,0:�ɹ�,����ʧ��
//ע:���ջ�������С���λ��1��ʾ���Զ�ģʽ
signed char UsartDev_RcvStart(struct _UsartDev *pDev,  //�����豸
                              unsigned char *pBuf,     //���ջ�����
                              unsigned short Size,     //���ջ�������С
                              UsartDevInt_t RcvEndInt);//���ջص�����

//--------------------Usart��������ǿ��ֹͣ---------------------------
//ֹͣ���ڶ������е����ݻ�ѽ�����Ϊֹͣ״̬
//�����ѽ��յ������ݸ���
unsigned short UsartDev_RcvStop(struct _UsartDev *pDev);

//--------------------Usar����������������--------------------------------
//����USART�н�������,�����Ƿ������ɹ�
//���������Ƿ�ɹ�,0:�ɹ�,����ʧ��
//ע:���ͻ�������С���λ��1��ʾ���Զ�ģʽ
signed char UsartDev_SendStart(struct _UsartDev *pDev,      //�����豸
                               const unsigned char *pBuf,   //���ͻ�����
                               unsigned short Size,         //���ͻ�������С
                               UsartDevInt_t SendEndInt);    //���ͻص�����

//--------------------Usart��������ֹͣ����---------------------------
//ֹͣ����д�����е����ݻ�ѷ�����Ϊֹͣ״̬
//�����ѷ��͵������ݸ���
unsigned short UsartDev_SendStop(struct _UsartDev *pDev);

//--------------------�õ��ѽ������ݴ�С---------------------------
//unsigned short UsartDev_GetRcvLen(struct _UsartDev *pDev);
#define UsartDev_GetRcvLen(pdev) ((pdev)->RcvLen)

//--------------------�õ��ѷ������ݴ�С---------------------------
//unsigned short UsartDev_GetSendLen(struct _UsartDev *pDev);
#define UsartDev_GetSendLen(pdev) ((pdev)->SendLen)

//---------------------USARTӲ�������жϴ�����----------------------------
void UsartDev_RcvIRQ(struct _UsartDev *pDev);



#endif

