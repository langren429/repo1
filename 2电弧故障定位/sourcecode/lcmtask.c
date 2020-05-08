/*
 * (C) Copyright 2013 Thingtek
 *
 * Written by: randy <shenminghua@thingtek.com>
 */
 

#include "lcmtask.h"
#include "jlx12864g.h"
#include "common.h"
#include <stdio.h> 

static uint8_t IsKeyDown_Down(void) {if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) == Bit_SET) return 1; return 0;}
static uint8_t IsKeyDown_Up(void)   {if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_7) == Bit_SET) return 1; return 0;}

extern	s_GATEWAY_DEVICE gateway_dev;
extern e_KEY_TYPE KEY_TYPE;
char stra1[20];
char stra3[20];

char strb1[20];
char strb2[20];
char strb3[20];

char *a1 = "��ǰֵ";  
char *a2 = "�ֶ�����        ";
char *a3 = "��λ";
char *a4 = "����            ";

char *b1 = "������";  
char *b2 = "վ��";
char *b3 = "�ɼ����";
char *b4 = "�˳�            ";
/*float current_value = 1;
int Baudrate = 4096;
int endpoint_num = 0;
int Collection_interval = 60;
float max_value = 906.1;*/
e_SET_STATUS 	SET_STATUS;
s_LCM_SET LCM_SET;
//u8 move_num;
Menu MainMenu = { //�������˵�
 0,3,4,0,0,//Ĭ����ʾ0-3��,�ܹ�4��,��ǰѡ��0���ǰҳ���
  {
  "����1          ",
  "����2          ",
  "����ָ���    ",
  "��ѯ           "
  }
};

Menu searchMenu = {//��ѯ�˵�
 0,3,4,0,1,
 {
  "���˼�¼��ϸ    ",
  "δ�ɼ���¼��    ",
  "�豸����        ",
  "�˳�            "
 }
};
Menu *currentMenu;//��ǰ�Ĳ˵�

int find_number(int num,int site)//num ������ site �ڼ�λ1 2 3 4  ������
{
	int i = 0;
	site--;
	
	for(i = 0;i < site;i ++)
	{
		num /= 10;
	}
	return num % 10;
}

int set_number(int num,int total_site,int site,int bit_num)//num ������ total_site ��λ�� site �ڼ�λ1 2 3 4  ��Ҫ�ı���� ������
{
	int index1[10];
	int x = 0,j = 0,k,tempx = 0;
	int temp_site = total_site;
	site--;
	
  while(temp_site--)
	{
		index1[x] = num % 10;
		num /= 10;
		x ++;
	}
	index1[site] = bit_num;
	temp_site = total_site;
  for(x = 0;x < total_site;x ++)
	{
		tempx += index1[--temp_site];
		tempx *= 10;
	}
	
	return tempx/10;
}

void func(void)
{

//printf("hello\r\n");
}


//��ʾ�˵� site 0--15 ��16*16��������
void display(u8 line) //��ʾ�˵�������ѡ����� �Ƿ���˸ 1 ��˸ ��˸λ�ôӺ���ǰ 0��ʼ
{ 
 int i,j,bit_num;
 char shanshuo_flag;
 u8 move_num = 4;
 line = 3-(currentMenu->range_to-line);
 clear_screen();
 for(i = 0;i<4;i++)
 {
	 if(line == i)
	 {
		 j = 1;
	 }
	 else
	 {
		 j = 0;
	 }
	 display_GB2312_string(1 + 2*i,1,j,currentMenu->menuItems[i+currentMenu->range_from]);
 }

}

void func_Baudrate(void)//�����ʴ�����
{
	
	int i,j,bit_num;
	u8 line = 0;//��ǰ���׵��� 
	u8 site = 15;//��ǰ��˸��λ��
	char shanshuo_flag;
  line = 3-(currentMenu->range_to-line);
  clear_screen();
	
		while(1)//������״̬ 
		{
			shanshuo_flag = !shanshuo_flag;
			clear_screen();
			for(i = 0;i<4;i++)
		 {

			 if(line == i)
			 {
				 j = 1;
				 display_GB2312_string_shanshuo(1 + 2*i,1,j,shanshuo_flag,site,currentMenu->menuItems[i+currentMenu->range_from]);
			 }
			 else
			 {
				 j = 0;
				 display_GB2312_string(1 + 2*i,1,j,currentMenu->menuItems[i+currentMenu->range_from]);
			
			 } 

		 }
			 if(KEY_TYPE == KEY_NON)
			{
				osDelay(500);
				continue;
			}
			else if(KEY_TYPE == KEY_S2_UP)
			{
					if(LCM_SET.Baudrate == 4096)
					{
						LCM_SET.Baudrate = 9600;
					}
					else if(LCM_SET.Baudrate == 9600)
					{
						LCM_SET.Baudrate = 115200;
					}
					else if(LCM_SET.Baudrate == 115200)
					{
						LCM_SET.Baudrate = 4096;
					}
			
			}
			else if(KEY_TYPE == KEY_S3_DOWN)
			{
					if(LCM_SET.Baudrate == 115200)
					{
						LCM_SET.Baudrate = 9600;
					}
					else if(LCM_SET.Baudrate == 9600)
					{
						LCM_SET.Baudrate = 4096;
					}
					else if(LCM_SET.Baudrate == 4096)
					{
						LCM_SET.Baudrate = 115200;
					}
				
			}
			else if(KEY_TYPE == KEY_S1_OK)
			{
				break;	
			}
      KEY_TYPE = KEY_NON;
			memset(strb1,'\0',20);
	    sprintf(strb1,"%s    %6d",b1,LCM_SET.Baudrate);
		  searchMenu.menuItems[0] = strb1;
		}
		KEY_TYPE = KEY_NON;
		SET_STATUS = SET_STATUS_NO;
	
		display(currentMenu->selected);

}

//��ʾ�˵� site 0--15 ��16*16�������� 
//վ�����ô�����
void func_endpoint_num() //��ʾ�˵�������ѡ����� �Ƿ���˸ 1 ��˸ ��˸λ�ôӺ���ǰ 0��ʼ
{ 
 int i,j,bit_num;
 char shanshuo_flag;
 u8 line = 1;//��ǰ���׵��� 
 u8 site = 15;//��ǰ��˸��λ��
 u8 move_num = 2; //���ƶ�2λ 
 line = 3-(currentMenu->range_to-line);
 clear_screen();


		while(1)//������״̬ 
		{
			shanshuo_flag = !shanshuo_flag;
			clear_screen();
			for(i = 0;i<4;i++)
		 {

			 if(line == i)
			 {
				 j = 1;
				 display_GB2312_string_shanshuo(1 + 2*i,1,j,shanshuo_flag,site,currentMenu->menuItems[i+currentMenu->range_from]);
			 }
			 else
			 {
				 j = 0;
				 display_GB2312_string(1 + 2*i,1,j,currentMenu->menuItems[i+currentMenu->range_from]);
			
			 } 

		 }
			 if(KEY_TYPE == KEY_NON)
			{
				osDelay(500);
				continue;
			}
			else if(KEY_TYPE == KEY_S2_UP)
			{
				
      	if(site == (16 - move_num))
				{
					site = 15;
				}
				else
				{
					site --;
				}

			}
			else if(KEY_TYPE == KEY_S3_DOWN)
			{
				
				bit_num = find_number(LCM_SET.endpoint_num,16 - site);
				if(bit_num == 9)
				{
					bit_num = 0;
				}
				else
				{
					bit_num++;
				}
				
				LCM_SET.endpoint_num =  set_number(LCM_SET.endpoint_num,2,16 - site,bit_num);//2λ��
				
			}
			else if(KEY_TYPE == KEY_S1_OK)
			{
				break;	
			}
      KEY_TYPE = KEY_NON;
			memset(strb2,'\0',20);
			sprintf(strb2,"%s          %02d",b2,LCM_SET.endpoint_num);
			searchMenu.menuItems[1] = strb2;
		}
		KEY_TYPE = KEY_NON;
		SET_STATUS = SET_STATUS_NO;

		
		display(currentMenu->selected);

}

//��ʾ�˵� site 0--15 ��16*16��������
//�ɼ����������
void func_Collection_interval() //��ʾ�˵�������ѡ����� �Ƿ���˸ 1 ��˸ ��˸λ�ôӺ���ǰ 0��ʼ
{ 
 int i,j,bit_num;
 char shanshuo_flag;
 u8 line = 2;//��ǰ���׵��� 
 u8 site = 14;//��ǰ��˸��λ��
 u8 move_num = 3; //���ƶ�3λ 
 line = 3-(currentMenu->range_to-line);
 clear_screen();


		while(1)//������״̬ 
		{
			shanshuo_flag = !shanshuo_flag;
			clear_screen();
			for(i = 0;i<4;i++)
		 {

			 if(line == i)
			 {
				 j = 1;
				 display_GB2312_string_shanshuo(1 + 2*i,1,j,shanshuo_flag,site,currentMenu->menuItems[i+currentMenu->range_from]);
			 }
			 else
			 {
				 j = 0;
				 display_GB2312_string(1 + 2*i,1,j,currentMenu->menuItems[i+currentMenu->range_from]);
			
			 } 

		 }
			 if(KEY_TYPE == KEY_NON)
			{
				osDelay(500);//��˸Ƶ��
				continue;
			}
			else if(KEY_TYPE == KEY_S2_UP)
			{
				
      	if(site == (15 - move_num))
				{
					site = 14;
				}
				else
				{
					site --;
				}

			}
			else if(KEY_TYPE == KEY_S3_DOWN)
			{
				
				bit_num = find_number(LCM_SET.Collection_interval,15 - site);
				if(bit_num == 9)
				{
					bit_num = 0;
				}
				else
				{
					bit_num++;
				}
				
				LCM_SET.Collection_interval =  set_number(LCM_SET.Collection_interval,3,15 - site,bit_num);//3λ��
				
			}
			else if(KEY_TYPE == KEY_S1_OK)
			{
				break;	
			}
      KEY_TYPE = KEY_NON;
			memset(strb3,'\0',20);
			sprintf(strb3,"%s    %03ds",b3,LCM_SET.Collection_interval);
			searchMenu.menuItems[2] = strb3;	
		}
		KEY_TYPE = KEY_NON;
		SET_STATUS = SET_STATUS_NO;

		
		display(currentMenu->selected);

}

//��ʾ�˵� site 0--15 ��16*16��������
//��λ���ô�����
void func_max_value() //��ʾ�˵�������ѡ����� �Ƿ���˸ 1 ��˸ ��˸λ�ôӺ���ǰ 0��ʼ
{ 
 int i,j,bit_num;
 char shanshuo_flag;
 u8 line = 2;//��ǰ���׵��� 
 u8 site = 13;//��ǰ��˸��λ��
 u8 move_num = 4; //���ƶ�4λ 
	int max_value_int;  
 line = 3-(currentMenu->range_to-line);
 clear_screen();


		while(1)//������״̬ 
		{
			shanshuo_flag = !shanshuo_flag;
			clear_screen();
			for(i = 0;i<4;i++)
		 {

			 if(line == i)
			 {
				 j = 1;
				 display_GB2312_string_shanshuo(1 + 2*i,1,j,shanshuo_flag,site,currentMenu->menuItems[i+currentMenu->range_from]);
			 }
			 else
			 {
				 j = 0;
				 display_GB2312_string(1 + 2*i,1,j,currentMenu->menuItems[i+currentMenu->range_from]);
			
			 } 

		 }
			 if(KEY_TYPE == KEY_NON)
			{
				osDelay(500);
				continue;
			}
			else if(KEY_TYPE == KEY_S2_UP)
			{
				
      	if(site == (13 - move_num))
				{
					site = 13;
				}
				else if(site == 13)
				{
					site = 11;//����С����
				}
				else
				{
					site --;
				}

			}
			else if(KEY_TYPE == KEY_S3_DOWN)
			{
				//��float������С������0
				
			//	max_value
				max_value_int = ((int)(LCM_SET.max_value))*100 + ((int)(LCM_SET.max_value *10))%10;
				bit_num = find_number(max_value_int,14 - site);
				if(bit_num == 9)
				{
					bit_num = 0;
				}
				else
				{
					bit_num++;
				}
				
				max_value_int =  set_number(max_value_int,5,14 - site,bit_num);//5λ�� С�����0
				LCM_SET.max_value = ((float)(max_value_int/10 + max_value_int%10))/10;
				
			}
			else if(KEY_TYPE == KEY_S1_OK)
			{
				break;	
			}
      KEY_TYPE = KEY_NON;
			memset(stra3,'\0',20);
			sprintf(stra3,"%s     %05.1fmm",a3,LCM_SET.max_value);
			MainMenu.menuItems[2] = stra3;
		}
		KEY_TYPE = KEY_NON;
		SET_STATUS = SET_STATUS_NO;

		display(currentMenu->selected);

}


//��ʾ�˵� site 0--15 ��16*16��������
//��λ���ô�����
void func_set_limit() //��ʾ�˵�������ѡ����� �Ƿ���˸ 1 ��˸ ��˸λ�ôӺ���ǰ 0��ʼ
{ 
 int i,j,k,bit_num;
 char shanshuo_flag;
 u8 line = 1;//��ǰ���׵��� 
 //u8 site = 13;//��ǰ��˸��λ��
// u8 move_num = 4; //���ƶ�4λ 
	int max_value_int;  
 line = 3-(currentMenu->range_to-line);
 clear_screen();


		while(1)//������״̬ 
		{
			//////////////////////////��ʾ����/////////////////////////////////
			shanshuo_flag = !shanshuo_flag;
			clear_screen();
			k = !k;
			for(i = 0;i<4;i++)
		 {
       if(k == 0)
			 {
				 display_GB2312_string(1 + 2*i,1,0,currentMenu->menuItems[i+currentMenu->range_from]);
			 }
			 else
			 {
				 if(line == i)
				 {
					 j = 1;
						display_GB2312_string(1 + 2*i,1,j,currentMenu->menuItems[i+currentMenu->range_from]);
					 
				 }
				 else
				 {
					 j = 0;
					 display_GB2312_string(1 + 2*i,1,j,currentMenu->menuItems[i+currentMenu->range_from]);
				
				 } 
			 }

		 }
		 //////////////////////////��ʾ����/////////////////////////////////
			 if(KEY_TYPE == KEY_NON)//�а�������ʱKEY_TYPE���ж��ﱻ�ı�
			{
				osDelay(300);
				continue;
			}
			else if(KEY_TYPE == KEY_S1_OK)
			{
				break;	
			}
			while(IsKeyDown_Down())
			{
				GPIO_PinWrite(GPIOB, 12, 0);	//LED_TX ����Ч
			}
			
			while(IsKeyDown_Up())
			{
				GPIO_PinWrite(GPIOB, 13, 0);	//LED_TX ����Ч
			}
			GPIO_PinWrite(GPIOB, 12, 1);	//LED_TX ����Ч
	    GPIO_PinWrite(GPIOB, 13, 1);//LED_RX �͵�ƽ��Ч
		/*	if(IsKeyDown_Down() == 1)
			{
				
				//�¼�����δ�ɿ�  �ɿ��˳�ѭ��
				GPIO_PinWrite(GPIOB, 12, 0);	//LED_TX ����Ч
	     
				
			}
			else if(IsKeyDown_Up() == 1)
			{
				
				//�¼�����δ�ɿ�  �ɿ��˳�ѭ��
				GPIO_PinWrite(GPIOB, 13, 0);//LED_RX �͵�ƽ��Ч
				
			}
			else if((IsKeyDown_Down() == 0)&&(IsKeyDown_Up() == 0))
			{
				//�������ɿ� 
				GPIO_PinWrite(GPIOB, 12, 1);	//LED_TX ����Ч
	      GPIO_PinWrite(GPIOB, 13, 1);//LED_RX �͵�ƽ��Ч
			}*/
      KEY_TYPE = KEY_NON;
		}
		KEY_TYPE = KEY_NON;
		SET_STATUS = SET_STATUS_NO;
		
		GPIO_PinWrite(GPIOB, 12, 1);	//LED_TX ����Ч
		GPIO_PinWrite(GPIOB, 13, 1);//LED_RX �͵�ƽ��Ч

		display(currentMenu->selected);

}

//��ʼ���˵�:
void initMenu()
{

 MainMenu.subMenus = malloc(sizeof(&MainMenu)*4);
 MainMenu.subMenus[0] = NULL;//��1��3��û���Ӳ˵���null,ѡ����������func����Ӧ���еĺ���
 MainMenu.subMenus[1] = NULL;
 MainMenu.subMenus[2] = NULL;
 MainMenu.subMenus[3] = &searchMenu;//�������ѯ���Ӳ˵�
 MainMenu.func = malloc(sizeof(&func)*4);
 MainMenu.func[0] = NULL;  //��ǰ����ֵ
 MainMenu.func[1] = &func_set_limit; //�ֶ�����
 MainMenu.func[2] = &func_max_value;//��λ
 MainMenu.func[3] = NULL;//����
 MainMenu.parent = NULL;//��ʾ�Ƕ����˵�
//���˵�	
	memset(stra1,'\0',20);
	sprintf(stra1,"%s   %05.1fmm",a1,LCM_SET.current_value);
  MainMenu.menuItems[0] = stra1; //��ǰֵ
  MainMenu.menuItems[1] = a2;  //�ֶ�����
	memset(stra3,'\0',20);
	sprintf(stra3,"%s     %05.1fmm",a3,LCM_SET.max_value);
  MainMenu.menuItems[2] = stra3;//��λ
  MainMenu.menuItems[3] = a4;  //����
//���ò˵�	
	memset(strb1,'\0',20);
	sprintf(strb1,"%s    %6d",b1,LCM_SET.Baudrate);
  searchMenu.menuItems[0] = strb1;//������
	memset(strb2,'\0',20);
	sprintf(strb2,"%s          %02d",b2,LCM_SET.endpoint_num);
  searchMenu.menuItems[1] = strb2;//վ��
	memset(strb3,'\0',20);
	sprintf(strb3,"%s    %03ds",b3,LCM_SET.Collection_interval);
  searchMenu.menuItems[2] = strb3;//�ɼ����	
	searchMenu.menuItems[3] = b4;
 
 searchMenu.subMenus = malloc(sizeof(&searchMenu)*4);
 searchMenu.subMenus[0] = searchMenu.subMenus[1] = searchMenu.subMenus[2] = searchMenu.subMenus[3]  =  NULL;
 searchMenu.func = malloc(sizeof(&func)*4);//����
 searchMenu.func[0] = &func_Baudrate;//������
 searchMenu.func[1] = &func_endpoint_num;//վ��
 searchMenu.func[2] = &func_Collection_interval;//�ɼ����
 searchMenu.func[3] = NULL;//�˳�
 //searchMenu.func[0] = searchMenu.func[2] = searchMenu.func[2] = searchMenu.func[3] = searchMenu.func[4] = NULL;
 searchMenu.parent = &MainMenu;//��һ���˵���MainMenu.�����ѯ�Ӳ˵��󰴷��ؼ�,������ʾ����˵���
 
 currentMenu = &MainMenu;
 
}

void lcm_task(void const *argument){
 
	
	gateway_dev.lcm_task_id = osThreadGetId();
	while(gateway_dev.lcm_task_id == NULL);
	initMenu();
	display(currentMenu->selected);
	while(1){

//	  osDelay(6000);	//��ʱ�ȴ�6S �൱�ڶ�ʱ��
		if(KEY_TYPE == KEY_NON)
		{
			osDelay(10);
			continue;
		}
		if(KEY_TYPE == KEY_S2_UP)
		{
			
				if(currentMenu->selected > 0)//��ǰѡ���Ϊ��һ��
				{
					 currentMenu->selected--;
					 if(currentMenu->selected < currentMenu->range_from)//???????
					 {
						currentMenu->range_from = currentMenu->selected;
						currentMenu->range_to = currentMenu->range_from+3;
					 }
					 display(currentMenu->selected);
				}
				else if(currentMenu->selected == 0)
				{
					 currentMenu->selected = currentMenu->itemCount - 1;
					if(currentMenu->selected>currentMenu->range_to)
					{
						currentMenu->range_from = currentMenu->selected - 3;
					  currentMenu->range_to = currentMenu->selected;
					}

					 display(currentMenu->selected);
				}

				
		}
		else if(KEY_TYPE == KEY_S3_DOWN)
		{
				if(currentMenu->selected < currentMenu->itemCount-1)//��ǰѡ���Ϊ���һ��
				{
					 currentMenu->selected++;
					 if(currentMenu->selected>currentMenu->range_to)
					 {
						currentMenu->range_to = currentMenu->selected;
						currentMenu->range_from = currentMenu->range_to-3;
					 }
					 display(currentMenu->selected);
			  }
				else if(currentMenu->selected == currentMenu->itemCount-1)
				{
					currentMenu->selected = 0;

					currentMenu->range_from = currentMenu->selected;
					currentMenu->range_to = currentMenu->range_from+3;
				 
				 display(currentMenu->selected);
				}
		}
		else if(KEY_TYPE == KEY_S1_OK)
		{
				if(currentMenu->subMenus[currentMenu->selected] !=NULL)
				{
				 currentMenu = currentMenu->subMenus[currentMenu->selected];
				 display(currentMenu->selected);
				}
				else if((currentMenu->current_page == 1)&&(currentMenu->parent!=NULL)&&(currentMenu->selected == 3))//�Ӳ˵�
				{
					 currentMenu = currentMenu->parent;
           display(currentMenu->selected);
				}
				 else if(currentMenu->func[currentMenu->selected] != NULL)//��������״̬ ��ʼ��˸
				 {
					 if(SET_STATUS == SET_STATUS_NO)
					 {
						 KEY_TYPE = KEY_NON;//�������� 
						 SET_STATUS = SET_STATUS_YES;//��������״̬
 
						 currentMenu->func[currentMenu->selected]();// ����������һ��ѭ�� δ�˳�����ģʽʱ ������ѭ���м��
             flash_writedata(LCM_SET_ADDRESS, &(LCM_SET.enable_flag));

					 }
				 }
		}

		KEY_TYPE = KEY_NON;

	}
	
}


