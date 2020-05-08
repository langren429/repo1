
#include "usart.h"	  

char XX[5]={8,8,8,8,8}; 

extern osMessageQId(tcp_recv_msg);
/*
************************************************************
*	函数名称：	Usart2_Init
*
*	函数功能：	串口2初始化
*
*	入口参数：	baud：设定的波特率
*
*	返回参数：	无
*
*	说明：		TX-PA2		RX-PA3
************************************************************
*/
void Usart2_Init(unsigned int baud)
{
 
	GPIO_InitTypeDef gpioInitStruct;
	USART_InitTypeDef usartInitStruct;
	NVIC_InitTypeDef nvicInitStruct;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	
	//PA2	TXD
	gpioInitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	gpioInitStruct.GPIO_Pin = GPIO_Pin_2;
	gpioInitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &gpioInitStruct);
	
	//PA3	RXD
	gpioInitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	gpioInitStruct.GPIO_Pin = GPIO_Pin_3;
	gpioInitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &gpioInitStruct);
	
	usartInitStruct.USART_BaudRate = baud;
	usartInitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//无硬件流控
	usartInitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;			//接收和发送
	usartInitStruct.USART_Parity = USART_Parity_No;					//无校验
	usartInitStruct.USART_StopBits = USART_StopBits_1;				//1位停止位
	usartInitStruct.USART_WordLength = USART_WordLength_8b;				//8位数据位
	USART_Init(USART2, &usartInitStruct);
	
	USART_Cmd(USART2, ENABLE);														//使能串口
	
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);									//使能接收中断
	
	nvicInitStruct.NVIC_IRQChannel = USART2_IRQn;
	nvicInitStruct.NVIC_IRQChannelCmd = ENABLE;
	nvicInitStruct.NVIC_IRQChannelPreemptionPriority = 0;
	nvicInitStruct.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&nvicInitStruct);
 
}
/*
************************************************************
*	函数名称：	Usart_SendString
*
*	函数功能：	串口数据发送
*
*	入口参数：	USARTx：串口组
*				str：要发送的数据
*				len：数据长度
*
*	返回参数：	无
*
*	说明：		
************************************************************
*/
 
void Usart_SendString(USART_TypeDef *USARTx, unsigned char *str, unsigned short len)
{
 
	unsigned short count = 0;
	
	for(; count < len; count++)
	{
		USART_SendData(USARTx, *str++);						//发送数据
		while(USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);		//等待发送完成
	}
 
}

/*
************************************************************
*	函数名称：	USART2_IRQHandler
*
*	函数功能：	串口2收发中断
*
*	入口参数：	无
*
*	返回参数：	无
*
*	说明：		
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
        crc_data1 = USART_ReceiveData(USART2);
			Tcp_cmd_flag = 0;
    }

		//USART_ClearITPendingBit(USART2, USART_IT_RXNE);
}*/


void USART2_IRQHandler(void)
{
	uint8_t	recv_char;
		static uint8_t lora_head_flag = 0;
	//static uint8_t AT_head_flag = 0;
//	static unsigned short crc_data1;
	//unsigned int crc_data2;
	static uint16_t  crc_data1;
	static s_FROM_SERV_TCP_CMD from_serv_tcp_cmd={0};
	static uint8_t Tcp_cmd_flag = 0;
	
  if (USART_GetFlagStatus(USART2, USART_FLAG_ORE) != RESET)
  {
      USART_ReceiveData(USART2);
      USART_ClearFlag(USART2, USART_FLAG_ORE);
  }

  if (USART_GetFlagStatus(USART2, USART_FLAG_RXNE) != RESET)
    {
        USART_ClearFlag(USART2, USART_FLAG_RXNE);
 			  USART_ClearITPendingBit(USART2, USART_IT_RXNE);
			
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
								
						//		Usart_SendString(USART2, XX, 5);
								
								
								from_serv_tcp_cmd.data[from_serv_tcp_cmd.size++]=recv_char;
			
								//接受的数据帧中7E 7D转换为7C 5E等
								from_serv_tcp_cmd.size=Recv_msg_Translate (from_serv_tcp_cmd.data,from_serv_tcp_cmd.size);
								//将数据发送到特殊队列
								osMessagePut(tcp_recv_msg, (uint32_t)(&from_serv_tcp_cmd), 0);
								from_serv_tcp_cmd.size=0;
								//一帧数据的结束
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
							  //数据超过20，则认为数据错误
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

	//	USART_ClearITPendingBit(USART2, USART_IT_RXNE);
}
/*
void uart_init(u32 bound){
  //GPIO端口设置
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//使能USART1，GPIOA时钟
  
	//USART1_TX   GPIOA.9
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
  GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.9
   
  //USART1_RX	  GPIOA.10初始化
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PA10
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
  GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.10  

  //Usart1 NVIC 配置
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
  
   //USART 初始化设置

	USART_InitStructure.USART_BaudRate = bound;//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

  USART_Init(USART1, &USART_InitStructure); //初始化串口1
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启串口接受中断
  USART_Cmd(USART1, ENABLE);                    //使能串口1 

}

void USART1_IRQHandler(void)                	//串口1中断服务程序
	{
	u8 Res;

	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
		{
		Res =USART_ReceiveData(USART1);	//读取接收到的数据
		
		if((USART_RX_STA&0x8000)==0)//接收未完成
			{
			if(USART_RX_STA&0x4000)//接收到了0x0d
				{
				if(Res!=0x0a)USART_RX_STA=0;//接收错误,重新开始
				else USART_RX_STA|=0x8000;	//接收完成了 
				}
			else //还没收到0X0D
				{	
				if(Res==0x0d)USART_RX_STA|=0x4000;
				else
					{
					USART_RX_BUF[USART_RX_STA&0X3FFF]=Res ;
					USART_RX_STA++;
					if(USART_RX_STA>(USART_REC_LEN-1))USART_RX_STA=0;//接收数据错误,重新开始接收	  
					}		 
				}
			}   		 
     } 

} */


