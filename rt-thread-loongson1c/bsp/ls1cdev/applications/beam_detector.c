#include "rtthread.h"
#include "beam_detector.h"
#include "drv_gpio.h"
#include "ls1c.h"
#include <finsh.h>
#include <rthw.h>
#include "ls1c_pin.h" 
#include "ls1c_timer.h" 
#include <drivers/pin.h>

unsigned char flag_active=0;
extern u8 Alarm;
void beam_detector_Test(void *parameter)
{

    while(1)
    {
        rt_thread_mdelay(1000); //1s读取一次     
        flag_active = rt_pin_read(beam_detector);
        if(flag_active)
           Alarm|=0x04;        
    }
}


void test_beam_detector(void)
{
	rt_thread_t tid;
    rt_pin_mode(beam_detector, PIN_MODE_INPUT);// 引脚为输入模式
	tid = rt_thread_create("beam_detectorTest",beam_detector_Test, RT_NULL,2048, 20, 5);
	if (tid != RT_NULL) rt_thread_startup(tid);
}


