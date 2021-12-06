//=============================================================================
#include <Classes.h>
#include <AdvClasses.h>
#include <PowerMetr.h>
#include <cstdlib>
#include <cmath>

extern class TPowerMetrManager      *PowerMetrManager;
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
  if( /*Passw != NULL && */Passw != "")
  { index += TBuffer::CopyString(&Buffer->BUF[index], (Byte*)Passw.c_str());}
  else
  { index += TBuffer::CopyString(&Buffer->BUF[index], (Byte*)ptr->PassWord);}
  
  Buffer->BUF[index++] = ',';
  
  if( /*Addr != NULL &&*/ Addr != "")
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
  
  if( /*INDICATION != NULL &&*/ INDICATION !="" )
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
              { returnValue = GetCMDbyKey( "UnknCMD" );}
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
                    TBuffer::Split(&Buffer->BUF[index], lenth, ">>,", BaseCMD->LastData);
                  }
                }
                BaseCMD->LastErrorIndex = OK_IND;
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
void TCmd_msg::ClearMainAndCurrMessages(void)          
{
  if( BaseCMD->MainMessages != NULL )
  {
    for(int i = 0; i < BaseCMD->MainMessages->size(); i++)
    {
      CLEAR_WDT;
      TCmdMessage *CurrMessage = BaseCMD->MainMessages->operator[](i);
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
void  TCmd_msg::GetTypes(Tlist<string> &Data, Tlist<TEt*> &ReturnValue)
{
 string         DataString = "";
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
      ReturnValue.push_back( new TEt( atoi(TmpList[0].c_str()), atoi(TmpList[1].c_str()) ) );
      TEt *CurrEt = *ReturnValue.back();
      int SubSize = PList.size();
      for(int j = 1; j < SubSize; j++)
      {
        TmpList.clear();
        DataString = PList[j];
        TBuffer::Split(DataString, ",", TmpList);
        int s = TmpList.size();
        if(s < 2)
        { return;}
        
        //TPt CurrPt( atoi(TmpList[0].c_str()), atoi(TmpList[1].c_str()) );//
        TPt *CurrPt = new TPt( atoi(TmpList[0].c_str()), atoi(TmpList[1].c_str()) );//
        
        for(int k = 2; k < s; k++)//пишем сами данные
        {CurrPt->Value.push_back( TmpList[k] );}
        CurrEt->Ptypes.push_back( CurrPt );
      }
      //ReturnValue.push_back( CurrEt );              
    }
    return;
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
#pragma optimize=z none
TCmd* TCmd_UnknownCMD::CMD_Execute(void* p1,void* p2)
{ 
 TBuffer        *Buffer   = (TBuffer*)p2;
 TCmd *ExecutableCMD      = BaseCMD;
  //Word         Len = sizeof(BOOT_PARAMS);;

   if(BaseCMD->LastCMD == "UB")
   {
      ///Boot_Params tmp = BOOT_PARAMS;
      ///tmp.BootStatus = 0x00;
      ///TObject::WriteBufToPage((Byte const*)&BOOT_PARAMS, (Byte*)&tmp, Len-1 );  //записали куда надо
      Data_Params      Params        = *TObject::PtrToConstants;
      Params.BOOT_PARAMS.BootStatus = 0;
      TObject::Save_ParamsInInfo(&Params);
      SetNeedReset();
     
//      SetNeedAnswer();//not need!!
//      ClearLastData();
//      BaseCMD->LastErrorIndex = OK_IND;
//      if( BaseCMD->LastCMD == "")
//      BaseCMD->LastCMD = "ERROR";
//      Buffer->DataLen = CreateCMD(Buffer, BaseCMD->LastCMD, string((char*)ERROR_MASS[this->BaseCMD->LastErrorIndex]), BaseCMD->LastData);
//      SetFase(TX_CMD);
      return ExecutableCMD;
   }
   else if(BaseCMD->LastCMD == "Reboot")
   {
       __disable_interrupt();
      while(1);
     
     //SetNeedReset();
   }
    else if(BaseCMD->LastCMD != "CfData")
    {
        ClearLastData();
        BaseCMD->LastErrorIndex = CMD_ER_IND;
        if( BaseCMD->LastCMD == "")
          BaseCMD->LastCMD = "ERROR";
        Buffer->DataLen = CreateCMD(Buffer, BaseCMD->LastCMD, string((char*)ERROR_MASS[this->BaseCMD->LastErrorIndex]), BaseCMD->LastData);
        SetFase(TX_CMD);
        return ExecutableCMD;
    }

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
 
    Tlist<TEt*> Equipments;
    GetTypes( BaseCMD->LastData, Equipments);
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
  
 Data_Params      Params        = *TObject::PtrToConstants;
 
    Tlist<TEt*> Equipments;
    GetTypes( BaseCMD->LastData, Equipments);
    int size = Equipments.size();
    
    ClearLastData();
    if(size == 0)
    {
      BaseCMD->LastErrorIndex = NO_DATA;
    }
    for(int i = 0; i < size; i++)
    {
      TEt *CurrEt = Equipments[i];
      int CurrSize = CurrEt->Ptypes.size();
        for(int j = 0; j < CurrSize; j++)
        { 
          TPt *CurrPt = CurrEt->Ptypes[j];
          if( CurrEt->EType == 0 || CurrEt->EType == 2)
          {// параметры самого устрйства мониторинга
              switch(CurrPt->PType)
              {
               case 108://et !!!!!!!!!!!!!!!!!!!!!!!
                if(CurrPt->Value.size() == 1)
                { 
                  Word et = atoi(CurrPt->Value[0].c_str());
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
                if(CurrPt->Value.size() == 1)
                { 
                  Word en = atoi(CurrPt->Value[0].c_str());
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
                if(CurrPt->Value.size() == 1)
                {
                  Word pn = atoi(CurrPt->Value[0].c_str());
                  Params.wReserved[2] = pn;
                  flag = true;
                }
                else
                { 
                  BaseCMD->LastErrorIndex = UNKNOWN_ER_IND;
                  flag = false;
                }
                break;
                
              case 111://adr
                if(CurrPt->Value.size() == 1)
                {
                  string ID = CurrPt->Value[0];
                  if(ID.size() == 2)
                  {
                    for(int l=0;l<ID.size();l++)
                    { Params.DevAddress[l]        = ID.c_str()[l];}
                    Params.DevAddress[ID.size()]  = 0;
                    flag = true;
                  }
                  else
                  { 
                    BaseCMD->LastErrorIndex = UNKNOWN_ER_IND;
                    flag = false;
                  }
                }
                else
                { 
                  BaseCMD->LastErrorIndex = UNKNOWN_ER_IND;
                  flag = false;
                }
                break;
                
              case 100:
                if(CurrPt->Value.size() == 1)
                {
                  long    Serial;
                  Serial  = atol( (char*)CurrPt->Value[0].c_str() );                                    
                  Params.Serial = Serial;
                  flag = true;
                }
                else
                { 
                  BaseCMD->LastErrorIndex = UNKNOWN_ER_IND;  
                }
                break;
                
              case 112://pass
                if(CurrPt->Value.size() == 1)
                {
                  string PASSWORD = CurrPt->Value[0];
                  if(PASSWORD.size() <= 8)
                  {
                    for(int l=0;l<PASSWORD.size();l++){ Params.PassWord[l]        = PASSWORD.c_str()[l];}
                    flag = true;
                  }
                  else
                  { 
                    BaseCMD->LastErrorIndex = UNKNOWN_ER_IND;
                    flag = false;
                  }
                }
                else
                { 
                  BaseCMD->LastErrorIndex = UNKNOWN_ER_IND;
                  flag = false;
                }
                break;
                
              case 113://Задаём Регламентные времена
                if(CurrPt->Value.size() == 1)
                {
                  DWord    Time1;
                  Time1  = atol( (char*)CurrPt->Value[0].c_str() );                                    
                  Params.ReglamentTime = Time1;
                  flag = true;
                }
                else
                { 
                  BaseCMD->LastErrorIndex = UNKNOWN_ER_IND;  
                }
                break;
                
              default:
                BaseCMD->LastErrorIndex = UNKNOWN_ER_IND;
                flag = false;
                break;          
              }
          }
          else if( CurrEt->EType == 60)
          {  
             switch (CurrPt->PType)
             {
             case 83:
               if(CurrEt->EName == 2)
               {//запись коэффициентов напрямую
                  if(CurrPt->Value.size() == 1)
                  {
                    if( (CurrPt->PName > 0)&(CurrPt->PName <= 8) )
                    {
                      if( PowerMetrManager != NULL && PowerMetrManager->PowerMetrs != NULL )
                      {
                        float koef = atof( (char*)CurrPt->Value[0].c_str() );
                        Params.CalVolt[CurrPt->PName-1] = koef;
                        flag = true;                        
                      }
                    }
                    else
                    { 
                      BaseCMD->LastErrorIndex = UNKNOWN_ER_IND;
                      flag = false;
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
    if( flag == true )//all ok - save in memory
    { 
        TObject::Save_ParamsInInfo(&Params);
        SetNeedReset();
        SetNeedAnswer();
    }

    ClearLastData();
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
//#pragma optimize=z none
TCmd* TCmd_GetCfg::CMD_Execute(void* p1,void* p2)
{
 TBuffer    *Buffer           = (TBuffer*)p2;
 BaseCMD->LastErrorIndex      = OK_IND;
 TCmd *ExecutableCMD          = BaseCMD;
 
 Data_Params const      *ptr1     = TObject::PtrToConstants; 
 Word regTime                           = ptr1->ReglamentTime;
 
 
    Tlist<TEt*> Equipments;
    GetTypes( BaseCMD->LastData, Equipments);
    int size = Equipments.size();
    
    ClearLastData();
   
    string Data ="";
    if(size > 0)
    {
       if(PowerMetrManager !=NULL)
      {
        Data ="60,1,\r\n";
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
      }
    }
    else//(size == 0)
    {//Нужно будет убрать столь простой способ задания ET и EN
       Word addr = atoi((char const*)ptr1->DevAddress);
       
       Data+="0,2,";
       Data+="\r\n>,100,"+TBuffer::WordToString(addr)+","+
                      TBuffer::WordToString(DEVICE_CURR_VER)+","+
                        TBuffer::WordToString(DEVICE_SUB_VER)+","+
                          TBuffer::WordToString(ptr1->BOOT_PARAMS.CurVersion)+","+
                            TBuffer::LongToString(ptr1->Serial) + ',';
       
       Data+="\r\n>,113,1,"+TBuffer::WordToString(regTime)+",";

       Data +=">>,60,2,\r\n";
       for(Byte i=0; i < 8; i++){
         Data+=">,83,"+TBuffer::WordToString(i+1)+","+TBuffer::FloatToString(ptr1->CalVolt[i],0,4)+",\r\n";
       }
       Data+=">,101,1,"+TBuffer::WordToString(ptr1->wReserved[0])+","+TBuffer::WordToString(ptr1->wReserved[1])+","+TBuffer::WordToString(ptr1->wReserved[2])+",";
    }

    if(Data.size()> 0)
      PushBackToLastData(Data);
    
    string AnswerState      = string((char*)ERROR_MASS[BaseCMD->LastErrorIndex]);
    Buffer->DataLen = CreateCMD(Buffer, BaseCMD->LastCMD, AnswerState, BaseCMD->LastData);
    ClearLastData();
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
  TBuffer    *Buffer           = (TBuffer*)p2;
  BaseCMD->LastErrorIndex      = OK_IND;
  TCmd *ExecutableCMD          = BaseCMD;
  
 
  Data_Params      Params        = *TObject::PtrToConstants;
  Params.BOOT_PARAMS.BootStatus = 0;
  TObject::Save_ParamsInInfo(&Params);
  
  SetNeedReset();
  SetNeedAnswer();
  
  ClearLastData();
  string AnswerState      = string((char*)ERROR_MASS[this->BaseCMD->LastErrorIndex]);
  Buffer->DataLen = CreateCMD(Buffer, BaseCMD->LastCMD, AnswerState, BaseCMD->LastData);
  SetFase(TX_CMD);

  return ExecutableCMD;
}
//=============================================================================
//=============================================================================
//======= end =================================================================