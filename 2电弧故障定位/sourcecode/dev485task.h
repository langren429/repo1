/*
 * (C) Copyright 2013 Thingtek
 *
 * Written by: randy <shenminghua@thingtek.com>
 */
 
#ifndef _SERVER_TASK_H
#define _SERVER_TASK_H

#include "common.h"

//��С��ת����������
#define Tranversel16(X)  (((X&0xff00)>>8)|((X&0x00ff)<<8))
#define Tranversel32(X)  (((X&0xff000000)>>24)|((X&0x00ff0000)>>8)|((X&0x0000ff00)<<8)|((X&0x000000ff)<<24))

#define RS485_REON()  GPIO_ResetBits(GPIOA, GPIO_Pin_11) //ʱ�������õ�
#define RS485_REOFF()  GPIO_SetBits(GPIOA, GPIO_Pin_11)  //ʱ�������ø�

#define RS485_DEON()  GPIO_SetBits(GPIOA, GPIO_Pin_12)  //ʱ�������ø�
#define RS485_DEOFF()  GPIO_ResetBits(GPIOA, GPIO_Pin_12) //ʱ�������õ�

#pragma pack(1)
//��������
typedef enum 
{	
	SERVER_TCP_NON,
	//����ͼ��ͳ������
  SERVER_TCP_ONE = 0x3101,
  SERVER_TCP_SEVEN = 0x3102,
  SERVER_TCP_THIRTY = 0x3103,	
	SERVER_TCP_1K = 0x3104,
	SERVER_TCP_PAST_RECORD = 0x3105
}e_serv_data_type;

//=================SERV protocal======================
typedef enum 
{	
	GPRS_CMD_IDENTITY = 0x1101,//�����������֤
	GPRS_CMD_DATA_TODAY = 0x1102,	//���������ϱ�
	GPRS_CMD_REQ_FIRST_DATE = 0x1103, //����������������ʷ��������
	GPRS_CMD_REQ_DATA = 0x1104, //������������ʷ����
	SERV_CMD_REQ_DATA = 0x2101, //������������ʷ����	
	SERV_CMD_SET_SETTLE = 0x2102,  //���ó����ǹ�������
	SERV_CMD_END_TCP = 0x2103,//����ͨѶָ��

}e_cmd_type;

typedef enum{
	SERV_CMD_NON = 0x0000,
	SERV_CMD_DATA, //�ϴ�����ͳ��
	SERV_CMD_ACK,
	SERV_CMD_GPRS_UP = 0x1101, //SN�ϴ�
}e_serv_gprs_cmd_type;

//����Ӧ������
typedef enum 
{	
	SERV_CMD_ACK_NON,
	SERV_CMD_ACK_HEART = 0x1102,	//����ָ��
	SERV_CMD_ACK_WARNING = 0x2102, //�澯
	SERV_CMD_ACK_CAPTION = 0x2103, //��ʾ	
	SERV_CMD_ACK_CACHE_ONE = 0x3201,  //����ָ��
	SERV_CMD_ACK_CACHE_SEVEN = 0x3202,
	SERV_CMD_ACK_CACHE_THIRTY = 0x3203,	
	SERV_CMD_ACK_CACHE_1K = 0x3204,
	SERV_CMD_ACK_CACHE = 0X4101,   //Ԥ���ֶ�7�ֽ�
	SERV_CMD_ACK_PRE_CACHE = 0X4102,  //Ԥ���ֶ�64�ֽ�
}e_serv_cmd_ack_type;

//�Ƿ���Ҫ�жϻ���ָ��
typedef enum 
{	
	SERVER_CACHE_NON,
  SERVER_CACHE_YES,
  SERVER_CACHE_NO,
	
}e_serv_cache_judge;

typedef enum{
	SERVER_IDENTITY_NULL = 0x0000,//δ���������֤
	SERVER_IDENTITY_SUCCESS = 0x0001,//�����֤�ɹ�
	SERVER_IDENTITY_FAIL = 0x0002,//�����֤ʧ��
}e_SERVER_IDENTITY_STATE;


typedef enum{
	GPRS_REQ_FIRST_DATE_NULL = 0x0000,//δ���л�ȡ������ʷ��������
	GPRS_REQ_FIRST_DATE_SUCCESS = 0x0001,//��ȡ������ʷ�������ڳɹ�
	GPRS_REQ_FIRST_DATE_FAIL = 0x0002,//��ȡ������ʷ��������ʧ��
}e_GPRS_REQ_FIRST_DATE_STATE;

typedef struct{
	uint16_t	size;
	uint8_t	data[800];//150
}s_SERV_TCP_DATA; //TCP������������

typedef struct{
	uint8_t	size;
	uint8_t	data[70];
}s_SERV_TCP_AT; //TCP���յ���

typedef struct{
	uint8_t	size;
	uint8_t	data[70];
}s_LORA_AT; //LORA���յ���

typedef struct{
	uint8_t	size;
	uint8_t	data[10];
}s_485_DEV_DATA; //485���յ���

typedef struct{
	
	uint8_t  slave_addr;
	uint8_t  function;
	uint8_t  num;
  uint8_t	 data[2];
}s_DEV485_RECV_PACKAGE;
/*
typedef struct{
	uint8_t  slave_addr;
	uint8_t  function;
	uint8_t  num;
  uint8_t	 data[2];
	uint8_t  crc[2];
}s_DEV485_SEND_PACKAGE;
*/
typedef struct{
	
	uint8_t		slave_addr;
	uint8_t		function;
	uint8_t		addr[2];
	uint8_t		data[2];
	uint8_t  crc[2];
}s_DEV485_SEND_PACKAGE;

typedef struct{
	e_serv_data_type	serv_up_data_type;  //�ϴ���������
	e_serv_data_type	serv_ack_data_type;  //��ƽ̨Ӧ���ϴ����ݷ�������
	uint8_t   cpap_data_up_state;  //GPRS���յ��������ϴ�ָ���е�����״̬
	uint8_t   send_all_end;
	uint8_t   pack1k_all_size; //1K���ݷְ�����
	uint8_t   pack1k_big;      //�ְ������ڷ��͵����ݰ�
	uint16_t  tcp_all_packet;  //�����ݰ�
	uint16_t  serv_ack_packet; //TCPӦ��������Ű�	
	uint16_t  cpap_send_packet; //��������ǰ����������Ű�
	uint16_t  cpap_data_length;
}s_SERV_DATA_GPRS;
 
//GPRS����ƽ̨֮�����������
typedef struct{
	e_serv_gprs_cmd_type	serv_cmd_type;	//��������
	e_serv_cmd_ack_type  serv_cmd_ack_type;  //����Ӧ������
	uint8_t   SN_timeout_count;  //��ʱ����
	e_serv_cache_judge  serv_cache;
  uint8_t  serv_cache_count; //����ָ��󻺴�ָ���ʱ��������6Sδ���յ��·�����ָ������Ϊ��ƽ̨�޻���ָ��
	uint8_t  serv_cache_7b[7];
	uint8_t  serv_cache_64b[64];
}s_GPRS_CMD_SERV;

//�������·���GPRSЭ�鹲ͬ����
typedef struct
{
	uint8_t  head; //����֡ͷ
	uint8_t  protocol_version;//Э��汾��
	uint8_t  reserve[7];//Ԥ���ֶ�
	uint16_t cmd;//������
	uint8_t  stacode;//״̬��
}s_PROTOCOL_COMMON;

typedef struct
{
	s_PROTOCOL_COMMON protocol_common;
	uint8_t  SN[20];
	uint8_t  hard_version[10];//Ӳ���汾��
	uint8_t  soft_version[10];//����汾��
	uint8_t  password[50];
	uint16_t  crc;
	uint8_t  tail;
}s_GPRS_2_SERVER_IDENTITY;//���������������֤

typedef struct
{
	s_PROTOCOL_COMMON protocol_common;
	uint16_t  crc;
	uint8_t  tail;
}s_SERVER_2_GPRS_IDENTITY_TODAY;//�����������֤�͵������ݷ��� ����������ʷ�������� ���������ù�����������

typedef struct
{
	s_PROTOCOL_COMMON protocol_common;
	uint32_t serv_req_date_start;
	uint32_t serv_req_date_end;
	uint16_t  crc;
	uint8_t  tail;
}s_SERVER_REQ_HISTORY_DATA;//������������ʷ���� ������������ʷ����

typedef struct
{
	s_PROTOCOL_COMMON protocol_common;
	uint32_t serv_back_first_date;
	uint16_t  crc;
	uint8_t  tail;
}s_SERVER_BACK_FIRST_DATE;//����������������ʷ��������

typedef struct
{
	s_PROTOCOL_COMMON protocol_common;
	uint8_t work_parameters[40];//��������
	uint16_t  crc;
	uint8_t  tail;
}s_SERVER_WORK_PARAMETERS;//���������ù�������

typedef struct
{
	s_PROTOCOL_COMMON protocol_common;
	uint32_t  measure_data_today[12];//
	uint16_t  screen_gprs_set[20];
	uint32_t  first_date;
	uint16_t  crc;
	uint8_t  tail;
}s_GPRS_2_SERVER_TODAY;//�������ݷ���������

typedef struct
{
	s_PROTOCOL_COMMON protocol_common;
	uint32_t  start_date;
	uint32_t  end_date;
	uint16_t  crc;
	uint8_t  tail;
}s_SERVER_2_GPRS_REQ_DATA;//�������ݷ���������

typedef struct
{
	uint8_t  head;
	uint8_t  version;
	uint8_t  device;
	uint8_t  reserve[7];
	uint16_t cmd;
	uint8_t  stacode;
}s_SERV_COMMON;


typedef struct{
  s_SERV_COMMON  cmd_common;
	uint8_t  SN[32];
	uint8_t  Warning;
  uint8_t  WClass;
	uint8_t  W_Content[64];
	uint16_t  crc;
	uint8_t  tail;
}s_SERV_Warning;

typedef struct{
  s_SERV_COMMON  cmd_common;
	uint8_t  SN[32];
  uint8_t  CClass;
	uint8_t  C_Content[64];
	uint16_t  crc;
	uint8_t  tail;
}s_SERV_Caption;

typedef struct{
  s_SERV_COMMON  cmd_common;
	uint8_t	 state;
	uint8_t	 sn[32];
	uint8_t	 hw_version[32];
	uint8_t	 sw_version[32];
	uint16_t  crc;
	uint8_t  tail;
}s_SERV_GPRS_Msg; //��Ϣ�ϱ�

//��ҪָЭ����ָͬ���ͨ������ָ���
//����SN����  ����Ӧ�𼰷��� �澯��ʾ���� ����ָ���·���Ӧ�� ����Ӧ��
typedef struct{
	s_SERV_COMMON  cmd_common;
	uint16_t  crc;
	uint8_t  tail;
}s_SERV_CMD_UP_DOWN;

//Ԥ������ָ���ֶ� 64λ
typedef struct{
	s_SERV_COMMON  cmd_common;
	uint8_t cache_msg[64];
	uint16_t  crc;
	uint8_t  tail;
}s_SERV_CMD_CACHE_64B;

 
#pragma pack()


uint32_t sim900_read(uint8_t *buffer);
uint32_t sim900_write(uint8_t *buffer, uint32_t size);

void dev485_recv_task(void const *argument);
//void serv_close_net_task(void const *argument);
//void enter_standby_task(void const *argument);
void dev485_send_task(void const *argument);
//void GPRS_Judge_task(void const *argument);
//void serv_cpap_task(void const *argument);
	
uint8_t sim900_init(void);
uint8_t get_serv_time(void);
uint8_t close_tcp_serv(void);
uint8_t CLOSED_PDP_DEACT(void);
uint8_t start_tcp_serv(void);
//unsigned short cal_serv_crc_N(unsigned char *message, unsigned int len,unsigned short crc_reg);
//unsigned short cal_serv_crc(unsigned char *message, unsigned int len);
uint16_t CRC16_MODBUS(uint8_t *puchMsg, uint16_t usDataLen);

//uint16_t Send_msg_Translate (uint8_t *tmsg,uint16_t len);
uint32_t send_to_tcp_serv(uint8_t data[], uint32_t size);
void Timer_Callback (void const *arg);
//void Timer2_Callback (void const *arg);
void Dev_485_init(void);
uint8_t gettimeval(uint8_t *resp);
void time_init(void); //���Գ���
uint8_t judge_serv_time(uint8_t *resp);
void read_screen_userset();//��ȡ��Ļ����
void read_lcm_userset();//��ȡҺ������������   0X0807D000
void read_module_userset();//��ȡ��Ļ����   0X0807D000
#endif
