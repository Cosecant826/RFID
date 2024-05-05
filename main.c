#include <STC12C5A60S2.H>
#include "rc522.h"
#include "define.h"
#include "Delay.h"
#include "DS1302.h"
#include "lcd12864.h"
#include "MatrixKey.h"
#include "Timer.h"
#include "HX711.h"

unsigned char xdata shu[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};	 //IC内容暂存数组变量
unsigned char xdata RevBuffer[4];	  //IC卡的唯一序列号
unsigned char xdata DefaultKey[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}; //密码
unsigned char xdata value1[4] = {0x33, 0, 0, 0};
float xdata pricecode[10] = {5.6, 7.2, 10.3};

unsigned char code RFID1[16] = {0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x01, 0xFE, 0x01, 0xFE};
float Weight_Shiwu, Weight_Maopi;


char check_card (unsigned char block)  	//验证卡  读取卡
{
    char  status;
    PcdReset();										//复位
    status = PcdRequest (PICC_REQIDL, &RevBuffer[0]); //寻天线区内未进入休眠状态的卡，返回卡片类型 2字节

    if (status != MI_OK)
        return 1;

    status = PcdAnticoll (&RevBuffer[0]); //防冲撞，返回卡的序列号4字节

    if (status != MI_OK)
        return 1;

    status = PcdSelect (&RevBuffer[0]);	 //选卡部分

    if (status != MI_OK)
        return 1;

    status = PcdAuthState (0x61, block, DefaultKey, &RevBuffer[0]); // 校验卡密码B

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
    Lcd12864_ShowString (0, 0, " - -欢迎光临- - ");
    Lcd12864_ShowString (1, 0, "编码:        ");
    Lcd12864_ShowString (2, 0, "重量:       KG");
    Lcd12864_ShowString (3, 0, "总价:       元");

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
                    Lcd12864_ShowString (0, 0, "品名: 苹果");
                    Lcd12864_ShowString (1, 0, "单价: 5.6 RMB/KG");
                }
                else if (Fruitcode == 10022)
                {
                    Lcd12864_Init();
                    Lcd12864_ShowString (0, 0, "品名: 香蕉");
                    Lcd12864_ShowString (1, 0, "单价: 7.2 RMB/KG");
                }
                else if (Fruitcode == 10023)
                {
                    Lcd12864_Init();
                    Lcd12864_ShowString (0, 0, "品名: 桔子");
                    Lcd12864_ShowString (1, 0, "单价:10.3 RMB/KG");
                }
                else
                {
                    Fruitcode = 0;
                    i = 0;
                    Lcd12864_ShowString (3, 0, "未找到此商品");
                    Delay (1000);
                    Lcd12864_ShowString (1, 0, "编码:        ");
                    Lcd12864_ShowString (3, 0, "            ");
                }

                kgprice = pricecode[Fruitcode - 10021];

                if (Fruitcode)
                {
                    Lcd12864_ShowString (2, 0, "重量:       公斤");
                    Lcd12864_ShowString (3, 0, "价格:       元");
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
                        Lcd12864_ShowFloatNum (2, 5, getweight % 10, 1); //显示最后一位
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
					Lcd12864_ShowString (3, 0, "余额:       元");
					PcdRead(1,&shu);
					price=shu[0]+shu[1]*256;
					Lcd12864_ShowNum (3, 3, price / 100, 3);
                        Lcd12864_WriteData (0x2E);
                        Lcd12864_ShowNum (3, 5, price % 100, 2);
            Delay (1000);
					Lcd12864_ShowString (3, 0, "总价:       元");
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
            Lcd12864_ShowFloatNum (2, 5, getweight % 10, 1); //显示最后一位
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
    TL0 = 0x66;				//设置定时初始值
    TH0 = 0xFC;				//设置定时初始值
    T0Count++;

    if (T0Count == 20)
    {
        Key_Loop();    //20ms调用一次按键驱动函数
        T0Count = 0;
    }
}