/*
 * (C) Copyright 2013 Thingtek
 *
 * Written by: randy <shenminghua@thingtek.com>
 */
 
#include "backgrouptask.h"
//#include "breathtask.h"
#include "common.h"
#include "stm32f10x_flash.h"

//variabl
extern	s_GATEWAY_DEVICE gateway_dev;

//extern  s_SCREEN_SET SCREEN_SET;
//bgtask
void bg_task(void const *argument){
 
	
	gateway_dev.bg_task_id = osThreadGetId();
	while(gateway_dev.bg_task_id == NULL);
	osDelay(1000);

				
	while(1){
		
		IWDG_ReloadCounter();				//ι��

	  osDelay(6000);	//��ʱ�ȴ�6S �൱�ڶ�ʱ��

		//�˴�����Ӻ�̨������:IP��ַ��⡢�º�ǿ�ȼ���	
		//
		
	}
}



