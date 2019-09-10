/******************************************************************************

                   Usart�豸Ӳ��֮LPC_Arm���ʵ��

******************************************************************************/
#include "UsartHw_LPC_Arm.h"
#include "LPC12XXBit.h"

/******************************************************************************
                        ���ʵ��
******************************************************************************/

//=========================����UART0�����ʺ���============================	
//����:
//0-3:DIVADDVAL,ϸ��ֵ��λֵ
//4-7:MULVAL,,ϸ��ֵʣλֵ
//12-27:��Ƶֵ
//��1��ʾ����ʧ�ܣ�ԭ��Ϊ��Ƶ̫�߻���Ч��̫����
static unsigned int _GetBandrater(unsigned int Bandrate,//���õĲ����ʣ�
                           unsigned int Mck)            //ģ��ʱ��
{
  //�������ʳ�������ʱ��ʼ��Ϊ9600
  if((Bandrate < 1000) && (Bandrate > 172800)) Bandrate = 9600;

  //�õ�����ֵ,Q12��С��(�̶�16��ƵQ4+Q8)
  unsigned int ulBaundDiv = (Mck << 6) / Bandrate;
  ulBaundDiv <<= 2;

  //��ϵͳ��Ƶ��������(��ӽ�����)��ϵͳʱ,ֱ�ӷ�Ƶ,��С������
  unsigned int ulBaundMod = ulBaundDiv & 0x00000fff;
  if((ulBaundMod < 0x0000000f) || (ulBaundMod > 0x00000ff0)){
    if(ulBaundMod > 0x00000ff0) ulBaundDiv += (1 << 12);
    ulBaundDiv &= ~0xfff;
    return ulBaundDiv | 0x10; //MULVAL����Ϊ1
  }

  //������������ϵʱ,ʹ��С��,�����㷨��:
  //  ��������Ϊ2�Ĵη���,��ֵ��2�Ĵη����ƽ�ֱ����Ϊ1-2֮��
  //  С������Ϊ��-1,��0-1֮���ֵ�÷��ӷ�ĸȥ�ƽ�,ֱ�������С
  unsigned int ulDifNum = 0x1000;//ϸ�ֳ���ֵ,Q12
  while((ulBaundDiv /  ulDifNum) != 1) ulDifNum <<= 1;

  ulBaundMod = ulBaundDiv / (ulDifNum >> 12);//�õ�Q12��С��У��ֵ,Ϊ1-2֮��
  ulBaundMod -= 0x1000;		//�õ�С������0-1֮��,Q12��С��
  ulBaundDiv =  ulDifNum;	//�õ���������,2�Ĵη���

  //���ñƽ����õ�С���ķ��ӷ�ĸ
  ulDifNum = 0xffffffff;	//�ò�ֵΪ���
  unsigned char uDiv,uMui;
  //jΪ��ĸֵ,iΪ����ֵ
  for(unsigned char j = 1; j <= 15; j++){
    for(unsigned char i = j + 1; i <= 15; i++){
      //�õ�Q12��С��ֵ,4��5��
      unsigned int ulDifNum2 = (j << 13) / i;
      if(ulDifNum2 & 0x1) ulDifNum2++;
      ulDifNum2 >>= 1;

      //����ֵ��Сʱ��ס��ǰֵ����ֵ
      if(ulDifNum2 >= ulBaundMod) ulDifNum2 -=  ulBaundMod;
	  else ulDifNum2 = ulBaundMod - ulDifNum2;
      if(ulDifNum2 < ulDifNum){
        uDiv = j;
        uMui = i;
        ulDifNum = ulDifNum2;
      }
    }
  }
  ulBaundDiv &= ~0xfff;
  return ulBaundDiv |= uDiv | (uMui << 4);
}

//---------------------------------���ú���---------------------------------
//ʹ��Usart�Ƽ������ýṹ,�����Ƿ�ɹ�
signed char UsartHw_Cfg(struct _USART_HW *pUartHw,//�����õ�Ӳ��
                        const struct _UsartDevCfg *pCfg)
{
  unsigned long Bandrate = ((unsigned long)pCfg->BuadH << 16) + pCfg->BuadL;
  //�õ����ò����ʲ�����
  unsigned int Bandrater = _GetBandrater(Bandrate,UsartHw_cbGetMck());

  //�õ���������
  unsigned int HwCfg;//pUartHw->LCR�е�Cfg;
  unsigned char Cfg = pCfg->Cfg;
  if(Cfg & USART_DEV_CFG_PAR_EN){
    if(Cfg & USART_DEV_CFG_ODD) HwCfg = LPC_PARITY_ODD;
    else HwCfg = LPC_PARITY_EVEN;
  }
  else{
    if(Cfg & USART_DEV_CFG_ODD) HwCfg = LPC_PARITY_FORCE1;
    else HwCfg = LPC_PARITY_FORCE0;
  }
  if(Cfg & USART_DEV_CFG_2_STOP) HwCfg |= LPC_STOPBIT2;
  if(Cfg & USART_DEV_CFG_7_BIT) HwCfg |= LPC_WORD_LEN7;
  else  HwCfg |= LPC_WORD_LEN8;

  //����LSR��ʼ
  pUartHw->LCR = LPC_DLAB;
  pUartHw->LCR = HwCfg | LPC_DLAB;
  pUartHw->FDR = Bandrater & 0xff;
  //�����Ƶ��
  Bandrater >>= 12;//��Ƶ��
  if(Bandrater < 0x100){//ֱ�ӷ�Ƶ
    pUartHw->DLL = (unsigned char)Bandrater;
    pUartHw->DLM = 0;
  }
  else if(Bandrater < 0x1000){//ʹ��DLM��Ƶ��Ϊ16����ϵ
    pUartHw->DLL = (unsigned char)(Bandrater & 0x0f);
    pUartHw->DLM = (unsigned char)(Bandrater >> 4);
  }
  else{//�ֲ�������!
    pUartHw->DLL = 0xff;
    pUartHw->DLM = 0xff;
  }
  //�������
  pUartHw->LCR &= ~LPC_DLAB;
  //ֹͣ����λ�����Դ
  pUartHw->FCR = LPC_FIFO_EN;
  pUartHw->FCR = LPC_FIFO_EN | LPC_RXFIFO_RST | LPC_TXFIFO_RST;
  pUartHw->IER = 0;
  return 0;
}