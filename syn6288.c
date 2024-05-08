#include <STC12C5A60S2.H>
#include <stdio.h>
#include <string.h>
#include "uart.h"
#include "define.h"
#include "stdarg.h"
//使用该模块下载程序的时候需要将模块上的RXD,TXD拔了
//背景音乐Music，我给删了
//在连续读不同语音时，帧与帧之间的间隔必须大于8ms
//	例：SYN_FrameInfo(0, "[v8][t5]请刷卡");
//		Delay100ms(5);
//		SYN_FrameInfo(0, "[v8][t5]刷卡成功");
//		Delay100ms(5);
//帧与帧之间的Delay会被倍数放大，例如Delay100ms(60);将会60s后才会播报第二句话
//extern unsigned char SYN_StopCom[];
//extern unsigned char SYN_SuspendCom[];
//extern unsigned char SYN_RecoverCom[];
//extern unsigned char SYN_ChackCom[];
//extern unsigned char SYN_PowerDownCom[];

//Music:选择背景音乐。(0：无背景音乐  1-15：背景音乐可选)
//*HZdata:m[0~16]:0背景音乐为静音，16背景音乐音量最大
//		  v[0~16]:0朗读音量为静音，16朗读音量最大
//        t[0~5]:0朗读语速最慢，5朗读语速最快
//		  +需要读出的文字
//其他不常用功能请参考数据手册

void SYN_FrameInfo(unsigned char Music, unsigned char *HZdata)
{												//指针HZdata实际是个地址，里面可以存许多字节

	unsigned char  Frame_Info[50];
	unsigned char  HZ_Length;
	unsigned char  ecc  = 0;  			
	unsigned int i = 0;
	HZ_Length = strlen((char*)HZdata); 			

	Frame_Info[0] = 0xFD ; 			
	Frame_Info[1] = 0x00 ; 			
	Frame_Info[2] = HZ_Length + 3; 		
	Frame_Info[3] = 0x01 ; 			
	Frame_Info[4] = 0x01 ;//| Music << 4 ; 

	for(i = 0; i < 5; i++)   				
	{
		ecc = ecc ^ (Frame_Info[i]);	
	}

	for(i = 0; i < HZ_Length; i++)   		
	{
		ecc = ecc ^ (HZdata[i]); 				
	}

	memcpy(&Frame_Info[5], HZdata, HZ_Length);//向Frame_Info中写入字节数为HZ_Length的数据HZdata
	Frame_Info[5 + HZ_Length] = ecc;
	UART1_SendString(Frame_Info, 5 + HZ_Length + 1);
	Music=0;  //去除警告
}									//这里通过串口，单片机返回的值到6288中而不是电脑


void SYN6288_Say(char *fmt, ...)
{
    static char  str_1[200];   // 缓存区1，模块每次可转换200字节
    static char  str_2[200];   // 缓存区2，模块每次可转换200字节
    va_list ap;
    va_start(ap, fmt);
    vsprintf(str_1, fmt, ap);
    va_end(ap);
    sprintf(str_2, "[d][V12][m15][t5]%s", str_1);  // [d]恢复默认状态，[V12]朗读音量0~16，[m15]背景音量0~16，[t5]语速0~5
    SYN_FrameInfo(0, (u8 *)str_2); // 无背景音乐
}
/*要用语音播报模块必须要有以下UART函数
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

void UART1_Init(void)
{
	SCON = 0x50;		
	TMOD |= 0x20;		
	TL1 = 0xFD;		
	TH1 = 0xFD;		
	ET1 = 0;		
	TR1 = 1;		
	ES = 1;
	EA = 1;
	RI = 0;
	TI = 0;
}*/