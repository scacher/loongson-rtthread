/*
 //////////////////////////////////////////////////////////////////////////////////   
 //本程序只供学习使用，未经作者许可，不得用于其它任何用途
 //作者： 安徽大学电子信息工程学院  王FN QQ2388309910
 // 
 ////////////////////////////////////////////////////////////////////////////////// 

/*代码 Uart.c*/
#include <finsh.h>
#include <rtthread.h>
#include <drivers/pin.h>
#include <rthw.h>

#include "ls1c_gpio.h"
#include "ls1c.h"
#include "ls1c_public.h"
#include "ls1c_uart.h"
#include "ls1c_pin.h"  

#include "Uart.h"

/* 串 口 设 备 句 柄 */
rt_device_t serial1;
u8 UART1_Get_OK;//
u8 UART1_RX_Buff[UART_BUFF_LEN];     //
extern u8 Alarm;
static rt_thread_t usart1Thread=RT_NULL;
static struct rt_semaphore usart1Sem;

//static rt_sem_t usart1Sem = RT_NULL;


void uart1_irqhandler(int IRQn, void *param)
{
   ls1c_uart_t uartx = uart_irqn_to_uartx(IRQn);
   void *uart_base = uart_get_base(uartx);
   unsigned char iir = reg_read_8(uart_base + LS1C_UART_IIR_OFFSET);
   // 判断是否为接收超时或接收到有效数据
   if ((IIR_RXTOUT & iir) || (IIR_RXRDY & iir))
   {
      // 是，则读取数据，并原样发送回去
      while (LSR_RXRDY & reg_read_8(uart_base + LS1C_UART_LSR_OFFSET))
      {
         uart_putc(uartx, reg_read_8(uart_base + LS1C_UART_DAT_OFFSET));
      }
   }    
   return ;
}


// 接 收 数 据 回 调 函 数 /
static rt_err_t uart_input(rt_device_t dev, rt_size_t size)
{
   rt_device_read(dev, 0, UART1_RX_Buff, size);
   rt_sem_release(&usart1Sem);
//   if(UART1_RX_Buff[0]==0xAB)
//   {
  //    UART1_Get_OK=1;
  //    UART1_RX_Buff[8]=size;
//   }
   return RT_EOK;
//   rt_device_read(dev, 0, UART1_RX_Buff,0);
//   rt_device_read(dev, 0, UART1_RX_Buff, size);
//  if(UART1_RX_Buff[0]==0xAB)
//   {
//      UART1_Get_OK=1;
//      UART1_RX_Buff[8]=size;
//   } 

   // rt_kprintf("%d\n",UART1_No_Cnt);
   // while(UART1_No_Cnt)
//   {
   //   uart_putc((ls1c_uart_t)LS1C_UART1,UART1_RX_Buff[UART1_No_Cnt-1]);
   //  UART1_No_Cnt--;
//   }
   //rt_device_write(dev,0,UART1_RX_Buff,UART1_No_Cnt);
}


static void Uart1_Init(void *parameter)
{  
    //  char result;
/*
      unsigned int rx_gpio,tx_gpio;
		ls1c_uart_info_t uart_info = {0};	
  
      rx_gpio = 2;
      tx_gpio = 3;
      pin_set_remap(tx_gpio, PIN_REMAP_FOURTH);
      pin_set_remap(rx_gpio, PIN_REMAP_FOURTH);
      uart_info.UARTx = LS1C_UART1;//uart1 P02 P03
      uart_info.baudrate = baudrate;
      uart_info.rx_enable = TRUE;  
      uart_init(&uart_info);
      rt_hw_interrupt_umask(LS1C_UART1_IRQ);		
      uart_print((ls1c_uart_t)LS1C_UART1, "\r\nThis is uart1 receive string:");
		rt_hw_interrupt_install(LS1C_UART1_IRQ, uart1_irqhandler, RT_NULL, "UART1");
		*/

//
   //char str[] = "hello RT-Thread!\r\n";
   //char str[] = "hi\r\n";
   // struct serial_configure config = RT_SERIAL_CONFIG_DEFAULT; // 初 始 化 配 置 参 数
   serial1 = rt_device_find("uart1");
	//if (!serial)   
   {
      // rt_kprintf("find uart1 failed!\n");
      // return ;
   }
	
	// config.baud_rate = BAUD_RATE_9600;
	//rt_device_control(serial, RT_DEVICE_CTRL_CONFIG, &config);
	 
	// 以 DMA 接 收 及 轮 询 发 送 方 式 打 开 串 口 设 备 
	if (rt_device_open(serial1, RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_INT_RX)==RT_EOK)	
	   rt_kprintf("Open uart1 OK\n");
	else
	   rt_kprintf("Open uart1 Fail\n");
	 	
   // 设 置 接 收 回 调 函 数 
   rt_device_set_rx_indicate(serial1, uart_input);
   // 发 送 字 符 串 
   //rt_device_write(serial1, 0, str, (sizeof(str) - 1));
   //
	while(1)
   {
     // while(rt_device_read(serial1, 0, UART1_RX_Buff, UART_BUFF_LEN)!=1)
         rt_sem_take(&usart1Sem,RT_WAITING_FOREVER);
         printf("temp=%d\r\n",(UART1_RX_Buff[0]*256+UART1_RX_Buff[1]));
         if((UART1_RX_Buff[0]*256+UART1_RX_Buff[1])>300)
         {
             Alarm|=0x01;
         }
      //rt_thread_delay(1000); 
   }
}

void test_usart1(void)
{
   //rt_pin_mode(beam_detector, PIN_MODE_INPUT);// 引脚为输入模式
   rt_sem_init(&usart1Sem,"usart1Sem", 0, RT_IPC_FLAG_FIFO);   //创建
	usart1Thread = rt_thread_create("usart1",Uart1_Init, RT_NULL,2048, 5, 5);
	if (usart1Thread != RT_NULL) rt_thread_startup(usart1Thread);
}




/* 导出到 msh 命令列表中 */
//MSH_CMD_EXPORT(test_Led, startup a Led test);




