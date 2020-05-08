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
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);//��Ƶ6
	
	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;//????
	GPIO_Init(GPIOC, &GPIO_InitStructure); //GPIOC.0
	
	ADC_DeInit(ADC1); //?��λADC1
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent; //ADC ����ģʽ
	ADC_InitStructure.ADC_ScanConvMode = DISABLE; //��ͨ��ģʽ
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE; //����ת��ģʽ
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;//ת�����������
	//???????????
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right; //ADC �����Ҷ���
	ADC_InitStructure.ADC_NbrOfChannel = 1; //ͨ����Ŀ
	ADC_Init(ADC1, &ADC_InitStructure); //
	ADC_Cmd(ADC1, ENABLE); //?????ADC1
	ADC_ResetCalibration(ADC1); //������λУ׼
	while(ADC_GetResetCalibrationStatus(ADC1)); //�ȴ���λУ׼����
	ADC_StartCalibration(ADC1); //����ADУ׼
	while(ADC_GetCalibrationStatus(ADC1)); //�ȴ�ADУ׼����

}

uint16_t Get_Adc(uint8_t ch)
{
	//????ADC ??????,??????????????
	ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_239Cycles5 );
	//ͨ��ch �������˳��1 ����ʱ��239.5����
	ADC_SoftwareStartConvCmd(ADC1, ENABLE); //
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));//
	return ADC_GetConversionValue(ADC1); //����ADC1ת�����
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
//  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);//TIM3��
//  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|
//                         RCC_APB2Periph_AFIO,ENABLE);//GPIOA��
// 
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;  //����PA6
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //���
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
//  TIM_TimeBaseStructure.TIM_Period = arr; //�Զ�װ������ֵ
//  TIM_TimeBaseStructure.TIM_Prescaler = psc; //����Ԥ��Ƶֵ
//  TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�
//  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //���ϼ���ģʽ
//  TIM_TimeBaseInit(TIM3,&TIM_TimeBaseStructure); //TIM3
// 
//  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //PWMģʽ2
//  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
//  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //������Ը�
//  TIM_OC1Init(TIM3,&TIM_OCInitStructure); //TIM3
// 
//  TIM_OC1PreloadConfig(TIM3,TIM_OCPreload_Enable); //ʹ��Ԥװ�ؼĴ���
//  TIM_Cmd(TIM3,ENABLE); //ʹ��TIM3
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
        RCC_ADCCLKConfig(RCC_PCLK2_Div6);//����ADCʱ��(ADCCLK) 72MHZ/6 = 12MHZ
        
        ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;//����ģʽ
        ADC_InitStructure.ADC_ScanConvMode = DISABLE;//����ģʽ(��ͨ��) ����
        ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;//����ģʽ
        ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T2_CC2;//ת����TIM2 CC2 trig
        ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//ADC�����Ҷ���
        ADC_InitStructure.ADC_NbrOfChannel = 1;//����ת����ADC��ͨ����Ŀ,ȡֵ��Χ1-16
        ADC_Init(ADC1,&ADC_InitStructure);//��ʼ������
   
        
        
       // ADC_RegularChannelConfig(ADC1, ADC_Channel_12, 2, ADC_SampleTime_7Cycles5);
	
      ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_7Cycles5);
	    // ADC_RegularChannelConfig(ADC1, ADC_Channel_8, 1, ADC_SampleTime_7Cycles5);
	
		//		ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 2, ADC_SampleTime_7Cycles5);
		//		ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 3, ADC_SampleTime_7Cycles5);
        
        ADC_DMACmd(ADC1, ENABLE);//enable DMA

        ADC_Cmd(ADC1,ENABLE);// enable adc
        
        ADC_ResetCalibration(ADC1);
        while( ADC_GetResetCalibrationStatus(ADC1));//��ȡADC����У׼�Ĵ�����״̬ �ȴ���λ����
        ADC_StartCalibration(ADC1);//��ʼָ��ADC��У׼״̬
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
   DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&(ADC1->DR);     //DMA��Ӧ���������ַ
   DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&ADCConvertedValue;
   DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
   DMA_InitStructure.DMA_BufferSize = 60000;
   DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
   DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;//  �ڴ�����
   DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;//16λ
   DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;//16λ
   DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;//DMA normal
   DMA_InitStructure.DMA_Priority = DMA_Priority_High;
   DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
   DMA_Init(DMA1_Channel1, &DMA_InitStructure);
   
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;     // �����ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;            // �����ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

   DMA_ITConfig(DMA1_Channel1,DMA_IT_TC, ENABLE);//ʹ�ܻ�ʧ��ָ����ͨ��x�ж�(DMA1 ͨ��1)


   /* Enable DMA1 channel1 */
   DMA_Cmd(DMA1_Channel1, ENABLE);

}

void TIM2_Configration(void)
{
        TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
        TIM_OCInitTypeDef TIM_OCInitStructure;
        //TIM_DeInit(TIM2);
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 , ENABLE);
        
        TIM_TimeBaseStructure.TIM_Period=(72 - 1); //72                                              /*  �Զ���װ�ؼĴ������ڵ�ֵ������ֵ��*/
        /* �ۼ�TIM_Period��Ƶ�ʺ����һ�����»����ж�*/
        TIM_TimeBaseStructure.TIM_Prescaler= (10 - 1);                                    // Ԥ��Ƶ��72000000/3200 = 22500 = 225 * 100
        TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;                 /* ������Ƶ*/
        TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; /* ���ϼ���ģʽ*/
        TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
        TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
        
        TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;//PWMģʽ1
        TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;//ʹ��

        TIM_OCInitStructure.TIM_Pulse = 50;//�����ȣ����������ռ�ձ�
        TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;//LOW��ƽ��Ч

        TIM_OC2Init(TIM2,&TIM_OCInitStructure);//��ʼ��
                
        TIM_Cmd(TIM2,ENABLE);
        
        TIM_InternalClockConfig(TIM2);
        TIM_OC2PreloadConfig(TIM2,TIM_OCPreload_Enable);
        TIM_UpdateDisableConfig(TIM2,DISABLE);
        
        
}

//ADC1  
//???????????,??DMA1ͨ������
//ͨ��0 
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
    ADC_InitStructure.ADC_ScanConvMode =DISABLE; //����ģʽ(��ͨ��) ����
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE; //?????????????  
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T2_CC2;//ת����TIM2 CC2 trig 
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;  //ADC�Ҷ���  
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
        while( ADC_GetResetCalibrationStatus(ADC1));//��ȡADC����У׼�Ĵ�����״̬ �ȴ���λ����
        ADC_StartCalibration(ADC1);//��ʼָ��ADC��У׼״̬
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
//???????????,??DMA1ͨ������ 
//ͨ��8  
//�ܽ�PB0 
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
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;//ת����TIM2 CC2 trig 
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
        while( ADC_GetResetCalibrationStatus(ADC2));//��ȡADC����У׼�Ĵ�����״̬ �ȴ���λ����
        ADC_StartCalibration(ADC2);//��ʼָ��ADC��У׼״̬
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
        
        //����ʱ��
        RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);                              //ʹ��DMA����
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1|RCC_APB2Periph_ADC2,ENABLE);   //ʹ��ADC����        
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB,ENABLE); //ʹ��GPIO����

        
        //GPIOA��Ϊģ��ͨ����������
        GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0;
        GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AIN;//ģ������
        GPIO_Init(GPIOA,&GPIO_InitStructure);      //��ʼ��GPIOA       
        
  //GPIOB��Ϊģ��ͨ����������
        GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0;
        GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AIN;//ģ������
        GPIO_Init(GPIOB,&GPIO_InitStructure);      //��ʼ��GPIOB        
        
        ADC_DeInit(ADC1);                 //��λADC1
        RCC_ADCCLKConfig(RCC_PCLK2_Div6); //����ADCʱ�ӣ�72/6=12MHz

  //-----------ADC1��������----------------------------------
        ADC_InitStructure.ADC_Mode=ADC_Mode_RegSimult;      //ADCͬ������ģʽ
        ADC_InitStructure.ADC_ScanConvMode=DISABLE;          //��ͨ��ģʽ
        ADC_InitStructure.ADC_ContinuousConvMode=DISABLE;    //����ת��ģʽ
        ADC_InitStructure.ADC_ExternalTrigConv=ADC_ExternalTrigConv_T2_CC2;//ת������������������ⲿ����
        ADC_InitStructure.ADC_DataAlign=ADC_DataAlign_Right;//ADC�����Ҷ���
        ADC_InitStructure.ADC_NbrOfChannel=1;               //˳����й���ת����ADCͨ����Ŀ1
        ADC_Init(ADC1,&ADC_InitStructure);                  //��ʼ��ADC1

        ADC_RegularChannelConfig(ADC1,ADC_Channel_0,1,ADC_SampleTime_7Cycles5);  //ADC_Test1����        
    //    ADC_RegularChannelConfig(ADC1,ADC_Channel_6, 2,ADC_SampleTime_71Cycles5);  //ADC_Test2����        
    //    ADC_RegularChannelConfig(ADC1,ADC_Channel_4, 3,ADC_SampleTime_71Cycles5);  //ADC_Test3����        
    //    ADC_RegularChannelConfig(ADC1,ADC_Channel_2, 4,ADC_SampleTime_71Cycles5);  //ADC_Test4����        
        
  //-----------ADC2��������----------------------------------
        ADC_InitStructure.ADC_Mode=ADC_Mode_RegSimult;      //ADCͬ������ģʽ
        ADC_InitStructure.ADC_ScanConvMode=DISABLE;          //��ͨ��ģʽ
        ADC_InitStructure.ADC_ContinuousConvMode=DISABLE;    //����ת��ģʽ
        ADC_InitStructure.ADC_ExternalTrigConv=ADC_ExternalTrigConv_T2_CC2;//ת������������������ⲿ����
        ADC_InitStructure.ADC_DataAlign=ADC_DataAlign_Right;//ADC�����Ҷ���
        ADC_InitStructure.ADC_NbrOfChannel=1;               //˳����й���ת����ADCͨ����Ŀ4
        ADC_Init(ADC2,&ADC_InitStructure);                  //��ʼ��ADC1

        ADC_RegularChannelConfig(ADC2,ADC_Channel_8 ,1,ADC_SampleTime_7Cycles5);  //FreeBack1����        
    //    ADC_RegularChannelConfig(ADC2,ADC_Channel_7 , 2,ADC_SampleTime_71Cycles5);  //FreeBack2����        
    //    ADC_RegularChannelConfig(ADC2,ADC_Channel_5,  3,ADC_SampleTime_71Cycles5);  //FreeBack3����        
    //    ADC_RegularChannelConfig(ADC2,ADC_Channel_3,  4,ADC_SampleTime_71Cycles5);  //FreeBack4����        
        
        ADC_ExternalTrigConvCmd(ADC2, ENABLE);                                     //ʹ��ADC2�ⲿ����ת��
        
  //------------DMA��������----------------------------------
        DMA_DeInit(DMA1_Channel1);                                 //��DMA��ͨ��x�Ĵ�������Ϊȱʡֵ
        DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&ADC1->DR; //DMA�������ַ
        DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&ADCConvertedValue;    //DMA�ڴ����ַ
        DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;         //���ݴ��䷽�򣬴��ڴ��ȡ
        DMA_InitStructure.DMA_BufferSize = 60000;                      //DMAͨ����DMA����Ĵ�С
        DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;             //�����ַ�Ĵ�������
        DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                      //�ڴ��ַ�Ĵ�������
        DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;      //���ݿ��Ϊ32λ
        DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;              //���ݿ��Ϊ32λ
        DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;          //������ѭ������ģʽ
        DMA_InitStructure.DMA_Priority = DMA_Priority_High;     //DMAͨ�� xӵ���е����ȼ� 
        DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;            //DMAͨ��xû������Ϊ�ڴ浽�ڴ洫��
        DMA_Init(DMA1_Channel1, &DMA_InitStructure);            //����DMA_InitStruct��ָ���Ĳ�����ʼ��DMA��ͨ��USART1_Tx_DMA_Channel����ʶ�ļĴ���
        
//		NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn;
//		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;     // �����ȼ�0
//		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;            // �����ȼ�0
//		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//		NVIC_Init(&NVIC_InitStructure);

//	    DMA_ITConfig(DMA1_Channel1,DMA_IT_TC, ENABLE);//ʹ�ܻ�ʧ��ָ����ͨ��x�ж�(DMA1 ͨ��1)
   
        DMA_Cmd(DMA1_Channel1,ENABLE);                          //��DMA����
        
        ADC_Cmd(ADC1,ENABLE);                 //ʹ��ADC1
        ADC_Cmd(ADC2,ENABLE);                 //ʹ��ADC2
        
        ADC_ResetCalibration(ADC1);                 //������λУ׼
        while(ADC_GetResetCalibrationStatus(ADC1)); //�ȴ���λУ׼����
        ADC_StartCalibration(ADC1);                 //����ADУ׼
        while(ADC_GetCalibrationStatus(ADC1));      //�ȴ�У׼����
        
        ADC_ResetCalibration(ADC2);                 //������λУ׼
        while(ADC_GetResetCalibrationStatus(ADC2)); //�ȴ���λУ׼����
        ADC_StartCalibration(ADC2);                 //����ADУ׼
        while(ADC_GetCalibrationStatus(ADC2));      //�ȴ�У׼����
        
        
        ADC_DMACmd(ADC1, ENABLE);                      //ADC1 DMA�������ݴ� 
        ADC_SoftwareStartConvCmd(ADC1,ENABLE);         //ʹ��ָ����ADC1�����ת������
}