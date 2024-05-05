#include <STC12C5A60S2.H>
#include "rc522.h"
#include "define.h"
#include "Delay.h"
#include "DS1302.h"
#include "lcd12864.h"
#include "MatrixKey.h"
#include "Timer.h"
#include "HX711.h"

unsigned char xdata shu[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};	 //IC�����ݴ��������
unsigned char xdata RevBuffer[4];	  //IC����Ψһ���к�
unsigned char xdata DefaultKey[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}; //����
unsigned char xdata value1[4] = {0x33, 0, 0, 0};
float xdata pricecode[10] = {5.6, 7.2, 10.3};

unsigned char code RFID1[16] = {0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x01, 0xFE, 0x01, 0xFE};
float Weight_Shiwu, Weight_Maopi;


char check_card (unsigned char block)  	//��֤��  ��ȡ��
{
    char  status;
    PcdReset();										//��λ
    status = PcdRequest (PICC_REQIDL, &RevBuffer[0]); //Ѱ��������δ��������״̬�Ŀ������ؿ�Ƭ���� 2�ֽ�

    if (status != MI_OK)
        return 1;

    status = PcdAnticoll (&RevBuffer[0]); //����ײ�����ؿ������к�4�ֽ�

    if (status != MI_OK)
        return 1;

    status = PcdSelect (&RevBuffer[0]);	 //ѡ������

    if (status != MI_OK)
        return 1;

    status = PcdAuthState (0x61, block, DefaultKey, &RevBuffer[0]); // У�鿨����B

    if (status != MI_OK)
        return 1;

    return 0;
}

void main()
{
    u8 KeyNum, LastKeyNum[5], i,status;
    float kgprice;
    u16 Fruitcode = 0, allprice=0, price, getweight;
    DS1302_Init();
    Lcd12864_Init();
    Timer0_Init();
    Weight_Maopi = HX711_Read();

    for (i = 0; i < 6; i++)
    {
        if (i % 2)
        {
            Lcd12864_Init();
            Lcd12864_ShowString (0, i / 2, " CSSKXT");
            Lcd12864_ShowString (1, i / 2, "   21");
            Delay (500);
        }
        else
        {
            Lcd12864_Init();
            Lcd12864_ShowString (0, i / 2, "CSSKXT");
            Lcd12864_ShowString (1, i / 2, "  21");
            Delay (500);
        }
    }

    i = 0;
    Delay (1000);
    Lcd12864_Init();
    Lcd12864_ShowString (0, 0, " - -��ӭ����- - ");
    Lcd12864_ShowString (1, 0, "����:        ");
    Lcd12864_ShowString (2, 0, "����:       KG");
    Lcd12864_ShowString (3, 0, "�ܼ�:       Ԫ");

    while (1)
    {
        KeyNum = Key();
				status=check_card(1);
			
        if (KeyNum <= 10 && KeyNum > 0)
        {
            Lcd12864_WriteCMD (0x93 + i / 2);

            if (i % 2)
                Lcd12864_WriteData (LastKeyNum[i - 1] % 10 + 48);

            Lcd12864_WriteData (KeyNum % 10 + 48);
            Fruitcode *= 10;
            Fruitcode += KeyNum % 10;
            LastKeyNum[i] = KeyNum;
            i++;

            if (i == 5)
            {
                if (Fruitcode == 10021)
                {
                    Lcd12864_Init();
                    Lcd12864_ShowString (0, 0, "Ʒ��: ƻ��");
                    Lcd12864_ShowString (1, 0, "����: 5.6 RMB/KG");
                }
                else if (Fruitcode == 10022)
                {
                    Lcd12864_Init();
                    Lcd12864_ShowString (0, 0, "Ʒ��: �㽶");
                    Lcd12864_ShowString (1, 0, "����: 7.2 RMB/KG");
                }
                else if (Fruitcode == 10023)
                {
                    Lcd12864_Init();
                    Lcd12864_ShowString (0, 0, "Ʒ��: ����");
                    Lcd12864_ShowString (1, 0, "����:10.3 RMB/KG");
                }
                else
                {
                    Fruitcode = 0;
                    i = 0;
                    Lcd12864_ShowString (3, 0, "δ�ҵ�����Ʒ");
                    Delay (1000);
                    Lcd12864_ShowString (1, 0, "����:        ");
                    Lcd12864_ShowString (3, 0, "            ");
                }

                kgprice = pricecode[Fruitcode - 10021];

                if (Fruitcode)
                {
                    Lcd12864_ShowString (2, 0, "����:       ����");
                    Lcd12864_ShowString (3, 0, "�۸�:       Ԫ");
                    KeyNum = 0;

                    while (!KeyNum)
                    {
                        KeyNum = Key();
                        Get_Weight();

                        if (Weight_Shiwu < 51000)
                            Lcd12864_ShowString (2, 2, ": ");
                        else
                            Weight_Shiwu = 0;

                        Weight_Shiwu *= 50;
                        Weight_Shiwu /= 1000;
                        getweight = Weight_Shiwu;
                        Lcd12864_ShowNum (2, 3, getweight / 10, 4);
                        Lcd12864_ShowFloatNum (2, 5, getweight % 10, 1); //��ʾ���һλ
                        price = (unsigned int) (kgprice * getweight * 10);
                        allprice += price;
                        Lcd12864_ShowNum (3, 3, price / 100, 3);
                        Lcd12864_WriteData (0x2E);
                        Lcd12864_ShowNum (3, 5, price % 100, 2);
                    }

                    if (KeyNum == 12)
                    {
                    }
                }
            }
        }
        else if (KeyNum == 11)
        {
            if (i > 0)
            {
                i--;
                Fruitcode /= 10;
                Lcd12864_WriteCMD (0x93 + i / 2);

                if (i % 2)
                    Lcd12864_WriteData (LastKeyNum[i - 1] % 10 + 48);

                Lcd12864_WriteData (0x20);
                Lcd12864_WriteCMD (0x93 + i / 2);

                if (i % 2)
                    Lcd12864_WriteData (LastKeyNum[i - 1] % 10 + 48);
            }
        }
        else if (!status)
        {
					Lcd12864_ShowString (3, 0, "���:       Ԫ");
					PcdRead(1,&shu);
					price=shu[0]+shu[1]*256;
					Lcd12864_ShowNum (3, 3, price / 100, 3);
                        Lcd12864_WriteData (0x2E);
                        Lcd12864_ShowNum (3, 5, price % 100, 2);
            Delay (1000);
					Lcd12864_ShowString (3, 0, "�ܼ�:       Ԫ");
        }
        else
        {
            Get_Weight();

            if (Weight_Shiwu < 51000)
                Lcd12864_ShowString (2, 2, ": ");
            else
                Weight_Shiwu = 0;

            Weight_Shiwu *= 50;
            Weight_Shiwu /= 1000;
            getweight = Weight_Shiwu;
            Lcd12864_ShowNum (2, 3, getweight / 10, 4);
            Lcd12864_ShowFloatNum (2, 5, getweight % 10, 1); //��ʾ���һλ
            Lcd12864_ShowNum (3, 3, allprice / 100, 3);
            Lcd12864_WriteData (0x2E);
            Lcd12864_ShowNum (3, 5, allprice % 100, 2);
        }
    }

//        if (KeyNum == 12)
//            menu();
//        else if (KeyNum == 13)
//            Showcost();
//        else if (KeyNum == 14)
//            Showsale();
}


void Timer0_Isr (void) interrupt 1
{
    static unsigned int T0Count;
    TL0 = 0x66;				//���ö�ʱ��ʼֵ
    TH0 = 0xFC;				//���ö�ʱ��ʼֵ
    T0Count++;

    if (T0Count == 20)
    {
        Key_Loop();    //20ms����һ�ΰ�����������
        T0Count = 0;
    }
}