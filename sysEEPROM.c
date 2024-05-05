#include <STC12C5A60S2.H>
#include <INTRINS.H>
#include "define.h"

void IAPIdle() //¹Ø±Õ¹¦ÄÜ
{
    IAP_CONTR = 0;
    IAP_CMD = 0;
    IAP_TRIG = 0;
    IAP_ADDRH = 0xFF;
    IAP_ADDRL = 0xFF;
}

u8 IAPRead(u16 addr)
{
    u8 dat;
    IAP_CONTR = 0x83;
    IAP_CMD = 1;
    IAP_ADDRH = addr >> 8;
    IAP_ADDRL = addr;
    IAP_TRIG = 0x5A;
    IAP_TRIG = 0xA5;
    _nop_();
    dat = IAP_DATA;
    IAPIdle();
    return dat;
}

void IAPWrite(u16 addr, u8 dat)
{
    IAP_CONTR = 0x83;
    IAP_CMD = 2;
    IAP_ADDRH = addr >> 8;
    IAP_ADDRL = addr;
    IAP_DATA = dat;
    IAP_TRIG = 0x5A;
    IAP_TRIG = 0xA5;
    _nop_();
    IAPIdle();
}

void IAPErase(u16 addr)
{
    IAP_CONTR = 0x83;
    IAP_CMD = 3;
    IAP_ADDRH = addr >> 8;
    IAP_ADDRL = addr;
    IAP_TRIG = 0x5A;
    IAP_TRIG = 0xA5;
    _nop_();
    IAPIdle();
}
