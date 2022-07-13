/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2017-09-06     勤为本       first version
 */

#include <rtthread.h>
#include "ds18b20.h"
#include "drv_gpio.h"
#include "ls1c.h"
#include <finsh.h>
#include <rthw.h>
#include "ls1c_pin.h" 
#include "ls1c_timer.h" 
#include <drivers/pin.h>
#include "ls1c_gpio.h"
#include "ls1c_delay.h"
//#include <clint.h>
//#include <sysctl.h>

//rt_mutex_t rs232_send_mx = RT_NULL;
//rs232_send_mx = rt_mutex_create ("ds18b20_mx", RT_IPC_FLAG_FIFO);

unsigned char DS18B20_Reset(void)
{
    unsigned char DS1820_flag;
    //rt_base_t level;

    //level = rt_hw_interrupt_disable();    //API：进入临界区，退出前系统不会发生任务调度
    //rt_enter_critical();
    gpio_init(DS18B20_DQ,gpio_mode_output);// 引脚为输出模式
    gpio_set(DS18B20_DQ,gpio_level_low);       //拉低DQ
    delay_us(750);                     //拉低750us（480 ~ 960us）
    gpio_set(DS18B20_DQ,gpio_level_high);       //拉高DQ
    delay_us(10);
    gpio_init(DS18B20_DQ,gpio_mode_input); //设置为输入，即可释放总线的占用
    DS1820_flag=gpio_get(DS18B20_DQ);
    delay_us(400);
    //rt_exit_critical();
    //rt_hw_interrupt_enable(level);    //API：退出临界区   
    return DS1820_flag;
    //进入接收模式，等待应答信号。
}

//写一个字节到DS18B20
//dat：要写入的字节
void DS18B20_Write_Byte(unsigned char dat)     
{             
    unsigned char j;  
    unsigned char testb;
    rt_pin_mode(DS18B20_DQ, PIN_MODE_OUTPUT); // DQ引脚为输出模式
    for (j=1;j<=8;j++) 
    {
        testb=dat&0x01;  
        dat=dat>>1;
        if (testb) //输出高
        {
           rt_pin_write(DS18B20_DQ, PIN_LOW);       //拉低DQ，主机输出低电平
           delay_us(2);                       //延时2us
           rt_pin_write(DS18B20_DQ, PIN_HIGH);       //主机输出高电平
           delay_us(60);                      //延时60us            
        }
        else       //输出低
        {
           rt_pin_write(DS18B20_DQ, PIN_LOW);       //拉低DQ，主机输出低电平
           delay_us(60);                      //延时60us
           rt_pin_write(DS18B20_DQ, PIN_HIGH);       //主机输出高电平
           delay_us(2);                        //延时2us        
        }  
    }
    rt_pin_mode(DS18B20_DQ, PIN_MODE_INPUT);         //设置为输入，释放总线
}

//从DS18B20读取一个位
//返回值：1/0
unsigned char  DS18B20_Read_Bit(void)              // read one bit
{
    unsigned char data;
    rt_pin_mode(DS18B20_DQ, PIN_MODE_OUTPUT); // DQ引脚为输出模式
    rt_pin_write(DS18B20_DQ, PIN_LOW);       //拉低DQ，主机输出低电平
    delay_us(2);                       //延时2us
    rt_pin_mode(DS18B20_DQ, PIN_MODE_INPUT); //设置为输入，释放总线
    delay_us(1);//延时1us

    if(rt_pin_read(DS18B20_DQ))
        data=1;//读取总线数据
    else 
        data=0;     
    delay_us(60);  //延时60us（读一位至少60us）         
    return data;
}

//从DS18B20读取一个字节
//返回值：读到的数据
unsigned char DS18B20_Read_Byte(void)    // read one byte
{        
    unsigned char i,j,dat;
    dat=0;

    for (i=1;i<=8;i++) 
    {
      j=DS18B20_Read_Bit();
      dat=(j<<7)|(dat>>1);
    }                            
    return dat;
}

/*void DS18B20_Init()
{
    DS18B20_Reset
    DS18B20_Write_Byte(0xCC);// 跳过 ROM
    DS18B20_Write_Byte(0x4E);// 写暂存器
    DS18B20_Write_Byte(0x20);// 往暂存器的第三字节中写上限值
    DS18B20_Write_Byte(0x00);// 往暂存器的第四字节中写下限值
    DS18B20_Write_Byte(0x7F);// 将配置寄存器配置为 12 位精度
    DS18B20_Reset
}*/

//从ds18b20得到温度值
//精度：0.5度
//返回值：温度值 （-550~1250） 
short DS18B20_Get_Temp(void)
{
    unsigned char temp;         //用来判断符号
    unsigned char TL,TH;
    short tem;
    //rt_base_t level;
    unsigned char flag;

    // ds1820 start convert
    //level = rt_hw_interrupt_disable();    //API：进入临界区，退出前系统不会发生任务调度
    rt_enter_critical();
    DS18B20_Reset(); 
    flag=DS18B20_Reset();   
    printf("flag = %d\r\n", flag);
    //DS18B20_Check();
    DS18B20_Write_Byte(0xcc); // skip rom
    DS18B20_Write_Byte(0x44); // convert
    //rt_hw_interrupt_enable(level);    //API：退出临界区    
    rt_exit_critical();

    rt_thread_mdelay(800);  //等待转换完成,至少750ms

    //level = rt_hw_interrupt_disable();    //API：进入临界区，退出前系统不会发生任务调度
    rt_enter_critical();
    DS18B20_Reset();        //复位
    //DS18B20_Check();     
    DS18B20_Write_Byte(0xcc);   // skip rom
    DS18B20_Write_Byte(0xbe);   // 读取命令  
    TL=DS18B20_Read_Byte(); 
    TH=DS18B20_Read_Byte(); 
    //rt_hw_interrupt_enable(level);    //API：退出临界区   
    rt_exit_critical();    
    if(TH>7)
    {
        TH=~TH;
        TL=~TL; 
        temp=0;//温度为负  
    }else temp=1;//温度为正          
    tem=TH; //获得高八位
    tem<<=8;    
    tem+=TL;//获得底八位
    tem=(float)tem*0.625;//转换 
    //tem_update=1;
    if(temp)
        return tem; //返回温度值
    else 
        return -tem;    
}
/* 
说明：ds18b20温度获取线程的入口函数
*/
void ds18b20_Test(void *parameter)
{ 
    // TL,TH;
    //DS18B20_Init();
    //unsigned char flag;
    short tem;
    while(1)
    {
        tem=DS18B20_Get_Temp();
        delay_us(1000); //1s读取一次    
        //flag=DS18B20_Reset();   
        //printf("flag = %d\r\n", flag);
        //printf("tem = %.2f\r\n", ((float)tem)/10);
    }
}
void test_ds18b20(void)
{
	rt_thread_t tid;
	tid = rt_thread_create("ds18b20Test",ds18b20_Test, RT_NULL,2048, 20, 50);
	if (tid != RT_NULL) rt_thread_startup(tid);
}





