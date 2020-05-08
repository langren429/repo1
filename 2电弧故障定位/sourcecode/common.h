/*
 * (C) Copyright 2013 Thingtek
 *
 * Written by: randy <shenminghua@thingtek.com>
 */
 
#ifndef _COMMON_H
#define _COMMON_H

#include "cmsis_os.h"
#include "STM32F10x.h"

#include "GPIO_STM32F10x.h"
#include "DMA_STM32F10x.h"
#include "stm32f10x_iwdg.h"
#include "stm32f10x_flash.h"
#include "Driver_UART.h"

#include "RTE_Device.h"
#include "RTE_Components.h"
#include "stm32f10x_rcc.h"

#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include "device.h"

#include "backgrouptask.h"
#include "tcptask.h"
#include "dev485task.h"
#include "rs232task.h"
#include "exti.h"
#include "motor.h"
#include "rtcdog.h"
//#include "RTC_PCF8563.h"
#include "adctest.h"
//#include "jlx12864g.h"
#include "sram.h"
#include "dac.h"
#include "mcp4017.h"
//PCB_VERSION为0表示旧版 为1表示新版
#define PCB_VERSION  2

#define Protocol_Version  0x03
 
#define  SUCCESS  0X01
#define  FAILURE  0X00

#define ONE_PACK_NUM 500

#define MODULE_ID   299

#if PCB_VERSION==1

	#define LED_ON		GPIO_PinWrite(GPIOA, 6, 0)
	#define LED_OFF		GPIO_PinWrite(GPIOA, 6, 1)

	#define LED_Toggle	{	GPIO_PinRead(GPIOA, 6)?\
												GPIO_PinWrite(GPIOA, 6, 0):\
												GPIO_PinWrite(GPIOA, 6, 1);\
												}

#elif PCB_VERSION==2

	#define LED_ON		GPIO_PinWrite(GPIOA, 2, 0)
	#define LED_OFF		GPIO_PinWrite(GPIOA, 2, 1)

	#define LED_Toggle	{	GPIO_PinRead(GPIOA, 2)?\
												GPIO_PinWrite(GPIOA, 2, 0):\
												GPIO_PinWrite(GPIOA, 2, 1);\
												}

#endif
												
#define LED1_ON		GPIO_PinWrite(GPIOC, 14, 0)
#define LED1_OFF		GPIO_PinWrite(GPIOC, 14, 1)					

#define LED2_ON		GPIO_PinWrite(GPIOC, 15, 0)
#define LED2_OFF		GPIO_PinWrite(GPIOC, 15, 1)		

#define LED3_ON		GPIO_PinWrite(GPIOD, 2, 0)
#define LED3_OFF		GPIO_PinWrite(GPIOD, 2, 1)		

#define RST_OFF		GPIO_PinWrite(GPIOA, 7, 1)
#define RST_ON		GPIO_PinWrite(GPIOA, 7, 0)		

#define BBB_Toggle	{	GPIO_PinRead(GPIOB, 1)?\
											GPIO_PinWrite(GPIOB, 1, 0):\
											GPIO_PinWrite(GPIOB, 1, 1);\
											}												

#define USERSET_ADDRESS 0X0807F000  //第254页起始地址			
#define STANDARD_MEASURE_ADDRESS 0X08030000 //测量数据起始地址 第一条为标准数据
#define WAKE_UP_MEASURE_ADDRESS 0X08030800 												
//每次测量数据大小为0x30 48字节		
#define SCREEN_SET_ADDRESS 0X0807D000 //屏幕设置			
#define LCM_SET_ADDRESS 0X0807D000 //屏幕设置												
#define PACKAGE_SIZE 5		
#define  DNS_ENABLE 1												
typedef struct{
	uint8_t tcpip[4];
	uint8_t tcpport[2];
	uint8_t netapnlen;
	char netapn[50];
	uint8_t netuserlen;
	char netuser[30];
	uint8_t netkeylen;
	char netkey[30];
	uint8_t flag;
}SERV_USERSET;

typedef struct{
	uint8_t sim;
	uint8_t signal;
	uint16_t  max_tcp_send_length;
}SERV_SIM;

struct Date
{
 int year;
 int month;
 int day; 
};

typedef struct{
	uint8_t Minute;
	uint8_t Second;
	uint16_t Msecond;
	uint16_t Usecond;
}TIME_RECORD;


void sim900_restart(void);
void per_init(void);
void sim900_power_init(void);
void sim900_power_off();
void device_init(s_GATEWAY_DEVICE *gateway_dev);
uint8_t Compare_SN(uint8_t arry1[],uint8_t arry2[],uint8_t num);
void BSP_IWDG_Init(uint16_t Dog_time);
void flash_writeset();
void flash_writedata(uint32_t Page_Address, uint32_t *measure_data);
void flash_writedata_16(uint32_t Page_Address, uint16_t *measure_data);
void flash_writepage(uint32_t Page_Address, uint32_t *measure_data);
uint32_t flash_read_data(uint32_t Page_Address);
void flash_read_item(uint32_t Page_Address);
uint16_t flash_read_data16(uint32_t Page_Address);
void flash_read_page(uint32_t Page_Address);
//int32_t sum(uint8_t y,uint8_t m,uint8_t d);
int32_t day_diff(uint8_t year_start, uint8_t month_start, uint8_t day_start, uint8_t year_end, uint8_t month_end, uint8_t day_end);
struct Date nextNDay(struct Date d,uint8_t n);
void copy_task(void const *argument);
void tcp_send_task(void const *argument);
void electric_task(void const *argument);


void test_task(void const *argument);
void all_init(void);
void read_flash2screen();
void Sys_Enter_Standby(void);
void GPRS_server_task(void const *argument);
unsigned short cal_serv_crc(unsigned char *message, unsigned int len);
void delay_ms(u16 time);
//void DMA1CH7_CopyMem(u32 src,u32 des,u32 len,u32 datasize);
//void sim900_power_init1(void);
#endif

