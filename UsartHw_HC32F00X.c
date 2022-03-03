/******************************************************************************

                   Usart�豸Ӳ����HC32F00X��F003,F005�ȣ��е�ʵ��
//��ģ�鸺�𴮿���ص�����,�ݲ�֧��У��,��ʹ�ö�Ӧ��ʱ��
//һ��ֹͣλʱ��Ϊģʽ1������ֹͣλʱ��Ϊģʽ3(��ʱB8λ����ֹͣλ)
******************************************************************************/
#include "UsartHw_HC32.h"
#include "UsartDevCfg.h"
#include "CMSIS.h"

#define USART_TypeDef  M0P_UART_TypeDef

/******************************************************************************
                        ���ʵ��
******************************************************************************/

//��HC32F005�У�����ȡ1����F190����
//��HC32F190�У�overȡ32����F005����
#ifdef SUPPORT_HC32F190
  #define   _PRE_SHIFT   4  //ȡ16��Ƶ
#else
  #define   _PRE_SHIFT   5  //�̶� 32��Ƶ
#endif

//-----------------------�õ���ʱ��ֵ����------------------------------
static unsigned short _GetSCNT(unsigned long Bandrate,
                                 unsigned long Clk)        //��ǰ����ʹ�õ�ʱ��
{
  //���Ϲ�ʽ: Bandrate = (����ȡ1*FpClk) / (overȡ32 * SCNT)
  //foscΪFpClkʱ��
  Clk /= Bandrate; // = over * SCNT
  if(Clk & 0x01) Clk ++; //��������
  return Clk >> _PRE_SHIFT;
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
  unsigned short SCNT = _GetSCNT(UsartDevCfg_GetBuad(pCfg), Clk);
  UsartHw_cbUpdateTimer(pUsartHw, SCNT); //F005ʱTM = 56635 - SCNT;
  
  //ֹͣλ��������
	if(pCfg->Cfg  & USART_DEV_CFG_2_STOP)//����ֹͣλʱ
    pHw->SCON = (3 << 6) | (0 << 3); //����ģʽ3(b6),TB8����ֹͣλ0(b3)
  else
    pHw->SCON = (1 << 6) | (0 << 3); //����ģʽ1
}


