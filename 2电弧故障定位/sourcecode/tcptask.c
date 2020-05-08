/*
 * (C) Copyright 2013 Thingtek
 *
 * Written by: randy <shenminghua@thingtek.com>
 */
 

#include "tcptask.h"
#include "backgrouptask.h"
#include "common.h"
#include "rs232task.h"
//#include "usart.h"	  

#define	LORA_SEND_MSG_SIZE		8 //��������
#define	LORA_RECV_MSG_SIZE		7 //��������


typedef enum {CPAP_STOP_RECV=0, CPAP_START_RECV=!CPAP_STOP_RECV}e_cpap_recv_flag;
//extern ARM_DRIVER_UART Driver_UART3;
//ARM_DRIVER_UART *BreathingDev = &Driver_UART3;
//extern ARM_DRIVER_UART Driver_UART2;
//ARM_DRIVER_UART *RF_Lora_Dev = &Driver_UART2;
extern SERV_USERSET user_set;
extern uint8_t test_dir;//1 ���� 0����
extern e_GPRS_REQ_FIRST_DATE_STATE gprs_req_first_date_state;
extern e_DATA_TODAY_SEND_STATE data_today_need_send; //0 �����ݷ��� 1 �����ݷ��� 2 ���ڷ��� ����
extern uint8_t RAM_current;//��ǰ���ڷ������ݵ�ram���
extern uint8_t RAM_flag[4];//ram���Ƿ�������
extern uint8_t Sending;//���ڷ���
extern volatile uint8_t ram_flag;
SERV_SIM sim_state;
s_SCREEN_SET SCREEN_SET;
s_MODULE_SET MODULE_SET;
s_SCREEN_DATA SCREEN_DATA;
s_SCREEN_DATA SCREEN_DATA_Temp;
s_SCREEN_MEASURE SCREEN_MEASURE;
e_SCREEN_STATE SCREEN_STATE;
e_STANDBY_ENTER_MODE STANDBY_ENTER_MODE;
extern uint8_t gprs_cmd_req_data_start;
uint8_t measure_working;//�������б�־
uint8_t screen_set_updata;
uint8_t screen_standard_updata;
uint8_t set_current_standard_updata;
uint8_t measure_start_prepare_flag = 0;

extern s_LORA_AT   AT_recv_buffer;
extern uint16_t test_count;
extern	s_GATEWAY_DEVICE gateway_dev;
extern  s_SERV_DATA_GPRS  serv_data_gprs;
extern  s_GPRS_CMD_SERV  gprs_cmd_serv; 
extern uint8_t test_count_temp;
extern uint16_t sleep_time_num;
extern uint16_t close_screen_count;//Ϣ������
extern uint8_t measure_start_prepare_count;
extern uint8_t gateway_address;//������ַ	
extern osMessageQId(rs232_send_msg);

extern uint8_t recv_return_flag[50];
extern uint8_t send_unit_ID[50];
extern uint8_t send_constant_ID[100];
extern uint8_t recv_constant_flag[100];
extern uint8_t recv_4A_flag[100];
extern uint8_t recv_5A_flag[100];
extern uint8_t recv_6A_flag[100];
extern uint8_t recv_7A_flag[100];
extern uint8_t TCP_Link_OK;

s_CPAP_DATA		cpap_data={ {0x00 },0x00};
s_CPAP_TCP_DATA TCP_clear_data[5]={{0x00 },0x00};

s_SCREEN_RECV_CMD   SCREEN_recv_buffer={0};
//s_TCP_RECV_CMD lora_recv_buffer={0};
//s_FROM_SERV_TCP_CMD from_serv_tcp_cmd={0};
uint8_t cpap_j = 0x00;
uint32_t flash_temp[512];//2048/4
//uint32_t flash_temp1[128];//2048/4

uint8_t Tcp_Ask_cmd[20];	

osMessageQDef(tcp_recv_msg, 10, s_TCP_ASK_CMD_PACKAGE);
osMessageQDef(tcp_send_msg, 8, s_LORA_SEND_PACKAGE);
osMessageQId(tcp_recv_msg);
osMessageQId(tcp_send_msg);
//extern osMessageQId(serv_send_msg);
//���������ַ�ת��,ͬʱ��ȡ  //��װ֮���͸���ƽ̨������ת��
uint16_t Fetch_msg_Translate (uint8_t *tmsg,uint16_t len)	 //*tmsgת��ǰ(1k)  *atmsgת���(2k)
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
  return j;      //ת���ĳ���
}


//����һ�����ݵ�CRCֵ
uint8_t cal_cpap_crc(s_CPAP_PACKAGE cpap_package)
{
	uint8_t crc=0;
	crc += cpap_package.head;
	crc += cpap_package.cmd;
	crc += cpap_package.data1;
	crc += cpap_package.data2;
	crc &= 0xff;	
	return crc;
}
//��lora���ڶ�ȡһ���ֽڵ�����
static uint8_t read_lora_char(void)
{
	uint8_t return_char;
//	RF_Lora_Dev->ReadData(&return_char, 1);
	return return_char;
}

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
}
/*
************************************************************
*	�������ƣ�	USART2_IRQHandler
*
*	�������ܣ�	����2�շ��ж�
*
*	��ڲ�����	��
*
*	���ز�����	��
*
*	˵����		
************************************************************
*/
/*
void USART2_IRQHandler(void)
{
	uint8_t	recv_char;
		static uint8_t lora_head_flag = 0;
	//static uint8_t AT_head_flag = 0;
//	static unsigned short crc_data1;
	//unsigned int crc_data2;
	uint16_t  crc_data1;
	static s_FROM_SERV_TCP_CMD from_serv_tcp_cmd={0};
	static uint8_t Tcp_cmd_flag = 0;
	
  if (USART_GetFlagStatus(USART2, USART_FLAG_ORE) != RESET)
    {
        recv_char = USART_ReceiveData(USART2);
			
				switch(recv_char)
				{
					case 0x7e:
              if(Tcp_cmd_flag == 0)
							{
								Tcp_cmd_flag = 1;
								from_serv_tcp_cmd.size=0;
								from_serv_tcp_cmd.data[from_serv_tcp_cmd.size++]=recv_char;								
							}
						break;

					case 0x7d:
							if(Tcp_cmd_flag == 1)
							{
								from_serv_tcp_cmd.data[from_serv_tcp_cmd.size++]=recv_char;
			
								//���ܵ�����֡��7E 7Dת��Ϊ7C 5E��
								from_serv_tcp_cmd.size=Recv_msg_Translate (from_serv_tcp_cmd.data,from_serv_tcp_cmd.size);
								//�����ݷ��͵��������
								osMessagePut(tcp_recv_msg, (uint32_t)(&from_serv_tcp_cmd), 0);
								from_serv_tcp_cmd.size=0;
								//һ֡���ݵĽ���
								Tcp_cmd_flag = 0;
							}


						break;
							
					default:
						if(Tcp_cmd_flag == 1)
						{
							from_serv_tcp_cmd.data[from_serv_tcp_cmd.size++]=recv_char;
						}
							
							if(from_serv_tcp_cmd.size>20){
								from_serv_tcp_cmd.size=0; 
							  //���ݳ���20������Ϊ���ݴ���
								Tcp_cmd_flag = 0;
							}
					break;
					
				}
    }
//		if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
//    {

//        USART_ReceiveData(USART2);
//        
//        USART_ClearITPendingBit(USART2, USART_IT_RXNE);
//    }

		USART_ClearITPendingBit(USART2, USART_IT_RXNE);
}*/

//CPAP callback function
static void lora_callback(ARM_UART_EVENT event)
{
	uint8_t	recv_char;
	static uint8_t lora_head_flag = 0;
	//static uint8_t AT_head_flag = 0;
//	static unsigned short crc_data1;
	//unsigned int crc_data2;
	uint16_t  crc_data1;
	static s_FROM_SERV_TCP_CMD from_serv_tcp_cmd={0};
	static uint8_t Tcp_cmd_flag = 0;
	//unsigned char mess[6] = {0x05,0x03,0x3E,0x00,0x01,0x00};
	switch(event)
	{			
		case ARM_UART_EVENT_RX_THRESHOLD:
			
		//	if (USART_GetFlagStatus(USART2, USART_FLAG_ORE) != RESET)
			{
				recv_char = read_lora_char();
		    switch(recv_char)
				{
					case 0x7e:
              if(Tcp_cmd_flag == 0)
							{
								Tcp_cmd_flag = 1;
								from_serv_tcp_cmd.size=0;
								from_serv_tcp_cmd.data[from_serv_tcp_cmd.size++]=recv_char;								
							}
						break;

					case 0x7d:
							if(Tcp_cmd_flag == 1)
							{
								from_serv_tcp_cmd.data[from_serv_tcp_cmd.size++]=recv_char;
			
								//���ܵ�����֡��7E 7Dת��Ϊ7C 5E��
								from_serv_tcp_cmd.size=Recv_msg_Translate (from_serv_tcp_cmd.data,from_serv_tcp_cmd.size);
								//�����ݷ��͵��������
								osMessagePut(tcp_recv_msg, (uint32_t)(&from_serv_tcp_cmd), 0);
								from_serv_tcp_cmd.size=0;
								//һ֡���ݵĽ���
								Tcp_cmd_flag = 0;
							}


						break;
							
					default:
						if(Tcp_cmd_flag == 1)
						{
							from_serv_tcp_cmd.data[from_serv_tcp_cmd.size++]=recv_char;
						}
							
							if(from_serv_tcp_cmd.size>20){
								from_serv_tcp_cmd.size=0; 
							  //���ݳ���20������Ϊ���ݴ���
								Tcp_cmd_flag = 0;
							}
					break;
					
				}
			}
			break;

		default:
			break;
	}
}
 

 
//function  ���ܴ���Ļ����������
void tcp_recv_task(void const *argument){

	osEvent				lora_recv_event;

	s_LORA_RECV_PACKAGE *lora_recv_package;
	s_LORA_SEND_PACKAGE *lora_send_package_p; 
	s_TCP_RECV_CMD *tcp_recv_buffer;
	static u16 Vol;
	static u8 Res,temp_res;
	static s_RS232_SEND_PACKAGE rs232_send_package; 
	static s_TCP_ASK_CMD_PACKAGE tcp_ask_cmd_package;
//	static s_LORA_SEND_PACKAGE lora_send_package; 
	uint16_t crc_data,length,i,Translate_size,temp_ID;

	gateway_dev.lora_dev.lora_recv_task_id = osThreadGetId();
	while(gateway_dev.lora_dev.lora_recv_task_id==NULL);	
	tcp_recv_msg = osMessageCreate( osMessageQ(tcp_recv_msg), NULL );
	tcp_send_msg = osMessageCreate( osMessageQ(tcp_send_msg), NULL );

	while(1){
		//����������������ÿ2S�ж�һ�γ�ʱ ������ݺ����������жϳ�ʱ ���˳�������ָ��
		lora_recv_event = osMessageGet(tcp_recv_msg, 100);
		if(lora_recv_event.status == osEventMessage){
		//	lora_recv_package = (s_LORA_RECV_PACKAGE *)((s_LORA_RECV_CMD *)lora_recv_event.value.p)->data;
			tcp_recv_buffer = (s_TCP_RECV_CMD *)lora_recv_event.value.p;
			
			temp_ID = 0; 
			temp_ID = temp_ID | tcp_recv_buffer->data[3];
			temp_ID = temp_ID << 8;
			temp_ID = temp_ID | tcp_recv_buffer->data[2];
			
			if(temp_ID == MODULE_ID)//id�뱾��Ԫ��ͬ
			{
				switch(tcp_recv_buffer->data[1]){
					case 0x12: //���ڱ�����ֵ
						if(Sending == 0)//���ڷ���ʱ��������Ч �ڷ���״̬�ܵ��������� ֱ�Ӻ���
						{						
					   	LED1_ON;
							Vol = (u16)(tcp_recv_buffer->data[4])|(u16)((tcp_recv_buffer->data[5])<<8);
							MODULE_SET.DAC_val = Vol;
					    Dac1_Set_Vol(Vol);
					
							tcp_ask_cmd_package.head = 0x7E;
							tcp_ask_cmd_package.state = 0x02;
							tcp_ask_cmd_package.module_id = MODULE_ID;//2�ֽ�
							
							memcpy(Tcp_Ask_cmd,(void*)(&tcp_ask_cmd_package.head) , 4);
							crc_data=cal_serv_crc_N((Tcp_Ask_cmd+1), 3,0x00);
							Tcp_Ask_cmd[5] = (crc_data&0x00ff);
							Tcp_Ask_cmd[4] = (crc_data&0xff00)>>8;
							//��������֮��ת���ַ�  
							Translate_size = Send_msg_Translate (Tcp_Ask_cmd+1,5);
							Tcp_Ask_cmd[Translate_size+1] = 0x7d;
//							RF_Lora_Dev->WriteData(Tcp_Ask_cmd, Translate_size + 2);
					    Usart_SendString(USART2, Tcp_Ask_cmd, Translate_size + 2);
							
							flash_writedata_16(SCREEN_SET_ADDRESS, &(MODULE_SET.flag));
							
					    LED1_OFF;
						}
					break;
					case 0x14: //���ڷŴ���
						if(Sending == 0)//���ڷ���ʱ��������Ч �ڷ���״̬�ܵ��������� ֱ�Ӻ���
						{						
					   	LED1_ON;
							Res = tcp_recv_buffer->data[4];
					    MODULE_SET.RES_val = Res;
				      MCP4017_WriteOneByte(MODULE_SET.RES_val);
							
							temp_res = MCP4017_ReadOneByte();
							if(temp_res == Res)
							{
								tcp_ask_cmd_package.head = 0x7E;
								tcp_ask_cmd_package.state = 0x04;
								tcp_ask_cmd_package.module_id = MODULE_ID;
								
								memcpy(Tcp_Ask_cmd,(void*)(&tcp_ask_cmd_package.head) , 4);
								crc_data=cal_serv_crc_N((Tcp_Ask_cmd+1), 3,0x00);
								Tcp_Ask_cmd[5] = (crc_data&0x00ff);
								Tcp_Ask_cmd[4] = (crc_data&0xff00)>>8;
								//��������֮��ת���ַ�  
								Translate_size = Send_msg_Translate (Tcp_Ask_cmd+1,5);
								Tcp_Ask_cmd[Translate_size+1] = 0x7d;
	//							RF_Lora_Dev->WriteData(Tcp_Ask_cmd, Translate_size + 2);
								Usart_SendString(USART2, Tcp_Ask_cmd, Translate_size + 2);
								
								flash_writedata_16(SCREEN_SET_ADDRESS, &(MODULE_SET.flag));
							}

					    LED1_OFF;
						}
					break;
					case 0x10: //�ɹ�
						ram_flag = 0;//������ɺ�����       ���û�յ��ɹ����� ��һֱ��������
					break;
					case 0x11: //ʧ��
						//�ڷ������ݺ�100ms���յ���ʧ�ܵķ��� ����ǰRAM���ݱ��� ��־λ������һ ����һ��ѭ������
					//		RAM_flag[RAM_current] = 1;//	
					break;
					case 0x13: //���ӳɹ�
						TCP_Link_OK = 1;
					break;
					default: 
						
					break;
						
					
				}
			}

		 }	 //���ʱ�����ݳ�ʱ�ж� ����ʱ�������������״̬δ�ı�Ϊ�������Զ����������˳�
     else if(lora_recv_event.status == osEventTimeout){ //���Ӻ�����������ճ�ʱ�ж�

	  }
  }
}


void lora_send_task(void const *argument){
	osEvent				tcp_send_event;

	s_LORA_SEND_PACKAGE *lora_send_package;
  uint8_t lora_add_head[15];
	gateway_dev.lora_dev.lora_send_task_id = osThreadGetId();
	while(gateway_dev.lora_dev.lora_send_task_id == NULL);

	while(1){
		tcp_send_event = osMessageGet(tcp_send_msg, osWaitForever);

		if(tcp_send_event.status == osEventMessage){
			lora_send_package = tcp_send_event.value.p;
			
		/*	lora_add_head[0] = 0xFF;
			lora_add_head[1] = 0xFF;//��ͬһ�ŵ��㲥
			lora_add_head[2] = 23;//�ŵ�
			memcpy(&lora_add_head[3],(uint8_t *)lora_send_package,LORA_SEND_MSG_SIZE);
			*/
			LED2_ON;
	//		RF_Lora_Dev->WriteData(lora_add_head, LORA_SEND_MSG_SIZE+3);
//			RF_Lora_Dev->WriteData((uint8_t *)lora_send_package, LORA_SEND_MSG_SIZE);
			LED2_OFF;
	//		
			/*cpap_send_package = cpap_send_event.value.p;
      sim900_dev.cpap_status.recv_status = CPAP_RECV_STATUS_NO; 
			cpap_send_package->crc = cal_cpap_crc(*cpap_send_package);

			RF_Lora_Dev->WriteData((uint8_t *)cpap_send_package, CPAP_DEAULT_PACKAGE_LEN);*/
			
		}
	//	osDelay(100);
	}
}

/*void RF_Lora_Dev_init()
{
	extern ARM_DRIVER_UART *RF_Lora_Dev;
	
	RF_Lora_Dev->Initialize(lora_callback, 16);
	RF_Lora_Dev->PowerControl(ARM_POWER_FULL);
//921600
	RF_Lora_Dev->Configure(460800, 8, ARM_UART_PARITY_NONE, ARM_UART_STOP_BITS_1, ARM_UART_FLOW_CONTROL_NONE);
	RF_Lora_Dev->SetRxThreshold(1);
}*/

