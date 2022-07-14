//单片机:STC8单片机完全
//功能：LED 闪烁 串口发送验证单片机电路工作
//      
///
#include<stc8.h>
#include <intrins.h>
				 
#include <stdio.h>				 
#include <stdlib.h>		
#include <string.h>	

#include "ds1820.h"	



typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned long u32;


void delay_ms(u16 n);
void InitUART(void);
void SendOneByte(unsigned char c);
void SendString(unsigned char *buff);
void SendBuff(unsigned char *buff,u8 Len);

sbit LED_Out=P3^3;//

//u8  Send
bit  LED;

main(void)/////////////////////////////////
{
    u16 da;
	u8 Text[32]; 
	P_SW1=0x00;		 //RXD/P3.0, TXD/P3.1

    P3M0 = 0x00;                                //设置P2.0~P2.7为双向口模式	  
	P3M1 = 0x00;

    InitUART();
    while(1)
    {
		da=DS18B20_Get_Temp();
		SendBuff((u8*)(&da),2);
//		sprintf(Text,"%d",da);
	//	SendOneByte(0xAB);
	//	SendOneByte(da/256);
	//	SendOneByte(da%256);

	 // 	SendString(Text);
    LED_Out = LED;
    LED=!LED;
    delay_ms(1000);////延时 x ms
	}
}

void delay_ms(u16 n)   //误差 -0.651041666667us
{
    unsigned char a,b;
	while(n--)
    {
	  for(b=142;b>0;b--)
        for(a=15;a>0;a--);
	}
}

void InitUART(void)
{
 //   TMOD = 0x20;
    SCON = 0x50;
    T2L = 0xe8;
    T2H = 0xFF;
    AUXR = 0x15;
 //   EA = 1;
    ES = 1;
 //   TR1 = 1;
}



void UARTInterrupt(void) interrupt 4
{
    if(RI)
    {
        RI = 0;
        //add your code here!
    }
    else
        TI = 0;
}

void SendOneByte(unsigned char c)
{
    SBUF = c;
    while(!TI);
	delay_ms(2);
    TI = 0;
}

void SendBuff(unsigned char *buff,u8 Len)
{
u8 i=0;
 	while(Len)
	{
        SBUF = buff[i];
        while(!TI);
        TI = 0;
		Len--;   
		i++; 
	} 
}

void SendString(unsigned char *buff)
{
  u8 i=0;
//	SBUF = 0xAB;
//	while(!TI);
//	TI=0;
	while(buff[i]!='\0')
	{
        SBUF = buff[i];
        while(!TI);
//	  delay_ms(2);
        TI = 0;
		i++;    
	} 
}
