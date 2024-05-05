#ifndef __UART_H__
#define __UART_H__

void UART1_SendByte(unsigned char DAT);
void UART1_SendString(unsigned char *DAT, unsigned char len);
void UART1_Init(void);


#endif