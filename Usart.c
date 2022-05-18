/*******************************************************************************

                   Usartʵ��ģ��ʵ��

*******************************************************************************/
#include "Usart.h"
#include "IoCtrl.h"
#include <string.h>

struct _Usart Usart; //ʵ��

/*******************************************************************************
		                      ��غ���ʵ��
*******************************************************************************/

//----------------------------------��ʼ��-------------------------------------
//����UsartDevCfg_Init()�����
void Usart_Init(void)
{
  memset(&Usart, 0, sizeof(struct _Usart));
  CfgUsart(); //IoCtrl.h:   IO�ڳ�ʼ��
  //���ײ�ֱ��ʼ��
  for(unsigned char UsartId = 0; UsartId < USART_COUNT; UsartId++){
    UsartDev_Init(&Usart.Dev[UsartId], //�ײ�Ӳ��ͨѶ��ʼ��
                  Usart_cbHw[UsartId]); 
    Usart_cbCfgInt[UsartId]();  //��������ж�
  }
}

//----------------------------------��ָ��õ�ID-------------------------------
//��ֵδ�ҵ�
signed char Usart_GetUsartId(const struct _UsartDev *pDev)
{
  for(unsigned char UsartId = 0; UsartId < USART_COUNT; UsartId++){
    if(pDev == &Usart.Dev[UsartId]) return UsartId;
  }
  return -1;//δ�ҵ�
}


