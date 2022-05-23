/******************************************************************************

                   Usart�豸Ӳ����HC32F4X��ʵ��
//��ģ�鸺�𴮿���ص�����,�ݲ�֧��У��,��ʹ�ö�Ӧ��ʱ��
//��У��ʱΪģʽ1����У��ʱΪģʽ3(��ʱB8λ��У��λ��Ӳ���Զ�ʵ��)
******************************************************************************/
#include "UsartHw_HC32.h"
#include "UsartDevCfg.h"
#include "CMSIS.h"

#define USART_TypeDef  M4_USART_TypeDef

/******************************************************************************
                        ���ʵ��
******************************************************************************/

static const unsigned char _PrShiftLut[] = {0,2,4,6};
//-----------------------�õ���ʱ��ֵ����------------------------------
//����B14-15Ϊ��Ƶֵ��B8~0Ϊ����ֵ,�����㣬����С������Ҳ�ܴﵽ�ϸ߾���
static unsigned short _GetSCNT(unsigned long Bandrate,
                                 unsigned long Clk)        //��ǰ����ʹ�õ�ʱ��
{
  //���Ϲ�ʽ: B = C / (8*(2-OV8) �� (DIV_Integer+1))
  //OV8ȡ1��16, B�ɷ�Ƶ�������<= 256; ��(DIV_Integer+1) = C / B / (8*1)
  unsigned char Pr = 0;
  unsigned short Div;
  for(; Pr < 4; Pr++){
    unsigned long B = Clk >> _PrShiftLut[Pr];
    B /= Bandrate;   //C / B
    if(B & 0x04) B+=0x04; //��������
    Div = B >> 3; // (8*1)
    if(Div <= 256) break;
  }
  return Div | (unsigned short)Pr << 14;
}

//----------------------------HC32���ڱ�׼����ʵ��----------------------------
//ֻ�����봮�ڵ������֣������봮�ڵ�ʹ�ܺ������Ĵ�������
void UsartHw_HC32(struct _UsartDevCfg *pCfg,//�������ýṹ��
                   void * pUsartHw,           //Ӳ���豸ָ��
                   unsigned long Clk)        //��ǰ����ʹ�õ�ʱ��
{  
  USART_TypeDef *pHw = pUsartHw;
  //��ǿ�ƹرշ��������(��TE=0&RE=0)�����ò���Ч
  unsigned char En = pHw->CR1 & ((1 << 2) | (1 << 3)); //�����ֳ�
  pHw->CR1 &= ~((1 << 2) | (1 << 3));
  unsigned short SCNT = _GetSCNT(UsartDevCfg_GetBuad(pCfg), Clk);
  pHw->PR = SCNT >> 14;
  pHw->BRR = ((SCNT & 0x1ff) - 1) << 8;

    
  //��MCU��֧��7������λ(��֧��8~9λ)
  unsigned long Cr1 = pHw->CR1 & 0xE0; //�����ֳ�����ֹ�շ������־
  Cr1 |= (1 << 15); //B15:OVER,����Ĭ��Ϊ0
  //У�����
	if(pCfg->Cfg & USART_DEV_CFG_PAR_EN){//У���ʱ
    Cr1 |= (1 << 10); //ʹ��У��λ
    if(pCfg->Cfg & USART_DEV_CFG_ODD)
      Cr1 |= (1 << 9); //ʹ��У��У��
  }
  pHw->CR1 = Cr1; 
    
  //ֹͣλ��������,CF2,����Ĭ��Ϊ0
	if(pCfg->Cfg & USART_DEV_CFG_2_STOP)//����ֹͣλʱ
    pHw->CR2 = (1 << 13);
  else  pHw->CR2 = 0;
                    
  pHw->CR3 = 0;
  pHw->CR1 |= En; //�ָ��ֳ����������
}

