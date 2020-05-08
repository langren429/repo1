#include "common.h"
#include "exti.h"
//#include "motor.h"
#include "encode.h"


osThreadId	copy_task_id;
uint16_t test_count;
volatile uint8_t electric_flag = 0;//�ŵ��жϱ�־
volatile uint8_t electric_flag_temp[4];//

uint8_t test_dir = 1;//1 ���� 0����
s32 encoder_count1[10];
s32 encoder_count0[10];
s32 encoder_count_ave[10];
uint8_t test_count_temp;
extern ARM_DRIVER_UART *Dev_485;
extern s_SCREEN_SET SCREEN_SET;
extern osMessageQId(tcp_send_msg);
extern uint8_t pcf8563_time[6];
//extern ARM_DRIVER_UART *RF_Lora_Dev;
extern e_SCREEN_STATE SCREEN_STATE;
extern s_SCREEN_DATA SCREEN_DATA;
extern uint32_t flash_temp[512];//2048/4
extern e_SETTLE_WORK_STATE SETTLE_WORK_STATE;
extern uint16_t sleep_time_num;
extern uint8_t gprs_cmd_req_data_start; //�������������ݿ�ʼ
uint16_t send_screen_again;
uint8_t rtc_wakeup_set;
uint8_t program_runing = 1;//�������������б�־λ
extern uint16_t sleep_time_num_xxx;
extern uint16_t PA0_wake_up;
e_DATA_TODAY_SEND_STATE data_today_need_send = DATA_TODAY_NO; //0 �����ݷ��� 1 �����ݷ��� 2 ���ڷ���
extern	s_GATEWAY_DEVICE gateway_dev;
extern uint32_t voltage_temp;
extern uint8_t measure_working;
extern uint8_t screen_state;//��Ļ�� 1 �ر� 0״̬
extern uint8_t measure_start_prepare_flag;
extern uint8_t measure_start_prepare_count;
extern uint8_t enter_stopmode_flag;//Ϣ������¼��� �Ƿ����͹��� ��־
extern uint8_t rtc_init_success;
uint8_t open_screen_flag; //�� 1
uint8_t WWDG_CNT=0x7f;
uint8_t rtc_wakeup_time;
uint8_t req_data_from_serv_continue;
e_KEY_TYPE KEY_TYPE;
volatile TIME_RECORD Temp_time_record;
TIME_RECORD time_record0;
TIME_RECORD time_record1;
volatile TIME_RECORD Time_current;

volatile uint32_t xx_Temp_time_record;
volatile uint32_t xx_Time_current;
volatile uint32_t Heart_beat_time;
volatile uint32_t xx_time_record0;
volatile uint32_t DMA1_Channel1_cnt;

volatile uint8_t ram_flag = 0;
extern __IO uint16_t ADCConvertedValue[60000];
volatile uint16_t ADCValueTemp0[60000];
extern s_MODULE_SET MODULE_SET;


volatile uint8_t DMA_FLAG = 0;
volatile uint8_t SI_EVENT_FLAG = 0;

uint8_t Tcp_send_Data[1200];
uint8_t time_2_zero;
volatile uint8_t heart_beat_flag = 0;
uint8_t Tcp_Link_test[20];
volatile uint8_t TCP_Link_OK = 0;
uint16_t TCP_Link_cnt = 0;//���Ӵ���
volatile uint8_t Sending = 0;//���ڷ���

void delay_ms(u16 time)
{    
   u16 i=0;  
   while(time--)
   {
      i=12000;  //�Լ�����
      while(i--) ;    
   }
}

//void DMA1CH7_CopyMem(u32 src,u32 des,u32 len,u32 datasize)
//{
//  DMA1->IFCR=DMA1_IT_GL7|DMA1_IT_TC7|DMA1_IT_HT7|DMA1_IT_TE7;
//  DMA1_Channel7->CPAR=src;
//  DMA1_Channel7->CMAR=des;
//  DMA1_Channel7->CNDTR=len;
//  if(datasize==0)     //��8bit������
//  {
//    DMA1_Channel7->CCR=DMA_M2M_Enable|DMA_Priority_Low|DMA_MemoryDataSize_Byte|DMA_PeripheralDataSize_Byte
//                       |DMA_MemoryInc_Enable|DMA_PeripheralInc_Enable|DMA_Mode_Normal|DMA_DIR_PeripheralSRC
//                       |0x01;
//  }
//  else if(datasize==1)  //��16bit������
//  {
//    DMA1_Channel7->CCR=DMA_M2M_Enable|DMA_Priority_Low|DMA_MemoryDataSize_Byte|DMA_PeripheralDataSize_Byte
//                       |DMA_MemoryInc_Enable|DMA_PeripheralInc_Enable|DMA_Mode_Normal|DMA_DIR_PeripheralSRC
//                       |0x01;
//  }
//  else if(datasize==2)  //��32bit������
//  {
//    DMA1_Channel7->CCR=DMA_M2M_Enable|DMA_Priority_Low|DMA_MemoryDataSize_Byte|DMA_PeripheralDataSize_Byte
//                       |DMA_MemoryInc_Enable|DMA_PeripheralInc_Enable|DMA_Mode_Normal|DMA_DIR_PeripheralSRC
//                       |0x01;
//  }
//  else
//  {
//    DMA1_Channel7->CCR=0x00;//����Ҫ�����ߴ�����???
//    return;
//  }
//  while(!(DMA1->ISR&(DMA1_FLAG_TC7|DMA1_FLAG_TE7))){};
//  DMA1_Channel7->CCR=0x00;    //��һ��ֻ�����ENλ
//  DMA1_Channel7->CCR=0x00;    //�ڶ��β���ȫ�����Ϊ0
//  DMA1->IFCR=DMA1_IT_GL7|DMA1_IT_TC7|DMA1_IT_HT7|DMA1_IT_TE7;//�����־,���Բ�Ҫ��,�´��ò����
//}

void Sys_Standby(void)
{
	//RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE); //ʹ��PWR ����ʱ��
	//PWR_WakeUpPinCmd(ENABLE); //ʹ�ܻ��ѹܽŹ���
	PWR_EnterSTANDBYMode(); //����(STANDBY)ģʽ
	//PWR_EnterSTOPMode(PWR_Regulator_ON, PWR_STOPEntry_WFI);//STOPģʽ
}

void Sys_Enter_Standby(void)
{

  //RCC_APB2PeriphResetCmd(0X01FC,DISABLE); //��λ����IO 
  //SystemInit();
//	EXTI_ClearITPendingBit(EXTI_Line0 | EXTI_Line1 | EXTI_Line4 | EXTI_Line10 | EXTI_Line11);     // ?? EXIT ????
  Sys_Standby();
}

//TCPУ���룬���뷽ʽ��CRC16  ������������CRC
unsigned short cal_serv_crc_N(unsigned char *message, unsigned int len,unsigned short crc_reg)
{	
	int i, j;
	unsigned short current;
		 
	for (i = 0; i < len; i++)
	{
		current = message[i] << 8;
		for (j = 0; j < 8; j++)
		{
			if ((short)(crc_reg ^ current) < 0)
					crc_reg = (crc_reg << 1) ^ 0x1021;
			else
					crc_reg <<= 1;
			current <<= 1;           
		}
	}
	return crc_reg;
}

//���������ַ�ת��,ͬʱ��ȡ  //��װ֮���͸���ƽ̨������ת��
uint16_t Send_msg_Translate (uint8_t *tmsg,uint16_t len)	 //*tmsgת��ǰ(1k)  *atmsgת���(2k)
{
	uint16_t i=0,tran_len=0,j=0;
	uint8_t temp[1200],flag=0;
//	temp=(uint8_t*)malloc(800);
	if(len>sizeof(temp))
	{
		return 0;
	}
	memcpy(temp,tmsg, len);
//	memset(tmsg, 0, len);
	while(i<len)
	{    
		if((temp[i]==0x7c)|(temp[i]==0x7d)|(temp[i]==0x7e))
		{ tmsg[tran_len++]=0x7c;
			tmsg[tran_len]=(temp[i]&0x0f)|0x50;
		}
		
		else 
		{
			tmsg[tran_len]=temp[i];
		}
		i++;
		tran_len++;
	}
//  free(temp);	
  return tran_len;      //ת���ĳ���
}

//���������ַ�ת��,ͬʱ��ȡ ���յ���ƽ̨����ת��
uint16_t Recv_msg_Translate (uint8_t *atmsg,uint16_t len)	 //*tmsgת��ǰ(1k)  *atmsgת���(2k)
{
	uint16_t i=0,j=0,k;
	uint8_t temp[150];
//	temp=(uint8_t*)malloc(len);
	if(len>sizeof(temp))
	{
		return 0;
	}
	memcpy(temp,atmsg,len);
	while(i<len){
	    if(temp[i]==0x7c){ 
						atmsg[j]=(temp[i]&0xf0)|(temp[i+1]&0x0f);
				    i++;
				}
       else if(temp[i]==0x7D){
				 atmsg[j]=temp[i];
				 k=j;
				 break;
			 }		 
			 else{ 
						atmsg[j]=temp[i];
				}		 
		i++;
		j++;
	}
	while(j<len)
	{
	 j++;
	 atmsg[j]=0x00;		
	}
  return k;      //ת���ĳ���
}

void IO_init(void)
{
	
	GPIO_PortClock(GPIOC, true);	
	GPIO_PinConfigure(GPIOC, 14, GPIO_OUT_PUSH_PULL , GPIO_MODE_OUT50MHZ);
	GPIO_PinConfigure(GPIOC, 15, GPIO_OUT_PUSH_PULL , GPIO_MODE_OUT50MHZ);
	
	LED1_OFF;
//	LED1_ON;
	LED2_OFF;
//	LED2_ON;
		GPIO_PortClock(GPIOB, true);
  GPIO_PinConfigure(GPIOB, 1, GPIO_OUT_PUSH_PULL , GPIO_MODE_OUT50MHZ);
  GPIO_PinWrite(GPIOB, 1, 1);	//LED_TX ����Ч
	
	GPIO_PortClock(GPIOA, true);
  GPIO_PinConfigure(GPIOA, 7, GPIO_OUT_PUSH_PULL , GPIO_MODE_OUT50MHZ);
	RST_OFF;


}
void electric_task(void const *argument)
{
	uint8_t i;
	
	while(1)
	{
		if(electric_flag == 1)
		{
			osDelay(5);//5ms������
			electric_flag = 0;
			for(i=0;i<3;i++)
			{
				LED1_ON;
				osDelay(300);
				LED1_OFF;
				osDelay(300);
			}
			
		}
		osDelay(200);
	}
}
 void cycle_send()
{
	static uint16_t total_packet,last_packet_size;
	uint32_t k,sum,num,i;
	uint32_t average;
	static uint8_t BIG_NUM = 0;
	static s_TCP_ASK_CMD_PACKAGE tcp_ask_cmd_package;
	uint16_t crc_data,Translate_size;//ת���ĳ���
	s_TCP_SEND_DATA_PACKAGE TCP_SEND_DATA_PACKAGE;
	
	
	BIG_NUM ++;//ÿ�η��ʹ����ż�һ
	
		if(120000%ONE_PACK_NUM == 0)
	{
		total_packet = 120000/ONE_PACK_NUM;
		last_packet_size = 0;
	}
	else
	{
		total_packet = 120000/ONE_PACK_NUM + 1;
		last_packet_size = 120000%ONE_PACK_NUM;
	}
	
	////////////////////////ƽ��ֵ����//////////////////////////////						
		sum = 0;
		num = 0;
		for(k = 0;k < 60000;k ++)
		{
			if(ADCValueTemp0[k] >  MODULE_SET.DAC_val)
			{
				sum += ADCValueTemp0[k];
				num ++;
			}
			
		}
		average = sum/num;
		////////////////////////��������//////////////////////////////							
		for(i = 1;i <= total_packet;i ++)
			{
				if(i == total_packet)//���һ��ĩβ ���ź�ǿ��4���ֽ� 1���ֽڵ绡״̬
				{
						TCP_SEND_DATA_PACKAGE.head = 0x7E;//ͷ1
						TCP_SEND_DATA_PACKAGE.state = 0x00;//״̬��1
						TCP_SEND_DATA_PACKAGE.module_id = MODULE_ID;//ģ�鵥Ԫ��2
						TCP_SEND_DATA_PACKAGE.big_num = BIG_NUM;//������2
						TCP_SEND_DATA_PACKAGE.total_packet_num = total_packet;//�ܰ���2
						TCP_SEND_DATA_PACKAGE.current_packet_num = i;//��ǰ�����2
						TCP_SEND_DATA_PACKAGE.data_num = ONE_PACK_NUM + 12;//��Ϣ�峤��2
						memcpy(Tcp_send_Data,(void*)(&TCP_SEND_DATA_PACKAGE.head) , 12);
					
						memcpy((Tcp_send_Data+12), (void *)(&ADCValueTemp0[(i-1)*(ONE_PACK_NUM/2)]), ONE_PACK_NUM);
						memcpy((Tcp_send_Data+12+ONE_PACK_NUM), (void *)(&average), 4);
						memcpy((Tcp_send_Data+16+ONE_PACK_NUM), (void *)(&xx_time_record0), 4); 
						memcpy((Tcp_send_Data+20+ONE_PACK_NUM), (void *)(&DMA1_Channel1_cnt), 4);//DMA1_Channel1_cnt ��������
							
						crc_data=cal_serv_crc_N((Tcp_send_Data+1), (TCP_SEND_DATA_PACKAGE.data_num+11),0x00);
						Tcp_send_Data[TCP_SEND_DATA_PACKAGE.data_num+13] = (crc_data&0x00ff);
						Tcp_send_Data[TCP_SEND_DATA_PACKAGE.data_num+12] = (crc_data&0xff00)>>8;
						//��������֮��ת���ַ� 
						Translate_size = Send_msg_Translate (Tcp_send_Data+1,TCP_SEND_DATA_PACKAGE.data_num+13);
						Tcp_send_Data[Translate_size+1] = 0x7d;
					//	RF_Lora_Dev->WriteData(Tcp_Link_test, Translate_size + 2);
						Usart_SendString(USART2,Tcp_send_Data, Translate_size + 2);

						

				}
				else
				{
						TCP_SEND_DATA_PACKAGE.head = 0x7E;//ͷ1
						TCP_SEND_DATA_PACKAGE.state = 0x00;//״̬��1
						TCP_SEND_DATA_PACKAGE.module_id = MODULE_ID;//ģ�鵥Ԫ��2
						TCP_SEND_DATA_PACKAGE.big_num = BIG_NUM;//������2
						TCP_SEND_DATA_PACKAGE.total_packet_num = total_packet;//�ܰ���2
						TCP_SEND_DATA_PACKAGE.current_packet_num = i;//��ǰ�����2
						TCP_SEND_DATA_PACKAGE.data_num = ONE_PACK_NUM;//��Ϣ�峤��2
						memcpy(Tcp_send_Data,(void*)(&TCP_SEND_DATA_PACKAGE.head) , 12);
					
						memcpy((Tcp_send_Data+12), (void *)(&ADCValueTemp0[(i-1)*(ONE_PACK_NUM/2)]), ONE_PACK_NUM);
						
						crc_data=cal_serv_crc_N((Tcp_send_Data+1), (TCP_SEND_DATA_PACKAGE.data_num+11),0x00);
						Tcp_send_Data[TCP_SEND_DATA_PACKAGE.data_num+13] = (crc_data&0x00ff);
						Tcp_send_Data[TCP_SEND_DATA_PACKAGE.data_num+12] = (crc_data&0xff00)>>8;
						//��������֮��ת���ַ�  
						 Translate_size = Send_msg_Translate (Tcp_send_Data+1,TCP_SEND_DATA_PACKAGE.data_num+13);
						 Tcp_send_Data[Translate_size+1] = 0x7d;
					 //  RF_Lora_Dev->WriteData(Tcp_send_Data, Translate_size + 2);
						Usart_SendString(USART2,Tcp_send_Data, Translate_size + 2);
							
				}

			}
		
}
void TCP_test()
{
	static s_TCP_ASK_CMD_PACKAGE tcp_ask_cmd_package;
	uint16_t crc_data,Translate_size;//ת���ĳ���
/////////////////////////////////////��������//////////////////////
	
		tcp_ask_cmd_package.head = 0x7E;
		tcp_ask_cmd_package.state = 0x03;
		tcp_ask_cmd_package.module_id = MODULE_ID;
		
		memcpy(Tcp_Link_test,(void*)(&tcp_ask_cmd_package.head) , 4);
		crc_data=cal_serv_crc_N((Tcp_Link_test+1), 3,0x00);
		Tcp_Link_test[5] = (crc_data&0x00ff);
		Tcp_Link_test[4] = (crc_data&0xff00)>>8;
		//��������֮��ת���ַ�  
		Translate_size = Send_msg_Translate (Tcp_Link_test+1,5);
		Tcp_Link_test[Translate_size+1] = 0x7d;
		Usart_SendString(USART2,Tcp_Link_test, Translate_size + 2);	
	 // RF_Lora_Dev->WriteData(Tcp_Link_test, Translate_size + 2);
	//	osDelay(100);
}
void tcp_send_task(void const *argument){
	
  osEvent				beep_event;
	uint8_t i;
	uint32_t average;
	uint32_t k,sum,num,power;
	static uint32_t time_now;
  static uint16_t total_packet,last_packet_size;
	static s_TCP_ASK_CMD_PACKAGE tcp_ask_cmd_package;
	uint16_t crc_data,Translate_size;//ת���ĳ���
	//uint8_t *Tcp_send_Data;
  s_TCP_SEND_DATA_PACKAGE TCP_SEND_DATA_PACKAGE;
//	beep_task_id = osThreadGetId();
//	while(beep_task_id == NULL);
	//��������Ҫ������������û��Ҫ��ʱ�ȴ�����������״̬
//	osSignalClear(beep_task_id, 0x02);
  
	while(1)
	{
		
		if(time_2_zero == 1)
		{
			  LED1_ON;
			  osDelay(50);
			  time_2_zero = 0;
			  LED1_OFF;
		}
		
		if(heart_beat_flag == 1)
		{
			heart_beat_flag = 0;
			tcp_ask_cmd_package.head = 0x7E;
			tcp_ask_cmd_package.state = 0x05;
			tcp_ask_cmd_package.module_id = MODULE_ID;
			
			memcpy(Tcp_Link_test,(void*)(&tcp_ask_cmd_package.head) , 4);
			crc_data=cal_serv_crc_N((Tcp_Link_test+1), 3,0x00);
			Tcp_Link_test[5] = (crc_data&0x00ff);
			Tcp_Link_test[4] = (crc_data&0xff00)>>8;
			//��������֮��ת���ַ�  
			Translate_size = Send_msg_Translate (Tcp_Link_test+1,5);
			Tcp_Link_test[Translate_size+1] = 0x7d;
			Usart_SendString(USART2,Tcp_Link_test, Translate_size + 2);
		}
				 

		if(ram_flag == 1)
		{
				LED2_ON;

				if(TCP_Link_OK == 0)
				{
					TCP_test();					
				}
				
				for(i = 0;i < 50;i ++)
				{
					if(TCP_Link_OK == 1)
					{
						break;
					}
					
					osDelay(20);  
				}
					
				if(TCP_Link_OK == 1)
				{
						Sending = 1;//��ʾ���ڷ���
						TCP_Link_OK = 0;//��־����
						
						cycle_send();	
						
						Sending = 0;//��ʾ�������
				}
				
					LED2_OFF;
		}
     osDelay(1000);
	
	}
}

void copy_task(void const *argument){
	
  osEvent				copy_event;
	uint16_t i;
	uint32_t k;
	uint16_t crc_data,Translate_size;//ת���ĳ���
	static uint32_t time_now;
	static uint16_t total_packet,last_packet_size;
	static s_TCP_ASK_CMD_PACKAGE tcp_ask_cmd_package;
	//uint8_t *Tcp_send_Data;
 // s_TCP_SEND_DATA_PACKAGE TCP_SEND_DATA_PACKAGE;
	copy_task_id = osThreadGetId();
	while(copy_task_id == NULL);
	//��������Ҫ������������û��Ҫ��ʱ�ȴ�����������״̬
	osSignalClear(copy_task_id, 0x05);
  
	while(1){
		
		copy_event = osSignalWait(0x05, osWaitForever);
		if(copy_event.status == osEventSignal)
			{
				if(ram_flag == 0)//���ڷ���״̬ �˴δ�������Ч
         {
					LED1_ON;					

					 xx_time_record0 = xx_Temp_time_record;//��¼ʱ��

					 
						if(SI_EVENT_FLAG == 1)
						{

								if(ram_flag == 0)
								{
									memcpy((void *)ADCValueTemp0,(void *)ADCConvertedValue,120000);
									ram_flag = 1;
								}

						}
						
						SI_EVENT_FLAG = 0; 
						LED1_OFF;
					}
			}
		}
	}

/*
void WWDG_NVIC_Init()
{
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = WWDG_IRQn; //WWDG 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3; //??2 ????3 ?2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3; //??2,????3,?2
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure); //NVIC ???
}

//void WWDG_Init(u8 tr,u8 wr,u32 fprer)
void WWDG_Init(uint8_t tr,uint8_t wr,uint8_t fprer)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_WWDG, ENABLE); // WWDG ʱ��ʹ��
	WWDG_CNT=tr&WWDG_CNT; //��ʼ��WWDG_CNT.
	WWDG_SetPrescaler(fprer); //����IWDGԤ��Ƶֵ
	WWDG_SetWindowValue(wr); //���ô���ֵ
	WWDG_Enable(WWDG_CNT); //ʹ�ܿ��Ź� ����counter
	WWDG_ClearFlag(); //�����ǰ�����жϱ�־λ
	WWDG_NVIC_Init(); //NVIC
	WWDG_EnableIT(); //�������ڿ��Ź��ж�
}

//����WWDG ��������ֵ
void WWDG_Set_Counter(u8 cnt)
{
	WWDG_Enable(cnt); //ʹ�ܿ��Ź� ����counter .
}
*/


void EXTI_init(void){
	
   EXTI_InitTypeDef EXTI_InitStructure;
 	 NVIC_InitTypeDef NVIC_InitStructure;
   GPIO_InitTypeDef GPIO_InitStructure;
  //  C4 RS ; C5 TS1; B0 TS2; A0 F_KEY1(WAKEUP)
	// 	B1 RS ; B11 TS1; B10 TS2; A0 F_KEY1(WAKEUP)
 
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC,ENABLE);//ʹ��PORTB,PORTA,PORTCʱ��
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);	//ʹ�ܸ��ù���ʱ��

		
		GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0;//A0
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; //���ó���������
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOC, &GPIO_InitStructure);//��ʼ��GPIOA0
	
	
		GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_4;//A0
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //���ó���������
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOC, &GPIO_InitStructure);//��ʼ��GPIOA0


  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC,GPIO_PinSource0);
  	EXTI_InitStructure.EXTI_Line=EXTI_Line0;
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
		EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_Init(&EXTI_InitStructure);	  	//C0 Button_Down�������ж�
		
		
		GPIO_EXTILineConfig(GPIO_PortSourceGPIOC,GPIO_PinSource4);
  	EXTI_InitStructure.EXTI_Line=EXTI_Line4;
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;//�ĳ��½��ػ�������
		EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_Init(&EXTI_InitStructure);	  	//C4 �½����ж�
		
	  NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;			//���δ����ж�
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;	//��ռ���ȼ�2 
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;					//�����ȼ�0 
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//ʹ���ⲿ�ж�ͨ��
  	NVIC_Init(&NVIC_InitStructure);  	  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���
		
		NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;			//����������ж�
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;	//��ռ���ȼ�1 
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;					//�����ȼ�0 
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//ʹ���ⲿ�ж�ͨ��
  	NVIC_Init(&NVIC_InitStructure);  	  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���
	

		EXTI_ClearITPendingBit(EXTI_Line0);  //���жϱ�־
		EXTI_ClearITPendingBit(EXTI_Line4);  //���жϱ�־

		EXTI_ClearFlag(EXTI_Line0);
		EXTI_ClearFlag(EXTI_Line4);

}

//ͨ�ö�ʱ��3�жϳ�ʼ��
//����ʱ��ѡ��ΪAPB1��2������APB1Ϊ36M
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��
//����ʹ�õ��Ƕ�ʱ��3!
void TIM3_Int_Init(u16 arr,u16 psc)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //ʱ��ʹ��
	
	//��ʱ��TIM3��ʼ��
	TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
 
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE ); //ʹ��ָ����TIM3�ж�,��������ж�

	//�ж����ȼ�NVIC����
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;  //�����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //��ʼ��NVIC�Ĵ���


	TIM_Cmd(TIM3, ENABLE);  //ʹ��TIMx					 
}

//��ʱ��3�жϷ������
void TIM3_IRQHandler(void)   //TIM3�ж�
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)  //���TIM3�����жϷ������
		{
		   xx_Time_current += 100;
			 Heart_beat_time += 100;
			
			if(Heart_beat_time >= 30000000 + 10000 * MODULE_ID)//30s + 10ms*ID
			{
				Heart_beat_time = 10000 * MODULE_ID;
				heart_beat_flag = 1;//����������
			}

			TIM_ClearITPendingBit(TIM3, TIM_IT_Update  );  //���TIMx�����жϱ�־ 
			
		}
}
void DMA1_Channel1_IRQHandler(void)
{


    if(DMA_GetFlagStatus(DMA1_FLAG_TC1))
    {
        DMA_ClearITPendingBit(DMA1_FLAG_TC1);
  
			if(DMA_FLAG == 0)
			{
				DMA_FLAG = 1;
				osSignalSet(copy_task_id, 0x05);//����
			}
			
    }

}
void EXTI0_IRQHandler(void)//���δ����ж�
{
 
	if(EXTI_GetITStatus(EXTI_Line0) != RESET)
	{
   //   if((SI_EVENT_FLAG == 0)&&(Sending == 0))
		  if((SI_EVENT_FLAG == 0)&&(ram_flag == 0))
			{				
				xx_Temp_time_record = xx_Time_current;
				SI_EVENT_FLAG = 1;
				DMA1_Channel1_cnt = DMA_GetCurrDataCounter(DMA1_Channel1);
				DMA_FLAG = 0;
			//	osSignalSet(copy_task_id, 0x05);//����
			}

	    EXTI_ClearITPendingBit(EXTI_Line0);
	}

}
 

void EXTI4_IRQHandler(void)//����������ж�
{
		if(EXTI_GetITStatus(EXTI_Line4) != RESET)//ֹͣ C9 S1
	{

		xx_Time_current = 0;
		
		time_2_zero = 1;

	    EXTI_ClearITPendingBit(EXTI_Line4);
	}
}








