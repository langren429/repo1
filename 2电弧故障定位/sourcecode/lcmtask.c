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

char *a1 = "当前值";  
char *a2 = "手动设限        ";
char *a3 = "限位";
char *a4 = "设置            ";

char *b1 = "波特率";  
char *b2 = "站号";
char *b3 = "采集间隔";
char *b4 = "退出            ";
/*float current_value = 1;
int Baudrate = 4096;
int endpoint_num = 0;
int Collection_interval = 60;
float max_value = 906.1;*/
e_SET_STATUS 	SET_STATUS;
s_LCM_SET LCM_SET;
//u8 move_num;
Menu MainMenu = { //定义主菜单
 0,3,4,0,0,//默认显示0-3项,总共4项,当前选择0项，当前页序号
  {
  "设置1          ",
  "设置2          ",
  "输送指令发送    ",
  "查询           "
  }
};

Menu searchMenu = {//查询菜单
 0,3,4,0,1,
 {
  "记账记录明细    ",
  "未采集记录数    ",
  "设备机号        ",
  "退出            "
 }
};
Menu *currentMenu;//当前的菜单

int find_number(int num,int site)//num 输入数 site 第几位1 2 3 4  返回数
{
	int i = 0;
	site--;
	
	for(i = 0;i < site;i ++)
	{
		num /= 10;
	}
	return num % 10;
}

int set_number(int num,int total_site,int site,int bit_num)//num 输入数 total_site 总位数 site 第几位1 2 3 4  需要改变的数 返回数
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


//显示菜单 site 0--15 对16*16不起作用
void display(u8 line) //显示菜单并设置选中项反白 是否闪烁 1 闪烁 闪烁位置从后往前 0开始
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

void func_Baudrate(void)//波特率处理函数
{
	
	int i,j,bit_num;
	u8 line = 0;//当前反白的行 
	u8 site = 15;//当前闪烁的位置
	char shanshuo_flag;
  line = 3-(currentMenu->range_to-line);
  clear_screen();
	
		while(1)//在设置状态 
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

//显示菜单 site 0--15 对16*16不起作用 
//站号设置处理函数
void func_endpoint_num() //显示菜单并设置选中项反白 是否闪烁 1 闪烁 闪烁位置从后往前 0开始
{ 
 int i,j,bit_num;
 char shanshuo_flag;
 u8 line = 1;//当前反白的行 
 u8 site = 15;//当前闪烁的位置
 u8 move_num = 2; //可移动2位 
 line = 3-(currentMenu->range_to-line);
 clear_screen();


		while(1)//在设置状态 
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
				
				LCM_SET.endpoint_num =  set_number(LCM_SET.endpoint_num,2,16 - site,bit_num);//2位数
				
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

//显示菜单 site 0--15 对16*16不起作用
//采集间隔处理函数
void func_Collection_interval() //显示菜单并设置选中项反白 是否闪烁 1 闪烁 闪烁位置从后往前 0开始
{ 
 int i,j,bit_num;
 char shanshuo_flag;
 u8 line = 2;//当前反白的行 
 u8 site = 14;//当前闪烁的位置
 u8 move_num = 3; //可移动3位 
 line = 3-(currentMenu->range_to-line);
 clear_screen();


		while(1)//在设置状态 
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
				osDelay(500);//闪烁频率
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
				
				LCM_SET.Collection_interval =  set_number(LCM_SET.Collection_interval,3,15 - site,bit_num);//3位数
				
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

//显示菜单 site 0--15 对16*16不起作用
//限位设置处理函数
void func_max_value() //显示菜单并设置选中项反白 是否闪烁 1 闪烁 闪烁位置从后往前 0开始
{ 
 int i,j,bit_num;
 char shanshuo_flag;
 u8 line = 2;//当前反白的行 
 u8 site = 13;//当前闪烁的位置
 u8 move_num = 4; //可移动4位 
	int max_value_int;  
 line = 3-(currentMenu->range_to-line);
 clear_screen();


		while(1)//在设置状态 
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
					site = 11;//跳过小数点
				}
				else
				{
					site --;
				}

			}
			else if(KEY_TYPE == KEY_S3_DOWN)
			{
				//将float数据中小数点变成0
				
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
				
				max_value_int =  set_number(max_value_int,5,14 - site,bit_num);//5位数 小数点变0
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


//显示菜单 site 0--15 对16*16不起作用
//限位设置处理函数
void func_set_limit() //显示菜单并设置选中项反白 是否闪烁 1 闪烁 闪烁位置从后往前 0开始
{ 
 int i,j,k,bit_num;
 char shanshuo_flag;
 u8 line = 1;//当前反白的行 
 //u8 site = 13;//当前闪烁的位置
// u8 move_num = 4; //可移动4位 
	int max_value_int;  
 line = 3-(currentMenu->range_to-line);
 clear_screen();


		while(1)//在设置状态 
		{
			//////////////////////////显示部分/////////////////////////////////
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
		 //////////////////////////显示部分/////////////////////////////////
			 if(KEY_TYPE == KEY_NON)//有按键按下时KEY_TYPE在中断里被改变
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
				GPIO_PinWrite(GPIOB, 12, 0);	//LED_TX 低有效
			}
			
			while(IsKeyDown_Up())
			{
				GPIO_PinWrite(GPIOB, 13, 0);	//LED_TX 低有效
			}
			GPIO_PinWrite(GPIOB, 12, 1);	//LED_TX 低有效
	    GPIO_PinWrite(GPIOB, 13, 1);//LED_RX 低电平有效
		/*	if(IsKeyDown_Down() == 1)
			{
				
				//下键按下未松开  松开退出循环
				GPIO_PinWrite(GPIOB, 12, 0);	//LED_TX 低有效
	     
				
			}
			else if(IsKeyDown_Up() == 1)
			{
				
				//下键按下未松开  松开退出循环
				GPIO_PinWrite(GPIOB, 13, 0);//LED_RX 低电平有效
				
			}
			else if((IsKeyDown_Down() == 0)&&(IsKeyDown_Up() == 0))
			{
				//按键都松开 
				GPIO_PinWrite(GPIOB, 12, 1);	//LED_TX 低有效
	      GPIO_PinWrite(GPIOB, 13, 1);//LED_RX 低电平有效
			}*/
      KEY_TYPE = KEY_NON;
		}
		KEY_TYPE = KEY_NON;
		SET_STATUS = SET_STATUS_NO;
		
		GPIO_PinWrite(GPIOB, 12, 1);	//LED_TX 低有效
		GPIO_PinWrite(GPIOB, 13, 1);//LED_RX 低电平有效

		display(currentMenu->selected);

}

//初始化菜单:
void initMenu()
{

 MainMenu.subMenus = malloc(sizeof(&MainMenu)*4);
 MainMenu.subMenus[0] = NULL;//第1到3项没有子菜单置null,选择后程序会调用func中相应项中的函数
 MainMenu.subMenus[1] = NULL;
 MainMenu.subMenus[2] = NULL;
 MainMenu.subMenus[3] = &searchMenu;//第四项查询有子菜单
 MainMenu.func = malloc(sizeof(&func)*4);
 MainMenu.func[0] = NULL;  //当前测量值
 MainMenu.func[1] = &func_set_limit; //手动设限
 MainMenu.func[2] = &func_max_value;//限位
 MainMenu.func[3] = NULL;//设置
 MainMenu.parent = NULL;//表示是顶级菜单
//主菜单	
	memset(stra1,'\0',20);
	sprintf(stra1,"%s   %05.1fmm",a1,LCM_SET.current_value);
  MainMenu.menuItems[0] = stra1; //当前值
  MainMenu.menuItems[1] = a2;  //手动设限
	memset(stra3,'\0',20);
	sprintf(stra3,"%s     %05.1fmm",a3,LCM_SET.max_value);
  MainMenu.menuItems[2] = stra3;//限位
  MainMenu.menuItems[3] = a4;  //设置
//设置菜单	
	memset(strb1,'\0',20);
	sprintf(strb1,"%s    %6d",b1,LCM_SET.Baudrate);
  searchMenu.menuItems[0] = strb1;//波特率
	memset(strb2,'\0',20);
	sprintf(strb2,"%s          %02d",b2,LCM_SET.endpoint_num);
  searchMenu.menuItems[1] = strb2;//站号
	memset(strb3,'\0',20);
	sprintf(strb3,"%s    %03ds",b3,LCM_SET.Collection_interval);
  searchMenu.menuItems[2] = strb3;//采集间隔	
	searchMenu.menuItems[3] = b4;
 
 searchMenu.subMenus = malloc(sizeof(&searchMenu)*4);
 searchMenu.subMenus[0] = searchMenu.subMenus[1] = searchMenu.subMenus[2] = searchMenu.subMenus[3]  =  NULL;
 searchMenu.func = malloc(sizeof(&func)*4);//王健
 searchMenu.func[0] = &func_Baudrate;//波特率
 searchMenu.func[1] = &func_endpoint_num;//站号
 searchMenu.func[2] = &func_Collection_interval;//采集间隔
 searchMenu.func[3] = NULL;//退出
 //searchMenu.func[0] = searchMenu.func[2] = searchMenu.func[2] = searchMenu.func[3] = searchMenu.func[4] = NULL;
 searchMenu.parent = &MainMenu;//上一级菜单是MainMenu.进入查询子菜单后按返回键,将会显示这个菜单项
 
 currentMenu = &MainMenu;
 
}

void lcm_task(void const *argument){
 
	
	gateway_dev.lcm_task_id = osThreadGetId();
	while(gateway_dev.lcm_task_id == NULL);
	initMenu();
	display(currentMenu->selected);
	while(1){

//	  osDelay(6000);	//延时等待6S 相当于定时器
		if(KEY_TYPE == KEY_NON)
		{
			osDelay(10);
			continue;
		}
		if(KEY_TYPE == KEY_S2_UP)
		{
			
				if(currentMenu->selected > 0)//当前选中项不为第一项
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
				if(currentMenu->selected < currentMenu->itemCount-1)//当前选中项不为最后一项
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
				else if((currentMenu->current_page == 1)&&(currentMenu->parent!=NULL)&&(currentMenu->selected == 3))//子菜单
				{
					 currentMenu = currentMenu->parent;
           display(currentMenu->selected);
				}
				 else if(currentMenu->func[currentMenu->selected] != NULL)//进入设置状态 开始闪烁
				 {
					 if(SET_STATUS == SET_STATUS_NO)
					 {
						 KEY_TYPE = KEY_NON;//按键清零 
						 SET_STATUS = SET_STATUS_YES;//进入设置状态
 
						 currentMenu->func[currentMenu->selected]();// 处理函数中有一个循环 未退出设置模式时 按键在循环中检测
             flash_writedata(LCM_SET_ADDRESS, &(LCM_SET.enable_flag));

					 }
				 }
		}

		KEY_TYPE = KEY_NON;

	}
	
}


