#include "common.h"
#include "rtcdog.h"
uint8_t rtc_feed_dog;
extern uint8_t program_runing;//�������������б�־λ

extern uint16_t sleep_time_num;
extern uint16_t send_screen_again;
extern uint8_t xxx;
extern uint8_t enter_stopmode_flag;//Ϣ������¼��� �Ƿ����͹��� ��־
//RTC????
static void RTC_NVIC_Config(void)
{    
    NVIC_InitTypeDef NVIC_InitStructure;
	  EXTI_InitTypeDef EXTI_InitStructure;  
	
	  EXTI_ClearITPendingBit(EXTI_Line17);  
    EXTI_InitStructure.EXTI_Line = EXTI_Line17;  
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;  
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;  
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;  
    EXTI_Init(&EXTI_InitStructure);  

  
  	NVIC_InitStructure.NVIC_IRQChannel = RTCAlarm_IRQn;        //RTCȫ���ж�RTC_IRQChannel RTC_IRQn RTCAlarm_IRQChannel
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;    //?????1?,????3?
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;   
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;        //???????
    NVIC_Init(&NVIC_InitStructure);        //??NVIC_InitStruct???????????NVIC???
}

//RTC?????:???????LSI?RTC????????40000??1Hz
//??????WORK_TIMES
void RTC_Alarm_Configuration(void)
{
	/* Enable PWR and BKP clocks */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
	/* Allow access to BKP Domain */
	PWR_BackupAccessCmd(ENABLE);	
	/* Reset Backup Domain */
	BKP_DeInit();
	

    /* RTC clock source configuration ----------------------------------------*/
	/* Enable the LSI OSC */
  	RCC_LSICmd(ENABLE);
    /* Wait till LSI is ready */
    while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET)
    {
    }
    /* Select the RTC Clock Source */
    RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);
    /* Enable the RTC Clock */
    RCC_RTCCLKCmd(ENABLE);
	/* Wait for RTC registers synchronization */
	RTC_WaitForSynchro();
	/* Wait until last write operation on RTC registers has finished */
	RTC_WaitForLastTask(); 
	/* ??RTC????*/
	RTC_ITConfig(RTC_IT_ALR, ENABLE);	
	/* Wait until last write operation on RTC registers has finished */
	RTC_WaitForLastTask();	
	/* Set RTC prescaler: set RTC period to 1sec */
	RTC_SetPrescaler(40000);	
	/* Wait until last write operation on RTC registers has finished */
	RTC_WaitForLastTask();
		
	//????
	RTC_NVIC_Config();

	//????WORK_TIMES
	RTC_SetAlarm(RTC_GetCounter() + 20);
	RTC_WaitForLastTask();
}


void RTC_Enter_StopMode(u32 s)
{

	
	  if(program_runing == 0)//�����ڵ͹���״̬�±�����
			{



					 
				SystemInit();
			
			//	GPIO_PinWrite(GPIOC, 5, 1);//LED1 �͵�ƽ��Ч
			//	GPIO_PinWrite(GPIOA, 4, 1);//���������� �ߵ�ƽ��Ч

			//	sleep_time_num = 0;//˯�߼�ʱ����
			//	send_screen_again = 1;//���·������õ���Ļ
			//	 PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);
				 rtc_feed_dog = 1; //ι����־λ
		 }
		 RTC_SetAlarm(RTC_GetCounter() + s);
     RTC_WaitForLastTask();	


}

//??????
 //RTC_IRQHandler
void RTCAlarm_IRQHandler(void)
{
 //	if(RTC_GetFlagStatus(RTC_IT_ALR))
	if(RTC_GetITStatus(RTC_IT_ALR)!= RESET)//????
 {
  	RTC_ClearFlag(RTC_FLAG_SEC);
	  EXTI_ClearITPendingBit(EXTI_Line17);  // ?EXTI_Line17??? 
	  RTC_ClearITPendingBit(RTC_IT_ALR);  //?????
	  RTC_WaitForLastTask(); 
	  EXTI_ClearITPendingBit(EXTI_Line0 | EXTI_Line1 | EXTI_Line4 | EXTI_Line10 | EXTI_Line11 );     // ?? EXIT ????
	 	USART_ClearITPendingBit(USART1, USART_IT_RXNE);
		USART_ClearITPendingBit(USART2, USART_IT_RXNE);
		if(PWR_GetFlagStatus(PWR_FLAG_WU) != RESET)
	 {
		 PWR_ClearFlag(PWR_FLAG_WU);//????
	 }

	  
	 // RTC_Enter_StopMode(20);//????

	 	  if(program_runing == 0)//�����ڵ͹���״̬�±�����
			{
				SystemInit();
				enter_stopmode_flag = 0x01;//Ϣ��ʱ�䳬��30�� ����͹���ģʽ
//				RTC_WaitForSynchro();
		//		program_runing = 1;
	//			rtc_feed_dog = 1; //ι����־λ
//				xxx = !xxx;
//	      GPIO_PinWrite(GPIOC, 4, xxx);//LED1 �͵�ƽ��Ч
//				IWDG_ReloadCounter();				//ι��	
//				PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);
		 }
		//	IWDG_ReloadCounter();				//ι��	
	 	 RTC_SetAlarm(RTC_GetCounter() + 20);
     RTC_WaitForLastTask();	
 }                 
}




 


