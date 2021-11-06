/******************************************************************************

                UsartDevģ��-��̬����(polymorphic type)ʵ��ʵ��

******************************************************************************/
#include "UsartDevPt.h"

/******************************************************************************
                             ���ʵ��ʵ��
******************************************************************************/
//---------------------------UsartDev��̬����Ӳ���ӿ�-------------------------
#include "UsartDev.h"
const struct _UsartDevPt  UsartDevPt_Hw = {
  (UsartDevPt_RcvStart_t)  UsartDev_RcvStart, //��������
  (UsartDevPt_RcvStop_t)   UsartDev_RcvStop,//����ֹͣ
  (UsartDevPt_SendStart_t) UsartDev_SendStart,//��������    
  (UsartDevPt_SendStop_t)  UsartDev_SendStop,//����ֹͣ
};

//---------------------------UsartDev��̬����IOģ��ӿ�-------------------------
#ifdef SUPPORT_USART_DEV_IO    //����IOʵ��ʱ
#include "UsartDevIo.h"
const struct _UsartDevPt UsartDevPt_Io = {
  (UsartDevPt_RcvStart_t)  UsartDevIo_RcvStart, //��������
  (UsartDevPt_RcvStop_t)   UsartDevIo_RcvStop,//����ֹͣ
  (UsartDevPt_SendStart_t) UsartDevIo_SendStart,//��������    
  (UsartDevPt_SendStop_t)  UsartDevIo_SendStop,//����ֹͣ
};

#endif

