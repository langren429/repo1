/*
 * (C) Copyright 2013 Thingtek
 *
 * Written by: randy <shenminghua@thingtek.com>
 1.软件库函数更新之后，程序烧写之前需要将UART_STM32F10x.c 
 串口1 3 的buffer最大尺寸改为1024 同时将UART_IRQHandler函数中的val = (ptr_uart->rx_buffer_size - 1) - val;话屏蔽

 2.在看门狗库函数IWDG与时钟RCC库函数中增加#include "stm32f10x_conf.h"包含文件
 
 2016.4.20软件更改功能：
GPRS模块不主动连接云平台，所有的任务均有呼吸机发起。
上电及断网不主动联网，呼吸机发起联网任务。

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



osThreadDef(read_encoder_task, osPriorityNormal, 1, SETTLE_STKSIZE);//add by wj编码器

osThreadDef(copy_task, osPriorityHigh, 1, SETTLE_STKSIZE);//add by wj 蜂鸣器
osThreadDef(tcp_send_task, osPriorityNormal, 1, SETTLE_STKSIZE);//add by wj 蜂鸣器

osThreadDef(electric_task, osPriorityNormal, 1, SETTLE_STKSIZE);//add by wj 放电检测
//osThreadDef(GPRS_server_task, osPriorityNormal, 1, SETTLE_STKSIZE);//add by wj GPRS与服务器之间通讯
//osThreadDef(beep_task, osPriorityNormal, 1, CPAP_STKSIZE);//add by wj
//osThreadDef(test_task, osPriorityNormal, 1, CPAP_STKSIZE);//add by wj

osThreadDef(lcm_task, osPriorityNormal, 1, BG_STKSIZE);

osThreadDef(bg_task, osPriorityBelowNormal, 1, BG_STKSIZE);
//串口屏接受发送任务
osThreadDef(tcp_recv_task, osPriorityAboveNormal, 1, SETTLE_STKSIZE);//cpap_recv_task
osThreadDef(lora_send_task, osPriorityNormal, 1, SETTLE_STKSIZE);//cpap_send_task
//上位机接受发送任务
osThreadDef(rs232_recv_task, osPriorityAboveNormal, 1, SETTLE_STKSIZE);//cpap_recv_task
osThreadDef(rs232_send_task, osPriorityNormal, 1, SETTLE_STKSIZE);//cpap_send_task

//TCP任务处理 REC send
osThreadDef(dev485_recv_task, osPriorityAboveNormal, 1, SERV_STKSIZE);

osThreadDef(dev485_send_task, osPriorityNormal, 1, SERV_STKSIZE);

//osThreadDef(serv_cpap_task, osPriorityNormal, 1, SERV_STKSIZE);
//优先级需要高于cpap_recv_task
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
uint8_t gateway_address = 0x01;//本机地址

extern  s_SCREEN_SET SCREEN_SET;

	//u32 i=0;
	//u8 temp=0;
	


int main()
{
 // DBGMCU_Config(DBGMCU_STOP, ENABLE);

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //设置NVIC中断分组2:2位抢占优先级，2位响应优先级 

	all_init();

 // RTC_Alarm_Configuration();
//	WWDG_Init(0X7F,0X5F,WWDG_Prescaler_8);//使用窗口看门狗

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
//wj测试		
		if( osThreadCreate( osThread(tcp_recv_task), NULL ) == NULL){
//			//DEBUG
//			LED_ON;
		}

//   
//		if( osThreadCreate( osThread(dev485_send_task),NULL ) == NULL){
////			//DEBUG
////			LED_ON;
//		}
		
		//wj测试		
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
			}//add by wj 编码器任务
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
