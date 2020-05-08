#include "common.h"
#include "exti.h"
//#include "motor.h"
#include "encode.h"


osThreadId	copy_task_id;
uint16_t test_count;
volatile uint8_t electric_flag = 0;//放电中断标志
volatile uint8_t electric_flag_temp[4];//

uint8_t test_dir = 1;//1 向下 0向上
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
extern uint8_t gprs_cmd_req_data_start; //沉降仪请求数据开始
uint16_t send_screen_again;
uint8_t rtc_wakeup_set;
uint8_t program_runing = 1;//程序在正常运行标志位
extern uint16_t sleep_time_num_xxx;
extern uint16_t PA0_wake_up;
e_DATA_TODAY_SEND_STATE data_today_need_send = DATA_TODAY_NO; //0 无数据发送 1 有数据发送 2 正在发送
extern	s_GATEWAY_DEVICE gateway_dev;
extern uint32_t voltage_temp;
extern uint8_t measure_working;
extern uint8_t screen_state;//屏幕打开 1 关闭 0状态
extern uint8_t measure_start_prepare_flag;
extern uint8_t measure_start_prepare_count;
extern uint8_t enter_stopmode_flag;//息屏情况下计数 是否进入低功耗 标志
extern uint8_t rtc_init_success;
uint8_t open_screen_flag; //打开 1
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
uint16_t TCP_Link_cnt = 0;//连接次数
volatile uint8_t Sending = 0;//正在发送

void delay_ms(u16 time)
{    
   u16 i=0;  
   while(time--)
   {
      i=12000;  //自己定义
      while(i--) ;    
   }
}

//void DMA1CH7_CopyMem(u32 src,u32 des,u32 len,u32 datasize)
//{
//  DMA1->IFCR=DMA1_IT_GL7|DMA1_IT_TC7|DMA1_IT_HT7|DMA1_IT_TE7;
//  DMA1_Channel7->CPAR=src;
//  DMA1_Channel7->CMAR=des;
//  DMA1_Channel7->CNDTR=len;
//  if(datasize==0)     //传8bit的数据
//  {
//    DMA1_Channel7->CCR=DMA_M2M_Enable|DMA_Priority_Low|DMA_MemoryDataSize_Byte|DMA_PeripheralDataSize_Byte
//                       |DMA_MemoryInc_Enable|DMA_PeripheralInc_Enable|DMA_Mode_Normal|DMA_DIR_PeripheralSRC
//                       |0x01;
//  }
//  else if(datasize==1)  //传16bit的数据
//  {
//    DMA1_Channel7->CCR=DMA_M2M_Enable|DMA_Priority_Low|DMA_MemoryDataSize_Byte|DMA_PeripheralDataSize_Byte
//                       |DMA_MemoryInc_Enable|DMA_PeripheralInc_Enable|DMA_Mode_Normal|DMA_DIR_PeripheralSRC
//                       |0x01;
//  }
//  else if(datasize==2)  //传32bit的数据
//  {
//    DMA1_Channel7->CCR=DMA_M2M_Enable|DMA_Priority_Low|DMA_MemoryDataSize_Byte|DMA_PeripheralDataSize_Byte
//                       |DMA_MemoryInc_Enable|DMA_PeripheralInc_Enable|DMA_Mode_Normal|DMA_DIR_PeripheralSRC
//                       |0x01;
//  }
//  else
//  {
//    DMA1_Channel7->CCR=0x00;//到底要传多大尺寸数据???
//    return;
//  }
//  while(!(DMA1->ISR&(DMA1_FLAG_TC7|DMA1_FLAG_TE7))){};
//  DMA1_Channel7->CCR=0x00;    //第一次只能清除EN位
//  DMA1_Channel7->CCR=0x00;    //第二次才能全部清除为0
//  DMA1->IFCR=DMA1_IT_GL7|DMA1_IT_TC7|DMA1_IT_HT7|DMA1_IT_TE7;//清除标志,可以不要管,下次用才清除
//}

void Sys_Standby(void)
{
	//RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE); //使能PWR 外设时钟
	//PWR_WakeUpPinCmd(ENABLE); //使能唤醒管脚功能
	PWR_EnterSTANDBYMode(); //进入(STANDBY)模式
	//PWR_EnterSTOPMode(PWR_Regulator_ON, PWR_STOPEntry_WFI);//STOP模式
}

void Sys_Enter_Standby(void)
{

  //RCC_APB2PeriphResetCmd(0X01FC,DISABLE); //复位所有IO 
  //SystemInit();
//	EXTI_ClearITPendingBit(EXTI_Line0 | EXTI_Line1 | EXTI_Line4 | EXTI_Line10 | EXTI_Line11);     // ?? EXIT ????
  Sys_Standby();
}

//TCP校验码，编码方式：CRC16  多包数据组合求CRC
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

//数据区域字符转移,同时提取  //封装之后发送给云平台的数据转移
uint16_t Send_msg_Translate (uint8_t *tmsg,uint16_t len)	 //*tmsg转译前(1k)  *atmsg转译后(2k)
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
  return tran_len;      //转译后的长度
}

//数据区域字符转移,同时提取 接收到云平台数据转移
uint16_t Recv_msg_Translate (uint8_t *atmsg,uint16_t len)	 //*tmsg转译前(1k)  *atmsg转译后(2k)
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
  return k;      //转译后的长度
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
  GPIO_PinWrite(GPIOB, 1, 1);	//LED_TX 低有效
	
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
			osDelay(5);//5ms后清零
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
	uint16_t crc_data,Translate_size;//转译后的长度
	s_TCP_SEND_DATA_PACKAGE TCP_SEND_DATA_PACKAGE;
	
	
	BIG_NUM ++;//每次发送大包序号加一
	
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
	
	////////////////////////平均值计算//////////////////////////////						
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
		////////////////////////正常波形//////////////////////////////							
		for(i = 1;i <= total_packet;i ++)
			{
				if(i == total_packet)//最后一包末尾 加信号强度4个字节 1个字节电弧状态
				{
						TCP_SEND_DATA_PACKAGE.head = 0x7E;//头1
						TCP_SEND_DATA_PACKAGE.state = 0x00;//状态码1
						TCP_SEND_DATA_PACKAGE.module_id = MODULE_ID;//模块单元号2
						TCP_SEND_DATA_PACKAGE.big_num = BIG_NUM;//大包序号2
						TCP_SEND_DATA_PACKAGE.total_packet_num = total_packet;//总包数2
						TCP_SEND_DATA_PACKAGE.current_packet_num = i;//当前包序号2
						TCP_SEND_DATA_PACKAGE.data_num = ONE_PACK_NUM + 12;//消息体长度2
						memcpy(Tcp_send_Data,(void*)(&TCP_SEND_DATA_PACKAGE.head) , 12);
					
						memcpy((Tcp_send_Data+12), (void *)(&ADCValueTemp0[(i-1)*(ONE_PACK_NUM/2)]), ONE_PACK_NUM);
						memcpy((Tcp_send_Data+12+ONE_PACK_NUM), (void *)(&average), 4);
						memcpy((Tcp_send_Data+16+ONE_PACK_NUM), (void *)(&xx_time_record0), 4); 
						memcpy((Tcp_send_Data+20+ONE_PACK_NUM), (void *)(&DMA1_Channel1_cnt), 4);//DMA1_Channel1_cnt 触发点数
							
						crc_data=cal_serv_crc_N((Tcp_send_Data+1), (TCP_SEND_DATA_PACKAGE.data_num+11),0x00);
						Tcp_send_Data[TCP_SEND_DATA_PACKAGE.data_num+13] = (crc_data&0x00ff);
						Tcp_send_Data[TCP_SEND_DATA_PACKAGE.data_num+12] = (crc_data&0xff00)>>8;
						//数据重组之后转移字符 
						Translate_size = Send_msg_Translate (Tcp_send_Data+1,TCP_SEND_DATA_PACKAGE.data_num+13);
						Tcp_send_Data[Translate_size+1] = 0x7d;
					//	RF_Lora_Dev->WriteData(Tcp_Link_test, Translate_size + 2);
						Usart_SendString(USART2,Tcp_send_Data, Translate_size + 2);

						

				}
				else
				{
						TCP_SEND_DATA_PACKAGE.head = 0x7E;//头1
						TCP_SEND_DATA_PACKAGE.state = 0x00;//状态码1
						TCP_SEND_DATA_PACKAGE.module_id = MODULE_ID;//模块单元号2
						TCP_SEND_DATA_PACKAGE.big_num = BIG_NUM;//大包序号2
						TCP_SEND_DATA_PACKAGE.total_packet_num = total_packet;//总包数2
						TCP_SEND_DATA_PACKAGE.current_packet_num = i;//当前包序号2
						TCP_SEND_DATA_PACKAGE.data_num = ONE_PACK_NUM;//消息体长度2
						memcpy(Tcp_send_Data,(void*)(&TCP_SEND_DATA_PACKAGE.head) , 12);
					
						memcpy((Tcp_send_Data+12), (void *)(&ADCValueTemp0[(i-1)*(ONE_PACK_NUM/2)]), ONE_PACK_NUM);
						
						crc_data=cal_serv_crc_N((Tcp_send_Data+1), (TCP_SEND_DATA_PACKAGE.data_num+11),0x00);
						Tcp_send_Data[TCP_SEND_DATA_PACKAGE.data_num+13] = (crc_data&0x00ff);
						Tcp_send_Data[TCP_SEND_DATA_PACKAGE.data_num+12] = (crc_data&0xff00)>>8;
						//数据重组之后转移字符  
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
	uint16_t crc_data,Translate_size;//转译后的长度
/////////////////////////////////////测试链接//////////////////////
	
		tcp_ask_cmd_package.head = 0x7E;
		tcp_ask_cmd_package.state = 0x03;
		tcp_ask_cmd_package.module_id = MODULE_ID;
		
		memcpy(Tcp_Link_test,(void*)(&tcp_ask_cmd_package.head) , 4);
		crc_data=cal_serv_crc_N((Tcp_Link_test+1), 3,0x00);
		Tcp_Link_test[5] = (crc_data&0x00ff);
		Tcp_Link_test[4] = (crc_data&0xff00)>>8;
		//数据重组之后转移字符  
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
	uint16_t crc_data,Translate_size;//转译后的长度
	//uint8_t *Tcp_send_Data;
  s_TCP_SEND_DATA_PACKAGE TCP_SEND_DATA_PACKAGE;
//	beep_task_id = osThreadGetId();
//	while(beep_task_id == NULL);
	//此任务需要别的任务唤醒因此没必要延时等待呼吸机连接状态
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
			//数据重组之后转移字符  
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
						Sending = 1;//表示正在发送
						TCP_Link_OK = 0;//标志清零
						
						cycle_send();	
						
						Sending = 0;//表示发送完成
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
	uint16_t crc_data,Translate_size;//转译后的长度
	static uint32_t time_now;
	static uint16_t total_packet,last_packet_size;
	static s_TCP_ASK_CMD_PACKAGE tcp_ask_cmd_package;
	//uint8_t *Tcp_send_Data;
 // s_TCP_SEND_DATA_PACKAGE TCP_SEND_DATA_PACKAGE;
	copy_task_id = osThreadGetId();
	while(copy_task_id == NULL);
	//此任务需要别的任务唤醒因此没必要延时等待呼吸机连接状态
	osSignalClear(copy_task_id, 0x05);
  
	while(1){
		
		copy_event = osSignalWait(0x05, osWaitForever);
		if(copy_event.status == osEventSignal)
			{
				if(ram_flag == 0)//不在发送状态 此次触发才有效
         {
					LED1_ON;					

					 xx_time_record0 = xx_Temp_time_record;//记录时间

					 
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
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_WWDG, ENABLE); // WWDG 时钟使能
	WWDG_CNT=tr&WWDG_CNT; //初始化WWDG_CNT.
	WWDG_SetPrescaler(fprer); //设置IWDG预分频值
	WWDG_SetWindowValue(wr); //设置窗口值
	WWDG_Enable(WWDG_CNT); //使能看门狗 设置counter
	WWDG_ClearFlag(); //清除提前唤醒中断标志位
	WWDG_NVIC_Init(); //NVIC
	WWDG_EnableIT(); //开启窗口看门狗中断
}

//重置WWDG 计数器的值
void WWDG_Set_Counter(u8 cnt)
{
	WWDG_Enable(cnt); //使能看门狗 设置counter .
}
*/


void EXTI_init(void){
	
   EXTI_InitTypeDef EXTI_InitStructure;
 	 NVIC_InitTypeDef NVIC_InitStructure;
   GPIO_InitTypeDef GPIO_InitStructure;
  //  C4 RS ; C5 TS1; B0 TS2; A0 F_KEY1(WAKEUP)
	// 	B1 RS ; B11 TS1; B10 TS2; A0 F_KEY1(WAKEUP)
 
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC,ENABLE);//使能PORTB,PORTA,PORTC时钟
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);	//使能复用功能时钟

		
		GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0;//A0
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; //设置成上拉输入
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOC, &GPIO_InitStructure);//初始化GPIOA0
	
	
		GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_4;//A0
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //设置成上拉输入
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOC, &GPIO_InitStructure);//初始化GPIOA0


  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC,GPIO_PinSource0);
  	EXTI_InitStructure.EXTI_Line=EXTI_Line0;
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
		EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_Init(&EXTI_InitStructure);	  	//C0 Button_Down上升沿中断
		
		
		GPIO_EXTILineConfig(GPIO_PortSourceGPIOC,GPIO_PinSource4);
  	EXTI_InitStructure.EXTI_Line=EXTI_Line4;
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;//改成下降沿会有问题
		EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_Init(&EXTI_InitStructure);	  	//C4 下降沿中断
		
	  NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;			//波形触发中断
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;	//抢占优先级2 
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;					//子优先级0 
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//使能外部中断通道
  	NVIC_Init(&NVIC_InitStructure);  	  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器
		
		NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;			//脉冲板清零中断
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;	//抢占优先级1 
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;					//子优先级0 
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//使能外部中断通道
  	NVIC_Init(&NVIC_InitStructure);  	  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器
	

		EXTI_ClearITPendingBit(EXTI_Line0);  //清中断标志
		EXTI_ClearITPendingBit(EXTI_Line4);  //清中断标志

		EXTI_ClearFlag(EXTI_Line0);
		EXTI_ClearFlag(EXTI_Line4);

}

//通用定时器3中断初始化
//这里时钟选择为APB1的2倍，而APB1为36M
//arr：自动重装值。
//psc：时钟预分频数
//这里使用的是定时器3!
void TIM3_Int_Init(u16 arr,u16 psc)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //时钟使能
	
	//定时器TIM3初始化
	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //根据指定的参数初始化TIMx的时间基数单位
 
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE ); //使能指定的TIM3中断,允许更新中断

	//中断优先级NVIC设置
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //先占优先级0级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;  //从优先级3级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //初始化NVIC寄存器


	TIM_Cmd(TIM3, ENABLE);  //使能TIMx					 
}

//定时器3中断服务程序
void TIM3_IRQHandler(void)   //TIM3中断
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)  //检查TIM3更新中断发生与否
		{
		   xx_Time_current += 100;
			 Heart_beat_time += 100;
			
			if(Heart_beat_time >= 30000000 + 10000 * MODULE_ID)//30s + 10ms*ID
			{
				Heart_beat_time = 10000 * MODULE_ID;
				heart_beat_flag = 1;//发送心跳包
			}

			TIM_ClearITPendingBit(TIM3, TIM_IT_Update  );  //清除TIMx更新中断标志 
			
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
				osSignalSet(copy_task_id, 0x05);//测试
			}
			
    }

}
void EXTI0_IRQHandler(void)//波形触发中断
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
			//	osSignalSet(copy_task_id, 0x05);//测试
			}

	    EXTI_ClearITPendingBit(EXTI_Line0);
	}

}
 

void EXTI4_IRQHandler(void)//脉冲板清零中断
{
		if(EXTI_GetITStatus(EXTI_Line4) != RESET)//停止 C9 S1
	{

		xx_Time_current = 0;
		
		time_2_zero = 1;

	    EXTI_ClearITPendingBit(EXTI_Line4);
	}
}








