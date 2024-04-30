#include <STC12C5A60S2.H>

void Timer0_Init(void)		//1毫秒@11.0592MHz
{
    AUXR &= 0x7F;			//定时器时钟12T模式
    TMOD &= 0xF0;			//设置定时器模式
    TMOD |= 0x01;			//设置定时器模式
    TL0 = 0x66;				//设置定时初始值
    TH0 = 0xFC;				//设置定时初始值
    TF0 = 0;				//清除TF0标志
    TR0 = 1;				//定时器0开始计时
    EA = 1;
    ET0 = 1;				//使能定时器0中断
}

void Timer1_Init(void)		//1毫秒@11.0592MHz
{
    AUXR &= 0xBF;			//定时器时钟12T模式
    TMOD &= 0x0F;			//设置定时器模式
    TMOD |= 0x10;			//设置定时器模式
    TL1 = 0x66;				//设置定时初始值
    TH1 = 0xFC;				//设置定时初始值
    TF1 = 0;				//清除TF1标志
    TR1 = 1;				//定时器1开始计时
    EA = 1;
    ET1 = 1;				//使能定时器1中断
}
