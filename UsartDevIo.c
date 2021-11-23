/******************************************************************************

                   Usart�豸������ʹ��IOģ��ʱ��ʵ��
//��ģ����HC32Fork����ģ��ṹ����һ��
******************************************************************************/

#ifdef SUPPORT_EX_PREINCLUDE//��֧��Preinlude�r
  #include "Preinclude.h"
#endif

#ifdef SUPPORT_USART_DEV_IO    //����IOʵ��ʱ
  #include "UsartDevIo.h"
#else
  #include "UsartDev.h"
#endif

#include <string.h>

#include "UsartHwIo.h"

//---------------------------�ڲ�����-----------------------------------------
#define USART_TypeDef  struct _UsartHwIo


//-------------------------��ʼ������---------------------------------------
#ifdef SUPPORT_USART_DEV_IO    //����IOʵ��ʱ
void UsartDevIo_Init(struct _UsartDev *pDev,
                   void * pUsartHw)  //�ҽӵ�Ӳ��
#else
void UsartDev_Init(struct _UsartDev *pDev,
                   void * pUsartHw)  //�ҽӵ�Ӳ��
#endif
{
  memset(pDev, 0, sizeof(struct _UsartDev));
  pDev->pUsartHw = pUsartHw;
  USART_TypeDef *pHw = (USART_TypeDef *)pUsartHw;
}

//---------------------Usart��ʼ�������ݺ���--------------------------------
//����USART��������,�����Ƿ������ɹ�
//���������Ƿ�ɹ�,0:�ɹ�,����ʧ��
//ע:���ջ�������С���λ��1��ʾ���Զ�ģʽ
#ifdef SUPPORT_USART_DEV_IO    //����IOʵ��ʱ
signed char UsartDevIo_RcvStart(struct _UsartDev *pDev,  //�����豸
                              unsigned char *pBuf,     //���ջ�����
                              unsigned short Size,     //���ջ�������С
                              UsartDevInt_t RcvEndInt)//���ջص�����
#else
signed char UsartDev_RcvStart(struct _UsartDev *pDev,  //�����豸
                              unsigned char *pBuf,     //���ջ�����
                              unsigned short Size,     //���ջ�������С
                              UsartDevInt_t RcvEndInt)//���ջص�����
#endif
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

  USART_TypeDef *pUsartHw = pDev->pUsartHw;
  //�򿪽���ʹ�����ж�
  pUsartHw->SCON |= USART_HW_IO_SCON_RE | USART_HW_IO_SCON_RCIE;
  UsartHwIo_SendStop(pUsartHw);//��֧�ְ�˫����ֹͣ���ܵķ���
  UsartHwIo_RcvStart(pUsartHw); //��������
  return 0;
}

//--------------------Usart��������ǿ��ֹͣ---------------------------
//ֹͣ���ڶ������е����ݻ�ѽ�����Ϊֹͣ״̬
//�����ѽ��յ������ݸ���
#ifdef SUPPORT_USART_DEV_IO    //����IOʵ��ʱ
unsigned short UsartDevIo_RcvStop(struct _UsartDev *pDev)
#else
unsigned short UsartDev_RcvStop(struct _UsartDev *pDev)
#endif
{
  USART_TypeDef *pUsartHw = pDev->pUsartHw;
  UsartHwIo_RcvStop(pUsartHw);
  pDev->Flag &= ~USART_DEV_RCV_BUSY;//�����ֹͣ��־
  return pDev->RcvLen;
}

//--------------------Usar����������������--------------------------------
//����USART�н�������,�����Ƿ������ɹ�
//���������Ƿ�ɹ�,0:�ɹ�,����ʧ��
//ע:���ͻ�������С���λ��1��ʾ���Զ�ģʽ
#ifdef SUPPORT_USART_DEV_IO    //����IOʵ��ʱ
signed char UsartDevIo_SendStart(struct _UsartDev *pDev,      //�����豸
                               const unsigned char *pBuf,         //���ͻ�����
                               unsigned short Size,         //���ͻ�������С
                               UsartDevInt_t SendEndInt) //���ͻص�����
#else
signed char UsartDev_SendStart(struct _UsartDev *pDev,      //�����豸
                               const unsigned char *pBuf,         //���ͻ�����
                               unsigned short Size,         //���ͻ�������С
                               UsartDevInt_t SendEndInt) //���ͻص�����
#endif

{
  //if(pDev->Flag & USART_DEV_SEND_BUSY) return -1;  //�����÷����أ�
  pDev->Flag |= USART_DEV_SEND_BUSY;
  pDev->Flag &= ~(USART_DEV_SEND_ERR | //��ط��ͱ�־��ʼ��
                  USART_DEV_SEND_AUTO);
  pDev->pSendBuf = pBuf;
  pDev->SendEndInt = SendEndInt;
  if(Size & 0x8000)//�Զ�ģʽ
    pDev->Flag |= USART_DEV_SEND_AUTO;
  pDev->SendCount = Size & ~0x8000;
  pDev->SenLen = 0;

  USART_TypeDef *pUsartHw = pDev->pUsartHw;
  pUsartHw->SBUF = *pBuf;//װ������׼������
  pUsartHw->SCON |= USART_HW_IO_SCON_TCIE;//��������ж�ʹ��
  UsartHwIo_RcvStop(pUsartHw);//��֧�ְ�˫����ֹͣ���ܵĽ���
  UsartHwIo_SendStart(pUsartHw);
  return 0;
}

//--------------------Usart��������ֹͣ����---------------------------
//ֹͣ����д�����е����ݻ�ѷ�����Ϊֹͣ״̬
//�����ѷ��͵������ݸ���
#ifdef SUPPORT_USART_DEV_IO    //����IOʵ��ʱ
unsigned short UsartDevIo_SendStop(struct _UsartDev *pDev)
#else
unsigned short UsartDev_SendStop(struct _UsartDev *pDev)
#endif
{
  USART_TypeDef *pUsartHw = pDev->pUsartHw;
  UsartHwIo_SendStop(pUsartHw);
  pDev->Flag &= ~USART_DEV_SEND_BUSY;//�����ֹͣ��־
  return pDev->SenLen;
}

//---------------------�ڲ��շ�ֹͣ��----------------------------
#ifdef SUPPORT_USART_DEV_IO    //����IOʵ��ʱ
  #define _RcvStop(dev) UsartDevIo_RcvStop(dev)
  #define _SendStop(dev) UsartDevIo_SendStop(dev)
#else
  #define _RcvStop(dev) UsartDev_RcvStop(dev)
  #define _SendStop(dev) UsartDev_SendStop(dev)
#endif

//---------------------USARTӲ�������жϴ�����----------------------------
#ifdef SUPPORT_USART_DEV_IO    //����IOʵ��ʱ
void UsartDevIo_RcvIRQ(struct _UsartDev *pDev)
#else
void UsartDev_RcvIRQ(struct _UsartDev *pDev)
#endif
{
  USART_TypeDef *pUsartHw = pDev->pUsartHw;//��ʱָ��
   unsigned char RcvData;
	 unsigned short RcvLen;
  
	//��������ж�
	if(pUsartHw->ISR & USART_HW_IO_ISR_RI){
	  RcvData = pUsartHw->SBUF;//��������������
    pUsartHw->ISR |= USART_HW_IO_ISR_RI;//������ж�
    if(pUsartHw->SCON & USART_HW_IO_SCON_RCIE){//�����ݹ�����ʱ
      pDev->RcvData = RcvData;
       //�ж�֡����
      if(pUsartHw->ISR & USART_HW_IO_ISR_FE){
        pUsartHw->ISR &= ~USART_HW_IO_ISR_FE; //������־
        pDev->Flag |= USART_DEV_RCV_ERR;
        return;
      }
      RcvLen = pDev->RcvLen;
      //������������,���ʱ�������Ҳ�����ˣ�
      if(RcvLen >= pDev->RcvCount){
        if(pDev->RcvEndInt(pDev))//��������ǿ�����(��������)
          _RcvStop(pDev);
        return;
      }
      //�յ���������
      *(pDev->pRcvBuf + RcvLen) = RcvData;
      pDev->RcvLen++;
      if(!(pDev->Flag & USART_DEV_RCV_AUTO)){
        if(pDev->RcvEndInt(pDev))//�ֹ�ֹͣ��
          _RcvStop(pDev);
      }
    }//end �����ݹ�����ʱ
	}
 
	//��������ж�
  if(pUsartHw->ISR & USART_HW_IO_ISR_TI){
    pUsartHw->ISR |= USART_HW_IO_ISR_TI;//���ж�
    if(pUsartHw->SCON & USART_HW_IO_SCON_TCIE){//�����ݹ�����ʱ
      pDev->SenLen++; //��д�뻺����һ������
      if(pDev->SenLen < pDev->SendCount){
        if(!(pDev->Flag & USART_DEV_SEND_AUTO)){//������ÿ����ͨ��
          if(pDev->SendEndInt(pDev)){//�û��ý�����
            _SendStop(pDev);
            return;
          }
        }
        pUsartHw->SBUF = pDev->pSendBuf[pDev->SenLen];//����������
      }
      //���һ�������룬��û����,�л�Ϊ����ж�
      else{
        _SendStop(pDev);//�رշ�������ж�
        pDev->SendEndInt(pDev);//������ɻص�����
        pUsartHw->SCON &= ~USART_HW_IO_SCON_TCIE;
      }
	  }//end �����ݹ�����ʱ
  }
}



