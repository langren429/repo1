//功能函数体
#include "servertask.h"
//TCP求检验码 传递数组及数据个数 返回CRC校验值
//字节累加求和取反
extern ARM_DRIVER_UART *ServerDev;
uint8_t return_CRC(e_SERV_RETURN *struc)
{	
  uint8_t i;
	uint16_t value,sum=0;
  sum+=Tranversel16(struc->pro_com.device);//定义时大小端已转换 
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
//服务器发送数据
void server_send_len(char *send_string,uint16_t number)
{
  ServerDev->WriteData((uint8_t *)send_string,number); 
	while(!ServerDev->TxDone);
}
//服务器发送数据
void server_send_string(char *send_string)
{
  ServerDev->WriteData((uint8_t *)send_string,strlen(send_string)); 
	while(!ServerDev->TxDone);
}

//服务器接收数据
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

//清空数组函数
void Clear_arry(uint8_t *array, uint8_t num)
{
	uint8_t i;
	for(i=0;i<num;i++)
	 *(array+i)=0;
}
//发送给TCP判断数据帧中的7E 7D 7C将其转换成2字节序列 
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
//接收到TCO数据数据帧有0X7C的数据和后面数据结合
//判断接收数据是否正确
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
//将指针内数据赋给数组
void PTR_arry(uint8_t array[],uint8_t *pointer)
{
	uint8_t i;
	for(i=0;*(pointer+i)!='\0';i++)
	{
    array[i]=*(pointer+i);
	}
}

//提取IP地址 查找字符串中的数字字符将其复制到数组后重新处理
//void GET_Message(char *REC_string, Scomp_String CMP)
void GET_IPMessage(char *REC_string,uint8_t IP_address[])
{
	char *ptr_start,*ptr_end;
	char temp[5];
	uint8_t i,j,k,data;
	//初始先找到字符':'的首地址
  ptr_start=strchr(REC_string ,'\n');   //在一个串中查找给定字符的第一个匹配之处
  //找到<CR>结尾处，将有用的信息赋值到新数组
	ptr_end=strrchr(REC_string ,'\r');    //在串中查找指定字符的最后一个出现
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

//提取软件版本号 
void GET_SOFT(char *REC_string,char soft_mess[])
{
	char *ptr_start,*ptr_end;
	char temp[5];
	uint8_t i,j,k,data;
	if(strstr(REC_string, "Revision"))
	{
		ptr_start=strchr(REC_string ,'\r');   //在一个串中查找给定字符的第一个匹配之处
		*ptr_start=0x30;
		//初始先找到字符':'的首地址
		ptr_start=strchr(REC_string ,':');   //在一个串中查找给定字符的第一个匹配之处
		//找到<CR>结尾处，将有用的信息赋值到新数组
		ptr_end=strchr(REC_string ,'\r');    //在串中查找指定字符的最后一个出现
		for(i=1;(ptr_start+i)<(ptr_end);i++)
		{
			soft_mess[i-1]=*(ptr_start+i);
		}	
	}
}
//提取GPRS状态 
void GET_STATE(char *REC_string,char state_mess[])
{
	char *ptr_start,*ptr_end;
	char temp[5];
	uint8_t i,j,k,data;
	if(strstr(REC_string, "STATE"))
	{
		//初始先找到字符':'的首地址
		ptr_start=strchr(REC_string ,':');   //在一个串中查找给定字符的第一个匹配之处
		//找到<CR>结尾处，将有用的信息赋值到新数组
		ptr_end=strrchr(REC_string ,'\r');    //在串中查找指定字符的最后一个出现
		for(i=1;(ptr_start+i)<(ptr_end);i++)
		{
			state_mess[i-1]=*(ptr_start+i);
		}	
	}
}
//提取硬件版本号 
void GET_HARD(char *REC_string,char hard_mess[])
{
	char *ptr_start,*ptr_end;
	char temp[5];
	uint8_t i,j,k,data;
	if(strstr(REC_string, "OK"))
	{
		//初始先找到字符':'的首地址
		ptr_start=strchr(REC_string ,'\r');   //在一个串中查找给定字符的第一个匹配之处
		*ptr_start=0x30;
		ptr_start=strchr(REC_string ,'\n'); 
		//找到<CR>结尾处，将有用的信息赋值到新数组
		ptr_end=strchr(REC_string ,'\r');    //在串中查找指定字符的最后一个出现
		for(i=1;(ptr_start+i)<(ptr_end);i++)
		{
			hard_mess[i-1]=*(ptr_start+i);
		}	
	}
}

