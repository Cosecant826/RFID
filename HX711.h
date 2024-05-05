#include <STC12C5A60S2.H>
#ifndef __HX711_H__
#define __HX711_H__


extern float Weight_Shiwu,Weight_Maopi;

sbit  HX711_SCK = P2^4;   // 时钟引脚
sbit HX711_DOUT = P2^3;     // 数据引脚
void Delay__hx711_us(void);
unsigned long HX711_Read(void);
void Get_Weight();




#endif