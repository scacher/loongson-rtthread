/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-05-10     zhuangwei    first version
 */

#include <rtthread.h>
#include "ls1c_pin.h"
#include "ls1c_gpio.h"

#include <rtthread.h>
#include <drivers/pin.h>
#include "ls1c_gpio.h"
#include "ls1c.h"
#include <rthw.h>
#include "ls1c_timer.h" 
#include "ls1c_delay.h"

#include "Led.h"
#include "Uart.h" 
#include "Srf05.h"
//#include "ds18b20.h"
#include "beam_detector.h"
u8 Alarm;
u16 SpeakTime;

int main(int argc, char** argv)
{   
	int i=0;
	unsigned char flag;
	unsigned char num1,num2;
	Alarm =0;
    SpeakTime=0;
	test_Led();
	test_usart1();
	test_Srf05();
	test_beam_detector();

	gpio_init(90,gpio_mode_input);
	gpio_init(62,gpio_mode_output);
	gpio_set(62,gpio_level_high);
	gpio_init(64,gpio_mode_output);
	gpio_set(64,gpio_level_high);
	gpio_init(66,gpio_mode_output);
	gpio_set(66,gpio_level_high);

	printf("main() function i=%d.\r\n",i++);
	while(1)
	{
            if(SpeakTime)
			{
				Alarm=0;
				SpeakTime--;
			}

            if(Alarm&0x01)
			{
                        gpio_set(66,gpio_level_low);
						rt_thread_delay(100);
						gpio_set(66,gpio_level_high);
						Alarm&=0xFE;
						SpeakTime=10;
			}
			else if(Alarm&0x02)
			{
                        gpio_set(64,gpio_level_low);
						rt_thread_delay(100);
						gpio_set(64,gpio_level_high);
						Alarm&=0xFD;
						SpeakTime=10;
			}
			else if(Alarm&0x04)
			{
                        gpio_set(62,gpio_level_low);
						rt_thread_delay(100);
						gpio_set(62,gpio_level_high);
						Alarm&=0xFB;
						SpeakTime=10;
			}
		rt_thread_delay(1000); 
	}

    return 0;
}
