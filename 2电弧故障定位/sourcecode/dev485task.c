/*
 * (C) Copyright 2013 Thingtek
 *
 * Written by: randy <shenminghua@thingtek.com>
 */
 
//#include "servertask.h"
#include "common.h"
#include "stm32f10x_flash.h"
#include <stdint.h>
#include "encode.h"
s_SERV_TCP_DATA		serv_tcp_data={0};
s_LORA_AT   AT_recv_buffer={0};
s_485_DEV_DATA dev485_recv_data={0};//485接收数据buffer

//表示GPRS与云平台之间传输数据属性
s_SERV_DATA_GPRS  serv_data_gprs={SERVER_TCP_NON,0x00};
//#define  DNS_ENABLE 1
//extern ARM_DRIVER_UART *RF_Lora_Dev;
extern s_SCREEN_SET SCREEN_SET;
extern s_MODULE_SET MODULE_SET;
extern s_SCREEN_DATA SCREEN_DATA;
extern s_SCREEN_MEASURE SCREEN_MEASURE;
e_SETTLE_WORK_STATE SETTLE_WORK_STATE;
extern uint16_t send_screen_again;
extern uint8_t rtc_wakeup_set;
extern uint8_t rtc_feed_dog;
extern uint8_t pcf8563_time[6];
extern uint32_t flash_temp[512];//2048/4
extern e_DATA_TODAY_SEND_STATE data_today_need_send; //0 无数据发送 1 有数据发送 2 正在发送
extern uint32_t read_flash_measure_data[12];
extern uint8_t measure_working;//测量正在进行标志
extern e_SCREEN_STATE SCREEN_STATE;
extern uint8_t screen_set_updata;
extern uint8_t screen_standard_updata;
extern uint8_t measure_start_prepare_flag;
uint8_t measure_start_prepare_count;
e_SERVER_IDENTITY_STATE server_identity_state;
e_GPRS_REQ_FIRST_DATE_STATE gprs_req_first_date_state;
uint32_t serv_req_date_start_global;
uint32_t serv_req_date_end_global;
uint32_t gprs_req_date_start_global;//沉降仪请求服务器历史数据开始结束日期
//uint32_t gprs_req_date_end_global; 
uint32_t serv_req_date_start_verify;//历史数据开始日期 需与服务器返回比对
uint32_t serv_req_date_end_verify;
uint8_t serv_cmd_req_data_start;
uint8_t gprs_cmd_req_data_start; //沉降仪请求数据开始
uint8_t gprs_req_first_date_ask;//沉降仪获取最早日期回应
//uint8_t gprs_req_first_date_ok;//沉降仪获取最早日期成功

uint8_t serv_req_packet_num;
uint8_t serv_req_packet_last;
uint32_t first_date_from_server;
uint32_t first_date_from_server_save; //保存接收到服务器返回的第一天的日期 
uint8_t gprs_req_packet_num; //沉降仪向服务器请求数据总包数
uint8_t gprs_req_packet_last;//沉降仪向服务器请求数据最后一包大小
uint32_t serv_back_req_date_end;
uint8_t rtc_init_success = 0;
//s_SCREEN_SEND_PACKAGE screen_send_package;
#define	DEV485_RECV_MSG_SIZE		7  //主机接收返回
#define	DEV485_SEND_MSG_SIZE		8 //主机发送请求

extern ARM_DRIVER_UART Driver_UART1;
ARM_DRIVER_UART *Dev_485 = &Driver_UART1;
extern	s_GATEWAY_DEVICE gateway_dev;
extern	s_CPAP_DATA		cpap_data;
extern SERV_SIM sim_state;
extern uint8_t program_runing;
extern uint8_t req_data_from_serv_continue;
extern s_LCM_SET LCM_SET;
extern uint8_t recv_return_flag[50];
extern uint8_t send_unit_ID[50];
extern uint8_t send_constant_ID[100];
extern uint8_t recv_constant_flag[100];
extern uint8_t recv_4A_flag[100];
extern uint8_t recv_5A_flag[100];
extern uint8_t recv_6A_flag[100];
extern uint8_t recv_7A_flag[100];

s_GPRS_CMD_SERV  gprs_cmd_serv;

s_SERVER_REQ_HISTORY_DATA gprs_req_history_data_package;//沉降仪请求历史数据


osMessageQDef(dev485_recv_msg, 8, s_DEV485_RECV_PACKAGE);
osMessageQDef(serv_recvat_msg, DEV485_RECV_MSG_SIZE, s_SERV_TCP_AT);
osMessageQDef(dev485_send_msg, 8, s_DEV485_SEND_PACKAGE);

osMessageQId(dev485_recv_msg);
osMessageQId(serv_recvat_msg);
osMessageQId(dev485_send_msg);

extern osMessageQId(lora_send_msg);
extern osMessageQId(rs232_send_msg);
//extern osMessageQId(lora_recv_msg);
extern SERV_USERSET user_set;
extern uint8_t open_screen_flag; //打开 1
extern uint8_t gateway_address;//本机地址

uint8_t rtc_set_time[7];
uint8_t time_temp[50];
uint16_t sleep_time_num;
uint16_t sleep_time_num_xxx;
uint16_t PA0_wake_up;

uint8_t xxx;
uint8_t hard_reset_time; //网关重发次数
uint8_t server_identity_ask;

uint8_t server_today_ask;
uint8_t server_today_ok;
uint8_t server_history_ask;
uint8_t gprs_history_ask;//gprs向服务器请求数据 服务器有返回 此标志置1
uint8_t gprs_req_date_num;//沉降仪请求服务器天数
//uint8_t server_set_work_parameters;
//uint8_t server_end_tcp;
uint8_t server_end_tcp_ok;//tcp已经关闭
uint16_t close_screen_count;//息屏计数
uint16_t enter_stopmode_count;//息屏情况下计数 是否进入低功耗
uint8_t enter_stopmode_flag;//息屏情况下计数 是否进入低功耗 标志

uint8_t screen_state;//屏幕打开 1 关闭 0状态
uint8_t	no_cmd_sleep_start;//从接收到服务器第一条指令开始计时 60s后无指令则进入睡眠
uint8_t	no_cmd_sleep_count;//计数清零
uint8_t	no_cmd_sleep_flag;//计数清零

static const uint8_t auchCRCHi[] = {
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81,
0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
0x40
};
 
static const uint8_t auchCRCLo[] = {
0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7, 0x05, 0xC5, 0xC4,
0x04, 0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD,
0x1D, 0x1C, 0xDC, 0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32, 0x36, 0xF6, 0xF7,
0x37, 0xF5, 0x35, 0x34, 0xF4, 0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE,
0x2E, 0x2F, 0xEF, 0x2D, 0xED, 0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 0x61, 0xA1, 0x63, 0xA3, 0xA2,
0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68, 0x78, 0xB8, 0xB9, 0x79, 0xBB,
0x7B, 0x7A, 0xBA, 0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0, 0x50, 0x90, 0x91,
0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98, 0x88,
0x48, 0x49, 0x89, 0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83, 0x41, 0x81, 0x80,
0x40
};

uint16_t CRC16_MODBUS(uint8_t *puchMsg, uint16_t usDataLen)
{
  uint8_t uchCRCHi = 0xFF; // ?CRC?????
  uint8_t uchCRCLo = 0xFF; // ?CRC ?????
  uint32_t uIndex; // CRC??????
  while (usDataLen--) // ???????
  {
    uIndex = uchCRCHi ^ *puchMsg++; // ??CRC
    uchCRCHi = uchCRCLo ^ auchCRCHi[uIndex];
    uchCRCLo = auchCRCLo[uIndex];
  }
  return (((uint16_t)uchCRCLo << 8u) | uchCRCHi);
}

//TCP校验码，编码方式：CRC16  多包数据组合求CRC
//unsigned short cal_serv_crc_N(unsigned char *message, unsigned int len,unsigned short crc_reg)
//{	
//	int i, j;
//	unsigned short current;
//		 
//	for (i = 0; i < len; i++)
//	{
//		current = message[i] << 8;
//		for (j = 0; j < 8; j++)
//		{
//			if ((short)(crc_reg ^ current) < 0)
//					crc_reg = (crc_reg << 1) ^ 0x1021;
//			else
//					crc_reg <<= 1;
//			current <<= 1;           
//		}
//	}
//	return crc_reg;
//}
//求校验码，编码方式：CRC16
/*unsigned short cal_serv_crc(unsigned char *message, unsigned int len)
{	
	int i, j;
	unsigned short crc_reg = 0;
	unsigned short current;
		 
	for (i = 0; i < len; i++){
		current = message[i] << 8;
		for (j = 0; j < 8; j++){
			if ((short)(crc_reg ^ current) < 0)
					crc_reg = (crc_reg << 1) ^ 0x1021;
			else
					crc_reg <<= 1;
			current <<= 1;           
		}
	}
	return crc_reg;
}*/

//获取DNS得到的服务器IP地址
uint16_t* getdnsip(uint8_t *resp)
{
	char* ptr_start; 	
	uint8_t i;
	static uint16_t temp_ip[4];
	ptr_start=strstr(resp, "\",\"");		//+CDNSGIP: 1,"www.curativecloud.net","61.155.212.44" 不判断.com",",可能是.net，改为判断","
	if(ptr_start == NULL)
		return NULL;
	ptr_start = ptr_start+3;	
	for(i=0;i<4;i++)
	{		
		temp_ip[i]=atoi(ptr_start);	
		if((temp_ip[i]>=10)&&(temp_ip[i]<100))
		{
			ptr_start = ptr_start+3;
		}
		else if(temp_ip[i]<10)
		{
			ptr_start = ptr_start+2;
		}
		else
		{
			ptr_start = ptr_start+4;
		}
	}	
	if(temp_ip[0]+temp_ip[1]+temp_ip[2]+temp_ip[3]==0)
		return NULL;
	else
		return temp_ip;
//		memcpy(user_set.tcpip,temp_ip,4);
  return NULL;
}

/*
//获取DNS得到的服务器IP地址
uint16_t* getdnsip(uint8_t *resp)
{
	char* ptr_start,*ptr_end; 	
	uint8_t i,j,k, temp[5];
	static uint16_t temp_ip[4];
	ptr_start = strrchr(resp, ',');		//+CDNSGIP: 1,"www.curativecloud.net","61.155.212.44" 不判断.com",",可能是.net，改为判断","
	ptr_end = strrchr(resp, '\r');
	if(ptr_start<ptr_end){
		for(i=0,j=0,k=3;i<=(ptr_end-ptr_start);i++){
			if((*(ptr_start+i)!=0x2e)&&(*(ptr_start+i)!=0x0d)){
				if((*(ptr_start+i)>='0')&&(*(ptr_start+i)<='9')){
					 temp[j]=*(ptr_start+i);
					 j++;
				}
			}
			else if((*(ptr_start+i)==0x2e)||(*(ptr_start+i)==0x0d)){
				temp[j]='\0';
				temp_ip[k]=atoi(temp);
				k--;
				j=0;
			}
		}
	}
	if(temp_ip[0]+temp_ip[1]+temp_ip[2]+temp_ip[3]==0)
		return NULL;
	else
		return temp_ip;
}*/

 
 //Query the IP Address of Given Domain Name
uint8_t execute_dns(char *domain)
{
	uint8_t i=0,state=1,send_str[50];
	osEvent	serv_recvat_event;
	s_SERV_TCP_AT  *AT_Buffer;
	uint16_t* dns_get_ip = NULL;
	AT_recv_buffer.size=0;	
  sprintf(send_str,"AT+CDNSGIP=\"%s\"\r",domain);
	for(i=0;i<3;i++)
	{
	 sim900_write(send_str, strlen(send_str));
	 osDelay(3000); 
	 serv_recvat_event = osMessageGet(serv_recvat_msg, 1000);
	 if(serv_recvat_event.status == osEventMessage)
	 {
			AT_Buffer=serv_recvat_event.value.p;	
			if(strstr(AT_Buffer->data, "+CDNSGIP: 1"))
			{ //数据缓冲区内接收到的数据存在OK   "220.250.64.225"联通 				
				if(strstr(AT_Buffer->data, "220.250.64.225"))	//域名错误时，联通卡解析得到的总是该IP地址
				{
					memset(AT_Buffer->data, 0, AT_recv_buffer.size);
					AT_recv_buffer.size=0;
					continue;
					//return 0;
				}					
				else
					dns_get_ip = getdnsip(AT_Buffer->data);	
				if(dns_get_ip != NULL)								//成功解析得到IP地址
					memcpy(SCREEN_SET.tcp_addr,dns_get_ip,sizeof(SCREEN_SET.tcp_addr));
				state=0;
				memset(AT_Buffer->data, 0, AT_recv_buffer.size);
				AT_recv_buffer.size=0;
				break;
			}
			else
			{
				state=1;
				memset(AT_Buffer->data, 0, AT_recv_buffer.size);
				AT_recv_buffer.size=0;
				continue;
			}
	 }
	}
	return state;	
}

void sim900_power_off()
{
//	do
//	{
//		GPIO_PinWrite(GPIOB, 0, 0);											//通过反相器拉低
//		delay_ms(3000);
		GPIO_PinWrite(GPIOC, 9, 1);											//通过反相器拉低
		//IWDG_ReloadCounter();
		osDelay(3000);
		GPIO_PinWrite(GPIOC, 9, 0);											//通过反相器释放
		osDelay(4000);
	//	IWDG_ReloadCounter();		
//	}while(read_sim900_status_pin()!=(uint32_t)0x01);  //成功开机status电平为高	
}

/*
**		函数名：sim_module_reset
**		参  数：
**		返回值：
**		说  明：用于在工作过程中复位SIM模块
*/
void sim_module_reset(void)
{
	uint8_t		cmd_response[56];
	uint16_t sim_init_return=0;
	uint8_t poerdown_wait=0,reset_counter=0,reset_wait_counter=0,i;

	gateway_dev.serv_status.init_status=SERV_INIT_STATUS_NET_NO; //SIM900需要重新初始化
	//sim900_dev.serv_link = SERV_LINK_STATUS_NO;
	while(1)
	{
	//	sim900_dev.serv_status.recv_status = SERV_RECV_STATUS_AT_CACHE;
		sim900_power_off();        //可能会死机到不响应AT指令		
		for(poerdown_wait=0;poerdown_wait<10;poerdown_wait++){
			osDelay(5000);
			sim900_read(cmd_response);
			//memset(cmd_response, 0, sizeof(cmd_response));
			if(strstr(cmd_response, "NORMAL")==NULL)
				continue;
			else
				break;
		}
		if(reset_counter>=3){					//连续复位多次都没有成功，则等待一段时间
			for(reset_wait_counter=0;reset_wait_counter<6;reset_wait_counter++){
				osDelay(10000);
	//			IWDG_ReloadCounter();				
			}			
		}
		sim900_power_init();
//		if(reset_counter>=3){					//连续复位多次都没有成功，则等待一段时间
//			reset_counter=0;
//			for(reset_wait_counter=0;reset_wait_counter<11;reset_wait_counter++){
//				osDelay(10000);
//		//		IWDG_ReloadCounter();				
//			}			
//		}
//		osDelay(10000);
//		sim_init_return = sim900_init();
//		if(sim_init_return){
//			reset_counter++;					//连续初始化均未成功计数			
//			continue;			
//		}
//		else{
//			sim900_dev.serv_status.init_status=SERV_INIT_STATUS_NET_YES;	 //SIM900需要重新初始化成功
//		//	sim900_dev.serv_status.recv_status = SERV_RECV_STATUS_TCP;
//			reset_counter=0;
//			break;
//		}
	}
//	for(i=0;i<5;i++){	
	//	LED_Toggle;
	//	osDelay(100);  
	//}
//	GPIO_PinWrite(GPIOA, 2, 1);				//关闭灯		
}
//根据当前时间戳判断 被哪种任务唤醒 
uint8_t judge_serv_time(uint8_t *resp) 
{
//	char* ptr_start; 
//	char temp_buffer[6];
	char rtc_time_temp[6];
//	uint8_t i,j,signal;
//	ptr_start=strstr(resp, ":");
//	memcpy(time_temp, ptr_start, 50);
//	rtc_set_time[1] = (time_temp[3] - 0x30) * 10 + time_temp[4] - 0x30;//年
//	rtc_set_time[2] = (time_temp[6] - 0x30) * 10 + time_temp[7] - 0x30;//月
//	rtc_set_time[3] = (time_temp[9] - 0x30) * 10 + time_temp[10] - 0x30;//日
//	rtc_set_time[4] = (time_temp[12] - 0x30) * 10 + time_temp[13] - 0x30;//时
//	rtc_set_time[5] = (time_temp[15] - 0x30) * 10 + time_temp[16] - 0x30;//分
//	rtc_set_time[6] = (time_temp[18] - 0x30) * 10 + time_temp[19] - 0x30;//秒
//	
//  PCF8563_init();//rtc初始化
	
	/*PCF8563_read_time(rtc_time_temp); //读出时间 012345 年月日时分秒
	
	if((rtc_time_temp[3] == 0x08)&&(rtc_time_temp[4] <= 0x05)) //8点
	{
			//打开太阳能充电开关
		  SETTLE_WORK_STATE = WAKE_WORK_MODEL;
		
			PCF8563_WriteOneByte(PCF_DEVICE_ADD, Hour_Alarm,0x17); // 17点报警
		  PCF8563_WriteOneByte(PCF_DEVICE_ADD, STATUS_REG2,0x12); //报警有效 INT引脚激活 报警中断使能	
	}
	else if((rtc_time_temp[3] == 0x11)&&(rtc_time_temp[4] <= 0x05))//17点
	{
		//关闭太阳能充电开关
			if(SCREEN_SET.measure_period == 2)
			{
				PCF8563_WriteOneByte(PCF_DEVICE_ADD, Hour_Alarm,0x20); // 20点报警
		    PCF8563_WriteOneByte(PCF_DEVICE_ADD, STATUS_REG2,0x12); //报警有效 INT引脚激活 报警中断使能	
			}
			else if(SCREEN_SET.measure_period == 1)
			{
				PCF8563_WriteOneByte(PCF_DEVICE_ADD, Hour_Alarm,0x08); // 8点报警
				PCF8563_WriteOneByte(PCF_DEVICE_ADD, STATUS_REG2,0x12); //报警有效 INT引脚激活 报警中断使能	
			}
	}
	else if((rtc_time_temp[3] == 0x14)&&(rtc_time_temp[4] <= 0x05))//20点
	{
		SETTLE_WORK_STATE = WAKE_WORK_MODEL;		
		PCF8563_WriteOneByte(PCF_DEVICE_ADD, Hour_Alarm,0x08); // 8点报警
		PCF8563_WriteOneByte(PCF_DEVICE_ADD, STATUS_REG2,0x12); //报警有效 INT引脚激活 报警中断使能	
	}
	*/

  return 0;
} 
//获取当前时间戳 初始化RTC模块
uint8_t gettimeval(uint8_t *resp) 
{
	char* ptr_start; 
	char temp_buffer[6];
	uint8_t i,j,signal;
	ptr_start=strstr(resp, ":");
	memcpy(time_temp, ptr_start, 50);
	rtc_set_time[1] = (time_temp[3] - 0x30) * 10 + time_temp[4] - 0x30;//年
	rtc_set_time[2] = (time_temp[6] - 0x30) * 10 + time_temp[7] - 0x30;//月
	rtc_set_time[3] = (time_temp[9] - 0x30) * 10 + time_temp[10] - 0x30;//日
	rtc_set_time[4] = (time_temp[12] - 0x30) * 10 + time_temp[13] - 0x30;//时
	rtc_set_time[5] = (time_temp[15] - 0x30) * 10 + time_temp[16] - 0x30;//分
	rtc_set_time[6] = (time_temp[18] - 0x30) * 10 + time_temp[19] - 0x30;//秒
	
	//rtc_set_time[5] = 0x01;//分wj
	//PCF8563_init();//rtc初始化
	if(rtc_set_time[1] < 17)
	{
		return 1;//获取时间出错
	}
	else
	{
		return 0;
	}
	

}  


void time_init(void) //测试程序
{

	rtc_set_time[1] = 0x0f;//年
	rtc_set_time[2] = 0x01;//月
	rtc_set_time[3] = 0x11;//日
	rtc_set_time[4] = 0x09;//时
	rtc_set_time[5] = 0x00;//分
	rtc_set_time[6] = 0x00;//秒
//	PCF8563_init();//rtc初始化

} 

void read_module_userset()//读取屏幕设置   0X0807D000
{
	uint8_t i;
	uint16_t *p;
	p = &(MODULE_SET.flag);
	for(i = 0;i < 3;i ++)
	{
		*(p + i) = flash_read_data16(SCREEN_SET_ADDRESS + i*2);
	}
}
void read_screen_userset()//读取屏幕设置   0X0807D000
{
	uint8_t i;
	uint16_t *p;
	p = &(SCREEN_SET.deep);
	for(i = 0;i < 180;i ++)
	{
		*(p + i) = flash_read_data16(SCREEN_SET_ADDRESS + i*2);
	}
}

void read_lcm_userset()//读取液晶屏设置设置   0X0807D000
{
	uint8_t i;
	uint32_t *p;
	p = &(LCM_SET.enable_flag);
	for(i = 0;i < 6;i ++)
	{
		*(p + i)  = flash_read_data((LCM_SET_ADDRESS + (i * 4)));
	}
}

//数据区域字符转移,同时提取  //封装之后发送给云平台的数据转移
//uint16_t Send_msg_Translate (uint8_t *tmsg,uint16_t len)	 //*tmsg转译前(1k)  *atmsg转译后(2k)
//{
//	uint16_t i=0,tran_len=0,j=0;
//	uint8_t temp[1200],flag=0;
////	temp=(uint8_t*)malloc(800);
//	if(len>sizeof(temp))
//	{
//		return 0;
//	}
//	memcpy(temp,tmsg, len);
////	memset(tmsg, 0, len);
//	while(i<len)
//	{    
//		if((temp[i]==0x7c)|(temp[i]==0x7d)|(temp[i]==0x7e))
//		{ tmsg[tran_len++]=0x7c;
//			tmsg[tran_len]=(temp[i]&0x0f)|0x50;
//		}
//		
//		else 
//		{
//			tmsg[tran_len]=temp[i];
//		}
//		i++;
//		tran_len++;
//	}
////  free(temp);	
//  return tran_len;      //转译后的长度
//}

//从485串口读取一个字节的数据
static uint8_t read_485_char(void)
{
	uint8_t return_char;
	Dev_485->ReadData(&return_char, 1);
	return return_char;
}

//数据区域字符转移,同时提取 接收到云平台数据转移
//uint16_t Recv_msg_Translate (uint8_t *atmsg,uint16_t len)	 //*tmsg转译前(1k)  *atmsg转译后(2k)
//{
//	uint16_t i=0,j=0,k;
//	uint8_t temp[1200]; //150
////	temp=(uint8_t*)malloc(len);
//	if(len>sizeof(temp))
//	{
//		return 0;
//	}
//	memcpy(temp,atmsg,len);
//	while(i<len){
//	    if(temp[i]==0x7c){ 
//						atmsg[j]=(temp[i]&0xf0)|(temp[i+1]&0x0f);
//				    i++;
//				}
//       else if(temp[i]==0x7D){
//				 atmsg[j]=temp[i];
//				 k=j;
//				 break;
//			 }		 
//			 else{ 
//						atmsg[j]=temp[i];
//				}		 
//		i++;
//		j++;
//	}
//	while(j<len)
//	{
//	 j++;
//	 atmsg[j]=0x00;		
//	}
//  return k;      //转译后的长度
//}

static void Dev_485_callback(ARM_UART_EVENT event)
{
	uint8_t recv_char;
  uint16_t crc_data;
  static uint8_t dev485_head_flag;//表示收到从机号完整的头
	switch(event)
	{
		case ARM_UART_EVENT_RX_THRESHOLD:
		
				recv_char = read_485_char();
		
				if(dev485_head_flag == 0)
				{
					dev485_recv_data.data[dev485_recv_data.size++]=recv_char;//找 03 02 头
					if((recv_char == 0x02)&&(dev485_recv_data.size >= 3)&&(dev485_recv_data.data[dev485_recv_data.size - 2] == 0X03))//在第二个以后的位置发现0x03 且其上一个字节为从地址
					{
						dev485_head_flag = 1;//找到完整的头
						dev485_recv_data.data[0]=dev485_recv_data.data[dev485_recv_data.size - 3];
						dev485_recv_data.data[1]=0x03;
						dev485_recv_data.data[2]=0x02;
						dev485_recv_data.size = 3;
					}//写命令返回 根据地址低位 及其上一位为05 判断头
					else if(((recv_char == 0x2E)||(recv_char == 0x4A)||(recv_char == 0x5A)||(recv_char == 0x6A)||(recv_char == 0x7A))&&(dev485_recv_data.size >= 3)&&(dev485_recv_data.data[dev485_recv_data.size - 2] == 0x05))
					{
						dev485_head_flag = 1;//找到完整的头
						dev485_recv_data.data[0]=dev485_recv_data.data[dev485_recv_data.size - 3];
						dev485_recv_data.data[1]=0x05;
						dev485_recv_data.data[2]=recv_char;
						dev485_recv_data.size = 3;
					}
				}
				else if(dev485_head_flag == 1)
				{
					dev485_recv_data.data[dev485_recv_data.size++]=recv_char;
					if((dev485_recv_data.size == 0x07)&&(dev485_recv_data.data[1] == 0x03))//读命令返回
					{
             crc_data = CRC16_MODBUS(dev485_recv_data.data, 5);					
						if(((crc_data & 0x00ff) == dev485_recv_data.data[dev485_recv_data.size - 2])&&((crc_data >> 8) == dev485_recv_data.data[dev485_recv_data.size - 1]))
							{
									//将数据发送到特殊队列
									osMessagePut(dev485_recv_msg, (uint32_t)(&dev485_recv_data), 0);
									dev485_recv_data.size=0;
									//一帧数据的结束
							}
							dev485_recv_data.size=0;
							dev485_head_flag = 0;
					}
					else if((dev485_recv_data.size == 0x08)&&(dev485_recv_data.data[1] == 0x05))//写命令返回
					{
						crc_data = CRC16_MODBUS(dev485_recv_data.data, 6);
						
							if(((crc_data & 0x00ff) == dev485_recv_data.data[dev485_recv_data.size - 2])&&((crc_data >> 8) == dev485_recv_data.data[dev485_recv_data.size - 1]))
							{
									//将数据发送到特殊队列
									osMessagePut(dev485_recv_msg, (uint32_t)(&dev485_recv_data), 0);
									dev485_recv_data.size=0;
									//一帧数据的结束
							}
							dev485_recv_data.size=0;
							dev485_head_flag = 0;
					}
				}
		break;

		default:
			break;
	}
}


void Timer_Callback (void const *arg)
{

	
	IWDG_ReloadCounter();				//喂狗	程序正常运行
	

}
//TCP任务放在REC send 任务中处理
//function
void dev485_recv_task(void const *argument){
	osEvent				dev485_recv_event;
	s_DEV485_RECV_PACKAGE *dev485_recv_package;	
	s_DEV485_SEND_PACKAGE *dev485_send_package; 
	uint8_t  cmd_respond[100],loop_count,i;
	static s_LORA_SEND_PACKAGE screen_send_package; 
  uint16_t crc_check;

	static uint16_t crc_temp;
	//static s_DEV485_SEND_PACKAGE dev485_send_package; 
	static s_RS232_SEND_PACKAGE rs232_send_package; 
	uint16_t crc_data,length;
  uint32_t item_write_page; 
  uint32_t item_write_offset; 
	FLASH_Status  flash_status; //FLASH当前状态
	uint16_t send_data_num;
	s_485_DEV_DATA *dev485_recv_buffer;
		
	gateway_dev.dev_485.dev485_recv_task_id = osThreadGetId();
	while(gateway_dev.dev_485.dev485_recv_task_id == NULL);
	
  dev485_recv_msg = osMessageCreate( osMessageQ(dev485_recv_msg), NULL);
	dev485_send_msg = osMessageCreate( osMessageQ(dev485_send_msg), NULL);
	osDelay(1000);


	while(1){

	  //TCP连接成功后再执行接收
			dev485_recv_event=osMessageGet(dev485_recv_msg, 6000);	 
   if(dev485_recv_event.status == osEventMessage)		
		{
			dev485_recv_buffer = (s_485_DEV_DATA *)dev485_recv_event.value.p;
		//	dev485_recv_package = (s_DEV485_RECV_PACKAGE *)((s_485_DEV_DATA *)dev485_recv_event.value.p)->data;
			if(dev485_recv_buffer->data[1] == 0x03)
			{
				  dev485_recv_package = (s_DEV485_RECV_PACKAGE *)dev485_recv_buffer->data;
					rs232_send_package.head[0] = 'T';
					rs232_send_package.head[1] = 'T';
					rs232_send_package.head[2] = 'E';
					rs232_send_package.head[3] = 'K';
				  rs232_send_package.seq[0] = 0;
				  rs232_send_package.seq[1] = 0;
			  	rs232_send_package.type[0] = 0;
				  rs232_send_package.type[1] = 0X01;
					rs232_send_package.cmd[0] = 0;
				  rs232_send_package.cmd[1] = 0X02; //水位仪数据回复
					rs232_send_package.length[0] = 0;
				  rs232_send_package.length[1] = 0X08;
					rs232_send_package.data[0] = 0;
				  rs232_send_package.data[1] = 0X01;//网关ID
					rs232_send_package.data[2] = 0;
				  rs232_send_package.data[3] = dev485_recv_package->slave_addr;//单元ID
				if((dev485_recv_package->data[0] == 0xFF)&&(dev485_recv_package->data[1] == 0xFF))
				{
					rs232_send_package.data[4] = 0XFF;
				  rs232_send_package.data[5] = 0XFF;
				  rs232_send_package.data[6] = 0XFF;
				  rs232_send_package.data[7] = 0XFF;//数据
				}
				else
				{
					rs232_send_package.data[4] = dev485_recv_package->data[0];
				  rs232_send_package.data[5] = dev485_recv_package->data[1];
				  rs232_send_package.data[6] = 0X00;
				  rs232_send_package.data[7] = 0X00;//数据
				}

					length = rs232_send_package.length[1];
					crc_data = cal_serv_crc(&(rs232_send_package.head[0]), 12 + length);
				
					rs232_send_package.data[8] = (crc_data & 0xff00)>> 8;;
				  rs232_send_package.data[9] = crc_data & 0x00ff;
					 					
					osMessagePut(rs232_send_msg, (uint32_t)(&rs232_send_package), 0);
					
			}
			else if(dev485_recv_buffer->data[1] == 0x05)
			{
				dev485_send_package = (s_DEV485_SEND_PACKAGE *)dev485_recv_buffer->data;
				switch(dev485_send_package ->addr[0]){
				case 0x2E: //开始停止测量 返回
				if(dev485_send_package->data[1] == 0xFF)//收到开始测量返回
				{
					rs232_send_package.head[0] = 'T';
					rs232_send_package.head[1] = 'T';
					rs232_send_package.head[2] = 'E';
					rs232_send_package.head[3] = 'K';
				  rs232_send_package.seq[0] = 0;
				  rs232_send_package.seq[1] = 0;
			  	rs232_send_package.type[0] = 0;
				  rs232_send_package.type[1] = 0X01;
					rs232_send_package.cmd[0] = 0;
				  rs232_send_package.cmd[1] = 0X04; //开始测量返回
					rs232_send_package.length[0] = 0;
				  rs232_send_package.length[1] = 0X04;
					rs232_send_package.data[0] = 0;
				  rs232_send_package.data[1] = 0X01;//网关ID
					rs232_send_package.data[2] = 0;
				  rs232_send_package.data[3] = dev485_send_package->slave_addr;//单元ID
				
					length = rs232_send_package.length[1];
				
				  crc_data = cal_serv_crc(&(rs232_send_package.head[0]), 12 + length);
				
					rs232_send_package.data[4] = (crc_data & 0xff00)>> 8;;
				  rs232_send_package.data[5] = crc_data & 0x00ff;
					 					
					osMessagePut(rs232_send_msg, (uint32_t)(&rs232_send_package), 0);
				}
				else if(dev485_send_package->data[1] == 0x00)//收到停止测量返回
				{
					rs232_send_package.head[0] = 'T';
					rs232_send_package.head[1] = 'T';
					rs232_send_package.head[2] = 'E';
					rs232_send_package.head[3] = 'K';
				  rs232_send_package.seq[0] = 0;
				  rs232_send_package.seq[1] = 0;
			  	rs232_send_package.type[0] = 0;
				  rs232_send_package.type[1] = 0X01;
					rs232_send_package.cmd[0] = 0;
				  rs232_send_package.cmd[1] = 0X05; //停止测量返回
					rs232_send_package.length[0] = 0;
				  rs232_send_package.length[1] = 0X04;
					rs232_send_package.data[0] = 0;
				  rs232_send_package.data[1] = 0X01;//网关ID
					rs232_send_package.data[2] = 0;
				  rs232_send_package.data[3] = dev485_send_package->slave_addr;//单元ID
				
					length = rs232_send_package.length[1];
				
				  crc_data = cal_serv_crc(&(rs232_send_package.head[0]), 12 + length);
				
					rs232_send_package.data[4] = (crc_data & 0xff00)>> 8;;
				  rs232_send_package.data[5] = crc_data & 0x00ff;
					 					
					osMessagePut(rs232_send_msg, (uint32_t)(&rs232_send_package), 0);
				}
					
				break;
				case 0x4A: //写探针采集周期返回
          for(i = 0;i < 100;i ++)
					{
						if(send_constant_ID[i] == dev485_send_package->slave_addr)//网关收到单元回复
						{
							recv_4A_flag[i] = 0x01;
							break;//找到了直接退出 不要再找
						}
					}
					
				break;
				case 0x5A: //写垂直比尺返回
          for(i = 0;i < 100;i ++)
					{
						if(send_constant_ID[i] == dev485_send_package->slave_addr)//网关收到单元回复
						{
							recv_5A_flag[i] = 0x01;
							break;//找到了直接退出 不要再找
						}
					}
					
				break;
				case 0x6A: //写测针常数小数返回
          for(i = 0;i < 100;i ++)
					{
						if(send_constant_ID[i] == dev485_send_package->slave_addr)//网关收到单元回复
						{
							recv_6A_flag[i] = 0x01;
							break;//找到了直接退出 不要再找
						}
					}
					
				break;
				case 0x7A: //写测针常数整数返回
          for(i = 0;i < 100;i ++)
					{
						if(send_constant_ID[i] == dev485_send_package->slave_addr)//网关收到单元回复
						{
							recv_7A_flag[i] = 0x01;
							break;//找到了直接退出 不要再找
						}
					}
					
				break;
				default:
					break;
				
			 }
				
				for(i = 0;i < 100;i ++)
				{
					if((recv_4A_flag[i] == 0x01)&&(recv_5A_flag[i] == 0x01)&&(recv_6A_flag[i] == 0x01)&&(recv_7A_flag[i] == 0x01))//网关收到单元回复
					{
						recv_constant_flag[i] = 0x01;//4个都接收到了 才将标志位置一
					}
				}
			 
			}

			}
			else if(dev485_recv_event.status == osEventTimeout){
         //判断3次重传SN超时

			}	
		
	 }
}

////GPRS发送数据到云平台
//void GPRS_Judge_task(void const *argument){
//	//参数定义及声明
//	osEvent	serv_recvat_event;
//  s_SERV_TCP_AT  *AT_Buffer;
//	sim900_dev.serv_dev.GPRS_Judge_task_id = osThreadGetId();
//	while(sim900_dev.serv_dev.GPRS_Judge_task_id == NULL);
//	
////	gprs_send_msg = osMessageCreate( osMessageQ(gprs_send_msg), NULL);
//	while(1){
//		 //后台任务中检测 TCP状态或者AT多余指令在此处处理
//	 if(sim900_dev.serv_link == SERV_LINK_STATUS_YES){
//		serv_recvat_event = osMessageGet(serv_recvat_msg, osWaitForever);
//		 if(serv_recvat_event.status == osEventMessage){		  
//				AT_Buffer=serv_recvat_event.value.p;	
//			 if(strstr(AT_Buffer->data, "CLOSED")||strstr(AT_Buffer->data, "+PDP: DEACT")){
//					 sim900_dev.serv_status.tcp_status= SERV_TCP_STATUS_CLOSED;
//           sim900_dev.serv_link = SERV_LINK_STATUS_NO; //连接网络未成功
//					//服务器关闭TCP 需要重新连接TCP
//					 if(CLOSED_PDP_DEACT()){
//					   sim900_dev.serv_status.init_status = SERV_INIT_STATUS_SOFT;						 
//					 }
//					 else
//						 sim900_dev.serv_status.tcp_status = SERV_TCP_STATUS_CLOSED;
////					 BEEP_ON;
//				}			 
//		}  
//	 }
//	 osDelay(3000);
//  }
//}

//透明模式下实时数据传输
//void serv_cpap_task(void const *argument){
//	
//  osEvent				serv_send_event;
//	s_CPAP_PACKAGE  cpap_send_package={GPRS2CPAP};
//	s_CPAP_TCP_DATA  *tcp_send_data;
//	uint8_t *Gprs_Return_Tcp_Data,Tcp_return_com[13]; //GPRS返回数据公共值 
//	uint16_t crc_data;
//	
//	sim900_dev.serv_dev.serv_cpap_task_id = osThreadGetId();
//	while(sim900_dev.serv_dev.serv_cpap_task_id == NULL);

//	serv_send_msg = osMessageCreate( osMessageQ(serv_send_msg), NULL);
//	while(1){
//		serv_send_event=osMessageGet(serv_send_msg, osWaitForever);		
//		if((serv_send_event.status == osEventMessage)&&(sim900_dev.serv_status.tcp_status == SERV_TCP_STATUS_YES)){
//			 tcp_send_data = (s_CPAP_TCP_DATA *)serv_send_event.value.p;
//			 //公共部分数据 申请数据内存空间
//			 Gprs_Return_Tcp_Data = (uint8_t*)malloc(1000);  //申请内存大小暂时不能固定
//			 //TCP上传数据，公共部分赋值
//			 Tcp_return_com[0] = 0x7e;
//			 Tcp_return_com[1] = Protocol_Version;
//			 Tcp_return_com[2] = 0x00; 
//			 Tcp_return_com[4] = (serv_data_gprs.pack1k_big&0x00ff);
//			 Tcp_return_com[3] = (serv_data_gprs.pack1k_big&0xff00)>>8;
//			 Tcp_return_com[6] = 0x00;
//			 Tcp_return_com[5] = 0x00;
//			 Tcp_return_com[8] = 0x00;
//			 Tcp_return_com[7] = 0x00;
//			 Tcp_return_com[9] = 0x00;
//			 Tcp_return_com[11] = (serv_data_gprs.serv_up_data_type&0x00ff);
//			 Tcp_return_com[10] = (serv_data_gprs.serv_up_data_type&0xff00)>>8;
//			 Tcp_return_com[12] = serv_data_gprs.send_all_end ; //数据续传
//			 memcpy(Gprs_Return_Tcp_Data,Tcp_return_com, 13); 

//			 //将实际接收到的数据复制，数据其实应该先转义之后再发送
//			 memcpy((Gprs_Return_Tcp_Data+13), tcp_send_data->data_1k, tcp_send_data->size); 
//			 crc_data=cal_serv_crc_N((Gprs_Return_Tcp_Data+2), (tcp_send_data->size+11),0x00);
//			 Gprs_Return_Tcp_Data[tcp_send_data->size+14] = (crc_data&0x00ff);
//			 Gprs_Return_Tcp_Data[tcp_send_data->size+13] = (crc_data&0xff00)>>8;
//			 //数据重组之后转移字符
//			 tcp_send_data->size = Send_msg_Translate (Gprs_Return_Tcp_Data+1,tcp_send_data->size+14);
//			 Gprs_Return_Tcp_Data[tcp_send_data->size+1] = 0x7d;
////			 LED_Toggle;
//			 //协议转换之后的数据发送
//			 send_to_tcp_serv(Gprs_Return_Tcp_Data, tcp_send_data->size+2);
//			 //释放消息体数据内存空间
//			 free(Gprs_Return_Tcp_Data);
//			 if(!serv_data_gprs.send_all_end){ //所有数据传输完毕之后再发送心跳
//					sim900_dev.work_mode = SIM900_MODE_GPRS;   //GPRS转换为非透传模式
//				  serv_data_gprs.serv_up_data_type = SERVER_TCP_NON;
//          serv_data_gprs.pack1k_big = 0x00;				 
//				}				
//			}
//	}
//}

/*void enter_standby_task(void const *argument)
{
	osEvent				enter_standby_event;

	sim900_dev.serv_dev.enter_standby_task_id = osThreadGetId();
	while(sim900_dev.serv_dev.enter_standby_task_id == NULL);
	//此任务需要别的任务唤醒因此没必要延时等待呼吸机连接状态
	osSignalClear(sim900_dev.serv_dev.enter_standby_task_id, 0x03);
  
	while(1){
		enter_standby_event = osSignalWait(0x03, osWaitForever);
		if(enter_standby_event.status == osEventSignal){

					do{
					GPIO_PinWrite(GPIOC, 9, 1);											//通过反相器拉低
					osDelay(3000);
					IWDG_ReloadCounter();				//喂狗
					GPIO_PinWrite(GPIOC, 9, 0);											//通过反相器释放
					osDelay(4000);
				}while((GPIO_PinRead(GPIOC,6))!=(uint32_t)0x00);  //成功关机机status电平为低
				Sys_Enter_Standby();
			
	
		}
  }
}*/
//断网任务处理
/*void serv_close_net_task(void const *argument){
	
  osEvent				serv_close_net_event;
	s_CPAP_PACKAGE  cpap_send_package={GPRS2CPAP};
	
	sim900_dev.serv_dev.serv_close_net_task_id = osThreadGetId();
	while(sim900_dev.serv_dev.serv_close_net_task_id == NULL);
	//此任务需要别的任务唤醒因此没必要延时等待呼吸机连接状态
	osSignalClear(sim900_dev.serv_dev.serv_close_net_task_id, 0x02);
  
	while(1){
		serv_close_net_event = osSignalWait(0x02, osWaitForever);
		if(serv_close_net_event.status == osEventSignal){
		 //透传模式时，退出透传模式。
		 if(sim900_dev.work_mode == SIM900_MODE_CPAP){
			 sim900_dev.work_mode = SIM900_MODE_GPRS;
			 serv_data_gprs.serv_up_data_type = SERVER_TCP_NON;	
			 serv_data_gprs.send_all_end = 0x00;
			 serv_data_gprs.pack1k_big = 0x00;	
		 }
			//向呼吸机发送断网指令
			cpap_send_package.cmd = CPAP_CMD_CLOSED_NET;
			cpap_send_package.data1 = 0x00;
			cpap_send_package.data2 = 0x00;
			osMessagePut(screen_send_msg, (uint32_t)(&cpap_send_package), 1000);			
			//GPRS自行关闭CLOSE
			sim900_dev.serv_status.tcp_status = SERV_TCP_STATUS_CLOSED;
			sim900_dev.serv_link = SERV_LINK_STATUS_NO; //连接网络未成功
			close_tcp_serv();			
		}
  }
}*/


//GPRS发送呼吸机TCP请求数据  
//注目前数据只分一大包所有1K不管分成几小包只有一个包头包尾，中间小包数据为纯数据
void dev485_send_task(void const *argument){

	uint16_t remain_pack,send_data_num;   //剩余数据包个数
	//呼吸机接收数据大小是否满足要求，满足继续执行否则重新获取数据
	static uint16_t crc_data;
	uint8_t i;
	osEvent		dev485_send_event;
	s_DEV485_SEND_PACKAGE *dev485_send_package;
	s_SERV_GPRS_Msg		serv_gprs_msg_package;
  s_SERV_CMD_UP_DOWN			serv_ack_package;	
	s_CPAP_PACKAGE cpap_send_package={GPRS2CPAP};
	gateway_dev.dev_485.dev485_send_task_id = osThreadGetId();
	while(gateway_dev.dev_485.dev485_send_task_id == NULL);
	//此任务需要别的任务唤醒因此没必要延时等待呼吸机连接状态
//	osSignalClear(sim900_dev.serv_dev.serv_send_task_id, 0x05);


	while(1){
	
		dev485_send_event = osMessageGet(dev485_send_msg, osWaitForever);

		if(dev485_send_event.status == osEventMessage){
			dev485_send_package = dev485_send_event.value.p;
			RS485_DEON();
      RS485_REOFF();//写数据
			LED3_ON;
			osDelay(5);
			Dev_485->WriteData((uint8_t *)dev485_send_package, DEV485_SEND_MSG_SIZE);
			osDelay(10);
			LED3_OFF;
			RS485_DEOFF();
      RS485_REON(); //平时打开接收
			
		}
	//	osDelay(500);
	}
}

void Dev_485_init()
{
	extern ARM_DRIVER_UART *Dev_485;

	Dev_485->Initialize(Dev_485_callback, 16);
	Dev_485->PowerControl(ARM_POWER_FULL);
	//Dev_485->PowerControl(ARM_POWER_OFF);
	Dev_485->Configure(9600, 8, ARM_UART_PARITY_NONE, ARM_UART_STOP_BITS_1, ARM_UART_FLOW_CONTROL_NONE);
	Dev_485->SetRxThreshold(1);
	
  RS485_DEOFF();
  RS485_REON(); //平时打开接收
}
//获取TCP数据最大长度，若数据》600则直接取600.如果数据小于600则取获取值
uint16_t TCP_UP_Maxlength(uint8_t *databuffer)
{
  uint16_t length;
	char* ptr_start; 
	char temp_buffer[6];
	uint8_t i,j;
	ptr_start=strstr(databuffer, ":");
	for(i=1,j=0; i<50;)
	{
		if(*(ptr_start+i)!=0x0d){
			if((*(ptr_start+i)>='0')&&(*(ptr_start+i)<='9')){
		     temp_buffer[j]=*(ptr_start+i);
				 i++;
				 j++;
			}
			else
				i++;
		}
		else{
		  temp_buffer[j]='\0';
			length=atoi(temp_buffer);
			return length;
		}
	}	
  return 0;
}

/////////////////////////////////////获取信号值///未获取返回0
uint8_t getsignal(uint8_t *resp) 
{
	char* ptr_start; 
	char temp_buffer[6];
	uint8_t i,j,signal;
	ptr_start=strstr(resp, ":");
	for(i=1,j=0; i<50;)
	{
		if(*(ptr_start+i)!=0x2c){
			if((*(ptr_start+i)>='0')&&(*(ptr_start+i)<='9')){
		     temp_buffer[j]=*(ptr_start+i);
				 i++;
				 j++;
			}
			else
				i++;
		}
		else{
		  temp_buffer[j]='\0';
			signal=atoi(temp_buffer);
			return signal;
		}
	}	
  return 0;
}
//=============sim900 function==============

//TCP连接服务器 返回值state state为零表示TCP连接网络成功，state为数值表示TCP未连接网络成功，
uint8_t start_tcp_serv(void){
 uint8_t i=0,state=1,send_str[50],string_ip[50];
 osEvent	serv_recvat_event;
 s_SERV_TCP_AT  *AT_Buffer;
 AT_recv_buffer.size=0;
	
#if DNS_ENABLE
	if(SCREEN_SET.enable_dns_flag)			//需解析域名
	{
		for(i=0;i<SCREEN_SET.domainlen;i++)
		  string_ip[i] = SCREEN_SET.domain[i];
		  execute_dns((char *)&string_ip);
	}
	else															//无需解析域名
	{
		i = SCREEN_SET.enable_dns_flag;		//防止该位被读取覆盖
		read_screen_userset();//读取屏幕设置   0X0807D000
		SCREEN_SET.enable_dns_flag = i;
		
	}
#endif
 //获取SIM900信号强度
 for(i=0;i<3;i++){
   sim900_write("AT+CSQ\r",strlen("AT+CSQ\r"));
   osDelay(200); 
	 serv_recvat_event = osMessageGet(serv_recvat_msg, 50);
	 if(serv_recvat_event.status == osEventMessage){
		  AT_Buffer=serv_recvat_event.value.p;	
		  if(strstr(AT_Buffer->data, "+CSQ")){ //数据缓冲区内接收到的数据存在OK
				sim_state.signal = getsignal(AT_Buffer->data);
				state=0;
				memset(AT_Buffer->data, 0, AT_recv_buffer.size);
        AT_recv_buffer.size=0;
				break;
			}
			else{
			  state=1;
				memset(AT_Buffer->data, 0, AT_recv_buffer.size);
        AT_recv_buffer.size=0;
				continue;
			}
	 }
 }	
 
 
 //设置发送数据时是否显示>和send ok
 for(i=0;i<3;i++){
   sim900_write("AT+CIPSPRT=2\r", strlen("AT+CIPSPRT=2\r"));
   osDelay(100); 
	 serv_recvat_event = osMessageGet(serv_recvat_msg, 50);
	 if(serv_recvat_event.status == osEventMessage){
		  AT_Buffer=serv_recvat_event.value.p;	
		  if(strstr(AT_Buffer->data, "OK")){ //数据缓冲区内接收到的数据存在OK
				state=0;
				memset(AT_Buffer->data, 0, AT_recv_buffer.size);
        AT_recv_buffer.size=0;
				break;
			}
			else{
			  state=2;
				memset(AT_Buffer->data, 0, AT_recv_buffer.size);
        AT_recv_buffer.size=0;
				continue;
			}
	 }
 }
 
	sprintf(send_str,"AT+CIPSTART=\"TCP\",\"%d.%d.%d.%d\",%d\r",SCREEN_SET.tcp_addr[0],SCREEN_SET.tcp_addr[1],\
  SCREEN_SET.tcp_addr[2],SCREEN_SET.tcp_addr[3],SCREEN_SET.tcp_port);
	sim900_write(send_str, strlen(send_str));
	for(i=0;i<3;i++){ 
   osDelay(3000); 
	 serv_recvat_event = osMessageGet(serv_recvat_msg, 50);
	 if(serv_recvat_event.status == osEventMessage){
		  AT_Buffer=serv_recvat_event.value.p;	
		  if((strstr(AT_Buffer->data, "CONNECT OK"))||(strstr(AT_Buffer->data, "ALREADY CONNECT"))){ //表示TCP已经连接成功
				state=0;
				memset(AT_Buffer->data, 0, AT_recv_buffer.size);
        AT_recv_buffer.size=0;
				break;
			}
			else{
			  state=4;
				memset(AT_Buffer->data, 0, AT_recv_buffer.size);
        AT_recv_buffer.size=0;
				continue;
			}
	 }
 } 
  if(!state){
// IWDG_ReloadCounter();				//喂狗 
	 //TCP发送数据最大上传字节数
	 for(i=0;i<5;i++){   
		 sim900_write("AT+CIPSEND?\r", strlen("AT+CIPSEND?\r"));
		 osDelay(1000); 
		 serv_recvat_event = osMessageGet(serv_recvat_msg, 50);
		 if(serv_recvat_event.status == osEventMessage){
				AT_Buffer=serv_recvat_event.value.p;	
				if(strstr(AT_Buffer->data, "+CIPSEND")){ //数据缓冲区内接收到的数据存在OK
					sim_state.max_tcp_send_length=TCP_UP_Maxlength(AT_Buffer->data);
					if(!sim_state.max_tcp_send_length) //获取最大长度为0 则重联TCP
						state=5;
          else{ //如果最大长度大于600字节 取最大长度为600	
						if(sim_state.max_tcp_send_length>=600)
							 sim_state.max_tcp_send_length = 600;
					  state=0;
					}
					memset(AT_Buffer->data, 0, AT_recv_buffer.size);
          AT_recv_buffer.size=0;
					break;
				}
				else{
					state=5;
					memset(AT_Buffer->data, 0, AT_recv_buffer.size);
          AT_recv_buffer.size=0;
					continue;
				}
		 }
	 }
 }

  memset(AT_Buffer->data, 0, 60);
	return state;
}

//利用固定长度来发送数据，原来使用的是不定长
uint32_t send_to_tcp_serv(uint8_t data[], uint32_t size)
{
	uint8_t	cmd_response[50];
	uint8_t	state=0,i,j;
	uint8_t tcpsend[20]; 
	uint8_t end_flag[1]={0x1a};
	uint32_t	send_size;
	
	sprintf(tcpsend,"AT+CIPSEND=%d\r",size);
	sim900_write(tcpsend, strlen(tcpsend));
	osDelay(300);
	send_size=sim900_write(data, size);
	osDelay(1000);
	return send_size;
}

uint8_t CLOSED_PDP_DEACT(void)
{
 uint8_t i=0,state=0;
 osEvent	serv_recvat_event;
 s_SERV_TCP_AT  *AT_Buffer;
 AT_recv_buffer.size=0;
 for(i=0;i<3;i++){
   sim900_write("AT+CIPSHUT\r", strlen("AT+CIPSHUT\r"));
   osDelay(1000); 
	 serv_recvat_event = osMessageGet(serv_recvat_msg, 50);
	 if(serv_recvat_event.status == osEventMessage){
		  AT_Buffer=serv_recvat_event.value.p;	
		  if(strstr(AT_Buffer->data, "SHUT OK")){ //数据缓冲区内接收到的数据存在OK
				state=0;
				memset(AT_Buffer->data, 0, AT_recv_buffer.size);
        AT_recv_buffer.size=0;
				break;
			}
			else{
			  state=1;
				memset(AT_Buffer->data, 0, AT_recv_buffer.size);
        AT_recv_buffer.size=0;
				continue;
			}
	 }
 }
  memset(AT_Buffer->data, 0,60);
	return state;
}

//TCP与服务器断开连接，断开成功则返回state为0  不成功返回1
uint8_t close_tcp_serv(void){
 uint8_t i=0,state=0;
 osEvent	serv_recvat_event;
 s_SERV_TCP_AT  *AT_Buffer;
 AT_recv_buffer.size=0;
 for(i=0;i<3;i++){
   sim900_write("AT+CIPCLOSE=1\r", strlen("AT+CIPCLOSE=1\r"));
   osDelay(3000); 
	 serv_recvat_event = osMessageGet(serv_recvat_msg, 50);
	 if(serv_recvat_event.status == osEventMessage){
		  AT_Buffer=serv_recvat_event.value.p;	
		  if(strstr(AT_Buffer->data, "CLOSE OK")){ //数据缓冲区内接收到的数据存在OK
				state=0;
				memset(AT_Buffer->data, 0, AT_recv_buffer.size);
        AT_recv_buffer.size=0;
				break;
			}
			else{
			  state=1;
				memset(AT_Buffer->data, 0, AT_recv_buffer.size);
        AT_recv_buffer.size=0;
				continue;
			}
	 }
 }
  memset(AT_Buffer->data, 0,60);
	return state;
}

//获得服务器中的时间  函数返回值state为数值表示初始化失败，返回0表示初始化成功
uint8_t get_serv_time(void){
 uint8_t i=0,state=0,send_str[50];
 osEvent	serv_recvat_event;
 s_SERV_TCP_AT  *AT_Buffer;
 AT_recv_buffer.size = 0;
// IWDG_ReloadCounter();				//喂狗
 gateway_dev.serv_status.recv_status = SERV_RECV_STATUS_AT_TCP;	
	
	 for(i=0;i<3;i++){
   sim900_write("AT\r", strlen("AT\r"));
   osDelay(100); 
	 serv_recvat_event = osMessageGet(serv_recvat_msg, 50);//50
	 if(serv_recvat_event.status == osEventMessage){		  
		  AT_Buffer=serv_recvat_event.value.p;	
		  if(strstr(AT_Buffer->data, "OK")){ //数据缓冲区内接收到的数据存在OK
				state=0;
				memset(AT_Buffer->data, 0, AT_recv_buffer.size);
				AT_recv_buffer.size=0;
				break;
			}
			else{
			  state=1;
				memset(AT_Buffer->data, 0, AT_recv_buffer.size);
				AT_recv_buffer.size=0;
				continue;
			}
	 }else if(serv_recvat_event.status ==osEventTimeout){
				state=1;
				memset(AT_Buffer->data, 0, AT_recv_buffer.size);
				AT_recv_buffer.size=0;
				continue;
	 }	 
 } 
	 
	for(i=0;i<3;i++){
	sim900_write("AT+SAPBR=3,1,\"Contype\",\"GPRS\"\r", strlen("AT+SAPBR=3,1,\"Contype\",\"GPRS\"\r"));
   osDelay(100); 
	 serv_recvat_event = osMessageGet(serv_recvat_msg, 50);
	 if(serv_recvat_event.status == osEventMessage){
		  AT_Buffer=serv_recvat_event.value.p;	
		  if(strstr(AT_Buffer->data, "OK")){ //数据缓冲区内接收到的数据存在OK
				state=0;
				memset(AT_Buffer->data, 0, AT_recv_buffer.size);
				AT_recv_buffer.size=0;
				break;
			}
			else{
			  state=2;
				memset(AT_Buffer->data, 0, AT_recv_buffer.size);
				AT_recv_buffer.size=0;
				continue;
			}
	 }else if(serv_recvat_event.status ==osEventTimeout){
				state=2;
				memset(AT_Buffer->data, 0, AT_recv_buffer.size);
				AT_recv_buffer.size=0;
				continue;
	 }
 }
	
 	for(i=0;i<3;i++){
	sim900_write("AT+SAPBR=3,1,\"APN\",\"CMNET\"\r", strlen("AT+SAPBR=3,1,\"APN\",\"CMNET\"\r"));
   osDelay(100); 
	 serv_recvat_event = osMessageGet(serv_recvat_msg, 50);
	 if(serv_recvat_event.status == osEventMessage){
		  AT_Buffer=serv_recvat_event.value.p;	
		  if(strstr(AT_Buffer->data, "OK")){ //数据缓冲区内接收到的数据存在OK
				state=0;
				memset(AT_Buffer->data, 0, AT_recv_buffer.size);
				AT_recv_buffer.size=0;
				break;
			}
			else{
			  state=3;
				memset(AT_Buffer->data, 0, AT_recv_buffer.size);
				AT_recv_buffer.size=0;
				continue;
			}
	 }else if(serv_recvat_event.status ==osEventTimeout){
				state=3;
				memset(AT_Buffer->data, 0, AT_recv_buffer.size);
				AT_recv_buffer.size=0;
				continue;
	 }
 }
	
  for(i=0;i<5;i++){
	sim900_write("AT+SAPBR=1,1\r", strlen("AT+SAPBR=1,1\r"));
   osDelay(100); //7000
	 serv_recvat_event = osMessageGet(serv_recvat_msg, 50);
	 if(serv_recvat_event.status == osEventMessage){
		  AT_Buffer=serv_recvat_event.value.p;	
		  if(strstr(AT_Buffer->data, "OK")){ //数据缓冲区内接收到的数据存在OK
				state=0;
				memset(AT_Buffer->data, 0, AT_recv_buffer.size);
				AT_recv_buffer.size=0;
				break;
			}
			else{
				state=4;
				memset(AT_Buffer->data, 0, AT_recv_buffer.size);
				AT_recv_buffer.size=0;
			  continue;
			}		
	 }
	 else if(serv_recvat_event.status ==osEventTimeout){
					state=4;
					memset(AT_Buffer->data, 0, AT_recv_buffer.size);
				  AT_recv_buffer.size=0;
					continue;
	 }
 }
	
 for(i=0;i<5;i++){
	sim900_write("AT+CNTPCID=1\r", strlen("AT+CNTPCID=1\r"));
   osDelay(100); //7000
	 serv_recvat_event = osMessageGet(serv_recvat_msg, 50);
	 if(serv_recvat_event.status == osEventMessage){
		  AT_Buffer=serv_recvat_event.value.p;	
		  if(strstr(AT_Buffer->data, "OK")){ //数据缓冲区内接收到的数据存在OK
				state=0;
				memset(AT_Buffer->data, 0, AT_recv_buffer.size);
				AT_recv_buffer.size=0;
				break;
			}
			else{
				state=5;
				memset(AT_Buffer->data, 0, AT_recv_buffer.size);
				AT_recv_buffer.size=0;
			  continue;
			}		
	 }
	 else if(serv_recvat_event.status ==osEventTimeout){
					state=5;
					memset(AT_Buffer->data, 0, AT_recv_buffer.size);
				  AT_recv_buffer.size=0;
					continue;
	 }
 }
  	
 for(i=0;i<3;i++){
	sim900_write("AT+CNTP=\"202.120.2.101\",32\r", strlen("AT+CNTP=\"202.120.2.101\",32\r"));
   osDelay(100); 
	 serv_recvat_event = osMessageGet(serv_recvat_msg, 50);
	 if(serv_recvat_event.status == osEventMessage){
		  AT_Buffer=serv_recvat_event.value.p;	
		  if(strstr(AT_Buffer->data, "OK")){ //数据缓冲区内接收到的数据存在OK
				state=0;
				memset(AT_Buffer->data, 0, AT_recv_buffer.size);
				AT_recv_buffer.size=0;
				break;
			}
			else{
			  state=6;
				memset(AT_Buffer->data, 0, AT_recv_buffer.size);
				AT_recv_buffer.size=0;
				continue;
			}
	 }else if(serv_recvat_event.status ==osEventTimeout){
				state=6;
				memset(AT_Buffer->data, 0, AT_recv_buffer.size);
				AT_recv_buffer.size=0;
				continue;
	 }
 }
 
  for(i=0;i<5;i++){
	sim900_write("AT+CNTP\r", strlen("AT+CNTP\r"));
   osDelay(100); //7000
	 serv_recvat_event = osMessageGet(serv_recvat_msg, 50);
	 if(serv_recvat_event.status == osEventMessage){
		  AT_Buffer=serv_recvat_event.value.p;	
		  if(strstr(AT_Buffer->data, "OK")){ //数据缓冲区内接收到的数据存在OK
				state=0;
				memset(AT_Buffer->data, 0, AT_recv_buffer.size);
				AT_recv_buffer.size=0;
				break;
			}
			else{
				state=7;
				memset(AT_Buffer->data, 0, AT_recv_buffer.size);
				AT_recv_buffer.size=0;
			  continue;
			}		
	 }
	 else if(serv_recvat_event.status ==osEventTimeout){
					state=7;
					memset(AT_Buffer->data, 0, AT_recv_buffer.size);
				  AT_recv_buffer.size=0;
					continue;
	 }
 }
	
  for(i=0;i<3;i++){
   sim900_write("AT+CCLK?\r", strlen("AT+CCLK?\r"));	
   osDelay(100); 
	 serv_recvat_event = osMessageGet(serv_recvat_msg, 50);
	 if(serv_recvat_event.status == osEventMessage){		  
		  AT_Buffer=serv_recvat_event.value.p;	
		 if(strstr(AT_Buffer->data, "+CCLK:")){ //数据缓冲区内接收到的数据存在OK
			 
		if(gettimeval(AT_Buffer->data) == 0)
		{
			state=0;
		//	PCF8563_init();//rtc初始化
		}
		else
		{
			state=8;
		}
		
//			 judge_serv_time(AT_Buffer->data);
		memset(AT_Buffer->data, 0, AT_recv_buffer.size);
				AT_recv_buffer.size=0;
				break;
			}
			else{
			  state=8;
				memset(AT_Buffer->data, 0, AT_recv_buffer.size);
				AT_recv_buffer.size=0;
				continue;
			}
	 }else if(serv_recvat_event.status ==osEventTimeout){
				state=8;
				memset(AT_Buffer->data, 0, AT_recv_buffer.size);
				AT_recv_buffer.size=0;
				continue;
	 }
 } 

 return state;
}


//SIM900初始化 函数返回值state为数值表示初始化失败，返回0表示初始化成功
/*uint8_t sim900_init(void){
 uint8_t i=0,state=0,send_str[50];
 osEvent	serv_recvat_event;
 s_SERV_TCP_AT  *AT_Buffer;
 AT_recv_buffer.size = 0;
 IWDG_ReloadCounter();				//喂狗
 sim900_dev.serv_status.recv_status = SERV_RECV_STATUS_AT_TCP;		
 for(i=0;i<3;i++){
   sim900_write("AT\r", strlen("AT\r"));
   osDelay(100); 
	 serv_recvat_event = osMessageGet(serv_recvat_msg, 50);
	 if(serv_recvat_event.status == osEventMessage){		  
		  AT_Buffer=serv_recvat_event.value.p;	
		  if(strstr(AT_Buffer->data, "OK")){ //数据缓冲区内接收到的数据存在OK
				state=0;
				memset(AT_Buffer->data, 0, AT_recv_buffer.size);
				AT_recv_buffer.size=0;
				break;
			}
			else{
			  state=1;
				memset(AT_Buffer->data, 0, AT_recv_buffer.size);
				AT_recv_buffer.size=0;
				continue;
			}
	 }else if(serv_recvat_event.status ==osEventTimeout){
				state=1;
				memset(AT_Buffer->data, 0, AT_recv_buffer.size);
				AT_recv_buffer.size=0;
				continue;
	 }	 
 } 
 //设置回显模式 回显关闭
 for(i=0;i<3;i++){
   sim900_write("ATE0\r", strlen("ATE0\r"));	
   osDelay(100); 
	 serv_recvat_event = osMessageGet(serv_recvat_msg, 50);
	 if(serv_recvat_event.status == osEventMessage){		  
		  AT_Buffer=serv_recvat_event.value.p;	
		  if(strstr(AT_Buffer->data, "OK")){ //数据缓冲区内接收到的数据存在OK
				state=0;
				memset(AT_Buffer->data, 0, AT_recv_buffer.size);
				AT_recv_buffer.size=0;
				break;
			}
			else{
			  state=2;
				memset(AT_Buffer->data, 0, AT_recv_buffer.size);
				AT_recv_buffer.size=0;
				continue;
			}
	 }else if(serv_recvat_event.status ==osEventTimeout){
				state=2;
				memset(AT_Buffer->data, 0, AT_recv_buffer.size);
				AT_recv_buffer.size=0;
				continue;
	 }
 } 

 //用来判断SIM卡是否存在 读取SIM卡服务运营商
 sim_state.sim = 0x00;	     //bit0位为1表示没有SIM卡
 for(i=0;i<3;i++){
   sim900_write("AT+CSPN?\r",strlen("AT+CSPN?\r"));	
   osDelay(100); 
	 serv_recvat_event = osMessageGet(serv_recvat_msg, 50);
	 if(serv_recvat_event.status == osEventMessage){
		  AT_Buffer=serv_recvat_event.value.p;	
		  if(strstr(AT_Buffer->data, "+CSPN:")){ //数据缓冲区内接收到的数据存在OK
				state=0;
				sim_state.sim = 0x01;	        //bit0位为1表示有SIM卡
				memset(AT_Buffer->data, 0, AT_recv_buffer.size);
				AT_recv_buffer.size=0;
				break;
			}
			else{
			  state=3;
				memset(AT_Buffer->data, 0, AT_recv_buffer.size);
				AT_recv_buffer.size=0;
				continue;
			}
	 }
 }

 ////query attach to gprs?
 for(i=0;i<12;i++){
   sim900_write("AT+CGATT?\r", strlen("AT+CGATT?\r"));
   osDelay(100); 
	 serv_recvat_event = osMessageGet(serv_recvat_msg, 50);
	 if(serv_recvat_event.status == osEventMessage){
		  AT_Buffer=serv_recvat_event.value.p;	
		  if(strstr(AT_Buffer->data, "CGATT: 1")){ //数据缓冲区内接收到的数据存在OK
				state=0;
				memset(AT_Buffer->data, 0, AT_recv_buffer.size);
				AT_recv_buffer.size=0;
				break;
			}
			else{
			  state=4;
				memset(AT_Buffer->data, 0, AT_recv_buffer.size);
				AT_recv_buffer.size=0;
				osDelay(3000); 
				continue;
			}
	   }else if(serv_recvat_event.status ==osEventTimeout){
				state=2;
				memset(AT_Buffer->data, 0, AT_recv_buffer.size);
				AT_recv_buffer.size=0;
			 osDelay(3000); 
				continue;
	  }
  }
 
 ////获取SIM900信号强度
 for(i=0;i<3;i++){
   sim900_write("AT+CSQ\r",strlen("AT+CSQ\r"));
   osDelay(200); 
	 serv_recvat_event = osMessageGet(serv_recvat_msg, 50);
	 if(serv_recvat_event.status == osEventMessage){
		  AT_Buffer=serv_recvat_event.value.p;	
		  if(strstr(AT_Buffer->data, "+CSQ")){ //数据缓冲区内接收到的数据存在OK
				sim_state.signal = getsignal(AT_Buffer->data);
				state=0;
				memset(AT_Buffer->data, 0, AT_recv_buffer.size);
        AT_recv_buffer.size=0;
				break;
			}
			else{
			  state=1;
				memset(AT_Buffer->data, 0, AT_recv_buffer.size);
        AT_recv_buffer.size=0;
				continue;
			}
	 }
 }
	
 ////setapn + username + password
// for(i=0;i<3;i++){
//	if(user_set.netuserlen==0)
//		sprintf(send_str,"AT+CSTT=\"%s\"\r",user_set.netapn);
//	else{
//		if(user_set.netkeylen==0)
//			sprintf(send_str,"AT+CSTT=\"%s\",\"%s\"\r",user_set.netapn,user_set.netuser);
//		else sprintf(send_str,"AT+CSTT=\"%s\",\"%s\",\"%s\"\r",user_set.netapn,user_set.netuser,user_set.netkey);
//	}
//	sim900_write(send_str, strlen(send_str));
//   osDelay(100); 
//	 serv_recvat_event = osMessageGet(serv_recvat_msg, 50);
//	 if(serv_recvat_event.status == osEventMessage){
//		  AT_Buffer=serv_recvat_event.value.p;	
//		  if(strstr(AT_Buffer->data, "OK")){ //数据缓冲区内接收到的数据存在OK
//				state=0;
//				memset(AT_Buffer->data, 0, AT_recv_buffer.size);
//				AT_recv_buffer.size=0;
//				break;
//			}
//			else{
//			  state=10;
//				memset(AT_Buffer->data, 0, AT_recv_buffer.size);
//				AT_recv_buffer.size=0;
//				continue;
//			}
//	 }else if(serv_recvat_event.status ==osEventTimeout){
//				state=2;
//				memset(AT_Buffer->data, 0, AT_recv_buffer.size);
//				AT_recv_buffer.size=0;
//				continue;
//	 }
// }
 
 ////bring up wireless connection
 for(i=0;i<3;i++){
	 sim900_write("AT+CIICR\r", strlen("AT+CIICR\r"));
   osDelay(300); 
	 serv_recvat_event = osMessageGet(serv_recvat_msg, 5000);
	 if(serv_recvat_event.status == osEventMessage){
		  AT_Buffer=serv_recvat_event.value.p;	
      if(strstr(AT_Buffer->data, "OK")){ 	//
			  state=0;
				memset(AT_Buffer->data, 0, AT_recv_buffer.size);
				AT_recv_buffer.size=0;				
				break;
			}
			else{
			  state=9;
				memset(AT_Buffer->data, 0, AT_recv_buffer.size);
				AT_recv_buffer.size=0;
				osDelay(5000);				
				continue;
			}
	 }else if(serv_recvat_event.status ==osEventTimeout){
				state=2;
				memset(AT_Buffer->data, 0, AT_recv_buffer.size);
				AT_recv_buffer.size=0;
				osDelay(5000); 
				continue;
	 }
 }
 
  ////get local ip address
 for(i=0;i<3;i++){
	sim900_write("AT+CIFSR\r", strlen("AT+CIFSR\r"));
   osDelay(300); 
	 serv_recvat_event = osMessageGet(serv_recvat_msg, 50);
	 if(serv_recvat_event.status == osEventMessage){
		  AT_Buffer=serv_recvat_event.value.p;	
      if(strstr(AT_Buffer->data, "ERROR")){ 	//
			  state=9;
				memset(AT_Buffer->data, 0, AT_recv_buffer.size);
				AT_recv_buffer.size=0;
				osDelay(2000); 
				continue;
			}
			else{
			  state=0;
				memset(AT_Buffer->data, 0, AT_recv_buffer.size);
				AT_recv_buffer.size=0;
				break;
			}
	 }else if(serv_recvat_event.status ==osEventTimeout){
				state=2;
				memset(AT_Buffer->data, 0, AT_recv_buffer.size);
				AT_recv_buffer.size=0;
				osDelay(2000); 
				continue;
	 }
 }
///*****************set bearer parameter*********************/	
// for(i=0;i<3;i++){
//	sim900_write("AT+SAPBR=3,1,\"Contype\",\"GPRS\"\r", strlen("AT+SAPBR=3,1,\"Contype\",\"GPRS\"\r"));
//   osDelay(100); 
//	 serv_recvat_event = osMessageGet(serv_recvat_msg, 50);
//	 if(serv_recvat_event.status == osEventMessage){
//		  AT_Buffer=serv_recvat_event.value.p;	
//		  if(strstr(AT_Buffer->data, "OK")){ //数据缓冲区内接收到的数据存在OK
//				state=0;
//				memset(AT_Buffer->data, 0, AT_recv_buffer.size);
//				AT_recv_buffer.size=0;
//				break;
//			}
//			else{
//			  state=5;
//				memset(AT_Buffer->data, 0, AT_recv_buffer.size);
//				AT_recv_buffer.size=0;
//				continue;
//			}
//	 }else if(serv_recvat_event.status ==osEventTimeout){
//				state=2;
//				memset(AT_Buffer->data, 0, AT_recv_buffer.size);
//				AT_recv_buffer.size=0;
//				continue;
//	 }
// }

// for(i=0;i<5;i++){
//	sim900_write("AT+SAPBR=1,1\r", strlen("AT+SAPBR=1,1\r"));
//   osDelay(7000); 
//	 serv_recvat_event = osMessageGet(serv_recvat_msg, 50);
//	 if(serv_recvat_event.status == osEventMessage){
//		  AT_Buffer=serv_recvat_event.value.p;	
//		  if(strstr(AT_Buffer->data, "OK")){ //数据缓冲区内接收到的数据存在OK
//				state=0;
//				memset(AT_Buffer->data, 0, AT_recv_buffer.size);
//				AT_recv_buffer.size=0;
//				break;
//			}
//			else{
//				state=7;
//				memset(AT_Buffer->data, 0, AT_recv_buffer.size);
//				AT_recv_buffer.size=0;
//			  continue;
//			}		
//	 }
//	 else if(serv_recvat_event.status ==osEventTimeout){
//					state=7;
//					memset(AT_Buffer->data, 0, AT_recv_buffer.size);
//				  AT_recv_buffer.size=0;
//					continue;
//	 }
// }

// if(state)  return state; 
// 
////打开IP应用承载
// for(i=0;i<3;i++){
//	sim900_write("AT+SAPBR=2,1\r", strlen("AT+SAPBR=2,1\r"));
//   osDelay(300); 
//	 serv_recvat_event = osMessageGet(serv_recvat_msg, 50);
//	 if(serv_recvat_event.status == osEventMessage){
//		  AT_Buffer=serv_recvat_event.value.p;	
//      if(strstr(AT_Buffer->data, "SAPBR: 1,1,")){ 	//成功获取IP地址 
//			  state=0;
//				memset(AT_Buffer->data, 0, AT_recv_buffer.size);
//				AT_recv_buffer.size=0;
//				break;
//			}
//			else{
//			  state=9;
//				memset(AT_Buffer->data, 0, AT_recv_buffer.size);
//				AT_recv_buffer.size=0;
//				continue;
//			}
//	 }else if(serv_recvat_event.status ==osEventTimeout){
//				state=2;
//				memset(AT_Buffer->data, 0, AT_recv_buffer.size);
//				AT_recv_buffer.size=0;
//				continue;
//	 }
// }

// //设置用户名
// for(i=0;i<3;i++){
//	if(user_set.netuserlen==0)
//		sprintf(send_str,"AT+CIPCSGP=1,\"%s\"\r",user_set.netapn);
//	else{
//		if(user_set.netkeylen==0)
//			sprintf(send_str,"AT+CIPCSGP=1,\"%s\",\"%s\"\r",user_set.netapn,user_set.netuser);
//		else sprintf(send_str,"AT+CIPCSGP=1,\"%s\",\"%s\",\"%s\"\r",user_set.netapn,user_set.netuser,user_set.netkey);
//	}
//	sim900_write(send_str, strlen(send_str));
//   osDelay(100); 
//	 serv_recvat_event = osMessageGet(serv_recvat_msg, 50);
//	 if(serv_recvat_event.status == osEventMessage){
//		  AT_Buffer=serv_recvat_event.value.p;	
//		  if(strstr(AT_Buffer->data, "OK")){ //数据缓冲区内接收到的数据存在OK
//				state=0;
//				memset(AT_Buffer->data, 0, AT_recv_buffer.size);
//				AT_recv_buffer.size=0;
//				break;
//			}
//			else{
//			  state=10;
//				memset(AT_Buffer->data, 0, AT_recv_buffer.size);
//				AT_recv_buffer.size=0;
//				continue;
//			}
//	 }else if(serv_recvat_event.status ==osEventTimeout){
//				state=2;
//				memset(AT_Buffer->data, 0, AT_recv_buffer.size);
//				AT_recv_buffer.size=0;
//				continue;
//	 }
// }

/* return state;
}*/



//SIM900初始化 函数返回值state为数值表示初始化失败，返回0表示初始化成功
uint8_t sim900_init(void){
 uint8_t i=0,state=0,send_str[50];
 osEvent	serv_recvat_event;
 s_SERV_TCP_AT  *AT_Buffer;
 AT_recv_buffer.size = 0;
// IWDG_ReloadCounter();				//喂狗
 gateway_dev.serv_status.recv_status = SERV_RECV_STATUS_AT_TCP;	
 
 for(i=0;i<3;i++){
   sim900_write("AT\r", strlen("AT\r"));
   osDelay(100); 
	 serv_recvat_event = osMessageGet(serv_recvat_msg, 50);
	 if(serv_recvat_event.status == osEventMessage){		  
		  AT_Buffer=serv_recvat_event.value.p;	
		  if(strstr(AT_Buffer->data, "OK")){ //数据缓冲区内接收到的数据存在OK
				state=0;
				memset(AT_Buffer->data, 0, AT_recv_buffer.size);
				AT_recv_buffer.size=0;
				break;
			}
			else{
			  state=1;
				memset(AT_Buffer->data, 0, AT_recv_buffer.size);
				AT_recv_buffer.size=0;
				continue;
			}
	 }else if(serv_recvat_event.status ==osEventTimeout){
				state=1;
				memset(AT_Buffer->data, 0, AT_recv_buffer.size);
				AT_recv_buffer.size=0;
				continue;
	 }	 
 } 
 //设置回显模式 回显关闭
 for(i=0;i<3;i++){
   sim900_write("ATE0\r", strlen("ATE0\r"));	
   osDelay(100); 
	 serv_recvat_event = osMessageGet(serv_recvat_msg, 50);
	 if(serv_recvat_event.status == osEventMessage){		  
		  AT_Buffer=serv_recvat_event.value.p;	
		  if(strstr(AT_Buffer->data, "OK")){ //数据缓冲区内接收到的数据存在OK
				state=0;
				memset(AT_Buffer->data, 0, AT_recv_buffer.size);
				AT_recv_buffer.size=0;
				break;
			}
			else{
			  state=2;
				memset(AT_Buffer->data, 0, AT_recv_buffer.size);
				AT_recv_buffer.size=0;
				continue;
			}
	 }else if(serv_recvat_event.status ==osEventTimeout){
				state=2;
				memset(AT_Buffer->data, 0, AT_recv_buffer.size);
				AT_recv_buffer.size=0;
				continue;
	 }
 } 
 


 //用来判断SIM卡是否存在 读取SIM卡服务运营商
 sim_state.sim = 0x00;	     //bit0位为1表示没有SIM卡
 for(i=0;i<3;i++){
   sim900_write("AT+CSPN?\r",strlen("AT+CSPN?\r"));	
   osDelay(100); 
	 serv_recvat_event = osMessageGet(serv_recvat_msg, 50);
	 if(serv_recvat_event.status == osEventMessage){
		  AT_Buffer=serv_recvat_event.value.p;	
		  if(strstr(AT_Buffer->data, "+CSPN:")){ //数据缓冲区内接收到的数据存在OK
				state=0;
				sim_state.sim = 0x01;	        //bit0位为1表示有SIM卡
				memset(AT_Buffer->data, 0, AT_recv_buffer.size);
				AT_recv_buffer.size=0;
				break;
			}
			else{
			  state=3;
				memset(AT_Buffer->data, 0, AT_recv_buffer.size);
				AT_recv_buffer.size=0;
				continue;
			}
	 }
 }
 
 
  ////query attach to gprs?  add 20170907
 for(i=0;i<12;i++){
   sim900_write("AT+CGATT?\r", strlen("AT+CGATT?\r"));
   osDelay(100); 
	 serv_recvat_event = osMessageGet(serv_recvat_msg, 50);
	 if(serv_recvat_event.status == osEventMessage){
		  AT_Buffer=serv_recvat_event.value.p;	
		  if(strstr(AT_Buffer->data, "CGATT: 1")){ //数据缓冲区内接收到的数据存在OK
				state=0;
				memset(AT_Buffer->data, 0, AT_recv_buffer.size);
				AT_recv_buffer.size=0;
				break;
			}
			else{
			  state=4;
				memset(AT_Buffer->data, 0, AT_recv_buffer.size);
				AT_recv_buffer.size=0;
				osDelay(3000); 
				continue;
			}
	   }else if(serv_recvat_event.status ==osEventTimeout){
				state=2;
				memset(AT_Buffer->data, 0, AT_recv_buffer.size);
				AT_recv_buffer.size=0;
			 osDelay(3000); 
				continue;
	  }
  }
	
	 ////获取SIM900信号强度
 for(i=0;i<3;i++){
   sim900_write("AT+CSQ\r",strlen("AT+CSQ\r"));
   osDelay(200); 
	 serv_recvat_event = osMessageGet(serv_recvat_msg, 50);
	 if(serv_recvat_event.status == osEventMessage){
		  AT_Buffer=serv_recvat_event.value.p;	
		  if(strstr(AT_Buffer->data, "+CSQ")){ //数据缓冲区内接收到的数据存在OK
				sim_state.signal = getsignal(AT_Buffer->data);
				state=0;
				memset(AT_Buffer->data, 0, AT_recv_buffer.size);
        AT_recv_buffer.size=0;
				break;
			}
			else{
			  state=1;
				memset(AT_Buffer->data, 0, AT_recv_buffer.size);
        AT_recv_buffer.size=0;
				continue;
			}
	 }
 }
	 ////setapn + username + password   add 20170907
 for(i=0;i<3;i++){
	if(SCREEN_SET.netuserlen==0)
		sprintf(send_str,"AT+CSTT=\"%s\"\r",SCREEN_SET.netapn);
	else{
		if(SCREEN_SET.netkeylen==0)
			sprintf(send_str,"AT+CSTT=\"%s\",\"%s\"\r",SCREEN_SET.netapn,SCREEN_SET.netuser);
		else sprintf(send_str,"AT+CSTT=\"%s\",\"%s\",\"%s\"\r",SCREEN_SET.netapn,SCREEN_SET.netuser,SCREEN_SET.netkey);
	}
	sim900_write(send_str, strlen(send_str));
   osDelay(100); 
	 serv_recvat_event = osMessageGet(serv_recvat_msg, 50);
	 if(serv_recvat_event.status == osEventMessage){
		  AT_Buffer=serv_recvat_event.value.p;	
		  if(strstr(AT_Buffer->data, "OK")){ //数据缓冲区内接收到的数据存在OK
				state=0;
				memset(AT_Buffer->data, 0, AT_recv_buffer.size);
				AT_recv_buffer.size=0;
				break;
			}
			else{
			  state=10;
				memset(AT_Buffer->data, 0, AT_recv_buffer.size);
				AT_recv_buffer.size=0;
				continue;
			}
	 }else if(serv_recvat_event.status ==osEventTimeout){
				state=2;
				memset(AT_Buffer->data, 0, AT_recv_buffer.size);
				AT_recv_buffer.size=0;
				continue;
	 }
 }
 
 ////bring up wireless connection  add 20170907
 for(i=0;i<3;i++){
	 sim900_write("AT+CIICR\r", strlen("AT+CIICR\r"));
   osDelay(300); 
	 serv_recvat_event = osMessageGet(serv_recvat_msg, 5000);
	 if(serv_recvat_event.status == osEventMessage){
		  AT_Buffer=serv_recvat_event.value.p;	
      if(strstr(AT_Buffer->data, "OK")){ 	//
			  state=0;
				memset(AT_Buffer->data, 0, AT_recv_buffer.size);
				AT_recv_buffer.size=0;				
				break;
			}
			else{
			  state=9;
				memset(AT_Buffer->data, 0, AT_recv_buffer.size);
				AT_recv_buffer.size=0;
				osDelay(5000);				
				continue;
			}
	 }else if(serv_recvat_event.status ==osEventTimeout){
				state=2;
				memset(AT_Buffer->data, 0, AT_recv_buffer.size);
				AT_recv_buffer.size=0;
				osDelay(5000); 
				continue;
	 }
 }
 
  ////get local ip address   add 20170907
 for(i=0;i<3;i++){
	sim900_write("AT+CIFSR\r", strlen("AT+CIFSR\r"));
   osDelay(300); 
	 serv_recvat_event = osMessageGet(serv_recvat_msg, 50);
	 if(serv_recvat_event.status == osEventMessage){
		  AT_Buffer=serv_recvat_event.value.p;	
      if(strstr(AT_Buffer->data, "ERROR")){ 	//
			  state=9;
				memset(AT_Buffer->data, 0, AT_recv_buffer.size);
				AT_recv_buffer.size=0;
				osDelay(2000); 
				continue;
			}
			else{
			  state=0;
				memset(AT_Buffer->data, 0, AT_recv_buffer.size);
				AT_recv_buffer.size=0;
				break;
			}
	 }else if(serv_recvat_event.status ==osEventTimeout){
				state=2;
				memset(AT_Buffer->data, 0, AT_recv_buffer.size);
				AT_recv_buffer.size=0;
				osDelay(2000); 
				continue;
	 }
 }
 
 
 
 

 //SIM卡是否被网络注册
 /*for(i=0;i<3;i++){
   sim900_write("AT+CREG?\r", strlen("AT+CREG?\r"));
   osDelay(100); 
	 serv_recvat_event = osMessageGet(serv_recvat_msg, 50);
	 if(serv_recvat_event.status == osEventMessage){
		  AT_Buffer=serv_recvat_event.value.p;	
		  if(strstr(AT_Buffer->data, "CREG: 0,1")){ //数据缓冲区内接收到的数据存在OK
				state=0;
				memset(AT_Buffer->data, 0, AT_recv_buffer.size);
				AT_recv_buffer.size=0;
				break;
			}
			else{
			  state=4;
				memset(AT_Buffer->data, 0, AT_recv_buffer.size);
				AT_recv_buffer.size=0;
				continue;
			}
	   }else if(serv_recvat_event.status ==osEventTimeout){
				state=2;
				memset(AT_Buffer->data, 0, AT_recv_buffer.size);
				AT_recv_buffer.size=0;
				continue;
	  }
  }
 
  //获取SIM900信号强度
 for(i=0;i<3;i++){
   sim900_write("AT+CSQ\r",strlen("AT+CSQ\r"));
   osDelay(200); 
	 serv_recvat_event = osMessageGet(serv_recvat_msg, 50);
	 if(serv_recvat_event.status == osEventMessage){
		  AT_Buffer=serv_recvat_event.value.p;	
		  if(strstr(AT_Buffer->data, "+CSQ")){ //数据缓冲区内接收到的数据存在OK
				sim_state.signal = getsignal(AT_Buffer->data);
				state=0;
				memset(AT_Buffer->data, 0, AT_recv_buffer.size);
        AT_recv_buffer.size=0;
				break;
			}
			else{
			  state=1;
				memset(AT_Buffer->data, 0, AT_recv_buffer.size);
        AT_recv_buffer.size=0;
				continue;
			}
	 }
 }*/
 
 return state;
}

uint32_t sim900_read(uint8_t *buffer)
{
	uint32_t read_len;

	read_len = Dev_485->DataAvailable();
	Dev_485->ReadData(buffer, read_len);

	return read_len;
}

uint32_t sim900_write(uint8_t *buffer, uint32_t size)
{	
	uint32_t write_len;
//	write_len = RF_Lora_Dev->WriteData(buffer, size);
//	while(!RF_Lora_Dev->TxDone());	
	return write_len;
}





