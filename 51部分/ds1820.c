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

//дһ���ֽڵ�DS18B20
//dat��Ҫд����ֽ�
void DS18B20_Write_Byte(u8 dat)
{
    u8 j;
    u8 testb; 
    for (j=1;j<=8;j++)
    {
        testb=dat&0x01;
        dat=dat>>1;
        if (testb) //�����
        {
           DS18B20_DQ=0;       //����DQ����������͵�ƽ
           Delay_us(2);                       //��ʱ2us
           DS18B20_DQ=1;       //��������ߵ�ƽ
           Delay_us(60);                      //��ʱ60us
        }
        else       //�����
        {
           DS18B20_DQ=0;       //����DQ����������͵�ƽ
           Delay_us(60);                      //��ʱ60us
           DS18B20_DQ=1;       //��������ߵ�ƽ
           Delay_us(2);                        //��ʱ2us
        }
    }
}

////��DS18B20��ȡ����
//����ֵ������������
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

//��ds18b20�õ��¶�ֵ
//���ȣ�0.5��
//����ֵ���¶�ֵ ��-550~1250��
short DS18B20_Get_Temp(void)
{
    u8 temp;         //�����жϷ���
    u8 TL,TH;
    short tem;

    // ds1820 start convert
    DS18B20_Reset();
    DS18B20_Write_Byte(0xcc); // skip rom
    DS18B20_Write_Byte(0x44); // convert

    Delay_ms2(800);  //�ȴ�ת�����,����750ms

    DS18B20_Reset();        //��λ
    DS18B20_Write_Byte(0xcc);   // skip rom
    DS18B20_Write_Byte(0xbe);   // ��ȡ����
    TL=DS18B20_Read_Byte();
    TH=DS18B20_Read_Byte();
    if(TH>7)
    {
        TH=~TH;
        TL=~TL;
        temp=0;//�¶�Ϊ��
    }else temp=1;//�¶�Ϊ��
    tem=TH; //��ø߰�λ
    tem<<=8;
    tem+=TL;//��õװ�λ
    tem=(float)tem*0.625;//ת��
    if(temp)
        return tem; //�����¶�ֵ
    else
        return -tem;
}

/*//��DS18B20��ȡһ��λ
//����ֵ��1/0
u8 DS18B20_Read_Bit(void)              // read one bit
{
    u8 tempdata;
    DS18B20_DQ=0;       //����DQ����������͵�ƽ
    Delay_us(2);                       //��ʱ2us
    Delay_us(1);//��ʱ1us
    if(DS18B20_DQ)
        tempdata=1;//��ȡ��������
    else
        tempdata=0;
    Delay_us(60);  //��ʱ60us����һλ����60us��
    return tempdata;
}

//��DS18B20��ȡһ���ֽ�
//����ֵ������������
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



