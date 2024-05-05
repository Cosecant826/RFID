#include <STC12C5A60S2.H>


void UART1_SendByte(unsigned char DAT)
{
	SBUF = DAT;
	while(TI == 0);
	TI=0;
}


void UART1_SendString(unsigned char *DAT, unsigned char len)
{
	unsigned char i;
	for(i = 0; i < len; i++)
	{
		UART1_SendByte(*DAT++);//DAT的地址加一，*（）读地址中的数
	}
}

void Uart1_Init(void)	//9600bps@12.000MHz
{
	PCON &= 0x7F;		//波特率不倍速
	SCON = 0x50;		//8位数据,可变波特率
	AUXR |= 0x04;		//定时器时钟1T模式
	BRT = 0xD9;			//设置定时重载值
	AUXR |= 0x01;		//串口1使用独立波特率发射器为波特率发生器
	AUXR |= 0x10;		//启动独立波特率发射器	
	ES = 0;
	EA = 1;
	RI = 0;
	TI = 0;
}

