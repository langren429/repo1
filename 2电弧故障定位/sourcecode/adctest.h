#ifndef __ADCTEST_H__
#define __ADCTEST_H__
//#include "sys.h" 
//#include "stm32f10x.h"
//void TIM3_PWM_Init(u16 arr,u16 psc);
void ADC_test_Init(void);
uint16_t Get_Adc(uint8_t ch);
uint16_t Get_Adc_Average(uint8_t ch,uint8_t times);

void  Adc_GPIO_Init(void);
void	Adc_single_Init(void);
void	ADC1_DMA_Init(void);
void	TIM2_Configration(void);
void Adc1_Multi_Init(void);
void Adc2_Multi_Init(void);  
void ADC12_Init(void);

#endif
