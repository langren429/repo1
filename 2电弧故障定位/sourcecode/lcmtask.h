/*
 * (C) Copyright 2013 Thingtek
 *
 * Written by: randy <shenminghua@thingtek.com>
 */

#ifndef _LCM_TASK_H
#define _LCM_TASK_H

#include "common.h"

typedef struct menu//定义一个菜单
{
 u8 range_from,range_to; //当前显示的项开始及结束序号
 u8 itemCount;//项目总数
 u8 selected;//当前选择项
 u8 current_page;//当前页序号 从0开始
 char *menuItems[17];//菜单项目
 struct menu **subMenus;//子菜单
 struct menu *parent;//?上级菜单 ,?如果是顶级则为null
 void (**func)();//选择相应项按键后执行的函数

}Menu;

void lcm_task(void const *argument);
void func(void);
#endif

