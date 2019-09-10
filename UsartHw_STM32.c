/******************************************************************************
                   Usart�豸Ӳ����STM32��ʵ��
//��ģ�鸺�𴮿���ص����ã������ʡ���żУ�飩									 
******************************************************************************/
#include "UsartHw_STM32.h"
#include "CMSIS.h"

/******************************************************************************
                        ���ʵ��
******************************************************************************/

//-----------------------�õ�UBRR����------------------------------
//������ʵ�ʺ���Ϊ�����ʵ�Q4��
static unsigned short _GetBandQ4(unsigned long Bandrate,
                                   unsigned long Clk)        //��ǰ����ʹ�õ�ʱ��
{
  //�����ʼ��㣺UBRR = fosc/(16 * BAUD);
  //foscΪ�����ʱ��(PCLK1����USART2��3��4��5��PCLK2����USART1)
  Clk <<= 4; //�������Q4������
  Clk /= Bandrate; //Q4 * 16 = Q8
  if((Clk & 0x0f) >= 0x08) Clk += 0x10; //��������
  return Clk >> 4;
}

//----------------------------STM32���ڱ�׼����ʵ��----------------------------
//ֻ�����봮�ڵ������֣������봮�ڵ�ʹ�ܺ������Ĵ�������
void UsartHw_STM32(struct _UsartDevCfg *pCfg,//�������ýṹ��
                   void * pUsartHw,           //Ӳ���豸ָ��
                   unsigned long Clk)        //��ǰ����ʹ�õ�ʱ��
{
  //�ٶ�����λ�ڿ�����һֱ����Ĭ��ֵ  
  USART_TypeDef *Hw = pUsartHw; 
  //������
	Hw->BRR = _GetBandQ4(((unsigned long)(pCfg->BuadH) << 16) + pCfg->BuadL, Clk);
  unsigned char Cfg = pCfg->Cfg;
  //��żУ��λʹ��
  Hw->CR1 &= ~USART_CR1_M;
	if(Cfg & USART_DEV_CFG_PAR_EN){
	  Hw->CR1 |= USART_CR1_PCE;
    if(!(Cfg & USART_DEV_CFG_7_BIT)) //8������λʱ,��9λ����У��λ
      Hw->CR1 |= USART_CR1_M;
  }
  else{//��У��ʱ,û�е�9λ
    Hw->CR1 &= ~USART_CR1_PCE;
  }
  //��У��ʹ��
	if(Cfg & USART_DEV_CFG_ODD)
	  Hw->CR1 |= USART_CR1_PS;
  else Hw->CR1 &= ~USART_CR1_PS;
  //ֹͣλ��������
  Hw->CR2 &= ~USART_CR2_STOP;
	if(Cfg & USART_DEV_CFG_2_STOP){
    //8������λ,��7������λ,����У��λʱ,��������ֹͣλ
    if(!(Cfg & USART_DEV_CFG_7_BIT) || (Cfg & USART_DEV_CFG_PAR_EN))
      Hw->CR2 |= USART_CR2_STOP_1;
  }
  //ע:1��ֹͣλʱ+7������λ��У��,Ӳ����֧��(ȫ��ֻ��8bit,Ӳ����С9��)
}


