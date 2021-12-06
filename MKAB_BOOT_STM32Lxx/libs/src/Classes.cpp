//=============================================================================
#include <Classes.h>
#include <flash_map.h>
#include <type_define.h>
#include <stm32l1xx_flash.h>
#include <stm32l1xx_rcc.h>
#include <stm32l1xx_usart.h>
//=================================================
 
//=============================================================================
//*****************************************************************************
//*** Class TObject
//*****************************************************************************
//=============================================================================
//  
//=======================================
//typedef  void (*pFunction)(void);
//pFunction Jump_To_Application;
void  TObject::SetDataTime(TDateTime &dt, string &date, string &time)
{
  //static bool flg = false;
  CLEAR_WDT;
  vector<string> tmpvect;   
  TRtcTime DateTime;
  TBuffer::Split(date,".",tmpvect);
  if(tmpvect.size() != 3){ return;}
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
  if( div > 2)//если разница большая - прямая запись значений времени
  {
    dt.SetTime(DateTime);//установка нового времени
  }

}
//=============================================================================
// Запись в память констант  
//=======================================
bool TObject::Save_ParamsInInfo(Data_Params  *Params)
{
//Перезапись всей страницы с временным(защитным) хранилищем в другой странице 
 Word         Len;
 Byte        *Tmp                  = (Byte *)Params;  //взяли указатель 
  
  Len = sizeof(*Params); 
  for(Word i=0; i<3; i++)
  {
    WriteEepromBufToPage((Byte const*)&TMP_PARAMS, Tmp, Len-1 );  //записали во временную(защитную) память
    WriteEepromBufToPage((Byte const*)&PARAMS    , Tmp, Len-1 );  //записали куда надо
  
    if(VerifyMemory((Byte const*)&PARAMS,Tmp, Len))
    {//удалили временную запись 
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
 Data_Params  TmpParam            = *PtrToConstants;    //сохранили текущие значения параметров в оперативке
 Byte        *Tmp                 = (Byte *)&TmpParam;  //взяли указатель 
  
  Len = sizeof(TmpParam);
  
  for(Word i = 0;i<3;i++)
  {
    CLEAR_WDT;
    WriteEepromBufToPage((Byte const*)&PARAMS    , Tmp, Len-1 );  //записали куда надо
    if(VerifyMemory((Byte const*)&PARAMS,Tmp, Len))
    {//удалили временную запись 
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
  Byte        *Tmp             = (Byte *)&TmpParam;  //взяли указатель 
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
  
  TmpParam.BOOT_PARAMS.BootStatus           = 0x00;
  TmpParam.BOOT_PARAMS.CurVersion           = BOOTLOADER_CURR_VER;
  
  Len = sizeof(TmpParam);
  WriteEepromBufToPage((Byte const*)&PARAMS    , Tmp, Len-1 );  //записали куда надо
}
//=============================================================================
//  
//=======================================
void TObject::WriteBufToPage(Byte const* PageAddr, Byte *Buf, Word BufLen)
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
//  
//=======================================
void  TObject::WriteHexBufToPage(Byte const* PageAddr, Byte *HexBuf, Word BufLen)
{
  Byte buffer_send[256] = {0};
  //uint32_t temp = 0; 
    for (Word i = 0;i<BufLen/2;i++)
      buffer_send[i] = TBuffer::HexToByte(HexBuf+i*2);
    
    WriteByteToFlash_MY(PageAddr, BufLen/2, buffer_send);

}
//=============================================================================
Word  TObject::ReturnFlashCRC(Byte const* PageAddr, Word BufLen)
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
Word  TObject::ReturnLongFlashCRC(Byte const* PageAddr, Word BufLen)
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
    }
    */
    if((*(DWord*)&PageAddr % PageSize) == 0)
      ClrFlashSegment( PageAddr );


  FLASH_Unlock();
  FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR | FLASH_FLAG_SIZERR | FLASH_FLAG_OPTVERR | FLASH_FLAG_OPTVERRUSR);
  //FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);	  // Clear All pending flags
  CLEAR_WDT;
  for(i=0;i<size_wr/4;i++)
  {
      FLASH_FastProgramWord( (uint32_t)PageAddr, *((uint32_t *)&ram_buf[i*4]) ); // запись 4х байтового слова
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
      DATA_EEPROM_FastProgramByte( (uint32_t)PageAddr, *((uint32_t *)&ram_buf[i]) ); // запись 1 байта
      PageAddr = PageAddr+1;
  }
  DATA_EEPROM_Lock();
  return true;
}
//=============================================================================
//  
//=======================================
#pragma optimize=z none
void  TObject::Soft_Reset(void)
{
    __disable_interrupt();
    NVIC_SystemReset();
}
//--------------------------------------------------------------------------------------------------
// Передать управление приложению по заданному адресу
//--------------------------------------------------------------------------------------------------
void   TObject::DirectJump(DWord Addr)
{
//uint32_t JumpAddress = 0;
//
//if (((*(__IO uint32_t*)Addr) & 0x2FFE0000 ) == 0x20000000)           // Test if user code is programmed starting from address "ApplicationAddress"
//    { 
//    JumpAddress = *(__IO uint32_t*) (Addr + 4);   // исполнительный адрес пользовательского приложения
//    Jump_To_Application = (pFunction) JumpAddress;
//    __set_MSP(*(__IO uint32_t*) Addr);            // инициализируем указатель стека в пользовательском приложении
//     Jump_To_Application();                                     // передаем управление пользовательскому приложению
//    }

  
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
    PocessList[i](NULL); 
    CLEAR_WDT;
  }
  //__low_power_mode_3(); 
}
//*****************************************************************************
//*** Class TBuffer
//*****************************************************************************
//=============================================================================
// 
//=======================================
Word TBuffer::Crc(Byte const* pcBlock, Word len)
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
Word TBuffer::AppCrc(DWord StartAddr, Word BlockCNT)
{
  Word counter = PAGE_BLOCK_LEN;
  Word CRC_SUM = 0;
  DWord  CurrAddr = StartAddr;
  
  while(BlockCNT)
  {
    CLEAR_WDT;
    
//    if(CurrAddr >= MEM_FIRM_START)
//    {
      CRC_SUM += TBuffer::Crc((Byte const*)CurrAddr, counter);
      BlockCNT--; 
    //} 
//    else
//    { CLEAR_WDT;}
    CurrAddr += counter;
  }
  return CRC_SUM;  
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
  return number.Data_s;
}
//=============================================================================
//
//=======================================
DWord TBuffer::HexToDWord(Byte *Ptr)
{
  //while(*Ptr < 0x20) Ptr++;
  Blong number;
  number.Data_l = 0;
  number.Data_b[3] =  HexToByte(Ptr); 
  Ptr+=2;
  number.Data_b[2] =  HexToByte(Ptr); 
  Ptr+=2;
  number.Data_b[1] =  HexToByte(Ptr); 
  Ptr+=2;
  number.Data_b[0] =  HexToByte(Ptr); 

  return number.Data_l;
}
//=============================================================================
//
//=======================================
DWord TBuffer::HexToDWord_(Byte *ptr)
{
Byte ch  = *ptr++;
Byte k   = 0;      
Byte result[7] = {0};
uint32_t out = 0;


    while(k < 7)
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
  out = 0;
  out = out | result[0] << 24;
  out = out | result[1] << 20;
  out = out | result[2] << 16;
  out = out | result[3] << 12;
  out = out | result[4] << 8;
  out = out | result[5] << 4;
  out = out | result[6];
  return out;
}
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
string TBuffer::WordToHex(Word WORD)
{
  Byte HEX[5] = {0x30,0x30,0x30,0x30,0};
   
  ByteToHex(HEX, (Byte)( WORD >> 8) );
  ByteToHex(&HEX[2], (Byte)( WORD) );
  
  return string((char*)HEX);
}
//=============================================================================
//
//=======================================
string TBuffer::DWordToHex(DWord Value)
{
  Byte HEX[9] = {0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0};
   
  ByteToHex(HEX, (Byte)( Value >> 24) );
  ByteToHex(&HEX[2], (Byte)( Value >> 16) );
  
   ByteToHex(&HEX[4], (Byte)( Value >> 8) );
  ByteToHex(&HEX[6], (Byte)( Value) ); 
  
  return string((char*)HEX);
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
Byte TBuffer::FloatToString(Byte *ReturnBUF, float Val, Byte AllSymb, Byte AfteComma)
{
  sprintf((char*)ReturnBUF,"%0*.*f",AllSymb,AfteComma,Val);
  return AllSymb;
}
//=============================================================================
//
//=======================================
string TBuffer::FloatToString(float Val, Byte AllSymb, Byte AfteComma)
{
  char buff[64];
  if(AllSymb == 0)
  { sprintf(buff,"%.*f",AfteComma,Val);}
  else
  { sprintf(buff,"%0*.*f",AllSymb,AfteComma,Val);}
  string MyString(buff);
  return MyString;
}
/*
//=============================================================================
//
//=======================================
Byte TBuffer::IntToString(Byte *ReturnBUF, int Val, Byte AllSymb)
{
  sprintf((char*)ReturnBUF,"%0*i",AllSymb,Val);
  return AllSymb;
}
//=============================================================================
//
//=======================================
string TBuffer::IntToString(int Val,Byte AllSymb)
{
char buff[64];
  if(AllSymb == 0)
  { sprintf(buff,"%i",Val);}
  else
  { sprintf(buff,"%0*i",AllSymb,Val);}
  string MyString(buff);
return MyString;
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
string TBuffer::LongToString(long Val,Byte AllSymb)
{
char buff[64];
  if(AllSymb == 0)
  { sprintf(buff,"%li",Val);}
  else
  { sprintf(buff,"%0*li",AllSymb,Val);}
  string MyString(buff);
return MyString;
}
//=============================================================================
//
//=======================================
void  TBuffer::Split(string &str, string separator, vector<string> &results, int Len)
{
    int found;
    found = str.find(separator);
    while(found != string::npos)
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
    }
    if(str.length() > 0)
    { results.push_back(str);}
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
    if(GlobalSeconds++ > 86400L)//ограничили макс-значение
    { GlobalSeconds = DateTime.Hour*3600L+DateTime.Minute*60L+DateTime.Second;}
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
    // Проверка високосного года
    if( (Year%4 == 0) && ((Year % 100 != 0) || (Year % 400 == 0)) )
    {  return true;} // високосный
    else
    {  return false;} //не високосный
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
  
 __enable_interrupt();

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
// читаем байт
//=======================================
bool TComPort::RecvByte(Byte *data)
{
  if ( this->DATA_Rx == NULL || this->DATA_Rx->empty() )
  {  return false;}
  else
  {
    *data = this->DATA_Rx->pop();
    this->SetTimer(RX_BYTE_TIMEOUT, time_outs.ReadIntervalTimeout);//брос межбайтового таймаута
    return true;
  }   
}
//=============================================================================
// пишем байт
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
     TTimer::wait_1us( (SMCLK / BaudRate)/15);
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
// Читаем (выход по таймауту)
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
         if ( GetTimValue(RX_BYTE_TIMEOUT) <= 0) //проверка на таймаут
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
// Читаем
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
           if ( GetTimValue(RX_BYTE_TIMEOUT) <= 0) //проверка на таймаут
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

}
//=============================================================================
// 
//=======================================
bool TComPort::GetTxIntFlag(void)
{
  uint16_t status = USART1->SR;
  return (bool)(status & USART_SR_TC);
}
//=============================================================================
// 
//=======================================
void TComPort::SetTxIntFlag(bool state)
{

}
//=============================================================================
// 
//=======================================
bool TComPort::UsartIsBusy(void)
{
  uint16_t status;
  uint16_t out;
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
// подготовка к приему данных
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
  CLEAR_WDT;
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
__disable_interrupt(); 
bool state;
  if (TimNam<TimerCNT)
  {
    Timer[TimNam] = Value;
    state = true;
  }
  else
  {
    state = false;
  }
  
__enable_interrupt(); 
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
  __disable_interrupt(); 
  sDWord Val;
  if (TimNam>=TimerCNT)
      Val = -1;
  else
    Val = Timer[TimNam];
  
  __enable_interrupt(); 
  return Val;
}
//=============================================================================
// 
//=======================================
void TTimer::wait_1ms(Word cnt)
{
    Word i;

        CLEAR_WDT;
	for (i = 0; i < cnt; i++)
        { 
//          __delay_cycles(14745);
        }
        CLEAR_WDT;
}
//=============================================================================
// 
//=======================================
void TTimer::wait_1us(Word cnt)
{
	for (Word i = 0; i < cnt; i++)
        { 
//          __delay_cycles(14);
        }
}

//*****************************************************************************
//*****************************************************************************