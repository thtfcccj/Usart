/******************************************************************************

           UsartDev�е�struct _UsartDevCfg,�ӿڲ�������
//��ģ���ͨ��struct _UsartDevCfgָ������ṹ��Ա�������������𱣴�EEPROM
******************************************************************************/
#ifndef __USART_DEV_CFG_H
#define __USART_DEV_CFG_H


/******************************************************************************
                             �������
******************************************************************************/

//����ģʽʱ��struct _UsartDevCfgռ1Byte, ������Ϊö����
//#define SUPPORT_USART_DEV_CFG_TINY    


/******************************************************************************
                             ��ؽṹ
******************************************************************************/

struct _UsartDevCfg{
  #ifndef SUPPORT_USART_DEV_CFG_TINY    //����ģʽʱ������Cfg��λ���ö�������
    unsigned short BuadL;   //�����ʵ�16bit
    unsigned char  BuadH;   //�����ʸ�8bit
  #endif  
  unsigned char  Cfg;    //�������,������
};

//������ö���Ϊ:
#define USART_DEV_CFG_ODD       0x01//��У��,����ΪżУ��
#define USART_DEV_CFG_PAR_EN    0x02//У������,������У��
#define USART_DEV_CFG_2_STOP    0x04//2��ֹͣλ,����Ϊ1��
#define USART_DEV_CFG_7_BIT     0x08//7������λ,����Ϊ8��

#ifdef SUPPORT_USART_DEV_CFG_TINY    //����ģʽʱ��Cfg��λΪ������ö����
  #define USART_DEV_BUAD_SHIFT  4
  #define USART_DEV_BUAD_MASK   (0x07 << USART_DEV_BUAD_SHIFT) //��������Ϊ:
  #define USART_DEV_BUAD_9600    0  //Ĭ��
  #define USART_DEV_BUAD_4800    1
  #define USART_DEV_BUAD_2400    2
  #define USART_DEV_BUAD_1200    3
  #define USART_DEV_BUAD_19200   4
  #define USART_DEV_BUAD_38400   5
  #define USART_DEV_BUAD_57600   6
  #define USART_DEV_BUAD_115200  7
#endif

/******************************************************************************
                             ��غ���
******************************************************************************/


//----------------------------���������----------------------------------
#ifdef SUPPORT_USART_DEV_CFG_TINY    //����ģʽʱ��Cfg��λΪ������ö����
  extern const unsigned long UsartDevCfg_Buad[];
  #define UsartDevCfg_GetBuad(pcfg) \
    UsartDevCfg_Buad[((pcfg)->Cfg & USART_DEV_BUAD_MASK) >> USART_DEV_BUAD_SHIFT]
    //#define UsartDevCfg_SetBuadc(pcfg, buad) //ʹ��UsartDevCfg_SetUserCfg()
#else
  #define UsartDevCfg_GetBuad(pcfg)  \
    (((unsigned long)((pcfg)->BuadH) << 16) + (pcfg)->BuadL)
  #define UsartDevCfg_SetBuad(pcfg, buad) \
      do{(pcfg)->BuadH = buad >> 16; (pcfg)->BuadL = buad & 0xffff;}while(0)      
#endif

//-------------------------------����λ����������--------------------------
#define UsartDevCfg_GetCfg(pcfg) ((pcfg)->Cfg)
#define UsartDevCfg_SetCfg(pcfg, cfg) do{(pcfg)->Cfg = cfg;}while(0)

//-----------------------------ͨѶ����λ����--------------------------------
#define UsartDevCfg_GetCommCfg(pcfg) ((pcfg)->Cfg & 0x0f)
#define UsartDevCfg_SetCommCfg(pcfg,commcfg) do{\
  (pcfg)->Cfg = (commcfg & 0x0f) | ((pcfg)->Cfg & 0xf0);}while(0)
    
//----------------------------�û��йܲ���λ����-------------------------------
#define UsartDevCfg_GetUserCfg(pcfg) ((pcfg)->Cfg & 0xf0)
#define UsartDevCfg_SetUserCfg(pcfg,userCfg) do{\
  (pcfg)->Cfg = (userCfg & 0xf0) | ((pcfg)->Cfg & 0x0f);}while(0)
    
//----------------------------��żУ��λ------------------------------------
//��У��,����ΪżУ��  
#define UsartDevCfg_IsOdd(pcfg)  ((pcfg)->Cfg & USART_DEV_CFG_ODD)
#define UsartDevCfg_SetOdd(pcfg) do{(pcfg)->Cfg |= USART_DEV_CFG_ODD;}while(0)  
#define UsartDevCfg_ClrOdd(pcfg) do{(pcfg)->Cfg &= ~USART_DEV_CFG_ODD;}while(0)
    
//--------------------------------У������λ------------------------------------
//У������,������У��
#define UsartDevCfg_IsParEn(pcfg)  ((pcfg)->Cfg & USART_DEV_CFG_PAR_EN)
#define UsartDevCfg_SetParEn(pcfg) do{(pcfg)->Cfg |= USART_DEV_CFG_PAR_EN;}while(0)  
#define UsartDevCfg_ClrParEn(pcfg) do{(pcfg)->Cfg &= ~USART_DEV_CFG_PAR_EN;}while(0)  
    
//------------------------------------ֹͣλ------------------------------------
//2��ֹͣλ,����Ϊ1��
#define UsartDevCfg_Is2Stop(pcfg)  ((pcfg)->Cfg & USART_DEV_CFG_2_STOP)
#define UsartDevCfg_Set2Stop(pcfg) do{(pcfg)->Cfg |= USART_DEV_CFG_2_STOP;}while(0)  
#define UsartDevCfg_Clr2Stop(pcfg) do{(pcfg)->Cfg &= ~USART_DEV_CFG_2_STOP;}while(0)    
    
//------------------------------------����λ------------------------------------
//7������λ,����Ϊ8��
#define UsartDevCfg_Is7Bit(pcfg)  ((pcfg)->Cfg & USART_DEV_CFG_7_BIT)
#define UsartDevCfg_Set7Bit(pcfg) do{(pcfg)->Cfg |= USART_DEV_CFG_7_BIT;}while(0)  
#define UsartDevCfg_Clr7Bit(pcfg) do{(pcfg)->Cfg &= ~USART_DEV_CFG_7_BIT;}while(0)     

#endif

