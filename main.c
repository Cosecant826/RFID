#include <STC12C5A60S2.H>
unsigned char xdata shu[16]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};	    //IC内容暂存数组变量
unsigned char xdata RevBuffer[4];	  //IC卡的唯一序列号
unsigned char  IDH[4];	//当先检测的IC卡序列号
unsigned char code DefaultKey[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}; //密码
unsigned long UID;
unsigned char  value1[4]={0x33,0,0,0};	//当先检测的IC卡序列号

unsigned char RFID1[16]={0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x01,0xFE,0x01,0xFE};



void check_card(unsigned char block) {	//验证卡  读取卡
    char  status;
    PcdReset();										//复位
    status = PcdRequest(PICC_REQIDL, &RevBuffer[0]); //寻天线区内未进入休眠状态的卡，返回卡片类型 2字节
    if(status != MI_OK) {
        return;
    }						  // http://find.qq.com/index.html?version=1&im_version=5389&width=910&height=610&search_target=0#
    status = PcdAnticoll(&RevBuffer[0]); //防冲撞，返回卡的序列号4字节
    if(status != MI_OK) {
        return;
    }
    IDH[0] = RevBuffer[0];	 //赋值给序列号显示数组对应的序列号
    IDH[1] = RevBuffer[1];
    IDH[2] = RevBuffer[2];
    IDH[3] = RevBuffer[3];					//赋值给序列号显示数组对应的序列号
    status = PcdSelect(&RevBuffer[0]);	 //选卡部分
    if(status != MI_OK) {
        return;
    }
    status = PcdAuthState(0x61, block, DefaultKey, &RevBuffer[0]); // 校验卡密码B
    if(status != MI_OK) {
        return;}
}

void main()
{
		DS1302_Init();
		 Lcd12864_Init();
    Lcd12864_ShowString(0, 0, "ZNJJXT");
    Lcd12864_ShowString(1, 0, "0826");
}
