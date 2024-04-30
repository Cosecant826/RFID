#include "lcd12864.h"
#include "Delay.h"
void Lcd12864_CheckBusy()
{
    LcdData = 0xff;
    LcdRS = 0;
    LcdRW = 1;
    LcdEN = 1;
    Delay(5);

    while (P0 & 0x80) {}

    LcdEN = 0;
}

void Lcd12864_WriteCMD(unsigned char CMD)
{
    //  Lcd12864_CheckBusy();
    LcdRS = 0;
    LcdRW = 0;
    LcdData = CMD;
    Delay(1);
    LcdEN = 1;
    Delay(1);
    LcdEN = 0;
}
void Lcd12864_WriteData(unsigned char dat)
{
    // Lcd12864_CheckBusy();
    LcdRS = 1;
    LcdRW = 0;
    LcdData = dat;
    Delay(1);
    LcdEN = 1;
    Delay(1);
    LcdEN = 0;
}

void Lcd12864_Init()
{
    PSB = 1;
    Delay(1);
    Lcd12864_WriteCMD(0x30);
    Delay(1);
    Lcd12864_WriteCMD(0x01);
    Delay(1);
    Lcd12864_WriteCMD(0x0C);
    Delay(1);
}

void Lcd12864_ShowPosition(unsigned char X, unsigned char Y)
{
    unsigned char addr;

    if (X == 0)
    {
        addr = 0x80;
    }

    if (X == 1)
    {
        addr = 0x90;
    }

    if (X == 2)
    {
        addr = 0x88;
    }

    if (X == 3)
    {
        addr = 0x98;
    }

    if (X == 4)
    {
        addr = 0xA0;
    }

    if (X == 5)
    {
        addr = 0xB0;
    }

    if (X == 6)
    {
        addr = 0xA8;
    }

    if (X == 7)
    {
        addr = 0xB8;
    }

    addr += Y;
    Lcd12864_WriteCMD(addr);
}

void Lcd12864_ShowChar(unsigned char X, unsigned char Y, unsigned char cha)
{
    Lcd12864_ShowPosition(X, Y);
    Lcd12864_WriteData(cha);
}

void Lcd12864_ShowString(unsigned char X, unsigned char Y, unsigned char str[])
{
    Lcd12864_ShowPosition(X, Y);

    while (*str != '\0')
    {
        Lcd12864_WriteData(*str);
        str++;
    }
}
int LCD_Pow(int X, int Y)
{
    unsigned char i;
    int Result = 1;

    for (i = 0; i < Y; i++)
    {
        Result *= X;
    }

    return Result;
}
void Lcd12864_ShowNum(unsigned char X, unsigned char Y, unsigned long Num, unsigned char Length)
{
    unsigned char i;
    Lcd12864_ShowPosition(X, Y);

    for (i = Length; i > 0; i--)
    {
        Lcd12864_WriteData(Num / LCD_Pow(10, i - 1) % 10 + '0');
    }
}
void Lcd12864_ShowFloatNum(unsigned char X, unsigned char Y, unsigned long Num, unsigned char Length)
{
    unsigned char i;
    Lcd12864_ShowPosition(X, Y);

    for (i = Length + 1; i > 0; i--)
    {
        if (i == Length + 1)
        {
            Lcd12864_WriteData(0x2E);
        }
        else
        {
            Lcd12864_WriteData(Num / LCD_Pow(10, i - 2) % 10 + '0');
        }
    }
}



void Roll(unsigned char inum)
{
    Lcd12864_WriteCMD(0x34);
    Lcd12864_WriteCMD(0x03);
    Lcd12864_WriteCMD(0x40 | inum);
    Lcd12864_WriteCMD(0x30);
}

void Right(unsigned char i)
{
    for (; i > 0; i--)
    {
        Lcd12864_WriteCMD(0x1C);
    }
}

void Left(unsigned char i)
{
    for (; i > 0; i--)
    {
        Lcd12864_WriteCMD(0x18);
    }
}

void ShutdownDisplay()
{
    Lcd12864_WriteCMD(0x08);
}
void Display()
{
    Lcd12864_WriteCMD(0x0C);
}