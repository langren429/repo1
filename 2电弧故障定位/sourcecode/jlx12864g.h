/*
 * (C) Copyright 2013 Thingtek
 *
 * Written by: wei <wei@thingtek.com>
 */
 
#ifndef  JLX12864G_H
#define JLX12864G_H
#include "common.h"

#define RS_L GPIO_ResetBits(GPIOB, GPIO_Pin_1) //时钟引脚置底
#define RS_H GPIO_SetBits(GPIOB, GPIO_Pin_1)  //时钟引脚置高

#define SCK_L  GPIO_ResetBits(GPIOC, GPIO_Pin_6) //时钟引脚置底
#define SCK_H  GPIO_SetBits(GPIOC, GPIO_Pin_6)  //时钟引脚置高

#define SDA_L  GPIO_ResetBits(GPIOB, GPIO_Pin_14) //时钟引脚置底
#define SDA_H  GPIO_SetBits(GPIOB, GPIO_Pin_14)  //时钟引脚置高

#define RESET_L  GPIO_ResetBits(GPIOB, GPIO_Pin_10) //时钟引脚置底
#define RESET_H  GPIO_SetBits(GPIOB, GPIO_Pin_10)  //时钟引脚置高

#define CS_L  GPIO_ResetBits(GPIOB, GPIO_Pin_11)  //数据引脚置底
#define CS_H  GPIO_SetBits(GPIOB, GPIO_Pin_11)    //数据引脚置高

#define ROM_CS_L  GPIO_ResetBits(GPIOA, GPIO_Pin_4)  //数据引脚置底
#define ROM_CS_H  GPIO_SetBits(GPIOA, GPIO_Pin_4)    //数据引脚置高

/*
#define RESET_L  GPIO_ResetBits(GPIOB, GPIO_Pin_1) //时钟引脚置底
#define RESET_H  GPIO_SetBits(GPIOB, GPIO_Pin_1)  //时钟引脚置高

#define SCL_L  GPIO_ResetBits(GPIOB, GPIO_Pin_10) //时钟引脚置底
#define SCL_H  GPIO_SetBits(GPIOB, GPIO_Pin_10)  //时钟引脚置高

#define SDA_L  GPIO_ResetBits(GPIOB, GPIO_Pin_11)  //数据引脚置底
#define SDA_H  GPIO_SetBits(GPIOB, GPIO_Pin_11)    //数据引脚置高

#define SDA_IN()   GPIO_PinConfigure(GPIOB, 11, GPIO_IN_PULL_UP, GPIO_MODE_INPUT)//设置SDA为输入引脚
#define SDA_OUT()  GPIO_PinConfigure(GPIOB, 11, GPIO_OUT_PUSH_PULL, GPIO_MODE_OUT50MHZ)  //设置SDA为输出引脚
#define SDA_read()  GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_11)  //读SDA引脚高低电平

#define ROM_IN_L  GPIO_ResetBits(GPIOA, GPIO_Pin_7) //时钟引脚置底
#define ROM_IN_H  GPIO_SetBits(GPIOA, GPIO_Pin_7)  //时钟引脚置高

#define ROM_OUT_L  GPIO_ResetBits(GPIOA, GPIO_Pin_6)  //数据引脚置底
#define ROM_OUT_H  GPIO_SetBits(GPIOA, GPIO_Pin_6)    //数据引脚置高

#define ROM_SCK_L  GPIO_ResetBits(GPIOA, GPIO_Pin_5)  //数据引脚置底
#define ROM_SCK_H  GPIO_SetBits(GPIOA, GPIO_Pin_5)    //数据引脚置高

#define ROM_CS_L  GPIO_ResetBits(GPIOA, GPIO_Pin_4)  //数据引脚置底
#define ROM_CS_H  GPIO_SetBits(GPIOA, GPIO_Pin_4)    //数据引脚置高

#define ROM_OUT_IN()   GPIO_PinConfigure(GPIOA, 6, GPIO_IN_PULL_UP, GPIO_MODE_INPUT)//设置SDA为输入引脚
#define ROM_OUT_OUT()  GPIO_PinConfigure(GPIOA, 6, GPIO_OUT_PUSH_PULL, GPIO_MODE_OUT50MHZ)  //设置SDA为输出引脚
#define ROM_OUT_read()  GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6)  //读SDA引脚高低电平
*/
//void transfer(char data1);
//void start_flag();
//void stop_flag();
//void clear_screen();
//void send_command_to_ROM( char datu );
//static char get_data_from_ROM( );
void get_n_bytes_data_from_ROM(char addrHigh,char addrMid,char addrLow,char *pBuff,char DataLen );
//void display_graphic_128x64(int page,int column,char *dp);
//void display_graphic_32x32(int page,int column,char *dp);
//void display_graphic_16x16(int page,int column,char *dp);
//void display_graphic_8x16(int page,int column,char *dp);
//void display_graphic_5x7(int page,char column,char *dp);

//void display_string_5x7(char y,char x,char *text);
//void initial_lcd();
//void delay_us(uint16_t count);
void SPI1_Init(void);
void SPI1_SetSpeed(u8 SPI_BaudRatePrescaler);
u8 SPI1_ReadWriteByte(u8 TxData);

void transfer_command_lcd(int data1);
void transfer_data_lcd(int data1);
void initial_lcd();
void clear_screen();
void display_graphic_16x16(int page,int column,char reverse,char *dp);
void display_graphic_8x16(int page,char column,char reverse,char *dp);
void display_graphic_5x7(int page,char column,char reverse,char *dp);
void display_string_5x7(char y,char x,char reverse,char *text);
void display_GB2312_string(char y,char x,char reverse,char *text);
void display_GB2312_string_shanshuo(char y,char x,char reverse,char shanshuo_reserve,char site,char *text);
#endif
