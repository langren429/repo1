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

//��ʾ�������Ƿ���յ�SN��
typedef enum{
	CPAP_LINK_STATUS_NO = 0x00,
	CPAP_LINK_STATUS_YES = 0x02,
}e_CPAP_LINK_STATUS;


//����������ָ��״̬ Ŀǰû������ָ�� ֻ��CMD������
typedef enum{
	CPAP_RECV_STATUS_NO,
	CPAP_RECV_STATUS_CMD,
	CPAP_RECV_STATUS_DATA,
}e_CPAP_RECV_STATUS;


typedef struct{
	e_CPAP_LINK_STATUS link_status; //����������״̬ ���Ƿ���SN��
	e_CPAP_RECV_STATUS recv_status;	
}s_CPAP_STATUS;

//��ʾ��ƽ̨���ݷ�������״̬Ϊ���������� ������
typedef enum{
	SERV_DATA_STATUS_NULL,
	SERV_DATA_STATUS_HEART,
	SERV_DATA_STATUS_CACHE,//��������ָ��
  SERV_DATA_STATUS_DATA	
}e_SERV_DATA_STATUS;

//��ʾ��ƽ̨��������״̬
typedef enum{
	SERV_RECV_STATUS_NULL,  //�����ж����ж�����
	SERV_RECV_STATUS_AT_TCP, //�ж����ж�AT TCPЭ��
  SERV_RECV_STATUS_FTP	 //���ݸ����汻
}e_SERV_RECV_STATUS;

typedef enum{
	SERV_INIT_STATUS_NET_STOP,
	SERV_INIT_STATUS_HARD,  //��ʾGPRS��ҪӲ����λ
	SERV_INIT_STATUS_SOFT,  //��ʾGPRS��Ҫ�����λ
	SERV_INIT_STATUS_NET_NO, //SIM900δ��ʼ��
	SERV_INIT_STATUS_NET_YES, //SIM900��ʼ���ɹ�
	
}e_SERV_INIT_STATUS; //�����ʼ����־

typedef enum{
	SERV_TCP_STATUS_NO,  //TCP��ʼ״̬
//	SERV_TCP_STATUS_INIT,  //
	SERV_TCP_STATUS_YES,  //TCP ���ӳɹ�
	SERV_TCP_STATUS_CLOSED, //TCP���ӶϿ�
}e_SERV_TCP_STATUS; 
//TCP��ʼ����־ ��Ҫ������ʾTCP�������¿�ʼ ��Ϣ�ϴ�

//��ʾGPRS�Ƿ����ϴ�ʱ�����
typedef enum{
  SERV_IDLE_STAUS_NO, //��ʾ���������ڿ���״̬ δ��������
	SERV_IDLE_STAUS_YES,
}s_SERV_IDLE_STATUS; //TCP�Ƿ��ڿ���״̬���Ƿ������ݣ�

typedef enum{
	SERV_LINK_STATUS_NO=0x00, //�豸���ӷ�������ʼ״̬
	SERV_LINK_STATUS_YES, //�豸���ӷ������ɹ�
}e_SERV_LINK_STATUS;
//TCP����״̬��־λ

//GPRS����ģʽ ͸��ģʽ����͸��ģʽ
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
  e_SERV_LINK_STATUS serv_link; //��������״̬ �Ƿ����

	s_SERV_IDLE_STATUS  serv_idle; //GPRS�Ƿ�������ͨ��
	e_SIM900_MODE	 work_mode;
}s_GATEWAY_DEVICE;


#endif
