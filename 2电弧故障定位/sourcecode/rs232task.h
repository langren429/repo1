/*
 * (C) Copyright 2013 Thingtek
 *
 * Written by: randy <shenminghua@thingtek.com>
 */

#ifndef _RS232_TASK_H
#define _RS232_TASK_H

#include "common.h"

#define	CPAP_DEAULT_PACKAGE_LEN		5
#define	LORA_DEAULT_PACKAGE_LEN		7






typedef struct{
	uint16_t	size;
	uint8_t	data[500];
}s_RS232_RECV_CMD; //232接收到的

typedef struct{
	
	uint8_t		head[4];
	uint8_t		seq[2];
	uint8_t		type[2];
	uint8_t		cmd[2];
	uint8_t		length[2];
	uint8_t		data[100];
}s_RS232_RECV_PACKAGE;

typedef struct{
	uint8_t		head[4];
	uint8_t		seq[2];
	uint8_t		type[2];
	uint8_t		cmd[2];
	uint8_t		length[2];
	uint8_t		data[100];
}s_RS232_SEND_PACKAGE;

//uint8_t cal_cpap_crc(s_CPAP_PACKAGE cpap_package);
void rs232_recv_task(void const *argument);
void rs232_send_task(void const *argument);
void RS_232_Dev_init(void);
uint16_t CRC_ADD(uint8_t *puchMsg, uint16_t usDataLen);

#endif

