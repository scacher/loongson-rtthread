#include <stc8.h>

sbit DS18B20_DQ=P3^2;

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned long u32;

void Delay_us(u16 n)
{
    while(n>0)
        n--;
}

void Delay_ms2(u16  n)
{
   u8 i;
   u8 j;
   while(n>0)
   {
     for(i=0;i<16;i++)
        for(j=0;j<147;j++);
     n--;
   }
}

u8 DS18B20_Reset(void)
{
	u8 DS18B20_flag;
	DS18B20_DQ=0;
	Delay_us(480);
	DS18B20_DQ=1;
	Delay_us(80);
	DS18B20_flag=DS18B20_DQ;
	Delay_us(400);
	return DS18B20_flag;
}

//写一个字节到DS18B20
//dat：要写入的字节
void DS18B20_Write_Byte(u8 dat)
{
    u8 j;
    u8 testb; 
    for (j=1;j<=8;j++)
    {
        testb=dat&0x01;
        dat=dat>>1;
        if (testb) //输出高
        {
           DS18B20_DQ=0;       //拉低DQ，主机输出低电平
           Delay_us(2);                       //延时2us
           DS18B20_DQ=1;       //主机输出高电平
           Delay_us(60);                      //延时60us
        }
        else       //输出低
        {
           DS18B20_DQ=0;       //拉低DQ，主机输出低电平
           Delay_us(60);                      //延时60us
           DS18B20_DQ=1;       //主机输出高电平
           Delay_us(2);                        //延时2us
        }
    }
}

////从DS18B20读取数据
//返回值：读到的数据
u8 DS18B20_Read_Byte(void)
{
	u8 i,tempdata;
	for(i=8;i>0;i--)
	{
		tempdata>>=1;
		DS18B20_DQ=0;
		Delay_us(4);
		DS18B20_DQ=1;
		Delay_us(8);
		if(DS18B20_DQ==1){tempdata|=0x80;}
		else{tempdata&=0x7F;}
		Delay_us(250);
	}
	return tempdata;
}

//从ds18b20得到温度值
//精度：0.5度
//返回值：温度值 （-550~1250）
short DS18B20_Get_Temp(void)
{
    u8 temp;         //用来判断符号
    u8 TL,TH;
    short tem;

    // ds1820 start convert
    DS18B20_Reset();
    DS18B20_Write_Byte(0xcc); // skip rom
    DS18B20_Write_Byte(0x44); // convert

    Delay_ms2(800);  //等待转换完成,至少750ms

    DS18B20_Reset();        //复位
    DS18B20_Write_Byte(0xcc);   // skip rom
    DS18B20_Write_Byte(0xbe);   // 读取命令
    TL=DS18B20_Read_Byte();
    TH=DS18B20_Read_Byte();
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
    if(temp)
        return tem; //返回温度值
    else
        return -tem;
}

/*//从DS18B20读取一个位
//返回值：1/0
u8 DS18B20_Read_Bit(void)              // read one bit
{
    u8 tempdata;
    DS18B20_DQ=0;       //拉低DQ，主机输出低电平
    Delay_us(2);                       //延时2us
    Delay_us(1);//延时1us
    if(DS18B20_DQ)
        tempdata=1;//读取总线数据
    else
        tempdata=0;
    Delay_us(60);  //延时60us（读一位至少60us）
    return tempdata;
}

//从DS18B20读取一个字节
//返回值：读到的数据
u8 DS18B20_Read_Byte(void)    // read one byte
{
    u8 i,j,dat;
    dat=0;
    for (i=1;i<=8;i++)
    {
      j=DS18B20_Read_Bit();
      dat=(j<<7)|(dat>>1);
    }
    return dat;
} */



