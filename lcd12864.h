#include <STC12C5A60S2.H>

#ifndef __LCD12864_H__
#define __LCD12864_H__

#define LcdData P0
sbit LcdRS = P2 ^ 6;
sbit LcdRW = P2 ^ 5;
sbit LcdEN = P2 ^ 7;
sbit PSB = P3 ^ 2;

void Lcd12864_CheckBusy();
void Lcd12864_WriteCMD(unsigned char CMD);
void Lcd12864_WriteData(unsigned char dat);
void Lcd12864_Init();
void Lcd12864_ShowPosition(unsigned char X, unsigned char Y);
void Lcd12864_ShowChar(unsigned char X, unsigned char Y, unsigned char cha);
void Lcd12864_ShowString(unsigned char X, unsigned char Y, unsigned char str[]);
void Roll(unsigned char inum);
void Display();
void ShutdownDisplay();
void Left(unsigned char i);
void Right(unsigned char i);
void Lcd12864_ShowNum(unsigned char X, unsigned char Y, unsigned long Num, unsigned char Length);
void Lcd12864_ShowFloatNum(unsigned char X, unsigned char Y, unsigned long Num, unsigned char Length);
#endif