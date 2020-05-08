/*
 * (C) Copyright 2014.7.30 Thingtek
 *
 * Written by: 魏本萍
 * 文件说明：PCF8563 时钟程序
 * 文件内容：PCF8563 时钟芯片使用接口为I2C接口 PCF8563初始化 PCF8563设置时间
             读取PCF8563时间参数   
 * 注意事项： PCF8563读出或写入的时间参数为BCD码 因此需要注意转换
 * 文件模块： PCF8563 函数操作
 */
 
#include "RTC_PCF8563.h"

//全局变量声明区域
//extern osMessageQId(gprs_send_msg);
extern uint8_t rtc_set_time[7];
extern s_SCREEN_SET SCREEN_SET;
uint8_t REG2_VALUE; //寄存器2赋值使用全局变量目的为方便修改及中断时修改其值
//函数申明区域

void PCF8563_GPIO_Config(void){
	//PCF8563引脚初始化
//  GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure; 
	EXTI_InitTypeDef          EXTI_InitStructure;	
	
//	NVIC_PriorityGroupConfig( NVIC_PriorityGroup_2 ); 
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC, ENABLE);
	//GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource1);
	
	//此种方法为使用RTX操作系统中的函数申明引脚
	//实时时钟中断引脚声明 PB4
	//GPIO_PortClock(GPIOB, true);
//	GPIO_PinConfigure(GPIOB, 7, GPIO_IN_PULL_UP , GPIO_MODE_INPUT);
	
	GPIO_PortClock(GPIOB, true);
 	GPIO_PinConfigure(GPIOB, 4, GPIO_IN_PULL_UP , GPIO_MODE_INPUT);
	
	// Configure I2C pins: PB8 PB9
	GPIO_PortClock(GPIOB, true);
	GPIO_PinConfigure(GPIOB, 8, GPIO_OUT_PUSH_PULL, GPIO_MODE_OUT50MHZ);
	GPIO_PinConfigure(GPIOB, 9, GPIO_OUT_PUSH_PULL, GPIO_MODE_OUT50MHZ);
	//此种方法为使用STM32内部库函数声明引脚
	// Configure I2C pins: PB8 PB9
//	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_8 | GPIO_Pin_9;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP; //开漏输出 开始不用改变输入的情况下读取IO电平
//	GPIO_Init(GPIOB, &GPIO_InitStructure);
	//实时时钟中断引脚声明 PB7
//	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_7; 
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU;
//	GPIO_Init(GPIOB, &GPIO_InitStructure);
	SCL_H; //引脚拉高
	SDA_H;
	// Enable the EXTI1_IRQn Interrupt 中断优先级      EXTI1_IRQn
//	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_Init(&NVIC_InitStructure);
//	
//	//将PC1设置为时钟外部中断引脚
//	EXTI_InitStructure.EXTI_Line    = EXTI_Line7;
//	EXTI_InitStructure.EXTI_Mode    = EXTI_Mode_Interrupt;
//	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
//	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
//	EXTI_Init(&EXTI_InitStructure);
//	
//	EXTI_ClearITPendingBit(EXTI_Line7);  //清中断标志
	
}

//延时函数
void delay_us(uint16_t count){
	
	uint16_t i;
	for(i=0;i<count;i++);
}
//模拟I2C起始信号函数
void I2C_Start(void){
	
	SDA_OUT();
  SDA_H;
	SCL_H;
	delay_us(50);
	SDA_L;
	delay_us(50);
}

//模拟I2C终止信号函数
void I2C_Stop(void){
	
	SDA_OUT();
	SDA_L;
	delay_us(50);
	SCL_H;
	delay_us(50);
	SDA_H;
	delay_us(50);
}
//模拟I2C等待应答函数
//返回说明：返回值为1表示接收应答失败 为0表示接收应答成功
void I2C_Wait_ACK(void){
  uint16_t i;
	SDA_H;
	SDA_IN();
	SCL_L;	
	delay_us(50);
	SCL_H;	
	while(SDA_read&&(i<300)) i++;
	SCL_L;
}
//模拟I2C应答信号函数
void I2C_SEND_ACK(void){
 
	SDA_OUT();
  SCL_L;
	delay_us(50);
	SDA_L;
	delay_us(50);
	SCL_H;
	delay_us(50);
	SCL_L;
}
//模拟I2C无应答信号函数
void I2C_SEND_NACK(void){
 
	SDA_OUT();
  SCL_L;
	delay_us(50);
	SDA_H;
	delay_us(50);
	SCL_H;
	delay_us(50);
	SCL_L;
}	

//模拟I2C发送一个字节
void I2C_Send_OneByte(uint8_t Sendbyte){
  
	uint8_t i;	
	SDA_OUT();
	SCL_L;
	for(i=0;i<8;i++){	  
		delay_us(50);
		if(Sendbyte&0x80) //数据从高位到低位
			SDA_H;
		else
			SDA_L;
		delay_us(50);
		SCL_H;
		delay_us(50);
		SCL_L;
		Sendbyte<<=1;
	}
	SDA_H; 
}

//模拟I2C接收一个字节
uint8_t I2C_Receive_OneByte(void){
 
	uint8_t i,Receivebyte=0;
	SDA_H;
	SDA_IN();
	SCL_L;
	for(i=0;i<8;i++){
		Receivebyte<<=1; //数据从高位到低位
		if(SDA_read)
		  Receivebyte|= 0x01;
    else
      Receivebyte&= ~(0x01);			
	  SCL_H;
		delay_us(50);
		SCL_L;
		delay_us(50);		
	}
	SDA_OUT();
	SDA_H;
	
  return Receivebyte;
}
//PCF8563时钟芯片写一个字节 
//SendByte：待写入数据 DeviceAdd：器件类型  WriteAdd：待写入地址
void PCF8563_WriteOneByte(uint8_t DeviceAdd,uint8_t WriteAdd,uint8_t SendByte){
  
	I2C_Start(); //起始信号
	I2C_Send_OneByte(DeviceAdd); //写命令
	I2C_Wait_ACK();
	I2C_Send_OneByte(WriteAdd); //写地址
	I2C_Wait_ACK();
  I2C_Send_OneByte(SendByte); //写数据
	I2C_Wait_ACK();
	I2C_Stop(); //终止信号
}

//PCF8563时钟芯片读一个字节 
//DeviceAdd：器件类型  READAdd：待读出数据的地址
uint8_t PCF8563_ReadOneByte(uint8_t DeviceAdd,uint8_t READAdd){
 
  uint8_t ReceiveByte; 	
	I2C_Start(); //起始信号
	I2C_Send_OneByte(DeviceAdd); //写命令
	I2C_Wait_ACK();
	I2C_Send_OneByte(READAdd); //写地址
	I2C_Wait_ACK();
  I2C_Start(); //起始信号
	I2C_Send_OneByte(DeviceAdd|0x01); //读命令
	I2C_Wait_ACK();
	ReceiveByte = I2C_Receive_OneByte();
	I2C_SEND_NACK();
	I2C_Stop(); //终止信号
	return  ReceiveByte;
}

void PCF8563_ReadlenByte(uint8_t DeviceAdd,uint8_t ReadAdd,uint8_t len,uint8_t *recv_buff){

   uint8_t i;
	 I2C_Start(); //起始信号
	 I2C_Send_OneByte(DeviceAdd); //写命令
	 I2C_Wait_ACK();
	 I2C_Send_OneByte(ReadAdd); //写地址
	 I2C_Wait_ACK();
   I2C_Start(); //起始信号
	 I2C_Send_OneByte(DeviceAdd|0x01); //读命令
	 I2C_Wait_ACK();
	 for(i=0;i<len;i++){
	   recv_buff[i] = I2C_Receive_OneByte();
		 if(i<len-1) I2C_SEND_ACK();
	 }
	 I2C_SEND_NACK();
	 I2C_Stop(); //终止信号
}

//PCF8563写入时间参数  设置PCF8563时间参数 //时间赋值位高地址为年 低地址为秒
void PCF8563_write_time(uint8_t set_time[]){
//设置时间 数组 世纪年月星期日小时分钟秒  月中包含世纪
	uint8_t pcf_set_time[7]; //根据PCF8563格式设置时间
	uint8_t i,pcf8563_wtim_bcd[7];
	//设置的时间不包含世纪与星期 因此需要转换增加星期
	memcpy(pcf_set_time+3,set_time+3,4);
  pcf_set_time[2] = 0x02;  	//日
	pcf_set_time[1] =set_time[2]&0x0f+0x50;//((((set_time[1]/10)<<4)&0xf0)|((set_time[1]%10)&0x0f))|0x80;//月
	pcf_set_time[0] =set_time[1]; //年
//	pcf_set_time[0] = (uint8_t)(year%2000);
	//设置RTC时间需要将时钟停止后再运行设置时间才能成功
	PCF8563_WriteOneByte(PCF_DEVICE_ADD, STATUS_REG1,0x20); // 停止RTC时钟
	//将写入的时间由十进制转换为BCD码
	for(i=0;i<7;i++){
	  pcf8563_wtim_bcd[i]=(((pcf_set_time[i]/10)<<4)&0xf0)|((pcf_set_time[i]%10)&0x0f);	
//	  pcf8563_wtim_bcd[i]=(((pcf8563_time[i]/10)<<4)&0xf0)|((pcf8563_time[i]%10)&0x0f);
		PCF8563_WriteOneByte(PCF_DEVICE_ADD, SEC+6-i, pcf8563_wtim_bcd[i]);
		//2~8 寄存器 秒 分 时 天 星期 世纪\月 年
		//8~2  年 月 日 时 分 秒
		//pcf_set_time 年 世纪\月 星期 天 时 分 秒 
		//set_time     1   2      0x02 
	}
	PCF8563_WriteOneByte(PCF_DEVICE_ADD, STATUS_REG1,0x00); // 运行RTC时钟
}
//返回的时钟信息不包含世纪与星期
void PCF8563_read_time(uint8_t *pcf8563_time){
  
	//注:读出数据为BCD码需要将其转换为10机制 pcf8563_all_time获取RTC时钟内所有的时钟信息 
	uint8_t i , pcf8563_rtim_bcd[7],pcf8563_all_time[8];
//	PCF8563_ReadlenByte(PCF_DEVICE_ADD,SEC,0x07,pcf8563_rtim_bcd);
	//2~8 寄存器 秒 分 时 天 星期 世纪\月 年
	for(i=0;i<7;i++)
	  pcf8563_rtim_bcd[i]=PCF8563_ReadOneByte(PCF_DEVICE_ADD,SEC+i);
	//年
	pcf8563_rtim_bcd[6] &=0xff;
	pcf8563_all_time[6] = (pcf8563_rtim_bcd[6]>>4)*10+(pcf8563_rtim_bcd[6]&0x0f);
	pcf8563_time[0] = pcf8563_all_time[6]; //年
	//月/世纪  月的最高位表示世纪 最高位为1则为20世纪 最高位为0则为19世纪 
	pcf8563_rtim_bcd[5] &=0x9f; 	
	if(pcf8563_rtim_bcd[5]&0x80) 
		pcf8563_all_time[7] = 20;
	else
		pcf8563_all_time[7] = 19;
	pcf8563_rtim_bcd[5] &=0x1f;
	pcf8563_all_time[5] = (pcf8563_rtim_bcd[5]>>4)*10+(pcf8563_rtim_bcd[5]&0x0f);
	pcf8563_time[1] = pcf8563_all_time[5]; //月
	//星期
	pcf8563_all_time[4] = pcf8563_rtim_bcd[4] &0x07;;
	//日
	pcf8563_rtim_bcd[3] &=0x3f;
	pcf8563_all_time[3] = (pcf8563_rtim_bcd[3]>>4)*10+(pcf8563_rtim_bcd[3]&0x0f);
	pcf8563_time[2] = pcf8563_all_time[3]; //日
	//时
	pcf8563_rtim_bcd[2] &=0x3f;
	pcf8563_all_time[2] = (pcf8563_rtim_bcd[2]>>4)*10+(pcf8563_rtim_bcd[2]&0x0f);
	pcf8563_time[3] = pcf8563_all_time[2]; //时
	//分钟
	pcf8563_rtim_bcd[1] &=0x7f;
	pcf8563_all_time[1] = (pcf8563_rtim_bcd[1]>>4)*10+(pcf8563_rtim_bcd[1]&0x0f);
	pcf8563_time[4] = pcf8563_all_time[1]; //分
	//秒
  pcf8563_rtim_bcd[0] &=0x7f;
	pcf8563_all_time[0] = (pcf8563_rtim_bcd[0]>>4)*10+(pcf8563_rtim_bcd[0]&0x0f);
	pcf8563_time[5] = pcf8563_all_time[0]; //秒
//	send_to_tcp_serv(pcf8563_time,8);
}

//PB7--INT //用于PCF8563中断引脚
//void EXTI9_5_IRQHandler(void)
//{
//	//PCF8563时钟中断处理
//	if(EXTI_GetITStatus(EXTI_Line7) != RESET)
//	{
//		EXTI_ClearITPendingBit(EXTI_Line7);		
//		//相应时钟中断处理操作
//		PCF8563_WriteOneByte(PCF_DEVICE_ADD, STATUS_REG2,REG2_VALUE&0xfb);
//    LED2_Toggle;
////		BEEP_Toggle;
//	}
//}
//void EXTI4_IRQHandler(void)//RTC中断
//{
//	static uint8_t ti = 0;
////	osDelay(100);
//	if(EXTI_GetITStatus(EXTI_Line4) != RESET)
//	{
//		
//    EXTI_ClearITPendingBit(EXTI_Line4);
//	//	GPIO_PinWrite(GPIOB, 12, 1);	//蜂鸣器
//		//GPIO_PinWrite(GPIOC, 4, ti);
//	  GPIO_PinWrite(GPIOC, 5, ti);
//		ti = ~ti;
//		PCF8563_WriteOneByte(PCF_DEVICE_ADD, Min_Alarm,0x26); // 8点报警
//	  PCF8563_WriteOneByte(PCF_DEVICE_ADD, STATUS_REG2,0x12); //报警有效 INT引脚激活 报警中断使能	


//	}
//}
void PCF8563_CLKOUT_1S(void){

  PCF8563_WriteOneByte(PCF_DEVICE_ADD, STATUS_REG2,0x00); //禁止定时器输出，闹铃输出
//  PCF8563_WriteOneByte(PCF_DEVICE_ADD, TIMER_CTL,0x00); //关闭定时器 
  PCF8563_WriteOneByte(PCF_DEVICE_ADD, TIMER_CTL,0x03);  //写入1 定时器控制为0
	PCF8563_WriteOneByte(PCF_DEVICE_ADD, CLKOUT_REG,0x83);
}

void PCF8563_init(void){	
	uint8_t pcf_time[8]={0}; //从PCF8563读出的时间
	
	REG2_VALUE=0x01;
	PCF8563_GPIO_Config(); //PCF8563引脚声明
  

		rtc_set_time[1] = 17;//年
	rtc_set_time[2] = 8;//月
	rtc_set_time[3] = 25;//日
	rtc_set_time[4] = 11;//时
	rtc_set_time[5] = 59;//分
	rtc_set_time[6] = 2;//秒
	
	PCF8563_write_time( rtc_set_time); //设置时间
	PCF8563_WriteOneByte(PCF_DEVICE_ADD, STATUS_REG1,0x00); // 运行RTC时钟
	
	PCF8563_WriteOneByte(PCF_DEVICE_ADD, Min_Alarm, 0x80); // 关闭分钟报警
	PCF8563_WriteOneByte(PCF_DEVICE_ADD, Hour_Alarm, 0x80); // 关闭小时报警
 
//  PCF8563_WriteOneByte(PCF_DEVICE_ADD, Min_Alarm, 0x00); // 8点报警 打开分钟报警
//	PCF8563_WriteOneByte(PCF_DEVICE_ADD, Hour_Alarm, 0x12); // 8点报警 打开小时报警

//	PCF8563_WriteOneByte(PCF_DEVICE_ADD, STATUS_REG2,0x12); //报警有效 INT引脚激活 报警中断使能	
//一天测一次代码	
	/*if((rtc_set_time[4] <= 0x07)&&(rtc_set_time[4] >= 0x00))//0点到7点
	{
	  //关闭太阳能充电开关
		GPIO_PinWrite(GPIOB, 0, 1);//太阳能充电开关 低电平有效
		PCF8563_WriteOneByte(PCF_DEVICE_ADD, Hour_Alarm,0x08); // 8点报警
		PCF8563_WriteOneByte(PCF_DEVICE_ADD, STATUS_REG2,0x12); //报警有效 INT引脚激活 报警中断使能	
	}
	else if((rtc_set_time[4] >= 0x08)&&(rtc_set_time[4] <= 0x10))//8点到16点
	{
	  //打开太阳能充电开关
		GPIO_PinWrite(GPIOB, 0, 0);//太阳能充电开关 低电平有效
		PCF8563_WriteOneByte(PCF_DEVICE_ADD, Hour_Alarm,0x17); // 17点报警
		PCF8563_WriteOneByte(PCF_DEVICE_ADD, STATUS_REG2,0x12); //报警有效 INT引脚激活 报警中断使能			
	}
	else if((rtc_set_time[4] >= 0x11)&&(rtc_set_time[4] <= 0x13))//17点到19点
	{
		//关闭太阳能充电开关
      GPIO_PinWrite(GPIOB, 0, 1);//太阳能充电开关 低电平有效
			PCF8563_WriteOneByte(PCF_DEVICE_ADD, Hour_Alarm,0x08); // 8点报警
			PCF8563_WriteOneByte(PCF_DEVICE_ADD, STATUS_REG2,0x12); //报警有效 INT引脚激活 报警中断使能
		
	}
	else if((rtc_set_time[4] >= 0x14)&&(rtc_set_time[4] <= 0x18))//20点到24点
	{
		//关闭太阳能充电开关  
		  GPIO_PinWrite(GPIOB, 0, 1);//太阳能充电开关 低电平有效
		  PCF8563_WriteOneByte(PCF_DEVICE_ADD, Hour_Alarm,0x08); // 8点报警
			PCF8563_WriteOneByte(PCF_DEVICE_ADD, STATUS_REG2,0x12); //报警有效 INT引脚激活 报警中断使能
	}*/
//一天测两次代码	
	/*if((rtc_set_time[4] <= 0x07)&&(rtc_set_time[4] >= 0x00))//0点到7点
	{
	  //关闭太阳能充电开关
		GPIO_PinWrite(GPIOB, 0, 1);//太阳能充电开关 低电平有效
		PCF8563_WriteOneByte(PCF_DEVICE_ADD, Hour_Alarm,0x08); // 8点报警
		PCF8563_WriteOneByte(PCF_DEVICE_ADD, STATUS_REG2,0x12); //报警有效 INT引脚激活 报警中断使能	
	}
	else if((rtc_set_time[4] >= 0x08)&&(rtc_set_time[4] <= 0x10))//8点到16点
	{
	  //打开太阳能充电开关
		GPIO_PinWrite(GPIOB, 0, 0);//太阳能充电开关 低电平有效
		PCF8563_WriteOneByte(PCF_DEVICE_ADD, Hour_Alarm,0x17); // 17点报警
		PCF8563_WriteOneByte(PCF_DEVICE_ADD, STATUS_REG2,0x12); //报警有效 INT引脚激活 报警中断使能			
	}
	else if((rtc_set_time[4] >= 0x11)&&(rtc_set_time[4] <= 0x13))//17点到19点
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
	else if((rtc_set_time[4] >= 0x14)&&(rtc_set_time[4] <= 0x18))//20点到24点
	{
		//关闭太阳能充电开关  
		  PCF8563_WriteOneByte(PCF_DEVICE_ADD, Hour_Alarm,0x08); // 8点报警
			PCF8563_WriteOneByte(PCF_DEVICE_ADD, STATUS_REG2,0x12); //报警有效 INT引脚激活 报警中断使能
	}*/
	
//	PCF8563_WriteOneByte(PCF_DEVICE_ADD, CLKOUT_REG,0xf0); //CLKOUT输出激活 输出频率为32.768KHZ
//	//设置定时器有效 3S使能一次
//	PCF8563_WriteOneByte(PCF_DEVICE_ADD, STATUS_REG2,REG2_VALUE); //定时器中断有效
//	PCF8563_WriteOneByte(PCF_DEVICE_ADD, TIMER_CTL,0x82);   //倒计时定时器寄存器 定时器有效 定时频率为1HZ
//	PCF8563_WriteOneByte(PCF_DEVICE_ADD, TIMER_Count,0x01); //定时器倒计时数值 3S
//	PCF8563_CLKOUT_1S();
//	PCF8563_read_time(pcf_time); //读出时间
}










