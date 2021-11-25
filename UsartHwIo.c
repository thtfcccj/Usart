/******************************************************************************

                Usart�豸Ӳ���ײ�ʵ��

******************************************************************************/
#include "UsartHwIo.h"
#include <string.h>

/*********************************************************************
                        ϵͳ������غ���ʵ��
*********************************************************************/

//---------------�����õõ��շ�λ����--------------------
//��, 0bУ������, 1b��ֹͣλ ��2b7������λ
static const unsigned char _Cfg2MaxLen[] = {
   10, //0,1��+8��+0У+1ͣ
   11, //1,1��+8��+1У+1ͣ 
   11, //2,1��+8��+0У+2ͣ
   12, //3,1��+8��+1У+2ͣ
   9,  //4,1��+7��+0У+1ͣ 
   10, //5,1��+7��+1У+1ͣ 
   10, //6,1��+7��+0У+2ͣ 
   11, //7,1��+7��+1У+2ͣ   
};
#define _GetMaxLen(hw) _Cfg2MaxLen[((hw)->UartCfg & 0x0f) >> 1]

//-----------------------�õ�У��λ����-----------------------------
//����������1�ĸ���
static unsigned char _GetPar(unsigned char SBUF)
{
  unsigned char BitHiCount = 0;
  for(unsigned char Mask = 0x01; Mask > 0; Mask <<= 1){
    if(SBUF & Mask) BitHiCount++;
  }
  return BitHiCount;
}

//-----------------------������ɴ�����-----------------------------
static void _RcvPro(struct _UsartHwIo *pHwIo)
{
  unsigned short Data = pHwIo->BitData;
  if(Data & 0x01){//��ʼ����
    pHwIo->ISR = USART_HW_IO_ISR_FE | USART_HW_IO_ISR_ESTART;
    return;
  }
  Data >>= 1;//������
  unsigned char SBUF = Data; //Ԥȡ����
  if(pHwIo->UartCfg & USART_DEV_CFG_7_BIT){
    SBUF &= ~0x80;//ȥ���λ
    Data >>= 7; //�����Ժ���
  }
  else Data >>= 8; //�����Ժ���
  pHwIo->SBUF = SBUF; //��ȡ������
  //У��λ
  if(pHwIo->UartCfg & USART_DEV_CFG_PAR_EN){
    //��żУ��
    unsigned char Par = ((_GetPar(SBUF) & 0x01) ^ (Data & 0x01));
    if(pHwIo->UartCfg & USART_DEV_CFG_ODD){//��У��
      if(!Par) pHwIo->ISR = USART_HW_IO_ISR_FE | USART_HW_IO_ISR_EPAR;
    }
    else{//żУ�飺��ʵ�������С�1���ĸ���Ϊż����ʱ�����У��λ���ǡ�0��
      if(Par) pHwIo->ISR = USART_HW_IO_ISR_FE | USART_HW_IO_ISR_EPAR;
    }
    Data >>= 1;//��żУ�����
  }
  //˫ֹͣλ
  if(pHwIo->UartCfg & USART_DEV_CFG_2_STOP){
    if(!(Data & 0x01)) //ֹͣλӦΪ�ߵ�ƽ
      pHwIo->ISR = USART_HW_IO_ISR_FE | USART_HW_IO_ISR_ESTOP;
    Data >>= 1;//�����һֹͣλ��    
  }
  if(!(Data & 0x01)) //ֹͣλӦΪ�ߵ�ƽ
    pHwIo->ISR = USART_HW_IO_ISR_FE | USART_HW_IO_ISR_ESTOP;
}

//-----------------------�õ�����λ����-----------------------------
static unsigned short  _GetSendBitData(const struct _UsartHwIo *pHwIo)
{
  unsigned short Data = (unsigned short)pHwIo->SBUF << 1;//ֹͣλΪ0
  unsigned short Mask;
  if(pHwIo->UartCfg & USART_DEV_CFG_7_BIT){
    Data &= ~0x100;//ȡ�����λ
    Mask = (1 << 8);
  }
  else Mask = (1 << 9);
  //У��λ
  if(pHwIo->UartCfg & USART_DEV_CFG_PAR_EN){
    unsigned char Par = _GetPar(pHwIo->SBUF) & 0x01;
    if(pHwIo->UartCfg & USART_DEV_CFG_ODD){//��У��
      if(!Par) Data |= Mask;
    }
    else{//żУ��
      if(Par) Data |= Mask;
    }
    Mask <<= 1; //��ֹͣλ��
  }
  Data |= Mask; //ֹͣλ1
  if(pHwIo->UartCfg & USART_DEV_CFG_2_STOP){//ֹͣλ2
    Data |= (Mask << 1); //ֹͣλ1
  }
  return Data;
}

//--------------------------��ʼ��-----------------------------
//���ô˺���ǰȷ��Tx,Rx��ʼ�����
void UsartHwIo_Init(struct _UsartHwIo *pHwIo,
                    unsigned char Id,//��ģ���ID��
                    void *pHwTimer) //�ҽӵ�Ӳ����ʱ��
{
  memset(pHwIo, 0, sizeof(struct _UsartHwIo));
  pHwIo->Id = Id;
  pHwIo->pHwTimer = pHwTimer;  
}

//-------------------Ӳ����ʱ���жϴ�����--------------------
//�����Ӧ��Ӳ����ʱ���ж���
void UsartHwIo_TimerIRQ(struct _UsartHwIo *pHwIo)
{
  //===================����ʱ����=======================
  if(pHwIo->SCON & USART_HW_IO_SCON_RE){
    //���������յ�ǰλ
    if(UsartHwIo_cbIsRx(pHwIo->Id)){
      pHwIo->CurBitData |= (unsigned short)1 << pHwIo->BitPos;
    }    
    //δ�յ���ʼ�����쳣��
    if(pHwIo->SCON & USART_HW_IO_SCON_RX_WAIT) return;
    //�����׸����յ���ʱ���м��ˣ�Ԥ����Ϊ��ʽ����
    if(!pHwIo->BitPos) UsartHwIo_cbTimerStart(pHwIo->pHwTimer);

    pHwIo->BitPos++;//��һλ 

    //�������
    if(pHwIo->BitPos > _GetMaxLen(pHwIo)){
      //�����������ܵĽ��շ�ֹ��ʧ
      pHwIo->ISR = USART_HW_IO_ISR_RX_FINAL; //������ɴ�����
      pHwIo->BitData = pHwIo->CurBitData;
      
      UsartHwIo_RcvStart(pHwIo); //��������һ��
    }
  }
  //===================����ʱ����=======================
  if(pHwIo->SCON & USART_HW_IO_SCON_TCIE){
    //����֡�����꣬��ʼ����������ʼ֡
    if(pHwIo->SCON & USART_HW_IO_SCON_TX_SPACE){
      pHwIo->SCON &= ~USART_HW_IO_SCON_TX_SPACE;
      UsartHwIo_SendStart(pHwIo);
      return;
    }
    //���ݷ������
    pHwIo->BitPos++;//������һλ�� 
    if(pHwIo->BitPos >= _GetMaxLen(pHwIo)){
      UsartHwIo_cbSetTx(pHwIo->Id);//�ָ��ߵ�ƽ
      UsartHwIo_cbTimerStop(pHwIo->pHwTimer);//ֹͣ��ʱ��
      pHwIo->ISR = USART_HW_IO_ISR_TX_FINAL; //������ɴ�����
    }
    //һλλ��������
    if(pHwIo->CurBitData & ((unsigned short)1 << pHwIo->BitPos))
      UsartHwIo_cbSetTx(pHwIo->Id);
    else UsartHwIo_cbClrTx(pHwIo->Id);
  }
}

//---------------Rx�����յ���ʼ���жϴ�����--------------------
//����Rx�����ж���
void UsartHwIo_RxIRQ(struct _UsartHwIo *pHwIo)
{
  //���������Ϊ���׸���ʱ
  if((pHwIo->SCON & (USART_HW_IO_SCON_RE | USART_HW_IO_SCON_RX_WAIT))
      == (USART_HW_IO_SCON_RE | USART_HW_IO_SCON_RX_WAIT)){
    //Ԥ������ʱ���Խ����׸���
    UsartHwIo_cbTimerStartRdy(pHwIo->pHwTimer);
    UsartHwIo_cbRxStop(pHwIo->Id);//ֹͣ�����ж�
    pHwIo->SCON &= ~USART_HW_IO_SCON_RX_WAIT;
  }
}

//-------------------------����������-----------------------
//����ϵͳ���������в�ѯ
void UsartHwIo_FastTask(struct _UsartHwIo *pHwIo)
{
  unsigned char IsFinal = pHwIo->ISR & USART_HW_IO_ISR_TR_FINAL;
  if(!IsFinal) return;//û�д���������� 
  pHwIo->ISR &= ~USART_HW_IO_ISR_TR_FINAL;//��ȡ��
  
  //���ݷ��ͺ�������:
  if(IsFinal & USART_HW_IO_ISR_TX_FINAL){
    pHwIo->ISR |= USART_HW_IO_ISR_TI;//����������ж�
    UsartHwIo_cbUsartIRQ(pHwIo);//�����ж�
    //���������¸���ʱ���������ͼ��
    if(pHwIo->SCON & USART_HW_IO_SCON_TCIE){
      pHwIo->SCON |= USART_HW_IO_SCON_TX_SPACE;
      UsartHwIo_cbSetTx(pHwIo->Id);//�ߵ�ƽ����
      UsartHwIo_cbTimerStartSpace(pHwIo);
    }
    //else UsartHwIo_cbTimerStop();//��������ˣ��ϲ�Ӧ�Զ�����ֹͣ
    return;
  }
  //���ݽ��պ�������
  /*if(IsFinal & USART_HW_IO_ISR_RX_FINAL)*/{  
    _RcvPro(pHwIo);//���մ���
    pHwIo->ISR |= USART_HW_IO_ISR_RI;//����������ж�
    UsartHwIo_cbUsartIRQ(pHwIo);//�����ж�
    //��ֹ��ֹͣ
    if(!(pHwIo->SCON & USART_HW_IO_SCON_RE)){
      UsartHwIo_RcvStop(pHwIo);
    }
  }
}

/*********************************************************************
                      UARTӲ���������շ���غ���
*********************************************************************/
//------------------------------��������-----------------------------
//д�����λSCON��SBUF�󣬵��ô˺�����������
void UsartHwIo_SendStart(struct _UsartHwIo *pHwIo)
{
  pHwIo->BitPos = 0;
  pHwIo->CurBitData = _GetSendBitData(pHwIo); 
  pHwIo->ISR = 0;//��λ����״̬���
  
  __disable_irq(); //�ٽ�������
  UsartHwIo_cbClrTx(pHwIo->Id);//�͵�ƽ����,�ж�ʱ��������λ��
  UsartHwIo_cbTimerStart(pHwIo->pHwTimer); //������ʱ��
	__enable_irq();
}

//-----------------------------ֹͣ����------------------------------
//����ǿ��ֹͣ��������
void UsartHwIo_SendStop(struct _UsartHwIo *pHwIo)
{
  UsartHwIo_cbTimerStop(pHwIo->pHwTimer);//��ֹͣ��ʱ��
  pHwIo->SCON &= ~(USART_HW_IO_SCON_TCIE | 
                   USART_HW_IO_SCON_TX_SPACE);//ֹͣ����,�ط����ж�
  pHwIo->ISR &= ~USART_HW_IO_ISR_TI;//�巢���жϱ�־
  UsartHwIo_cbSetTx(pHwIo->Id);//�ָ�ΪĬ�ϵ�ƽ
}

//------------------------------��������-----------------------------
//д�����λSCON�󣬵��ô˺�����������
void UsartHwIo_RcvStart(struct _UsartHwIo *pHwIo)
{
  UsartHwIo_cbTimerStop(pHwIo->pHwTimer);//ֹͣ��ʱ�����յ��׸���ʱ����
  pHwIo->BitPos = 0; //����ʼλ��ʼ
  pHwIo->CurBitData = 0;//����ȡ���
  pHwIo->SCON |= USART_HW_IO_SCON_RX_WAIT;//�ȴ�������
  UsartHwIo_cbRxStart(pHwIo->Id); //����Rx�����ж�
}

//-----------------------------ֹͣ����------------------------------
//����ǿ��ֹͣ��������
void UsartHwIo_RcvStop(struct _UsartHwIo *pHwIo)
{
  UsartHwIo_cbRxStop(pHwIo->Id); //ֹͣRx�����ж�
  UsartHwIo_cbTimerStop(pHwIo->pHwTimer);//ֹͣ��ʱ��
  //ֹͣ���գ��رս����ж�
  pHwIo->SCON &= ~(USART_HW_IO_SCON_RE | USART_HW_IO_SCON_RCIE | 
                   USART_HW_IO_SCON_RX_WAIT);
  pHwIo->ISR &= ~USART_HW_IO_ISR_RI;  //��������жϱ�־��
}

//---------------------------��׼����ʵ��-------------------------
//ֻ�����봮�ڵ������֣������봮�ڵ�ʹ�ܺ������Ĵ�������
void UsartHwIo_Cfg(const struct _UsartDevCfg *pCfg,//�������ýṹ��
                   void * pUsartHw,           //Ӳ���豸ָ��
                   unsigned long Clk)        //��ǰ����ʹ�õ�ʱ��
{
  //�ٶ�����λ�ڿ�����һֱ����Ĭ��ֵ  
  struct _UsartHwIo *pHw = pUsartHw; 
  //���Ϳ���λִ�����ж�
  pHw->UartCfg = pCfg->Cfg & USART_DEV_CFG_TYPE_MASK;
  //���ò�����
  UsartHwIo_cbCfgTimer(pHw->pHwTimer, UsartDevCfg_GetBuad(pCfg));
}



