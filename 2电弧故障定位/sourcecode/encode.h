#ifndef __ENCODE_H__
#define __ENCODE_H__
//#include "sys.h" 
#include "stm32f10x.h"
extern s32 currentCount;
void TIM4_Mode_Config(void);//ģʽ����
void TIM4_Init(void);//��ʼ��
s16  Enc_GetCount(void);
void read_encoder_task(void const *argument);


#endif
