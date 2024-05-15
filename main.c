#include <STC15F2K60S2.H>

#include "rc522.h"
#include "define.h"
#include "Delay.h"
#include "DS1302.h"
#include "lcd12864.h"
#include "MatrixKey.h"
#include "Timer.h"
#include "HX711.h"
#include "sysEEPROM.h"
#include "uart.h"
#include "syn6288.h"

unsigned char xdata shu[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};	 //IC�����ݴ��������
unsigned char xdata RevBuffer[4];	  //IC����Ψһ���к�
unsigned char xdata DefaultKey[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}; //����
unsigned char xdata value1[4] = {0x33, 0, 0, 0};
float xdata pricecode[10] = {0, 5.6, 7.2, 10.3};

unsigned char code RFID1[16] = {0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x01, 0xFE, 0x01, 0xFE};
float Weight_Shiwu, Weight_Maopi;

u8 KeyNum, skipflag, LastKeyNum[5], i, status, firweight[8], lastweight[8], fruitflag, maxtime[3], costmenuflag[8], costfruit[8], t, costmenurun = 0, maxcostmenurun, maxshow, point[8], code name[8][10] = {"ƻ��", "�㽶", "����", "���", "��Ʒ", "��һ", "�¶�", "�ܼ�"};
float kgprice, sayprice;
u16 Fruitcode = 0,  maxcost = 0, sale[8], allprice = 0, price = 0, getweight, wallet, stepsale[8], richvalue, newprice;





////
////// ����ֵ����
////#define KV_NO                      (0x00)          // ��
////#define KV_UP                      (0x02)          // ��
////#define KV_DN                      (0x04)          // ��
////#define KV_MD                      (0x08)          // ��
////#define KV_LF                      (0x10)          // ��
////#define KV_RT                      (0x20)          // ��
u8 M_MENU_NUM;
u8  PAGE_DISP_NUM = 3;

typedef struct
{
    u8 Cursor;             // ���ֵ(��ǰ���λ��)
    u8 PageNo;             // �˵�ҳ(��ʾ��ʼ��)
    u8 Index;             // �˵�����(��ǰѡ��Ĳ˵���)
    u8 DispNum;             // ��ʾ����(ÿҳ�������ڲ˵���)
    u8 MaxPage;             // ���ҳ��(����ж�������ʾҳ)
} MENU_PRMT;
MENU_PRMT  MenuPrmt;  //ext ern
// �˵�ִ��
typedef struct
{
    u8 *MenuItem;            // �˵���Ŀ����
    u16 stesale;
    u8 showweight;
    void (*ItemHook) (u8 index);                   // Ҫ���еĲ˵�����
} MENU_TABLE;

MENU_TABLE MMenu[8];

void showmenu1()
{
    Lcd12864_Init();
    Lcd12864_ShowString (0, 0, " -= ��ӭ���� =- ");
    Lcd12864_ShowString (1, 0, "����:        ");
    Lcd12864_ShowString (2, 0, "����:       ����");
    Lcd12864_ShowString (3, 0, "�ܼ�:       Ԫ");
}

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


void pay()
{
    Lcd12864_Init();
    Lcd12864_ShowString (0, 0, "��ˢ��");
    KeyNum = 0;

    while (status)
    {
        status = check_card (1);
        KeyNum = Key();

        if (KeyNum) break;
    }

    if (KeyNum)
    {
        KeyNum = 0;
        goto exit;
    }

    PcdRead (1, &shu);
    wallet = shu[0] + shu[1] * 256;

    if (wallet < allprice)
        Lcd12864_ShowString (2, 0, "���㣡��");
    else
    {
        Lcd12864_ShowString (2, 0, "����:       Ԫ");
        Lcd12864_ShowNum (2, 3, allprice / 100, 3);
        Lcd12864_WriteData (0x2E);
        Lcd12864_ShowNum (2, 5, allprice % 100, 2);
        value1[0] = allprice % 256;
        value1[1] = allprice / 256 % 256;
        value1[2] = allprice / 65536 % 256;
        sayprice = allprice / 100.0;
        PcdValue (0xC0, 1, value1);

        for (i = 0; i < 7; i++)
        {
            costmenuflag[i] = 0;
            lastweight[i] += firweight[i];
            firweight[i] = 0;
        }

        if (allprice >= maxcost)
        {
            DS1302_ReadTime();
            maxcost = allprice;
            maxtime[0] = DS1302_Time[1];
            maxtime[1] = DS1302_Time[4];
            maxtime[2] = DS1302_Time[5];
        }

        allprice = 0;
        Lcd12864_ShowString (3, 0, "���:       Ԫ");

        if (RevBuffer[0] == 19 && RevBuffer[1] == 206 && RevBuffer[2] == 59 && RevBuffer[3] == 45)
            Lcd12864_ShowString (1, 0, "�����û������ã�");
        else if (RevBuffer[0] == 3 && RevBuffer[1] == 102 && RevBuffer[2] == 156 && RevBuffer[3] == 40)
            Lcd12864_ShowString (1, 0, "�׿��û������ã�");

        PcdRead (1, &shu);
        wallet = shu[0] + shu[1] * 256;
        Lcd12864_ShowNum (3, 3, wallet / 100, 3);
        Lcd12864_WriteData (0x2E);
        Lcd12864_ShowNum (3, 5, wallet % 100, 2);
        SYN6288_Say ("�ۿ�: %6.2fԪ", sayprice);
    }

    Delay (3000);
exit:
    Fruitcode = 0;
    i = 0;
}


//void null()
//{
//}

//3.��ʼ���˵�����

/**************************************************************************************
* FunctionName   : AppMenuPrmtInit()
* Description    : ��ʼ���˵�����
* EntryParameter : None
* ReturnValue    : None
**************************************************************************************/
void AppMenuPrmtInit (void)
{
    MenuPrmt.Cursor  = 0;           // �������
    MenuPrmt.PageNo = 0;    //        ҳ����
    MenuPrmt.Index   = 0;           //        ��������
}

//4.�˵���ʾ����

/**************************************************************************************
* FunctionName   : App_MenuSet()
* Description    : ���ò˵���ʾ
* EntryParameter : num - ҳ�˵���ʾ�page - ���ҳ��
* ReturnValue    : None
**************************************************************************************/
void App_MenuSet (u8 num, u8 page)
{
    MenuPrmt.DispNum = num;   //        ҳ�����ʾ��Ŀ��
    MenuPrmt.MaxPage = page;  //        ���ҳ��
}

//5.�˵���ʾ

/**************************************************************************************
* FunctionName   : App_MenuDisplay()
* Description    : ��ʾ�˵�
* EntryParameter : page - ��ʾҳ��dispNum - ÿһҳ����ʾ�cursor - ���λ��
* ReturnValue    : None
**************************************************************************************/
void App_MenuDisplay (MENU_TABLE *menu, u8 page, u8 dispNum, u8 cursor)
{
    u8 i;

    for (i = 0;  i < dispNum + 1 && i < t + 1; i++)
    {
        if (cursor == i)
        {
//            LCMDisplayStrRvs(0, (i+1)*2, menu[page+i].MenuItem); // ������ʾ�˵���
            Lcd12864_ShowString (i + 1, 0, " >");
            Lcd12864_ShowString (i + 1, 1, menu[page + i].MenuItem);
            Lcd12864_ShowString (i + 1, 3, ":");
            Lcd12864_ShowString (i + 1, 7, "Ԫ");
            Lcd12864_ShowNum (i + 1, 4, MMenu[page + i].stesale / 100, 3);
            Lcd12864_WriteData (0x2E);
            Lcd12864_ShowNum (i + 1, 6, MMenu[page + i].stesale % 100, 2);
        }
        else
        {
//            LCMDisplayStr(0, (i+1)*2, menu[page+i].MenuItem);         // ��ʾ�˵���
            Lcd12864_ShowString (i + 1, 0, "  ");
            Lcd12864_ShowString (i + 1, 1, menu[page + i].MenuItem);
            Lcd12864_ShowString (i + 1, 3, ":");
            Lcd12864_ShowString (i + 1, 7, "Ԫ");
            Lcd12864_ShowNum (i + 1, 4, MMenu[page + i].stesale / 100, 3);
            Lcd12864_WriteData (0x2E);
            Lcd12864_ShowNum (i + 1, 6, MMenu[page + i].stesale % 100, 2);
        }
    }
}

void App_MenuDisplay1 (MENU_TABLE *menu, u8 page, u8 dispNum)
{
    u8 i;

    for (i = 0;  i < dispNum; i++)
    {
        Lcd12864_ShowString (i + 1, 0, menu[page + i].MenuItem);
        Lcd12864_ShowString (i + 1, 2, ":");
        Lcd12864_ShowString (i + 1, 6, "����");
        Lcd12864_ShowNum (i + 1, 3, MMenu[page + i].showweight / 10, 4);
        Lcd12864_ShowFloatNum (i + 1, 5, MMenu[page + i].showweight % 10, 1); //��ʾ���һλ
    }
}
//6.�˵�����

/**************************************************************************************
* FunctionName   : App_MenuMove()
* Description    : �˵��ƶ�
* EntryParameter : None
* ReturnValue    : ��ȷ�Ϸ���0�����򷵻�1
**************************************************************************************/
u8 App_MenuMove (u8 key)
{
    u8 rValue = 1;

    switch (key)
    {
    case 1:                            // ����
        if (MenuPrmt.Cursor != 0)         // ��겻�ڶ���
        {
            MenuPrmt.Cursor--;             // �������
        }
        else                               // ����ڶ���
        {
            if (MenuPrmt.PageNo != 0)      // ҳ��û�е���С
            {
                MenuPrmt.PageNo--;          // ���Ϸ�
            }
            else
            {
                MenuPrmt.Cursor = MenuPrmt.DispNum - 1;        // ��굽��
                MenuPrmt.PageNo = MenuPrmt.MaxPage - 1; // ���ҳ
            }
        }

        break;

    case 2:                               // ����
        if (MenuPrmt.Cursor < MenuPrmt.DispNum - 1) // ���û�е��ף��ƶ����
        {
            MenuPrmt.Cursor++;           // ��������ƶ�
        }
        else                             // ��굽��
        {
            if (MenuPrmt.PageNo < MenuPrmt.MaxPage - 1)  // ҳ��û�е��ף�ҳ���ƶ�
            {
                MenuPrmt.PageNo++;       // �·�һҳ
            }
            else                         // ҳ��͹�궼���ף����ؿ�ʼҳ
            {
                MenuPrmt.Cursor = 0;
                MenuPrmt.PageNo = 0;
            }
        }

        break;

    case 4:
   
        break;

    case 12:
        pay();

        if (allprice)
            skipflag = 1;
        else  skipflag = 2;

        break;

    case 13:
        skipflag = 2;
        break;

    case 16:
        MenuPrmt.Index = MenuPrmt.Cursor + MenuPrmt.PageNo;      // ����ִ���������
        rValue = 0;
        break;

    default:
        break;
    }

    return rValue;           // ����ִ������
}
void App_MenuMove1 (u8 key)
{
    switch (key)
    {
    case 1:                            // ����
        if (MenuPrmt.PageNo != 0)      // ҳ��û�е���С
        {
            MenuPrmt.PageNo--;          // ���Ϸ�
        }

        break;

    case 2:
        if (MenuPrmt.PageNo < MenuPrmt.MaxPage - 1)  // ҳ��û�е��ף�ҳ���ƶ�
        {
            MenuPrmt.PageNo++;       // �·�һҳ
        }

        break;

    default:
        skipflag = 1;
        break;
    }
}

//7.�˵�ʵ��
/**************************************************************************************
* FunctionName   : App_ManageMenu()
* Description    : ����˵�
* EntryParameter : None
* ReturnValue    : None
**************************************************************************************/
void App_ManageMenu (void)
{
    Lcd12864_ShowString (0, 0, " -= �����嵥 =- ");      // �˵�������ʾ
    App_MenuSet (PAGE_DISP_NUM, M_MENU_NUM + 1 - PAGE_DISP_NUM); // ҳ��ʾ3�7���˵���5ҳ��ʾ
    App_MenuDisplay (MMenu, MenuPrmt.PageNo, MenuPrmt.DispNum, MenuPrmt.Cursor); // ��ʾ�˵���

    if (KeyNum)
    {
        if (App_MenuMove (KeyNum) == 0x00)  // �˵��ƶ�
        {
//            LCMClearScreen (LCM_WHITE);           // ����
            Lcd12864_Init();
            MMenu[MenuPrmt.Index].ItemHook (MenuPrmt.Index);     // ִ����Ӧ��
        }

        KeyNum = 0;                  // �������ֵ
    }
}
void App_ManageMenu1 (void)
{
    Lcd12864_ShowString (0, 0, " -= ����ͳ�� =- ");      // �˵�������ʾ
    App_MenuSet (PAGE_DISP_NUM, M_MENU_NUM + 1 - PAGE_DISP_NUM); // ҳ��ʾ3�7���˵���5ҳ��ʾ
    App_MenuDisplay1 (MMenu, MenuPrmt.PageNo, MenuPrmt.DispNum); // ��ʾ�˵���

    if (KeyNum)
    {
        App_MenuMove1 (KeyNum);
        KeyNum = 0;                  // �������ֵ
    }
}



void displayweight()
{
    Get_Weight();

    if (Weight_Shiwu <= 50000 || Weight_Shiwu >= 62000)
    {
        if (Weight_Shiwu >= 62000) Weight_Shiwu = 0;

        Lcd12864_ShowString (2, 6, "����");
        Weight_Shiwu *= 50;
        Weight_Shiwu /= 1000;
        getweight = Weight_Shiwu;
        Lcd12864_ShowNum (2, 3, getweight / 10, 4);
        Lcd12864_ShowFloatNum (2, 5, getweight % 10, 1); //��ʾ���һλ
    }
    else
    {
        Lcd12864_ShowString (2, 3, "����      ");
        Weight_Shiwu = 0;
    }

    Lcd12864_ShowNum (3, 3, allprice / 100, 3);
    Lcd12864_WriteData (0x2E);
    Lcd12864_ShowNum (3, 5, allprice % 100, 2);
    Lcd12864_ShowString (2, 2, ": ");
}


void checkwallet()
{
    Lcd12864_ShowString (3, 0, "���:       Ԫ");

    if (RevBuffer[0] == 19 && RevBuffer[1] == 206 && RevBuffer[2] == 59 && RevBuffer[3] == 45)
        Lcd12864_ShowString (0, 0, "�����û������ã�");
    else if (RevBuffer[0] == 3 && RevBuffer[1] == 102 && RevBuffer[2] == 156 && RevBuffer[3] == 40)
        Lcd12864_ShowString (0, 0, "�׿��û������ã�");

    PcdRead (1, &shu);
    wallet = shu[0] + shu[1] * 256;
    Lcd12864_ShowNum (3, 3, wallet / 100, 3);
    Lcd12864_WriteData (0x2E);
    Lcd12864_ShowNum (3, 5, wallet % 100, 2);

    while (!status)
        status = check_card (1);

    Delay (1000);
    Fruitcode = 0;
    i = 0;
    showmenu1();
}
void showsale()
{
    skipflag = 0;
    KeyNum = 0;
    Lcd12864_Init();
    costmenurun = 0;

    for (i = 0; i < 7; i++)
    {
        if (lastweight[i])
        {
            //MenuPrmt.Index = MenuPrmt.Cursor + MenuPrmt.PageNo;
            MMenu[costmenurun].MenuItem = name[i];
            MMenu[costmenurun].showweight = lastweight[i];
            costmenurun++;
        }
    }

    M_MENU_NUM = costmenurun;

    if (costmenurun < 3) PAGE_DISP_NUM = costmenurun;
    else PAGE_DISP_NUM = 3;

    AppMenuPrmtInit();

    while (1)
    {
        KeyNum = Key();
        App_ManageMenu1();

        if (skipflag == 1) goto end;
    }

//    Lcd12864_Init();
//    Lcd12864_ShowString (0, 0, " -= ����ͳ�� =- ");
//    for (i = 0; i < 3; i++)
//    {
//        costmenurun++;
//        Lcd12864_ShowString (costmenurun % 4,  0, "����:       ����");
//        Lcd12864_ShowString (costmenurun % 4, 0, name[i]);
//        Lcd12864_ShowNum (costmenurun % 4, 3, lastweight[i] / 10, 4);
//        Lcd12864_ShowFloatNum (costmenurun % 4, 5, lastweight[i] % 10, 1);
//    }
//    KeyNum = 0;
//    while (!KeyNum)
//        KeyNum = Key();
end:
    skipflag = 0;
    costmenurun = 0;
    Fruitcode = 0;
    i = 0;
    showmenu1();
}


void showcost()
{
    Lcd12864_Init();
    Lcd12864_ShowString (0, 0, " -= ������� =- ");
    Lcd12864_ShowString (1, 0, "���:       Ԫ");
    Lcd12864_ShowNum (1, 3, maxcost / 100, 3);
    Lcd12864_WriteData (0x2E);
    Lcd12864_ShowNum (1, 5, maxcost % 100, 2);
    Lcd12864_ShowString (2, 0, " -= ����ʱ�� =- ");
    Lcd12864_ShowString (3, 0, "  ��  ��  ");
    Lcd12864_ShowNum (3, 0, DS1302_Time[3], 2);
    Lcd12864_ShowNum (3, 2, DS1302_Time[4], 2);
    Lcd12864_ShowNum (3, 4, DS1302_Time[5], 2);
    KeyNum = 0;

    while (!KeyNum)
        KeyNum = Key();

    Fruitcode = 0;
    i = 0;
    showmenu1();
}
//void costmenu()
//{
//show:
//    KeyNum = 0;
//    Lcd12864_Init();
//    Lcd12864_ShowString (0, 0, " -= �����嵥 =- ");
//    t = 0;

//    for (i = 0; i < 6; i++)
//    {
//        if (costmenuflag[i])
//        {
//            Lcd12864_ShowString (1, 0, " >");
//            costmenurun++;
//            Lcd12864_ShowString (costmenurun % 4, 1, "    :       Ԫ");
//            Lcd12864_ShowString (costmenurun % 4, 1, name[i]);
//            Lcd12864_ShowNum (costmenurun % 4, 4, stepsale[i] / 100, 3);
//            Lcd12864_WriteData (0x2E);
//            Lcd12864_ShowNum (costmenurun % 4, 6, stepsale[i] % 100, 2);
//            point[t] = i;
//            t++;
//        }
//    }

//    maxcostmenurun = costmenurun;
//    costmenurun++;
//    Lcd12864_ShowString (costmenurun, 1, "�ܼ�:       Ԫ");
//    Lcd12864_ShowNum (costmenurun, 4, allprice / 100, 3);
//    Lcd12864_WriteData (0x2E);
//    Lcd12864_ShowNum (costmenurun, 6, allprice % 100, 2);
//    costmenurun = 1;

//    while (!KeyNum)
//    {
//        KeyNum = Key();

//        if (KeyNum == 13) break;
//        else if (KeyNum == 1)
//        {
//            Lcd12864_ShowString (costmenurun, 0, "  ");

//            if (costmenurun % 3 == 1)
//            {
//                costmenurun = maxcostmenurun;
//                Lcd12864_ShowString (costmenurun, 0, " >");
//            }
//            else
//            {
//                costmenurun--;
//                Lcd12864_ShowString (costmenurun, 0, " >");
//            }
//        }
//        else if (KeyNum == 2)
//        {
//            Lcd12864_ShowString (costmenurun, 0, "  ");
//            costmenurun %= maxcostmenurun;
//            costmenurun++;
//            Lcd12864_ShowString (costmenurun, 0, " >");
//        }
//        else if (KeyNum == 16)
//        {
//            pay();
//            break;
//        }
//        else if (KeyNum == 12)
//        {
//            costmenuflag[point[costmenurun - 1]] = 0;
//            allprice -= stepsale[point[costmenurun - 1]];
//            firweight[point[costmenurun - 1]] = 0;
//            costmenurun = 0;
//            goto show;
//        }

//        KeyNum = 0;
//    }

//    costmenurun = 0;
//    Fruitcode = 0;
//    i = 0;
//    showmenu1();
//}
void rich()
{
    Lcd12864_Init();
    Lcd12864_ShowString (0, 0, " -= ��Ա��ֵ =- ");
    Lcd12864_ShowString (1, 0, "��ˢ��");
    KeyNum = 0;

    while (status)
    {
        status = check_card (1);
        KeyNum = Key();

        if (KeyNum) break;
    }

    if (KeyNum)
    {
        KeyNum = 0;
        goto exit;
    }

    if (RevBuffer[0] == 19 && RevBuffer[1] == 206 && RevBuffer[2] == 59 && RevBuffer[3] == 45)
        richvalue = 10000;
    else if (RevBuffer[0] == 3 && RevBuffer[1] == 102 && RevBuffer[2] == 156 && RevBuffer[3] == 40)
        richvalue = 20000;

    value1[0] = richvalue % 256;
    value1[1] = richvalue / 256 % 256;
    value1[2] = richvalue / 65536 % 256;
    PcdValue (0xC1, 1, value1);
    Lcd12864_Init();
    Lcd12864_ShowString (0, 0, " -= ��Ա��ֵ =- ");
    Lcd12864_ShowString (1, 0, "��ֵ�ɹ�");
    Lcd12864_ShowString (2, 0, "��ֵ        Ԫ");
    Lcd12864_ShowString (3, 0, "���:       Ԫ");
    Lcd12864_ShowNum (2, 3, richvalue / 100, 3);
    Lcd12864_WriteData (0x2E);
    Lcd12864_ShowNum (2, 5, richvalue % 100, 2);
    PcdRead (1, &shu);
    wallet = shu[0] + shu[1] * 256;
    Lcd12864_ShowNum (3, 3, wallet / 100, 3);
    Lcd12864_WriteData (0x2E);
    Lcd12864_ShowNum (3, 5, wallet % 100, 2);
    Delay (1000);
exit:
    Fruitcode = 0;
    i = 0;
    showmenu1();
}

void addcode()
{
    i = 0;
    Fruitcode = 0;
    Lcd12864_Init();
    Lcd12864_ShowString (0, 0, " -= �����Ʒ =- ");
    Lcd12864_ShowString (1, 0, "����:        ");
    KeyNum = 0;

    while (!KeyNum)
    {
        KeyNum = Key();

        if (KeyNum <= 10 && KeyNum > 0)
        {
            if (i < 5)
            {
                Lcd12864_WriteCMD (0x93 + i / 2);

                if (i % 2)
                    Lcd12864_WriteData (LastKeyNum[i - 1] % 10 + 48);

                Lcd12864_WriteData (KeyNum % 10 + 48);
                Fruitcode *= 10;
                Fruitcode += KeyNum % 10;
                LastKeyNum[i] = KeyNum;
                i++;
            }
        }
        else if (KeyNum == 11)
        {
            if (i > 0)
            {
                i--;
                Fruitcode /= 10;
                Lcd12864_ShowString (1, 3 + i / 2, "  ");

                if (i % 2)
                    Lcd12864_ShowChar (1, 3 + i / 2, LastKeyNum[i - 1] % 10 + 48);
            }
        }
        else if (KeyNum == 12)
        {
            i = 0;
            newprice = 0;
            Lcd12864_ShowString (2, 0, "����: 00.0CNY/KG");
            KeyNum = 0;

            while (!KeyNum)
            {
                KeyNum = Key();

                if (KeyNum <= 10 && KeyNum > 0)
                {
                    if (i < 4)
                    {
                        Lcd12864_WriteCMD (0x8B + i / 2);

                        if (i % 2)
                            Lcd12864_WriteData (LastKeyNum[i - 1] % 10 + 48);

                        if (i == 2)
                        {
                            Lcd12864_WriteData ('.');
                            i++;
                        }

                        Lcd12864_WriteData (KeyNum % 10 + 48);
                        newprice *= 10;
                        newprice += KeyNum % 10;
                        LastKeyNum[i] = KeyNum;
                        i++;
                    }
                }
                else if (KeyNum == 11)
                {
                    switch (i)
                    {
                    case 1:
                        i--;
                        newprice /= 10;
                        Lcd12864_ShowString (2, 3, "00");
                        break;

                    case 2:
                        i--;
                        newprice /= 10;
                        Lcd12864_ShowChar (2, 3, LastKeyNum[i - 1] % 10 + 48);
                        Lcd12864_WriteData ('0');
                        break;

                    case 4:
                        i -= 2;
                        newprice /= 10;
                        Lcd12864_ShowString (2, 4, ".0");
                        break;

                    default:
                        break;
                    }
                }
                else if (KeyNum == 12)
                {
                    switch (i)
                    {
                    case 1:
                        newprice *= 100;
                        break;

                    case 2:
                        newprice *= 10;
                        break;

                    default:
                        break;
                    }

                    t = IAPRead (600);
                    IAPWrite (t, Fruitcode % 256);
                    IAPWrite (t + 1, Fruitcode / 256);
                    IAPWrite (t + 2, newprice);
                    IAPErase (600);
                    IAPWrite (600, t + 3);
                    Lcd12864_ShowString (3, 2, "��ӳɹ�");
                    Delay (1500);
                    goto exit;
                }
                else if (KeyNum)
                {
                    i = 0;
                    Fruitcode = 0;
                    Lcd12864_Init();
                    Lcd12864_ShowString (0, 0, " -= �����Ʒ =- ");
                    Lcd12864_ShowString (1, 0, "����:        ");
                    KeyNum = 0;
                    break;
                }

                KeyNum = 0;
            }
        }
        else if (KeyNum) break;

        KeyNum = 0;
    }

exit:
    Fruitcode = 0;
    i = 0;
    showmenu1();
}

void deletecost (u8 index)
{
    costmenuflag[point[index + 1]] = 0;
    allprice -= stepsale[point[index + 1]];
    stepsale[point[index + 1]] = 0;
    firweight[point[index + 1]] = 0;
    skipflag = 1;
}
void deleteall()
{
    for (i = 0; i < 7; i++)
    {
        costmenuflag[i] = 0;
        stepsale[i] = 0;
        firweight[i] = 0;
        t++;
        point[t] = 0;
    }

    allprice = 0;
    skipflag = 1;
}
void costmenu()
{
start:	skipflag = 0;
    KeyNum = 0;
    Lcd12864_Init();
    t = 0;
    costmenurun = 0;

    for (i = 0; i < 7; i++)
    {
        if (costmenuflag[i])
        {
            //MenuPrmt.Index = MenuPrmt.Cursor + MenuPrmt.PageNo;
            MMenu[costmenurun].MenuItem = name[i];
            MMenu[costmenurun].stesale = stepsale[i];
            MMenu[costmenurun].ItemHook = deletecost;
            t++;
            costmenurun++;
            point[t] = i;
        }
    }

    MMenu[costmenurun].MenuItem = name[7];
    MMenu[costmenurun].stesale = allprice;
    MMenu[costmenurun].ItemHook = deleteall;
    M_MENU_NUM = ++costmenurun;

    if (costmenurun < 3) PAGE_DISP_NUM = costmenurun;
    else PAGE_DISP_NUM = 3;

    AppMenuPrmtInit();

    while (1)
    {
        KeyNum = Key();
        App_ManageMenu();

        if (skipflag == 1) goto start;
        else if (skipflag == 2) goto end;
    }

end:
    skipflag = 0;
    costmenurun = 0;
    Fruitcode = 0;
    i = 0;
    showmenu1();
}


void main()
{
    P0M0 = 0X00;
    P0M1 = 0X00;
    P1M0 = 0X00;
    P1M1 = 0X00;
    P2M0 = 0X00;
    P2M1 = 0X00;
    P3M0 = 0X00;
    P3M1 = 0X00;
    P4M0 = 0X00;
    P4M1 = 0X00;
    P5M0 = 0X00;
    P5M1 = 0X00;
    P6M0 = 0X00;
    P6M1 = 0X00;
    P7M0 = 0X00;
    P7M1 = 0X00;
    DS1302_Init();
    DS1302_SetTime();
    UART1_Init();
    Lcd12864_Init();
    Weight_Maopi = HX711_Read();

    for (i = 0; i < 7; i++)
    {
        costmenuflag[i] = 0;
        stepsale[i] = 0;
        firweight[i] = 0;
        lastweight[i] = 0;
    }

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
    Fruitcode = 0;
    i = 0;
    Timer0_Init();
    showmenu1();

    while (1)
    {
        KeyNum = Key();
        status = check_card (1);

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
                for (i = 0; i < 7; i++)
                {
                    if (Fruitcode == (IAPRead (3 * i + 1) + 256 * IAPRead (3 * i + 2)))
                    {
                        fruitflag = i + 1;
                        break;
                    }
                    else  fruitflag = 0;
                }

                if (fruitflag)
                {
                }
                else
                {
                    Lcd12864_ShowString (3, 0, "δ�ҵ�����Ʒ    ");
                    Delay (1000);
                    Fruitcode = 0;
                    i = 0;
                    showmenu1();
                }

                kgprice = (float) IAPRead (3 * fruitflag) / 10.0;

                if (Fruitcode)
                {
                    Lcd12864_Init();
                    Lcd12864_ShowString (0, 0, "Ʒ��: ");
                    Lcd12864_ShowString (1, 0, "����:     CNY/KG");
                    Lcd12864_ShowString (0, 3, name[i]);
                    Lcd12864_ShowNum (1, 3, IAPRead (3 * fruitflag) / 10, 2);
                    Lcd12864_ShowFloatNum (1, 4, IAPRead (3 * fruitflag) % 10, 1);
                    Lcd12864_ShowString (2, 0, "����:       ����");
                    Lcd12864_ShowString (3, 0, "�۸�:       Ԫ");
                    KeyNum = 0;

                    while (!KeyNum)
                    {
                        KeyNum = Key();
                        Get_Weight();

                        if (Weight_Shiwu <= 52000)
                        {
//                            Lcd12864_ShowString (2, 2, ": ");
                        }
                        else
                        {
                            Weight_Shiwu = 0;
                            price = 0;
                        }

                        Lcd12864_ShowString (2, 6, "����");
                        Weight_Shiwu *= 50;
                        Weight_Shiwu /= 1000;
                        getweight = Weight_Shiwu;
                        Lcd12864_ShowNum (2, 3, getweight / 10, 4);
                        Lcd12864_ShowFloatNum (2, 5, getweight % 10, 1); //��ʾ���һλ
                        price = (unsigned int) (kgprice * getweight * 10);
                        Lcd12864_ShowNum (3, 3, price / 100, 3);
                        Lcd12864_WriteData (0x2E);
                        Lcd12864_ShowNum (3, 5, price % 100, 2);

                        if (KeyNum == 16)
                        {
                            allprice += price;
                            pay();
                            showmenu1();
                        }
                        else if (KeyNum == 13)
                        {
                            Fruitcode = 0;
                            i = 0;
                            showmenu1();
                        }
                        else if (KeyNum == 12)
                        {
                            stepsale[i] += price;
                            firweight[i] += getweight;
                            allprice += price;

                            if (price > 0)
                                costmenuflag[i] = 1;

                            Fruitcode = 0;
                            i = 0;
                            showmenu1();
                        }
                        else KeyNum = 0;
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
                Lcd12864_ShowString (1, 3 + i / 2, "  ");

                if (i % 2)
                    Lcd12864_ShowChar (1, 3 + i / 2, LastKeyNum[i - 1] % 10 + 48);
            }
        }
        else if (KeyNum == 12)
            costmenu();
        else if (KeyNum == 16)
            addcode();
        else if (KeyNum == 13)
            rich();
        else if (KeyNum == 14)
            showcost();
        else if (KeyNum == 15)
            showsale();
        else if (!status)
            checkwallet();
        else
            displayweight();
    }
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