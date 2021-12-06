//=============================================================================
#include <AdvClasses.h>

//*****************************************************************************
//*** Класс TUart0ComPort //232
//*****************************************************************************
//=============================================================================
// Constructor
//=======================================


extern class TCmd                 BaseCmd;
extern class TDateTime               SystemTime;

//*****************************************************************************
//*** Класс TComPort0 
//*****************************************************************************
//=============================================================================
// Constructor
//=======================================
TComPort0::TComPort0(Word IntBufSize)
{ 
  DATA_Rx = new TFifo<Byte>(IntBufSize);
}
//=============================================================================
// Destructor
//=======================================
TComPort0::~TComPort0()
{
  delete DATA_Rx;
}
//=============================================================================
//
//=======================================
void TComPort0::RX_Ready(void)
{ 

}
//=============================================================================
//
//=======================================
 void TComPort0::RX_Disable(void)
{
  if(DATA_Rx!=NULL)
  {DATA_Rx->clear();}
}
//=============================================================================
//
//=======================================
void TComPort0::TX_Ready(void)
{

}
//*****************************************************************************
//*** Класс  
//*****************************************************************************
//=============================================================================
// Constructor
//=======================================
TCmd::TCmd()
{ 
  this->Fase = 0;
    SetFase(RX_CMD);
}
//=============================================================================
// Destructor
//=======================================
TCmd::~TCmd()
{
}
//=============================================================================
// 
//=======================================
Byte TCmd::GetFase(void)
{
  return Fase;
}
//=============================================================================
// 
//=======================================
void TCmd::SetFase(Byte fase)
{
  Fase = fase;
}
//=============================================================================
// 
//=======================================
TCmd* TCmd::CMD_Recv(void* p1,void* p2)
{
 TComPort *Port         = (TComPort *)p1;
 TBuffer  *Buffer       = (TBuffer*)p2;

 
  Buffer->DataLen = Port->RecvTo(Buffer->BUF, Buffer->MaxSize, ETX );
  if( Buffer->DataLen > 0)
  { 
    Port->ClrStxFlag(); 
    SetFase(EXE_CMD);
  }
      
  return &BaseCmd;
}
//=============================================================================
// 
//=======================================
TCmd* TCmd::CMD_Execute(void* p1,void* p2)
{
  TComPort         *Port  =   (TComPort*)p1;
  TBuffer          *BufferB  = (TBuffer*)p2;

   BufferB->BUF[BufferB->DataLen] = 0;
   TCmd* CmdPtr = ParsingAnswer(BufferB);
   if(CmdPtr == NULL)
   { 
       Port->ClrStxFlag();           
       SetFase(RX_CMD);
       CmdPtr = &BaseCmd;
    }
    return CmdPtr;
}
//=============================================================================
// 
//=======================================
TCmd* TCmd::CMD_Send(void* p1,void* p2)
{
 TComPort *Port         = (TComPort *)p1;
 TBuffer *Buffer        = (TBuffer*)p2;

      sWord size = Port->Send(Buffer->BUF, Buffer->DataLen, true); 
      if(size > 0)
      {
        ErrorCNT         = 0;
        Buffer->DataLen  = 0;
        SetFase(RX_CMD);
      }
      else if(size == 0)
      {//timeout
        ErrorCNT         = 0;
        Buffer->DataLen  = 0;
        SetFase(RX_CMD);
      }
  return &BaseCmd;
}
//=============================================================================
// 
//=======================================
TCmd* TCmd::ParsingAnswer( TBuffer *Buffer )
{
  CLEAR_WDT;
  Data_Params const *ptr      = TObject::PtrToConstants;
  TCmd *returnValue   = &BaseCmd;

  char stx = STX;
  char etx = ETX;
  int start = TBuffer::find_first_of(Buffer, stx);//ParsingString.find_first_of(stx);
  int end   = TBuffer::find_first_of(Buffer, etx);//ParsingString.find_first_of(etx);
  
  CLEAR_WDT;
  //if(start >=0 && end > 0 && (end-start) > 1)
  if(start >=0 && end > 0 && start < end)
  {//ok
    Buffer->DataLen = Buffer->DataLen-2;
    int index = TBuffer::find_last_of(Buffer, ',');
    if(index < 0 )
    {
        TCmd::LastCMD = "";
        returnValue = NULL;
        return returnValue;
    }
    index++;
    string CRC_SUM    = string( (char*)&Buffer->BUF[index], (Buffer->DataLen-index)  );
    Word   RecvCRC    = TBuffer::HexToWord( (Byte*)CRC_SUM.c_str() );
    Word   CalcCRC    = TBuffer::Crc(&Buffer->BUF[start], (index-start));
//    Byte  *CRC_SUM        = &Buffer->BUF[index];
//    Word   RecvCRC    = TBuffer::HexToWord( CRC_SUM );
//    Word   CalcCRC    = TBuffer::Crc(&Buffer->BUF[start], (index-start));
    
    char const *tmpptr = CRC_SUM.c_str();
    while( *tmpptr <' ' ){CalcCRC+=*tmpptr++;}
    CLEAR_WDT;
    if(RecvCRC!=CalcCRC)
    {
        TCmd::LastCMD = "";
        returnValue = NULL;
        return returnValue;
    }
    Buffer->DataLen = index;
    CLEAR_WDT;
    string TitleParsingString = string( (char*)&Buffer->BUF[start], (Buffer->DataLen-start) );
    int len = 0;
    while(len < TitleParsingString.size())
    { //удалил все управляющие символы
      CLEAR_WDT;
      if(TitleParsingString[len] >= ' ')
      { len++;}
      else
      { TitleParsingString.erase(len,1); }
    }
    vector<string> AnswerList;
    TBuffer::Split(TitleParsingString, ",", AnswerList,8);
    TitleParsingString.clear();
    int size = AnswerList.size();
    if(  size >= 5 )
    {
          string PASSWORD       = AnswerList[0];
          string ID             = AnswerList[1];//
          TCmd::Addr            = ID.substr(7,2);
          ID                    = ID.substr(0,7);
          string date           = AnswerList[2];
          string time           = AnswerList[3];
          TCmd::LastCMD         = AnswerList[4];
          
          string pass = string((char const*)ptr->PassWord);
          if( PASSWORD != pass )
          {
            TCmd::LastCMD = "";
            returnValue = NULL;
            return returnValue;           
          }
          else if( atoi(ID.c_str()) != atoi((char const*)ptr->DevAddress) )
          {
            TCmd::LastCMD = "";
            returnValue = NULL;
            return returnValue;  
          }
          else
          {//ok
            TRtcTime    DateTime = SystemTime.GetTime();
            TObject::SetDataTime(SystemTime,date,time);
            returnValue   = GetCMDbyKey(TCmd::LastCMD);
            if(returnValue == NULL)
            {
              TCmd::LastCMD = "";
              returnValue = NULL;
              return returnValue;
            }
            else
            {
              CLEAR_WDT;
              AnswerList.erase(AnswerList.begin(),AnswerList.begin()+5);
              int size = AnswerList.size();
              TCmd::LastData.clear();
              TCmd::LastData = AnswerList;
              AnswerList.clear();
              LastErrorIndex = OK_IND;
              return returnValue;
            }
          }
    }
    else
    {
        Buffer->DataLen  = 0;
        TCmd::LastCMD = "";
        returnValue = NULL;
        return returnValue;
    }
  }
  else
  {
      SetFase(RX_CMD);
  }
  return returnValue;
}
//=============================================================================
// 
//=======================================
string TCmd::CreateCMD(string CMD,string INDICATION, vector<string> Equpments, TRtcTime *DateTime)
{
  string              RetAnswer;
  string              Tmp;
  char                setx[2] = {STX,ETX};
  Data_Params const   *ptr = TObject::PtrToConstants;
  TRtcTime            Time = SystemTime.GetTime();
  char const*tmpptr = RetAnswer.c_str();
  
  if(DateTime != NULL){ Time = *DateTime;}
  
  CLEAR_WDT;
  RetAnswer.push_back(STX);
  RetAnswer.push_back(',');
  RetAnswer.append( (const char*)ptr->PassWord );
  RetAnswer.push_back(',');  
  RetAnswer.append( (const char*)ptr->DevAddress );
  RetAnswer.push_back(',');
  RetAnswer.append( TBuffer::WordToString(Time.Day) );
  RetAnswer.push_back('.');
  RetAnswer.append( TBuffer::WordToString(Time.Month) );
  RetAnswer.push_back('.');
  RetAnswer.append( TBuffer::WordToString(Time.Year) );
  RetAnswer.push_back(',');
  RetAnswer.append( TBuffer::WordToString(Time.Hour) );
  RetAnswer.push_back(':');  
  RetAnswer.append( TBuffer::WordToString(Time.Minute) );
  RetAnswer.push_back(':');  
  RetAnswer.append( TBuffer::WordToString(Time.Second) );
  RetAnswer.push_back(',');  
  RetAnswer.append( CMD );
  RetAnswer.push_back(','); 
  RetAnswer.append( INDICATION ); 
  RetAnswer.push_back(','); 
  
  if(!Equpments.empty())
  {
    RetAnswer.push_back('\r');     
    RetAnswer.push_back('\n');       
    int sz = Equpments.size();
      for(int i=0; i < sz; i++)
      {
        CLEAR_WDT;
        RetAnswer.push_back('>');         
        RetAnswer.push_back('>');
        RetAnswer.push_back(',');
        string tmpstr =  Equpments[i];
        RetAnswer.append( tmpstr );
        RetAnswer.push_back(',');         
        RetAnswer.push_back('\r');     
        RetAnswer.push_back('\n');             
      }
      RetAnswer.append( ">>");
      RetAnswer.push_back(',');       
  }
  RetAnswer.push_back('\r'); 
  RetAnswer.push_back('\n');   
  Word CRC_SUM = TBuffer::Crc((Byte*)RetAnswer.c_str(), RetAnswer.size());
  Byte Buf[5] = {0,0,0,0,0};
  TBuffer::WordToHex(Buf, CRC_SUM);
  RetAnswer+=string((char*)Buf)+","+string(&setx[1],1); 
  return RetAnswer;
}
//=============================================================================
// 
//=======================================
void  TCmd::AddToCMD_Messages(TCmd* msg)
{
  if( msg !=NULL)
  CMD_Messages.push_back(msg);
}
//=============================================================================
TCmd* TCmd::GetCMDbyKey(string key)
{
  const char* Buf = key.c_str();
  while(*Buf <' ')
  {Buf++;}
  key = Buf; 
  TCmd* ReturnMsg = NULL;
  for(int i = 0; i < CMD_Messages.size(); i++)
  {
    CLEAR_WDT;
    TCmd* curr = CMD_Messages[i];
    if(curr->CmdText == key)
    {
      ReturnMsg = CMD_Messages[i];
      break;
    }
  }
  return ReturnMsg;
}
//=============================================================================
//
//=======================================
sWord TCmd::FindEtPtStrings(const vector<string> &InData, vector<TEt*> &Equpments)
{
    return Equpments.size();                        
}
//=============================================================================
// 
//=======================================
vector<TEt>  TCmd::GetTypes(vector<string> Data)
{
 string         DataString;
 vector<string> PList;
 vector<TEt>    ReturnValue;
 vector<string> TmpList; 
 int size = Data.size();
    return ReturnValue;
}
//*****************************************************************************
//*** 
//*****************************************************************************
TCmd* TCmdErrorCMD::CMD_Execute(void* p1,void* p2)
{
 TBuffer   *BufferModem   = (TBuffer*)p2;

 string AnswerState     = string((char*)ERROR_MASS[LastErrorIndex]);

  TCmd::LastData.clear();    
  string ErrorMsg      = CreateCMD(LastCMD, AnswerState,TCmd::LastData, NULL);
  BufferModem->DataLen = TBuffer::CopyString(BufferModem->BUF, (Byte*)ErrorMsg.c_str());
  ErrorMsg.clear();
  SetFase(TX_CMD);
  return &BaseCmd;
}
//*****************************************************************************
//*** Класс TCmdUB 
//*****************************************************************************
TCmd* TCmdUB::CMD_Execute(void* p1,void* p2)
{
 TBuffer *Buffer     = (TBuffer*)p2;
 Boot_Params   BootParams   = *TObject::PtrToBootConstants; //взяли существующие значения
 //Word          *Ptr         = &BootParams.AppAdrStart;
  CLEAR_WDT;

      Word LastDataSize = TCmd::LastData.size();
      if( LastDataSize == 3 )
      {
        //BootParams.AppAdrStart    = TBuffer::HexToDWord( (Byte*)TCmd::LastData[0].c_str() );
        BootParams.AppAdrStart    = TBuffer::HexToDWord_( (Byte*)TCmd::LastData[0].c_str() );
        BootParams.Segment_Count  = TBuffer::HexToByte( (Byte*)TCmd::LastData[1].c_str() );
        BootParams.AppCRC         = TBuffer::HexToWord( (Byte*)TCmd::LastData[2].c_str() );
        BootParams.AppCurrAdr     = BootParams.AppAdrStart;
        if(TObject::Save_ParamsInInfoA(&BootParams) == true)
        {
            //GlobalCRC             = 0;
            //GlobalAddr            = 0;
        }
        else
        { LastErrorIndex  = UNKNOWN_ER_IND;}
      }
      else
      { LastErrorIndex  = UNKNOWN_ER_IND;}

      
  TCmd::LastData.clear();
  string AnswerState     = string((char*)ERROR_MASS[LastErrorIndex]);
  string Msg             = CreateCMD(LastCMD, AnswerState, TCmd::LastData, NULL);
  Buffer->DataLen        = TBuffer::CopyString(Buffer->BUF, (Byte*)Msg.c_str());
  Msg.clear();
  SetFase(TX_CMD);
  return &BaseCmd;
}
//=============================================================================
// 
//=======================================
TCmd* TCmdUD::CMD_Execute(void* p1,void* p2)
{
 TBuffer *Buffer     = (TBuffer*)p2;
 DWord         NewPageAddr = 0;
 Word          NewPageLen  = 0;
 Byte         *NewPage;
 Word LastDataSize = 0; 
 string PA = "";
 
 
        LastDataSize = TCmd::LastData.size();
        if( LastDataSize == 2 )
        {
            PA = TCmd::LastData[0];
            NewPageAddr = TBuffer::HexToWord((Byte*)PA.c_str());
            NewPageLen  = TCmd::LastData[1].size();
            TBuffer::CopyString(Buffer->BUF,(Byte*)TCmd::LastData[1].c_str());
            Buffer->DataLen = 0;
            NewPage = Buffer->BUF;
            NewPageAddr = NewPageAddr + 0x08000000;
            TObject::WriteHexBufToPage( (Byte const*)NewPageAddr, NewPage, NewPageLen);  
            Boot_Params   BootParams    = *TObject::PtrToBootConstants; //взяли существующие значения
            BootParams.AppCurrAdr       = NewPageAddr;
            TObject::Save_ParamsInInfoA(  &BootParams );
            
        }
        else
        { LastErrorIndex = UNKNOWN_ER_IND;}
  
  
  for(int i = 0; i< LastDataSize; i++)
  {
    CLEAR_WDT;
    TCmd::LastData[i].clear();
  }
  TCmd::LastData.clear();
  if(PA.size() > 0)
  { TCmd::LastData.push_back( PA );}
  string AnswerState     = string((char*)ERROR_MASS[LastErrorIndex]);
  string Msg             = CreateCMD(LastCMD, AnswerState, TCmd::LastData, NULL);
  Buffer->DataLen        = TBuffer::CopyString(Buffer->BUF, (Byte*)Msg.c_str());
  TCmd::LastData.clear();
  Msg.clear();
  SetFase(TX_CMD);
  return &BaseCmd;
}
//=============================================================================
// 
//=======================================
#pragma optimize=z none
TCmd* TCmdUE::CMD_Execute(void* p1,void* p2)
{ 
 TBuffer *Buffer     = (TBuffer*)p2;

       Word        AppCRC     =  TObject::PtrToBootConstants->AppCRC;
       DWord       StartAddr  =  TObject::PtrToBootConstants->AppAdrStart;
       Word        BlockCNT   =  TObject::PtrToBootConstants->Segment_Count;
       Word        CalcCRC    =  TBuffer::AppCrc(StartAddr, BlockCNT);
       
        if( CalcCRC == AppCRC)
        {
          Boot_Params   BootParams    = *TObject::PtrToBootConstants; //взяли существующие значения
          BootParams.BootStatus       = 1;
          BootParams.AppCurrAdr       = BootParams.AppAdrStart;
          TObject::Save_ParamsInInfoA(  &BootParams );
          TCmd::ResetFlg           = true;  
        }
        else
        { LastErrorIndex  = APPL_CRC_ERROR_IND;}

  TCmd::LastData.clear();
  string AnswerState     = string((char*)ERROR_MASS[LastErrorIndex]);
  string Msg             = CreateCMD(LastCMD, AnswerState, TCmd::LastData, NULL);
  Buffer->DataLen        = TBuffer::CopyString(Buffer->BUF, (Byte*)Msg.c_str());
  Msg.clear();
  SetFase(TX_CMD);
  return &BaseCmd;
}
//=============================================================================
// 
//=======================================
TCmd* TCmdGV::CMD_Execute(void* p1,void* p2)
{
 TBuffer *Buffer     = (TBuffer*)p2;
 string AnswerVer;
 
  TCmd::LastData.clear();
  if( atoi(TCmd::Addr.c_str()) == 0 )
  {
    Boot_Params   BootParams   = *TObject::PtrToBootConstants; //взяли существующие значения
    AnswerVer="0,1,>,100,1,"+TBuffer::IntToString(BootParams.CurVersion,1);
  }
  else
  { 
    LastErrorIndex = UNKNOWN_ER_IND;
  }
  TCmd::LastData.push_back(AnswerVer);
  string AnswerState     = string((char*)ERROR_MASS[LastErrorIndex]);
  
  string Msg             = CreateCMD(LastCMD, AnswerState, TCmd::LastData, NULL);
  Buffer->DataLen        = TBuffer::CopyString(Buffer->BUF, (Byte*)Msg.c_str());
  Msg.clear();
  SetFase(TX_CMD);
  return &BaseCmd;
}
//=============================================================================
// 
//=======================================
TCmd* TCmdGD::CMD_Execute(void* p1,void* p2)
{
 TBuffer *Buffer     = (TBuffer*)p2;
 string Answer;
  
  TCmd::LastData.clear();

    Boot_Params   BootParams   = *TObject::PtrToBootConstants; //взяли существующие значения
    Answer= TBuffer::WordToHex(BootParams.AppAdrStart)+','+
                  TBuffer::WordToHex(BootParams.Segment_Count)+','+
                    TBuffer::WordToHex(BootParams.AppCRC)+','+
                       TBuffer::DWordToHex(BootParams.AppCurrAdr);

    
  TCmd::LastData.push_back(Answer);
  string AnswerState     = "BL";
  string Msg             = CreateCMD(LastCMD, AnswerState, TCmd::LastData, NULL);
  Buffer->DataLen        = TBuffer::CopyString(Buffer->BUF, (Byte*)Msg.c_str());
  Msg.clear();
  SetFase(TX_CMD);
  return &BaseCmd;
}

//======= end =================================================================