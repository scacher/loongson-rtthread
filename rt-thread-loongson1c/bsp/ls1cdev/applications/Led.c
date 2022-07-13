/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-05-10     zhuangwei    first version
 */
/*代码 Led.c*/
#include <finsh.h>
#include <rtthread.h>
#include <drivers/pin.h>
#include "ls1c_gpio.h"
#include "Led.h"

/* 线程入口 */
static void Led_thread(void* paramter)
{
   int Gpio_num;
   
    gpio_init(LED1_BLUE,gpio_mode_output);
    gpio_init(LED2_YELLOW,gpio_mode_output);
    gpio_init(D2_GREEN,gpio_mode_output);
//	gpio_init(53,gpio_mode_output);
//	gpio_init(54,gpio_mode_output);
	Gpio_num=28;
	while(1)
		{
		   
	//	   gpio_init(Gpio_num,gpio_mode_output);
		//   printf("Gpio=%d\n",Gpio_num);
		   gpio_set(LED1_BLUE,gpio_level_low);
		   gpio_set(LED2_YELLOW,gpio_level_low);
		   gpio_set(D2_GREEN,gpio_level_low);
		   rt_thread_delay(1000);		   
		   gpio_set(LED1_BLUE,gpio_level_high);
		   gpio_set(LED2_YELLOW,gpio_level_high);
		   gpio_set(D2_GREEN,gpio_level_high);
		   if(++Gpio_num>=77)
		   	   Gpio_num=28;		   	
		   rt_thread_delay(1000);	
		}
}


void test_Led(void)
{
	rt_thread_t tid;
	tid = rt_thread_create("LedTest", Led_thread, RT_NULL,2048, 25, 5);
	if (tid != RT_NULL) 
	   rt_thread_startup(tid);
	else
	   printf("LED fail\r\n");

}

/* 导出到 msh 命令列表中 */
//MSH_CMD_EXPORT(test_Led, startup a Led test);




