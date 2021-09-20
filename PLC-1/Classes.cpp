//=============================================================================
#include <Classes.h>
#include <intrinsics.h>
#include <type_define.h>
#include <stm32f10x_flash.h>
//=================================================
extern long   RtcTimerDivider;
extern Word64 SecCount;
//=============================================================================
extern Data_Params* PtrToConstants;
extern Boot_Params* PtrToBootConstants;
extern bool         NeedReload;
//=============================================================================
//*****************************************************************************
//*** Class TObject
//*****************************************************************************
//=============================================================================
// 
//=======================================
bool TObject::Save_ParamsInInfo(Data_Params  *Params)
{
 Word         Len;
 Byte        *Tmp                  = (Byte *)Params;  
  
  Len = sizeof(*Params); 
  for(Word i=0; i<3; i++)
  {

    WriteBufToPage((Byte const*)TMP_PARAMS_ADDR, Tmp, Len-1 );  //
    WriteBufToPage((Byte const*)PARAMS_ADDR    , Tmp, Len-1 );  //
  
    if(VerifyMemory((Byte const*)PARAMS_ADDR, Tmp, Len))
    {
      ClrFlashSegment( (Byte const*)TMP_PARAMS_ADDR );
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
bool TObject::RestoreMemParams(Data_Params *ptr)
{
 Word         Len;
 Data_Params  TmpParam            = *ptr;    //
 Byte        *Tmp                 = (Byte *)&TmpParam;  //
  
  Len = sizeof(TmpParam);
  
  for(Word i = 0;i<3;i++)
  {
    CLEAR_WDT;

    WriteBufToPage((Byte const*)PARAMS_ADDR,  Tmp, Len-1 );  //
    if(VerifyMemory((Byte const*)PARAMS_ADDR, Tmp, Len))
    {//
      ClrFlashSegment( (Byte const*)TMP_PARAMS_ADDR );
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
void TObject::RestoreDefault(void)
{
  Word         Len;
  Data_Params  TmpParam;
  Byte        *Tmp             = (Byte *)&TmpParam;  //

  
#ifdef UM_DEV   
        TmpParam.DATA[DEV_ADDR] = 2,
#else
        TmpParam.DATA[DEV_ADDR] = 1,
#endif 
  
        TmpParam.DATA[TIME_OUT]         = 50;
        TmpParam.DATA[BOUD_RATE]        = 9600;
        TmpParam.DATA[BYTES_CNT]        = 200;
        TmpParam.DATA[MIN_BYTE_LEN]     = 3;
        
        TmpParam.DATA[PLC_TIME_OUT]     = 200;
        TmpParam.DATA[PLC_BOUD_RATE]    = 600;
        TmpParam.DATA[PLC_BYTES_CNT]    = 250;
        TmpParam.DATA[PLC_MIN_BYTE_LEN] = 10;
        
        TmpParam.DATA[DEV_CURR_VER]       = 4;
        TmpParam.DATA[DEV_SUB_VER]        = 0;
        
        TmpParam.DATA[SERIAL_LOW]       = 0;
        TmpParam.DATA[SERIAL_HI]        = 0;
                
        TmpParam.filter = 1;                    // 0 - off, 1- on
        TmpParam.sensitive = 1;                 // 0 - off, 1- on
        TmpParam.deviation = 1;                 // 0 - off, 1- on
  
  Len = sizeof(TmpParam);
  WriteBufToPage((Byte const*)PARAMS_ADDR, Tmp, Len-1 );  //
}
//=============================================================================
//  
//=======================================
void TObject::WriteBufToPage(Byte const* PageAddr, Byte *Buf, Word BufLen)
{
  ClrFlashSegment( PageAddr );
  Buf[BufLen] = 0xAA;
  BufLen++;
  WriteByteToFlash(PageAddr, BufLen, Buf);
}
//=============================================================================
//  
//=======================================
void  TObject::WriteHexBufToPage(Byte const* PageAddr, Byte *HexBuf, Word BufLen)
{
  Byte buffer_send[256] = {0};
    for (Word i = 0;i<BufLen/2;i++)
      buffer_send[i] = TBuffer::HexToByte(HexBuf+i*2);
    if((*(DWord*)&PageAddr % 1024) == 0)
      ClrFlashSegment( PageAddr );

    WriteByteToFlash(PageAddr, BufLen/2, buffer_send);

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
////=============================================================================
////  
////=======================================
FLASH_Status FLASH_GetBank1Status(void)
{
  FLASH_Status flashstatus = FLASH_COMPLETE;
  
  if((FLASH->SR & FLASH_FLAG_BANK1_BSY) == FLASH_FLAG_BSY) 
  {
    flashstatus = FLASH_BUSY;
  }
  else 
  {  
    if((FLASH->SR & FLASH_FLAG_BANK1_PGERR) != 0)
    { 
      flashstatus = FLASH_ERROR_PG;
    }
    else 
    {
      if((FLASH->SR & FLASH_FLAG_BANK1_WRPRTERR) != 0 )
      {
        flashstatus = FLASH_ERROR_WRP;
      }
      else
      {
        flashstatus = FLASH_COMPLETE;
      }
    }
  }
  /* Return the Flash Status */
  return flashstatus;
}

//=============================================================================


bool TObject::ClrFlashSegment(Byte const *SegAddr)
{  
Byte status = FLASH_COMPLETE;
FLASH->KEYR = FLASH_KEY1;
FLASH->KEYR = FLASH_KEY2;


FLASH->SR = FLASH_FLAG_EOP;
FLASH->SR = FLASH_FLAG_PGERR;
FLASH->SR = FLASH_FLAG_WRPRTERR;


while (FLASH->SR & FLASH_SR_BSY);
if (FLASH->SR & FLASH_SR_EOP) {
        FLASH->SR = FLASH_SR_EOP;
}

FLASH->CR |= FLASH_CR_PER;
FLASH->AR = *(uint32_t*)&SegAddr;
FLASH->CR |= FLASH_CR_STRT;
while (!(FLASH->SR & FLASH_SR_EOP));
FLASH->SR = FLASH_SR_EOP;
FLASH->CR &= ~FLASH_CR_PER;

FLASH->CR |= CR_LOCK_Set;
  /* Return the Erase Status */
return status;
}


//=============================================================================
//  
//=======================================
FLASH_Status FLASH_ProgramWord(uint32_t Address, uint32_t Data)
{
  FLASH_Status status = FLASH_COMPLETE;
  __IO uint32_t tmp = 0;


  /* Wait for last operation to be completed */
  status = FLASH_WaitForLastOperation(ProgramTimeout);
  
  if(status == FLASH_COMPLETE)
  {
    /* if the previous operation is completed, proceed to program the new first 
    half word */
    FLASH->CR |= CR_PG_Set;
  
    *(__IO uint16_t*)Address = (uint16_t)Data;
    /* Wait for last operation to be completed */
    status = FLASH_WaitForLastOperation(ProgramTimeout);
 
    if(status == FLASH_COMPLETE)
    {
      /* if the previous operation is completed, proceed to program the new second 
      half word */
      tmp = Address + 2;

      *(__IO uint16_t*) tmp = Data >> 16;
    
      /* Wait for last operation to be completed */
      status = FLASH_WaitForLastOperation(ProgramTimeout);
        
      /* Disable the PG Bit */
      FLASH->CR &= CR_PG_Reset;
    }
    else
    {
      /* Disable the PG Bit */
      FLASH->CR &= CR_PG_Reset;
    }
  }         
   
  /* Return the Program Status */
  return status;
}


//--------------------------------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------------------------------
FLASH_Status FLASH_WaitForLastOperation(uint32_t Timeout)
{ 
  FLASH_Status status = FLASH_COMPLETE;
   
  /* Check for the Flash Status */
  status = FLASH_GetBank1Status();
  /* Wait for a Flash operation to complete or a TIMEOUT to occur */
  while((status == FLASH_BUSY) && (Timeout != 0x00))
  {
    status = FLASH_GetBank1Status();
    Timeout--;
  }
  if(Timeout == 0x00 )
  {
    status = FLASH_TIMEOUT;
  }
  /* Return the operation status */
  return status;
}


//--------------------------------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------------------------------
bool TObject::WriteByteToFlash(Byte const * PageAddr, Word size_wr, Byte* ram_buf)
{
//static Byte buffer_for_write[4];
static Byte i;

FLASH->KEYR = FLASH_KEY1;													
FLASH->KEYR = FLASH_KEY2;

//FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);	  // Clear All pending flags
FLASH->SR = FLASH_FLAG_EOP;
FLASH->SR = FLASH_FLAG_PGERR;
FLASH->SR = FLASH_FLAG_WRPRTERR;


for(i=0;i<size_wr/4;i++)
    {
    FLASH_ProgramWord(*(uint32_t *) &PageAddr,*(uint32_t *) &ram_buf[i*4]); // запись 4х байтового слова
    PageAddr = PageAddr +4;
    }

  FLASH->CR |= CR_LOCK_Set;
  return true;
}








//=============================================================================
//  
//=======================================
void  TObject::Soft_Reset(void)
{
  NVIC_SystemReset();
}
//--------------------------------------------------------------------------------------------------
// Передать управление приложению по заданному адресу
//--------------------------------------------------------------------------------------------------

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
Word TBuffer::Crc(Byte *pcBlock, Word len)
{
  Byte ires = 0;
  for (int i = 0; i < len; i+=2){
    ires += HexToByte(&pcBlock[i]);
  }
//  return ires;
  return (0x100-ires);  
}
//=============================================================================
// 
//=======================================
Byte TBuffer::CycleCrcByte(Byte const* pcBlock, Byte len, int cur)
{
  Byte crc = 0;

    while (len>0)
    { 
    
    if(cur <  0)
    {
      cur = 6;
    }
    len --;
    crc += pcBlock[cur];
    cur--;
    crc += (crc >>1);
    }
    return (~crc);
}


//=============================================================================
// 
//=======================================
uint8_t tessst;
Byte TBuffer::CrcByte(Byte const* pcBlock, Word len)
{
    unsigned char crc = 0xFF;
    unsigned int i;

    while (len--)
    {
        crc ^= pcBlock[len];

        for (i = 0; i < 8; i++)
            crc = crc & 0x80 ? (crc << 1) ^ 0x31 : crc << 1;
    }

    return crc;
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
Word TBuffer::CopyBuffer(TBuffer* Dest, TBuffer* Src)
{
  Word len = 0;
  
  for(int i = 0 ; i < Src->DataLen; i ++)
  {
    Dest->BUF[i] = Src->BUF[i];
    len++;
  }
  Dest->DataLen = Src->DataLen;
  return len;
}   
//=============================================================================
//
//=======================================
Word TBuffer::CopyBuffer(Byte* Dest, Byte* Src, Word DataLen)
{
  Word len = 0;
  
  for(int i = 0 ; i < DataLen; i ++)
  {
    Dest[i] = Src[i];
    len++;
  }
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
}
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
//=======================================
//
//=======================================
Byte TBuffer::CheckSum(Byte *BUF, Word Len)
{
  int ires = 0;
  for (int i = 0; i < Len; i+=2){
    ires += HexToByte(&BUF[i]);
  }
  return (0x100-ires);
}
//=============================================================================
//
//=======================================
bool TBuffer::ParsingAnswer( Byte *BUF, Word Len)
{
  if(Len > 10)
  {
    if(BUF[Len-2]==0x0D && BUF[Len-1]==0x0A)
    {
      int s = TBuffer::find_first_of(BUF, Len, STX);
      if(s >=0 && (Len-s) > 10)
      {
        Byte addr = HexToByte(&BUF[s+1]);
        Byte needAddr = PtrToConstants->DATA[DEV_ADDR];
        if(addr != needAddr)
          return false;
        
        Byte lrc = HexToByte(&BUF[Len-4]);
        Byte calcLrc = CheckSum(&BUF[s+1], Len-s-5);
	if(lrc == calcLrc)
          return true;
      }
    }
    return false;
  }
  else 
    return false;
}
//=======================================
int  TBuffer::GetDataFroMessage(Byte *Buffer, Word DataLen)
{
  int index = 0;
  int s = find_first_of(Buffer, DataLen, STX);
  if(s >=0)
  {
        Bshort reg;
        Bshort data;
        Bshort cnt;
        Byte bf[] = {0,0,0,0};
        Byte addr = HexToByte(&Buffer[s+1]);
        Byte needAddr = PtrToConstants->DATA[DEV_ADDR];
        DataLen = 0;
        if(addr == needAddr)
        {
          Byte fun = HexToByte(&Buffer[s+3]);
          reg.Data_b[1] = HexToByte(&Buffer[s+5]);
          reg.Data_b[0] = HexToByte(&Buffer[s+7]);
          
          bool failFlag = true;
          Buffer[DataLen++] = STX;
          
          ByteToHex(bf, needAddr);
          Buffer[DataLen++] = bf[0];
          Buffer[DataLen++] = bf[1];
          
          ByteToHex(bf, fun);
          Buffer[DataLen++] = bf[0];
          Buffer[DataLen++] = bf[1];
          
          Word END_HOLDIG_REGISTERS_ADDR  = ( START_HOLDIG_REGISTERS_ADDR+sizeof(PtrToConstants->DATA)/2 );
          switch(fun)
          {
          case 3://START_HOLDIG_REGISTERS_ADDR  = 300
              if( reg.Data_s >= START_HOLDIG_REGISTERS_ADDR && reg.Data_s < END_HOLDIG_REGISTERS_ADDR)
              {
                  cnt.Data_b[1] = HexToByte(&Buffer[s+9]);
                  cnt.Data_b[0] = HexToByte(&Buffer[s+11]);
                  Word l = reg.Data_s+cnt.Data_s > END_HOLDIG_REGISTERS_ADDR ? (END_HOLDIG_REGISTERS_ADDR-reg.Data_s):cnt.Data_s;
                  
                  ByteToHex(bf, sizeof(data)*l);
                  Buffer[DataLen++] = bf[0];
                  Buffer[DataLen++] = bf[1];
              
                  Word const*data_ptr = &PtrToConstants->DATA[0];
                  for(int i=(reg.Data_s-START_HOLDIG_REGISTERS_ADDR); i < l; i++)
                  {
                    data.Data_s = data_ptr[i];
                    
                    ByteToHex(bf, data.Data_b[1]);
                    Buffer[DataLen++] = bf[0];
                    Buffer[DataLen++] = bf[1];
                    
                    ByteToHex(bf, data.Data_b[0]);
                    Buffer[DataLen++] = bf[0];
                    Buffer[DataLen++] = bf[1];
                  }
                  failFlag = false;
            }
            break;
          case 6://PtrToConstants addr = 0x1800(6144).  
            if( reg.Data_s >= START_HOLDIG_REGISTERS_ADDR && reg.Data_s < (END_HOLDIG_REGISTERS_ADDR - 1) )
            {
              data.Data_b[1] = HexToByte(&Buffer[s+9]);
              data.Data_b[0] = HexToByte(&Buffer[s+11]); 
              
              Data_Params Params = *PtrToConstants;
              int ind = reg.Data_s - START_HOLDIG_REGISTERS_ADDR;
              Word *param_ptr = (Word*)&Params;
              if(param_ptr[ind] == data.Data_s)
                return 0; //opd_par==new_par - do not write exit whitout answer 
              
              param_ptr[ind] = data.Data_s;
              TObject::Save_ParamsInInfo(&Params);//save
              
              ByteToHex(bf, reg.Data_b[1]);
              Buffer[DataLen++] = bf[0];
              Buffer[DataLen++] = bf[1];
              
              ByteToHex(bf, reg.Data_b[0]);
              Buffer[DataLen++] = bf[0];
              Buffer[DataLen++] = bf[1];
              
              Params = *PtrToConstants;
              data.Data_s = param_ptr[ind];
              
              ByteToHex(bf, data.Data_b[1]);
              Buffer[DataLen++] = bf[0];
              Buffer[DataLen++] = bf[1];
              
              ByteToHex(bf, data.Data_b[0]);
              Buffer[DataLen++] = bf[0];
              Buffer[DataLen++] = bf[1];
              
              failFlag    = false;
              //needReload  = true;
            }
            break;  
            
          case 16:
            if( reg.Data_s >= START_HOLDIG_REGISTERS_ADDR && reg.Data_s < (END_HOLDIG_REGISTERS_ADDR - 1) )
            {//
              data.Data_b[1] = HexToByte(&Buffer[s+9]);
              data.Data_b[0] = HexToByte(&Buffer[s+11]); 
              Word Q = data.Data_s;
              if(Q > (sizeof(Data_Params)/2))
                return 0;
              
              int si = 15;
              Data_Params Params = *PtrToConstants;
              Word *param_ptr = (Word*)&Params;
              bool needWrite = false;
              for(int i=0; i < Q; i++)
              {
                int ind = reg.Data_s+i - START_HOLDIG_REGISTERS_ADDR;
                if(ind >= 0){
                  data.Data_b[1] = HexToByte(&Buffer[s+si]);
                  data.Data_b[0] = HexToByte(&Buffer[s+(si+2)]); 
                  si+=4;
                  if(param_ptr[ind] != data.Data_s)
                  {
                    param_ptr[ind] = data.Data_s;
                    needWrite = true;
                  }
                }
              }
              if(needWrite)
              {
                TObject::Save_ParamsInInfo(&Params);//save
                ByteToHex(bf, reg.Data_b[1]);
                Buffer[DataLen++] = bf[0];
                Buffer[DataLen++] = bf[1];
                
                ByteToHex(bf, reg.Data_b[0]);
                Buffer[DataLen++] = bf[0];
                Buffer[DataLen++] = bf[1];  
                
                data.Data_s = Q;
                ByteToHex(bf, data.Data_b[1]);
                Buffer[DataLen++] = bf[0];
                Buffer[DataLen++] = bf[1];
                
                ByteToHex(bf, data.Data_b[0]);
                Buffer[DataLen++] = bf[0];
                Buffer[DataLen++] = bf[1];
                //NeedReload  = true;
              }
              else
                return 0;
              failFlag    = false;
            }
            break; 
            
          };
          if(failFlag){
            data.Data_s = 0x7FFF;
            ByteToHex(bf, sizeof(data));
            Buffer[DataLen++] = bf[0];
            Buffer[DataLen++] = bf[1];
            
            ByteToHex(bf, data.Data_b[1]);
            Buffer[DataLen++] = bf[0];
            Buffer[DataLen++] = bf[1];
            
            ByteToHex(bf, data.Data_b[0]);
            Buffer[DataLen++] = bf[0];
            Buffer[DataLen++] = bf[1];
          }
          ByteToHex(bf, CheckSum(&Buffer[1], DataLen-1));
          Buffer[DataLen++] = bf[0];
          Buffer[DataLen++] = bf[1];

          Buffer[DataLen++] = 0x0D;
          Buffer[DataLen++] = 0x0A;
        }
  }
  index = DataLen;
  return index;
}
//*****************************************************************************
//*****************************************************************************
//*** Class TSpiPort
//*****************************************************************************
TSpiPort::TSpiPort():enabled(false),spiPortHandle(INVALID_HANDLE_VALUE)
{
}
//=============================================================================
// Destructor
//=======================================
TSpiPort::~TSpiPort()
{
	if (spiPortHandle != INVALID_HANDLE_VALUE)
	{	CloseHandle(spiPortHandle);}
}
//=============================================================================
//
//=======================================
HANDLE TSpiPort::open(SPI_TypeDef* SPIx, DWord baudRate, DWord ReadIntervalTimeout)
{
//init slave  
  if(SPIx == SPI1)
  {
      //init MOSI MISO SCK  GPIO 
      GPIOA->CRL |= GPIO_CRL_CNF5_0;
      GPIOA->CRL |= GPIO_CRL_CNF6_1;
      GPIOA->CRL |= GPIO_CRL_MODE6;
      GPIOA->CRL |= GPIO_CRL_CNF7_0;
      RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;                                       // Включаем тактирование на SPI1
      SPIx->CR1 =  SPI_CR1_SSM;
      SPIx->CRCPR = 0x000A; 
      //RX_BUF_SPI1 = (Byte*)&SPI1->DR;
      //TX_BUF_SPI1 = (Byte*)&SPI1->DR;   
      //NVIC_EnableIRQ(SPI1_IRQn);     //Включение вектора прерывания
      
  }
      enabled                         = true;    
      spiPortHandle                  = (HANDLE)SPIx;
  return spiPortHandle; 
}
//=============================================================================
//
//=======================================
void TSpiPort::read_register (void)
{
    
    GPIOB->BSRR = GPIO_PIN_12;
    GPIOB->BSRR = GPIO_PIN_0;
    GPIOB->BSRR = (uint32_t)GPIO_PIN_12 << 16U;
}   
//=============================================================================
//
//=======================================
void TSpiPort::write_register (void)
{
    Byte write_param;  
    GPIOB->BSRR = GPIO_PIN_12;                                                      //REG_DATA_SET
    GPIOB->BRR = (uint32_t)GPIO_PIN_0 << 16U;                                       //RxTx_RESET
    SPI1->CR1 |=  SPI_CR1_SPE;

    while(1)
    {
      if((__HAL_SPI_GET_FLAG_MY(SPI1, SPI_SR_BSY) != 1 ) && ((__HAL_SPI_GET_FLAG_MY(SPI1, SPI_SR_TXE) != 0) ))
        break;
    }
    write_param = 0x13;
    
    if(params->filter == 1)
      write_param = write_param | 0x40;
    if(params->sensitive == 1)
      write_param = write_param | 0x80;
    SPI1->DR = write_param;
    while(1)
    {
    if((__HAL_SPI_GET_FLAG_MY(SPI1, SPI_SR_BSY) != 1 ) && ((__HAL_SPI_GET_FLAG_MY(SPI1, SPI_SR_TXE) != 0) ))
        break;
    }
    SPI1->DR = 0x62;

    while(1)
    {
    if((__HAL_SPI_GET_FLAG_MY(SPI1, SPI_SR_BSY) != 1 ) && ((__HAL_SPI_GET_FLAG_MY(SPI1, SPI_SR_TXE) != 0) ))
        break;
    }
    
    
    write_param = 0x02;//0x82;
    if(params->deviation == 1)
      write_param = write_param | 0x20;
    
    Word baudrate = params->DATA[PLC_BOUD_RATE];
    if(baudrate == 1200)
      write_param = write_param | 8;
    else if(baudrate == 2400)
      write_param = write_param | 0x10;
    else if(baudrate == 4800)
      write_param = write_param | 0x18;
    
    SPI1->DR = write_param;

    while(1)
    {
    if((__HAL_SPI_GET_FLAG_MY(SPI1, SPI_SR_BSY) != 1 ) && ((__HAL_SPI_GET_FLAG_MY(SPI1, SPI_SR_TXE) != 0) ))
        break;
    }

    SPI1->CR1 &= (~SPI_CR1_SPE);
    GPIOB->BSRR = GPIO_PIN_0;                                                       //RxTx_SET
    GPIOB->BRR = (uint32_t)GPIO_PIN_12 << 16U;                                      //REG_DATA_RESET
}
//=============================================================================
// Closes the com port 
//=======================================
bool TSpiPort::close()
{
	if (spiPortHandle != INVALID_HANDLE_VALUE)
	{	CloseHandle(spiPortHandle);}
	spiPortHandle = INVALID_HANDLE_VALUE;
	return true;
}
//=============================================================================
// Closes the com port Handle
//=======================================
bool TSpiPort::CloseHandle(HANDLE commPort)
{
    GPIOA->CRL &= (~GPIO_CRL_CNF5_0);
    GPIOA->CRL &= (~GPIO_CRL_CNF6_1);
    GPIOA->CRL &= (~GPIO_CRL_MODE6);
    GPIOA->CRL &= (~GPIO_CRL_CNF7_0);
    GPIOA->BSRR |= GPIO_PIN_4;                                       //SPI_UART = 1 / SET UART
  return true;
}

//*****************************************************************************
//*****************************************************************************
//*** Class TUsartPort
//*****************************************************************************
TUsartPort::TUsartPort():enabled(false),TTimer(4),usartPortHandle(INVALID_HANDLE_VALUE),ByteIndex(0),time_outs(),CRC_SUM(0),StxFlag(false),DataReceiving(false)
{
  DATA_Rx = NULL;
}
//=============================================================================
// Destructor
//=======================================
TUsartPort::~TUsartPort()
{
	if (usartPortHandle != INVALID_HANDLE_VALUE)
	{	CloseHandle(usartPortHandle);}
}
//=============================================================================
//
//=======================================
void TUsartPort::PushByteToDATA_Rx(Byte Data)
{
  if(DATA_Rx!=NULL)
  {
    DATA_Rx->push(Data);
  }
}
//=============================================================================
//
//=======================================
sWord TUsartPort::PopByteDATA_Rx(void)
{
  if(DATA_Rx!=NULL)
  { return DATA_Rx->pop();}
  else
  { return -1;}
}
//=============================================================================
// Closes the com port 
//=======================================
bool TUsartPort::close()
{
	if (usartPortHandle != INVALID_HANDLE_VALUE)
	{	CloseHandle(usartPortHandle);}
	usartPortHandle = INVALID_HANDLE_VALUE;
	return true;
}
//=============================================================================
// Closes the com port Handle
//=======================================
bool TUsartPort::CloseHandle(HANDLE commPort)
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
void TUsartPort::ClrByteIndex(void)
{
}
//=============================================================================
//=======================================
void TUsartPort::SetTimeOuts(COMMTIMEOUTS timeouts)
{
  time_outs = timeouts;
}
//=============================================================================
//=======================================        
void TUsartPort::GetTimeOuts(COMMTIMEOUTS &timeouts)
{
  timeouts = time_outs; 
}




//=============================================================================
// open com port 
//=======================================
HANDLE TUsartPort::open(Port comport, DWord baudRate, DWord ReadIntervalTimeout)
{
      COMMTIMEOUTS timeouts;
      bool flag     = true;
      __disable_interrupt(); 
      USART_TypeDef* USARTx;
      IRQn_Type IRQn;
      
      if(comport == COM1)
      {
          USARTx = USART1;
          IRQn = USART1_IRQn;
          
          //init RX_TX GPIO
          GPIOA->CRH = GPIO_CRH_CNF9_1;
          GPIOA->CRH |= GPIO_CRH_MODE9;
          GPIOA->CRH |= GPIO_CRH_CNF10_0;
          RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
          
          USARTx->CR1 =  USART_CR1_RE;
          USARTx->CR1 |=  USART_CR1_TE;
          USARTx->CR1 |= USART_CR1_RXNEIE;
          USARTx->CR1 |= USART_CR1_UE;
          
          InitPort(baudRate);    
          RX_BUF_USART = (Byte*)&USART1->DR;
          TX_BUF_USART = (Byte*)&USART1->DR;
      }
      else if(comport == COM2)
      {
          USARTx = USART2;
          IRQn = USART2_IRQn;
          //init RX_TX GPIO
          GPIOA->CRL = GPIO_CRL_CNF2_1;
          GPIOA->CRL |= GPIO_CRL_MODE2;
          GPIOA->CRL |= GPIO_CRL_CNF3_0;

          //init periph on usart
          RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
          
          USARTx->CR1 = USART_CR1_RE;
          USARTx->CR1 |= USART_CR1_TE;
          USARTx->CR1 |= USART_CR1_RXNEIE;
          USARTx->CR1 |= USART_CR1_UE;
          RX_BUF_USART = (Byte*)&USART2->DR;
          TX_BUF_USART = (Byte*)&USART2->DR;      
      }
     else if(comport == COM3)    
     {
          USARTx = USART3;
          IRQn = USART3_IRQn;
          //init RX_TX GPIO
          GPIOB->CRH |= GPIO_CRH_CNF10_1;
          GPIOB->CRH |= GPIO_CRH_MODE10;
          GPIOB->CRH |= GPIO_CRH_CNF11_0;

          //init periph on usart
          RCC->APB1ENR |= RCC_APB1ENR_USART3EN;
          
          USARTx->CR1 = USART_CR1_RE;
          USARTx->CR1 |= USART_CR1_TE;
          USARTx->CR1 |= USART_CR1_RXNEIE;
          USARTx->CR1 |= USART_CR1_UE;
          RX_BUF_USART = (Byte*)&USART3->DR;
          TX_BUF_USART = (Byte*)&USART3->DR;
      }
      else
      {
           if(flag)
          {
            __enable_interrupt();
          }
          enabled                         = false;  
          return INVALID_HANDLE_VALUE;
      }
      usartPortHandle  = (HANDLE)USARTx;
      InitPort(baudRate);  

      timeouts.ReadIntervalTimeout    = ReadIntervalTimeout;
      timeouts.ReadTotalTimeout       = timeouts.ReadIntervalTimeout*timeouts.ReadTotalTimeoutMultiplier;
      SetTimeOuts(timeouts); 
      
      enabled                         = true;    
      NVIC_EnableIRQ(IRQn);//Включение вектора прерывания
      
      if(flag)
      {
        __enable_interrupt();
      }
  return usartPortHandle; 
}
//=============================================================================
// читаем байт
//=======================================
bool TUsartPort::RecvByte(Byte *data)
{
  if ( DATA_Rx == NULL || DATA_Rx->empty() )
  {
    return false;
  }
  else
  {
    *data = DATA_Rx->pop();
    SetTimer(RX_BYTE_TIMEOUT, time_outs.ReadIntervalTimeout);//брос межбайтового таймаута
    return true;
  }   
}
//=============================================================================
// пишем байт
//=======================================
bool TUsartPort::SendByte(Byte data)
{    
       USART_TypeDef* USARTx = (USART_TypeDef*)usartPortHandle;
       if ( GetTxIntFlag() )
       {
          this->SetTxIntEnable(true);
          USARTx->DR = data;
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
sWord TUsartPort::Send(Byte *Buf, sWord Len, bool separate)
{
  if (Len <= 0)
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
sWord TUsartPort::Recv(Byte *Buf, Word Len, Word MaxLen)
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
         long tim = GetTimValue(RX_BYTE_TIMEOUT);
         if ( tim <= 0) //
         {    
             Word TmpWord = ByteIndex;
             ByteIndex       = 0x00;
             DataReceiving = false;
             if(TmpWord > 10)
             {//debug
               ByteIndex       = 0x00;
             }
             return TmpWord;
         }  
      }
      return -1;    
}
//=============================================================================
// Читаем
//=======================================
uint32_t temp_var;
sWord TUsartPort::RecvTo(Byte *Buf, Word MaxLen, Byte StopSymbol)
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
        Buf[ByteIndex++] = data;
        if(ByteIndex >=MaxLen)
        {
             Word TmpWord = ByteIndex;
             ByteIndex       = 0x00;
             DataReceiving = false;
             return TmpWord;
        }          
      };
      
      if(DataReceiving == true)
      { 
          temp_var = GetTimValue(RX_BYTE_TIMEOUT);
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
void TUsartPort::ClearRxBUF()
{
  if(DATA_Rx!=NULL)
  {DATA_Rx->clear();}
}
//=======================================
bool TUsartPort::Busy()
{
  return (bool)ByteIndex;
}
//=============================================================================
// 
//=======================================
void TUsartPort::InitPort(DWord BAUDRATE)
{
  USART_TypeDef* USARTx = (USART_TypeDef*)usartPortHandle;
  switch (BAUDRATE)
  {
    case 600:
        USARTx->BRR =(833<<4); 	// 80000000 / 16 / 600 = 833.33
        USARTx->BRR |= 5; 	// Дробная часть коэффициента деления USART 0.33 * 16 = 2.64
        break;
        
    case 1200:
        USARTx->BRR =(416<<4);
        USARTx->BRR |= 10;
        break;
    
    case 2400:
        USARTx->BRR =(208<<4);
        USARTx->BRR |= 5;
        break;
        
    case 4800:
        USARTx->BRR =(104<<4);
        USARTx->BRR |= 2;
        break;
    
    case 9600:
        USARTx->BRR =(52<<4);
        USARTx->BRR |= 1;
        break;
        
    case 19200:
        USARTx->BRR =(26<<4);
        break;
    
    case 38400:
        USARTx->BRR =(13<<4);
        break;
        
    case 115200:
    USARTx->BRR =(4<<4);
    USART2->BRR |= 5;
        break;

    case 250000:
    USARTx->BRR =(2<<4);
       break;
  } 
}
//=============================================================================
// 
//=======================================
bool TUsartPort::GetTxIntFlag()
{
  USART_TypeDef* USARTx = (USART_TypeDef*)usartPortHandle;
  uint16_t status = USARTx->SR;
  return (bool)(status & USART_SR_TC);
}
//=============================================================================
// 
//=======================================
void TUsartPort::SetTxIntFlag(bool state)
{

}
//=============================================================================
// 
//=======================================
bool TUsartPort::UsartIsBusy()
{
  USART_TypeDef* USARTx = (USART_TypeDef*)usartPortHandle;
  uint16_t status;
  uint16_t out;
  status = USARTx->SR;
  out = (~status & USART_SR_TC);
  //out = ~out;
  return (bool) out;
}

//=============================================================================
// 
//=======================================
void TUsartPort::SetTxIntEnable(bool state)
{
  USART_TypeDef* USARTx = (USART_TypeDef*)usartPortHandle;
  if(state == true)
  {  
     USARTx->CR1  |=  USART_CR1_TXEIE;
     //USARTx->CR1  &= ~USART_CR1_RXNEIE;
  }
  else
  {  
     USARTx->CR1  &= ~USART_CR1_TXEIE;
     //USARTx->CR1  |=  USART_CR1_RXNEIE;
  }
}
//=============================================================================
// 
//=======================================
void TUsartPort::SetRxIntEnable(bool state)
{
  USART_TypeDef* USARTx = (USART_TypeDef*)usartPortHandle;
  if(state == true)
  {  USARTx->CR1  |=  USART_CR1_RXNEIE;}
  else
  {  USARTx->CR1  &= ~USART_CR1_RXNEIE;}
}
//=============================================================================
// 
//=======================================
Byte TUsartPort::GetRxDataByte()
{
  //USART_TypeDef* USARTx = (USART_TypeDef*)usartPortHandle;
  return *RX_BUF_USART;
}
//=============================================================================
// 
//=======================================       
void TUsartPort::SetTxDataByte(Byte data)
{
  //USART_TypeDef* USARTx = (USART_TypeDef*)usartPortHandle;
  *TX_BUF_USART = data;
}
//=============================================================================
//
//=======================================
bool    TUsartPort::TestStxFlag(void)
{
   return StxFlag;
}
//=============================================================================
// подготовка к приему данных
//=======================================
void    TUsartPort::ClrStxFlag(void)
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
    DWord i;
    CLEAR_WDT;
    for (i = 0; i < cnt; i++){ 
      wait_1us(1000);
    }
    CLEAR_WDT;
}
//=============================================================================
// 
//=======================================
void TTimer::wait_1us(Word cnt)
{
    for (Word i = 0; i < cnt; i++){ 
          asm("nop");
          asm("nop");
          asm("nop");
          asm("nop");
          asm("nop");
          asm("nop");
          asm("nop");
          asm("nop");
          asm("nop");
          asm("nop");
     }
}
//*****************************************************************************
//*** Класс TSpiPort0 
//*****************************************************************************
//=============================================================================
// Constructor
//=======================================
TSpiPort0::TSpiPort0()
{ 
}
//=============================================================================
// Destructor
//=======================================
TSpiPort0::~TSpiPort0()
{
}
//*****************************************************************************
//*** Класс TUsartPort0 
//*****************************************************************************
//=============================================================================
// Constructor
//=======================================
TUsartPort0::TUsartPort0(Word IntBufSize)
{ 
  DATA_Rx = new TFifo<Byte>(IntBufSize);
}
//=============================================================================
// Destructor
//=======================================
TUsartPort0::~TUsartPort0()
{
  delete DATA_Rx;
}
//=============================================================================
//
//=======================================
void TUsartPort0::RX_Ready(void)
{ 

}
//=============================================================================
//
//=======================================
 void TUsartPort0::RX_Disable(void)
{
  if(DATA_Rx!=NULL)
  {DATA_Rx->clear();}
}
//=============================================================================
//
//=======================================
void TUsartPort0::TX_Ready(void)
{

}
//*****************************************************************************
//*** Класс TUsartPort3
//*****************************************************************************
//=============================================================================
// Constructor
//=======================================
TUsartPort3::TUsartPort3(Word IntBufSize)
{ 
  DATA_Rx = new TFifo<Byte>(IntBufSize);
}
//=============================================================================
// Destructor
//=======================================
TUsartPort3::~TUsartPort3()
{
  delete DATA_Rx;
}
//=============================================================================
//
//=======================================
  
void TUsartPort3::RX_Ready(void)
{ 
//  USART3->CR1  &= ~USART_CR1_RXNEIE;
//  TTimer::wait_1ms(50);//RxTx_SET  
  GPIOB->BSRR = GPIO_PIN_0; 
//  TTimer::wait_1ms(10);


}
//=============================================================================
//
//=======================================
 void TUsartPort3::RX_Disable(void)
{
  
  if(DATA_Rx!=NULL)
  {DATA_Rx->clear();}
}
//=============================================================================
//
//=======================================
void TUsartPort3::TX_Ready(void)
{
//  TTimer::wait_1ms(1);
  GPIOB->BRR = GPIO_PIN_0;                                  //RxTx_RESET
//  TTimer::wait_1ms(300);
//  USART3->CR1  |=  USART_CR1_RXNEIE;

}
//*****************************************************************************
//*****************************************************************************