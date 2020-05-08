/*
 * (C) Copyright 2013 Thingtek
 *
 * Written by: randy <shenminghua@thingtek.com>
 */

#ifndef _LCM_TASK_H
#define _LCM_TASK_H

#include "common.h"

typedef struct menu//����һ���˵�
{
 u8 range_from,range_to; //��ǰ��ʾ���ʼ���������
 u8 itemCount;//��Ŀ����
 u8 selected;//��ǰѡ����
 u8 current_page;//��ǰҳ��� ��0��ʼ
 char *menuItems[17];//�˵���Ŀ
 struct menu **subMenus;//�Ӳ˵�
 struct menu *parent;//?�ϼ��˵� ,?����Ƕ�����Ϊnull
 void (**func)();//ѡ����Ӧ�����ִ�еĺ���

}Menu;

void lcm_task(void const *argument);
void func(void);
#endif

