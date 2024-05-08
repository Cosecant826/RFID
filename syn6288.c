#include <STC12C5A60S2.H>
#include <stdio.h>
#include <string.h>
#include "uart.h"
#include "define.h"
#include "stdarg.h"
//ʹ�ø�ģ�����س����ʱ����Ҫ��ģ���ϵ�RXD,TXD����
//��������Music���Ҹ�ɾ��
//����������ͬ����ʱ��֡��֮֡��ļ���������8ms
//	����SYN_FrameInfo(0, "[v8][t5]��ˢ��");
//		Delay100ms(5);
//		SYN_FrameInfo(0, "[v8][t5]ˢ���ɹ�");
//		Delay100ms(5);
//֡��֮֡���Delay�ᱻ�����Ŵ�����Delay100ms(60);����60s��ŻᲥ���ڶ��仰
//extern unsigned char SYN_StopCom[];
//extern unsigned char SYN_SuspendCom[];
//extern unsigned char SYN_RecoverCom[];
//extern unsigned char SYN_ChackCom[];
//extern unsigned char SYN_PowerDownCom[];

//Music:ѡ�񱳾����֡�(0���ޱ�������  1-15���������ֿ�ѡ)
//*HZdata:m[0~16]:0��������Ϊ������16���������������
//		  v[0~16]:0�ʶ�����Ϊ������16�ʶ��������
//        t[0~5]:0�ʶ�����������5�ʶ��������
//		  +��Ҫ����������
//���������ù�����ο������ֲ�

void SYN_FrameInfo(unsigned char Music, unsigned char *HZdata)
{												//ָ��HZdataʵ���Ǹ���ַ��������Դ�����ֽ�

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

	memcpy(&Frame_Info[5], HZdata, HZ_Length);//��Frame_Info��д���ֽ���ΪHZ_Length������HZdata
	Frame_Info[5 + HZ_Length] = ecc;
	UART1_SendString(Frame_Info, 5 + HZ_Length + 1);
	Music=0;  //ȥ������
}									//����ͨ�����ڣ���Ƭ�����ص�ֵ��6288�ж����ǵ���


void SYN6288_Say(char *fmt, ...)
{
    static char  str_1[200];   // ������1��ģ��ÿ�ο�ת��200�ֽ�
    static char  str_2[200];   // ������2��ģ��ÿ�ο�ת��200�ֽ�
    va_list ap;
    va_start(ap, fmt);
    vsprintf(str_1, fmt, ap);
    va_end(ap);
    sprintf(str_2, "[d][V12][m15][t5]%s", str_1);  // [d]�ָ�Ĭ��״̬��[V12]�ʶ�����0~16��[m15]��������0~16��[t5]����0~5
    SYN_FrameInfo(0, (u8 *)str_2); // �ޱ�������
}
/*Ҫ����������ģ�����Ҫ������UART����
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
		UART1_SendByte(*DAT++);//DAT�ĵ�ַ��һ��*��������ַ�е���
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