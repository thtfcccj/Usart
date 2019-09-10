/******************************************************************************

                   Usart�豸������Stm32�е�ʵ��
//��ģ��֧�ֽӿ��ṩ�����й���
******************************************************************************/

#include "UsartDev.h"
#include <string.h>

#include "CMSIS.h"

//-------------------------��ʼ������---------------------------------------
void UsartDev_Init(struct _UsartDev *pDev,
                   void * pUsartHw)  //�ҽӵ�Ӳ��
{
  memset(pDev, 0, sizeof(struct _UsartDev));
  pDev->pUsartHw = pUsartHw;
  USART_TypeDef *pHw = (USART_TypeDef *)pUsartHw;
  //�رմ���,���ж�ʹ��
  pHw->CR1 &= ~(USART_CR1_RE | USART_CR1_RXNEIE | USART_CR1_TXEIE);
  pHw->CR1 |= USART_CR1_UE;//�򿪴��� 
	pHw->SR  = 0;//���״̬�Ĵ���ֵ	
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
  pUsartHw->CR1 |= USART_CR1_RXNEIE;
  return 0;
}

//--------------------Usart��������ǿ��ֹͣ---------------------------
//ֹͣ���ڶ������е����ݻ�ѽ�����Ϊֹͣ״̬
//�����ѽ��յ������ݸ���
unsigned short UsartDev_RcvStop(struct _UsartDev *pDev)
{
  USART_TypeDef *pUsartHw = pDev->pUsartHw;
  //ֹͣ���գ��رս����ж�
  pUsartHw->CR1 &= ~(USART_CR1_RE | USART_CR1_RXNEIE);
  //ִ��������жϱ�־��

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
  //�����ж�
  //pUsartHw->SR &= ~(USART_SR_TC | USART_SR_TXE);
  //�������һ���������ж�
  pUsartHw->DR = *pBuf;//װ������׼������
  pUsartHw->CR1 |= USART_CR1_TXEIE;//��������ж�ʹ��
  
  return 0;
}

//--------------------Usart��������ֹͣ����---------------------------
//ֹͣ����д�����е����ݻ�ѷ�����Ϊֹͣ״̬
//�����ѷ��͵������ݸ���
unsigned short UsartDev_SendStop(struct _UsartDev *pDev)
{
  USART_TypeDef *pUsartHw = pDev->pUsartHw;
  //ֹͣ����//�ط����ж�,עTE������,������ٿ������ȷ�һ��ǰ���ַ�
  pUsartHw->CR1 &= ~(/*USART_CR1_TE | */USART_CR1_TCIE | USART_CR1_TXEIE);
  //�巢���жϱ�־
  pUsartHw->SR &= ~USART_SR_TC;//�����������жϱ�־
  pDev->Flag &= ~USART_DEV_SEND_BUSY;//�����ֹͣ��־
  return pDev->SenLen;
}

//---------------------USARTӲ�������жϴ�����----------------------------
void UsartDev_RcvIRQ(struct _UsartDev *pDev)
{
  USART_TypeDef *pUsartHw = pDev->pUsartHw;//��ʱָ��
   unsigned char RcvData;
	 unsigned short RcvLen;
  
	//�����жϲ���
	if((pUsartHw->CR1 & USART_CR1_RXNEIE) && 
     (pUsartHw->SR & (USART_SR_RXNE | USART_SR_ORE))){//����ʱ������λRXNEλ
		RcvData = pUsartHw->DR;//��������������
    pUsartHw->SR &= ~(USART_SR_RXNE | USART_SR_ORE);//ȥ��ORE��־  
		pDev->RcvData = RcvData;
	   //�ж�֡�����������,��żУ�����
		if(pUsartHw->SR & (USART_SR_PE/* | USART_SR_FE | USART_SR_NE*/)){
			pDev->Flag |= USART_DEV_RCV_ERR;
			return;
		}
		RcvLen = pDev->RcvLen;
		//������������,���ʱ�������Ҳ�����ˣ�
		if(RcvLen >= pDev->RcvCount){
			//��������ǿ�����
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
 
	//����δ���
  if((pUsartHw->CR1 & USART_CR1_TXEIE) && (pUsartHw->SR & USART_SR_TXE)){
    pDev->SenLen++; //��д�뻺����һ������
    if(pDev->SenLen < pDev->SendCount){
      if(!(pDev->Flag & USART_DEV_SEND_AUTO)){//������ÿ����ͨ��
        if(pDev->SendEndInt(pDev)){//�û��ý�����
          UsartDev_SendStop(pDev);
          return;
        }
      }
      pUsartHw->DR = pDev->pSendBuf[pDev->SenLen];//����������
    }
    //���һ�������룬��û����,�л�Ϊ����ж�
    else{
      if(pUsartHw->SR & USART_SR_TC){//�Ѿ������
        UsartDev_SendStop(pDev);//�رշ�������ж�
        pDev->SendEndInt(pDev);//������ɻص�����
      }
      else{
        pUsartHw->CR1 &= ~USART_CR1_TXEIE;
        pUsartHw->CR1 |= USART_CR1_TCIE;
      }
    }
	}
  //���һλ�������,�رշ�������ж�
  else if((pUsartHw->CR1 & USART_CR1_TCIE) && (pUsartHw->SR & USART_SR_TC)){
    UsartDev_SendStop(pDev);//�رշ�������ж�
    pDev->SendEndInt(pDev);//������ɻص�����
  }
}



