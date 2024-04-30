#include <STC12C5A60S2.H>
#include "Delay.h"
unsigned char Key_KeyNumber;

/**
  * @brief  ��ȡ��������
  * @param  ��
  * @retval ���°����ļ��룬��Χ��0,1~4,0��ʾ�ް�������
  */
unsigned char Key(void)
{
    unsigned char Temp = 0;
    Temp = Key_KeyNumber;
    Key_KeyNumber = 0;
    return Temp;
}
unsigned char MatrixKey()
{
    unsigned char KeyNumber = 0;
    P1 = 0xFF;
    P13 = 0;

    if (P17 == 0 && P13 == 0)
    {
        KeyNumber = 1;
    }

    if (P16 == 0 && P13 == 0)
    {
        KeyNumber = 5;
    }

    if (P15 == 0 && P13 == 0)
    {
        KeyNumber = 9;
    }

    if (P14 == 0 && P13 == 0)
    {
        KeyNumber = 13;
    }

    P1 = 0xFF;
    P12 = 0;

    if (P17 == 0 && P12 == 0)
    {
        KeyNumber = 2;
    }

    if (P16 == 0 && P12 == 0)
    {
        KeyNumber = 6;
    }

    if (P15 == 0 && P12 == 0)
    {
        KeyNumber = 10;
    }

    if (P14 == 0 && P12 == 0)
    {
        KeyNumber = 14;
    }

    P1 = 0xFF;
    P11 = 0;

    if (P17 == 0 && P11 == 0)
    {
        KeyNumber = 3;
    }

    if (P16 == 0 && P11 == 0)
    {
        KeyNumber = 7;
    }

    if (P15 == 0 && P11 == 0)
    {
        KeyNumber = 11;
    }

    if (P14 == 0 && P11 == 0)
    {
        KeyNumber = 15;
    }

    P1 = 0xFF;
    P10 = 0;

    if (P17 == 0 && P10 == 0)
    {
        KeyNumber = 4;
    }

    if (P16 == 0 && P10 == 0)
    {
        KeyNumber = 8;
    }

    if (P15 == 0 && P10 == 0)
    {
        KeyNumber = 12;
    }

    if (P14 == 0 && P10 == 0)
    {
        KeyNumber = 16;
    }

    return KeyNumber;
}


void Key_Loop(void)
{
    static unsigned char NowState, LastState;
    LastState = NowState;				//����״̬����
    NowState = MatrixKey();		//��ȡ��ǰ����״̬

    //����ϸ�ʱ��㰴�����£����ʱ���δ���£���������˲�䣬�Դ˱������������ּ��
    if (LastState == 1 && NowState == 0)
    {
        Key_KeyNumber = 1;
    }

    if (LastState == 2 && NowState == 0)
    {
        Key_KeyNumber = 2;
    }

    if (LastState == 3 && NowState == 0)
    {
        Key_KeyNumber = 3;
    }

    if (LastState == 4 && NowState == 0)
    {
        Key_KeyNumber = 4;
    }

    if (LastState == 5 && NowState == 0)
    {
        Key_KeyNumber = 5;
    }

    if (LastState == 6 && NowState == 0)
    {
        Key_KeyNumber = 6;
    }

    if (LastState == 7 && NowState == 0)
    {
        Key_KeyNumber = 7;
    }

    if (LastState == 8 && NowState == 0)
    {
        Key_KeyNumber = 8;
    }

    if (LastState == 9 && NowState == 0)
    {
        Key_KeyNumber = 9;
    }

    if (LastState == 10 && NowState == 0)
    {
        Key_KeyNumber = 10;
    }

    if (LastState == 11 && NowState == 0)
    {
        Key_KeyNumber = 11;
    }

    if (LastState == 12 && NowState == 0)
    {
        Key_KeyNumber = 12;
    }

    if (LastState == 13 && NowState == 0)
    {
        Key_KeyNumber = 13;
    }

    if (LastState == 14 && NowState == 0)
    {
        Key_KeyNumber = 14;
    }

    if (LastState == 15 && NowState == 0)
    {
        Key_KeyNumber = 15;
    }

    if (LastState == 16 && NowState == 0)
    {
        Key_KeyNumber = 16;
    }
}