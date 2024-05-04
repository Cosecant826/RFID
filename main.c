#include <STC12C5A60S2.H>
#include "rc522.h"
#include "define.h"
#include "Delay.h"
#include "DS1302.h"
#include "lcd12864.h"
#include "MatrixKey.h"
#include "Timer.h"

unsigned char xdata shu[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};	 //IC内容暂存数组变量
unsigned char xdata RevBuffer[4];	  //IC卡的唯一序列号
unsigned char xdata DefaultKey[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}; //密码
unsigned char xdata value1[4] = {0x33, 0, 0, 0};	

unsigned char code RFID1[16] = {0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x01, 0xFE, 0x01, 0xFE};



void check_card(unsigned char block)  	//验证卡  读取卡
{
    char  status;
    PcdReset();										//复位
    status = PcdRequest(PICC_REQIDL, &RevBuffer[0]); //寻天线区内未进入休眠状态的卡，返回卡片类型 2字节

    if (status != MI_OK)
        return;

    status = PcdAnticoll(&RevBuffer[0]); //防冲撞，返回卡的序列号4字节

    if (status != MI_OK)
        return;
    status = PcdSelect(&RevBuffer[0]);	 //选卡部分

    if (status != MI_OK)
        return;

    status = PcdAuthState(0x61, block, DefaultKey, &RevBuffer[0]); // 校验卡密码B

    if (status != MI_OK)
        return;
}

void main()
{
    u8 KeyNum,i;
    DS1302_Init();
    Lcd12864_Init();
    Timer0_Init();
	
		for(i=0;i<7;i++)
	{
		if(i%2)
		{
			Lcd12864_Init();
			Lcd12864_ShowString(0, i, "CSSKXT");
			Lcd12864_ShowString(1, 2*i, "21");
			Delay(500);
		}
		else
		{
			Lcd12864_Init();
			Lcd12864_ShowString(0, i, " CSSKXT");
			Lcd12864_ShowString(1, 2*i, " 21");
			Delay(500);
		}
	}
    while (1)
		{
        KeyNum = Key();
				
			
		}
}

void Timer0_Isr(void) interrupt 1
{
    static unsigned int T0Count;
    TL0 = 0x66;				//设置定时初始值
    TH0 = 0xFC;				//设置定时初始值
    T0Count++;

    if (T0Count % 20 == 0)
    {
        Key_Loop();    //20ms调用一次按键驱动函数
    }
}


