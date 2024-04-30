#include <INTRINS.H>

void Delay(unsigned int t)	//@11.0592MHz
{
    unsigned char data i, j;

    while (t--)
    {
        _nop_();
        i = 11;
        j = 190;

        do
        {
            while (--j);
        } while (--i);
    }
}

void Delay10us(unsigned int t)	//@11.0592MHz
{
    while (t--)
    {
        unsigned char data i;
        _nop_();
        _nop_();
        _nop_();
        i = 24;

        while (--i);
    }
}

void Delay10u(void)	//@11.0592MHz
{
    unsigned char data i;
    _nop_();
    _nop_();
    _nop_();
    i = 24;

    while (--i);
}

void Delay25ms(void)	//@11.0592MHz
{
    unsigned char data i, j, k;
    i = 2;
    j = 13;
    k = 237;

    do
    {
        do
        {
            while (--k);
        } while (--j);
    } while (--i);
}
