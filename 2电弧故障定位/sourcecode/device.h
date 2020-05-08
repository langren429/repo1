/*
 * (C) Copyright 2013 Thingtek
 *
 * Written by: randy <shenminghua@thingtek.com>
 */
 
#ifndef _DEVICE_H
#define _DEVICE_H

#include "common.h"
#include <stdint.h>


typedef struct
{
	osThreadId	dev485_recv_task_id;
	osThreadId  dev485_send_task_id;
	
  osThreadId	serv_close_net_task_id;
//	osThreadId	enter_standby_task_id;
	
	osThreadId	GPRS_server_task_id;
//	osThreadId  serv_cpap_task_id;
//	osThreadId  GPRS_Judge_task_id
}s_SWY_485;

typedef struct
{
	osThreadId	lora_recv_task_id;
	osThreadId	lora_send_task_id;
}s_SWY_LORA;

typedef struct
{
	osThreadId	rs232_recv_task_id;
	osThreadId	rs232_send_task_id;
}s_SWY_RS232;

typedef struct{
	uint8_t soft_version[10];
	uint8_t hard_version[10];
	uint8_t	sn[20];	
}s_DATA_INFO;

//表示呼吸机是否接收到SN号
typedef enum{
	CPAP_LINK_STATUS_NO = 0x00,
	CPAP_LINK_STATUS_YES = 0x02,
}e_CPAP_LINK_STATUS;


//呼吸机接收指令状态 目前没有心跳指令 只有CMD及数据
typedef enum{
	CPAP_RECV_STATUS_NO,
	CPAP_RECV_STATUS_CMD,
	CPAP_RECV_STATUS_DATA,
}e_CPAP_RECV_STATUS;


typedef struct{
	e_CPAP_LINK_STATUS link_status; //呼吸机连接状态 即是否获得SN号
	e_CPAP_RECV_STATUS recv_status;	
}s_CPAP_STATUS;

//表示云平台数据发送数据状态为心跳，数据 还是无
typedef enum{
	SERV_DATA_STATUS_NULL,
	SERV_DATA_STATUS_HEART,
	SERV_DATA_STATUS_CACHE,//缓存数据指令
  SERV_DATA_STATUS_DATA	
}e_SERV_DATA_STATUS;

//表示云平台接收数据状态
typedef enum{
	SERV_RECV_STATUS_NULL,  //不在中断中判断数据
	SERV_RECV_STATUS_AT_TCP, //中断中判断AT TCP协议
  SERV_RECV_STATUS_FTP	 //数据更新真被
}e_SERV_RECV_STATUS;

typedef enum{
	SERV_INIT_STATUS_NET_STOP,
	SERV_INIT_STATUS_HARD,  //表示GPRS需要硬件复位
	SERV_INIT_STATUS_SOFT,  //表示GPRS需要软件复位
	SERV_INIT_STATUS_NET_NO, //SIM900未初始化
	SERV_INIT_STATUS_NET_YES, //SIM900初始化成功
	
}e_SERV_INIT_STATUS; //网络初始化标志

typedef enum{
	SERV_TCP_STATUS_NO,  //TCP初始状态
//	SERV_TCP_STATUS_INIT,  //
	SERV_TCP_STATUS_YES,  //TCP 链接成功
	SERV_TCP_STATUS_CLOSED, //TCP链接断开
}e_SERV_TCP_STATUS; 
//TCP初始化标志 主要用来表示TCP网络重新开始 信息上传

//表示GPRS是否处于上传时间过程
typedef enum{
  SERV_IDLE_STAUS_NO, //表示服务器处于空闲状态 未发送数据
	SERV_IDLE_STAUS_YES,
}s_SERV_IDLE_STATUS; //TCP是否处于空闲状态（是否发送数据）

typedef enum{
	SERV_LINK_STATUS_NO=0x00, //设备链接服务器初始状态
	SERV_LINK_STATUS_YES, //设备链接服务器成功
}e_SERV_LINK_STATUS;
//TCP连接状态标志位

//GPRS传输模式 透传模式，非透传模式
typedef enum{
	SIM900_MODE_GPRS,
	SIM900_MODE_CPAP,
}e_SIM900_MODE;

typedef struct{
	e_SERV_RECV_STATUS recv_status;
	e_SERV_INIT_STATUS init_status;
	e_SERV_TCP_STATUS  tcp_status;
	e_SERV_DATA_STATUS  data_status;
}s_SERV_STATUS;



typedef struct gateway_dev
{	
	uint8_t heart_times;
	osThreadId		bg_task_id;
	osThreadId		lcm_task_id;
	s_SWY_LORA	lora_dev;
	s_SWY_485	  dev_485;
	s_SWY_RS232 rs232_dev;
	
	s_DATA_INFO		data_info;
  
	s_CPAP_STATUS	cpap_status;
	s_SERV_STATUS	serv_status;
  e_SERV_LINK_STATUS serv_link; //网络连接状态 是否断网

	s_SERV_IDLE_STATUS  serv_idle; //GPRS是否有数据通信
	e_SIM900_MODE	 work_mode;
}s_GATEWAY_DEVICE;


#endif
