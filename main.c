#include <STC12C5A60S2.H>
unsigned char xdata shu[16]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};	    //IC�����ݴ��������
unsigned char xdata RevBuffer[4];	  //IC����Ψһ���к�
unsigned char  IDH[4];	//���ȼ���IC�����к�
unsigned char code DefaultKey[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}; //����
unsigned long UID;
unsigned char  value1[4]={0x33,0,0,0};	//���ȼ���IC�����к�

unsigned char RFID1[16]={0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x01,0xFE,0x01,0xFE};



void check_card(unsigned char block) {	//��֤��  ��ȡ��
    char  status;
    PcdReset();										//��λ
    status = PcdRequest(PICC_REQIDL, &RevBuffer[0]); //Ѱ��������δ��������״̬�Ŀ������ؿ�Ƭ���� 2�ֽ�
    if(status != MI_OK) {
        return;
    }						  // http://find.qq.com/index.html?version=1&im_version=5389&width=910&height=610&search_target=0#
    status = PcdAnticoll(&RevBuffer[0]); //����ײ�����ؿ������к�4�ֽ�
    if(status != MI_OK) {
        return;
    }
    IDH[0] = RevBuffer[0];	 //��ֵ�����к���ʾ�����Ӧ�����к�
    IDH[1] = RevBuffer[1];
    IDH[2] = RevBuffer[2];
    IDH[3] = RevBuffer[3];					//��ֵ�����к���ʾ�����Ӧ�����к�
    status = PcdSelect(&RevBuffer[0]);	 //ѡ������
    if(status != MI_OK) {
        return;
    }
    status = PcdAuthState(0x61, block, DefaultKey, &RevBuffer[0]); // У�鿨����B
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
