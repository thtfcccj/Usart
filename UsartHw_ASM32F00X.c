/******************************************************************************

                   Usart�豸Ӳ����ASM32F00X��F003,F005�ȣ��е�ʵ��
//��ģ�鸺�𴮿���ص�����,�ݲ�֧��У��,ʹ��ʹ��������������ģʽ
//һ��ֹͣλʱ��Ϊģʽ1������ֹͣλʱ��Ϊģʽ3(��ʱB8λ����ֹͣλ)
////��ģ����HC32Fork����ģ��ṹ����һ��
******************************************************************************/
#include "UsartHw_ASM32.h"
#include "UsartDevCfg.h"
#include "CMSIS.h"

#define USART_TypeDef  UART_TypeDef

/******************************************************************************
                        ���ʵ��
******************************************************************************/


//-----------------------�õ���ʱ��ֵ����------------------------------
static unsigned short _GetSCNT(unsigned long Bandrate,
                                 unsigned long Clk)        //��ǰ����ʹ�õ�ʱ��
{
  //ҳP311: ����UARTx_BAUDCR. SELF_BRG=1,ʹ��������������ģʽ
  //���Ϲ�ʽ: Bandrate = ((UARTx_SCON.BAUD=0 + 1)*FpClk) / (32 * (UARTx_BAUDCR.BRG + 1))
  //�����Ϊ�� UARTx_BAUDCR.BRG = ((FpClk / Bandrate) >> 5) - 1;
  Clk /= Bandrate;
  if(Clk & 0x01) Clk ++; //��������
  return (Clk >> 5) - 1;
}

//----------------------------ASM32���ڱ�׼����ʵ��----------------------------
//ֻ�����봮�ڵ������֣������봮�ڵ�ʹ�ܺ������Ĵ�������
void UsartHw_ASM32(const struct _UsartDevCfg *pCfg,//�������ýṹ��
                   void * pUsartHw,           //Ӳ���豸ָ��
                   unsigned long Clk)        //��ǰ����ʹ�õ�ʱ��
{
  //�ٶ�����λ�ڿ�����һֱ����Ĭ��ֵ  
  USART_TypeDef *pHw = pUsartHw; 
  //�������貨���ʣ� b16ΪUARTx_BAUDCR. SELF_BRG=1
  pHw->BRG = (1 << 16) | _GetSCNT(UsartDevCfg_GetBuad(pCfg), Clk);
  
  //ֹͣλ��������
	if(pHw->SCON  & USART_DEV_CFG_2_STOP)//����ֹͣλʱ
    pHw->SCON = (3 << 6) | (0 << 3); //����ģʽ3(b6),TB8����ֹͣλ0(b3)
  else
    pHw->SCON = (1 << 6) | (0 << 3); //����ģʽ1
}


