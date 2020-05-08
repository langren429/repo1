/*
 * (C) Copyright 2013 Thingtek
 *
 * Written by: randy <shenminghua@thingtek.com>
 */
 
#include "common.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "usart.h"	 

//��ʱ������1���ж�
osTimerDef(Timer,Timer_Callback);
//osTimerId  Timer_1s;
osTimerId  id1;
//��ʱ������1���ж�
//osTimerDef(Timer2,Timer2_Callback);

//osTimerId  id2;

//PCB_VERSIONΪ1��ʾ�ɰ� Ϊ2��ʾ�°�SERV_USERSET user_set;
//SERV_SIM sim_state;
extern s_SCREEN_SET SCREEN_SET;
extern s_MODULE_SET MODULE_SET;
extern s_SCREEN_DATA SCREEN_DATA;
extern osMessageQId(lora_send_msg);
extern e_SCREEN_STATE SCREEN_STATE;
extern e_SETTLE_WORK_STATE SETTLE_WORK_STATE;
extern uint32_t flash_temp[512];//2048/4
extern uint8_t  cmd_respond[100];
SERV_USERSET user_set;
extern e_STANDBY_ENTER_MODE STANDBY_ENTER_MODE;
extern uint32_t voltage_temp;
extern uint32_t first_date_from_server_save; //������յ����������صĵ�һ������� 
extern uint8_t gprs_cmd_req_data_start; //�������������ݿ�ʼ
extern uint32_t serv_back_req_date_end;
extern s_LCM_SET LCM_SET;
uint32_t read_flash_measure_data[12];
u8 temp=0;
float temp1;


//��У���룬���뷽ʽ��CRC16
unsigned short cal_serv_crc(unsigned char *message, unsigned int len)
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
}

int leap(uint8_t year)
{
 if((year%4==0) && (year%100!=0) || (year%400==0))
    return 1;
 else return 0;  
}

uint8_t days(uint8_t year ,uint8_t month)
{
 if(leap(year)&&month==2)
  return 29;
 if(month==2)
  return 28;
 if(month==4 || month==6 || month==9 || month==11)
     return 30;
 return 31;
}

struct Date nextDay(struct Date d)
{
 if(d.day==days(d.year,d.month))
 {
  d.day = 1;
  d.month=(d.month)+1;
  if(d.month==1) d.year++;
 }
 else d.day++;
 return d; 
}

struct Date nextNDay(struct Date d,uint8_t n)
{
 while(n--)
 {
  d = nextDay(d);
 }
  return d; 
}
/*int32_t sum(uint8_t y,uint8_t m,uint8_t d)
{
	uint8_t x[13]={0,31,28,31,30,31,30,31,31,30,31,30,31};
	int i,s=0;
	for(i=1;i<y;i++)
	if(i%4==0 && i%100!=0 || i%400==0)
	s+=366;//����
	else
	s+=365;//ƽ��

	if(y%4==0 && y%100!=0 || y%400==0)
	x[2]=29;

	for(i=1;i<m;i++)
	s+=x[i];//���µ�����
	s+=d;//�յ�����

	return s;//��������������Թ�Ԫ1��
}*/

int32_t day_diff(uint8_t year_start, uint8_t month_start, uint8_t day_start
   , uint8_t year_end, uint8_t month_end, uint8_t day_end)
{
 int y2, m2, d2;
 int y1, m1, d1;
 
 m1 = (month_start + 9) % 12;
 y1 = year_start - m1/10;
 d1 = 365*y1 + y1/4 - y1/100 + y1/400 + (m1*306 + 5)/10 + (day_start - 1);

 m2 = (month_end + 9) % 12;
 y2 = year_end - m2/10;
 d2 = 365*y2 + y2/4 - y2/100 + y2/400 + (m2*306 + 5)/10 + (day_end - 1);
 
 return (d2 - d1);
}

void flash_writeset()
{	
	uint8_t i;
	uint32_t *p;
	FLASH_Status  flash_status; //FLASH��ǰ״̬
	p = (uint32_t *)user_set.tcpip;
	FLASH_ClearFlag(FLASH_FLAG_PGERR|FLASH_FLAG_WRPRTERR|FLASH_FLAG_EOP);//�����־λ
	//FLASH������254ҳ��ÿ�β�����СΪ2K
	do{
		flash_status = FLASH_ErasePage(USERSET_ADDRESS);
	}while(flash_status!=FLASH_COMPLETE);
	
	for(i=0;i<31;i++)
	{
	//FLASHÿ��д������Ϊ��λ
		FLASH_ProgramWord( (USERSET_ADDRESS+i*4), *(p+i) );   //  
	}
}



void flash_writepage(uint32_t Page_Address, uint32_t *measure_data)
{	uint8_t i;
	uint32_t *p;
	FLASH_Status  flash_status; //FLASH��ǰ״̬
	p = measure_data;
	FLASH_ClearFlag(FLASH_FLAG_PGERR|FLASH_FLAG_WRPRTERR|FLASH_FLAG_EOP);//�����־λ
	//FLASH������254ҳ��ÿ�β�����СΪ2K
	do{

		flash_status = FLASH_ErasePage(Page_Address);

	}while(flash_status!=FLASH_COMPLETE);
	

		for(i=0;i<128;i++)
	{
	//FLASHÿ��д������Ϊ��λ
		FLASH_ProgramWord( (Page_Address+i*4), *(p+i) );   //  

	}
	 for(i=0;i<128;i++)
	{
	//FLASHÿ��д������Ϊ��λ
		FLASH_ProgramWord( (Page_Address+i*4+512), *(p+i+128) );   //  

	}
		 for(i=0;i<128;i++)
	{
	//FLASHÿ��д������Ϊ��λ
		FLASH_ProgramWord( (Page_Address+i*4+1024), *(p+i+256) );   //  

	}
		 for(i=0;i<128;i++)
	{
	//FLASHÿ��д������Ϊ��λ
		FLASH_ProgramWord( (Page_Address+i*4+1536), *(p+i+384) );   //  

	}
}

void flash_writedata(uint32_t Page_Address, uint32_t *measure_data)
{	uint8_t i;
	uint32_t *p;
	FLASH_Status  flash_status; //FLASH��ǰ״̬
	p = measure_data;
	FLASH_ClearFlag(FLASH_FLAG_PGERR|FLASH_FLAG_WRPRTERR|FLASH_FLAG_EOP);//�����־λ
	//FLASH������254ҳ��ÿ�β�����СΪ2K
	do{
		flash_status = FLASH_ErasePage(Page_Address);
	}while(flash_status!=FLASH_COMPLETE);
	
	for(i=0;i<6;i++)
	{
	//FLASHÿ��д������Ϊ��λ
		FLASH_ProgramWord( (Page_Address+i*4), *(p+i) );   //  

	}
}

void flash_writedata_16(uint32_t Page_Address, uint16_t *measure_data)
{	uint8_t i;
	uint16_t *p;
	FLASH_Status  flash_status; //FLASH��ǰ״̬
	p = measure_data;
	FLASH_ClearFlag(FLASH_FLAG_PGERR|FLASH_FLAG_WRPRTERR|FLASH_FLAG_EOP);//�����־λ
	//FLASH������254ҳ��ÿ�β�����СΪ2K
	do{
		flash_status = FLASH_ErasePage(Page_Address);
	}while(flash_status!=FLASH_COMPLETE);
	
	for(i=0;i<3;i++)
	{
	//FLASHÿ��д������Ϊ��λ

		FLASH_ProgramHalfWord((Page_Address+i*2), *(p+i) );

	}
}

uint32_t flash_read_data(uint32_t Page_Address)
{

	return *(vu32 *)Page_Address;
	
}

void flash_read_item(uint32_t Page_Address)
{
	uint8_t i;
	for(i = 0;i < 12;i ++)
	{
		read_flash_measure_data[i] = flash_read_data(Page_Address + i*4);
	}
}
//��FLASH 2k���ݴ��뻺��ȫ�ֱ���flash_temp��
void flash_read_page(uint32_t Page_Address)
{
	uint8_t i;
	for(i = 0;i < 128;i ++)
	{
		flash_temp[i] = flash_read_data((Page_Address + (i * 4)));
	}
	for(i = 0;i < 128;i ++)
	{
		flash_temp[i + 128] = flash_read_data((Page_Address + 512 + (i * 4)));
	}
	for(i = 0;i < 128;i ++)
	{
		flash_temp[i + 256] = flash_read_data((Page_Address + 1024 + (i * 4)));
	}
	for(i = 0;i < 128;i ++)
	{
		flash_temp[i + 384] = flash_read_data((Page_Address + 1536 + (i * 4)));
	}
}

uint16_t flash_read_data16(uint32_t Page_Address)
{

	return *(vu16 *)Page_Address;
	
}

/** ��ʽ��ƽ̨��ַ **/
void default_set()
{ 
   //����̩�ڲ����Ե�ַ
//		user_set.tcpip[0]=61;
//		user_set.tcpip[1]=155;
//		user_set.tcpip[2]=212;
//		user_set.tcpip[3]=44;	

//		user_set.tcpport[0]=80;
//		user_set.tcpport[1]=88;
	
//		user_set.tcpip[0]=221;
//		user_set.tcpip[1]=224;
//		user_set.tcpip[2]=99;
//		user_set.tcpip[3]=248;	

//		user_set.tcpport[0]=80;
//		user_set.tcpport[1]=88;
	
	user_set.tcpip[0]=180;
	user_set.tcpip[1]=114;
	user_set.tcpip[2]=167;
	user_set.tcpip[3]=141;	
	
	user_set.tcpport[0]=80;
	user_set.tcpport[1]=88;
//	//�ƶ�APN
//	user_set.netapnlen = strlen("CMNET");                                             
//	sprintf(user_set.netapn,"CMNET");
//	user_set.netuserlen = strlen("chen");
//	sprintf(user_set.netuser,"chen");
//	user_set.netkeylen = strlen("chen");
//	sprintf(user_set.netkey,"chen"); 
   //��ͨAPN
	user_set.netapnlen = strlen("UNIM2M.NJM2MAPN");
	sprintf(user_set.netapn,"UNIM2M.NJM2MAPN");
	user_set.netuserlen = strlen("wxxtkj");
	sprintf(user_set.netuser,"wxxtkj");
	user_set.netkeylen = strlen("wxxtkj");
	sprintf(user_set.netkey,"wxxtkj"); 
	
	user_set.flag=2;

	flash_writeset();
}

void read_flash2screen()
{
	uint8_t i;
	uint16_t *p;
	uint32_t *q;
	static uint16_t temp_data16;
	static uint32_t temp_data;
	static s_LORA_SEND_PACKAGE screen_send_package; 
	q = &(SCREEN_DATA.standard_ring[0]);
	p = &(SCREEN_SET.deep);
/*	screen_send_package.head1 = 0x5A;
	screen_send_package.head2 = 0xA5;
	
	for(i = 0;i < 16;i ++)
	{
		*(p+i) = flash_read_data16((SCREEN_SET_ADDRESS + i*2));
	}
	
	for(i = 0;i < 12;i ++)
	{
		*(q+i) = flash_read_data((STANDARD_MEASURE_ADDRESS + i*4));
	}
	
	for(i = 0;i < 14;i ++)
	{
		temp_data16 = *(&(SCREEN_SET.deep) + i);
		screen_send_package.num = 0x05;
		screen_send_package.cmd =0x82;		
		
		screen_send_package.addr[0] = 0x00;
		screen_send_package.addr[1] = 0x01 + i;
		screen_send_package.data[0] = (temp_data16 & 0xff00) >> 8;
		screen_send_package.data[1] = temp_data16 & 0xff;
		screen_send_package.data[2] = 0x00;
		screen_send_package.data[3] = 0x00;
		osMessagePut(lora_send_msg, (uint32_t)(&screen_send_package), 0);
		osDelay(200);
	}
	

		for(i = 0;i < 10; i ++)
		{

			temp_data = *(&(SCREEN_DATA.standard_ring[0]) + i);
			screen_send_package.num = 0x07;
			screen_send_package.cmd =0x82;
			screen_send_package.addr[0] = 0x00;
			screen_send_package.addr[1] = 0x10 + (i * 2);
			screen_send_package.data[0] = (temp_data & 0xff000000) >> 24;
			screen_send_package.data[1] = (temp_data & 0xff0000) >> 16;
			screen_send_package.data[2] = (temp_data & 0xff00) >> 8;
			screen_send_package.data[3] = temp_data & 0xff;
			osMessagePut(lora_send_msg, (uint32_t)(&screen_send_package), 0);
			osDelay(200);
		}
	////////////////////////////////////////��׼����	
			temp_data = *(&(SCREEN_DATA.standard_ring[0]) + 11);
			screen_send_package.num = 0x07;
			screen_send_package.cmd =0x82;
			screen_send_package.addr[0] = 0x00;
			screen_send_package.addr[1] = 0x10 + (10 * 2);
			screen_send_package.data[0] = (temp_data & 0xff000000) >> 24;
			screen_send_package.data[1] = (temp_data & 0xff0000) >> 16;
			screen_send_package.data[2] = (temp_data & 0xff00) >> 8;
			screen_send_package.data[3] = temp_data & 0xff;
			osMessagePut(lora_send_msg, (uint32_t)(&screen_send_package), 0);
			osDelay(200);
//////////////////////////////////////////////////////		
		screen_send_package.num = 0x05;
		screen_send_package.cmd =0x82;		
		
		screen_send_package.addr[0] = 0x00;        //��ǰ��ѹ
		screen_send_package.addr[1] = BATTERY_VOLTAGE;//0x80
		screen_send_package.data[0] = (voltage_temp & 0xff00) >> 8;
		screen_send_package.data[1] = voltage_temp & 0xff;
		screen_send_package.data[2] = 0x00;
		screen_send_package.data[3] = 0x00;
		osMessagePut(lora_send_msg, (uint32_t)(&screen_send_package), 0);
		osDelay(200);
		
		
		screen_send_package.addr[0] = 0x00;        //�汾��Ϣ v0.1
		screen_send_package.addr[1] = SOFTWARE_VERSION;//0x82
		screen_send_package.data[0] = (0x01 & 0xff00) >> 8;
		screen_send_package.data[1] = 0x01 & 0xff;
		screen_send_package.data[2] = 0x00;
		screen_send_package.data[3] = 0x00;
		osMessagePut(lora_send_msg, (uint32_t)(&screen_send_package), 0);
		
		osDelay(200);
			//��ʼ���ڷ�����Ļ��ʾ							
//////////////////////////////////////////////////////////////////////////////////////////////////			
	//first_date_from_server_save		
		if(gprs_cmd_req_data_start == 0x01)	//��������������ݿ�ʼ ���� ��ʾ
		{
			screen_send_package.head1 = 0x5A;
			screen_send_package.head2 = 0xA5;
			screen_send_package.num = 0x07;
			screen_send_package.cmd =0x82;
			screen_send_package.addr[0] = 0x00;
			screen_send_package.addr[1] = HISTORY_DATE_START;
			screen_send_package.data[3] = first_date_from_server_save & 0xff;
			screen_send_package.data[2] = (first_date_from_server_save & 0xff00) >> 8;
			screen_send_package.data[1] = (first_date_from_server_save & 0xff0000) >> 16;
			screen_send_package.data[0] = (first_date_from_server_save & 0xff000000) >> 24;
			osMessagePut(lora_send_msg, (uint32_t)(&screen_send_package), 0);
			osDelay(200);
			//////////////////////////////////////////////////////////////////////////////////////////////////	
			//�������ڷ�����Ļ��ʾ							
//////////////////////////////////////////////////////////////////////////////////////////////////									
			screen_send_package.head1 = 0x5A;
			screen_send_package.head2 = 0xA5;
			screen_send_package.num = 0x07;
			screen_send_package.cmd =0x82;
			screen_send_package.addr[0] = 0x00;
			screen_send_package.addr[1] = HISTORY_DATE_END;
			screen_send_package.data[3] = serv_back_req_date_end & 0xff;
			screen_send_package.data[2] = (serv_back_req_date_end & 0xff00) >> 8;
			screen_send_package.data[1] = (serv_back_req_date_end & 0xff0000) >> 16;
			screen_send_package.data[0] = (serv_back_req_date_end & 0xff000000) >> 24;
			osMessagePut(lora_send_msg, (uint32_t)(&screen_send_package), 0);

			
		}*/

    

	
}


void ipset_init()
{
	 if(*(__IO uint8_t*)(USERSET_ADDRESS+119)!=2) //дĬ������
	 {
     default_set(); 
	 }
}
void Array_chartoint(uint8_t* string,uint16_t * arry,uint8_t len)
{
	uint8_t i;
	for(i=0;i<len;i++)
	  arry[i] = string[i];
}


void module_default_set()
{
	MODULE_SET.flag = 1;
  MODULE_SET.DAC_val = 2500;
	MODULE_SET.RES_val = 20;
	flash_writedata_16(SCREEN_SET_ADDRESS, &(MODULE_SET.flag));

}
void screen_default_set()
{
	SCREEN_SET.deep = 100;
	SCREEN_SET.ring_num = 8;
	SCREEN_SET.measure_period = 1;
	SCREEN_SET.motor_speed = 1;
	SCREEN_SET.tcp_addr[0] = 117;
	SCREEN_SET.tcp_addr[1] = 84;
	SCREEN_SET.tcp_addr[2] = 30;
	SCREEN_SET.tcp_addr[3] = 216;
	SCREEN_SET.tcp_port = 8888;
	SCREEN_SET.ftp_addr[0] = 192;
	SCREEN_SET.ftp_addr[1] = 168;
	SCREEN_SET.ftp_addr[2] = 0;
	SCREEN_SET.ftp_addr[3] = 108;
	SCREEN_SET.ftp_port = 3344;
	SCREEN_SET.screen_set_flag = 2;
//	SCREEN_SET.standard_date_flag = 2;
//	SCREEN_SET.enter_standby_flag = 0;
//	SCREEN_SET.wake_up_time = 6;
	SCREEN_SET.domainlen = strlen("chang7e.eicp.net");
	Array_chartoint("chang7e.eicp.net",SCREEN_SET.domain,SCREEN_SET.domainlen);
	SCREEN_SET.netapnlen = strlen("UNIM2M.NJM2MAPN");
	Array_chartoint("UNIM2M.NJM2MAPN",SCREEN_SET.netapn,SCREEN_SET.netapnlen);
	SCREEN_SET.netuserlen = strlen("wxxtkj");
	Array_chartoint("wxxtkj",SCREEN_SET.netuser,SCREEN_SET.netuserlen);
	SCREEN_SET.netkeylen = strlen("wxxtkj");
	Array_chartoint("wxxtkj",SCREEN_SET.netkey,SCREEN_SET.netkeylen);
	
	SCREEN_SET.enable_dns_flag = 1; //����DNS����
	flash_writedata_16(SCREEN_SET_ADDRESS, &(SCREEN_SET.deep));

}

void standard_default_set()
{

	uint8_t i;
	
	SCREEN_DATA.standard_ring[0] = 1;
	SCREEN_DATA.standard_ring[1] = 2;
	SCREEN_DATA.standard_ring[2] = 3;
	SCREEN_DATA.standard_ring[3] = 4;
	SCREEN_DATA.standard_ring[4] = 5;
	SCREEN_DATA.standard_ring[5] = 6;
	SCREEN_DATA.standard_ring[6] = 7;
	SCREEN_DATA.standard_ring[7] = 8;
	SCREEN_DATA.standard_ring[8] = 9;
	SCREEN_DATA.standard_ring[9] = 10;
	SCREEN_DATA.battery_voltage_standard = 170725;//FLASH�����һ��д���ĵ�ַ ��ʾFLASH����Ŀ�� ������Ŀ��λ��Ϊ��ص�ѹ
	SCREEN_DATA.standard_date = 170725;
//	SCREEN_DATA.flash_write_num = 0;
	flash_writedata(STANDARD_MEASURE_ADDRESS, &(SCREEN_DATA.standard_ring[0]));

}
void lcm_default_set()
{

	LCM_SET.enable_flag = 2;//������Ļ����ʾ ��־λ
	LCM_SET.current_value = 400.0; //��ǰ����ֵ mm
	LCM_SET.Baudrate = 4096; //485�ڲ�����
	LCM_SET.endpoint_num = 1;//�豸վ��
	LCM_SET.Collection_interval = 50;//�ɼ���� ��
	LCM_SET.max_value = 500; //��λ mm
	
	flash_writedata(LCM_SET_ADDRESS, &(LCM_SET.enable_flag));

}

//void screenset_init()
//{
////	if(*(__IO uint16_t*)(SCREEN_SET_ADDRESS+30)!=2)
//	if(flash_read_data16(SCREEN_SET_ADDRESS + 28) != 2)
//	{
//		screen_default_set();
//		standard_default_set();
//	}

//}

void module_set_init()
{
//	if(*(__IO uint16_t*)(SCREEN_SET_ADDRESS+30)!=2)
	if(flash_read_data16(SCREEN_SET_ADDRESS) != 1)
	{
		module_default_set();
	}

}

void lcmset_init()
{
//	if(*(__IO uint16_t*)(SCREEN_SET_ADDRESS+30)!=2)
	if(flash_read_data(LCM_SET_ADDRESS) != 2)
	{
		lcm_default_set();
	}
}


static void LED_init()
{
	GPIO_PortClock(GPIOA, true);
	#if PCB_VERSION==1
	 GPIO_PinConfigure(GPIOA, 6, GPIO_OUT_PUSH_PULL, GPIO_MODE_OUT10MHZ);
	#elif PCB_VERSION==2
	 GPIO_PinConfigure(GPIOA, 2, GPIO_OUT_PUSH_PULL, GPIO_MODE_OUT10MHZ);
	#endif
	LED_OFF;
}

//void sim900_power_init()
//{
//	GPIO_PortClock(GPIOB, true);
//	GPIO_PinConfigure(GPIOB, 0, GPIO_OUT_PUSH_PULL, GPIO_MODE_OUT10MHZ);
//	GPIO_PortClock(GPIOC, true);
//	GPIO_PinConfigure(GPIOC, 5, GPIO_IN_FLOATING , GPIO_MODE_INPUT);
//	do
//	{
////		GPIO_PinWrite(GPIOB, 0, 0);											//ͨ������������
////		delay_ms(3000);
//		GPIO_PinWrite(GPIOB, 0, 1);											//ͨ������������
//		osDelay(3000);
//		GPIO_PinWrite(GPIOB, 0, 0);											//ͨ���������ͷ�
//		osDelay(4000);
//	}while(GPIO_PinRead(GPIOC,5)!=(uint32_t)0x01);  //�ɹ�����status��ƽΪ��	
//}
void sim900_power_init()
{
	
	GPIO_PortClock(GPIOC, true);

	GPIO_PinConfigure(GPIOC, 6, GPIO_IN_FLOATING , GPIO_MODE_INPUT);

	GPIO_PinConfigure(GPIOC, 9, GPIO_OUT_PUSH_PULL, GPIO_MODE_OUT10MHZ);	

	do{
		GPIO_PinWrite(GPIOC, 9, 1);											//ͨ������������
		osDelay(3000);
	//	IWDG_ReloadCounter();				//ι��
		GPIO_PinWrite(GPIOC, 9, 0);											//ͨ���������ͷ�
		osDelay(4000);
	}while((GPIO_PinRead(GPIOC,6))!=(uint32_t)0x01);  //�ɹ�����status��ƽΪ��	
	osDelay(100);

}

/***************************************************************************
 *  ��������BSP_IWDG_Init
 *  ˵  ������ʼ��iWDG���Ź���ʱ��
 *  ��  ����
 ***************************************************************************/
void BSP_IWDG_Init(uint16_t Dog_time)
{
	RCC_LSICmd(ENABLE);
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);			//�����Ź��Ĵ���д�빦��
	IWDG_SetPrescaler(IWDG_Prescaler_256);						//��ʱƵ�ʣ�40K/((4*2^prer)*rlr)����ʱ���ڣ�((4*2^prer)*rlr)/40 ms
	IWDG_SetReload(Dog_time);															//���ʱ��Ϊ20s	,������������0xfff	
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Disable);
//	IWDG_ReloadCounter();															//ι��
	IWDG_Enable();																		//ʹ�ܿ��Ź�
}
void  Adc_Init1(void)
{ 	
	ADC_InitTypeDef ADC_InitStructure; 
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA |RCC_APB2Periph_ADC1	, ENABLE );	  //ʹ��ADC1ͨ��ʱ��
 

	RCC_ADCCLKConfig(RCC_PCLK2_Div6);   //����ADC��Ƶ����6 72M/6=12,ADC���ʱ�䲻�ܳ���14M

	//PA1 ��Ϊģ��ͨ����������                         
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		//ģ����������
	GPIO_Init(GPIOA, &GPIO_InitStructure);	

	ADC_DeInit(ADC1);  //��λADC1,������ ADC1 ��ȫ���Ĵ�������Ϊȱʡֵ

	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	//ADC����ģʽ:ADC1��ADC2�����ڶ���ģʽ
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;	//ģ��ת�������ڵ�ͨ��ģʽ
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;	//ģ��ת�������ڵ���ת��ģʽ
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//ת��������������ⲿ��������
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	//ADC�����Ҷ���
	ADC_InitStructure.ADC_NbrOfChannel = 1;	//˳����й���ת����ADCͨ������Ŀ
	ADC_Init(ADC1, &ADC_InitStructure);	//����ADC_InitStruct��ָ���Ĳ�����ʼ������ADCx�ļĴ���   

  
	ADC_Cmd(ADC1, ENABLE);	//ʹ��ָ����ADC1
	
	ADC_ResetCalibration(ADC1);	//ʹ�ܸ�λУ׼  
	 
	while(ADC_GetResetCalibrationStatus(ADC1));	//�ȴ���λУ׼����
	
	ADC_StartCalibration(ADC1);	 //����ADУ׼
 
	while(ADC_GetCalibrationStatus(ADC1));	 //�ȴ�У׼����
 
//	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//ʹ��ָ����ADC1�����ת����������

}		

u16 Get_Adc1(u8 ch)   
{
  	//����ָ��ADC�Ĺ�����ͨ����һ�����У�����ʱ��
	ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_239Cycles5 );	//ADC1,ADCͨ��,����ʱ��Ϊ239.5����	  			    
  
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//ʹ��ָ����ADC1�����ת����������	
	 
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));//�ȴ�ת������

	return ADC_GetConversionValue(ADC1);	//�������һ��ADC1�������ת�����
}

u16 Get_Adc_Average1(u8 ch,u8 times)
{
	u32 temp_val=0;
	u8 t;
	for(t=0;t<times;t++)
	{
		temp_val+=Get_Adc1(ch);
		osDelay(5);
	}
	return temp_val/times;
} 	

/*void DMA1_Channel1_IRQHandler(void)
{


    if(DMA_GetFlagStatus(DMA1_FLAG_TC1))
    {
        DMA_ClearITPendingBit(DMA1_FLAG_TC1);
   //     memcpy(WriteBuff+ADC_BUFF_LEN/2, ADC_ConvertedValue+ADC_BUFF_LEN/2, ADC_BUFF_LEN*sizeof(uint16_t)/2);

			DMA_FLAG = 1;
    }

}*/

void all_init()
{
	u32 i=0;
//	u8 temp=0;
	u8 sval=0; 
	u16 adcx;
	
	FLASH_Unlock();//FLASH����
	FLASH_ClearFlag(FLASH_FLAG_PGERR|FLASH_FLAG_WRPRTERR|FLASH_FLAG_EOP);//�����־λ
	
	module_set_init();
	read_module_userset();//��ȡ��Ļ����   0X0807D000
	//screenset_init();
	
	//read_screen_userset();//��ȡ��Ļ����
	
	//lcmset_init();
	
	//read_lcm_userset();//��ȡҺ������������   0X0807D000
	
  /*LCM_SET.enable_flag = 2;//������Ļ����ʾ ��־λ
	LCM_SET.current_value = 400.0; //��ǰ����ֵ mm
	LCM_SET.Baudrate = 4096; //485�ڲ�����
	LCM_SET.endpoint_num = 1;//�豸վ��
	LCM_SET.Collection_interval = 50;//�ɼ���� ��
	LCM_SET.max_value = 500; //��λ mm
	*/
	
  BSP_IWDG_Init(0xfff);						//��ʼ����ʹ�ܿ���	��̨����ι��
	
	
	////BSP_IWDG_Init(625);						//��ʼ����ʹ�ܿ��Ź�	 1s��
	IO_init();
	
 // Adc_Init1();
	
	Dac1_Init();
	Dac1_Set_Vol(MODULE_SET.DAC_val);
	
	//adcx=Get_Adc_Average1(ADC_Channel_1,10);		//�õ�ADCת��ֵ	  
	//temp1=(float)adcx*(3.0/4096);			//�õ�ADC��ѹֵ
	
	EXTI_init();//0 1   0 3
	
  Adc_GPIO_Init();
	Adc_single_Init();
	ADC1_DMA_Init();//1 0
	TIM2_Configration();

//	Adc1_Multi_Init();
//	Adc2_Multi_Init();  



   // ADC12_Init();
	//	TIM2_Configration();
			
//		TIM3_Int_Init(4999,7199);//10Khz�ļ���Ƶ�ʣ�������5000Ϊ500ms  
    TIM3_Int_Init(99,71);//1Mhz�ļ���Ƶ�ʣ�������100Ϊ100us   0 2
		
		MCP4017_Init();
		MCP4017_WriteOneByte(MODULE_SET.RES_val);
		sval =  MCP4017_ReadOneByte();
		
		Usart2_Init(460800);//0 0
	//while(1);

	/*FSMC_SRAM_Init();
	
	for(i=0;i<1024*1024;i++)
	{
	FSMC_SRAM_WriteBuffer(&temp,i,1);
	temp++;
	//	temp = 8;
	}

	for(i=0;i<1024;i++)
	{
	FSMC_SRAM_ReadBuffer(&temp,i,1);
//	if(i==0)sval=temp;
//	else if(temp<=sval)break;
  osDelay(1);
	}*/
	

	
	/*for(i=0;i<1024*1024;i+=4096)
	{
	FSMC_SRAM_WriteBuffer(&temp,i,1);
	temp++;
	//	temp = 8;
	}

	for(i=0;i<1024*1024;i+=4096)
	{
	FSMC_SRAM_ReadBuffer(&temp,i,1);
//	if(i==0)sval=temp;
//	else if(temp<=sval)break;
  osDelay(1);
	}*/

	
	
//	EXTI_init();
//	TIM3_PWM_Init(18000,0);//9600
	
	//GPIO_PinWrite(GPIOD, 2, 1);//����������
	//TIM_SetCompare1(TIM3,8000);//2000

	//sim900_power_init();			//����				����SIM��Ϊ�˻�ȡʱ��� ����������		
//	id1 = osTimerCreate (osTimer(Timer),osTimerPeriodic,NULL);
//	osTimerStart(id1,500); //������ʱ��
//	
      //	RF_Lora_Dev_init();  //lora���� ����2��ʼ��

//	
//  Dev_485_init();//485�豸 ����1��ʼ��
// 
//  RS_232_Dev_init();

	
}
//void per_init()
//{
//	LED_init();

//	sim900_power_init();
//  //����5S��ʱ��
//	Timer_1s = osTimerCreate (osTimer(Timer),osTimerPeriodic,NULL);
//	osTimerStart(Timer_1s,1000); //������ʱ��
//	
//	BreathDev_init();
//	Dev_485_init();
//		
//	FLASH_Unlock();//FLASH����
//	FLASH_ClearFlag(FLASH_FLAG_PGERR|FLASH_FLAG_WRPRTERR|FLASH_FLAG_EOP);//�����־λ
//	
//	ipset_init();
//	read_userset();
//}
//�����Ƚ�SN���Ƿ���ȷ
uint8_t Compare_SN(uint8_t arry1[],uint8_t arry2[],uint8_t num)
{
	uint8_t i,temp;
	temp=1;
	for(i=0;i<num;i++)
	{
		if(arry1[i]!=arry2[i])
		{
			temp=0;
			break;
		}
	}
	 return temp;
}

void device_init(s_GATEWAY_DEVICE *gateway_dev)
{	
	//sim900_dev->cpap_status.recv_status = CPAP_RECV_STATUS_NO;
//	sim900_dev->cpap_status.link_status = CPAP_LINK_STATUS_NO; //�Ƿ�ɹ��Ӻ�������ȡ����ȷ��SN  
	
	gateway_dev->serv_status.recv_status = SERV_RECV_STATUS_NULL;//SERV_RECV_STATUS_NULL
	gateway_dev->serv_status.init_status = SERV_INIT_STATUS_NET_STOP;//SERV_INIT_STATUS_NET_NO //�����ɹ��������ӷ�����
//	sim900_dev->serv_status.data_status = SERV_DATA_STATUS_NULL;	
//	sim900_dev->serv_status.tcp_status = SERV_TCP_STATUS_NO;//(����ʹ��)//SERV_TCP_STATUS_CLOSED;//��ʾ�ϵ粻��������TCP�����յ�����������ָ��
	
	//sim900_dev->serv_idle = SERV_IDLE_STAUS_NO;
	//sim900_dev->work_mode = SIM900_MODE_GPRS;
	gateway_dev->serv_link = SERV_LINK_STATUS_NO;
	memcpy(gateway_dev->data_info.soft_version,"v1.3.1.1",sizeof("v1.3.1.1")); 
	memcpy(gateway_dev->data_info.hard_version,"v0.6",sizeof("v0.6")); 
	memcpy(gateway_dev->data_info.sn, "abcdefghijk", sizeof("abcdefghijk"));
	
}


