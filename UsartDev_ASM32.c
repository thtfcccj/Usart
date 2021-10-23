/******************************************************************************

                   Usart�豸������ASM32�е�ʵ��
//��ģ����HC32Fork����ģ��ṹ����һ��
******************************************************************************/

#include "UsartDev.h"
#include <string.h>

#include "CMSIS.h"

//---------------------------�ڲ�����-----------------------------------------
#define USART_TypeDef  UART_TypeDef

#define USART_SCON_RE         0x10 //����ʹ��
#define USART_SCON_RCIE       0x01 //ʹ�ܽ�������ж�
#define USART_SCON_TCIE       0x02 //ʹ�ܷ�������ж�

#define USART_ISR_FE          0x04 //����֡�����־λ
#define USART_ISR_TI          0x02 //��������ж�
#define USART_ISR_RI          0x01 //��������ж�

//#ifdef SUPPORT_USART_TX_BUF  //֧�ַ��ͻ���ʱ(�Ͷ˲�֧��)
//  #define USART_SCON_TXEIE     0x100   //�����ͻ���ʱ����
//#else
//  #define USART_SCON_TXEIE     0   //�޶���
//#endif


//-------------------------��ʼ������---------------------------------------
void UsartDev_Init(struct _UsartDev *pDev,
                   void * pUsartHw)  //�ҽӵ�Ӳ��
{
  memset(pDev, 0, sizeof(struct _UsartDev));
  pDev->pUsartHw = pUsartHw;
  USART_TypeDef *pHw = (USART_TypeDef *)pUsartHw;
  //�رմ���,���ж�ʹ��
  pHw->SCON &= ~(USART_SCON_RE | USART_SCON_RCIE | USART_SCON_TCIE);
  //pHw->SCON |= USART_SCON_UE;//�򿪴��� 
	pHw->ISR  = 0;//���״̬�Ĵ���ֵ	
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
  pUsartHw->SCON |= USART_SCON_RE;
  //�򿪽����ж�
  pUsartHw->SCON |= USART_SCON_RCIE;
  return 0;
}

//--------------------Usart��������ǿ��ֹͣ---------------------------
//ֹͣ���ڶ������е����ݻ�ѽ�����Ϊֹͣ״̬
//�����ѽ��յ������ݸ���
unsigned short UsartDev_RcvStop(struct _UsartDev *pDev)
{
  USART_TypeDef *pUsartHw = pDev->pUsartHw;
  //ֹͣ���գ��رս����ж�
  pUsartHw->SCON &= ~(USART_SCON_RE | USART_SCON_RCIE);
  //ִ��������жϱ�־��

  pDev->Flag &= ~USART_DEV_RCV_BUSY;//�����ֹͣ��־
  return pDev->RcvLen;
}

//--------------------Usar����������������--------------------------------
//����USART�н�������,�����Ƿ������ɹ�
//���������Ƿ�ɹ�,0:�ɹ�,����ʧ��
//ע:���ͻ�������С���λ��1��ʾ���Զ�ģʽ
signed char UsartDev_SendStart(struct _UsartDev *pDev,      //�����豸
                               const unsigned char *pBuf,         //���ͻ�����
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
  //pUsartHw->SCON |= USART_SCON_TE;
  //�����ж�
  //pUsartHw->ISR &= ~(USART_ISR_TI | USART_ISR_TI);
  //�������һ���������ж�
  pUsartHw->SBUF = *pBuf;//װ������׼������
  pUsartHw->SCON |= USART_SCON_TCIE;//��������ж�ʹ��
  
  return 0;
}

//--------------------Usart��������ֹͣ����---------------------------
//ֹͣ����д�����е����ݻ�ѷ�����Ϊֹͣ״̬
//�����ѷ��͵������ݸ���
unsigned short UsartDev_SendStop(struct _UsartDev *pDev)
{
  USART_TypeDef *pUsartHw = pDev->pUsartHw;
  //ֹͣ����//�ط����ж�
  pUsartHw->SCON &= ~USART_SCON_TCIE;
  //�巢���жϱ�־
  pUsartHw->ISR &= ~USART_ISR_TI;//�����������жϱ�־
  pDev->Flag &= ~USART_DEV_SEND_BUSY;//�����ֹͣ��־
  return pDev->SenLen;
}

//---------------------USARTӲ�������жϴ�����----------------------------
void UsartDev_RcvIRQ(struct _UsartDev *pDev)
{
  USART_TypeDef *pUsartHw = pDev->pUsartHw;//��ʱָ��
   unsigned char RcvData;
	 unsigned short RcvLen;
  
	//��������ж�
	if((pUsartHw->SCON & USART_SCON_RCIE) && (pUsartHw->ISR & USART_ISR_RI)){
		RcvData = pUsartHw->SBUF;//��������������
    pUsartHw->ICR |= USART_ISR_RI;
		pDev->RcvData = RcvData;
	   //�ж�֡����
		if(pUsartHw->ISR & USART_ISR_FE){
      pUsartHw->ICR |= USART_ISR_FE;
			pDev->Flag |= USART_DEV_RCV_ERR;
			return;
		}
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
 
	//��������ж�
  if((pUsartHw->SCON & USART_SCON_TCIE) && (pUsartHw->ISR & USART_ISR_TI)){
    pUsartHw->ICR |= USART_ISR_TI;
    pDev->SenLen++; //��д�뻺����һ������
    if(pDev->SenLen < pDev->SendCount){
      if(!(pDev->Flag & USART_DEV_SEND_AUTO)){//������ÿ����ͨ��
        if(pDev->SendEndInt(pDev)){//�û��ý�����
          UsartDev_SendStop(pDev);
          return;
        }
      }
      pUsartHw->SBUF = pDev->pSendBuf[pDev->SenLen];//����������
    }
    //���һ�������룬��û����,�л�Ϊ����ж�
    else{
      UsartDev_SendStop(pDev);//�رշ�������ж�
      pDev->SendEndInt(pDev);//������ɻص�����
      pUsartHw->SCON &= ~USART_SCON_TCIE;
    }
	}
}



