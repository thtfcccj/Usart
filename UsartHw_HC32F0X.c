/******************************************************************************

                   Usart�豸Ӳ����HC32F0X(��F030,F050)��ʵ��
//��ģ�鸺�𴮿���ص�����,�ݲ�֧��У��,��ʹ�ö�Ӧ��ʱ��
//��У��ʱΪģʽ1����У��ʱΪģʽ3(��ʱB8λ��У��λ��Ӳ���Զ�ʵ��)
******************************************************************************/
#include "UsartHw_HC32.h"
#include "UsartDevCfg.h"
#include "CMSIS.h"

#define USART_TypeDef  M0P_UART_TypeDef

/******************************************************************************
                        ���ʵ��
******************************************************************************/

//-----------------------�õ���ʱ��ֵ����------------------------------
static unsigned short _GetSCNT(unsigned long Bandrate,
                                 unsigned long Clk)        //��ǰ����ʹ�õ�ʱ��
{
  //���Ϲ�ʽ: Bandrate = (����ȡ1*FpClk) / (overȡ32 * SCNT)
  //foscΪFpClkʱ��
  Clk /= Bandrate; // = over * SCNT
  if(Clk & 0x01) Clk++; //��������
  return Clk >> 4; //Mode1/3�� 0��16������Ƶ
}

//----------------------------HC32���ڱ�׼����ʵ��----------------------------
//ֻ�����봮�ڵ������֣������봮�ڵ�ʹ�ܺ������Ĵ�������
void UsartHw_HC32(struct _UsartDevCfg *pCfg,//�������ýṹ��
                   void * pUsartHw,           //Ӳ���豸ָ��
                   unsigned long Clk)        //��ǰ����ʹ�õ�ʱ��
{
  //�ٶ�����λ�ڿ�����һֱ����Ĭ��ֵ  
  USART_TypeDef *pHw = pUsartHw; 
  //�������貨����
  pHw->SCNT = _GetSCNT(UsartDevCfg_GetBuad(pCfg), Clk);
  //ֹͣλ��������  
	if(pCfg->Cfg  & USART_DEV_CFG_2_STOP)//����ֹͣλʱ
    pHw->SCON_f.STOPBIT = 2;
  else pHw->SCON_f.STOPBIT = 0;//1��ֹͣλ
  
  //У�����
	if(pCfg->Cfg & USART_DEV_CFG_PAR_EN){//У���ʱ
    pHw->SCON_f.SM =  3; //����ģʽ3
    if(pCfg->Cfg  & USART_DEV_CFG_ODD)
      pHw->SCON_f.B8CONT = 2; //Ӳ����У��
    else pHw->SCON_f.B8CONT = 1;//Ӳ��żУ��
  }
  else{//��У��
    pHw->SCON_f.SM =  1; //����ģʽ1
    pHw->SCON_f.B8CONT = 0;//�ر�У��
  }
}


