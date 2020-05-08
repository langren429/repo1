/*
 * (C) Copyright 2013 Thingtek
 *
 * Written by: randy <shenminghua@thingtek.com>
 1.����⺯������֮�󣬳�����д֮ǰ��Ҫ��UART_STM32F10x.c 
 ����1 3 ��buffer���ߴ��Ϊ1024 ͬʱ��UART_IRQHandler�����е�val = (ptr_uart->rx_buffer_size - 1) - val;������

 2.�ڿ��Ź��⺯��IWDG��ʱ��RCC�⺯��������#include "stm32f10x_conf.h"�����ļ�
 
 2016.4.20������Ĺ��ܣ�
GPRSģ�鲻����������ƽ̨�����е�������к���������
�ϵ缰����������������������������������

 */
#include "main.h"
#include "backgrouptask.h"
#include "tcptask.h"
#include "dev485task.h"
#include "rs232task.h"
#include "common.h"
#include "device.h"

#include "encode.h"
//#include "motor.h"
//#include "exti.h"
//#include "RTC_PCF8563.h"
//#include "adctest.h"

#include "stdio.h"
#include "stdbool.h"
#include "jlx12864g.h"
#include "lcmtask.h"

#define BG_STKSIZE		0
#define SETTLE_STKSIZE	0
#define SERV_STKSIZE	0
#define TCP_STKSIZE   0



osThreadDef(read_encoder_task, osPriorityNormal, 1, SETTLE_STKSIZE);//add by wj������

osThreadDef(copy_task, osPriorityHigh, 1, SETTLE_STKSIZE);//add by wj ������
osThreadDef(tcp_send_task, osPriorityNormal, 1, SETTLE_STKSIZE);//add by wj ������

osThreadDef(electric_task, osPriorityNormal, 1, SETTLE_STKSIZE);//add by wj �ŵ���
//osThreadDef(GPRS_server_task, osPriorityNormal, 1, SETTLE_STKSIZE);//add by wj GPRS�������֮��ͨѶ
//osThreadDef(beep_task, osPriorityNormal, 1, CPAP_STKSIZE);//add by wj
//osThreadDef(test_task, osPriorityNormal, 1, CPAP_STKSIZE);//add by wj

osThreadDef(lcm_task, osPriorityNormal, 1, BG_STKSIZE);

osThreadDef(bg_task, osPriorityBelowNormal, 1, BG_STKSIZE);
//���������ܷ�������
osThreadDef(tcp_recv_task, osPriorityAboveNormal, 1, SETTLE_STKSIZE);//cpap_recv_task
osThreadDef(lora_send_task, osPriorityNormal, 1, SETTLE_STKSIZE);//cpap_send_task
//��λ�����ܷ�������
osThreadDef(rs232_recv_task, osPriorityAboveNormal, 1, SETTLE_STKSIZE);//cpap_recv_task
osThreadDef(rs232_send_task, osPriorityNormal, 1, SETTLE_STKSIZE);//cpap_send_task

//TCP������ REC send
osThreadDef(dev485_recv_task, osPriorityAboveNormal, 1, SERV_STKSIZE);

osThreadDef(dev485_send_task, osPriorityNormal, 1, SERV_STKSIZE);

//osThreadDef(serv_cpap_task, osPriorityNormal, 1, SERV_STKSIZE);
//���ȼ���Ҫ����cpap_recv_task
//osThreadDef(serv_close_net_task, osPriorityNormal, 1, TCP_STKSIZE);
//osPriorityBelowNormal
//osThreadDef(enter_standby_task, osPriorityNormal, 1, TCP_STKSIZE);

s_GATEWAY_DEVICE		gateway_dev;
uint8_t pcf8563_time[6];
uint8_t  cmd_respond[100];
uint16_t adcx;
uint32_t voltage_temp;
uint32_t temp_set[12];
uint16_t screen_set[14];
//int32_t x;
uint8_t gateway_address = 0x01;//������ַ

extern  s_SCREEN_SET SCREEN_SET;

	//u32 i=0;
	//u8 temp=0;
	


int main()
{
 // DBGMCU_Config(DBGMCU_STOP, ENABLE);

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ� 

	all_init();

 // RTC_Alarm_Configuration();
//	WWDG_Init(0X7F,0X5F,WWDG_Prescaler_8);//ʹ�ô��ڿ��Ź�

//	ADC_test_Init();
	//adcx = Get_Adc_Average(ADC_Channel_10,10);
  //voltage_temp=(float)(adcx*(3.3/4096)*50);


	if(osKernelInitialize() != osOK){
		//DEBUG
	}
//	
	if(osKernelRunning()){
		//DEBUG
//		 	if(osThreadCreate(osThread(enter_standby_task), NULL) == NULL){
////			//DEBUG
////			LED_ON;
//  		}
					
		if( osThreadCreate( osThread(bg_task), NULL ) == NULL){
			//DEBUG
		//	LED_ON;
		}
		
	/*	if( osThreadCreate( osThread(lcm_task), NULL ) == NULL){
			//DEBUG
		//	LED_ON;
		}*/

		if( osThreadCreate( osThread(copy_task), NULL ) == NULL){
			//DEBUG
	//		LED_ON;
		}//add by wj 
		
//		if( osThreadCreate( osThread(electric_task), NULL ) == NULL){
//			//DEBUG
//	//		LED_ON;
//		}//add by wj 
		
		if( osThreadCreate( osThread(tcp_send_task), NULL ) == NULL){
			//DEBUG
	//		LED_ON;
		}//add by wj 
//				
//		if( osThreadCreate( osThread(lora_send_task), NULL ) == NULL){
////			//DEBUG
////			LED_ON;
//		}

//		if( osThreadCreate( osThread(dev485_recv_task),NULL ) == NULL){
//			//DEBUG
//		//	LED_ON;
//		}
//wj����		
		if( osThreadCreate( osThread(tcp_recv_task), NULL ) == NULL){
//			//DEBUG
//			LED_ON;
		}

//   
//		if( osThreadCreate( osThread(dev485_send_task),NULL ) == NULL){
////			//DEBUG
////			LED_ON;
//		}
		
		//wj����		
//		if( osThreadCreate( osThread(rs232_recv_task), NULL ) == NULL){
////			//DEBUG
////			LED_ON;
//		}

//   
//		if( osThreadCreate( osThread(rs232_send_task),NULL ) == NULL){
////			//DEBUG
////			LED_ON;
//		}
		
		
/*		if( osThreadCreate( osThread(GPRS_server_task),NULL ) == NULL){
//			//DEBUG
//			LED_ON;
		}*/
		
		/*		if( osThreadCreate( osThread(read_encoder_task), NULL ) == NULL){
			//DEBUG
	//		LED_ON;
			}//add by wj ����������
        	*/	
//		if( osThreadCreate( osThread(serv_cpap_task),NULL ) == NULL){
//			//DEBUG
//			LED_ON;
//		}
//		
//		if(osThreadCreate(osThread(serv_close_net_task), NULL) == NULL){
//			//DEBUG
//			LED_ON;
//		}


//		
		if(osKernelStart() != osOK){
			//DEBUG
			LED_ON;
		}
	}	
	//TIM3_PWM_Init(300,0);//899

	//TIM_SetCompare1(TIM3,100);//300
//	while(1);
	return 0;
}
