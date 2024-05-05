#include <STC12C5A60S2.H>


void UART1_SendByte(unsigned char DAT)
{
	SBUF = DAT;
	while(TI == 0);
	TI=0;
}


void UART1_SendString(unsigned char *DAT, unsigned char len)
{
	unsigned char i;
	for(i = 0; i < len; i++)
	{
		UART1_SendByte(*DAT++);//DAT�ĵ�ַ��һ��*��������ַ�е���
	}
}

void Uart1_Init(void)	//9600bps@12.000MHz
{
	PCON &= 0x7F;		//�����ʲ�����
	SCON = 0x50;		//8λ����,�ɱ䲨����
	AUXR |= 0x04;		//��ʱ��ʱ��1Tģʽ
	BRT = 0xD9;			//���ö�ʱ����ֵ
	AUXR |= 0x01;		//����1ʹ�ö��������ʷ�����Ϊ�����ʷ�����
	AUXR |= 0x10;		//�������������ʷ�����	
	ES = 0;
	EA = 1;
	RI = 0;
	TI = 0;
}

