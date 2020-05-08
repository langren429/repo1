/*
 * (C) Copyright 2013 Thingtek
 *
 * Written by: randy <shenminghua@thingtek.com>
 */

#ifndef _TCP_TASK_H
#define _TCP_TASK_H

#include "common.h"

#define	CPAP_DEAULT_PACKAGE_LEN		5
#define	LORA_DEAULT_PACKAGE_LEN		7

//������������
typedef enum{
	GPRS2CPAP=0x01,
	CPAP2GPRS=0x81,
	CPAP_DATA_BEGIN_5A = 0x5a,
	
	CPAP_CMD_STATE = 0xA0,  //״ָ̬��
	CPAP_CMD_SN = 0xA2,   //SNָ�
	CPAP_CMD_LINK_NET = 0xA3,  //����ָ��
	CPAP_CMD_CLOSED_NET = 0xA4,  //����ָ��
	CPAP_CMD_HEART = 0xA5,  //����ָ��
	CPAP_CMD_UP_DATA = 0xA6, //�ϴ�����ָ��
	CPAP_CMD_PACK_BIG = 0xA7,  //�����ܰ���
	CPAP_CMD_DATA_ACK = 0xA8,  //���ݳɹ�ʧ��Ӧ��
	CPAP_CMD_DATA_END = 0xA9,  //���ݽ���ָ��	
	CPAP_CMD_REAL_CPAP = 0xB0,  //ʵʱ�������� ͸��ģʽ
	CPAP_CMD_SET_IP = 0xB1,
	CPAP_CMD_READ_IP = 0xB2,
	CPAP_CMD_SET_PORT = 0xB3,
	CPAP_CMD_READ_PORT = 0xB4,
	CPAP_CMD_READ_APN = 0xB5,
	CPAP_CMD_READ_VERSION = 0xB6,
	CPAP_CMD_SET_APN = 0xB7,
	CPAP_CMD_SET_NAME = 0xB8,
	CPAP_CMD_SET_PASSWORD = 0xB9,
	CPAP_CMD_READ_NAME = 0xBA,
	CPAP_CMD_READ_PASSWORD = 0xBB,
	CPAP_CMD_CACHE_4101_7B = 0xC5,
	CPAP_CMD_CACHE_4102_64B = 0xC6,
	
}e_CPAP_DIRECTION;

typedef enum{
	SCREEN_SET_ADDR_DEEP = 0x0001,
	SCREEN_SET_ADDR_MAGNET_RING_NUM = 0x0002,
	SCREEN_SET_ADDR_MEASURE_PERIOD = 0x0003,
	SCREEN_SET_ADDR_MOTOR_SPEED = 0x0004,
	
	SCREEN_SET_ADDR_TCP_ADDR0 = 0x0005,
	SCREEN_SET_ADDR_TCP_ADDR1 = 0x0006,
	SCREEN_SET_ADDR_TCP_ADDR2 = 0x0007,
	SCREEN_SET_ADDR_TCP_ADDR3 = 0x0008,
	SCREEN_SET_ADDR_TCP_PORT = 0x0009,
	
  SCREEN_SET_ADDR_FTP_ADDR0 = 0x000A,
	SCREEN_SET_ADDR_FTP_ADDR1 = 0x000B,
	SCREEN_SET_ADDR_FTP_ADDR2 = 0x000C,
	SCREEN_SET_ADDR_FTP_ADDR3 = 0x000D,
	SCREEN_SET_ADDR_FTP_PORT = 0x000E,
	
	SCREEN_STANDARD_ADDR_RING1 = 0x0010,
	SCREEN_STANDARD_ADDR_RING2 = 0x0012,
	SCREEN_STANDARD_ADDR_RING3 = 0x0014,
	SCREEN_STANDARD_ADDR_RING4 = 0x0016,
	SCREEN_STANDARD_ADDR_RING5 = 0x0018,
	SCREEN_STANDARD_ADDR_RING6 = 0x001A,
	SCREEN_STANDARD_ADDR_RING7 = 0x001C,
	SCREEN_STANDARD_ADDR_RING8 = 0x001E,
	SCREEN_STANDARD_ADDR_RING9 = 0x0020,
	SCREEN_STANDARD_ADDR_RING10 = 0x0022,
	SCREEN_STANDARD_ADDR_DATE = 0x0024,
	
	SCREEN_CURRENT_ADDR_RING1 = 0x0030,
	SCREEN_CURRENT_ADDR_RING2 = 0x0032,
	SCREEN_CURRENT_ADDR_RING3 = 0x0034,
	SCREEN_CURRENT_ADDR_RING4 = 0x0036,
	SCREEN_CURRENT_ADDR_RING5 = 0x0038,
	SCREEN_CURRENT_ADDR_RING6 = 0x003A,
	SCREEN_CURRENT_ADDR_RING7 = 0x003C,
	SCREEN_CURRENT_ADDR_RING8 = 0x003E,
	SCREEN_CURRENT_ADDR_RING9 = 0x0040,
	SCREEN_CURRENT_ADDR_RING10 = 0x0042,
	SCREEN_CURRENT_ADDR_DATE = 0x0044,
	
	SCREEN_SINK_ADDR_RING1 = 0x0050,
	SCREEN_SINK_ADDR_RING2 = 0x0052,
	SCREEN_SINK_ADDR_RING3 = 0x0054,
	SCREEN_SINK_ADDR_RING4 = 0x0056,
	SCREEN_SINK_ADDR_RING5 = 0x0058,
	SCREEN_SINK_ADDR_RING6 = 0x005A,
	SCREEN_SINK_ADDR_RING7 = 0x005C,
	SCREEN_SINK_ADDR_RING8 = 0x005E,
	SCREEN_SINK_ADDR_RING9 = 0x0060,
	SCREEN_SINK_ADDR_RING10 = 0x0062,
	SCREEN_SINK_ADDR_DAY_NUM = 0x0064,
	
	SCREEN_MEASURE_ADDR_KEY = 0x0070,
	SCREEN_SET_STANDARD = 0x0071,
	SCREEN_MEASURE_ADDR_CURRENT_DEEP = 0x0072,
	
	BATTERY_VOLTAGE = 0x0080,
	SOFTWARE_VERSION = 0x0082,
	
	REQ_HISTORY_DATA = 0x0090,
	HISTORY_DATE_START = 0x0092,
	HISTORY_DATE_END = 0x0094,
	
}e_SCREEN_ADDR;

//��Ļ״̬����
typedef enum{
	SCREEN_STATE_STOP = 0x0000,
	SCREEN_STATE_START = 0x0001,
	SCREEN_STATE_PAUSE = 0x0002,
	SCREEN_STATE_RESET = 0x0003,
	SCREEN_STATE_PREPARE = 0x0004,
}e_SCREEN_STATE;	

typedef enum{
	DATA_TODAY_NO = 0x0000,//�����������跢��
	DATA_TODAY_HAVE = 0x0001,//�����������跢��
	DATA_TODAY_SENDING = 0x0002,//�������ڷ���
}e_DATA_TODAY_SEND_STATE;




//������30s�� ����͹���
typedef enum{
	STANDBY_ENTER_MODE_NO = 0x0000,
	STANDBY_ENTER_MODE_YES = 0x0001,
}e_STANDBY_ENTER_MODE;	

//�����ǹ���ģʽ
typedef enum{
	MANUAL_WORK_MODEL = 0x0000,
	WAKE_WORK_MODEL = 0X0001,
}e_SETTLE_WORK_STATE;

//�������ϴ���������
typedef enum{
	CPAP_UP_DATA_NON ,
	CPAP_UP_DATA_ONE = 0X66,
	CPAP_UP_DATA_SEVEN = 0X67,
	CPAP_UP_DATA_THIRTY = 0X68,
	CPAP_UP_DATA_1K = 0X69,	
}e_CPAP_UP_DATA_TYPE;


//�������ݴ�ָ������
typedef enum{
	CPAP_CACHE_NON ,
	CPAP_CACHE_DATA = 0XBD,
	CPAP_CACHE_WARN = 0XBE,
	CPAP_CACHE_CAPTION = 0XBF,
}e_CPAP_CACHE_CMD_TYPE;

//�������ݴ�ָ���е���������
typedef enum{
	CPAP_CACHE_DATA_NON ,
	CPAP_CACHE_DATA_ONE = 0X52,
	CPAP_CACHE_DATA_SEVEN = 0X53,
	CPAP_CACHE_DATA_THIRTY = 0X54,
	CPAP_CACHE_DATA_1K = 0X55,
}e_CPAP_CACHE_DATA_TYPE;

//�������ݴ�ָ���е���������
typedef enum{
	KEY_NON,
	KEY_S1_OK,
	KEY_S2_UP,
	KEY_S3_DOWN,
}e_KEY_TYPE;

typedef enum{
	SET_STATUS_NO,//������״̬
  SET_STATUS_YES,//����״̬
}e_SET_STATUS;

////GPRS�������֮�����������
//typedef struct{
//	e_cpap_gprs_cmd_type	cpap_cmd_type;	//��������
//	uint8_t   cpap_timeout_count;  //��ʱ����

//}s_CPAP_CMD_GPRS;


typedef struct{
	uint8_t		head;
	uint8_t		cmd;
	uint8_t		data1;
	uint8_t		data2;
	uint8_t		crc;
}s_CPAP_PACKAGE;

typedef struct{
	uint16_t		measure_state;
	uint16_t		set_standard;
	int32_t		current_deep;


}s_SCREEN_MEASURE;

typedef struct{
	uint16_t		deep;
	uint16_t		ring_num;
	uint16_t		measure_period;
	uint16_t		motor_speed;
	uint16_t		  tcp_addr[4];
	uint16_t    tcp_port;
  uint16_t		  ftp_addr[4];
	uint16_t    ftp_port;

	uint16_t    screen_set_flag;//������Ļ����ʾ
//  uint16_t    enter_standby_flag; //������Ļ����ʾ
//	uint16_t    wake_up_time;//������Ļ����ʾ
	
  uint16_t domainlen;//������Ļ����ʾ  16
	uint16_t domain[50];//������Ļ����ʾ
	uint16_t netapnlen;
	uint16_t netapn[50];
	uint16_t netuserlen;
	uint16_t netuser[30];
	uint16_t netkeylen;
	uint16_t netkey[30];
	uint16_t enable_dns_flag;//������Ļ����ʾ 180

}s_SCREEN_SET; //��Ļ����

typedef struct{
	uint16_t		flag;
	uint16_t		DAC_val;
	uint16_t		RES_val;
}s_MODULE_SET; //ģ������


typedef struct{
 uint32_t enable_flag;//������Ļ����ʾ ��־λ
 float current_value; //��ǰ����ֵ mm
 int Baudrate; //485�ڲ�����
 int endpoint_num;//�豸վ��
 int Collection_interval;//�ɼ���� ��
 float max_value; //��λ mm
}s_LCM_SET; //Һ��������



typedef struct{

	int32_t standard_ring[10];
	uint32_t battery_voltage_standard;
	int32_t standard_date;
	int32_t flash_write_num;//�ڱ�׼���ݺ���д���������Ŀ����

  int32_t current_ring[10];
	uint32_t battery_voltage;
	int32_t current_date;
	
  int32_t sink_ring[10];
	int32_t sink_date;
	
		
}s_SCREEN_DATA;//�������

typedef struct{

	int32_t data[10];
	uint32_t last_date; //FLASH�����һ��д���ĵ�ַ ��ʾFLASH����Ŀ�� ������Ŀ��λ��Ϊ��ص�ѹ
	uint32_t first_date;
		
}s_FLASH_ITEM;//�������

typedef struct{
	
	uint8_t		cmd;
	uint8_t		addr[2];
	uint8_t		data_num;
	uint8_t		data[4];
}s_SCREEN_RECV_PACKAGE;

typedef struct{
	uint8_t  head1;
	uint8_t  head2;
	uint8_t  num;
	uint8_t		cmd;
	uint8_t		addr[2];
  uint8_t		data[4];
}s_SCREEN_SEND_PACKAGE;

typedef struct{
	uint8_t  slave_addr;
	uint8_t  function;
	uint8_t  num;
  uint8_t	 data[2];
}s_LORA_RECV_PACKAGE;

typedef struct{
	uint8_t  head;//ͷ
	uint8_t  state;//״̬��
	uint16_t  module_id;//ģ�鵥Ԫ��
  uint16_t	 big_num;//������
	uint16_t total_packet_num;//�ܰ���
	uint16_t current_packet_num;//��ǰ�����
	uint16_t data_num;//��Ϣ�峤��
}s_TCP_SEND_DATA_PACKAGE;

typedef struct{
	uint8_t  head;//ͷ
	uint8_t  state;//״̬��
	uint16_t  module_id;//ģ�鵥Ԫ��
	uint16_t crc;//��ǰ�����
	uint8_t  tail;//��Ϣ�峤��
}s_TCP_ASK_CMD_PACKAGE;

typedef struct{
	uint8_t	size;
	uint8_t	data[20];
}s_FROM_SERV_TCP_CMD; //TCP������������

/*
typedef struct{
	uint8_t  slave_addr;
	uint8_t  function;
	uint8_t  num;
  uint8_t	 data[2];
	uint8_t  crc[2];
}s_LORA_SEND_PACKAGE;
*/
typedef struct{ //��������
	
	uint8_t		slave_addr;
	uint8_t		function;
	uint8_t		addr[2];
	uint8_t		data[2];
	uint8_t  crc[2];
}s_LORA_SEND_PACKAGE;
/*typedef struct{
	uint8_t  head1;
	uint8_t  head2;
	uint8_t  num;
	uint8_t		cmd;
	uint8_t		addr[2];
  uint8_t		data[8];
}s_RTC_SEND_PACKAGE;*/

typedef struct{
	uint8_t	size;
	uint8_t	data[10];
}s_SCREEN_RECV_CMD; //TCP���յ���

typedef struct{
	uint8_t	size;
	uint8_t	data[20];
}s_TCP_RECV_CMD; //TCP���յ���

typedef enum {
	rcv_ok=1,
	rcv_error,
	rcving,
}e_userset_state;

typedef struct{
	uint8_t	data_1k[700];
	uint16_t	size;
}s_CPAP_TCP_DATA; //͸����������


typedef struct{
	uint8_t	data_1k[1200];
	uint16_t	size;
}s_CPAP_DATA;




uint8_t cal_cpap_crc(s_CPAP_PACKAGE cpap_package);
void tcp_recv_task(void const *argument);
void lora_send_task(void const *argument);
void RF_Lora_Dev_init(void);


#endif

