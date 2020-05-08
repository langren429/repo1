#ifndef __ENCODE_H__
#define __ENCODE_H__
//#include "sys.h" 
#include "stm32f10x.h"
extern s32 currentCount;
void TIM4_Mode_Config(void);//模式配置
void TIM4_Init(void);//初始化
s16  Enc_GetCount(void);
void read_encoder_task(void const *argument);


#endif
