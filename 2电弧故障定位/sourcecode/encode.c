//#include "stm32f10x.h"
#include "encode.h"
//#include "misc.h"
//#include "nvic.h"
//#include "sys.h" 
//#include "delay.h"
#include "common.h"

s32 currentCount;
uint8_t Init_Success;
//s16 count;
#define MAX_COUNT 10000
#define ENCODER_TIM_PERIOD 0xffff

void TIM4_Mode_Config(void)
{
	
	/*GPIO_PortClock(GPIOB, true);
	GPIO_PinConfigure(GPIOB, 0, GPIO_OUT_PUSH_PULL, GPIO_MODE_OUT10MHZ);
	GPIO_PortClock(GPIOC, true);
	GPIO_PinConfigure(GPIOC, 5, GPIO_IN_FLOATING , GPIO_MODE_INPUT);*/
	
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_ICInitTypeDef TIM_ICInitStructure;   	

	//PB6 ch1  A,PB7 ch2 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);//ʹ��TIM4ʱ��	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);//ʹ��GPIOBʱ��
	
	/*GPIO_PortClock(GPIOB, true);
	GPIO_PinConfigure(GPIOB, 6, GPIO_IN_FLOATING , GPIO_MODE_INPUT);
	GPIO_PinConfigure(GPIOB, 7, GPIO_IN_FLOATING , GPIO_MODE_INPUT);//PB6 PB7��������*/
	GPIO_StructInit(&GPIO_InitStructure);//��GPIO_InitStruct�еĲ�����ȱʡֵ����
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;         
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//PB6 PB7��������	
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);                           

	//NVIC_Config(2);

	TIM_DeInit(TIM4);
	TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
	TIM_TimeBaseStructure.TIM_Period = 65535;  //�趨��������װֵ   TIMx_ARR = 359*4
	TIM_TimeBaseStructure.TIM_Prescaler = 0; //TIM3ʱ��Ԥ��Ƶֵ
	TIM_TimeBaseStructure.TIM_ClockDivision =TIM_CKD_DIV1 ;//����ʱ�ӷָ� T_dts = T_ck_int	
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //TIM���ϼ��� 
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);              
                 
	TIM_EncoderInterfaceConfig(TIM4, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising ,TIM_ICPolarity_Rising);//ʹ�ñ�����ģʽ3�������½�������
	TIM_ICStructInit(&TIM_ICInitStructure);//���ṹ���е�����ȱʡ����
	TIM_ICInitStructure.TIM_ICFilter = 6;  //ѡ������Ƚ��˲��� 
	TIM_ICInit(TIM4, &TIM_ICInitStructure);//��TIM_ICInitStructure�е�ָ��������ʼ��TIM3
	
//	TIM_ARRPreloadConfig(TIM4, ENABLE);//ʹ��Ԥװ��
	TIM_ClearFlag(TIM4, TIM_FLAG_Update);//���TIM3�ĸ��±�־λ
	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);//���и����ж�
	//Reset counter
	TIM4->CNT = 0;//

	TIM_Cmd(TIM4, ENABLE);   //����TIM4��ʱ��

}


void TIM4_Init(void)
{
  TIM4_Mode_Config();
}



s16  Enc_GetCount(void)
{
  static  u16   lastCount = 0;
  u16  curCount = TIM4->CNT;
  s32 dAngle = curCount - lastCount;
  if(dAngle >= MAX_COUNT){
    dAngle -= ENCODER_TIM_PERIOD;
  }else if(dAngle < -MAX_COUNT){
    dAngle += ENCODER_TIM_PERIOD;
  }
  lastCount = curCount;
  return (s16)dAngle;
}

void read_encoder_task(void const *argument)
 {		
	
	//u16 count;//????? 
	//delay_init();	    	 //???????	  
	//uart_init(115200);	 //??????115200
	//EXTI_PA1_Config();//???Z?????????????
	read_flash2screen();//��ʼ����Ļ 
	TIM4_Init();//???????????
 //	LED_Init();			      //LED?????
//	KEY_Init();          //?????????????
	Init_Success = 1; 

 	while(1)
	{
	//	count = TIM4->CNT/4;//?????
		currentCount += Enc_GetCount();
		osDelay(10);
	//	delay_ms(10);//??1s?????????,????????  ??????
	//	printf("count = %d\n",count);
	}	 
 }

