/*
 * (C) Copyright 2013 Thingtek
 *
 * Written by: randy <shenminghua@thingtek.com>
 */
 

#include "rs232task.h"
#include "backgrouptask.h"
#include "common.h"


#define	RS232_SEND_MSG_SIZE		7
#define	RS232_RECV_MSG_SIZE		8


typedef enum {CPAP_STOP_RECV=0, CPAP_START_RECV=!CPAP_STOP_RECV}e_cpap_recv_flag;
//extern ARM_DRIVER_UART Driver_UART3;
//ARM_DRIVER_UART *BreathingDev = &Driver_UART3;
extern ARM_DRIVER_UART Driver_UART3;
ARM_DRIVER_UART *RS_232_Dev = &Driver_UART3;
extern SERV_USERSET user_set;
extern uint8_t test_dir;//1 向下 0向上
extern e_GPRS_REQ_FIRST_DATE_STATE gprs_req_first_date_state;
extern e_DATA_TODAY_SEND_STATE data_today_need_send; //0 无数据发送 1 有数据发送 2 正在发送 测试


extern uint8_t gprs_cmd_req_data_start;

extern s_LORA_AT   AT_recv_buffer;
extern uint16_t test_count;
extern	s_GATEWAY_DEVICE gateway_dev;
extern  s_SERV_DATA_GPRS  serv_data_gprs;
extern  s_GPRS_CMD_SERV  gprs_cmd_serv; 
extern uint8_t test_count_temp;
extern uint16_t sleep_time_num;
extern uint16_t close_screen_count;//息屏计数
extern uint8_t measure_start_prepare_count;
extern uint8_t gateway_address;//本机地址	
//extern osMessageQId(lora_send_msg);
extern osMessageQId(dev485_send_msg);
uint8_t recv_return_flag[100];
uint8_t send_unit_ID[100];
uint8_t send_constant_ID[100];
uint8_t recv_constant_flag[100];
uint8_t recv_4A_flag[100];
uint8_t recv_5A_flag[100];
uint8_t recv_6A_flag[100];
uint8_t recv_7A_flag[100];
union {
 float f;
 unsigned char x[4];
} data;
//s_CPAP_DATA		cpap_data={ {0x00 },0x00};
//s_CPAP_TCP_DATA TCP_clear_data[5]={{0x00 },0x00};

//s_SCREEN_RECV_CMD   SCREEN_recv_buffer={0};

s_RS232_RECV_CMD rs232_recv_buffer={0};

//uint8_t cpap_j = 0x00;
//uint32_t flash_temp[512];//2048/4
//uint32_t flash_temp1[128];//2048/4
	
osMessageQDef(rs232_recv_msg, RS232_RECV_MSG_SIZE, s_RS232_RECV_PACKAGE);
osMessageQDef(rs232_send_msg, RS232_SEND_MSG_SIZE, s_RS232_SEND_PACKAGE);
osMessageQId(rs232_recv_msg);
osMessageQId(rs232_send_msg);
//extern osMessageQId(serv_send_msg);
//数据区域字符转移,同时提取  //封装之后发送给云平台的数据转移
/*uint16_t Fetch_msg_Translate (uint8_t *tmsg,uint16_t len)	 //*tmsg转译前(1k)  *atmsg转译后(2k)
{
	uint16_t i=0,j=0;
	uint8_t temp[1200],flag=0;
  for(i=2,j=0;i<(len-3);i++){
		if((tmsg[i]==0x5a)&&(tmsg[i-1]==0xa5)&&(tmsg[i-2]==0x5a)){
				flag=1;
				i=i+5;
			}
		if(flag){
			temp[j++]=tmsg[i];
		}
	}
	memset(tmsg, 0, len);
	memcpy(tmsg,temp, j);
  return j;      //转译后的长度
}*/


//计算一包数据的CRC值
/*uint8_t cal_cpap_crc(s_CPAP_PACKAGE cpap_package)
{
	uint8_t crc=0;
	crc += cpap_package.head;
	crc += cpap_package.cmd;
	crc += cpap_package.data1;
	crc += cpap_package.data2;
	crc &= 0xff;	
	return crc;
}*/
float char_2_float(uint8_t *addr)
{
	/*data.x[0] = addr[0];
	data.x[1] = addr[1];
	data.x[2] = addr[2];
	data.x[3] = addr[3];*/
	data.x[0] = addr[3];
	data.x[1] = addr[2];
	data.x[2] = addr[1];
	data.x[3] = addr[0];
	return data.f;
}
uint16_t CRC_ADD(uint8_t *puchMsg, uint16_t usDataLen)
{
	uint16_t i,crc_result = 0,crc_result_temp = 0;
	for(i = 0;i < usDataLen;i += 2)
	{
		crc_result_temp = crc_result_temp | puchMsg[i];
		crc_result_temp = crc_result_temp << 8;
		crc_result_temp = crc_result_temp | puchMsg[i + 1];
		
		crc_result += crc_result_temp;
		crc_result_temp = 0;
		
	}
	return crc_result;
}

//从lora串口读取一个字节的数据
static uint8_t read_rs232_char(void)
{
	uint8_t return_char;
	RS_232_Dev->ReadData(&return_char, 1);
	return return_char;
}
/*
static void CPAP_cpature_version(uint8_t *init_ver,uint8_t cpap_ver[],uint8_t length)
{
  uint8_t i,j,m;
	char temp_buffer[4];
	for(i=0,j=0,m=0;i<=length;i++){
		if((init_ver[i]>='0')&&(init_ver[i]<='9')){
			temp_buffer[j++] =init_ver[i];
		}
    else if((init_ver[i]=='.')||(i==length)){
			temp_buffer[j] ='\0';
			j=0;			
			cpap_ver[m++] = atoi(temp_buffer);
		}  			
	}
}*/

//CPAP callback function
static void rs232_callback(ARM_UART_EVENT event)
{
	uint8_t	recv_char;
	static uint16_t length;
	static uint8_t rs232_head_flag = 0;
	//static uint8_t AT_head_flag = 0;
//	static unsigned short crc_data1;
	//unsigned int crc_data2;
	uint16_t  crc_data1;
	//unsigned char mess[6] = {0x05,0x03,0x3E,0x00,0x01,0x00};
	switch(event)
	{			
		case ARM_UART_EVENT_RX_THRESHOLD:
			
				recv_char = read_rs232_char();
		
				if(rs232_head_flag == 0)
				{
					rs232_recv_buffer.data[rs232_recv_buffer.size++]=recv_char;
					if((recv_char == 'K')&&(rs232_recv_buffer.size >= 4)&&(rs232_recv_buffer.data[rs232_recv_buffer.size - 2] == 'E')&&(rs232_recv_buffer.data[rs232_recv_buffer.size - 3] == 'T')&&(rs232_recv_buffer.data[rs232_recv_buffer.size - 4] == 'T'))//在第二个以后的位置发现0x03 且其上一个字节为从地址
					{
						rs232_head_flag = 1;//找到完整的头
						rs232_recv_buffer.data[0]='T';
						rs232_recv_buffer.data[1]='T';
						rs232_recv_buffer.data[2]='E';
						rs232_recv_buffer.data[3]='K';
						rs232_recv_buffer.size = 4;
					}
				}
				else if(rs232_head_flag == 1)
				{
					rs232_recv_buffer.data[rs232_recv_buffer.size++]=recv_char;
					if(rs232_recv_buffer.size == 12)
					{
					//	length = rs232_recv_buffer.data[11];
						length = 0;
						length = length | (rs232_recv_buffer.data[10]);
						length = length << 8;
						length = length | (rs232_recv_buffer.data[11]);
				
					}
					else if(rs232_recv_buffer.size == (length + 14))
					{
				//		crc_data1 = CRC_ADD(&(rs232_recv_buffer.data[0]), (12 + length));
						crc_data1 = cal_serv_crc(&(rs232_recv_buffer.data[0]), (12 + length));
						
						if(((crc_data1 & 0x00ff) == rs232_recv_buffer.data[rs232_recv_buffer.size - 1])&&((crc_data1 >> 8) == rs232_recv_buffer.data[rs232_recv_buffer.size - 2]))
						{
								//将数据发送到特殊队列
								osMessagePut(rs232_recv_msg, (uint32_t)(&rs232_recv_buffer), 0);
								rs232_recv_buffer.size=0;
								//一帧数据的结束
						}
						rs232_recv_buffer.size=0;
						rs232_head_flag = 0;
					}
				/*	else if(rs232_recv_buffer.size > (length + 14))
					{
						rs232_recv_buffer.size=0;
						rs232_head_flag = 0;
					}*/
						
				}				
			break;

		default:
			break;
	}
}
 

 
//function  接受从屏幕发来的命令
void rs232_recv_task(void const *argument){

	osEvent				rs232_recv_event;

	s_RS232_RECV_PACKAGE *rs232_recv_package;

	static s_RS232_SEND_PACKAGE rs232_send_package; 
	uint16_t crc_data,length = 0;
	static uint16_t num;
	static s_LORA_SEND_PACKAGE lora_send_package; 
	//uint8_t send_time_flag[50];//发送次数计数
  uint8_t i,j;
	float constant_temp,Vertical_scale_float;
	uint8_t Samp_frequency;

	uint16_t constant_int,constant_dec,Vertical_scale;
	gateway_dev.rs232_dev.rs232_recv_task_id = osThreadGetId();
	while(gateway_dev.rs232_dev.rs232_recv_task_id==NULL);	
	rs232_recv_msg = osMessageCreate( osMessageQ(rs232_recv_msg), NULL );
	rs232_send_msg = osMessageCreate( osMessageQ(rs232_send_msg), NULL );

	while(1){
		//呼吸机接收任务中每2S判断一次超时 如果数据和命令两次判断超时 则退出到心跳指令
		rs232_recv_event = osMessageGet(rs232_recv_msg, 6000);
		if(rs232_recv_event.status == osEventMessage){
			rs232_recv_package = (s_RS232_RECV_PACKAGE *)((s_RS232_RECV_CMD *)rs232_recv_event.value.p)->data;
			switch(rs232_recv_package ->cmd[1]){
				case 0x01: //测试链接
					rs232_send_package.head[0] = 'T';
					rs232_send_package.head[1] = 'T';
					rs232_send_package.head[2] = 'E';
					rs232_send_package.head[3] = 'K';
				  rs232_send_package.seq[0] = 0;
				  rs232_send_package.seq[1] = 0;
			  	rs232_send_package.type[0] = 0;
				  rs232_send_package.type[1] = 0X01;
					rs232_send_package.cmd[0] = 0;
				  rs232_send_package.cmd[1] = 0X01;
					rs232_send_package.length[0] = 0;
				  rs232_send_package.length[1] = 0X02;
					rs232_send_package.data[0] = 0;
				  rs232_send_package.data[1] = 0X01;
				
					//length = length | rs232_send_package.length[0];
					//length = length << 8;
				//	length = length | rs232_send_package.length[1];
			//	  length = rs232_send_package.length[1];
          crc_data = cal_serv_crc(&(rs232_send_package.head[0]), 14);
				 // crc_data = CRC_ADD(&(rs232_send_package.head[0]), 14);
				
					 rs232_send_package.data[2] = (crc_data & 0xff00)>> 8;;
				   rs232_send_package.data[3] = crc_data & 0x00ff;
					 					
					osMessagePut(rs232_send_msg, (uint32_t)(&rs232_send_package), 0);
				break;
				case 0x02: //查询水位仪数据  不用循环 上位机会频繁要
					length = 0;
					length = length | rs232_recv_package->length[0];
					length = length << 8;
					length = length | rs232_recv_package->length[1];
				  num = length/2 - 1;
				//  num = length/2;
				for(i = 0;i < num;i ++)
				{
					send_unit_ID[i] = rs232_recv_package->data[3+ 2*i];
				}
				//recv_return_flag

					for(i = 0;i < num;i ++)
					{

							lora_send_package.slave_addr = send_unit_ID[i];//向单元请求数据
							lora_send_package.function = 0x03;
							lora_send_package.addr[0] = 0X3E;
							lora_send_package.addr[1] = 0X00;
							lora_send_package.data[0] = 0X01;
							lora_send_package.data[1] = 0X00;
							crc_data = CRC16_MODBUS((uint8_t *)&(lora_send_package.slave_addr), 6);
							lora_send_package.crc[0] = crc_data & 0x00ff;
							lora_send_package.crc[1] = (crc_data & 0xff00)>> 8;
//							osMessagePut(lora_send_msg, (uint32_t)(&lora_send_package), 0);
							osMessagePut(dev485_send_msg, (uint32_t)(&lora_send_package), 0);
						
							osDelay(200);
						
					}
				
				memset(send_unit_ID,0,100*sizeof(uint8_t));
				memset(recv_return_flag,0,100*sizeof(uint8_t)); //发送完成标志位全部清零

				break;
				case 0x03: //下发常数
					length = 0;
					length = length | rs232_recv_package->length[0];
					length = length << 8;
					length = length | rs232_recv_package->length[1];
				 // num = length/6 - 1;
				  num = (length - 6)/7;
				  Vertical_scale_float = char_2_float(&(rs232_recv_package->data[2]));
			  	Vertical_scale = (uint16_t)Vertical_scale_float;
				
					rs232_send_package.head[0] = 'T';
					rs232_send_package.head[1] = 'T';
					rs232_send_package.head[2] = 'E';
					rs232_send_package.head[3] = 'K';
				  rs232_send_package.seq[0] = 0;
				  rs232_send_package.seq[1] = 0;
			  	rs232_send_package.type[0] = 0;
				  rs232_send_package.type[1] = 0X01;
					rs232_send_package.cmd[0] = 0;
				  rs232_send_package.cmd[1] = 0X03;
					rs232_send_package.length[0] = 0;
				  rs232_send_package.length[1] = 0X02;
					rs232_send_package.data[0] = 0;
				  rs232_send_package.data[1] = 0X01;
				
          crc_data = cal_serv_crc(&(rs232_send_package.head[0]), 14);
				
					 rs232_send_package.data[2] = (crc_data & 0xff00)>> 8;;
				   rs232_send_package.data[3] = crc_data & 0x00ff;
					 					
					osMessagePut(rs232_send_msg, (uint32_t)(&rs232_send_package), 0);//先回复上位机
					
				
				for(i = 0;i < num;i ++)
				{
					send_constant_ID[i] = rs232_recv_package->data[7 + 7*i];
				}
				//recv_return_flag
				 for(i = 0;i < 3;i ++)//常数下发到单元
				{
					for(j = 0;j <num;j++)
					{
						if(recv_constant_flag[j] == 0)//未收到单元回复							
						{				
							
							  constant_temp = char_2_float(&(rs232_recv_package->data[8 + 7*j]));
							  Samp_frequency = rs232_recv_package->data[12 + 7*j];
						  	constant_int = (uint16_t)constant_temp;//整数部分
						  	constant_dec = ((uint16_t)(constant_temp*100))%100;	//小数部分 *10
							
								lora_send_package.slave_addr = send_constant_ID[j];//写探针采集周期
								lora_send_package.function = 0x05;
								lora_send_package.addr[0] = 0X4A;
							  lora_send_package.addr[1] = 0X00;
								lora_send_package.data[0] = Samp_frequency;
								lora_send_package.data[1] = 0X00;
							  crc_data = CRC16_MODBUS((uint8_t *)&(lora_send_package.slave_addr), 6);
							  lora_send_package.crc[0] = crc_data & 0x00ff;
						  	lora_send_package.crc[1] = (crc_data & 0xff00)>> 8;
//	        			osMessagePut(lora_send_msg, (uint32_t)(&lora_send_package), 0);
							  osMessagePut(dev485_send_msg, (uint32_t)(&lora_send_package), 0);
							
							  osDelay(200);
							
								lora_send_package.slave_addr = send_constant_ID[j];//向单元发送测针常数小数部分
								lora_send_package.function = 0x05;
								lora_send_package.addr[0] = 0X6A;
							  lora_send_package.addr[1] = 0X00;
								lora_send_package.data[0] = constant_dec & 0x00ff;
								lora_send_package.data[1] = (constant_dec & 0xff00)>> 8;
							  crc_data = CRC16_MODBUS((uint8_t *)&(lora_send_package.slave_addr), 6);
							  lora_send_package.crc[0] = crc_data & 0x00ff;
						  	lora_send_package.crc[1] = (crc_data & 0xff00)>> 8;
//	        			osMessagePut(lora_send_msg, (uint32_t)(&lora_send_package), 0);
							  osMessagePut(dev485_send_msg, (uint32_t)(&lora_send_package), 0);
							
							  osDelay(200);
							
								lora_send_package.slave_addr = send_constant_ID[j];//向单元发送测针常数整数部分
								lora_send_package.function = 0x05;
								lora_send_package.addr[0] = 0X7A;
							  lora_send_package.addr[1] = 0X00;
								lora_send_package.data[0] = constant_int & 0x00ff;
								lora_send_package.data[1] = (constant_int & 0xff00)>> 8;
							  crc_data = CRC16_MODBUS((uint8_t *)&(lora_send_package.slave_addr), 6);
							  lora_send_package.crc[0] = crc_data & 0x00ff;
						  	lora_send_package.crc[1] = (crc_data & 0xff00)>> 8;
//	        			osMessagePut(lora_send_msg, (uint32_t)(&lora_send_package), 0);
							  osMessagePut(dev485_send_msg, (uint32_t)(&lora_send_package), 0);
							
							  osDelay(200);
								
								lora_send_package.slave_addr = send_constant_ID[j];//向单元发送垂直比尺
								lora_send_package.function = 0x05;
								lora_send_package.addr[0] = 0X5A;
							  lora_send_package.addr[1] = 0X00;
								lora_send_package.data[0] = Vertical_scale & 0x00ff;
								lora_send_package.data[1] = (Vertical_scale & 0xff00)>> 8;
							  crc_data = CRC16_MODBUS((uint8_t *)&(lora_send_package.slave_addr), 6);
							  lora_send_package.crc[0] = crc_data & 0x00ff;
						  	lora_send_package.crc[1] = (crc_data & 0xff00)>> 8;
//	        			osMessagePut(lora_send_msg, (uint32_t)(&lora_send_package), 0);
							  osMessagePut(dev485_send_msg, (uint32_t)(&lora_send_package), 0);
							
							  osDelay(200);
						}
					}
				}
				memset(send_constant_ID,0,100*sizeof(uint8_t));
				memset(recv_constant_flag,0,100*sizeof(uint8_t)); //发送完成标志位全部清零
				memset(recv_4A_flag,0,100*sizeof(uint8_t));
				memset(recv_5A_flag,0,100*sizeof(uint8_t));
				memset(recv_6A_flag,0,100*sizeof(uint8_t));
				memset(recv_7A_flag,0,100*sizeof(uint8_t));

				
				/*	rs232_send_package.slave_addr = local_address;
				  rs232_send_package.function = 0x03;
				  rs232_send_package.num = 0x02;
					rs232_send_package.data[0] = 0xff;
				  rs232_send_package.data[1] = 0xff;
				crc_data = CRC16_MODBUS((uint8_t *)&(rs232_send_package.slave_addr), 5);
				rs232_send_package.crc[0] = crc_data & 0x00ff;
				rs232_send_package.crc[1] = (crc_data & 0xff00)>> 8;*/
						
		//			osMessagePut(rs232_send_msg, (uint32_t)(&rs232_send_package), 0);
				break;
				case 0x04: //开始测量
					lora_send_package.slave_addr = rs232_recv_package->data[3];//开始测量
					lora_send_package.function = 0x05;
					lora_send_package.addr[0] = 0X2E;
					lora_send_package.addr[1] = 0X00;
					lora_send_package.data[0] = 0X00;
					lora_send_package.data[1] = 0XFF;
					crc_data = CRC16_MODBUS((uint8_t *)&(lora_send_package.slave_addr), 6);
					lora_send_package.crc[0] = crc_data & 0x00ff;
					lora_send_package.crc[1] = (crc_data & 0xff00)>> 8;
	//				osMessagePut(lora_send_msg, (uint32_t)(&lora_send_package), 0);
					osMessagePut(dev485_send_msg, (uint32_t)(&lora_send_package), 0);
//					osDelay(100);
	
				
				break;
				case 0x05: //停止测量
	
					lora_send_package.slave_addr = rs232_recv_package->data[3];//停止测量
					lora_send_package.function = 0x05;
					lora_send_package.addr[0] = 0X2E;
					lora_send_package.addr[1] = 0X00;
					lora_send_package.data[0] = 0X00;
					lora_send_package.data[1] = 0X00;
					crc_data = CRC16_MODBUS((uint8_t *)&(lora_send_package.slave_addr), 6);
					lora_send_package.crc[0] = crc_data & 0x00ff;
					lora_send_package.crc[1] = (crc_data & 0xff00)>> 8;
//					osMessagePut(lora_send_msg, (uint32_t)(&lora_send_package), 0);
					osMessagePut(dev485_send_msg, (uint32_t)(&lora_send_package), 0);
				
				break;
				default:
					break;
				
			 }


		 }	 //命令超时及数据超时判断 若长时间命令或者数据状态未改变为心跳则自动结束命令退出
     else if(rs232_recv_event.status == osEventTimeout){ //增加呼吸机命令接收超时判断
			//呼吸机命令超时6S仍为接收状态更改为CPAP_RECV_STATUS_NO
//			nameCount = 0x00;
//			sim900_dev.cpap_status.recv_status = CPAP_RECV_STATUS_NO;	
	  }
  }
}


void rs232_send_task(void const *argument){
	osEvent				rs232_send_event;

	s_RS232_SEND_PACKAGE *rs232_send_package;
	uint32_t length = 0;

	gateway_dev.rs232_dev.rs232_send_task_id = osThreadGetId();
	while(gateway_dev.rs232_dev.rs232_send_task_id == NULL);
	
	while(1){
		rs232_send_event = osMessageGet(rs232_send_msg, osWaitForever);

		if(rs232_send_event.status == osEventMessage){
			rs232_send_package = rs232_send_event.value.p;
			
		//	length = length | rs232_send_package->length[0];
		//	length = length << 8;
		//	length = length | rs232_send_package->length[1];
			length = rs232_send_package->length[1];
			
			LED1_ON;
			RS_232_Dev->WriteData((uint8_t *)rs232_send_package, length + 14);
		  LED1_OFF;
		}
		//osDelay(50);
	}
}

void RS_232_Dev_init()
{
	extern ARM_DRIVER_UART *RS_232_Dev;
	
	RS_232_Dev->Initialize(rs232_callback, 16);
	RS_232_Dev->PowerControl(ARM_POWER_FULL);

	RS_232_Dev->Configure(9600, 8, ARM_UART_PARITY_NONE, ARM_UART_STOP_BITS_1, ARM_UART_FLOW_CONTROL_NONE);
	RS_232_Dev->SetRxThreshold(1);
}

