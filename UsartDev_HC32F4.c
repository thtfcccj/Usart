/******************************************************************************

                   Usart�豸������HC32�е�ʵ��
//��ģ��֧�ֽӿ��ṩ�����й���
******************************************************************************/

#include "UsartDev.h"
#include <string.h>

#include "CMSIS.h"


//---------------------------�ڲ�����-----------------------------------------
#define USART_TypeDef  M4_USART_TypeDef

//CR1��ʹ�õ�λ:
#define USART_CR1_CORE    (1 <<19)//b19 ORE��־����λ
#define USART_CR1_CFE     (1 <<17)//b17 FE��־����λ FE��־����λ
#define USART_CR1_CPE     (1 <<16)//b16 PE��־����λ
#define USART_CR1_CERR    (USART_CR1_CORE | USART_CR1_CFE | USART_CR1_CPE)//���й���λ

#define USART_CR1_TXEIE   (1 << 7)//b7 �������ݼĴ������ж�ʹ��λ
#define USART_CR1_TCIE    (1 << 6)//b6 ��������ж�ʹ ��λ
#define USART_CR1_RIE     (1 << 5)//b5 �����ж�ʹ��λ
#define USART_CR1_TE      (1 << 3)//b3 ������ʹ��λ
#define USART_CR1_RE      (1 << 2)//b2 ������ʹ��λ ������ʹ��λ
  

//SR��ʹ�õ�λ:
#define USART_SR_TXE   (1 << 7)//b7 �������ݼĴ�����
#define USART_SR_TC    (1 << 6)//b6 ������ɱ�־
#define USART_SR_RXNE  (1 << 5)//b5 �������ݼĴ�����Ϊ��

#define USART_SR_ORE  (1 << 3)//b3 �������ݼĴ���δ����ȡ������£��ֽ��յ�һ֡�µ�����

#define USART_SR_FE    (1 << 1)//b1 ����֡����
#define USART_SR_PE    (1 << 1)//b0 ��������У������־

//-------------------------��ʼ������---------------------------------------
void UsartDev_Init(struct _UsartDev *pDev,
                   void * pUsartHw)  //�ҽӵ�Ӳ��
{
  memset(pDev, 0, sizeof(struct _UsartDev));
  pDev->pUsartHw = pUsartHw;
  USART_TypeDef *pHw = (USART_TypeDef *)pUsartHw;
  //�رմ���,���ж�ʹ��
  pHw->CR1 &= ~(USART_CR1_RE | USART_CR1_TE | 
                USART_CR1_RIE | USART_CR1_TCIE | USART_CR1_TXEIE);
  pHw->CR1 |=  USART_CR1_CFE | USART_CR1_CPE;//�������״ֵ̬	
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

  USART_TypeDef *pUsartHw = pDev->pUsartHw;
  //�򿪽���ʹ��
  pUsartHw->CR1 |= USART_CR1_RE;
  //�򿪽����ж�
  pUsartHw->CR1 |= USART_CR1_RIE;
  return 0;
}

//--------------------Usart��������ǿ��ֹͣ---------------------------
//ֹͣ���ڶ������е����ݻ�ѽ�����Ϊֹͣ״̬
//�����ѽ��յ������ݸ���
unsigned short UsartDev_RcvStop(struct _UsartDev *pDev)
{
  USART_TypeDef *pUsartHw = pDev->pUsartHw;
  //ֹͣ���գ��رս����ж�
  pUsartHw->CR1 &= ~(USART_CR1_RE | USART_CR1_RIE);
  //�������״ֵ̬	
  pUsartHw->CR1 |=  USART_CR1_CFE | USART_CR1_CPE | USART_CR1_CORE;
  
  if(pUsartHw->SR & USART_SR_RXNE){//����������ж�
    volatile unsigned char dump = pUsartHw->DR;
  }

  pDev->Flag &= ~USART_DEV_RCV_BUSY;//�����ֹͣ��־
  return pDev->RcvLen;
}

//--------------------Usar����������������--------------------------------
//����USART�н�������,�����Ƿ������ɹ�
//���������Ƿ�ɹ�,0:�ɹ�,����ʧ��
//ע:���ͻ�������С���λ��1��ʾ���Զ�ģʽ
signed char UsartDev_SendStart(struct _UsartDev *pDev,      //�����豸
                               const unsigned char *pBuf,   //���ͻ�����
                               unsigned short Size,         //���ͻ�������С
                               UsartDevInt_t SendEndInt)    //���ͻص�����
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
  //�򿪷���ʹ��
  pUsartHw->CR1 |= USART_CR1_TE;
  pUsartHw->DR = *pBuf;//װ������׼������
  //��������ж�ʹ��
  if(pDev->SendCount > 1)  pUsartHw->CR1 |= USART_CR1_TXEIE;
  else pUsartHw->CR1 |= USART_CR1_TCIE;//һ����ֱ�����
  return 0;
}

//--------------------Usart��������ֹͣ����---------------------------
//ֹͣ����д�����е����ݻ�ѷ�����Ϊֹͣ״̬
//�����ѷ��͵������ݸ���
unsigned short UsartDev_SendStop(struct _UsartDev *pDev)
{
  USART_TypeDef *pUsartHw = pDev->pUsartHw;
  //ֹͣ����,�ط����ж�
  pUsartHw->CR1 &= ~(USART_CR1_TCIE | USART_CR1_TXEIE | USART_CR1_TE);

  //pUsartHw->CR1 |=  USART_CR1_CORE;//�����ֵ
  //if(pUsartHw->SR & USART_SR_TXE)//д���巢���ж�
  //  pUsartHw->DR = 0;
  
  pDev->Flag &= ~USART_DEV_SEND_BUSY;//�����ֹͣ��־
  return pDev->SenLen;
}

//---------------------USARTӲ�������жϴ�����----------------------------
void UsartDev_RcvIRQ(struct _UsartDev *pDev)
{
   USART_TypeDef *pUsartHw = pDev->pUsartHw;//��ʱָ��
   unsigned char RcvData;
	 unsigned short RcvLen;
  
   //��������֡���󣬻���������ж�ʧʱ
   if(pUsartHw->SR & (USART_SR_ORE | USART_SR_FE | USART_SR_PE)){
     //ORE=1���ܼ����������ݣ�ʱ��ͬ��ģʽ��Ҳ���ܷ�������
     //FE=1ʱ�յ������ݻᱣ������RI�жϲ��ᷢ����FE=1���ܼ�����������
     //PE=1ʱ�յ������ݻᱣ������RI�жϲ��ᷢ����PE=1���ܼ�����������
     pUsartHw->CR1 |= USART_CR1_CORE | USART_CR1_CFE | USART_CR1_CPE;//ǿ�����
     pDev->Flag |= USART_DEV_RCV_ERR; //�ý��մ���
   }
   
	//��������ж�
	if((pUsartHw->CR1 & USART_CR1_RIE) && (pUsartHw->SR & USART_SR_RXNE)){
		RcvData = pUsartHw->DR >> 16;//��������������,�Զ�����տ��ж�
		pDev->RcvData = RcvData;
		RcvLen = pDev->RcvLen;
		//������������,���ʱ�������Ҳ�����ˣ�
		if(RcvLen >= pDev->RcvCount){
      if(pDev->RcvEndInt(pDev))//��������ǿ�����(��������)
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
 
	//���͹��̼�ȫ������ж�
  else if((pUsartHw->CR1 & USART_CR1_TXEIE) && (pUsartHw->SR & USART_SR_TXE) ||
     (pUsartHw->CR1 & USART_CR1_TCIE) && (pUsartHw->SR & USART_SR_TC)){
    pDev->SenLen++; //��д�뻺����һ������
    if(pDev->SenLen < pDev->SendCount){
      if(!(pDev->Flag & USART_DEV_SEND_AUTO)){//������ÿ����ͨ��
        if(pDev->SendEndInt(pDev)){//�û��ý�����
          UsartDev_SendStop(pDev);
          return;
        }
      }
      pUsartHw->DR = pDev->pSendBuf[pDev->SenLen];//���������������ж�
      //���һ���������ˣ��л��ж�����
      if((pDev->SenLen + 1) >= pDev->SendCount){
        pUsartHw->CR1 &= ~USART_CR1_TXEIE;
        pUsartHw->CR1 |= USART_CR1_TCIE;       
      }
    }
    //���һ���������
    else{
      UsartDev_SendStop(pDev);//�رշ�������ж�
      pDev->SendEndInt(pDev);//������ɻص�����
    }
	}
  else{//�����쳣��
    pDev->Flag |= USART_DEV_SEND_ERR | USART_DEV_RCV_ERR;
    UsartDev_RcvStop(pDev);
    UsartDev_SendStop(pDev);
  }
}



