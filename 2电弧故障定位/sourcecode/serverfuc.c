//���ܺ�����
#include "servertask.h"
//TCP������� �������鼰���ݸ��� ����CRCУ��ֵ
//�ֽ��ۼ����ȡ��
extern ARM_DRIVER_UART *ServerDev;
uint8_t return_CRC(e_SERV_RETURN *struc)
{	
  uint8_t i;
	uint16_t value,sum=0;
  sum+=Tranversel16(struc->pro_com.device);//����ʱ��С����ת�� 
	sum+=struc->pro_com.cmd;
	for(i=0;i<7;i++)
	{
	 sum+=struc->pro_com.reserve[i];
	}
	sum+=struc->stacode;
	value=(uint8_t)(~(sum&0x00ff));
	return value;
}

uint8_t MODESET_CRC(e_SERV_TCPmode *stru)
{	
  uint8_t i;
	uint16_t value,sum=0;
	sum+=stru->pro_com.device;
	sum+=stru->pro_com.cmd;
	for(i=0;i<7;i++)
	{
	 sum+=stru->pro_com.reserve[i];
	}
	sum+=stru->stacode;
	sum+=stru->Cdevice;
	value=(uint8_t)(~(sum&0x00ff));
	return value;
}
//��������������
void server_send_len(char *send_string,uint16_t number)
{
  ServerDev->WriteData((uint8_t *)send_string,number); 
	while(!ServerDev->TxDone);
}
//��������������
void server_send_string(char *send_string)
{
  ServerDev->WriteData((uint8_t *)send_string,strlen(send_string)); 
	while(!ServerDev->TxDone);
}

//��������������
int32_t server_rec_string(uint8_t *rec_string)
{
	uint8_t rec_num; 
  rec_num=ServerDev->DataAvailable();
	ServerDev->ReadData(rec_string,rec_num);	
	return rec_num;
}

uint8_t SERV_recv_data(void)
{
  uint8_t return_char;
	ServerDev->ReadData(&return_char, 1);
	return return_char;
}

//������麯��
void Clear_arry(uint8_t *array, uint8_t num)
{
	uint8_t i;
	for(i=0;i<num;i++)
	 *(array+i)=0;
}
//���͸�TCP�ж�����֡�е�7E 7D 7C����ת����2�ֽ����� 
uint8_t *GPRS_frame(uint8_t arry[],uint8_t number)
{
	uint8_t i=0;
	uint8_t j=0;
	uint8_t *arr;
	arr[j]=arry[i];
  for(i=1,j=1;i<number;i++,j++)
	{
		arr[j]=arry[i];
	  switch(arry[i])
		{			
			case 0x7E:
				arry[j]=0x7c;
				j+=1;
				arry[j]=0x5e;
			   break;	
	    case 0x7D:
				 arry[j]=0x7c;
				 j+=1;
				 arry[j]=0x5d;
			   break;	
	    case 0x7C:
				 arry[j]=0x7c;
				 j+=1;
				 arry[j]=0x5c;
			   break;	
	  }   		
	}
	return arr; 
}
//���յ�TCO��������֡��0X7C�����ݺͺ������ݽ��
//�жϽ��������Ƿ���ȷ
void TCP_frame(uint8_t *arry)
{
	uint8_t i,j;
  for(i=0,j=0;arry[i]!=0x7d;i++,j++)
	{
		arry[j]=arry[i];
		if(arry[i]==0x7c)
		{
	    switch(arry[i+1])
			{
				case 0x5e:
					arry[j]=0x7e;
					i+=1;
					break;
		    case 0x5d:
					arry[j]=0x7d;
					i+=1;
					break;
				case 0x5c:
					arry[j]=0x7c;
					i+=1;
					break;
		  }
	  }
	}
}
//��ָ�������ݸ�������
void PTR_arry(uint8_t array[],uint8_t *pointer)
{
	uint8_t i;
	for(i=0;*(pointer+i)!='\0';i++)
	{
    array[i]=*(pointer+i);
	}
}

//��ȡIP��ַ �����ַ����е������ַ����临�Ƶ���������´���
//void GET_Message(char *REC_string, Scomp_String CMP)
void GET_IPMessage(char *REC_string,uint8_t IP_address[])
{
	char *ptr_start,*ptr_end;
	char temp[5];
	uint8_t i,j,k,data;
	//��ʼ���ҵ��ַ�':'���׵�ַ
  ptr_start=strchr(REC_string ,'\n');   //��һ�����в��Ҹ����ַ��ĵ�һ��ƥ��֮��
  //�ҵ�<CR>��β���������õ���Ϣ��ֵ��������
	ptr_end=strrchr(REC_string ,'\r');    //�ڴ��в���ָ���ַ������һ������
  for(i=1; ;i++)
	{
    if((*(ptr_start+i)>='0')&&(*(ptr_start+i)<='9'))
		{
	    ptr_start=ptr_start+i;
			break;
	  }
	}	
	for(i=0,j=0,k=0;(ptr_start+i)<(ptr_end+1);i++)
	{
		if(*(ptr_start+i)!='.')
		{
			temp[k]=*(ptr_start+i);
			if(temp[k]==0x0d)
			{
				 temp[k]='\0';
				 data=atoi(temp);
				 IP_address[j]=data;
				 k=0;	
				 j++; 
		  }
			k++;
		}
		else 
		{
			temp[k]='\0';
			data=atoi(temp);
			IP_address[j]=data;
	    k=0;	
      j++;			
	  }
	}
}

//��ȡ����汾�� 
void GET_SOFT(char *REC_string,char soft_mess[])
{
	char *ptr_start,*ptr_end;
	char temp[5];
	uint8_t i,j,k,data;
	if(strstr(REC_string, "Revision"))
	{
		ptr_start=strchr(REC_string ,'\r');   //��һ�����в��Ҹ����ַ��ĵ�һ��ƥ��֮��
		*ptr_start=0x30;
		//��ʼ���ҵ��ַ�':'���׵�ַ
		ptr_start=strchr(REC_string ,':');   //��һ�����в��Ҹ����ַ��ĵ�һ��ƥ��֮��
		//�ҵ�<CR>��β���������õ���Ϣ��ֵ��������
		ptr_end=strchr(REC_string ,'\r');    //�ڴ��в���ָ���ַ������һ������
		for(i=1;(ptr_start+i)<(ptr_end);i++)
		{
			soft_mess[i-1]=*(ptr_start+i);
		}	
	}
}
//��ȡGPRS״̬ 
void GET_STATE(char *REC_string,char state_mess[])
{
	char *ptr_start,*ptr_end;
	char temp[5];
	uint8_t i,j,k,data;
	if(strstr(REC_string, "STATE"))
	{
		//��ʼ���ҵ��ַ�':'���׵�ַ
		ptr_start=strchr(REC_string ,':');   //��һ�����в��Ҹ����ַ��ĵ�һ��ƥ��֮��
		//�ҵ�<CR>��β���������õ���Ϣ��ֵ��������
		ptr_end=strrchr(REC_string ,'\r');    //�ڴ��в���ָ���ַ������һ������
		for(i=1;(ptr_start+i)<(ptr_end);i++)
		{
			state_mess[i-1]=*(ptr_start+i);
		}	
	}
}
//��ȡӲ���汾�� 
void GET_HARD(char *REC_string,char hard_mess[])
{
	char *ptr_start,*ptr_end;
	char temp[5];
	uint8_t i,j,k,data;
	if(strstr(REC_string, "OK"))
	{
		//��ʼ���ҵ��ַ�':'���׵�ַ
		ptr_start=strchr(REC_string ,'\r');   //��һ�����в��Ҹ����ַ��ĵ�һ��ƥ��֮��
		*ptr_start=0x30;
		ptr_start=strchr(REC_string ,'\n'); 
		//�ҵ�<CR>��β���������õ���Ϣ��ֵ��������
		ptr_end=strchr(REC_string ,'\r');    //�ڴ��в���ָ���ַ������һ������
		for(i=1;(ptr_start+i)<(ptr_end);i++)
		{
			hard_mess[i-1]=*(ptr_start+i);
		}	
	}
}

