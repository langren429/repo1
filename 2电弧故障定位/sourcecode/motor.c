#include "common.h"
//#include "motor.h"



void TIM3_PWM_Init(u16 arr,u16 psc)
{
 
  GPIO_InitTypeDef GPIO_InitStructure;
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
  TIM_OCInitTypeDef TIM_OCInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	
  
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);//TIM3��
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|
                         RCC_APB2Periph_AFIO,ENABLE);//GPIOB��
	
	GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3, ENABLE); //TIM3_CH1->PB4 ������ӳ��

  //GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);//JTAG����PB4�� ��Ҫ�ر�
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;  //����PB4
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //���
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB,&GPIO_InitStructure); //???GPIO
	
	//GPIO_ResetBits(GPIOA, GPIO_Pin_5 );
	//GPIO_SetBits(GPIOA, GPIO_Pin_5 );
 
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn; 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
	NVIC_Init(&NVIC_InitStructure);
  /* GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8; //PA8?DS0
   //???PA8?????DS0???,???PA8?????
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
   GPIO_Init(GPIOA,&GPIO_InitStructure);*/
 
  TIM_TimeBaseStructure.TIM_Period = arr; //�Զ�װ������ֵ
  TIM_TimeBaseStructure.TIM_Prescaler = psc; //����Ԥ��Ƶֵ
  TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //���ϼ���ģʽ
  TIM_TimeBaseInit(TIM3,&TIM_TimeBaseStructure); //TIM3
 
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //PWMģʽ2
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //������Ը�
  TIM_OC1Init(TIM3,&TIM_OCInitStructure); //TIM3
 
  TIM_OC1PreloadConfig(TIM3,TIM_OCPreload_Enable); //ʹ��Ԥװ�ؼĴ���
  TIM_Cmd(TIM3,ENABLE); //ʹ��TIM3
}

