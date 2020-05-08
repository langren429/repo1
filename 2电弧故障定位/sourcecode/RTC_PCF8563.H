/*
 * (C) Copyright 2013 Thingtek
 *
 * Written by: wei <wei@thingtek.com>
 */
 
#ifndef RTC_PCF8563_H
#define RTC_PCF8563_H

#include "common.h"

//PCF8563引脚设置
#define SCL_L  GPIO_ResetBits(GPIOB, GPIO_Pin_8) //时钟引脚置底
#define SCL_H  GPIO_SetBits(GPIOB, GPIO_Pin_8)  //时钟引脚置高

#define SDA_L  GPIO_ResetBits(GPIOB, GPIO_Pin_9)  //数据引脚置底
#define SDA_H  GPIO_SetBits(GPIOB, GPIO_Pin_9)    //数据引脚置高

#define SDA_IN()   GPIO_PinConfigure(GPIOB, 9, GPIO_IN_PULL_UP, GPIO_MODE_INPUT)//设置SDA为输入引脚
#define SDA_OUT()  GPIO_PinConfigure(GPIOB, 9, GPIO_OUT_PUSH_PULL, GPIO_MODE_OUT50MHZ)  //设置SDA为输出引脚
#define SDA_read  GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_9)  //读SDA引脚高低电平

//PCF8563地址声明
#define  PCF_DEVICE_ADD  0xa2
#define  STATUS_REG1  0X00
#define  STATUS_REG2  0x01
#define  CLKOUT_REG   0x0d
#define  TIMER_CTL    0x0e
#define  TIMER_Count  0x0f
#define  SEC    0x02
#define  MIN    0x03
#define  HOUR   0x04
#define  DAY    0x05
#define  WEEK   0x06
#define  MONTH  0x07
#define  YEAR   0x08
#define  Min_Alarm   0x09
#define  Hour_Alarm  0x0a
#define  Day_Alarm   0x0b
#define  Week_Alarm  0x0c

typedef struct{
  uint8_t year;
	uint8_t mouth;
	uint8_t week;
	uint8_t day;
	uint8_t hour;
	uint8_t min;
	uint8_t sec;
}PCF8563_Time;

//函数声明区域
void PCF8563_read_time(uint8_t *pcf8563_time);
void PCF8563_write_time(uint8_t set_time[]);
void PCF8563_WriteOneByte(uint8_t DeviceAdd,uint8_t WriteAdd,uint8_t SendByte);
uint8_t PCF8563_ReadOneByte(uint8_t DeviceAdd,uint8_t READAdd);
void PCF8563_init(void);
#endif
