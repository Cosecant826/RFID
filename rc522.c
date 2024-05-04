#include "rc522.h"
unsigned char flag;
void delay_ns(unsigned int ns) {
    unsigned int i;
    for(i = 0; i < ns; i++) {
        nop();
        nop();
        nop();
    }
}

unsigned char SPIReadByte(void) {
    unsigned char SPICount;                                       // Counter used to clock out the data
    unsigned char SPIData;
    SPIData = 0;
    for(SPICount = 0; SPICount < 8; SPICount++) {                 // Prepare to clock in the data to be read
        SPIData <<= 1;                                              // ���ƶ����Ѿ���������λ
        CLR_SPI_CK;
        nop();
        nop();                                   // Raise the clock to clock the data out of the MAX7456
        if(STU_SPI_MISO) {									//MISO:Read�����豸�����豸
            SPIData |= 0x01;												//�½��ش���
        }
        SET_SPI_CK;
        nop();
        nop();                                               // Drop the clock ready for the next bit
    }                                                             // and loop back
    return (SPIData);                              // Finally return the read data
}
void SPIWriteByte(unsigned char SPIData) {
    unsigned char  SPICount;                                       // Counter used to clock out the data
    for(SPICount = 0; SPICount < 8; SPICount++) {
        if(SPIData & 0x80) {						//�ж����λ���ȷ������λ
            SET_SPI_MOSI;								//MOSI:Write,���豸����豸
        } else {
            CLR_SPI_MOSI;
        }
        nop();
        nop();
        CLR_SPI_CK;
        nop();
        nop();
        SET_SPI_CK;
        nop();
        nop();
        SPIData <<= 1;
    }
}
/////////////////////////////////////////////////////////////////////
//��    �ܣ�Ѱ��
//����˵��: req_code[IN]:Ѱ����ʽ
//                0x52 = Ѱ��Ӧ�������з���14443A��׼�Ŀ�
//                0x26 = Ѱδ��������״̬�Ŀ�
//          pTagType[OUT]����Ƭ���ʹ���
//                0x4400 = Mifare_UltraLight
//                0x0400 = Mifare_One(S50)
//                0x0200 = Mifare_One(S70)
//                0x0800 = Mifare_Pro(X)
//                0x4403 = Mifare_DESFire
//��    ��: �ɹ�����MI_OK
/////////////////////////////////////////////////////////////////////
char PcdRequest(unsigned char  req_code, unsigned char *pTagType) { //Ѱ��
    char  status;
    unsigned int  unLen;
    unsigned char  ucComMF522Buf[MAXRLEN];
    ClearBitMask(Status2Reg, 0x08); //���RC522�Ĵ���λ��������������
    WriteRawRC(BitFramingReg, 0x07); //дRC632�Ĵ���
    SetBitMask(TxControlReg, 0x03); //��RC522�Ĵ���λ
    ucComMF522Buf[0] = req_code;  	//reg_code��14443�е�REQA���0x26��
    status = PcdComMF522(PCD_TRANSCEIVE, ucComMF522Buf, 1, ucComMF522Buf, &unLen);
    //status=0,��ζѰ���ɹ�
    if((status == MI_OK) && (unLen == 0x10)) {
        *pTagType     = ucComMF522Buf[0];//*pTagType����Ƭ���ʹ���
        *(pTagType + 1) = ucComMF522Buf[1];
    } else {
        status = MI_ERR;      //status=-2,��ζѰ��ʧ��
    }
    return status;
}
/////////////////////////////////////////////////////////////////////
//��    �ܣ�����ײ
//����˵��: pSnr[OUT]:��Ƭ���кţ�4�ֽ�
//��    ��: �ɹ�����MI_OK
/////////////////////////////////////////////////////////////////////
char PcdAnticoll(unsigned char *pSnr) {
    char  status;
    unsigned char  i, snr_check = 0;
    unsigned int  unLen;
    unsigned char  ucComMF522Buf[MAXRLEN]; //MAXRLLEN=18
    ClearBitMask(Status2Reg, 0x08);
    WriteRawRC(BitFramingReg, 0x00); //000 ָʾ���һ��
    ClearBitMask(CollReg, 0x80); //�ֽڵ�����λ��������1
    ucComMF522Buf[0] = PICC_ANTICOLL1;//14443�з���ײ���
    ucComMF522Buf[1] = 0x20;
    status = PcdComMF522(PCD_TRANSCEIVE, ucComMF522Buf, 2, ucComMF522Buf, &unLen);
    if(status == MI_OK) { //����ײ�ɹ�
        for(i = 0; i < 4; i++) {
            *(pSnr + i)  = ucComMF522Buf[i]; //pSnr[OUT]������ID
            snr_check ^= ucComMF522Buf[i];
        }
        if(snr_check != ucComMF522Buf[i]) {
            status = MI_ERR;
        }
    }
    SetBitMask(CollReg, 0x80);
    return status;//status=0������ײ�ɹ�;status=-2������ײʧ��
}

/////////////////////////////////////////////////////////////////////
//��    �ܣ�ѡ����Ƭ
//����˵��: pSnr[IN]:��Ƭ���кţ�4�ֽ�
//��    ��: �ɹ�����MI_OK
/////////////////////////////////////////////////////////////////////
char PcdSelect(unsigned char *pSnr) { //pSnr[OUT]������ID
    char  status;
    unsigned char  i;
    unsigned int  unLen;
    unsigned char  ucComMF522Buf[MAXRLEN];
    ucComMF522Buf[0] = PICC_ANTICOLL1;
    ucComMF522Buf[1] = 0x70;
    ucComMF522Buf[6] = 0;
    for(i = 0; i < 4; i++) {
        ucComMF522Buf[i + 2] = *(pSnr + i);
        ucComMF522Buf[6]  ^= *(pSnr + i);
    }
    CalulateCRC(ucComMF522Buf, 7, &ucComMF522Buf[7]);
    ClearBitMask(Status2Reg, 0x08);
    status = PcdComMF522(PCD_TRANSCEIVE, ucComMF522Buf, 9, ucComMF522Buf, &unLen);
    if((status == MI_OK) && (unLen == 0x18)) {
        status = MI_OK;
    } else {
        status = MI_ERR;
    }
    return status;
}

/////////////////////////////////////////////////////////////////////
//��    �ܣ���֤��Ƭ����
//����˵��: auth_mode[IN]: ������֤ģʽ
//                 0x60 = ��֤A��Կ
//                 0x61 = ��֤B��Կ
//          addr[IN]�����ַ
//          pKey[IN]������
//          pSnr[IN]����Ƭ���кţ�4�ֽ�
//��    ��: �ɹ�����MI_OK
/////////////////////////////////////////////////////////////////////
char PcdAuthState(unsigned char  auth_mode, unsigned char  addr, unsigned char *pKey, unsigned char *pSnr) {
    char  status;
    unsigned int  unLen;
    unsigned char  i, ucComMF522Buf[MAXRLEN];
    ucComMF522Buf[0] = auth_mode;
    ucComMF522Buf[1] = addr;
    for(i = 0; i < 6; i++) {
        ucComMF522Buf[i + 2] = *(pKey + i);
    }
    for(i = 0; i < 6; i++) {
        ucComMF522Buf[i + 8] = *(pSnr + i);
    }
//   memcpy(&ucComMF522Buf[2], pKey, 6);
//   memcpy(&ucComMF522Buf[8], pSnr, 4);
    status = PcdComMF522(PCD_AUTHENT, ucComMF522Buf, 12, ucComMF522Buf, &unLen);
    if((status != MI_OK) || (!(ReadRawRC(Status2Reg) & 0x08))) {
        status = MI_ERR;
    }
    return status;
}

/////////////////////////////////////////////////////////////////////
//��    �ܣ���ȡM1��һ������
//����˵��: addr[IN]�����ַ
//          pData[OUT]�����������ݣ�16�ֽ�
//��    ��: �ɹ�����MI_OK
/////////////////////////////////////////////////////////////////////
char PcdRead(unsigned char addr, unsigned char *pData) {
    char status;
    unsigned int  unLen;
    unsigned char i, ucComMF522Buf[MAXRLEN];
    ucComMF522Buf[0] = PICC_READ;
    ucComMF522Buf[1] = addr;
    CalulateCRC(ucComMF522Buf, 2, &ucComMF522Buf[2]);
    status = PcdComMF522(PCD_TRANSCEIVE, ucComMF522Buf, 4, ucComMF522Buf, &unLen);
    if((status == MI_OK) && (unLen == 0x90))
//   {   memcpy(pData, ucComMF522Buf, 16);   }
    {
        for(i = 0; i < 16; i++) {
            *(pData + i) = ucComMF522Buf[i];
        }
    } else {
        status = MI_ERR;
    }
    return status;
}

/////////////////////////////////////////////////////////////////////
//��    �ܣ�д���ݵ�M1��һ��
//����˵��: addr[IN]�����ַ
//          pData[IN]��д������ݣ�16�ֽ�
//��    ��: �ɹ�����MI_OK
/////////////////////////////////////////////////////////////////////
char PcdWrite(unsigned char addr, unsigned char *pData) {
    char status;
    unsigned int  unLen;
    unsigned char i, ucComMF522Buf[MAXRLEN];
    ucComMF522Buf[0] = PICC_WRITE;
    ucComMF522Buf[1] = addr;
    CalulateCRC(ucComMF522Buf, 2, &ucComMF522Buf[2]);
    status = PcdComMF522(PCD_TRANSCEIVE, ucComMF522Buf, 4, ucComMF522Buf, &unLen);
    if((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A)) {
        status = MI_ERR;
    }
    if(status == MI_OK) {
        //memcpy(ucComMF522Buf, pData, 16);
        for(i = 0; i < 16; i++) {
            ucComMF522Buf[i] = *(pData + i);
        }
        CalulateCRC(ucComMF522Buf, 16, &ucComMF522Buf[16]);
        status = PcdComMF522(PCD_TRANSCEIVE, ucComMF522Buf, 18, ucComMF522Buf, &unLen);
        if((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A)) {
            status = MI_ERR;
        }
    }
    return status;
}

/////////////////////////////////////////////////////////////////////
//��    �ܣ����Ƭ��������״̬
//��    ��: �ɹ�����MI_OK
/////////////////////////////////////////////////////////////////////
//char PcdHalt(void)
//{
//    char data status;
//    unsigned int  unLen;
//    unsigned char  ucComMF522Buf[MAXRLEN];
//
//    ucComMF522Buf[0] = PICC_HALT;
//    ucComMF522Buf[1] = 0;
//    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
//
//    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);
//
//    return MI_OK;
//}

/////////////////////////////////////////////////////////////////////
//��MF522����CRC16����
//*pIndata:ǰ����ҪУ��ĵ�ַ���£�
//len:��ҪУ������ݳ���
//*pOutData:CRCУ��λ���ݱ���ĵ�ַ
/////////////////////////////////////////////////////////////////////
void CalulateCRC(unsigned char *pIndata, unsigned char  len, unsigned char *pOutData) {
    unsigned char  i, n;
    ClearBitMask(DivIrqReg, 0x04);
    WriteRawRC(CommandReg, PCD_IDLE);
    SetBitMask(FIFOLevelReg, 0x80);
    for(i = 0; i < len; i++) {
        WriteRawRC(FIFODataReg, *(pIndata + i));
    }
    WriteRawRC(CommandReg, PCD_CALCCRC);
    i = 0xFF;
    do {
        n = ReadRawRC(DivIrqReg);
        i--;
    } while((i != 0) && !(n & 0x04));
    pOutData[0] = ReadRawRC(CRCResultRegL);
    pOutData[1] = ReadRawRC(CRCResultRegM);
}

/////////////////////////////////////////////////////////////////////
//��    �ܣ���λRC522
//��    ��: �ɹ�����MI_OK
/////////////////////////////////////////////////////////////////////
char PcdReset(void) {
    //PORTD|=(1<<RC522RST);
    SET_RC522RST;
    delay_ns(10);
    //PORTD&=~(1<<RC522RST);
    CLR_RC522RST;
    delay_ns(10);
    //PORTD|=(1<<RC522RST);
    SET_RC522RST;
    delay_ns(10);
    WriteRawRC(CommandReg, PCD_RESETPHASE);
    delay_ns(10);
    WriteRawRC(ModeReg, 0x3D);           //��Mifare��ͨѶ��CRC��ʼֵ0x6363
    WriteRawRC(TReloadRegL, 30);
    WriteRawRC(TReloadRegH, 0);
    WriteRawRC(TModeReg, 0x8D);
    WriteRawRC(TPrescalerReg, 0x3E);
    WriteRawRC(TxAutoReg, 0x40); //����Ҫ
    return MI_OK;
}
//////////////////////////////////////////////////////////////////////
//����RC632�Ĺ�����ʽ
//////////////////////////////////////////////////////////////////////
//char M500PcdConfigISOType(unsigned char  type)
//{
//   if (type == 'A')                     //ISO14443_A
//   {
//       ClearBitMask(Status2Reg,0x08);
//       WriteRawRC(ModeReg,0x3D);//3F
//       WriteRawRC(RxSelReg,0x86);//84
//       WriteRawRC(RFCfgReg,0x7F);   //4F
//   	   WriteRawRC(TReloadRegL,30);//tmoLength);// TReloadVal = 'h6a =tmoLength(dec)
//	   WriteRawRC(TReloadRegH,0);
//       WriteRawRC(TModeReg,0x8D);
//	   WriteRawRC(TPrescalerReg,0x3E);
//	   delay_ns(1000);
//       PcdAntennaOn();
//   }
//   else{ return -1; }
//
//   return MI_OK;
//}
/////////////////////////////////////////////////////////////////////
//��    �ܣ���RC632�Ĵ���
//����˵����Address[IN]:�Ĵ�����ַ
//��    �أ�������ֵ
/////////////////////////////////////////////////////////////////////
unsigned char ReadRawRC(unsigned char  Address) {
    unsigned char  ucAddr;
    unsigned char  ucResult = 0;
    CLR_SPI_CS;
    ucAddr = ((Address << 1) & 0x7E) | 0x80;
    SPIWriteByte(ucAddr);
    ucResult = SPIReadByte();
    SET_SPI_CS;
    return ucResult;
}

/////////////////////////////////////////////////////////////////////
//��    �ܣ�дRC632�Ĵ���
//����˵����Address[IN]:�Ĵ�����ַ
//          value[IN]:д���ֵ
/////////////////////////////////////////////////////////////////////
void WriteRawRC(unsigned char  Address, unsigned char  value) {
    unsigned char  ucAddr;
    CLR_SPI_CS;
    ucAddr = ((Address << 1) & 0x7E);
    SPIWriteByte(ucAddr);
    SPIWriteByte(value);
    SET_SPI_CS;
}
/////////////////////////////////////////////////////////////////////
//��    �ܣ���RC522�Ĵ���λ
//����˵����reg[IN]:�Ĵ�����ַ
//          mask[IN]:��λֵ
/////////////////////////////////////////////////////////////////////
void SetBitMask(unsigned char  reg, unsigned char  mask) {
    char  tmp = 0x0;
    tmp = ReadRawRC(reg);
    WriteRawRC(reg, tmp | mask); // set bit mask
}

/////////////////////////////////////////////////////////////////////
//��    �ܣ���RC522�Ĵ���λ
//����˵����reg[IN]:�Ĵ�����ַ
//          mask[IN]:��λֵ
/////////////////////////////////////////////////////////////////////
void ClearBitMask(unsigned char  reg, unsigned char  mask) {
    char  tmp = 0x0;
    tmp = ReadRawRC(reg);
    WriteRawRC(reg, tmp & ~mask);  // clear bit mask
}

/////////////////////////////////////////////////////////////////////
//��    �ܣ�ͨ��RC522��ISO14443��ͨѶ
//����˵����Command[IN]:RC522������
//          pInData[IN]:ͨ��RC522���͵���Ƭ������    14443-A
//          InLenByte[IN]:�������ݵ��ֽڳ���
//          pOutData[OUT]:���յ��Ŀ�Ƭ��������
//          *pOutLenBit[OUT]:�������ݵ�λ����
/////////////////////////////////////////////////////////////////////
char PcdComMF522(unsigned char  Command,
                 unsigned char *pInData,
                 unsigned char  InLenByte,
                 unsigned char *pOutData,
                 unsigned int *pOutLenBit) {
    char  status = MI_ERR;
    unsigned char  irqEn   = 0x00;
    unsigned char  waitFor = 0x00;
    unsigned char  lastBits;
    unsigned char  n;
    unsigned int  i;
    switch(Command) {
    case PCD_AUTHENT:
        irqEn   = 0x12;
        waitFor = 0x10;
        break;
    case PCD_TRANSCEIVE:
        irqEn   = 0x77;
        waitFor = 0x30;
        break;
    default:
        break;
    }
    WriteRawRC(ComIEnReg, irqEn | 0x80);
    ClearBitMask(ComIrqReg, 0x80);
    WriteRawRC(CommandReg, PCD_IDLE);
    SetBitMask(FIFOLevelReg, 0x80);
    for(i = 0; i < InLenByte; i++) {
        WriteRawRC(FIFODataReg, pInData[i]);
    }
    WriteRawRC(CommandReg, Command);
    if(Command == PCD_TRANSCEIVE) {
        SetBitMask(BitFramingReg, 0x80);
    }
    //i = 600;//����ʱ��Ƶ�ʵ���������M1�����ȴ�ʱ��25ms
    i = 2000;
    do {
        n = ReadRawRC(ComIrqReg);
        i--;
    } while((i != 0) && !(n & 0x01) && !(n & waitFor));
    ClearBitMask(BitFramingReg, 0x80);
    if(i != 0) {
        if(!(ReadRawRC(ErrorReg) & 0x1B)) {
            status = MI_OK;
            if(n & irqEn & 0x01) {
                status = MI_NOTAGERR;
            }
            if(Command == PCD_TRANSCEIVE) {
                n = ReadRawRC(FIFOLevelReg);
                lastBits = ReadRawRC(ControlReg) & 0x07;
                if(lastBits) {
                    *pOutLenBit = (n - 1) * 8 + lastBits;
                } else {
                    *pOutLenBit = n * 8;
                }
                if(n == 0) {
                    n = 1;
                }
                if(n > MAXRLEN) {
                    n = MAXRLEN;
                }
                for(i = 0; i < n; i++) {
                    pOutData[i] = ReadRawRC(FIFODataReg);
                }
            }
        } else {
            status = MI_ERR;
        }
    }
    SetBitMask(ControlReg, 0x80);          // stop timer now
    WriteRawRC(CommandReg, PCD_IDLE);
    return status;
}

/////////////////////////////////////////////////////////////////////
//��������
//ÿ��������ر����շ���֮��Ӧ������1ms�ļ��
/////////////////////////////////////////////////////////////////////
//void PcdAntennaOn(void)
//{
//    unsigned char  i;
//    i = ReadRawRC(TxControlReg);
//    if (!(i & 0x03))
//    {
//        SetBitMask(TxControlReg, 0x03);
//    }
//}


/////////////////////////////////////////////////////////////////////
//�ر�����
/////////////////////////////////////////////////////////////////////
//void PcdAntennaOff(void)
//{
//	ClearBitMask(TxControlReg, 0x03);
//}
//
///////////////////////////////////////////////////////////////////////
////��    �ܣ��ۿ�ͳ�ֵ
////����˵��: dd_mode[IN]��������
////               0xC0 = �ۿ�
////               0xC1 = ��ֵ
////          addr[IN]��Ǯ����ַ
////          pValue[IN]��4�ֽ���(��)ֵ����λ��ǰ
////��    ��: �ɹ�����MI_OK
///////////////////////////////////////////////////////////////////////

char PcdValue(unsigned char dd_mode,unsigned char addr,unsigned char *pValue)
{
    char status;
    unsigned int  unLen;
    unsigned char ucComMF522Buf[MAXRLEN];
    unsigned char i;

    ucComMF522Buf[0] = dd_mode;
    ucComMF522Buf[1] = addr;
    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);

    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);
	 flag=1;
    if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
    {   status = MI_ERR;   }

    if (status == MI_OK)
    {
        //memcpy(ucComMF522Buf, pValue, 4);
        for (i=0; i<16; i++)
        {    ucComMF522Buf[i] = *(pValue+i);   }
        CalulateCRC(ucComMF522Buf,4,&ucComMF522Buf[4]);
        unLen = 0;
        status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,6,ucComMF522Buf,&unLen);
		if (status != MI_ERR)
        {    status = MI_OK;    }
    }

    if (status == MI_OK)
    {
        ucComMF522Buf[0] = PICC_TRANSFER;
        ucComMF522Buf[1] = addr;
        CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);

        status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

        if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
        {   status = MI_ERR;   }
    }
    return status;
}