/******************************************************************************

                   Usart�豸������LPC armϵ��(arm7,m3,m0)�е�ʵ��
//��ģ��֧�ֽӿ��ṩ�����й���
******************************************************************************/

#include "UsartDev.h"
#include <stm32f10x.h>   
//#include <string.h>
//#include "LPC12xx.h"//��ͬоƬ�����//�轫LPC_UART_TypeDef����Ϊstruct _USART_HW
//#include "LPC12xxbit.h"//��ͬоƬ�����
extern void UsartDev_IRQ(struct _UsartDev *pDev); //�����жϺ�����

//-------------------------��ʼ������---------------------------------------
void UsartDev_Init(struct _UsartDev *pDev,
                   void * pUsartHw//,�ҽӵ�Ӳ��
                   /*void *pVoid*/) //�ص�����������Ҫ��ָ��
{
	struct USART_TypeDef *pUartHw = (struct _USART_HW *)pUsartHw;
  memset(pDev, 0, sizeof(struct _UsartDev));
  pDev->pUsartHw = pUsartHw;
  //pDev->pVoid = pVoid;  //�ص�����������Ҫ��ָ��
  //�رմ���,���ж�ʹ��
  //ֹͣ����λ�����Դ
  pUartHw->CR1 = 1<<13;//��ֹ����
  pUartHw->FCR = LPC_FIFO_EN | LPC_RXFIFO_RST | LPC_TXFIFO_RST;
  pUartHw->IER = 0;
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

  struct _USART_HW *pUartHw = pDev->pUsartHw;
  //��λ��ֹͣ���գ���Ϊ����״̬,����FIFO����
  if(pDev->Flag & USART_DEV_RCV_AUTO)
    pUartHw->FCR |= LPC_FIFO_EN | LPC_RXFIFO_RST | LPC_RX_TRI_LEV8;
  else
    pUartHw->FCR |= LPC_FIFO_EN | LPC_RXFIFO_RST | LPC_RX_TRI_LEV1;
  //���ճ�ʱ����ջ��������������ж�
  pUartHw->IER |= LPC_RBR_IE;
  //���ճ�ʱ��ʱ��ʼ
  return 0;
}

//--------------------Usart��������ǿ��ֹͣ---------------------------
//ֹͣ���ڶ������е����ݻ�ѽ�����Ϊֹͣ״̬
//�����ѽ��յ������ݸ���
unsigned short UsartDev_RcvStop(struct _UsartDev *pDev)
{
  struct _USART_HW *pUartHw = pDev->pUsartHw;
  //��λ��ֹͣ����
  pUartHw->FCR |= LPC_RXFIFO_RST;
  //�����жϽ�ֹ
  pUartHw->IER &= ~LPC_RBR_IE;
  pDev->Flag &= ~USART_DEV_RCV_BUSY;//�����ֹͣ��־
  return pDev->RcvLen;
}

//--------------------Usar����������������--------------------------------
//����USART�н�������,�����Ƿ������ɹ�
//���������Ƿ�ɹ�,0:�ɹ�,����ʧ��
//ע:���ͻ�������С���λ��1��ʾ���Զ�ģʽ
signed char UsartDev_SendStart(struct _UsartDev *pDev,      //�����豸
                               unsigned char *pBuf,         //���ͻ�����
                               unsigned short Size,         //���ʹ�С
                               UsartDevInt_t SendEndInt) //���ͻص�����
{
  //if(pDev->Flag & USART_DEV_SEND_BUSY) return -1;  //�����÷����أ�
  pDev->Flag |= USART_DEV_SEND_BUSY;
  pDev->Flag &= ~(USART_DEV_SEND_ERR | //��ط��ͱ�־��ʼ��
                  USART_DEV_SEND_AUTO);
  pDev->pSendBuf = pBuf;
  pDev->SendEndInt = SendEndInt;
  if(Size & 0x8000){//�Զ�ģʽ
    pDev->Flag |= USART_DEV_SEND_AUTO;
    Size &= ~0x8000;
  }
  pDev->SendCount = Size;

  struct _USART_HW *pUartHw = pDev->pUsartHw;
  //��λ��ֹͣ���ͣ���Ϊ����״̬
  pUartHw->FCR |= LPC_FIFO_EN | LPC_TXFIFO_RST;
  //������ɲ����ж�
  pUartHw->IER |= LPC_THRE_IE;

  //����ʱ�����޷�ʵ��

  //дFIFO
  if(pDev->Flag & USART_DEV_SEND_AUTO){
    unsigned char CurSize;
    if(Size > 14) CurSize = 14; //�׸������14���Ա�֤�жϼ�������
    else CurSize = Size;
    pDev->SendLen =  CurSize;//�ѷ��͸���
    //��������
    while(CurSize--) pUartHw->THR = *pBuf++;
  }
  else{ //дFIFO��һ��
    pUartHw->THR = *pBuf;
    pDev->SendLen = 1;
  }
  //��������
  pUartHw->TER = LPC_TXE;

  return 0;
}

//--------------------Usart��������ֹͣ����---------------------------
//ֹͣ����д�����е����ݻ�ѷ�����Ϊֹͣ״̬
//�����ѷ��͵������ݸ���
unsigned short UsartDev_SendStop(struct _UsartDev *pDev)
{
  struct _USART_HW *pUartHw = pDev->pUsartHw;
  //��λ��ֹͣ����
  pUartHw->FCR |= LPC_TXFIFO_RST;
  //�����жϽ�ֹ
  pUartHw->IER &= ~LPC_THRE_IE;
  //PDCʱ�ѷ������ݸ���(�޷��жϾ��巢�͸���)
  pDev->Flag &= ~USART_DEV_SEND_BUSY;//�����ֹͣ��־
  return pDev->SendLen;
}

//---------------------USARTӲ�������жϴ�����----------------------------
void UsartDev_IRQ(struct _UsartDev *pDev)
{
  struct _USART_HW *pUartHw = pDev->pUsartHw;//��ʱָ��
  if(pUartHw == NULL) return; //δ��ʼ�����

  unsigned char Ident = pUartHw->IIR & LPC_INTIDENT;
  switch(Ident){//���ʶ����
  //=====================���������ж�================
  case LPC_INT_RDA://FIFO��,ֱ��������
  case LPC_INT_CTI:{//FIFO��ʱ
    if(!(pDev->Flag & USART_DEV_RCV_BUSY)) break;//���ڽ���״̬
    if(pDev->Flag & USART_DEV_RCV_BUF_OV) break; //���������
    //�õ�������ո���
    unsigned char EnLen = pDev->RcvCount - pDev->RcvLen;
    unsigned char CurRcvLen;
    if(pDev->Flag & USART_DEV_RCV_AUTO) CurRcvLen = 8;
    else  CurRcvLen = 1;
    if(CurRcvLen > EnLen) CurRcvLen = EnLen;
    unsigned char *pRcvPos = pDev->pRcvBuf + pDev->RcvLen;
    if(Ident == LPC_INT_RDA){//FIFO��
      pDev->RcvLen += CurRcvLen;
      while(CurRcvLen){ *pRcvPos++= pUartHw->RBR; CurRcvLen--;}
    }
    else{//�յ�һ����(����7����ʱ,���ж�7��)
      *pRcvPos= pUartHw->RBR;
      pDev->RcvLen++;
    }
    pDev->RcvData = *pRcvPos;//�������һ�ν��յ�����
    //������������,���ʱ�������Ҳ�����ˣ�
    if(pDev->RcvLen >= pDev->RcvCount){
      UsartDev_RcvStop(pDev);//�Ƚ���
      pDev->Flag |= USART_DEV_RCV_BUF_OV; //������������
      pDev->RcvEndInt(pDev);//�жϽ�������
    }

    #ifdef USART_DEV_SUPPORT_RCV_OV_STOP
    //�������ݽ���ģʽ,���Զ�ģʽ���ճ�ʱʱ��ֹ
    else if(!(pDev->Flag & USART_DEV_RCV_AUTO) || (Ident ==LPC_INT_CTI)){
    #else//�������ݽ���ģʽ,�Զ�ģʽ���ճ�ʱʱ����Ϊ�����ݲ���������,��������
    else if(!(pDev->Flag & USART_DEV_RCV_AUTO)){
    #endif
      if(pDev->RcvEndInt(pDev))//�ֹ�ֹͣ��
        UsartDev_RcvStop(pDev);
    }
    break;
  }
   //======================���������ж�===================
  case LPC_INT_THER:{
    if(!(pDev->Flag & USART_DEV_SEND_BUSY)) break;//���ڷ���״̬
    //ע:SendLen���ڷ���ǰ������,������ɲ������
    unsigned char EnLen = pDev->SendCount - pDev->SendLen;
    if(!EnLen){//���ݷ������(���һ�����������֤ȫ�����������)
      UsartDev_SendStop(pDev);//ǿ�����
      pDev->SendEndInt(pDev);
      break;
    }
    else if(!(pDev->Flag & USART_DEV_SEND_AUTO)){//����ģʽʱ����ǰ����
      if(pDev->SendEndInt(pDev)){//�ֹ�ֹͣ��
        UsartDev_SendStop(pDev);
        break;
      }
    }

    //��������δ��ɵ�����
    unsigned char CurSendLen;
    if(pDev->Flag & USART_DEV_SEND_AUTO){
      CurSendLen = 14; //14��ʱ�������ж�,���2�������ܻ��ڻ�������
      if(CurSendLen > EnLen){//���һ���ܷ�����
        if(EnLen > 12) CurSendLen = 12;//��13,14��ʱ,Ϊ��֤���һ���������ж�,�ٷ�
        else CurSendLen = EnLen;
      }
    }
    else CurSendLen = 1;//���η���ģʽһ��һ��

    //��FIFO���ݣ�������
    unsigned char *pSendPos = pDev->pSendBuf + pDev->SendLen;
    pDev->SendLen += CurSendLen; //����ǰ����
    while(CurSendLen--) pUartHw->THR = *pSendPos++;


    break;
  }
  case LPC_INT_RLS:	 //Rx ��״̬/���� OE, PE, FE, ��BI,��߼�,����
  default:break;
  }
}