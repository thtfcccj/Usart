/******************************************************************************

                   Usart�豸Ӳ��֮Atmega���ʵ��

******************************************************************************/
#include "UsartHw_ATMEGA.h"

/******************************************************************************
                        ���ʵ��
******************************************************************************/

//-----------------------�õ�UBRR����------------------------------
//����-1��ʾ����ʧ�ܣ�ԭ��Ϊ��Ƶ̫�߻���Ч��̫����
//�����ʼ��㣺UBRR = fosc/(16 * BAUD) - 1;
//����ֵ��-1�����������ô���0��
#if USART_HW_CPU_TYPE != 0

static unsigned short _GetUbrr(unsigned long Bandrate,
                               unsigned long Mck)
{
  unsigned long MckDiv;
  //�ж��������
  if(Bandrate > (Mck >> 4)) return (unsigned short)-1;
  MckDiv = (Mck << 2) / (Bandrate << 0);//�Ŵ�32����������
  MckDiv >>= 5;
  if(MckDiv & 1) MckDiv++;
  MckDiv >>= 1;
  MckDiv -= 1;
  return MckDiv;
}
#endif //#if USART_HW_CPU_TYPE != 0

//-----------------------Atmega32��16�ı�׼����ʵ��------------------------------
//�������ò���,ͬʱ����ʵ���ϵĲ�����
//�˺�������λ����Usart�ĵ�ǰ����

#if USART_HW_CPU_TYPE == 1
void UsartHw_Mega32Cfg(struct _UsartDevCfg *pCfg)
{
  //��Atmega32��һ������,������ֱ��ʵ��
  //�رմ���,�ж�ʹ��
  UCSRB &= ~((1 << TXEN) | (1 << RXEN) |
                  (1 << RXCIE) | (1 << TXCIE) | (1 << UDRIE));

  //Ӳ���Ĵ�����ʼ��
  unsigned char Cfg = pCfg->Cfg;
  unsigned char Ucsrc;
  if(Cfg & USART_DEV_CFG_PAR_EN){
    if(Cfg & USART_DEV_CFG_ODD)
      Ucsrc = (1 << UPM1) | (1 << UPM0) | (1 << URSEL);
    else Ucsrc = (1 << UPM1) | (1 << URSEL);
  }
  else Ucsrc = (1 << URSEL);

  if(Cfg & USART_DEV_CFG_2_STOP)
    Ucsrc |= (1 << USBS);

  if(Cfg & USART_DEV_CFG_7_BIT)
    Ucsrc |= (1 << UCSZ1);
  else
    Ucsrc |= (1 << UCSZ1) | (1 << UCSZ0);
  UCSRC = Ucsrc;

  //���ò���Ч�ʲ�����
  unsigned long Buad = ((unsigned long)pCfg->BuadH << 16) | pCfg->BuadL;
  unsigned short Ubrr  = _GetUbrr(Buad, UsartHw_cbGetMck());
  UCSRA &= ~(1 << MPCM);
  if(Ubrr == (unsigned short)-1){
    //16���ز�����������Ҫ�󣬲���8���൱�ڽ�����Ƶһ�룬������
    Ubrr  = _GetUbrr(Buad, UsartHw_cbGetMck() >> 1);
    if(Ubrr == (unsigned short)-1) return;//���������̫����
    UCSRA |= 1 << U2X;//��Ƶ
  }
  //���ò�����
  UBRRL = (unsigned char)Ubrr;
  UBRRH = (unsigned char)(Ubrr >> 8);

}

#endif // USART_HW_CPU_TYPE == 0


