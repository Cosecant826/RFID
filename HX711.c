#include "HX711.h"
#include "intrins.h"


void Delay__hx711_us(void)
{
 _nop_();
 _nop_();
}

unsigned long HX711_Read(void) //增益128
{
 unsigned long count; 
 unsigned char i; 
 HX711_DOUT=1; 
 Delay__hx711_us();
 HX711_SCK=0; 
 count=0; 
 while(HX711_DOUT); 
 for(i=0;i<24;i++) 


 { 
 HX711_SCK=1; 
 count=count<<1; 
 HX711_SCK=0; 
 if(HX711_DOUT)
 count++; 
 } 
 HX711_SCK=1; 
 count=count^0x800000;//第25个脉冲下降沿时，转换数据

 Delay__hx711_us();
 HX711_SCK=0; 
 return(count);
}


void Get_Weight()
{
 Weight_Shiwu = HX711_Read();
 Weight_Shiwu = Weight_Shiwu - Weight_Maopi; //获取净重

 
 Weight_Shiwu = (unsigned int)((float)(Weight_Shiwu*10)/378); 
}

