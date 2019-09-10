/******************************************************************************

                   Usart�豸������ATMEGA�е�ʵ��
//��ģ��֧�ֽӿ��ṩ�����й���
******************************************************************************/

#include "UsartDev.h"
#include <string.h>
#include "UsartHw_ATMEGA.h"

//-------------------------��ʼ������---------------------------------------
void UsartDev_Init(struct _UsartDev *pDev,
                   void * pUsartHw)  //�ҽӵ�Ӳ��
{
  memset(pDev, 0, sizeof(struct _UsartDev));
  pDev->pUsartHw = pUsartHw;
  struct _USART_HW *pHw = (struct _USART_HW *)pUsartHw;
  //�رմ���,���ж�ʹ��
  pHw->UCSRB &= ~((1 << TXEN) | (1 << RXEN) |
                 (1 << RXCIE) | (1 << TXCIE) | (1 << UDRIE));
}

//---------------------Usart��ʼ�������ݺ���--------------------------------
//����USART��������,�����Ƿ������ɹ�
//���������Ƿ�ɹ�,0:�ɹ�,����ʧ��
//ע:���ջ�������С���λ��1��ʾ���Զ�ģʽ
signed char UsartDev_RcvStart(struct _UsartDev *pDev,  //�����豸
                              unsigned char *pBuf,     //���ջ�����
                              UsartSize_t Size,     //���ջ�������С
                              UsartDevInt_t RcvEndInt)//���ջص�����
{
  //if(pDev->Flag & USART_DEV_RCV_BUSY) return -1;  //�����ý����أ�
  pDev->Flag |= USART_DEV_RCV_BUSY;
  pDev->Flag &= ~(USART_DEV_RCV_BUF_OV | //��ط��ͱ�־��ʼ��
                  USART_DEV_RCV_ERR | USART_DEV_RCV_AUTO);
  pDev->pRcvBuf = pBuf;
  pDev->RcvEndInt = RcvEndInt;
  if(Size & USART_DEV_SIZE_AUTO)
    pDev->Flag |= USART_DEV_RCV_AUTO;
  pDev->RcvCount = Size & ~USART_DEV_SIZE_AUTO;
  pDev->RcvLen = 0;

  struct _USART_HW *pUsartHw = pDev->pUsartHw;
  //�򿪽���ʹ��
  pUsartHw->UCSRB |= (1 << RXEN);
  //�򿪽����ж�
  pUsartHw->UCSRB |= (1 << RXCIE);
  return 0;
}

//--------------------Usart��������ǿ��ֹͣ---------------------------
//ֹͣ���ڶ������е����ݻ�ѽ�����Ϊֹͣ״̬
//�����ѽ��յ������ݸ���
UsartSize_t UsartDev_RcvStop(struct _UsartDev *pDev)
{
  struct _USART_HW *pUsartHw = pDev->pUsartHw;
  //ֹͣ���գ��رս����ж�
  pUsartHw->UCSRB &= ~((1 << RXEN) | (1 << RXCIE));
  //ִ�����ݶ�ȡ����������������жϱ�־��
  unsigned char i = pUsartHw->UDR;
  pDev->Flag &= ~USART_DEV_RCV_BUSY;//�����ֹͣ��־
  return pDev->RcvLen;
}

//--------------------Usar���͵�������׼��--------------------------------
//����USART�н�������,�����Ƿ������ɹ�
//���������Ƿ�ɹ�,0:�ɹ�,����ʧ��
//ע:���ͻ�������С���λ��1��ʾ���Զ�ģʽ
signed char UsartDev_SendRdy(struct _UsartDev *pDev,      //�����豸
                          unsigned char *pBuf,         //���ͻ�����
                          UsartSize_t Size,         //���ͻ�������С
                          UsartDevInt_t SendRcvEndInt);//���ͻص�����


//--------------------Usar����������������--------------------------------
//����USART�н�������,�����Ƿ������ɹ�
//���������Ƿ�ɹ�,0:�ɹ�,����ʧ��
//ע:���ͻ�������С���λ��1��ʾ���Զ�ģʽ
signed char UsartDev_SendStart(struct _UsartDev *pDev,      //�����豸
                               unsigned char *pBuf,         //���ͻ�����
                               UsartSize_t Size,         //���ͻ�������С
                               UsartDevInt_t SendEndInt) //���ͻص�����
{
  //if(pDev->Flag & USART_DEV_SEND_BUSY) return -1;  //�����÷����أ�
  pDev->Flag |= USART_DEV_SEND_BUSY;
  pDev->Flag &= ~(USART_DEV_SEND_ERR | //��ط��ͱ�־��ʼ��
                  USART_DEV_SEND_AUTO);
  pDev->pSendBuf = pBuf;
  pDev->SendEndInt = SendEndInt;
  if(Size & USART_DEV_SIZE_AUTO)//�Զ�ģʽ
    pDev->Flag |= USART_DEV_SEND_AUTO;
  pDev->SendCount = Size & ~USART_DEV_SIZE_AUTO;
  pDev->SenLen = 0;

  struct _USART_HW *pUsartHw = pDev->pUsartHw;
  //�򿪷���ʹ��
  pUsartHw->UCSRA |= (1 << TXC);//������ͱ�־
  pUsartHw->UCSRB |= (1 << TXEN);
  //Ԥ��һ���������ж�ʱ�͵ڶ�����
  pUsartHw->UDR = *pBuf;//װ������׼������
  //���ʹ�ܿ��жϣ���ʼ����
  pUsartHw->UCSRB |= (1 << UDRIE);
  return 0;
}

//--------------------Usart��������ֹͣ����---------------------------
//ֹͣ����д�����е����ݻ�ѷ�����Ϊֹͣ״̬
//�����ѷ��͵������ݸ���
UsartSize_t UsartDev_SendStop(struct _UsartDev *pDev)
{
  struct _USART_HW *pUsartHw = pDev->pUsartHw;
  //ֹͣ����//�ط����ж�
  pUsartHw->UCSRB &= ~((1 << TXEN) | (1 << TXCIE) | (1 << UDRIE));
  //�巢���жϱ�־
  pUsartHw->UCSRA |= (1 << TXC) | (1 << UDRE);//������ͱ�־
  pDev->Flag &= ~USART_DEV_SEND_BUSY;//�����ֹͣ��־
  return pDev->SenLen;
}

//---------------------USARTӲ�������жϴ�����----------------------------
void UsartDev_RcvIRQ(struct _UsartDev *pDev)
{
  struct _USART_HW *pUsartHw = pDev->pUsartHw;//��ʱָ��
  unsigned char State = pUsartHw->UCSRA;
  unsigned char RcvData = pUsartHw->UDR;//��������������
  pDev->RcvData = RcvData;
  //�ж�֡�����������,��żУ�����
  if(State & ((1 << FE) | (1 << DOR) | (1 << PE))){
    pDev->Flag |= USART_DEV_RCV_ERR;
    return;
  }

  UsartSize_t RcvLen = pDev->RcvLen;
  //������������,���ʱ�������Ҳ�����ˣ�
  if(RcvLen >= pDev->RcvCount){
    //��������ǿ�����
    pDev->Flag |= USART_DEV_RCV_ERR;
    pDev->RcvEndInt(pDev);
    UsartDev_RcvStop(pDev);
    return;
  }
  //�յ���������
  *(pDev->pRcvBuf + RcvLen) = RcvData;
  pDev->RcvLen++;
  if(!(pDev->Flag & USART_DEV_RCV_AUTO)){
    if(pDev->RcvEndInt(pDev))//�ֹ�ֹͣ��
      UsartDev_RcvStop(pDev);
  }
}

//---------------------USARTӲ�������жϴ�����----------------------------
void UsartDev_SendIRQ(struct _UsartDev *pDev)
{
  struct _USART_HW *pUsartHw = pDev->pUsartHw;//��ʱָ��
  pDev->SenLen++;//�Ѿ�����һ����

  //δ�������ʱ
  if(pDev->SenLen < pDev->SendCount){
    if(!(pDev->Flag & USART_DEV_SEND_AUTO)){//�ж�ģʽʱ
      if(pDev->SendEndInt(pDev)){//�ֹ�ֹͣ��
        UsartDev_SendStop(pDev);
        return;
      }
    }
    pUsartHw->UDR = *(pDev->pSendBuf + pDev->SenLen);//�����¸���
  }
  //���һ�������Ƴ���,ת��Ϊ���ݷ����ж����
  //�˹���Ҳ������
  else if(pDev->SenLen == pDev->SendCount){
    pUsartHw->UCSRB &= ~(1 << UDRIE);
    pUsartHw->UCSRB |= (1 << TXCIE);
  }
  else{//�������
    pDev->SendEndInt(pDev);//ǿ�����
    UsartDev_SendStop(pDev);
    return;
  }
}



