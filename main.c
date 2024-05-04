#include <STC12C5A60S2.H>
#include "rc522.h"
#include "define.h"
#include "Delay.h"
#include "DS1302.h"
#include "lcd12864.h"
#include "MatrixKey.h"
#include "Timer.h"

unsigned char xdata shu[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};	 //IC�����ݴ��������
unsigned char xdata RevBuffer[4];	  //IC����Ψһ���к�
unsigned char xdata DefaultKey[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}; //����
unsigned char xdata value1[4] = {0x33, 0, 0, 0};	

unsigned char code RFID1[16] = {0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x01, 0xFE, 0x01, 0xFE};



void check_card(unsigned char block)  	//��֤��  ��ȡ��
{
    char  status;
    PcdReset();										//��λ
    status = PcdRequest(PICC_REQIDL, &RevBuffer[0]); //Ѱ��������δ��������״̬�Ŀ������ؿ�Ƭ���� 2�ֽ�

    if (status != MI_OK)
        return;

    status = PcdAnticoll(&RevBuffer[0]); //����ײ�����ؿ������к�4�ֽ�

    if (status != MI_OK)
        return;
    status = PcdSelect(&RevBuffer[0]);	 //ѡ������

    if (status != MI_OK)
        return;

    status = PcdAuthState(0x61, block, DefaultKey, &RevBuffer[0]); // У�鿨����B

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
    TL0 = 0x66;				//���ö�ʱ��ʼֵ
    TH0 = 0xFC;				//���ö�ʱ��ʼֵ
    T0Count++;

    if (T0Count % 20 == 0)
    {
        Key_Loop();    //20ms����һ�ΰ�����������
    }
}


