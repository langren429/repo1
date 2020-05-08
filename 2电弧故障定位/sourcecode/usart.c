
#include "usart.h"	  

char XX[5]={8,8,8,8,8}; 

extern osMessageQId(tcp_recv_msg);
/*
************************************************************
*	�������ƣ�	Usart2_Init
*
*	�������ܣ�	����2��ʼ��
*
*	��ڲ�����	baud���趨�Ĳ�����
*
*	���ز�����	��
*
*	˵����		TX-PA2		RX-PA3
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
	usartInitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//��Ӳ������
	usartInitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;			//���պͷ���
	usartInitStruct.USART_Parity = USART_Parity_No;					//��У��
	usartInitStruct.USART_StopBits = USART_StopBits_1;				//1λֹͣλ
	usartInitStruct.USART_WordLength = USART_WordLength_8b;				//8λ����λ
	USART_Init(USART2, &usartInitStruct);
	
	USART_Cmd(USART2, ENABLE);														//ʹ�ܴ���
	
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);									//ʹ�ܽ����ж�
	
	nvicInitStruct.NVIC_IRQChannel = USART2_IRQn;
	nvicInitStruct.NVIC_IRQChannelCmd = ENABLE;
	nvicInitStruct.NVIC_IRQChannelPreemptionPriority = 0;
	nvicInitStruct.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&nvicInitStruct);
 
}
/*
************************************************************
*	�������ƣ�	Usart_SendString
*
*	�������ܣ�	�������ݷ���
*
*	��ڲ�����	USARTx��������
*				str��Ҫ���͵�����
*				len�����ݳ���
*
*	���ز�����	��
*
*	˵����		
************************************************************
*/
 
void Usart_SendString(USART_TypeDef *USARTx, unsigned char *str, unsigned short len)
{
 
	unsigned short count = 0;
	
	for(; count < len; count++)
	{
		USART_SendData(USARTx, *str++);						//��������
		while(USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);		//�ȴ��������
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

	//	USART_ClearITPendingBit(USART2, USART_IT_RXNE);
}
/*
void uart_init(u32 bound){
  //GPIO�˿�����
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//ʹ��USART1��GPIOAʱ��
  
	//USART1_TX   GPIOA.9
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.9
   
  //USART1_RX	  GPIOA.10��ʼ��
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PA10
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.10  

  //Usart1 NVIC ����
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
  
   //USART ��ʼ������

	USART_InitStructure.USART_BaudRate = bound;//���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

  USART_Init(USART1, &USART_InitStructure); //��ʼ������1
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//�������ڽ����ж�
  USART_Cmd(USART1, ENABLE);                    //ʹ�ܴ���1 

}

void USART1_IRQHandler(void)                	//����1�жϷ������
	{
	u8 Res;

	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
		{
		Res =USART_ReceiveData(USART1);	//��ȡ���յ�������
		
		if((USART_RX_STA&0x8000)==0)//����δ���
			{
			if(USART_RX_STA&0x4000)//���յ���0x0d
				{
				if(Res!=0x0a)USART_RX_STA=0;//���մ���,���¿�ʼ
				else USART_RX_STA|=0x8000;	//��������� 
				}
			else //��û�յ�0X0D
				{	
				if(Res==0x0d)USART_RX_STA|=0x4000;
				else
					{
					USART_RX_BUF[USART_RX_STA&0X3FFF]=Res ;
					USART_RX_STA++;
					if(USART_RX_STA>(USART_REC_LEN-1))USART_RX_STA=0;//�������ݴ���,���¿�ʼ����	  
					}		 
				}
			}   		 
     } 

} */


