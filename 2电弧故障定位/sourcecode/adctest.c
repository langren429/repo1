#include "common.h"
#include "adctest.h"

#define NPT 64            /* NPT = No of FFT point*/
#define CH 3
//u32 testsram[250000] __attribute__((at(0X68000000)));

//__IO uint16_t ADCConvertedValue[NPT][CH];
//__IO uint16_t ADCConvertedValue[125000];
__IO  uint16_t ADCConvertedValue[60000];
//__IO uint16_t ADCConvertedValue[60000];
//uint16_t ADCConvertedValue[125000] __attribute__((at(0X68000000)));

void ADC_test_Init(void)
{
	ADC_InitTypeDef ADC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_ADC1,ENABLE);
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);//分频6
	
	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;//????
	GPIO_Init(GPIOC, &GPIO_InitStructure); //GPIOC.0
	
	ADC_DeInit(ADC1); //?复位ADC1
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent; //ADC 独立模式
	ADC_InitStructure.ADC_ScanConvMode = DISABLE; //单通道模式
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE; //单次转换模式
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;//转换由软件启动
	//???????????
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right; //ADC 数据右对齐
	ADC_InitStructure.ADC_NbrOfChannel = 1; //通道数目
	ADC_Init(ADC1, &ADC_InitStructure); //
	ADC_Cmd(ADC1, ENABLE); //?????ADC1
	ADC_ResetCalibration(ADC1); //开启复位校准
	while(ADC_GetResetCalibrationStatus(ADC1)); //等待复位校准结束
	ADC_StartCalibration(ADC1); //开启AD校准
	while(ADC_GetCalibrationStatus(ADC1)); //等待AD校准结束

}

uint16_t Get_Adc(uint8_t ch)
{
	//????ADC ??????,??????????????
	ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_239Cycles5 );
	//通道ch 规则采样顺序1 采样时间239.5周期
	ADC_SoftwareStartConvCmd(ADC1, ENABLE); //
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));//
	return ADC_GetConversionValue(ADC1); //返回ADC1转换结果
}

uint16_t Get_Adc_Average(uint8_t ch,uint8_t times)
{
	uint32_t temp_total=0;
	uint16_t *temp_val;
	uint8_t t,i,j;
	uint16_t temp;
	temp_val = (uint16_t *)malloc(sizeof(uint16_t) * times);
	for(t=0;t<times;t++)
	{
		temp_val[t] = Get_Adc(ch);
		osDelay(5);
	}
	
 for (j = 0; j < times - 1; j++)
 {
	for (i = 0; i < times - 1 - j; i++)
	{
	 if (temp_val[i] > temp_val[i + 1])
	 {
		temp = temp_val[i]; 
		temp_val[i] = temp_val[i + 1]; 
		temp_val[i + 1] = temp;
	 }
	}
 }
 
 for(t=1;t<times - 1;t++)
 {
	 temp_total += temp_val[t];
 }
 free(temp_val);

	return temp_total/(times - 2);
}
// for (j = 0; j < 9; j++)
// {
//  for (i = 0; i < 9 - j; i++)
//  {
//   if (a[i] > a[i + 1])
//   {
//    temp = a[i]; 
//    a[i] = a[i + 1]; 
//    a[i + 1] = temp;
//   }
//  }
// }
//void TIM3_PWM_Init(u16 arr,u16 psc)
//{
// 
//  GPIO_InitTypeDef GPIO_InitStructure;
//  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
//  TIM_OCInitTypeDef TIM_OCInitStructure;
//	NVIC_InitTypeDef NVIC_InitStructure;
// 
//  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);//TIM3打开
//  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|
//                         RCC_APB2Periph_AFIO,ENABLE);//GPIOA打开
// 
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;  //配置PA6
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //输出
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//  GPIO_Init(GPIOA,&GPIO_InitStructure); //???GPIO
//	
//	//GPIO_ResetBits(GPIOA, GPIO_Pin_5 );
//	//GPIO_SetBits(GPIOA, GPIO_Pin_5 );
// 
//	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn; 
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
//	NVIC_Init(&NVIC_InitStructure);
//  /* GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8; //PA8?DS0
//   //???PA8?????DS0???,???PA8?????
//   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
//   GPIO_Init(GPIOA,&GPIO_InitStructure);*/
// 
//  TIM_TimeBaseStructure.TIM_Period = arr; //自动装置周期值
//  TIM_TimeBaseStructure.TIM_Prescaler = psc; //设置预分频值
//  TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割
//  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //向上计数模式
//  TIM_TimeBaseInit(TIM3,&TIM_TimeBaseStructure); //TIM3
// 
//  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //PWM模式2
//  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
//  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //输出极性高
//  TIM_OC1Init(TIM3,&TIM_OCInitStructure); //TIM3
// 
//  TIM_OC1PreloadConfig(TIM3,TIM_OCPreload_Enable); //使能预装载寄存器
//  TIM_Cmd(TIM3,ENABLE); //使能TIM3
//}

//////////////////////////////////////////////////////////////////////////////////////////////
void Adc_GPIO_Init(void)
{
        GPIO_InitTypeDef GPIO_InitStructure;
        
        RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA, ENABLE); 
      //  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2;   
				GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0; 	
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
        GPIO_Init(GPIOA, &GPIO_InitStructure);

}

void Adc_single_Init(void)
{
        ADC_InitTypeDef ADC_InitStructure;
        
        ADC_DeInit(ADC1);
        
        RCC_APB2PeriphClockCmd( RCC_APB2Periph_ADC1, ENABLE); 
        RCC_ADCCLKConfig(RCC_PCLK2_Div6);//设置ADC时钟(ADCCLK) 72MHZ/6 = 12MHZ
        
        ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;//独立模式
        ADC_InitStructure.ADC_ScanConvMode = DISABLE;//单次模式(单通道) 王健
        ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;//连续模式
        ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T2_CC2;//转换由TIM2 CC2 trig
        ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//ADC数据右对齐
        ADC_InitStructure.ADC_NbrOfChannel = 1;//规则转换的ADC的通道数目,取值范围1-16
        ADC_Init(ADC1,&ADC_InitStructure);//初始化配置
   
        
        
       // ADC_RegularChannelConfig(ADC1, ADC_Channel_12, 2, ADC_SampleTime_7Cycles5);
	
      ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_7Cycles5);
	    // ADC_RegularChannelConfig(ADC1, ADC_Channel_8, 1, ADC_SampleTime_7Cycles5);
	
		//		ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 2, ADC_SampleTime_7Cycles5);
		//		ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 3, ADC_SampleTime_7Cycles5);
        
        ADC_DMACmd(ADC1, ENABLE);//enable DMA

        ADC_Cmd(ADC1,ENABLE);// enable adc
        
        ADC_ResetCalibration(ADC1);
        while( ADC_GetResetCalibrationStatus(ADC1));//获取ADC重置校准寄存器的状态 等待复位结束
        ADC_StartCalibration(ADC1);//开始指定ADC的校准状态
        while( ADC_GetCalibrationStatus(ADC1) );
        
        ADC_ExternalTrigConvCmd(ADC1,ENABLE);
        
        
        
}

void ADC1_DMA_Init(void)
{
         DMA_InitTypeDef DMA_InitStructure;
         NVIC_InitTypeDef NVIC_InitStructure;
        
   RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

   //ADC1 DMA channel 1
   DMA_DeInit(DMA1_Channel1);
   DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&(ADC1->DR);     //DMA对应的外设基地址
   DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&ADCConvertedValue;
   DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
   DMA_InitStructure.DMA_BufferSize = 60000;
   DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
   DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;//  内存自增
   DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;//16位
   DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;//16位
   DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;//DMA normal
   DMA_InitStructure.DMA_Priority = DMA_Priority_High;
   DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
   DMA_Init(DMA1_Channel1, &DMA_InitStructure);
   
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;     // 主优先级0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;            // 次优先级0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

   DMA_ITConfig(DMA1_Channel1,DMA_IT_TC, ENABLE);//使能或失能指定的通道x中断(DMA1 通道1)


   /* Enable DMA1 channel1 */
   DMA_Cmd(DMA1_Channel1, ENABLE);

}

void TIM2_Configration(void)
{
        TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
        TIM_OCInitTypeDef TIM_OCInitStructure;
        //TIM_DeInit(TIM2);
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 , ENABLE);
        
        TIM_TimeBaseStructure.TIM_Period=(72 - 1); //72                                              /*  自动重装载寄存器周期的值（计数值）*/
        /* 累计TIM_Period个频率后产生一个更新或者中断*/
        TIM_TimeBaseStructure.TIM_Prescaler= (10 - 1);                                    // 预分频数72000000/3200 = 22500 = 225 * 100
        TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;                 /* 采样分频*/
        TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; /* 向上计数模式*/
        TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
        TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
        
        TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;//PWM模式1
        TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;//使能

        TIM_OCInitStructure.TIM_Pulse = 50;//脉冲宽度，由这个设置占空比
        TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;//LOW电平有效

        TIM_OC2Init(TIM2,&TIM_OCInitStructure);//初始化
                
        TIM_Cmd(TIM2,ENABLE);
        
        TIM_InternalClockConfig(TIM2);
        TIM_OC2PreloadConfig(TIM2,TIM_OCPreload_Enable);
        TIM_UpdateDisableConfig(TIM2,DISABLE);
        
        
}

//ADC1  
//???????????,??DMA1通道传送
//通道0 
//PA0 
void Adc1_Multi_Init(void)  
{     
    ADC_InitTypeDef ADC_InitStructure;   
    GPIO_InitTypeDef GPIO_InitStructure;  
  
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA |RCC_APB2Periph_ADC1    , ENABLE );   //??ADC1????  
   
  
    RCC_ADCCLKConfig(RCC_PCLK2_Div6);   //??ADC????6 72M/6=12,ADC????????14M  
  
    //PA0  
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;  
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;       //??????  
    GPIO_Init(GPIOA, &GPIO_InitStructure);    
  
    ADC_DeInit(ADC1);  //??ADC1,??? ADC1 ????????????  
  
    ADC_InitStructure.ADC_Mode = ADC_Mode_RegSimult;    //ADC????:ADC1???????  
    ADC_InitStructure.ADC_ScanConvMode =DISABLE; //单次模式(单通道) 王健
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE; //?????????????  
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T2_CC2;//转换由TIM2 CC2 trig 
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;  //ADC右对齐  
    ADC_InitStructure.ADC_NbrOfChannel = 1; //?????????ADC?????  
    ADC_Init(ADC1, &ADC_InitStructure); //??ADC_InitStruct???????????ADCx????     
  
//    ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 1, ADC_SampleTime_239Cycles5 );  
//    ADC_RegularChannelConfig(ADC1, ADC_Channel_5, 2, ADC_SampleTime_239Cycles5 );  
//    ADC_RegularChannelConfig(ADC1, ADC_Channel_6, 3, ADC_SampleTime_239Cycles5 );  
//    ADC_RegularChannelConfig(ADC1, ADC_Channel_7, 4, ADC_SampleTime_239Cycles5 ); 
    ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_7Cycles5);
  

  //  ADC_DMACmd(ADC1, ENABLE);  
		
		        ADC_DMACmd(ADC1, ENABLE);//enable DMA

        ADC_Cmd(ADC1,ENABLE);// enable adc
        
        ADC_ResetCalibration(ADC1);
        while( ADC_GetResetCalibrationStatus(ADC1));//获取ADC重置校准寄存器的状态 等待复位结束
        ADC_StartCalibration(ADC1);//开始指定ADC的校准状态
        while( ADC_GetCalibrationStatus(ADC1) );
        
        ADC_ExternalTrigConvCmd(ADC1,ENABLE);
        
}     
//?????ADC?  
//AD_Value[]?DMA?????????  
/*
u16 Get_Multi_Adc1(void)  
{  
    u32 temp_val=0;  
    u8 t;  
    for(t=0;t<4;t++)  
    {  
        temp_val+=(AD_Value[t] & 0xffff);  
    }  
    return temp_val/4;  
}*/
//???ADC2  
//???????????,??DMA1通道传送 
//通道8  
//管脚PB0 
void Adc2_Multi_Init(void)  
{     
    ADC_InitTypeDef ADC_InitStructure;   
    GPIO_InitTypeDef GPIO_InitStructure;  
  
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB |RCC_APB2Periph_ADC2    , ENABLE );   //??ADC2????  
   
  
    RCC_ADCCLKConfig(RCC_PCLK2_Div6);   //??ADC????6 72M/6=12,ADC????????14M  
  
    //PB0,1 ??????????  
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;  
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;       //??????  
    GPIO_Init(GPIOB, &GPIO_InitStructure);    
  
    ADC_DeInit(ADC2);  //??ADC2,??? ADC2 ????????????  
  
    ADC_InitStructure.ADC_Mode = ADC_Mode_RegSimult;    //ADC????:ADC1???????  
    ADC_InitStructure.ADC_ScanConvMode =DISABLE; //???????????  
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE; //?????????????  
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;//转换由TIM2 CC2 trig 
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;  //ADC?????  
    ADC_InitStructure.ADC_NbrOfChannel = 1; //?????????ADC?????  
    ADC_Init(ADC2, &ADC_InitStructure); //??ADC_InitStruct???????????ADCx????     
  
//    ADC_RegularChannelConfig(ADC2, ADC_Channel_10, 1, ADC_SampleTime_239Cycles5 );  
//    ADC_RegularChannelConfig(ADC2, ADC_Channel_11, 2, ADC_SampleTime_239Cycles5 );  
//    ADC_RegularChannelConfig(ADC2, ADC_Channel_12, 3, ADC_SampleTime_239Cycles5 );  
//    ADC_RegularChannelConfig(ADC2, ADC_Channel_13, 4, ADC_SampleTime_239Cycles5 );  
   ADC_RegularChannelConfig(ADC2, ADC_Channel_8, 1, ADC_SampleTime_7Cycles5);
	 
 //   ADC_ExternalTrigConvCmd(ADC2, ENABLE);                                        //??ADC2???????   
  
    // ??ADC?DMA??(???DMA??,??????DMA?????)  
    ADC_DMACmd(ADC2, ENABLE);  
		
        
        ADC_ResetCalibration(ADC2);
        while( ADC_GetResetCalibrationStatus(ADC2));//获取ADC重置校准寄存器的状态 等待复位结束
        ADC_StartCalibration(ADC2);//开始指定ADC的校准状态
        while( ADC_GetCalibrationStatus(ADC2) );
        
        ADC_ExternalTrigConvCmd(ADC2,ENABLE);
        
}   
//?????ADC?  
//AD_Value2[]?DMA?????????  
/*
u16 Get_Multi_Adc2(void)  
{  
    u32 temp_val=0;  
    u8 t;  
    for(t=0;t<4;t++)  
    {  
        temp_val+=((AD_Value[t]>>16) & 0xffff);  
    }  
    return temp_val/4;  
}*/
void ADC12_Init(void)
{
        ADC_InitTypeDef ADC_InitStructure;
        DMA_InitTypeDef    DMA_InitStructure;
        GPIO_InitTypeDef GPIO_InitStructure;
	      NVIC_InitTypeDef NVIC_InitStructure;
        
        //开启时钟
        RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);                              //使能DMA传输
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1|RCC_APB2Periph_ADC2,ENABLE);   //使能ADC传输        
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB,ENABLE); //使能GPIO传输

        
        //GPIOA作为模拟通道输入引脚
        GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0;
        GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AIN;//模拟输入
        GPIO_Init(GPIOA,&GPIO_InitStructure);      //初始化GPIOA       
        
  //GPIOB作为模拟通道输入引脚
        GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0;
        GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AIN;//模拟输入
        GPIO_Init(GPIOB,&GPIO_InitStructure);      //初始化GPIOB        
        
        ADC_DeInit(ADC1);                 //复位ADC1
        RCC_ADCCLKConfig(RCC_PCLK2_Div6); //设置ADC时钟，72/6=12MHz

  //-----------ADC1参数设置----------------------------------
        ADC_InitStructure.ADC_Mode=ADC_Mode_RegSimult;      //ADC同步规则模式
        ADC_InitStructure.ADC_ScanConvMode=DISABLE;          //多通道模式
        ADC_InitStructure.ADC_ContinuousConvMode=DISABLE;    //连续转换模式
        ADC_InitStructure.ADC_ExternalTrigConv=ADC_ExternalTrigConv_T2_CC2;//转换由软件触发而不受外部触发
        ADC_InitStructure.ADC_DataAlign=ADC_DataAlign_Right;//ADC数据右对齐
        ADC_InitStructure.ADC_NbrOfChannel=1;               //顺序进行规则转换的ADC通道数目1
        ADC_Init(ADC1,&ADC_InitStructure);                  //初始化ADC1

        ADC_RegularChannelConfig(ADC1,ADC_Channel_0,1,ADC_SampleTime_7Cycles5);  //ADC_Test1采样        
    //    ADC_RegularChannelConfig(ADC1,ADC_Channel_6, 2,ADC_SampleTime_71Cycles5);  //ADC_Test2采样        
    //    ADC_RegularChannelConfig(ADC1,ADC_Channel_4, 3,ADC_SampleTime_71Cycles5);  //ADC_Test3采样        
    //    ADC_RegularChannelConfig(ADC1,ADC_Channel_2, 4,ADC_SampleTime_71Cycles5);  //ADC_Test4采样        
        
  //-----------ADC2参数设置----------------------------------
        ADC_InitStructure.ADC_Mode=ADC_Mode_RegSimult;      //ADC同步规则模式
        ADC_InitStructure.ADC_ScanConvMode=DISABLE;          //多通道模式
        ADC_InitStructure.ADC_ContinuousConvMode=DISABLE;    //连续转换模式
        ADC_InitStructure.ADC_ExternalTrigConv=ADC_ExternalTrigConv_T2_CC2;//转换由软件触发而不受外部触发
        ADC_InitStructure.ADC_DataAlign=ADC_DataAlign_Right;//ADC数据右对齐
        ADC_InitStructure.ADC_NbrOfChannel=1;               //顺序进行规则转换的ADC通道数目4
        ADC_Init(ADC2,&ADC_InitStructure);                  //初始化ADC1

        ADC_RegularChannelConfig(ADC2,ADC_Channel_8 ,1,ADC_SampleTime_7Cycles5);  //FreeBack1采样        
    //    ADC_RegularChannelConfig(ADC2,ADC_Channel_7 , 2,ADC_SampleTime_71Cycles5);  //FreeBack2采样        
    //    ADC_RegularChannelConfig(ADC2,ADC_Channel_5,  3,ADC_SampleTime_71Cycles5);  //FreeBack3采样        
    //    ADC_RegularChannelConfig(ADC2,ADC_Channel_3,  4,ADC_SampleTime_71Cycles5);  //FreeBack4采样        
        
        ADC_ExternalTrigConvCmd(ADC2, ENABLE);                                     //使能ADC2外部触发转换
        
  //------------DMA参数设置----------------------------------
        DMA_DeInit(DMA1_Channel1);                                 //将DMA的通道x寄存器重设为缺省值
        DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&ADC1->DR; //DMA外设基地址
        DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&ADCConvertedValue;    //DMA内存基地址
        DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;         //数据传输方向，从内存读取
        DMA_InitStructure.DMA_BufferSize = 60000;                      //DMA通道的DMA缓存的大小
        DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;             //外设地址寄存器不变
        DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                      //内存地址寄存器递增
        DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;      //数据宽度为32位
        DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;              //数据宽度为32位
        DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;          //工作在循环缓存模式
        DMA_InitStructure.DMA_Priority = DMA_Priority_High;     //DMA通道 x拥有中等优先级 
        DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;            //DMA通道x没有设置为内存到内存传输
        DMA_Init(DMA1_Channel1, &DMA_InitStructure);            //根据DMA_InitStruct中指定的参数初始化DMA的通道USART1_Tx_DMA_Channel所标识的寄存器
        
//		NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn;
//		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;     // 主优先级0
//		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;            // 次优先级0
//		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//		NVIC_Init(&NVIC_InitStructure);

//	    DMA_ITConfig(DMA1_Channel1,DMA_IT_TC, ENABLE);//使能或失能指定的通道x中断(DMA1 通道1)
   
        DMA_Cmd(DMA1_Channel1,ENABLE);                          //打开DMA传输
        
        ADC_Cmd(ADC1,ENABLE);                 //使能ADC1
        ADC_Cmd(ADC2,ENABLE);                 //使能ADC2
        
        ADC_ResetCalibration(ADC1);                 //开启复位校准
        while(ADC_GetResetCalibrationStatus(ADC1)); //等待复位校准结束
        ADC_StartCalibration(ADC1);                 //开启AD校准
        while(ADC_GetCalibrationStatus(ADC1));      //等待校准结束
        
        ADC_ResetCalibration(ADC2);                 //开启复位校准
        while(ADC_GetResetCalibrationStatus(ADC2)); //等待复位校准结束
        ADC_StartCalibration(ADC2);                 //开启AD校准
        while(ADC_GetCalibrationStatus(ADC2));      //等待校准结束
        
        
        ADC_DMACmd(ADC1, ENABLE);                      //ADC1 DMA传送数据打开 
        ADC_SoftwareStartConvCmd(ADC1,ENABLE);         //使能指定的ADC1的软件转换功能
}