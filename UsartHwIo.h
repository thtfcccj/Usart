/******************************************************************************

                Usart�豸Ӳ���ײ�ʵ��
1. ��ģ��ģģ��MCU�ڲ���USARTӲ���ṹʵ��ͨѶ
2. ֧�ֶ���������ֻ֧�ְ�˫����ÿ��ʵ����1��Ӳ������֧�ּ�Rx�ж�֧��
3. ��UARTΪ�첽ͨѶ�����鲨������4800��������MCU�㹻��
   �Ҷ�ʱ���ж���Ϊ�ж�Ƕ�׷�ʽ������Ż�����
��ʵ�ֶ�����Ӳ��
******************************************************************************/
#ifndef __USART_HW_IO_H
#define __USART_HW_IO_H

/******************************************************************************
                             ��ؽṹ 
******************************************************************************/

struct _UsartHwIo{
  //public: �ⲿ��ֱ�ӷ��ʱ���
  volatile unsigned char SCON;   //����λ��λ���������
  volatile unsigned char ISR;    //״̬�Ĵ�����λ��������� 
  unsigned char SBUF;            //������ɣ�������͵�����  
  //private: �ڲ�����
  unsigned char UartCfg;        //ͬUsartDevCfg->Cfg��8λ
  unsigned short BitData;        //�յ��������ڷ��͵�����
  //����FIFO����
  unsigned short CurBitData;    //���ڽ��յ�����
  volatile unsigned char BitPos;  //�����շ���λ
                         //2~9Ϊ���ݣ�10���Ժ������þ���
  //Ӳ��ӵ�����:
  unsigned char Id;     //��ģ���ID��,����TX��Rx���� 
  void *pHwTimer;       //ӵ�е�Ӳ����ʱ��
};

//����λSCONλ����Ϊ:
#define USART_HW_IO_SCON_TX_SPACE   0x40 //�������ݼ��(�ڲ�ʹ��)
#define USART_HW_IO_SCON_RX_WAIT    0x20 //��������ʱ�ȴ�������־(�ڲ�ʹ��)

#define USART_HW_IO_SCON_RE         0x10 //����ʹ��
#define USART_HW_IO_SCON_RCIE       0x01 //ʹ�ܽ�������ж�
#define USART_HW_IO_SCON_TCIE       0x02 //ʹ�ܷ�������ж�

//״̬�Ĵ���ISRλ����Ϊ:
#define USART_HW_IO_ISR_TR_FINAL    0xC0 //�շ���ɴ�����,����Ϊ��
#define USART_HW_IO_ISR_TX_FINAL    0x80 //������ɴ�����
#define USART_HW_IO_ISR_RX_FINAL    0x40 //������ɴ�����

#define USART_HW_IO_ISR_ESTART      0x20 //��ʼ֡����
#define USART_HW_IO_ISR_EPAR        0x10 //��żУ�����
#define USART_HW_IO_ISR_ESTOP       0x08 //ֹͣλ����

#define USART_HW_IO_ISR_FE          0x04 //����֡�����־λ
#define USART_HW_IO_ISR_TI          0x02 //��������ж�
#define USART_HW_IO_ISR_RI          0x01 //��������ж�

/*********************************************************************
                        ϵͳ������غ���
*********************************************************************/
//--------------------------��ʼ��-----------------------------
//���ô˺���ǰȷ��Tx,Rx��ʼ�����
void UsartHwIo_Init(struct _UsartHwIo *pHwIo,
                    unsigned char Id,//��ģ���ID��
                    void *pHwTimer); //�ҽӵ�Ӳ����ʱ��

//-------------------Ӳ����ʱ���жϴ�����--------------------
//�����Ӧ��Ӳ����ʱ���ж���,ע�����ǰӦ����ж�
void UsartHwIo_TimerIRQ(struct _UsartHwIo *pHwIo);

//---------------Rx�����յ���ʼ���жϴ�����--------------------
//����Rx�����ж���,ע�����ǰӦ����ж�
void UsartHwIo_RxIRQ(struct _UsartHwIo *pHwIo);

//-------------------------����������-----------------------
//����ϵͳ���������в�ѯ
void UsartHwIo_FastTask(struct _UsartHwIo *pHwIo);

/*********************************************************************
                      UARTӲ���������շ���غ���
*********************************************************************/

//------------------------------��������-----------------------------
//д�����λSCON��SBUF�󣬵��ô˺�����������
void UsartHwIo_SendStart(struct _UsartHwIo *pHwIo);

//-----------------------------ֹͣ����------------------------------
//����ǿ��ֹͣ��������
void UsartHwIo_SendStop(struct _UsartHwIo *pHwIo);

//------------------------------��������-----------------------------
//д�����λSCON�󣬵��ô˺�����������
void UsartHwIo_RcvStart(struct _UsartHwIo *pHwIo);

//-----------------------------ֹͣ����------------------------------
//����ǿ��ֹͣ��������
void UsartHwIo_RcvStop(struct _UsartHwIo *pHwIo);

//---------------------------��׼����ʵ��-------------------------
#include "UsartDevCfg.h"
//ֻ�����봮�ڵ������֣������봮�ڵ�ʹ�ܺ������Ĵ�������
void UsartHwIo_Cfg(const struct _UsartDevCfg *pCfg,//�������ýṹ��
                   void * pUsartHw,           //Ӳ���豸ָ��
                   unsigned long Clk);        //��ǰ����ʹ�õ�ʱ��

/*********************************************************************
                    Ӳ����ص�����
*********************************************************************/            
#include "IoCtrl.h" //����������ֱ��ʵ��

//----------------------Ӳ���жϻص�����------------------------
//�˺������շ���һ�����ݺ����
void UsartHwIo_cbUsartIRQ(struct _UsartHwIo *pHwIo);

//-------------------------��ʱ������----------------------------
//ͨ�����������ö�ʱ��,��ʵ��Ϊ��ʱ��Ԥ��Ϊ��Ӧ�����ʶ�ʱ�����
void UsartHwIo_cbCfgTimer(void *pHwTimer, unsigned long Buad);  
          
//Ԥ������ʱ������ҪΪ���յ���ʼ��ʱ����ʱ���ӿ�1�������м������
void UsartHwIo_cbTimerStartRdy(void *pHwTimer);                        
//����������ʱ��
void UsartHwIo_cbTimerStart(void *pHwTimer);
//����������ʱ�������ͼ��֡(��֡���ݼ�ļ��)
void UsartHwIo_cbTimerStartSpace(void *pHwTimer);
//->������ʱ���ͺ�Ҳ���ˣ�
//#define UsartHwIo_cbTimerStartSpace(hwTimer) UsartHwIo_cbTimerStart(hwTimer)

//ֹͣ��ʱ��,����ֹͣͬʱ��һ�´˶�ʱ���ж�
#ifndef UsartHwIo_cbTimerStop                
  void UsartHwIo_cbTimerStop(void *pHwTimer); 
#endif

//-------------------------Rx�жϲ���----------------------------
#ifndef UsartHwIo_cbRxStart
  void UsartHwIo_cbRxStart(unsigned char Id); //��������Rx�����ж�
#endif
#ifndef UsartHwIo_cbRxStop
  void UsartHwIo_cbRxStop(unsigned char Id); //ֹͣ����Rx�����ж�
#endif

//----------------------Tx��Rx���Ų���----------------------------
#ifndef UsartHwIo_cbIsRx
  unsigned short UsartHwIo_cbIsRx(unsigned char Id); //Rx�Ƿ�Ϊ�ߵ�ƽ
#endif
#ifndef UsartHwIo_cbSetTx
  void UsartHwIo_cbSetTx(unsigned char Id); //Tx��Ϊ�ߵ�ƽ
#endif  
#ifndef UsartHwIo_cbClrTx
  void UsartHwIo_cbClrTx(unsigned char Id); //Tx��Ϊ�͵�ƽ
#endif 

#endif

