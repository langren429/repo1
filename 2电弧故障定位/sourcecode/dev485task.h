/*
 * (C) Copyright 2013 Thingtek
 *
 * Written by: randy <shenminghua@thingtek.com>
 */
 
#ifndef _SERVER_TASK_H
#define _SERVER_TASK_H

#include "common.h"

//大小端转换函数定义
#define Tranversel16(X)  (((X&0xff00)>>8)|((X&0x00ff)<<8))
#define Tranversel32(X)  (((X&0xff000000)>>24)|((X&0x00ff0000)>>8)|((X&0x0000ff00)<<8)|((X&0x000000ff)<<24))

#define RS485_REON()  GPIO_ResetBits(GPIOA, GPIO_Pin_11) //时钟引脚置底
#define RS485_REOFF()  GPIO_SetBits(GPIOA, GPIO_Pin_11)  //时钟引脚置高

#define RS485_DEON()  GPIO_SetBits(GPIOA, GPIO_Pin_12)  //时钟引脚置高
#define RS485_DEOFF()  GPIO_ResetBits(GPIOA, GPIO_Pin_12) //时钟引脚置底

#pragma pack(1)
//数据类型
typedef enum 
{	
	SERVER_TCP_NON,
	//趋势图及统计数据
  SERVER_TCP_ONE = 0x3101,
  SERVER_TCP_SEVEN = 0x3102,
  SERVER_TCP_THIRTY = 0x3103,	
	SERVER_TCP_1K = 0x3104,
	SERVER_TCP_PAST_RECORD = 0x3105
}e_serv_data_type;

//=================SERV protocal======================
typedef enum 
{	
	GPRS_CMD_IDENTITY = 0x1101,//沉降仪身份认证
	GPRS_CMD_DATA_TODAY = 0x1102,	//当天数据上报
	GPRS_CMD_REQ_FIRST_DATE = 0x1103, //沉降仪请求最早历史数据日期
	GPRS_CMD_REQ_DATA = 0x1104, //沉降仪请求历史数据
	SERV_CMD_REQ_DATA = 0x2101, //服务器请求历史数据	
	SERV_CMD_SET_SETTLE = 0x2102,  //设置沉降仪工作参数
	SERV_CMD_END_TCP = 0x2103,//结束通讯指令

}e_cmd_type;

typedef enum{
	SERV_CMD_NON = 0x0000,
	SERV_CMD_DATA, //上传数据统称
	SERV_CMD_ACK,
	SERV_CMD_GPRS_UP = 0x1101, //SN上传
}e_serv_gprs_cmd_type;

//命令应答类型
typedef enum 
{	
	SERV_CMD_ACK_NON,
	SERV_CMD_ACK_HEART = 0x1102,	//心跳指令
	SERV_CMD_ACK_WARNING = 0x2102, //告警
	SERV_CMD_ACK_CAPTION = 0x2103, //提示	
	SERV_CMD_ACK_CACHE_ONE = 0x3201,  //缓存指令
	SERV_CMD_ACK_CACHE_SEVEN = 0x3202,
	SERV_CMD_ACK_CACHE_THIRTY = 0x3203,	
	SERV_CMD_ACK_CACHE_1K = 0x3204,
	SERV_CMD_ACK_CACHE = 0X4101,   //预留字段7字节
	SERV_CMD_ACK_PRE_CACHE = 0X4102,  //预留字段64字节
}e_serv_cmd_ack_type;

//是否需要判断缓存指令
typedef enum 
{	
	SERVER_CACHE_NON,
  SERVER_CACHE_YES,
  SERVER_CACHE_NO,
	
}e_serv_cache_judge;

typedef enum{
	SERVER_IDENTITY_NULL = 0x0000,//未进行身份验证
	SERVER_IDENTITY_SUCCESS = 0x0001,//身份验证成功
	SERVER_IDENTITY_FAIL = 0x0002,//身份验证失败
}e_SERVER_IDENTITY_STATE;


typedef enum{
	GPRS_REQ_FIRST_DATE_NULL = 0x0000,//未进行获取最早历史数据日期
	GPRS_REQ_FIRST_DATE_SUCCESS = 0x0001,//获取最早历史数据日期成功
	GPRS_REQ_FIRST_DATE_FAIL = 0x0002,//获取最早历史数据日期失败
}e_GPRS_REQ_FIRST_DATE_STATE;

typedef struct{
	uint16_t	size;
	uint8_t	data[800];//150
}s_SERV_TCP_DATA; //TCP主动接收数据

typedef struct{
	uint8_t	size;
	uint8_t	data[70];
}s_SERV_TCP_AT; //TCP接收到的

typedef struct{
	uint8_t	size;
	uint8_t	data[70];
}s_LORA_AT; //LORA接收到的

typedef struct{
	uint8_t	size;
	uint8_t	data[10];
}s_485_DEV_DATA; //485接收到的

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
	e_serv_data_type	serv_up_data_type;  //上传数据类型
	e_serv_data_type	serv_ack_data_type;  //云平台应答上传数据返回类型
	uint8_t   cpap_data_up_state;  //GPRS接收到呼吸机上传指令中的数据状态
	uint8_t   send_all_end;
	uint8_t   pack1k_all_size; //1K数据分包总数
	uint8_t   pack1k_big;      //分包后正在发送的数据包
	uint16_t  tcp_all_packet;  //总数据包
	uint16_t  serv_ack_packet; //TCP应答数据序号包	
	uint16_t  cpap_send_packet; //呼吸机当前发送数据序号包
	uint16_t  cpap_data_length;
}s_SERV_DATA_GPRS;
 
//GPRS与云平台之间的命令类型
typedef struct{
	e_serv_gprs_cmd_type	serv_cmd_type;	//命令类型
	e_serv_cmd_ack_type  serv_cmd_ack_type;  //命令应答类型
	uint8_t   SN_timeout_count;  //超时次数
	e_serv_cache_judge  serv_cache;
  uint8_t  serv_cache_count; //心跳指令后缓存指令计时，若超过6S未接收到下发缓存指令则认为云平台无缓存指令
	uint8_t  serv_cache_7b[7];
	uint8_t  serv_cache_64b[64];
}s_GPRS_CMD_SERV;

//服务器下发给GPRS协议共同部分
typedef struct
{
	uint8_t  head; //报文帧头
	uint8_t  protocol_version;//协议版本号
	uint8_t  reserve[7];//预留字段
	uint16_t cmd;//命令码
	uint8_t  stacode;//状态码
}s_PROTOCOL_COMMON;

typedef struct
{
	s_PROTOCOL_COMMON protocol_common;
	uint8_t  SN[20];
	uint8_t  hard_version[10];//硬件版本号
	uint8_t  soft_version[10];//软件版本号
	uint8_t  password[50];
	uint16_t  crc;
	uint8_t  tail;
}s_GPRS_2_SERVER_IDENTITY;//发往服务器身份验证

typedef struct
{
	s_PROTOCOL_COMMON protocol_common;
	uint16_t  crc;
	uint8_t  tail;
}s_SERVER_2_GPRS_IDENTITY_TODAY;//服务器身份验证和当天数据返回 请求最早历史数据日期 服务器设置工作参数返回

typedef struct
{
	s_PROTOCOL_COMMON protocol_common;
	uint32_t serv_req_date_start;
	uint32_t serv_req_date_end;
	uint16_t  crc;
	uint8_t  tail;
}s_SERVER_REQ_HISTORY_DATA;//服务器请求历史数据 沉降仪请求历史数据

typedef struct
{
	s_PROTOCOL_COMMON protocol_common;
	uint32_t serv_back_first_date;
	uint16_t  crc;
	uint8_t  tail;
}s_SERVER_BACK_FIRST_DATE;//服务器返回最早历史数据日期

typedef struct
{
	s_PROTOCOL_COMMON protocol_common;
	uint8_t work_parameters[40];//工作参数
	uint16_t  crc;
	uint8_t  tail;
}s_SERVER_WORK_PARAMETERS;//服务器设置工作参数

typedef struct
{
	s_PROTOCOL_COMMON protocol_common;
	uint32_t  measure_data_today[12];//
	uint16_t  screen_gprs_set[20];
	uint32_t  first_date;
	uint16_t  crc;
	uint8_t  tail;
}s_GPRS_2_SERVER_TODAY;//当天数据发往服务器

typedef struct
{
	s_PROTOCOL_COMMON protocol_common;
	uint32_t  start_date;
	uint32_t  end_date;
	uint16_t  crc;
	uint8_t  tail;
}s_SERVER_2_GPRS_REQ_DATA;//当天数据发往服务器

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
}s_SERV_GPRS_Msg; //信息上报

//主要指协议相同指令均通过此条指令定义
//包含SN返回  心跳应答及返回 告警提示返回 缓存指令下发及应答 数据应答
typedef struct{
	s_SERV_COMMON  cmd_common;
	uint16_t  crc;
	uint8_t  tail;
}s_SERV_CMD_UP_DOWN;

//预留缓存指令字段 64位
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
void time_init(void); //测试程序
uint8_t judge_serv_time(uint8_t *resp);
void read_screen_userset();//读取屏幕设置
void read_lcm_userset();//读取液晶屏设置设置   0X0807D000
void read_module_userset();//读取屏幕设置   0X0807D000
#endif
