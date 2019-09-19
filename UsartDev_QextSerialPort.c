/******************************************************************************

                   Usart�豸������PCƽ̨qextseroal�е�ʵ��
//��ģ��֧�ֽӿ��ṩ�����й���
******************************************************************************/

#include "UsartDev.h"
#include <string.h>

//------------------------�ӿں�������---------------------------------------
//�򿪴��ں���,���ش��Ƿ�ɹ�
//�˺���ͬʱ����������
extern int Win_QextSerialPort_open(struct Win_QextSerialPort *pSerial);

//�رմ��ں���
 extern void Win_QextSerialPort_close(struct Win_QextSerialPort *pSerial);

//���������ݺ���
//���ض����ݸ��������-1;
 extern int Win_QextSerialPort_readData(struct Win_QextSerialPort *pSerial,
                                              char *data, int maxSize);

//д�������ݺ���
//����д���ݸ��������-1;
 extern int Win_QextSerialPort_writeData(struct Win_QextSerialPort *pSerial,
                                              char *data, int maxSize);

//-------------------------��ʼ������---------------------------------------
void UsartDev_Init(struct _UsartDev *pDev,
                   void * pUsartHw,  //�ҽӵ�Ӳ��
                   void *pVoid)      //�ص�����������Ҫ��ָ��
{
  memset(pDev, 0, sizeof(struct _UsartDev));
  pDev->pUsartHw = pUsartHw;
   pDev->pVoid = pVoid;
  //�رմ���
  //Win_QextSerialPort_close(pUsartHw);
}

//---------------------Usart��ʼ�������ݺ���--------------------------------
//����USART��������,�����Ƿ������ɹ�
//���������Ƿ�ɹ�,0:�ɹ�,����ʧ��
//ע:���ջ�������С���λ��1��ʾ���Զ�ģʽ
signed char UsartDev_RcvStart(struct _UsartDev *pDev,  //�����豸
                              unsigned char *pBuf,     //���ջ�����
                              unsigned short Size,     //���ջ�������С
                              UsartDevInt_t RcvEndInt)//���ջص�����
{
  //if(pDev->Flag & USART_DEV_RCV_BUSY) return -1;  //�����ý����أ�
  pDev->Flag |= USART_DEV_RCV_BUSY;
  pDev->Flag &= ~(USART_DEV_RCV_BUF_OV | //��ط��ͱ�־��ʼ��
                  USART_DEV_RCV_ERR | USART_DEV_RCV_AUTO);
  pDev->pRcvBuf = pBuf;
  pDev->RcvEndInt = RcvEndInt;
  if(Size & 0x8000)
    pDev->Flag |= USART_DEV_RCV_AUTO;
  pDev->RcvCount = Size & ~0x8000;
  pDev->RcvLen = 0;
  return 0;
}

//--------------------Usart��������ǿ��ֹͣ---------------------------
//ֹͣ���ڶ������е����ݻ�ѽ�����Ϊֹͣ״̬
//�����ѽ��յ������ݸ���
unsigned short UsartDev_RcvStop(struct _UsartDev *pDev)
{
  pDev->Flag &= ~USART_DEV_RCV_BUSY;//�����ֹͣ��־
  return pDev->RcvLen;
}

//--------------------Usar����������������--------------------------------
//����USART�н�������,�����Ƿ������ɹ�
//���������Ƿ�ɹ�,0:�ɹ�,����ʧ��
//ע:���ͻ�������С���λ��1��ʾ���Զ�ģʽ
signed char UsartDev_SendStart(struct _UsartDev *pDev,      //�����豸
                               unsigned char *pBuf,         //���ͻ�����
                               unsigned short Size,         //���ͻ�������С
                               UsartDevInt_t SendEndInt) //���ͻص�����
{
  int Resume;
  //if(pDev->Flag & USART_DEV_SEND_BUSY) return -1;  //�����÷����أ�
  pDev->Flag |= USART_DEV_SEND_BUSY;
  pDev->Flag &= ~(USART_DEV_SEND_ERR | //��ط��ͱ�־��ʼ��
                  USART_DEV_SEND_AUTO);
  pDev->pSendBuf = pBuf;
  pDev->SendEndInt = SendEndInt;
  if(Size & 0x8000)//�Զ�ģʽ
    pDev->Flag |= USART_DEV_SEND_AUTO;
  pDev->SendCount = Size & ~0x8000;
  

  //��ʼ����,����Ϊ������ʽ����
  Resume = Win_QextSerialPort_writeData(pDev->pUsartHw,pBuf,pDev->SendCount);
  //if(Resume < 0) return -1;//д����
  pDev->SenLen = Resume;
  pDev->SendEndInt(pDev);   //ͨ�����

  return Resume;
}

//--------------------Usart��������ֹͣ����---------------------------
//ֹͣ����д�����е����ݻ�ѷ�����Ϊֹͣ״̬
//�����ѷ��͵������ݸ���
unsigned short UsartDev_SendStop(struct _UsartDev *pDev)
{
  //ֹͣǰ�ѽ���,����ֹͣ
  return pDev->SenLen;
}

//---------------------USARTӲ�������жϴ�����----------------------------
void UsartDev_RcvIRQ(struct _UsartDev *pDev)
{
  int Resume;
  unsigned short RcvLen = pDev->RcvLen;
  unsigned short RcvCount;

  //���ڽ��չ�����ʱ�����յ�������ֱ�Ӷ���
  if(!(pDev->Flag & USART_DEV_RCV_BUSY)){
    unsigned char DumpBuf[16];
    int i = 0;
    for(; i < 100; i++){
        if(Win_QextSerialPort_readData(pDev->pUsartHw,DumpBuf,16) <= 0) break;
    };
    return;
  }


  //�Զ���������ʱ����ȫ��������������
  //if(pDev->Flag & USART_DEV_RCV_AUTO)
    RcvCount = pDev->RcvCount - RcvLen;
  //else RcvCount = 1;//һ������

  Resume = Win_QextSerialPort_readData(pDev->pUsartHw,
                                           pDev->pRcvBuf + RcvLen,
                                           RcvCount);
  if(Resume < 0){//�쳣����
      pDev->Flag |=  USART_DEV_RCV_ERR;
      RcvCount = 0;
  }
  else{//��������
      RcvLen += Resume;
      RcvCount = Resume;
  }

  //������������,���ʱ�������Ҳ�����ˣ�
  if((RcvLen >= pDev->RcvCount) || (pDev->Flag & USART_DEV_RCV_ERR)){
    //��������ǿ�����
    pDev->Flag |= USART_DEV_RCV_ERR;
    if(pDev->RcvEndInt(pDev))//��������ǿ�����(��������)
        UsartDev_RcvStop(pDev);
    return;
  }

  //�Զ�ʱ
  if(pDev->Flag & USART_DEV_RCV_AUTO){
    pDev->RcvLen = RcvLen;
    if(pDev->RcvEndInt(pDev))//�ֹ�ֹͣ��
      UsartDev_RcvStop(pDev);
  }
  else{//�ֹ���ʱģ�ⵥ�����ݽ���
    for( ; RcvCount > 0; RcvCount--){
      pDev->RcvLen++;
      if(pDev->RcvEndInt(pDev)){//�ֹ�ֹͣ��
        UsartDev_RcvStop(pDev);
      }
    }
  } //end for

}



