#ifndef _SYSEEPROM_H_
#define _SYSEEPROM_H_
#include "define.h"

u8 IAPRead(u16 addr);
void IAPWrite(u16 addr, u8 dat);
void IAPErase(u16 addr);

#endif