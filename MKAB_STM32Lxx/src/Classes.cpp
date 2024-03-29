//=============================================================================
#include <Classes.h>
#include <flash_map.h>
#include <stm32l1xx_flash.h>
#include <stm32l1xx_rcc.h>
#include <stm32l1xx_usart.h>
//=============================================================================
//*****************************************************************************
//*** Class TObject
//*****************************************************************************
//=============================================================================
//  
//=======================================
TRtcTime  *TObject::GetRtcTime( string &date, string &time )
{
  CLEAR_WDT;
  Tlist<string> tmpvect;   
  TRtcTime *DateTime = new TRtcTime();
  TBuffer::Split(date,".",tmpvect);
  if(tmpvect.size() == 3)
  {
    DateTime->Day    = atoi(tmpvect[0].c_str());
    DateTime->Month  = atoi(tmpvect[1].c_str());
    DateTime->Year   = atoi(tmpvect[2].c_str());
  }
  else 
  { return NULL;}
  tmpvect.clear(); 
  TBuffer::Split(time,":",tmpvect);
  if(tmpvect.size() == 3)
  {
    DateTime->Hour     = atoi(tmpvect[0].c_str());
    DateTime->Minute   = atoi(tmpvect[1].c_str());
    DateTime->Second   = atoi(tmpvect[2].c_str());  
  }
  else
  { return NULL;}
  return DateTime;
}  
//=============================================================================
//  
//=======================================
void  TObject::SetDataTime(TDateTime &dt, string &date, string &time)
{
  //static bool flg = false;
  CLEAR_WDT;
  Tlist<string> tmpvect;   
  TRtcTime DateTime;
  TBuffer::Split(date,".", tmpvect);
  if(tmpvect.size() != 3)
  {
    return;
  }
  DateTime.Day    = atoi(tmpvect[0].c_str());
  DateTime.Month  = atoi(tmpvect[1].c_str());
  DateTime.Year   = atoi(tmpvect[2].c_str());
  tmpvect.clear(); 
  TBuffer::Split(time,":",tmpvect);
  if(tmpvect.size() != 3){ return;}
  DateTime.Hour     = atoi(tmpvect[0].c_str());
  DateTime.Minute   = atoi(tmpvect[1].c_str());
  DateTime.Second   = atoi(tmpvect[2].c_str());  
  
  long CurrGloalSeconds   = dt.DateTime.Hour*3600L + dt.DateTime.Minute*60L + dt.DateTime.Second*1L;
  long NewGlobalSeconds   = DateTime.Hour*3600L + DateTime.Minute*60L + DateTime.Second*1L;
  
  long div = labs(NewGlobalSeconds - CurrGloalSeconds);
  if( div > 1)//���� ������� ������� - ������ ������ �������� �������
  {
    RtcTimerDivider = 32767;
    Init_SysTick(RtcTimerDivider);//����� ������-��������
    //Init_TIMER_A0(RtcTimerDivider);//����� ������-��������
    dt.SetTime(DateTime);//��������� ������ �������
  }
  else if(div == 1)// ���� ����������� �� �������
  {
    //flg = true;
    if(NewGlobalSeconds > CurrGloalSeconds)//������, ����� �������
    { 
      /*if(flg == false)
      {
        RtcTimerDivider -=1;
        flg = true;
      }*/
      Init_SysTick(2);//���������� �������� ���� ���� ������� ��������� �������
      //Init_TIMER_A0(2);//���������� �������� ���� ���� ������� ��������� �������
    }
    else//������
    { 
      /*if(flg == false)
      {
        RtcTimerDivider +=1;
        flg = true;
      }*/
      Init_SysTick(RtcTimerDivider+16384);// ���� ������� ��������� ��������� 
      //Init_TIMER_A0(RtcTimerDivider+16384);// ���� ������� ��������� ��������� 
    }
  }
  //else//��� ��
  //{ flg = false;}
}
//=============================================================================
// ������ � ������ ��������  
//=======================================
bool TObject::Save_ParamsInInfo(Data_Params  *Params)
{
//���������� ���� �������� � ���������(��������) ���������� � ������ �������� 
 Word         Len;
 Byte        *Tmp                  = (Byte *)Params;  //����� ��������� 
  
  Len = sizeof(*Params); 
  for(Word i=0; i<3; i++)
  {
    WriteEepromBufToPage((Byte const*)&TMP_PARAMS, Tmp, Len-1 );  //�������� �� ���������(��������) ������
    WriteEepromBufToPage((Byte const*)&PARAMS    , Tmp, Len-1 );  //�������� ���� ����
  
    if(VerifyMemory((Byte const*)&PARAMS,Tmp, Len))
    {//������� ��������� ������ 
      ClrEepromFlashSegment( (Byte const*)&TMP_PARAMS );
      return true; 
    }
    else
    { continue;}
  }
  return false;
}
//=============================================================================
//  
//=======================================
bool TObject::VerifyMemory(Byte const* DestAddr, Byte* ScrAddr,Word Len)
{
  for(Word i=0; i<Len; i++)
  {
    CLEAR_WDT;
    if(DestAddr[i]!=ScrAddr[i])
    { return false;}
  }
  return true; 
}
//=============================================================================
//  
//=======================================
bool TObject::RestoreMemParams(void)
{
 Word         Len;
 Data_Params  TmpParam            = *PtrToConstants;    //��������� ������� �������� ���������� � ����������
 Byte        *Tmp                 = (Byte *)&TmpParam;  //����� ��������� 
  
  Len = sizeof(TmpParam);
  
  for(Word i = 0;i<3;i++)
  {
    CLEAR_WDT;
    WriteEepromBufToPage((Byte const*)&PARAMS    , Tmp, Len-1 );  //�������� ���� ����
    if(VerifyMemory((Byte const*)&PARAMS,Tmp, Len))
    {//������� ��������� ������ 
      ClrEepromFlashSegment( (Byte const*)&TMP_PARAMS );
      return true; 
    }
    else
    { 
      continue;
    }
  }
  return false;  
}
//=============================================================================
//  
//=======================================
void TObject::RestoreDefault(void)
{
  Word         Len;
  Data_Params  TmpParam;
  Byte        *Tmp             = (Byte *)&TmpParam;  //����� ��������� 
  Byte        Addr[]           = "11";
  Byte        PassWord[]       = "TM_DP485";
  Byte        Unit_Type[]      = "MKAB-12";
  Word        i;
 
  CLEAR_WDT;
  for(i=0;i<sizeof(Addr);i++)       { TmpParam.DevAddress[i]=Addr[i];} 
  for(i=0;i<sizeof(PassWord);i++)   { TmpParam.PassWord[i]=PassWord[i];}               
  for(i=0;i<sizeof(Unit_Type);i++)  { TmpParam.Unit_Type[i]=Unit_Type[i];}  

  TmpParam.MAIN_BOUD_RATE     = 38400;
  TmpParam.MAIN_timeout       = 500;
  
  TmpParam.wReserved[0]       = 26; // et
  TmpParam.wReserved[1]       = 1;  // en
  TmpParam.wReserved[2]       = 1;  // pn
  
  TmpParam.AdcMultiplier      = 4096;
  TmpParam.ReglamentTime      = 600;
  
  for(Byte i=0;i<MAX_ADC_INPUTS;i++)
  { TmpParam.CalVolt[i]=1.0;}
  
  TmpParam.BOOT_PARAMS.BootStatus = 0x01;
  
  
  Len = sizeof(TmpParam);
  WriteEepromBufToPage((Byte const*)&PARAMS    , Tmp, Len-1 );  //�������� ���� ����
}
//=============================================================================
//  
//=======================================
void TObject::WriteBufToPage(Byte const * PageAddr, Byte *Buf, Word BufLen)
{
  //ClrFlashSegment( PageAddr );
  Buf[BufLen] = 0xAA;
  BufLen++;
  WriteByteToFlash_MY(PageAddr, BufLen, Buf);
}
//=============================================================================
//  
//=======================================
void TObject::WriteEepromBufToPage(Byte const * PageAddr, Byte *Buf, Word BufLen)
{
  //ClrFlashSegment( PageAddr );
  Buf[BufLen] = 0xAA;
  BufLen++;
  WriteByteToEeprom_MY(PageAddr, BufLen, Buf);
}
//=============================================================================
Word  TObject::ReturnFlashCRC(Byte const * PageAddr, Word BufLen)
{
Word CRC_SUM = 0;
   while(BufLen--)
   {
     CLEAR_WDT;      
     CRC_SUM += *PageAddr++;
   }
   return CRC_SUM;
}
//=============================================================================
bool TObject::ClrFlashSegment(Byte const *SegAddr)
{  
  FLASH_Unlock();
  
  FLASH_ClearFlag(FLASH_FLAG_EOP|FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR | FLASH_FLAG_SIZERR | FLASH_FLAG_OPTVERR | FLASH_FLAG_OPTVERRUSR);
  
  CLEAR_WDT;
  FLASH_Status st = FLASH_ErasePage((uint32_t)SegAddr);
  
  FLASH_Lock();
  return st;
}
//=============================================================================
bool TObject::ClrEepromFlashSegment(Byte const *Address)
{  
  DATA_EEPROM_Unlock();
  
  /* Clear all pending flags */      
  FLASH_ClearFlag(FLASH_FLAG_EOP|FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR
                  | FLASH_FLAG_SIZERR | FLASH_FLAG_OPTVERR | FLASH_FLAG_OPTVERRUSR);
  
  FLASH_Status st;
  uint32_t NbrOfPage = 0x200/4; 
  
  CLEAR_WDT;
  /* Erase the Data EEPROM Memory pages by Word (32-bit) */
  for(int j = 0; j < NbrOfPage; j++)
  {
    CLEAR_WDT;
    st = DATA_EEPROM_EraseWord( (uint32_t)(Address + (4 * j)) );
  }
  
  DATA_EEPROM_Lock();
  return st;
}
//=============================================================================
bool TObject::WriteByteToFlash_MY(Byte const * PageAddr, Word size_wr, Byte* ram_buf)
{
//static Byte buffer_for_write[4];
static Byte i;

    volatile Word PageSize = 256;
    /*
    if(PageAddr >= (Byte const*)0x08000000 && PageAddr < (Byte const*)0x08000400){
      PageSize = 256;
    }
    else if(PageAddr >= (Byte const*)0x08000400 && PageAddr < (Byte const*)0x08001000){
      PageSize = 1024;
    }
    else{
       PageSize = 4096;
    }*/
    
    if((*(DWord*)&PageAddr % PageSize) == 0)
      ClrFlashSegment( PageAddr );


  FLASH_Unlock();
  FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR | FLASH_FLAG_SIZERR | FLASH_FLAG_OPTVERR | FLASH_FLAG_OPTVERRUSR);
  //FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);	  // Clear All pending flags
  CLEAR_WDT;
  for(i=0;i<size_wr/4;i++)
  {
      FLASH_FastProgramWord( (uint32_t)PageAddr, *((uint32_t *)&ram_buf[i*4]) ); // ������ 4� ��������� �����
      PageAddr = PageAddr + 4;
  }
  FLASH_Lock();
  return true;
}

//=============================================================================
bool TObject::WriteByteToEeprom_MY(Byte const * PageAddr, Word size_wr, Byte* ram_buf)
{
static Byte i;

  DATA_EEPROM_Unlock();
  
  /* Clear all pending flags */      
  FLASH_ClearFlag(FLASH_FLAG_EOP|FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR
                  | FLASH_FLAG_SIZERR | FLASH_FLAG_OPTVERR | FLASH_FLAG_OPTVERRUSR);
  CLEAR_WDT;
  for(i=0;i<size_wr;i++)
  {
    CLEAR_WDT;
      DATA_EEPROM_FastProgramByte( (uint32_t)PageAddr, *((uint32_t *)&ram_buf[i]) ); // ������ 1 �����
      PageAddr = PageAddr+1;
  }
  DATA_EEPROM_Lock();
  return true;
}
//=============================================================================
//  
//=======================================
#pragma optimize=z none
void  TObject::Soft_Reset(void)//inline
{
      __disable_interrupt();
    NVIC_SystemReset();
    //while(1);//wdt
}
//=============================================================================
//  
//=======================================
void   TObject::DirectJump(Word Addr)
{
}
//*****************************************************************************
//*** Class TDispatcher
//*****************************************************************************
TDispatcher::TDispatcher()
{
  
}
//=============================================================================
//  
//=======================================
TDispatcher::~TDispatcher()
{
    PocessList.clear();
}
//=============================================================================
//  
//=======================================     
void TDispatcher::CreateProcess(PFV Process)
{
    PocessList.push_back(Process);
}  
//=============================================================================
//  
//=======================================
void TDispatcher::Scheduler()
{
  for(Byte i = 0; i < PocessList.size(); i++)
  { 
    CLEAR_WDT;
    PocessList[i](NULL); 
  }
  //__low_power_mode_0(); 
}
//*****************************************************************************
//*** Class TBuffer
//*****************************************************************************
//=============================================================================
//  
//=======================================
const unsigned short Crc16Table[256] = {
 0x0000, 0xC0C1, 0xC181, 0x0140, 0xC301, 0x03C0, 0x0280, 0xC241,      //00h
 0xC601, 0x06C0, 0x0780, 0xC741, 0x0500, 0xC5C1, 0xC481, 0x0440,      //08h
 0xCC01, 0x0CC0, 0x0D80, 0xCD41, 0x0F00, 0xCFC1, 0xCE81, 0x0E40,      //10h
 0x0A00, 0xCAC1, 0xCB81, 0x0B40, 0xC901, 0x09C0, 0x0880, 0xC841,      //18h
 0xD801, 0x18C0, 0x1980, 0xD941, 0x1B00, 0xDBC1, 0xDA81, 0x1A40,      //20h
 0x1E00, 0xDEC1, 0xDF81, 0x1F40, 0xDD01, 0x1DC0, 0x1C80, 0xDC41,      //28h
 0x1400, 0xD4C1, 0xD581, 0x1540, 0xD701, 0x17C0, 0x1680, 0xD641,      //30h
 0xD201, 0x12C0, 0x1380, 0xD341, 0x1100, 0xD1C1, 0xD081, 0x1040,     //38h
 0xF001, 0x30C0, 0x3180, 0xF141, 0x3300, 0xF3C1, 0xF281, 0x3240,
 0x3600, 0xF6C1, 0xF781, 0x3740, 0xF501, 0x35C0, 0x3480, 0xF441,
 0x3C00, 0xFCC1, 0xFD81, 0x3D40, 0xFF01, 0x3FC0, 0x3E80, 0xFE41,
 0xFA01, 0x3AC0, 0x3B80, 0xFB41, 0x3900, 0xF9C1, 0xF881, 0x3840,
 0x2800, 0xE8C1, 0xE981, 0x2940, 0xEB01, 0x2BC0, 0x2A80, 0xEA41,
 0xEE01, 0x2EC0, 0x2F80, 0xEF41, 0x2D00, 0xEDC1, 0xEC81, 0x2C40,
 0xE401, 0x24C0, 0x2580, 0xE541, 0x2700, 0xE7C1, 0xE681, 0x2640,
 0x2200, 0xE2C1, 0xE381, 0x2340, 0xE101, 0x21C0, 0x2080, 0xE041,
 0xA001, 0x60C0, 0x6180, 0xA141, 0x6300, 0xA3C1, 0xA281, 0x6240,
 0x6600, 0xA6C1, 0xA781, 0x6740, 0xA501, 0x65C0, 0x6480, 0xA441,
 0x6C00, 0xACC1, 0xAD81, 0x6D40, 0xAF01, 0x6FC0, 0x6E80, 0xAE41,
 0xAA01, 0x6AC0, 0x6B80, 0xAB41, 0x6900, 0xA9C1, 0xA881, 0x6840,
 0x7800, 0xB8C1, 0xB981, 0x7940, 0xBB01, 0x7BC0, 0x7A80, 0xBA41,
 0xBE01, 0x7EC0, 0x7F80, 0xBF41, 0x7D00, 0xBDC1, 0xBC81, 0x7C40,
 0xB401, 0x74C0, 0x7580, 0xB541, 0x7700, 0xB7C1, 0xB681, 0x7640,
 0x7200, 0xB2C1, 0xB381, 0x7340, 0xB101, 0x71C0, 0x7080, 0xB041,
 0x5000, 0x90C1, 0x9181, 0x5140, 0x9301, 0x53C0, 0x5280, 0x9241,
 0x9601, 0x56C0, 0x5780, 0x9741, 0x5500, 0x95C1, 0x9481, 0x5440,
 0x9C01, 0x5CC0, 0x5D80, 0x9D41, 0x5F00, 0x9FC1, 0x9E81, 0x5E40,
 0x5A00, 0x9AC1, 0x9B81, 0x5B40, 0x9901, 0x59C0, 0x5880, 0x9841,
 0x8801, 0x48C0, 0x4980, 0x8941, 0x4B00, 0x8BC1, 0x8A81, 0x4A40,
 0x4E00, 0x8EC1, 0x8F81, 0x4F40, 0x8D01, 0x4DC0, 0x4C80, 0x8C41,
 0x4400, 0x84C1, 0x8581, 0x4540, 0x8701, 0x47C0, 0x4680, 0x8641,
 0x8201, 0x42C0, 0x4380, 0x8341, 0x4100, 0x81C1, 0x8081, 0x4040
};

Word TBuffer::Crc16(Byte * pcBlock, Word len)
{
 Word crc = 0xFFFF;
 
    //HOLD_WDT;
    while (len--)
    { 
      CLEAR_WDT;
      crc = (crc >> 8) ^ Crc16Table[(crc & 0xFF) ^ *pcBlock++];
    }
    return crc;
}
//=============================================================================
// 
//=======================================
Word TBuffer::Crc(Byte const * pcBlock, Word len)
{
  Word crc = 0;

    while (len--)
    { 
      crc += *pcBlock++;
      CLEAR_WDT;      
    }
    return crc;  
}
//=============================================================================
// 
//=======================================
void TBuffer::XOR_Crypt(Byte* in, Word inlen, string pass)
{
  for ( Word x = 0; x < inlen; x++)
  {
      in[x] = (Byte)( in[x] ^ pass[x%pass.length()] );
      CLEAR_WDT;
  }
}
//=============================================================================
// 
//=======================================
DWord TBuffer::ByteToLong(Byte* Buf)
{
  Blong   LBuf;
    for (Word i = 0; i < 4 ; i++)
      LBuf.Data_b[i] = Buf[i];
    return LBuf.Data_l;
}
//=============================================================================
//
//=======================================
Word TBuffer::ByteToShort(Byte* Buf)
{
 Bshort  SBuf;

 for (Word i = 0; i < 2 ; i++)
	 SBuf.Data_b[i] = Buf[i];
 return SBuf.Data_s;
}
//=============================================================================
//
//=======================================
Word TBuffer::DeByteToShort(Byte* Buf)
{
 Bshort  SBuf;

 for (Word i = 0; i < 2 ; i++)
	 SBuf.Data_b[1-i] = Buf[i];
 return SBuf.Data_s;
}
//=============================================================================
//
//=======================================
void   TBuffer::ClrBUF(Byte *BUF, Word Len)
{
  for (Word i=0;i<Len;i++)
  {
    CLEAR_WDT;
    BUF[i] = 0;
  }
}
//=============================================================================
//
//=======================================
bool   TBuffer::StrCmp(Byte const* Str1, Byte const* Str2)
{
  while(*Str1)
  {
    CLEAR_WDT;
    if(*Str1++ != *Str2++)
    {  return false;}
  }
  if(*Str2 != 0)
  {  return false;}
  return true;
}
//=============================================================================
//
//=======================================
Word TBuffer::HexToWord(Byte *ptr)
{
    //Word number = 0;
  while(*ptr < 0x20) ptr++;
  
  Bshort number;
  number.Data_b[1] =  HexToByte(ptr); 
  ptr+=2;
  number.Data_b[0] =  HexToByte(ptr); 
      
    
  //HexToByte(HEX, (Byte)( WORD >> 8) );
  //HexToByte(&HEX[2], (Byte)( WORD) );
    
  //sscanf((char*)ptr, "%X", &number);
  //return number;
  
  return number.Data_s;
}
//=============================================================================
//
//=======================================
/*
DWord TBuffer::HexToDWord(Byte *Ptr)
{
  DWord number = 0;
    
  sscanf((char*)Ptr, "%lX", &number);
  return number;
}*/
//=============================================================================
//
//=======================================
sWord TBuffer::HexToByte(Byte *ptr)
{
   Byte ch  = *ptr++;
   Byte k   = 0;      
   Byte result[2] = {0,0};  

  while(k < 2)
  { 
    CLEAR_WDT;
    if( (ch >= 'A') && (ch <= 'F') ) 
    {  result[k] = (ch - 'A') + 10; } 
    else if( (ch >= '0') && (ch <= '9') ) 
    {  result[k] = ch - '0';} 
    else
    {  return -1;}
    ch = *ptr++;   
    k++;
  }
  return (result[0] <<= 4) + result[1];
}
//=============================================================================
//
//=======================================
void TBuffer::ByteToHex(Byte *HEX, Byte BYTE)
{
 Byte ch = (BYTE >> 4) & 0x0F;
 Byte k  = 0;
  
   while(k < 2)
   {
     CLEAR_WDT;
     if(ch > 9)
     {  *HEX++ = ch + 'A' - 10;}  
     else 
     {  *HEX++ = ch + '0';}
  
     ch = BYTE & 0x0F;
     k++;
   }
   HEX = NULL;
}
//=============================================================================
//
//=======================================
void TBuffer::WordToHex(Byte *HEX, Word WORD)
{
  ByteToHex(HEX, (Byte)( WORD >> 8) );
  ByteToHex(&HEX[2], (Byte)( WORD) );
}
//=============================================================================
//
//=======================================
void TBuffer::DWordToHex(Byte *HEX, DWord DWORD)
{
  WordToHex(HEX, (Word)( DWORD >> 16) );
  WordToHex(&HEX[4], (Word)( DWORD) );
}
//=============================================================================
//
//=======================================
Word TBuffer::CopyString(Byte* DestStr, Byte* SrcStr)
{
  Word len = 0;
  
  while(*SrcStr)
  {
    *DestStr++ = *SrcStr++;
    len++;
    CLEAR_WDT;    
  }

  *DestStr = 0;
  return len;
}       
//=============================================================================
//
//=======================================
/*Byte TBuffer::FloatToString(Byte *ReturnBUF, float Val, Byte AllSymb, Byte AfteComma)
{
  Word begin_val = (Word)Val;
  Word after_val = (Word)((Val - begin_val) * pow(10.0,AfteComma)) ;
  
  Word val;
  
  Val = (Val - begin_val);
  
  for (Byte i= 1;AllSymb+1>i;i++)
    {
      Word mult = (Word)pow(10.0, (AllSymb-i));
      val = begin_val/mult + 0x30;
      begin_val = begin_val%mult;
      ReturnBUF[i-1] = (Byte)val;
    }
  
  ReturnBUF[AllSymb] = 0x2E;
  
  for (Byte j= 1;AfteComma+1>j;j++)
    {
    Word mult = (Word)pow(10.0,(AfteComma -j));
    val = after_val/mult + 0x30;
    after_val = after_val%mult;
    ReturnBUF[AllSymb + j] = (Byte)val;
    }

//  sprintf((char*)ReturnBUF,"%0*.*f",AllSymb,AfteComma,Val);
  return AllSymb;
}*/
//=============================================================================
//
//=======================================
//#pragma optimize=z none
string TBuffer::FloatToString(float Val, Byte AllSymb, Byte AfteComma)
{
  Word begin_val = (Word)Val;
  Word after_val = (Word)( (Val - begin_val) * pow(10.0, AfteComma) );
 
  return IntToString(begin_val) + "." + IntToString(after_val, AfteComma);
}
//=============================================================================
//
//=======================================
string TBuffer::IntToString(int Val, Byte AllSymb)
{
  if(Val == 0 && AllSymb == 0)
    return "0";
  char buff[16] ={0};
  if(AllSymb == 0)
  { 
    while(Val >=  (Word)pow(10.0, AllSymb))
      AllSymb++;
  }
  
  for (Byte i= 1;AllSymb+1>i;i++)
  {
      Word mult = (Word)pow(10.0, (AllSymb-i));
      Word val = Val/mult + 0x30;
      Val = Val%mult;
      buff[i-1] = (Byte)val;
  }
  return string(buff);
}
//=============================================================================
//
//=======================================
//#pragma optimize=z none
string TBuffer::LongToString(long Val, Byte AllSymb)
{
  if(Val == 0 && AllSymb == 0)
    return "0";
  char buff[32] ={0};
  if(AllSymb == 0)
  { 
    while(Val >=  pow(10.0, AllSymb))
      AllSymb++;
  }
  
  for (Byte i= 1;AllSymb+1>i;i++)
  {
      long mult = (long)pow(10.0, (AllSymb-i));
      long val = Val/mult + 0x30;
      Val = Val%mult;
      buff[i-1] = (Byte)val;
  }
  return string(buff);
}
/*
//=============================================================================
//
//=======================================
Word TBuffer::NewStringToInt(string Val)
{
string some_string;
istringstream buffer(Val);
Word some_int;
buffer >> some_int;
return some_int;
}

//=============================================================================
//
//=======================================
string TBuffer::NewIntToString(int Val)
{
    ostringstream buffer;
    buffer << Val;
    return buffer.str();
}*/
//=============================================================================
//
//=======================================
string TBuffer::WordToString(Word Val, Byte AllSymb)
{
  char buff[6] = {0x30,0x30,0x30,0x30,0x30,0};
  sByte index = 4;
  Word counter = 1;
  Word tmpcounter = 1;
  Word tmp1;
  Word tmp2;
  
  do
  {
    counter*=10;
    tmp1 = Val/counter;
    tmp2 = Val%counter;
    tmp2 = tmp2/tmpcounter;
    tmpcounter = counter;
    buff[index--] = tmp2+0x30;
  }
  while(tmp1 > 0 && index > 0);
  if(tmp1 > 0)
  { buff[index--] = tmp1+0x30;}
  if(AllSymb > 0)
  {
    sByte i = index+1;
    if(i > 0)
    {
      if(AllSymb > 5) 
      { AllSymb = 5;}
      Byte siz = 5-i;
      i=AllSymb-siz;
      if(i>0)
      {
        index-=i;
      }
    }
  }
  return string(&buff[index+1]);
}
//=============================================================================
//
//=======================================
string TBuffer::DateTimeToString(TRtcTime *datetime)
{
  return  string(
          TBuffer::WordToString(datetime->Day)+'.'+
          TBuffer::WordToString(datetime->Month)+'.'+ 
          TBuffer::WordToString(datetime->Year)+','+
          TBuffer::WordToString(datetime->Hour)+':'+ 
          TBuffer::WordToString(datetime->Minute)+':'+  
          TBuffer::WordToString(datetime->Second) 
                );
  
}
//=============================================================================
//
//=======================================
TRtcTime *TBuffer::StringToDateTime(string strdt)
{
 Tlist<string> results;
  
  TBuffer::Split(strdt, "," , results);
  if(results.size() != 2){ return NULL;}
  string date = results[0];
  string time = results[1];
  
  results.clear();
  TBuffer::Split(date, "." , results);
  if(results.size() != 3){ return NULL;}
  
  TRtcTime dt;
  dt.Day    = atoi( results[0].c_str() );
  dt.Month  = atoi( results[1].c_str() );
  dt.Year   = atoi( results[2].c_str() );
  
  results.clear();
  TBuffer::Split(time, ":" , results);
  if(results.size() != 3){ return NULL;}  
  dt.Hour    = atoi( results[0].c_str() );
  dt.Minute  = atoi( results[1].c_str() );
  dt.Second  = atoi( results[2].c_str() );
  
  TRtcTime *rdt = new TRtcTime();
  *rdt = dt;
  return rdt;
}
//=============================================================================
//
//=======================================
void  TBuffer::Split(string &str, string separator, Tlist<string> &results, int Len)
{
    int found = (int)str.find(separator);
    while(found > -1)
    {
        if(found > 0)
        { 
          results.push_back(str.substr(0, found));
          if(Len > 0 && results.size()>= Len)
            return;
        }
        str = str.substr(found+separator.size(), -1);//-1==npos!!!
        found = (int)str.find(separator);
    }
    /*
    while(found != std::string::npos)
    {
      CLEAR_WDT;
        if(found > 0)
        { 
          results.push_back(str.substr(0,found));
          if(Len > 0 && results.size()>= Len)
          { return;}
        }
        str = str.substr(found+separator.size());
        found = str.find(separator);
    }*/
    
    if(str.length() > 0)
      results.push_back(str);
}
//=============================================================================
//
//=======================================
void  TBuffer::Split(Byte *BUFFER, Word Size, string separator, Tlist<string> &results, Word Len)
{
    int found;
    found = TBuffer::find(BUFFER, Size, separator);
    while( found != -1 && Size > 0)
    {
      CLEAR_WDT;
        if(found > 0)
        { 
          string tmp = string((char*)BUFFER, found );
          results.push_back( tmp );
          if(Len > 0 && results.size()>= Len)
          { return;}
        }
        BUFFER = (Byte*)( (DWord)BUFFER+found+separator.size() );
        Size = Size - (found+separator.size());
        found = TBuffer::find(BUFFER, Size, separator);
    }
    if(Size > 0)
    { results.push_back( string((char*)BUFFER, Size ) );}
}
//=============================================================================
//
//=======================================
int  TBuffer::find_first_of(TBuffer *BUFFER, Byte simb)
{
  int returnVal = -1;
  for(int i=0; i < BUFFER->DataLen; i++)
  {
    CLEAR_WDT;
    if(BUFFER->BUF[i] == simb)
    {
      returnVal = i;
      break;
    }
  }
return returnVal;
}
//=============================================================================
//
//=======================================
int  TBuffer::find_first_of(Byte *BUFFER, Word Len, Byte simb)
{
  int returnVal = -1;
  for(int i=0; i < Len; i++)
  {
    CLEAR_WDT;
    if(BUFFER[i] == simb)
    {
      returnVal = i;
      break;
    }
  }
return returnVal;
}
//=============================================================================
//
//=======================================
int  TBuffer::find(TBuffer *BUFFER, string str)
{
  int returnVal = -1;
  Byte simb = str.c_str()[0];
  for(int i=0; i < BUFFER->DataLen; i++)
  {
    CLEAR_WDT;
    if(BUFFER->BUF[i] == simb)
    {
      string tmp = string( (char*)&BUFFER->BUF[i], str.size() );
      if(tmp == str)
      { 
        returnVal = i;
        break;
      }
    }
  }
return returnVal;
}
//=============================================================================
//
//=======================================
int  TBuffer::find(Byte *BUFFER, Word Len, string str)
{
  int returnVal = -1;
  Byte simb = str.c_str()[0];
  for(int i=0; i < Len; i++)
  {
    CLEAR_WDT;
    if(BUFFER[i] == simb)
    {
      string tmp = string( (char*)&BUFFER[i], str.size() );
      if(tmp == str)
      { 
        returnVal = i;
        break;
      }
    }
  }
return returnVal;
}
//=============================================================================
//
//=======================================
int  TBuffer::find_last_of(TBuffer *BUFFER, Byte simb)
{
  int returnVal = -1;
  for(int i=(BUFFER->DataLen-1); i > 0; i--)
  {
    CLEAR_WDT;
    if(BUFFER->BUF[i] == simb)
    {
      returnVal = i;
      break;
    }
  }
return returnVal;
}
//=============================================================================
//
//=======================================
Word TBuffer::clear_all_nulls(char *_ptr, Word _size)
{
        //HOLD_WDT;
        char *ptrDst;
        char *ptrSrc;
        for(int i = 0; i <= _size; i++)
        {
          CLEAR_WDT;
          ptrDst = &_ptr[i];
          if( *ptrDst < ' ')
          {
            ptrSrc = &_ptr[i+1]; 
            int rsz = (_size-i);
            for(int j = 0; j < rsz; j++)
            {
              CLEAR_WDT;
              ptrDst[j] = ptrSrc[j];
            }
            if(rsz > 0)
            {
              _size--;
              i--;
            }
          }
        }
        _ptr[_size] = 0;
        return _size;
}
//*****************************************************************************
//*** ObjectFunctionsTimer
//*****************************************************************************
TFastTimer TObjectFunctions::ObjectFunctionsTimer(2,&SecCount);  
//*****************************************************************************
//*** Class TObjectFunctions
//*****************************************************************************
void TObjectFunctions::ReleON(int rele)
{
         Word index = (Word)rele;
         if(index > 0) {index--;} 
         //ObjectFunctionsTimer.SetTimer(index, RELE_OFF_TIM_VAL);
         index  = 1 - index;
         
          //P9DIR |= 1 << index;
          //P9OUT |= 1 << index;
}
//=============================================================================
//  
//=======================================
void TObjectFunctions::ReleOFF(int rele)
{
         Word index = (Word)rele;
          if(index > 0) {index--;} 
          index  = 1 - index;
          
          //P9DIR |= 1 << index;
          //P9OUT &= ~(1 << index);
}
//*****************************************************************************
//***TDateTime
//*****************************************************************************
//=============================================================================
// 
//=======================================
TDateTime::TDateTime() 
{
    TRtcTime time;
      time.Year  = 2010; 	
      time.Month = 1;
      time.Day   = 1;
      time.Hour  = 0;
      time.Minute= 0;
      time.Second= 1;
      GlobalSeconds = 1;
    SetTime(time);
    TimeSynchIsActual=false;
}  
//=============================================================================
// 
//=======================================
TDateTime::TDateTime(TRtcTime time)
{ 
    SetTime(time);
}
//=============================================================================
// 
//=======================================
void TDateTime::SetSynchAct(void)
{
      TimeSynchIsActual=true;
}
//=============================================================================
// 
//=======================================
bool TDateTime::GetSynchAct(void)
{
      return TimeSynchIsActual;
}
//=============================================================================
// 
//=======================================
TDateTime::~TDateTime()
{}
//=============================================================================
// 
//=======================================  
TRtcTime TDateTime::GetTime(void)
{
      return DateTime;
}
//=============================================================================
// 
//=======================================    
void TDateTime::SetTime(TRtcTime dateTime)
{
      DateTime      = dateTime;
      GlobalSeconds = DateTime.Hour*3600L+DateTime.Minute*60L+DateTime.Second;
}
//=============================================================================
// 
//=======================================    
void TDateTime::IncSecond(void)
{
    if(++DateTime.Second >= 60)
    {
      DateTime.Second = 0;
      IncMinute();
    }
    if(GlobalSeconds++ > 86400L)//���������� ����-��������
    {
      GlobalSeconds = DateTime.Hour*3600L+DateTime.Minute*60L+DateTime.Second;
    }
}
//=============================================================================
// 
//=======================================    
DWord TDateTime::GetGlobalSeconds(void)
{ 
  return GlobalSeconds;
}

//=============================================================================
// 
//=======================================    
long TDateTime::GetFullGlobalSecond(void)
{ 
  
  return long(GlobalSeconds+(long)((2010-DateTime.Year)*8760+(DateTime.Month*730)+(DateTime.Day*24))*3600);
}

//=============================================================================
// 
//=======================================     
void TDateTime::IncMinute(void)
{
    if(++DateTime.Minute >= 60)
    {
      DateTime.Minute = 0;
      IncHour();
    }
}
//=============================================================================
// 
//=======================================   
void TDateTime::IncHour(void)
{
    if(++DateTime.Hour >= 24)
    {
      DateTime.Hour = 0;
      IncDay();
    }
}
//=============================================================================
// 
//=======================================    
void TDateTime::IncDay(void)
{
   Byte DaysInMonth = 31;
    if(DateTime.Month == 4 || DateTime.Month == 6 || DateTime.Month == 9 || DateTime.Month == 11 )
    { DaysInMonth = 30;}
    else if( DateTime.Month == 2)
    {
      if(IsLeapYear( DateTime.Year ) == true)
      { DaysInMonth = 29;}
      else
      { DaysInMonth = 28;}
    }    
    if(++DateTime.Day > DaysInMonth)
    {
      DateTime.Day = 1;
      IncMonth();
    }
}
//=============================================================================
// 
//=======================================  
void TDateTime::IncMonth(void)
{
    if(++DateTime.Month > 12)
    {
      DateTime.Month = 1;
      DateTime.Year++;
    }
}  
//=============================================================================
// 
//=======================================  
bool TDateTime::IsLeapYear(Word Year) 
{
    // �������� ����������� ����
    if( (Year%4 == 0) && ((Year % 100 != 0) || (Year % 400 == 0)) )
    {  return true;} // ����������
    else
    {  return false;} //�� ����������
}
//*****************************************************************************
//*****************************************************************************
//*** Class TComPort
//*****************************************************************************
TComPort::TComPort():enabled(false),TTimer(4),commPortHandle(INVALID_HANDLE_VALUE),ByteIndex(0),time_outs(),CRC_SUM(0),StxFlag(false),DataReceiving(false)
{
  DATA_Rx = NULL;
}
//=============================================================================
// Destructor
//=======================================
TComPort::~TComPort()
{
	if (commPortHandle != INVALID_HANDLE_VALUE)
	{	CloseHandle(commPortHandle);}
}
//=============================================================================
//
//=======================================
void TComPort::PushByteToDATA_Rx(Byte Data)
{
  if(DATA_Rx!=NULL)
  {
    DATA_Rx->push(Data);
  }
}
//=============================================================================
//
//=======================================
sWord TComPort::PopByteDATA_Rx(void)
{
  if(DATA_Rx!=NULL)
  { return DATA_Rx->pop();}
  else
  { return -1;}
}
//=============================================================================
// Closes the com port 
//=======================================
bool TComPort::close()
{
	if (commPortHandle != INVALID_HANDLE_VALUE)
	{	CloseHandle(commPortHandle);}
	commPortHandle = INVALID_HANDLE_VALUE;
	return true;
}
//=============================================================================
// Closes the com port Handle
//=======================================
bool TComPort::CloseHandle(HANDLE commPort)
{
  bool flag     = false;
  //Word old_SR   = __get_SR_register();
  //if(old_SR & GIE)
  {flag = true;}
  __disable_interrupt(); 
      switch (commPort)
      {
          case COM0:
              //close_UART0();
              break;
          
          case COM1:
         //     close_UART1();
              break;
          
          default:
              return false;
      } 
  enabled = false;   
  if(flag)
  {__enable_interrupt();}
  return true;
}
//=============================================================================
//=======================================
void TComPort::ClrByteIndex(void)
{
  ByteIndex = 0;
}
//=============================================================================
//=======================================
void TComPort::SetTimeOuts(COMMTIMEOUTS timeouts)
{
  time_outs = timeouts;
}
//=============================================================================
//=======================================        
void TComPort::GetTimeOuts(COMMTIMEOUTS &timeouts)
{
  timeouts = time_outs; 
}
//=============================================================================
// open com port 
//=======================================
HANDLE TComPort::open(Port port, DWord baudRate, DWord ReadIntervalTimeout)
{
  USART_InitTypeDef USART_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;
  COMMTIMEOUTS timeouts;
      switch (port)
      {
        case COM0: 
            /* Enable GPIO clock */
          RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
          
            /* Enable USART clock */
          RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
          
            /* Connect PXx to USARTx_Tx */
          GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);
  
            /* Connect PXx to USARTx_Rx */
          GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);
          
          /* Configure USART Tx and Rx as alternate function push-pull */
          GPIO_InitStructure.GPIO_Mode          = GPIO_Mode_AF;
          GPIO_InitStructure.GPIO_Speed         = GPIO_Speed_40MHz;
          GPIO_InitStructure.GPIO_OType         = GPIO_OType_PP;
          GPIO_InitStructure.GPIO_PuPd          = GPIO_PuPd_UP;
          GPIO_InitStructure.GPIO_Pin           = GPIO_Pin_9;
          GPIO_Init(GPIOA,        &GPIO_InitStructure);
          
          GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
          GPIO_Init(GPIOA, &GPIO_InitStructure);
          
          USART_InitStructure.USART_BaudRate                   = baudRate;
          USART_InitStructure.USART_WordLength                  = USART_WordLength_8b;
          USART_InitStructure.USART_StopBits                    = USART_StopBits_1;
          USART_InitStructure.USART_Parity                      = USART_Parity_No;
          USART_InitStructure.USART_HardwareFlowControl         = USART_HardwareFlowControl_None;
          USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
          USART_Init(USART1, &USART_InitStructure);
          
          RX_BUF = (Byte*)&USART1->DR;
          TX_BUF = (Byte*)&USART1->DR;
          
          USART1->CR1 |= USART_CR1_RXNEIE;
          NVIC_EnableIRQ(USART1_IRQn);   
          USART_Cmd(USART1, ENABLE);
        break;  

          default:
            __enable_interrupt();
              return INVALID_HANDLE_VALUE;
      }; 

      InitPort(baudRate);   
      enabled                         = true;    
      commPortHandle                  = (HANDLE)port;
      timeouts.ReadIntervalTimeout    = ReadIntervalTimeout;
      timeouts.ReadTotalTimeout       = timeouts.ReadIntervalTimeout*timeouts.ReadTotalTimeoutMultiplier;
      SetTimeOuts(timeouts); 

  return commPortHandle; 
}
//=============================================================================
// ������ ����
//=======================================
bool TComPort::RecvByte(Byte *data)
{
  if ( this->DATA_Rx == NULL || this->DATA_Rx->empty() )
  {  return false;}
  else
  {
    *data = this->DATA_Rx->pop();
    this->SetTimer(RX_BYTE_TIMEOUT, time_outs.ReadIntervalTimeout);//���� ������������ ��������
    return true;
  }   
}
//=============================================================================
// ����� ����
//=======================================
bool TComPort::SendByte(Byte data)
{    
       
       if ( GetTxIntFlag() )
       {
          this->SetTxIntEnable(true);
          //TTimer::wait_1ms(10);
          //this->DATA_Tx = data;
          USART1->DR = data;
          
          
          return true;  
       }     
       else 
       {  
         return false; 
       }   
}
//=============================================================================
//
//=======================================
sWord   TComPort::Send(Byte *Buf, Word Len)
{
  if (Len == 0)
  {  return 1;} 
  
   TX_Ready();
   if ( SendByte(Buf[ByteIndex]) )
   {  
     while( UsartIsBusy() ){  CLEAR_WDT;}       
     if ( ++ByteIndex >= Len )
     {     
       DataReceiving = false;
       ByteIndex = 0;
       RX_Ready();
       return Len;
     }    
   }   
  return 0;
}
//=============================================================================
//
//=======================================
sWord   TComPort::Send(Byte *Buf, Word Len, bool separate)
{
  if (Len == 0)
  {  return 1;} 
  
  if(separate == true)
  { ClearRxBUF();}
  
  TX_Ready();
  do
  {
    CLEAR_WDT;
     if( !SendByte(Buf[ByteIndex]) )
     {  break; }    
     while( UsartIsBusy() ){  CLEAR_WDT;}
     if ( ++ByteIndex >= Len )
     {     
       DataReceiving  = false;
       sWord tmp      = ByteIndex;
       ByteIndex      = 0;
       return tmp;
     }
  }
  while(separate);
  return 0;
}
//=============================================================================
// ������ (����� �� ��������)
//=======================================
sWord TComPort::Recv(Byte *Buf, Word Len, Word MaxLen)
{
  Byte data;  
      if(Len == 0){ return 0;}
      if(Len > MaxLen) { Len = MaxLen;}
      if(DataReceiving == false)
      {
         DataReceiving = true;
         SetTimer(RX_BYTE_TIMEOUT,time_outs.ReadIntervalTimeout);
         ByteIndex = 0;
      }
      RX_Ready();
      while( RecvByte(&data) )
      {   
        CLEAR_WDT;
        Buf[ByteIndex++] = data;
        if( ByteIndex >= Len )
        {
             Word TmpWord = ByteIndex;
             ByteIndex       = 0x00;
             DataReceiving = false;
             return TmpWord;
        }          
      };
      if(DataReceiving == true)
      { 
         if ( GetTimValue(RX_BYTE_TIMEOUT) <= 0) //�������� �� �������
         {    
             Word TmpWord = ByteIndex;
             ByteIndex       = 0x00;
             DataReceiving = false;
             return TmpWord;
         }  
      }
      return -1;    
}
//=============================================================================
// ������ (����� �� ��������)
//=======================================
sWord TComPort::Recv(Byte *Buf, Word Len, Word MaxLen, Word timeout, bool separate)
{
  Byte data;  
      if(Len == 0){ return 0;}
      if(Len > MaxLen) { Len = MaxLen;}
      if(DataReceiving == false)
      {
         DataReceiving = true;
         SetTimer(RX_BYTE_TIMEOUT, timeout);
         ByteIndex = 0;
      }
      
      do
      {
        RX_Ready();
        while( RecvByte(&data) )
        {   
          CLEAR_WDT;
          Buf[ByteIndex++] = data;
          if( ByteIndex >= Len )
          {
               Word TmpWord = ByteIndex;
               ByteIndex       = 0x00;
               DataReceiving = false;
               return TmpWord;
          }          
        };
        if(DataReceiving == true)
        { 
           if ( GetTimValue(RX_BYTE_TIMEOUT) <= 0) //�������� �� �������
           {    
               Word TmpWord = ByteIndex;
               ByteIndex       = 0x00;
               DataReceiving = false;
               return TmpWord;
           }  
        }
      }
      while(separate);
      return -1;    
}
//=============================================================================
// ������
//=======================================
sWord TComPort::RecvTo(Byte *Buf, Word MaxLen, Byte StopSymbol)
{
      Byte data;
      if(DataReceiving == false)
      {
         DataReceiving = true;
         SetTimer(RX_BYTE_TIMEOUT,time_outs.ReadIntervalTimeout);
         ByteIndex = 0;
      }
      RX_Ready();
      while( RecvByte(&data) )
      {
        CLEAR_WDT;
        Buf[ByteIndex++] = data;
        if(ByteIndex >=MaxLen)
        { ByteIndex = MaxLen-1;}
        if(data == StopSymbol)
        {
             Word TmpWord = ByteIndex;
             ByteIndex       = 0x00;
             DataReceiving = false;
             return TmpWord;
        }          
      };
      if(DataReceiving == true)
      { 
           if ( GetTimValue(RX_BYTE_TIMEOUT) <= 0) //�������� �� �������
           {    
             Word TmpWord = ByteIndex;
             ByteIndex       = 0x00;
             DataReceiving = false;
             return TmpWord;
           }  
      }
      return -1;    
}
//=======================================
void TComPort::ClearRxBUF()
{
  if(DATA_Rx!=NULL)
  {DATA_Rx->clear();}
}
//=======================================
bool TComPort::Busy()
{
  return (bool)ByteIndex;
}
//=============================================================================
// 
//=======================================
void TComPort::InitPort(DWord BAUDRATE)
{
  SetRxIntEnable(true);   
}
//=============================================================================
// 
//=======================================
bool TComPort::GetTxIntFlag(void)
{
  Word status = USART1->SR;
  return (bool)(status & USART_SR_TC);
}
//=============================================================================
// 
//=======================================
void TComPort::SetTxIntFlag(bool state)
{
//   if(state == true)
//   {  *IFG_REGISTER  |=  UCTXIFG;}
//   else
//   {  *IFG_REGISTER  &= ~UCTXIFG;}
}
//=============================================================================
// 
//=======================================
Short TComPort::GetIntVector(void)
{
   return *IVECTOR_REGISTER; 
}
//=============================================================================
// 
//=======================================
bool TComPort::UsartIsBusy(void)
{
  Word status;
  Word out;
  status = USART1->SR;
  out = (~status & USART_SR_TC);
  //out = ~out;
  return (bool) out;
}
//=============================================================================
// 
//=======================================
void TComPort::SetTxIntEnable(bool state)
{
   if(state == true)
   {  
     USART1->CR1  |=  USART_CR1_TXEIE;
     //USART1->CR1  &= ~USART_CR1_RXNEIE;
   }
   else
   {  
     USART1->CR1  &= ~USART_CR1_TXEIE;
     //USART1->CR1  |=  USART_CR1_RXNEIE;
   }
}
//=============================================================================
// 
//=======================================
void TComPort::SetRxIntEnable(bool state)
{
   if(state == true)
   {  USART1->CR1  |=  USART_CR1_RXNEIE;}
   else
   {  USART1->CR1  &= ~USART_CR1_RXNEIE;}
}
//=============================================================================
// 
//=======================================
Byte TComPort::GetRxDataByte()
{
  return *RX_BUF;
}
//=============================================================================
// 
//=======================================       
void TComPort::SetTxDataByte(Byte data)
{
  *TX_BUF = data;
}
//=============================================================================
//
//=======================================
bool    TComPort::TestStxFlag(void)
{
   return StxFlag;
}
//=============================================================================
// ���������� � ������ ������
//=======================================
void    TComPort::ClrStxFlag(void)
{
   StxFlag   = false;
   ByteIndex = 0;
   RX_Disable();
}
//*****************************************************************************
//*** TTimer
//*****************************************************************************
//=============================================================================
// Constructor
//=======================================
TTimer::TTimer(Byte Timers):TimerCNT(Timers)
{
  //HOLD_WDT;
    Timer = new DWord[TimerCNT];
    for(Word i = 0; i < TimerCNT; i++)
    { 
      CLEAR_WDT;
      SetTimer(i,0);
    }
} 
//=============================================================================
// Destructor
//======================================= 
TTimer::~TTimer()
{
    delete[] Timer;
}
//=============================================================================
// 
//=======================================  
bool TTimer::SetTimer(Byte TimNam, DWord Value)
{
  bool flag     = false;
//  Word old_SR   = __get_SR_register();
//  if(old_SR & GIE)
//  {flag = true;}
  __disable_interrupt(); 
bool state;
  if (TimNam<TimerCNT)
  {
    Timer[TimNam] = Value;
    state = true;
  }
  else
  {  state = false;}
  if(flag)
  {__enable_interrupt(); }
return state;
}
//=============================================================================
// 
//=======================================  
void TTimer::DecTimer(Byte TimNam)
{
  if (TimNam >= TimerCNT)//debug
  {  return;}
  if (Timer[TimNam] > 0)
  {  Timer[TimNam]--;}
}
//=============================================================================
// 
//=======================================
sDWord TTimer::GetTimValue(Byte TimNam)
{
  bool flag     = false;
//  Word old_SR   = __get_SR_register();
//  if(old_SR & GIE)
  {flag = true;}
  __disable_interrupt(); 
sDWord Val;
    if (TimNam>=TimerCNT)
    {  Val = -1;}
    else
    {  Val = Timer[TimNam];}
    if(flag)
    {__enable_interrupt();} 
return Val;
}
//=============================================================================
// 
//=======================================
void TTimer::wait_1ms(Word cnt)
{
    Word i;

        //HOLD_WDT;
	for (i = 0; i < cnt; i++)
        { 
          CLEAR_WDT;
//          __delay_cycles(14745);
        }
        CLEAR_WDT;
}
//=============================================================================
// 
//=======================================
void TTimer::wait_1us(Word cnt)
{
        // HOLD_WDT;
	for (Word i = 0; i < cnt; i++)
        { 
          CLEAR_WDT;
          //__delay_cycles(8);
          //__delay_cycles(8);//���� 8 �� ����� ���������� CLEAR_WDT; 500 �� ������������ � 610...
          //__delay_cycles(9);
        }
        CLEAR_WDT;
}


//*****************************************************************************
//***TCmd
//*****************************************************************************
//=============================================================================
Word TCmd::GetAddress()          
{
  return  atoi( (char*)TObject::PtrToConstants->DevAddress );
}
//*****************************************************************************
//***TDatchik
//*****************************************************************************
TDatchik::TDatchik(TPtype* ptype, TInput* pDryInput, Byte pPrPlace)
{
    this->Ptype     = ptype;
    this->DryInput  = pDryInput;    
    this->PrPlace   = pPrPlace;      
    this->Inversia  = 0;
    this->Value     = 0;     
    this->HValue    = 0;      
    this->StopWathSendOFF = 0;
}
//=============================================================================
TDatchik::TDatchik(TPtype* ptype, TInput* pDryInput, Byte pPrPlace, bool inv, bool StopWath)
{
    this->Ptype     = ptype;
    this->DryInput  = pDryInput;    
    this->PrPlace   = pPrPlace;      
    this->Inversia  = inv;
    this->Value     = 0;     
    this->HValue    = 0;      
    this->StopWathSendOFF = StopWath;
}

//=============================================================================
TDatchik::~TDatchik()
{
  if(DryInput !=NULL)
  { delete DryInput;}
}
//=============================================================================
string TDatchik::GetStringValue(void)
{ 
  return TBuffer::WordToString(GetValue());
}
//=============================================================================
string TDatchik::GetStringCfgValue(void)
{ 
     string ReturnString = "";
     return ReturnString;
}
//=============================================================================
void   TDatchik::SetManualState(bool Ena,bool Inv)
{
    StopWathSendOFF=Ena;
    Inversia=Inv;
}
//=============================================================================
Byte TDatchik::GetValue(void)
{
     this->Value = 0;
     if(this->DryInput !=NULL)
     {  
       if(!this->StopWathSendOFF)
         this->Value = this->DryInput->GetValue();
       else
         this->Value = 1; 
     }
     return  this->Value^this->Inversia; 
}
//=============================================================================   
   void  TDatchik::SetOffInput(void)
   {
     if(this->DryInput !=NULL)
     {  
       this->StopWathSendOFF=true;
     }
   }
//=============================================================================   
   void  TDatchik::SetONInput(void)
   {
     if(this->DryInput !=NULL)
     {  
       this->StopWathSendOFF=false;
     }     
   }  
//*****************************************************************************
//***TAutomatDevice
//*****************************************************************************
TAutomatDevice::TAutomatDevice(TPtype* ptype, Byte pn, TMkkoInput **glInp, Byte ia, Byte ib, Byte ic, Byte oa, Byte ob, Byte oc):Value(0), HValue(0),_parName(pn)
{//type=ia,in=ib,outstate=ic,parentname=pn
  Byte maxSize = 43;  
  _type = 0;
  _outState = 0;
  if(ptype->PType == 5 || ptype->PType == 6)
  {

    if(ia == 0){ _inA = NULL;}
    else
    { 
      if(ia >= maxSize) ia = maxSize;
      _inA = glInp[--ia];      
    }
    ////////
    if(ib == 0){ _inB = NULL;}
    else
    { 
      if(ib >= maxSize) ib = maxSize;
      _inB = glInp[--ib];      
    }
    ////////
    if(ic == 0){ _inC = NULL;}
    else
    { 
      if(ic >= maxSize) ic = maxSize;
      _inC = glInp[--ic];      
    }
    ///////
    if(oa == 0){ _outA = NULL;}
    else
    { 
      if(oa >= maxSize) oa = maxSize;
      _outA = glInp[--oa];      
    }
    ////////
    if(ob == 0){ _outB = NULL;}
    else
    { 
      if(ob >= maxSize) ob = maxSize;
      _outB = glInp[--ob];      
    }
    ////////
    if(oc == 0){ _outC = NULL;}
    else
    { 
      if(oc >= maxSize) oc = maxSize;
      _outC = glInp[--oc];      
    }
    if(_inA != NULL) _type++;
    if(_inB != NULL) _type++;
    if(_inC != NULL) _type++;
  }
  else if(ptype->PType == 7)
  {
    _type = ia;
    if(ib >= maxSize) ib = maxSize;
    _inB = glInp[--ib]; 
    _outState = ic;
  }
  this->Ptype     = ptype;
}
//=============================================================================  
TAutomatDevice::~TAutomatDevice()
{
  
}

//=============================================================================  
Byte TAutomatDevice::GetValue(void)
{
  Byte val = 0;
  Byte aval = 0;
  this->Value = 0;
  if( Ptype->PType == 5 || Ptype->PType == 6)
  {
     Byte cnt = 0;
      if(_inA != NULL && _outA != NULL) if(_inA->GetValue() == 1 && _outA->GetValue() == 1) cnt++;
      if(_inB != NULL && _outB != NULL) if(_inB->GetValue() == 1 && _outB->GetValue() == 1) cnt++;
      if(_inC != NULL && _outC != NULL) if(_inC->GetValue() == 1 && _outC->GetValue() == 1) cnt++;
      switch(_type)
      {
        case 0:
          break;
          
        case 1:
          if( cnt != 1) aval = 1;
          break;
          
        case 2:
          if( cnt != 2) aval = 1;
          break;
          
        case 3:
          if( cnt < 2) aval = 1;
          break;
          
        default:
          break;
      }
      val |=                                aval << 3;
      if(_outA != NULL) val |= _outA->GetValue() << 2; 
      if(_outB != NULL) val |= _outB->GetValue() << 1; 
      if(_outC != NULL) val |= _outC->GetValue(); 
  }
  else if(Ptype->PType == 7)
  {
    if(_inB != NULL) val = _inB->GetValue(); 
  }
  this->Value = val;
  return val;
}
//=============================================================================  
string TAutomatDevice::GetStringValue(void)
{
  string ret = "";

    if( Ptype->PType == 5 || Ptype->PType == 6)
    {
       if(Ptype->PType == 5)
       {
         ret += TBuffer::WordToString( (this->Value >> 3) & 0x01)+',';
       }
       ret += TBuffer::WordToString((this->Value >> 2) & 0x01 )+','+
              TBuffer::WordToString((this->Value >> 1) & 0x01 )+','+
              TBuffer::WordToString( this->Value  & 0x01 )+','+
              TBuffer::WordToString( _parName );
    }
    else if(Ptype->PType == 7)
    {
       ret += TBuffer::WordToString( this->Value )+','+
              TBuffer::WordToString( _parName );
    }
  return ret;
}
//=============================================================================  
string TAutomatDevice::GetStringCfgValue(void)
{
  string ret = "";
     
    if( Ptype->PType == 5 || Ptype->PType == 6)
    {
      if(Ptype->PType == 5)
      {
        if(_inA != NULL) ret += TBuffer::WordToString(_inA->Index)+','; else ret += "0,";
        if(_inB != NULL) ret += TBuffer::WordToString(_inB->Index)+','; else ret += "0,";
        if(_inC != NULL) ret += TBuffer::WordToString(_inC->Index)+','; else ret += "0,";
      }
      if(_outA != NULL) ret += TBuffer::WordToString(_outA->Index)+','; else ret += "0,";
      if(_outB != NULL) ret += TBuffer::WordToString(_outB->Index)+','; else ret += "0,";
      if(_outC != NULL) ret += TBuffer::WordToString(_outC->Index)+','; else ret += "0,";   

      ret += TBuffer::WordToString( _parName );
    }
    else if(Ptype->PType == 7)
    {
       ret += TBuffer::WordToString( _type )+',';
       if(_inB != NULL) ret += TBuffer::WordToString(_inB->Index)+','; else ret += "0,";
       ret += TBuffer::WordToString( _outState )+',';
       ret += TBuffer::WordToString( _parName );
    }
  return ret;
}
//=============================================================================
//=============================================================================  
//*****************************************************************************
//*****************************************************************************
//*****************************************************************************