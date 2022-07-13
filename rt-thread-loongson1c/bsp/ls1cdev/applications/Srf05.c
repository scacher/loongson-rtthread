#include <finsh.h>
#include <rtthread.h>
#include <drivers/pin.h>
#include <rthw.h>

#include "ls1c_gpio.h"
#include "ls1c.h"
#include "ls1c_public.h"
#include "ls1c_pin.h"
#include "Srf05.h"
#include "Uart.h"
#include "ls1c_timer.h"


unsigned char IRQ_flag=0;
rt_uint8_t    Srf05_st;
extern u8 Alarm;

timer_info_t timer_info ;
unsigned long timer_echo_rising,timer_echo_falling;
float distance1,distance2;
u8    dc_update;

static struct rt_semaphore irq1Sem;

static void Srf05_thread(void* paramter)
{
    Srf05_st=0;
    distance1=distance2=0;
    dc_update=0;
    while(1)
    {
            timer_info.timer=TIMER_PWM0;
            timer_info.time_ns=100*1000*1000;
            timer_init(&timer_info);    
            Srf05_st=WAITING_HIGH;
			gpio_set_irq_type(echo_gpio, IRQ_TYPE_EDGE_RISING);	
			rt_pin_irq_enable(echo_gpio, PIN_IRQ_ENABLE);
			   
		   	gpio_set(trig_gpio,gpio_level_high);
		    rt_thread_delay(1);
	        gpio_set(trig_gpio,gpio_level_low);	
		 
		rt_thread_delay(200);		 
		rt_pin_irq_enable(echo_gpio, PIN_IRQ_DISABLE);
		timer_stop(&timer_info);
		   
		//if(Srf05_st!=TIME_OUT)
		//   	printf("%d,IRQ=%d \r\n" , Srf05_st,IRQ_flag);
		   
		if(Srf05_st==CATCHING)
		{
		   	distance1=((float)(timer_echo_falling-timer_echo_rising)*170)/10000000; //cm
		}
		else
		   	distance1=450;
        distance2 = distance2*0.5+distance1*0.5;
		printf("distance=%.2f\r\n",distance2);
		if(distance2<=90&&distance2>=50)
		{
			Alarm|=0x02;
		}
//      dc_update=1;
		//printf("%d ns, distance= %.2f\r\n" , timer_echo_falling-timer_echo_rising,distance2);
		rt_thread_delay(800); 
		Srf05_st=TIME_OUT;
		IRQ_flag=0;
    }
}

void My_IRQ(void *param) 
{
	if(Srf05_st==WAITING_HIGH)
		{
		timer_echo_rising=timer_get_time_ns(&timer_info);
		Srf05_st=WAITING_LOW;
		gpio_set_irq_type(echo_gpio, IRQ_TYPE_EDGE_FALLING);	  
		// rt_hw_interrupt_umask(echo_irq);  
		}
	else if(Srf05_st==WAITING_LOW)
		{
		// if(0 == gpio_get(echo_gpio)))
		timer_echo_falling=timer_get_time_ns(&timer_info);
		Srf05_st = CATCHING;
		//rt_hw_interrupt_mask(echo_irq);
		timer_stop(&timer_info);
		rt_sem_release(&irq1Sem);
		}
	IRQ_flag++;
}

void Srf05_init(void)
{
    gpio_init(trig_gpio,gpio_mode_output);
    gpio_set(trig_gpio,gpio_level_low);
    rt_pin_mode(echo_gpio, PIN_MODE_INPUT_PULLDOWN);
    // 绑 定 中 断， 下 降 沿 模 式， 回 调 函 数 名 为beep_on */
    rt_pin_attach_irq(echo_gpio, PIN_IRQ_MODE_RISING, My_IRQ, RT_NULL);//PIN_IRQ_MODE_RISING
    // rt_pin_attach_irq(echo_gpio, PIN_IRQ_MODE_RISING_FALLING, My_IRQ, RT_NULL);//PIN_IRQ_MODE_RISING
    //  rt_pin_attach_irq(echo_gpio, PIN_IRQ_MODE_FALLING, My_IRQ, RT_NULL);
    // 使 能 中 断 */
    rt_pin_irq_enable(echo_gpio, PIN_IRQ_ENABLE);
	
	Srf05_st=TIME_OUT;
}

void test_Srf05(void)
{
    Srf05_init();
	rt_thread_t tid;
	rt_sem_init(&irq1Sem,"irq1Sem", 0, RT_IPC_FLAG_FIFO);
	tid = rt_thread_create("Srf05_Test", Srf05_thread, RT_NULL,2048, 5, 5);
	if (tid != RT_NULL) rt_thread_startup(tid);
}

