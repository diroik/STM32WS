//=============================================================================

#include <AdvClasses.h>
#include <Classes.h>
#include <PowerMetr.h>


extern class TPowerMetrManager      *PowerMetrManager;
//*****************************************************************************
//*** Класс TCom0 
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
   //P3DIR |= BIT2;
   //P3OUT &= ~BIT2;
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
   //P3DIR |= BIT2;
   //P3OUT |= BIT2;
}
//*****************************************************************************
//*** Класс TComPort1 
//*****************************************************************************
//=============================================================================
// Constructor
//=======================================
TComPort1::TComPort1(Word IntBufSize)//:CRC(0)
{ 
  DATA_Rx = new TFifo<Byte>(IntBufSize);
}
//=============================================================================
// Destructor
//=======================================
TComPort1::~TComPort1()
{
}
//=============================================================================
//
//=======================================
void TComPort1::RX_Ready(void)
{ 
}
//=============================================================================
//
//=======================================
void TComPort1::RX_Disable(void)
{
  if(DATA_Rx!=NULL)
  {DATA_Rx->clear();}
}
//=============================================================================
//
//=======================================
void TComPort1::TX_Ready(void)
{
}   
//*****************************************************************************
//*** Класс TComPort2
//*****************************************************************************
//=============================================================================
// Constructor
//=======================================
TComPort2::TComPort2(Word IntBufSize)//:CRC(0)
{ 
  DATA_Rx = new TFifo<Byte>(IntBufSize);
}
//=============================================================================
// Destructor
//=======================================
TComPort2::~TComPort2()
{
}
//=============================================================================
//
//=======================================
void TComPort2::RX_Ready(void)
{ 
   //P10DIR |= BIT3;
   //P10OUT &= ~BIT3;
}
//=============================================================================
//
//=======================================
void TComPort2::RX_Disable(void)
{
  if(DATA_Rx!=NULL)
  {DATA_Rx->clear();}
}
//=============================================================================
//
//=======================================
void TComPort2::TX_Ready(void)
{
   //P10DIR |= BIT3;
   //P10OUT |= BIT3;
}   
//*****************************************************************************
//*** Класс TComPort3
//*****************************************************************************
//=============================================================================
// Constructor
//=======================================
TComPort3::TComPort3(Word IntBufSize)//:CRC(0)
{ 
  DATA_Rx = new TFifo<Byte>(IntBufSize);
}
//=============================================================================
// Destructor
//=======================================
TComPort3::~TComPort3()
{
}
//=============================================================================
//
//=======================================
void TComPort3::RX_Ready(void)
{ 
   //P10DIR |= BIT3;
   //P10OUT &= ~BIT3;
}
//=============================================================================
//
//=======================================

void TComPort3::RX_Disable(void)
{
  if(DATA_Rx!=NULL)
  {DATA_Rx->clear();}
}
//=============================================================================
//
//=======================================

void TComPort3::TX_Ready(void)
{
   //P10DIR |= BIT3;
   //P10OUT |= BIT3;
}   
//=============================================================================
//*****************************************************************************
//*****************************************************************************
//*****************************************************************************
//*****************************************************************************
//*****************************************************************************
//***TCmd_msg
//*****************************************************************************
//=============================================================================
// Базовая ф-я приема text
//=======================================
TCmd *TCmd_msg::CMD_Recv(void* p1,void* p2)
{
  TComPort      *Port  =   (TComPort*)p1;
  TBuffer       *Buffer =  (TBuffer*)p2;
  TCmd          *ExecutableCMD = this;
  
    Buffer->DataLen = Port->RecvTo( Buffer->BUF, Buffer->MaxSize, ETX );
    if(  Buffer->DataLen > 0 )  
    {
      Port->ClrStxFlag(); 
      SetFase( EXE_CMD );
      ExecutableCMD = BaseCMD;      
    }
    else if(Buffer->DataLen == 0)//timeout
    { SetFase(WAIT_MES);}
    return ExecutableCMD;
}
//=============================================================================
TCmd *TCmd_msg::CMD_Send( void* p1,void* p2 )
{
 TComPort       *Port  =   (TComPort*)p1;
 TBuffer        *Buffer =  (TBuffer*)p2;
 TCmd           *ExecutableCMD = this;
  
  int Len = Port->Send(Buffer->BUF, Buffer->DataLen);
  if( Len > 0 )
  {//если передали все
      this->SetFase( RX_CMD );              
      ExecutableCMD  = BaseCMD; 
      TCmd_msg::_needAnswer = false;
  }
  return ExecutableCMD;
}
//=============================================================================
TCmd *TCmd_msg::GET_Message(void* p1,void* p2)
{

  TCmd        *ExecutableCMD  = this;
  
  SetFase(RX_CMD);
  return ExecutableCMD; 
}
//=============================================================================
TCmd *TCmd_msg::CMD_Execute(void* p1,void* p2)
{
 TComPort         *Port  =   (TComPort*)p1;
 TBuffer          *BufferB  = (TBuffer*)p2;
 BufferB->BUF[BufferB->DataLen] = 0;
 TCmd* CmdPtr = ParsingAnswer(BufferB);
 if(CmdPtr == NULL)
 { 
     Port->ClrStxFlag();           
     SetFase(RX_CMD);
     CmdPtr = BaseCMD;
  }
  return CmdPtr;
}
//=============================================================================
//
//=============================================================================
Word TCmd_msg::CreateCMD(TBuffer *Buffer, string CMD, string INDICATION, Tlist<string> &Data, bool WithQuotes, TRtcTime *DateTime, string Passw, string Addr)
{
  Word index = 0;
  Data_Params const  *ptr = TObject::PtrToConstants;
  TRtcTime            Time = SystemTime.GetTime();
  
  if(DateTime != NULL){ Time = *DateTime;}
  
  CLEAR_WDT;
  Buffer->BUF[index++] = STX;
  Buffer->BUF[index++] = ',';
  if( Passw != NULL && Passw != "")
  { index += TBuffer::CopyString(&Buffer->BUF[index], (Byte*)Passw.c_str());}
  else
  { index += TBuffer::CopyString(&Buffer->BUF[index], (Byte*)ptr->PassWord);}
  
  Buffer->BUF[index++] = ',';
  
  if( Addr != NULL && Addr != "")
  { index += TBuffer::CopyString(&Buffer->BUF[index], (Byte*)Addr.c_str());}
  else
  { index += TBuffer::CopyString(&Buffer->BUF[index], (Byte*)ptr->DevAddress);}
  
  Buffer->BUF[index++] = ',';
  
  index += TBuffer::CopyString(&Buffer->BUF[index], (Byte*)TBuffer::WordToString(Time.Day,2).c_str());
  Buffer->BUF[index++] = '.';
  index += TBuffer::CopyString(&Buffer->BUF[index], (Byte*)TBuffer::WordToString(Time.Month,2).c_str());
  Buffer->BUF[index++] = '.';
  index += TBuffer::CopyString(&Buffer->BUF[index], (Byte*)TBuffer::WordToString(Time.Year).c_str());
  Buffer->BUF[index++] = ',';
  index += TBuffer::CopyString(&Buffer->BUF[index], (Byte*)TBuffer::WordToString(Time.Hour,2).c_str());
  Buffer->BUF[index++] = ':';
  index += TBuffer::CopyString(&Buffer->BUF[index], (Byte*)TBuffer::WordToString(Time.Minute,2).c_str());
  Buffer->BUF[index++] = ':';
  index += TBuffer::CopyString(&Buffer->BUF[index], (Byte*)TBuffer::WordToString(Time.Second,2).c_str());
  Buffer->BUF[index++] = ',';
  index += TBuffer::CopyString( &Buffer->BUF[index], (Byte*)CMD.c_str() );
  Buffer->BUF[index++] = ',';
  
  if( INDICATION != NULL && INDICATION !="" )
  {
      index += TBuffer::CopyString( &Buffer->BUF[index], (Byte*)INDICATION.c_str() );
      Buffer->BUF[index++] = ',';
  }
  if(!Data.empty())
  {
    int sz = Data.size();
      for(int i=0; i < sz; i++)
      {
        CLEAR_WDT;
        if(WithQuotes == true)
        {
          //Buffer->BUF[index++] = '\r';
          //Buffer->BUF[index++] = '\n'; 
          Buffer->BUF[index++] = '>';
          Buffer->BUF[index++] = '>'; 
          Buffer->BUF[index++] = ',';             
        }
        index += TBuffer::CopyString( &Buffer->BUF[index], (Byte*)Data[i].c_str() );
        Data[i].clear();
        if(Buffer->BUF[index-1] != ',')
        {  Buffer->BUF[index++] = ',';  }    
      }
      if(WithQuotes == true)
      {       
        //Buffer->BUF[index++] = '\r';
        //Buffer->BUF[index++] = '\n';         
        Buffer->BUF[index++] = '>';
        Buffer->BUF[index++] = '>'; 
        Buffer->BUF[index++] = ',';    
      }
  }
  //Buffer->BUF[index++] = '\r';
  //Buffer->BUF[index++] = '\n';   
  Word CRC_SUM = TBuffer::Crc( Buffer->BUF, index);
  Byte Buf[5] = {0,0,0,0,0};
  TBuffer::WordToHex(Buf, CRC_SUM);
  index += TBuffer::CopyString( &Buffer->BUF[index], Buf );
  Buffer->BUF[index++] = ',';    
  Buffer->BUF[index++] = ETX;
  return index;
}


//=============================================================================
TCmd* TCmd_msg::ParsingAnswer( TBuffer *Buffer )
{
CLEAR_WDT;
  TCmd *returnValue             = NULL;
  //string ParsingString          = string((char*)Buffer->BUF,Buffer->DataLen);  
  char stx = STX;
  char etx = ETX;
  int start = TBuffer::find_first_of(Buffer, stx);
  int end   = TBuffer::find_first_of(Buffer, etx);
  
  if(start >=0 && end > 0 && start < end)
  {//ok
    Buffer->DataLen = Buffer->DataLen-2;
    int index = TBuffer::find_last_of(Buffer, ',');
    if(index < 0 )
    {
      this->BaseCMD->LastCMD = "";
      returnValue = NULL;
      return returnValue;
    }
    index++;
    Buffer->BUF[Buffer->DataLen] = 0;
    Byte  *CRC_SUM        = &Buffer->BUF[index];
    Word   RecvCRC    = TBuffer::HexToWord( CRC_SUM );
    Word   CalcCRC    = TBuffer::Crc(&Buffer->BUF[start], (index-start));
    while( *CRC_SUM <' ' ){CalcCRC+=*CRC_SUM++;}
    CLEAR_WDT;
    if(RecvCRC!=CalcCRC)
    {
        this->BaseCMD->LastCMD = "";
        returnValue = NULL;
        return returnValue;
    }
    Buffer->DataLen = index;
    CLEAR_WDT;
    Buffer->DataLen = TBuffer::clear_all_nulls( (char*)Buffer->BUF, Buffer->DataLen );
    ClearLastData();
    TBuffer::Split(Buffer->BUF, Buffer->DataLen, ",", BaseCMD->LastData, 6);//5
    int size = BaseCMD->LastData.size();
    if(  size >= 5 )
    {
          string PASSWORD   = BaseCMD->LastData[0];   
          string ID         = BaseCMD->LastData[1];
          if( ID.size() == 9 )
          {
            BaseCMD->Addr          = ID.substr(7,2);
            ID                     = ID.substr(0,7);          
          }
          else
          { BaseCMD->Addr = "0";}
          string date       = BaseCMD->LastData[2];
          string time       = BaseCMD->LastData[3];
          BaseCMD->LastCMD  = BaseCMD->LastData[4];
          string PS="";
          if(size>=6)
          {  PS         = BaseCMD->LastData[5];}
          
          string pass = string( BaseCMD->PasswordPtr );
          string adr  = ID;
          if(BaseCMD->AddrPtr != NULL) adr = string( BaseCMD->AddrPtr );
          
          Word address = atoi(ID.c_str());  
          if( PASSWORD !=  pass )
          {
            this->BaseCMD->LastCMD = "";
            returnValue = NULL;
            return returnValue;
          }
          else if( address != atoi( adr.c_str() ) )
          {
            this->BaseCMD->LastCMD = "";
            returnValue = NULL;
            return returnValue;
          }
          else
          {//ok
              if(BaseCMD->AddrPtr != NULL)//Присвоение времени!!
              {
                if(PS!="NS")
                {
                  TRtcTime    DateTime = SystemTime.GetTime();
                  TObject::SetDataTime(SystemTime, date, time);
                  if(!SystemTime.GetSynchAct())
                  {
                    SystemTime.SetSynchAct();
                  }            
                }
              }
              BaseCMD->ID = ID;              
              returnValue = GetCMDbyKey( BaseCMD->LastCMD );
              
              if( returnValue == NULL )
              { returnValue = GetCMDbyKey( "UnknownCMD" );}
              if(returnValue == NULL)
              {
                this->ClearLastData();
                return returnValue;
              }
              else
              {
                CLEAR_WDT;
                BaseCMD->LastState = "";
                if(size >= 6)
                { BaseCMD->LastState = BaseCMD->LastData[5]; }
                this->ClearLastData();
                index = TBuffer::find(Buffer, ">>,");
                if(index >= 0)
                {
                  int lenth = Buffer->DataLen - index;
                  if(lenth > 0)
                  { 
                    if(atoi( BaseCMD->Addr.c_str() ) > 0 )//lenth > 1000 && 
                    {
                      if( BaseCMD->AlternateMessages != NULL )
                      {
                        int size = BaseCMD->AlternateMessages->size();
                        for(int i = 0; i < size; i++)
                        {
                          CLEAR_WDT;
                          TCmdMessage *CurrMessage = BaseCMD->AlternateMessages->operator[](i);
                          if(CurrMessage != NULL)
                          {
                            if(CurrMessage->Data.size() > 0)
                            {
                              returnValue = NULL;
                              return returnValue;
                            }
                          }
                        }
                      }
                    }
                    TBuffer::Split(&Buffer->BUF[index], lenth, ">>,", BaseCMD->LastData);
                  }
                }
                this->BaseCMD->LastErrorIndex = OK_IND;
                return returnValue;
              }
          }
    }
    else
    {
        this->ClearLastData();
        this->BaseCMD->LastCMD = "";
        returnValue = NULL;//GetCMDbyKey("ERROR");
        return returnValue;
    }
  }
  else
  {
      SetFase(RX_CMD);
      Buffer->DataLen = 0;
  }
  return returnValue;
}
//=============================================================================
// 
//=======================================
bool TCmd_msg::PushToAlternateMessages(string &Pass, string &Addr, string IND, bool WithQuotes)          
{
  if( BaseCMD->AlternateMessages != NULL )
  {
    if( BaseCMD->AlternateMessages->size() < MAX_MESS_IN_ALTERNATE)
    {
      TCmdMessage *NewMessage = new TCmdMessage();
      NewMessage->CMD         =  BaseCMD->LastCMD;
      NewMessage->Data        =  BaseCMD->LastData;
      NewMessage->Password    =  Pass;
      NewMessage->Addr        =  Addr;
      NewMessage->State       =  IND;
      NewMessage->WithQuotes  = WithQuotes;
      BaseCMD->AlternateMessages->push_front(NewMessage);
      ClearLastData();
      return true;
    }
  }
  ClearLastData();
  return false;
}
//=============================================================================
// 
//=======================================
bool TCmd_msg::PushToMainMessages(string& Addr, string CMD)          
{
  if( BaseCMD->MainMessages != NULL )
  {
    if( BaseCMD->MainMessages->size() < MAX_MESS_IN_MAIN)
    {
      TCmdMessage *NewMessage = new TCmdMessage();
      NewMessage->Password    = string( this->PasswordPtr );
      NewMessage->Addr        =  Addr;
      NewMessage->CMD         = CMD;

      BaseCMD->MainMessages->push_front(NewMessage);
      ClearLastData();
      return true;
    }
  }
  ClearLastData();
  return false;
}
//=============================================================================
// 
//=======================================
void TCmd_msg::ClearAlternateMessages(void)          
{
  if( BaseCMD->AlternateMessages != NULL )
  {
    int size = BaseCMD->AlternateMessages->size();
    for(int i = 0; i < size; i++)
    {
      CLEAR_WDT;
      TCmdMessage *CurrMessage = *BaseCMD->AlternateMessages->front();//operator[](i);
      if(CurrMessage != NULL)
      {
          CurrMessage->Data.clear();
          if(CurrMessage->DateTime != NULL) { delete CurrMessage->DateTime;}
        //  delete CurrMessage;
      }
      BaseCMD->AlternateMessages->pop_front();
    }
    BaseCMD->AlternateMessages->clear();
  }
}
//=============================================================================
// 
//=======================================
void TCmd_msg::ClearMainAndCurrMessages(void)          
{
  if( BaseCMD->MainMessages != NULL )
  {
    TCmdMessage *CurrMessage = GetCurrNeedCmdAnswerMessage();
    if(CurrMessage != NULL)
    {
        CurrMessage->Data.clear();
        if(CurrMessage->DateTime != NULL) { delete CurrMessage->DateTime;}
        delete CurrMessage;
        SetCurrNeedCmdAnswerMessage(NULL);
        BaseCMD->ClearErrorCount();
    }
    for(int i = 0; i < BaseCMD->MainMessages->size(); i++)
    {
      CLEAR_WDT;
      CurrMessage = BaseCMD->MainMessages->operator[](i);
      if(CurrMessage != NULL)
      {
          CurrMessage->Data.clear();
          if(CurrMessage->DateTime != NULL) { delete CurrMessage->DateTime;}
          delete CurrMessage;
      }
    }
    BaseCMD->MainMessages->clear();
  }
}
//=============================================================================
// 
//=======================================
void TCmd_msg::ClearLastData(void)          
{
  BaseCMD->LastData.clear();
}
//=============================================================================
// 
//=======================================
void TCmd_msg::PushBackToLastData(string data)          
{
  BaseCMD->LastData.push_back(data);
}
//=============================================================================
bool TCmd_msg::CMD_CmpCRC(Byte  *RcvCRC, Word &CRC_SUM)
{
  Byte          CalcCRC[] = {0,0,0,0,0};
  
    while(*RcvCRC < 0x20)
    { 
      CLEAR_WDT;
      if(*RcvCRC == 0) return false;
      CRC_SUM +=*RcvCRC++;
    }
    TBuffer::WordToHex(CalcCRC,CRC_SUM);
    return TBuffer::StrCmp(RcvCRC,CalcCRC);
}
//=============================================================================
TCmd_msg* TCmd_msg::GetCMDbyKey(string key)
{
  const char* Buf = key.c_str();
  while(*Buf <' ')
  {Buf++;}
  key = Buf; 
  TCmd_msg* ReturnMsg = NULL;
  foreach(TCmd_msg* curr, BaseCMD->CMD_Messages)
  {
    if(curr->CmdText == key)
    {
      ReturnMsg = curr;
      break;
    }
  }
  return ReturnMsg;
}
//=============================================================================
void  TCmd_msg::AddToCMD_Messages(TCmd_msg* msg)
{
  BaseCMD->CMD_Messages.push_back(msg);
}
//=============================================================================
Byte TCmd_msg::GetFase(void)
{
  return this->BaseCMD->Fase;
}
//============================================================================= 
void TCmd_msg::SetFase(Byte NewFase)
{
  this->BaseCMD->Fase = NewFase;
}
//=============================================================================
//=============================================================================
// 
//=======================================
bool TCmd_msg::NeedReset(void)
{
  return this->BaseCMD->_needReset;
}
//=============================================================================
// 
//=======================================
void TCmd_msg::SetNeedReset(void)
{
  this->BaseCMD->_needReset = true;
}
//=============================================================================
// 
//=======================================
bool TCmd_msg::NeedAnswer(void)
{
  return this->BaseCMD->_needAnswer;
}
//=============================================================================
// 
//=======================================
void TCmd_msg::SetNeedAnswer(void)
{
  this->BaseCMD->_needAnswer = true;
}
//=============================================================================
// 
//=======================================
void TCmd_msg::SendRESET(void* p1,void* p2)
{
 TComPort   *Port             = (TComPort*)p1; 
 TBuffer    *Buffer           = (TBuffer*)p2;
 string     AnswerState       = string((char*)ERROR_MASS[OK_IND]);

  Tlist<string>          Data;
  //string Msg              = CreateCMD("RESET", AnswerState, Data);
  //Buffer->DataLen         = TBuffer::CopyString(Buffer->BUF, (Byte*)Msg.c_str());
  Buffer->DataLen = CreateCMD(Buffer, "RESET", AnswerState, Data);
  Port->ClrStxFlag();
  SetFase(TX_CMD);
  this->ClearLastData();
  return;
}
//=============================================================================
// 
//=======================================
Tlist<TEt>  TCmd_msg::GetTypes(Tlist<string> &Data)
{
 string         DataString;
 Tlist<string> PList;
 Tlist<TEt>    ReturnValue;
 Tlist<string> TmpList; 
 int size = Data.size();

    for(int i = 0; i< size;i++)
    {
     CLEAR_WDT;
      DataString = Data[i];
      PList.clear();
      TBuffer::Split(DataString, ",>,", PList);
      
      TmpList.clear();
      
      if(PList.size() == 0)
      { return ReturnValue;} 
      string Pstring = PList[0];
      
      TBuffer::Split(Pstring, ",", TmpList);
      int tmpsize = TmpList.size();
      if( tmpsize!= 2)
      { return ReturnValue;}      
      TEt CurrEt( atoi(TmpList[0].c_str()), atoi(TmpList[1].c_str()) );
      int SubSize = PList.size();
      for(int j = 1; j < SubSize; j++)
      {
        TmpList.clear();
        DataString = PList[j];
        TBuffer::Split(DataString, ",", TmpList);
        int s = TmpList.size();
        if(s < 2)
        { return ReturnValue;}
        TPt CurrPt( atoi(TmpList[0].c_str()), atoi(TmpList[1].c_str()) );//
        
        for(int k = 2; k < s; k++)//пишем сами данные
        {CurrPt.Value.push_back( TmpList[k] );}
        CurrEt.Ptypes.push_back( CurrPt );
      }
      ReturnValue.push_back( CurrEt );              
    }
    return ReturnValue;
}
//=============================================================================
// 
//=======================================
void  TCmd_msg::GetTypes(Tlist<string> &Data, Tlist<TEt> &ReturnValue)
{
 string         DataString;
 Tlist<string> PList;
 //Tlist<TEt>    ReturnValue;
 Tlist<string> TmpList; 
 int size = Data.size();

    for(int i = 0; i< size;i++)
    {
     CLEAR_WDT;
      DataString = Data[i];
      PList.clear();
      TBuffer::Split(DataString, ",>,", PList);
      
      TmpList.clear();
      
      if(PList.size() == 0)
      { return;} 
      string Pstring = PList[0];
      
      TBuffer::Split(Pstring, ",", TmpList);
      int tmpsize = TmpList.size();
      if( tmpsize!= 2)
      { return;}      
      //TEt CurrEt( atoi(TmpList[0].c_str()), atoi(TmpList[1].c_str()) );
      ReturnValue.push_back( TEt( atoi(TmpList[0].c_str()), atoi(TmpList[1].c_str()) ) );
      TEt *CurrEt = ReturnValue.back();
      int SubSize = PList.size();
      for(int j = 1; j < SubSize; j++)
      {
        TmpList.clear();
        DataString = PList[j];
        TBuffer::Split(DataString, ",", TmpList);
        int s = TmpList.size();
        if(s < 2)
        { return;}
        TPt CurrPt( atoi(TmpList[0].c_str()), atoi(TmpList[1].c_str()) );//
        
        for(int k = 2; k < s; k++)//пишем сами данные
        {CurrPt.Value.push_back( TmpList[k] );}
        CurrEt->Ptypes.push_back( CurrPt );
      }
      //ReturnValue.push_back( CurrEt );              
    }
    return;
}
//=============================================================================
TCmdMessage *TCmd_msg::GetCurrNeedCmdAnswerMessage(void)
{
  return this->BaseCMD->_currNeedCmdAnswerMessage;
}
//============================================================================= 
void TCmd_msg::SetCurrNeedCmdAnswerMessage(TCmdMessage *NewMess)
{
  this->BaseCMD->_currNeedCmdAnswerMessage = NewMess;
}
//============================================================================= 
void TCmd_msg::ClearErrorCount(void)
{
  this->BaseCMD->_errorCount = 0;
}
//=============================================================================
//*****************************************************************************
//***TCmd_ERROR
//*****************************************************************************
//=============================================================================
// 
//=======================================
TCmd* TCmd_ERROR::CMD_Execute(void* p1,void* p2)
{
 TBuffer        *Buffer   = (TBuffer*)p2;
 string AnswerState       = string((char*)ERROR_MASS[this->BaseCMD->LastErrorIndex]);
 TCmd *ExecutableCMD      = BaseCMD;

  ClearLastData();
  if( this->BaseCMD->LastCMD == "")
    this->BaseCMD->LastCMD = "ERROR";
  Buffer->DataLen = CreateCMD(Buffer, BaseCMD->LastCMD, AnswerState, BaseCMD->LastData);
  SetFase(TX_CMD);
  return ExecutableCMD;
}   
//*****************************************************************************
//***TCmd_UnknownCMD
//*****************************************************************************
//=============================================================================
// 
//=======================================
TCmd* TCmd_UnknownCMD::CMD_Execute(void* p1,void* p2)
{ 
 TBuffer        *Buffer   = (TBuffer*)p2;
 //Data_Params const   *ptr = TObject::PtrToConstants; 
 TCmd *ExecutableCMD      = BaseCMD;
 
  TCmdMessage *CurrMessage = GetCurrNeedCmdAnswerMessage();
  if(CurrMessage == NULL)
  {
      if(BaseCMD->LastCMD != "CfData")
      {
          ClearLastData();
          BaseCMD->LastErrorIndex = CMD_ER_IND;
          if( BaseCMD->LastCMD == "")
            BaseCMD->LastCMD = "ERROR";
          Buffer->DataLen = CreateCMD(Buffer, BaseCMD->LastCMD, string((char*)ERROR_MASS[this->BaseCMD->LastErrorIndex]), BaseCMD->LastData);
          SetFase(TX_CMD);
          return ExecutableCMD;
        /*
        BaseCMD->LastErrorIndex = CMD_ER_IND;
        ExecutableCMD = GetCMDbyKey("ERROR");
        return ExecutableCMD;*/
      }
  }
  /*
  else
  {
      if(SystemTime.GetSynchAct())
      {//Время синхронизировано....
        string neededCmd = "Cf"+CurrMessage->CMD;
        if( BaseCMD->LastCMD == neededCmd)//все ок - ответ есть
        {
          CurrMessage->Data.clear();
          if(CurrMessage->DateTime != NULL) { delete CurrMessage->DateTime;}
          delete CurrMessage;
          SetCurrNeedCmdAnswerMessage(NULL);
          BaseCMD->ClearErrorCount();
        }
      }
      else
      {//
          BaseCMD->ClearErrorCount();        
      }
  }
  */
  SetFase(RX_CMD);
  return ExecutableCMD;
}
//*****************************************************************************
//***TCmd_Data
//*****************************************************************************
//=============================================================================
// 
//=======================================
TCmd* TCmd_Data::CMD_Execute(void* p1,void* p2)
{ 
 //Data_Params const   *ptr = TObject::PtrToConstants; 
 TBuffer *Buffer          = (TBuffer*)p2;
 string AnswerState       = string((char*)ERROR_MASS[OK_IND]);
 string CMD               = "Data";
 TCmd *ExecutableCMD      = BaseCMD;
 
    this->ClearLastData();

    if( !BaseCMD->MainMessages->empty() )
    {
      TCmdMessage *CurrMessage = *BaseCMD->MainMessages->front();  
      BaseCMD->MainMessages->pop_front();
      Buffer->DataLen = CreateCMD(Buffer, 
                                  CurrMessage->CMD, 
                                  CurrMessage->State, 
                                  CurrMessage->Data, 
                                  CurrMessage->DateTime
                                    );   
       CurrMessage->Data.clear();
       if(CurrMessage->DateTime != NULL) { delete CurrMessage->DateTime;}
       delete CurrMessage;   
    }
    else
    {
      Buffer->DataLen           = CreateCMD(Buffer, BaseCMD->LastCMD, AnswerState, BaseCMD->LastData);      
    }  
    this->ClearLastData();
    //Buffer->DataLen = CreateCMD(Buffer, BaseCMD->LastCMD, AnswerState, BaseCMD->LastData);
    SetFase(TX_CMD);

  return ExecutableCMD;
}
//*****************************************************************************
//***TCmd_GetData
//*****************************************************************************
//=============================================================================
// 
//=======================================
TCmd* TCmd_GetData::CMD_Execute(void* p1,void* p2)
{ 
 //Data_Params const   *ptr = TObject::PtrToConstants; 
 TBuffer *Buffer          = (TBuffer*)p2;
 BaseCMD->LastErrorIndex  = OK_IND;
 TCmd *ExecutableCMD      = BaseCMD;
 
    Tlist<TEt> Equipments =  GetTypes( BaseCMD->LastData);
    int size = Equipments.size();
    this->ClearLastData();
    if(size == 0)
       if(PowerMetrManager !=NULL) 
         PowerMetrManager->FirstInitFlg = true;    

    string AnswerState      = string((char*)ERROR_MASS[this->BaseCMD->LastErrorIndex]);  
    Buffer->DataLen = CreateCMD(Buffer, BaseCMD->LastCMD, AnswerState, BaseCMD->LastData);
    SetFase(TX_CMD);

  return ExecutableCMD;
}
//*****************************************************************************
//***TCmd_GetCalibr
//*****************************************************************************
//=============================================================================
// 
//=======================================
TCmd* TCmd_GetCalibr::CMD_Execute(void* p1,void* p2)
{ 
 TBuffer    *Buffer           = (TBuffer*)p2;
 BaseCMD->LastErrorIndex      = OK_IND;
 TCmd *ExecutableCMD          = BaseCMD;
 TCalibrListConfig const *ptr =TObjectCalibr::CalibrListConfig; 
  
 
    Tlist<TEt> Equipments =  GetTypes( BaseCMD->LastData);
    int size = Equipments.size();
    this->ClearLastData();
    if(size == 0)
    {//Нужно будет убрать столь простой способ задания ET и EN
      
      string Data ="60,2,\r\n";
        for(Byte i=0; i<8; i++)
        {  
          Data+=">,83,"+TBuffer::WordToString(i+1)+","+TBuffer::FloatToString(ptr->CalVolt[i],0,4)+",\r\n";
        }
        if(Data.size() > 10) Data = string(Data.c_str(), Data.size()-3);
      PushBackToLastData(Data);
    }
    else
    {
      if(PowerMetrManager !=NULL)
      {
        string Data ="60,1,\r\n";
        int i = 1;
        foreach(IDetector* curr, PowerMetrManager->PowerMetrs->VoltSensors)
        {
            float val = curr->GetSimpleValue();
            Data+=">,83,"+TBuffer::WordToString(i++)+","+TBuffer::FloatToString(val, 0, 4)+",\r\n";
        }  
        //i=1;
        foreach(IDetector* curr, PowerMetrManager->PowerMetrs->TempSensors)
        {
            float val = curr->GetSimpleValue();
            Data+=">,83,"+TBuffer::WordToString(i++)+","+TBuffer::FloatToString(val, 0, 4)+",\r\n";
        } 
        PushBackToLastData(Data);
      }
    }
    string AnswerState      = string((char*)ERROR_MASS[BaseCMD->LastErrorIndex]);
    Buffer->DataLen = CreateCMD(Buffer, BaseCMD->LastCMD, AnswerState, BaseCMD->LastData);
    this->ClearLastData();
    SetFase(TX_CMD);

  return ExecutableCMD;
}
//*****************************************************************************
//***TCmd_SetCalibr
//*****************************************************************************
//=============================================================================
// 
//=======================================
TCmd* TCmd_SetCalibr::CMD_Execute(void* p1,void* p2)
{ 
 
 TBuffer *Buffer          = (TBuffer*)p2;
 BaseCMD->LastErrorIndex  = OK_IND;
 TCmd *ExecutableCMD      = BaseCMD;
 bool    CalibrFlag       = false;//Необходимость записи калибровок
 TCalibrListConfig CalibrParams = *TObjectCalibr::CalibrListConfig;
 
 
    Tlist<TEt> Equipments;
    GetTypes( BaseCMD->LastData, Equipments);
    int size = Equipments.size();
    this->ClearLastData();
    if(size == 0)
    {
      this->BaseCMD->LastErrorIndex = NO_DATA;
    }
    for(int i = 0; i < size; i++)
    {
      TEt CurrEt = Equipments[i];
      int CurrSize = CurrEt.Ptypes.size();
      for(int j = 0; j < CurrSize; j++)
      {
           TPt CurrPt = CurrEt.Ptypes[j];
           if( CurrEt.EType == 60)
           {  
             switch (CurrPt.PType)
             {
             case 83:
               if(CurrEt.EName == 2)
               {//запись коэффициентов напрямую
                  if(CurrPt.Value.size() == 1)
                  {
                    if( (CurrPt.PName > 0)&(CurrPt.PName <= 8) )
                    {
                      if( PowerMetrManager != NULL && PowerMetrManager->PowerMetrs != NULL )
                      {
                        float koef = atof( (char*)CurrPt.Value[0].c_str() );
                        CalibrParams.CalVolt[CurrPt.PName-1] = koef;
                        CalibrFlag = true;                        
                      }
                    }
                    else
                    { 
                      this->BaseCMD->LastErrorIndex = UNKNOWN_ER_IND;
                      CalibrFlag = false;
                    }
                  }
               }
               break;
             
             case 84:
               if(CurrEt.EName == 2)
               {//запись коэффициентов напрямую
                  if(CurrPt.Value.size() == 1)
                  {
                    if( (CurrPt.PName > 0)&(CurrPt.PName <= 4) )
                    {
                      if( PowerMetrManager != NULL && PowerMetrManager->PowerMetrs != NULL )
                      {
                        float koef = atof( (char*)CurrPt.Value[0].c_str() );
                        CalibrParams.CalVolt[CurrPt.PName+3] = koef;
                        CalibrFlag = true;                        
                      }
                    }
                    else
                    { 
                      this->BaseCMD->LastErrorIndex = UNKNOWN_ER_IND;
                      CalibrFlag = false;
                    }
                  }
               }
               break;
             default:
               break;
             }   
           }
      }
    }
    if( CalibrFlag == true )//all ok - save in memory
    { 
        TObjectCalibr::Save_CalibrConfigInFlash(&CalibrParams);
        this->SetNeedReset();
        this->SetNeedAnswer();
    }
    string AnswerState      = string((char*)ERROR_MASS[this->BaseCMD->LastErrorIndex]);
    Buffer->DataLen = CreateCMD(Buffer, BaseCMD->LastCMD, AnswerState, BaseCMD->LastData);
    SetFase(TX_CMD);
    
  return ExecutableCMD;
}
//*****************************************************************************
//***TCmd_SetCfg
//*****************************************************************************
//=============================================================================
// 
//=======================================
TCmd* TCmd_SetCfg::CMD_Execute(void* p1,void* p2)
{
 //Data_Params const   *ptr = TObject::PtrToConstants; 
 TBuffer *Buffer          = (TBuffer*)p2;
 BaseCMD->LastErrorIndex  = OK_IND;
 TCmd *ExecutableCMD      = BaseCMD;
 bool    flag             = false;//Необходимость записи настроек устройства
 bool    taskFlag         = false;//Необходимость записи настроек процессов
 
 Data_Params      Params        = *TObject::PtrToConstants;
 TTaskListConfig  TaskParams    = *TObjectTask::TaskListConfig;

    Tlist<TEt> Equipments;
    GetTypes( BaseCMD->LastData, Equipments);
    int size = Equipments.size();
    this->ClearLastData();
    if(size == 0)
    {
      this->BaseCMD->LastErrorIndex = NO_DATA;
    }
    for(int i = 0; i < size; i++)
    {
      TEt CurrEt = Equipments[i];
      int CurrSize = CurrEt.Ptypes.size();
      if( CurrEt.EType == 0 || CurrEt.EType == 2)
      {// параметры самого устрйства мониторинга
        for(int j = 0; j < CurrSize; j++)
        {
              TPt CurrPt = CurrEt.Ptypes[j];
              switch(CurrPt.PType)
              {
               case 108://et !!!!!!!!!!!!!!!!!!!!!!!
                if(CurrPt.Value.size() == 1)
                { 
                  Word et = atoi(CurrPt.Value[0].c_str());
                  Params.wReserved[0] = et;
                  flag = true;
                }
                else
                { 
                  BaseCMD->LastErrorIndex = UNKNOWN_ER_IND;
                  flag = false;
                }
                break;
                
               case 109://en !!!!!!!!!!!!!!!!!!!!!!!
                if(CurrPt.Value.size() == 1)
                { 
                  Word en = atoi(CurrPt.Value[0].c_str());
                  Params.wReserved[1] = en;
                  flag = true;
                }
                else
                { 
                  BaseCMD->LastErrorIndex = UNKNOWN_ER_IND;
                  flag = false;
                }
                break;
                
               case 110://pn !!!!!!!!!!!!!!!!!!!!!!!!!
                if(CurrPt.Value.size() == 1)
                {
                  Word pn = atoi(CurrPt.Value[0].c_str());
                  Params.wReserved[2] = pn;
                  flag = true;
                }
                else
                { 
                  this->BaseCMD->LastErrorIndex = UNKNOWN_ER_IND;
                  flag = false;
                }
                break;
                
              case 111://adr
                if(CurrPt.Value.size() == 1)
                {
                  string ID = CurrPt.Value[0];
                  if(ID.size() == 2)
                  {
                    for(int l=0;l<ID.size();l++)
                    { Params.DevAddress[l]        = ID.c_str()[l];}
                    Params.DevAddress[ID.size()]  = 0;
                    flag = true;
                  }
                  else
                  { 
                    this->BaseCMD->LastErrorIndex = UNKNOWN_ER_IND;
                    flag = false;
                  }
                }
                else
                { 
                  this->BaseCMD->LastErrorIndex = UNKNOWN_ER_IND;
                  flag = false;
                }
                break;
                
              case 100:
                if(CurrPt.Value.size() == 1)
                {
                  long    Serial;
                  Serial  = atol( (char*)CurrPt.Value[0].c_str() );                                    
                  Params.Serial = Serial;
                  flag = true;
                }
                else
                { 
                  this->BaseCMD->LastErrorIndex = UNKNOWN_ER_IND;  
                }
                break;
                
              case 112://pass
                if(CurrPt.Value.size() == 1)
                {
                  string PASSWORD = CurrPt.Value[0];
                  if(PASSWORD.size() <= 8)
                  {
                    for(int l=0;l<PASSWORD.size();l++){ Params.PassWord[l]        = PASSWORD.c_str()[l];}
                    flag = true;
                  }
                  else
                  { 
                    this->BaseCMD->LastErrorIndex = UNKNOWN_ER_IND;
                    flag = false;
                  }
                }
                else
                { 
                  this->BaseCMD->LastErrorIndex = UNKNOWN_ER_IND;
                  flag = false;
                }
                break;
                
              case 113://Задаём Регламентные времена
                if(CurrPt.Value.size() == 1)
                {
                  DWord    Time1;
                  Time1  = atol( (char*)CurrPt.Value[0].c_str() );                                    
                  TaskParams.ReglamentTime = Time1;
                  taskFlag = true;
                }
                else
                { 
                  this->BaseCMD->LastErrorIndex = UNKNOWN_ER_IND;  
                }
                break;
                
              default:
                this->BaseCMD->LastErrorIndex = UNKNOWN_ER_IND;
                flag = false;
                break;          
              }
        }
      }
      else
      {
      }
    }
    if( flag == true )//all ok - save in memory
    { 
        TObject::Save_ParamsInInfo(&Params);
        this->SetNeedReset();
        this->SetNeedAnswer();
    }
    if( taskFlag == true )//all ok - save in memory
    { 
        TObjectTask::Save_TaskConfigInFlash(&TaskParams);
        this->SetNeedReset();
        this->SetNeedAnswer();
    }    
    
    this->ClearLastData();
    string AnswerState      = string((char*)ERROR_MASS[this->BaseCMD->LastErrorIndex]);
    Buffer->DataLen = CreateCMD(Buffer, BaseCMD->LastCMD, AnswerState, BaseCMD->LastData);
    SetFase(TX_CMD);
    
  return ExecutableCMD;
}
//*****************************************************************************
//***TGet_Cfg
//*****************************************************************************
//=============================================================================
// 
//=======================================
TCmd* TCmd_GetCfg::CMD_Execute(void* p1,void* p2)
{
 TBuffer    *Buffer           = (TBuffer*)p2;
 BaseCMD->LastErrorIndex      = OK_IND;
 TCmd *ExecutableCMD          = BaseCMD;
 Data_Params const      *ptr1     = TObject::PtrToConstants; 
 TTaskListConfig const  *ptr      = TObjectTask::TaskListConfig; 
 Boot_Params const      *BOOTptr = TObject::PtrToBootConstants;
 //////////////////
// TCalibrListConfig const *cptr =TObjectCalibr::CalibrListConfig; 
 //////////////////
 
 
    Tlist<TEt> Equipments =  GetTypes( BaseCMD->LastData);
    int size = Equipments.size();
    this->ClearLastData();
    if(size == 0)
    {//Нужно будет убрать столь простой способ задания ET и EN
       Word addr = atoi((char const*)ptr1->DevAddress);
       string Data ="";
       Data+="0,2,";
       Data+="\r\n>,100,"+TBuffer::WordToString(addr)+","+
                      TBuffer::WordToString(DEVICE_CURR_VER)+","+
                        TBuffer::WordToString(DEVICE_SUB_VER)+","+
                          TBuffer::WordToString(BOOTptr->CurVersion)+","+
                            TBuffer::LongToString(ptr1->Serial) + ',';
       
       Data+="\r\n>,113,1,"+TBuffer::WordToString(ptr->ReglamentTime)+",";
       PushBackToLastData(Data);
       
       //////////////////
//       Data ="60,2,\r\n";
//        for(Byte i=0; i<8; i++)
//        {  Data+=">,83,"+TBuffer::WordToString(i+1)+","+TBuffer::FloatToString(cptr->CalVolt[i],0,4)+",\r\n";}
//        if(Data.size() > 10) Data = string(Data.c_str(), Data.size()-3);
//      PushBackToLastData(Data);
      //////////////////
      
       
    }
    else
    {
      this->ClearLastData();
      if(size == 0)
      {
        BaseCMD->LastErrorIndex = NO_DATA;
      }
      for(int i = 0; i < size; i++)
      {
        TEt CurrEt = Equipments[i];
        int CurrSize = CurrEt.Ptypes.size();
        /*
        switch(CurrEt.EType)
        {
        case 60:// параметры самого устрйства мониторинга
          {
           string Data ="";
           Data+="60,1,";
           for(Byte i=0;i<MAX_TERMO;i++)
             Data+="\r\n>,9,"+TBuffer::IntToString(i+1)+","
             +TBuffer::WordToString(ptr->TermoTunes[i].Enabled)+","
             +TBuffer::WordToString(ptr->TermoTunes[i].Visible)+","
             +TBuffer::IntToString(ptr->TermoTunes[i].T_MAX)+","
             +TBuffer::IntToString(ptr->TermoTunes[i].T_MIN)+","
             +TBuffer::IntToString(ptr->TermoTunes[i].T_DEF)+",";          
             this->PushBackToLastData(Data);         
          }
          break;          
        }
        */
        
      }
    }
    string AnswerState      = string((char*)ERROR_MASS[BaseCMD->LastErrorIndex]);
    Buffer->DataLen = CreateCMD(Buffer, BaseCMD->LastCMD, AnswerState, BaseCMD->LastData);
    this->ClearLastData();
    SetFase(TX_CMD);

  return ExecutableCMD;
}
//*****************************************************************************
//***TCmd_UB
//*****************************************************************************
//=============================================================================
// 
//=======================================
TCmd* TCmd_UB::CMD_Execute(void* p1,void* p2)
{
 Byte const *Addr             = (Byte const *)TObject::PtrToBootConstants; //взяли существующие значения
 TBuffer    *Buffer           = (TBuffer*)p2;
 BaseCMD->LastErrorIndex      = OK_IND;
 TCmd *ExecutableCMD          = BaseCMD;
  

    TObject::WriteByteToFlash(0,Addr);
    this->SetNeedReset();
    this->SetNeedAnswer();
    this->ClearLastData();
    string AnswerState      = string((char*)ERROR_MASS[this->BaseCMD->LastErrorIndex]);
    Buffer->DataLen = CreateCMD(Buffer, BaseCMD->LastCMD, AnswerState, BaseCMD->LastData);
    SetFase(TX_CMD);

  return ExecutableCMD;
}
//*****************************************************************************
//*****************************************************************************
//*****************************************************************************
//=============================================================================
// 
//=============================================================================
TReleObject::TReleObject(Byte *port, Byte bit, bool inv,  bool lock):Pt(50, 1)
{
    Inv    =  inv;
    BIT    =  bit;
    PORT   =  port+2;//out
    port  +=  4;
    HOLD   =  lock;
   *port  |=  BIT; //dir
   
    HValue=Value=GetState();
}
//=============================================================================
// 
//=============================================================================
TReleObject::TReleObject(Byte *port, Byte bit, bool inv,  bool lock, bool fstate):Pt(50, 1)
{
    Inv    =  inv;
    BIT    =  bit;
    PORT   =  port+2;//out
    port  +=  4;
    HOLD   =  lock;
    
    if(fstate) SwitchON();
    else SwitchOFF();
    
   *port  |=  BIT; //dir
    HValue=Value=GetState();
}
//=============================================================================
// 
//=============================================================================
TReleObject::~TReleObject()
{
}
//=============================================================================
// 
//=============================================================================
bool TReleObject::GetState()
{ 
  if(Inv==true)
    return (bool)!((*PORT)&BIT); 
  else 
    return (bool)((*PORT)&BIT) ; 
}
//=============================================================================
// 
//=============================================================================
void TReleObject::SwitchON(void)
{
  if(HOLD==true)
  {
    if(Inv==true)
      *PORT |= BIT;
    else
      *PORT &= ~BIT;
  }
  else
  {
    if(Inv==true)
      *PORT &= ~BIT;
    else
      *PORT |= BIT;    
  }
}
//=============================================================================
// 
//=============================================================================
void TReleObject::SwitchOFF(void)
{
  if(HOLD==true)
  {
    if(Inv==true)
      *PORT |= BIT;
    else
      *PORT &= ~BIT;
  }
  else
  {
    if(Inv==true)
      *PORT |= BIT;  
    else
      *PORT &= ~BIT;
  }
}
//=============================================================================
// 
//=============================================================================
string TReleObject::GetValueString(void)
{
        string ReturnStr = "\r\n>,"+
                          TBuffer::WordToString(Pt.Ptype)+','+
                            TBuffer::WordToString(Pt.Pname)+','+
                             TBuffer::WordToString(GetState())+',';
        return ReturnStr;
}
//=============================================================================
void TReleObject::SetManual(bool Block,bool Inversion)
{
  HOLD=Block;
  Inv=Inversion;
}
//*****************************************************************************
//*****************************************************************************
//*****************************************************************************
//=============================================================================
// 
//=============================================================================
TEventManager::TEventManager():TFastTimer(1,&SecCount), CurrEvent(NULL), Events(10)
{

}
//=============================================================================
//=============================================================================
//======= end =================================================================