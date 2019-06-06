#ifndef ADV_CLASSES_H
#define ADV_CLASSES_H
//=================================================
#include <Classes.h>
//*****************************************************************************
//*****************************************************************************
//*****************************************************************************
//*****************************************************************************
//=============================================================================
// TComPort0 declaration
// реализация СОМ порта процессора
//==================================== 
class TComPort0: public TComPort
{  
public:
  	TComPort0(Word IntBufSize);
       ~TComPort0();
               
         void     RX_Ready(void);
         
private:         
         void     TX_Ready(void); 
         void     RX_Disable(void);         
         //static __interrupt void     DATA_RX_TX(void);   //USB    
};
//=============================================================================
// TComPort1 declaration
// Реализация СОМ порта процессора
//==================================== 
class TComPort1: public TComPort
{ 
public:
  	TComPort1(Word IntBufSize);
       ~TComPort1(); 
            
         void     RX_Ready(void);
private:         
         void     TX_Ready(void); 
         void     RX_Disable(void);                     
         //static __interrupt void     DATA_RX_TX(void);   //USB1            
};
//=============================================================================
// TComPort2 declaration
// Реализация СОМ порта процессора
//==================================== 
class TComPort2: public TComPort
{ 
public:
  	TComPort2(Word IntBufSize);
       ~TComPort2(); 
            
         void     RX_Ready(void);
private:         
         void     TX_Ready(void); 
         void     RX_Disable(void);                     
         //static __interrupt void     DATA_RX_TX(void);   //          
};
//=============================================================================
// TComPort3 declaration
// Реализация СОМ порта процессора
//==================================== 
class TComPort3: public TComPort
{
    friend class TComPort0;
    friend class TModemComPort;    
public:
  	TComPort3(Word IntBufSize);
       ~TComPort3(); 
            
         void     inline RX_Ready(void);
private:         
         void     TX_Ready(void); 
         void     RX_Disable(void);                     
         //static __interrupt void     DATA_RX_TX(void);   //          
};

//=============================================================================
//*****************************************************************************
//*****************************************************************************
//*****************************************************************************
//*****************************************************************************
//=============================================================================
// TCmd_msg declaration
// базовый класс для текстовых команд в протоколе ТМ
//==================================== 
class TCmd_msg:public TCmd
{ 
public:    
             TCmd_msg():Fase(WAIT_MES),CalcCRC(0)
             { 
               BaseCMD         = this;
               Addr            = "0";
               ID              = "0";
               CmdText         = "";
              _needReset = _needAnswer = false;
               LastErrorIndex  = 0;
              _errorCount      = 0;               
               MainMessages               = NULL;
               AlternateMessages          = NULL;
               PasswordPtr                = NULL;
               AddrPtr                    = NULL;
              _currNeedCmdAnswerMessage   = NULL;
               OnAnswerErrorExecute       = NULL;
             }   
            ~TCmd_msg()  {} 
            
  TCmd*              GET_Message(void* p1 = 0,void* p2 = 0);               
  TCmd*              CMD_Execute(void* p1 = 0,void* p2 = 0);
  TCmd*              CMD_Recv( void* p1,void* p2 );  
  TCmd*              CMD_Send( void* p1,void* p2 );
  
  void                    AddToCMD_Messages(TCmd_msg* msg);
  TCmd_msg*               GetCMDbyKey(string key);
  Byte                    GetFase(void);   
  void                    SetFase(Byte NewFase);   
  bool                    NeedReset(void);
  bool                    NeedAnswer(void);  
  bool                    GetBroadcastAddr(void);
  void                    SendRESET(void* p1 = 0,void* p2 = 0);
  void                    ClearErrorCount(void);
    
  PFV                     OnAnswerErrorExecute;
  
  
  string                  CmdText;
  Tlist<TCmdMessage*>    *MainMessages;
  Tlist<TCmdMessage*>    *AlternateMessages;
  Byte                    LastErrorIndex;  
  string                  LastCMD;
  string                  LastState;
  string                  ID;
  string                  Addr;
  Tlist<string>          LastData;  
  char                   *PasswordPtr;
  char                   *AddrPtr;
  
protected: 
  
  TCmdMessage            *GetCurrNeedCmdAnswerMessage(void);   
  void                    SetCurrNeedCmdAnswerMessage(TCmdMessage *NewMess);   
  

  void                    SetNeedReset(void);  
  void                    SetNeedAnswer(void);
  
  Word                    CreateCMD(TBuffer *Buffer, string CMD,string INDICATION, Tlist<string> &Data, bool WithQuotes = true, TRtcTime *DateTime=NULL, string Passw="", string Addr="");
  TCmd *                  ParsingAnswer( TBuffer *Buffer );  
  
  bool                    PushToAlternateMessages(string &Pass, string &Addr, string IND, bool WithQuotes = true); 
  bool                    PushToMainMessages(string& Addr, string CMD);
  void                    ClearAlternateMessages(void);  
  
  void                    ClearMainAndCurrMessages(void);
  void                    ClearLastData(void);
  void                    PushBackToLastData(string data); 
  
  bool                    CMD_CmpCRC(Byte  *RcvCRC, Word &CRC_SUM);
  Tlist<TEt>              GetTypes( Tlist<string> &Data );
  void                    GetTypes(Tlist<string> &Data, Tlist<TEt> &ReturnValue);  
  TCmd_msg               *BaseCMD; 
private:   
  
  TCmdMessage               *_currNeedCmdAnswerMessage;
  Byte                      _errorCount;
  bool                      _needReset;
  bool                      _needAnswer;  
  Byte                      Fase;      
  Word                      CalcCRC;     
  Tlist<TCmd_msg*>          CMD_Messages;   
};
//=============================================================================
// Error
//==================================== 
class TCmd_ERROR:public TCmd_msg
{ 
public:
   TCmd_ERROR(TCmd_msg *baseCMD, string cmdText)
  {
     CmdText = cmdText;  
     BaseCMD = baseCMD;
   }
  ~TCmd_ERROR(){}
   TCmd*       CMD_Execute(void* p1 = 0,void* p2 = 0);
   
private: 
};
//=============================================================================
// Error
//==================================== 
class TCmd_UnknownCMD:public TCmd_msg
{ 
public:
   TCmd_UnknownCMD(TCmd_msg *baseCMD, string cmdText)
  {
     CmdText = cmdText;  
     BaseCMD = baseCMD;
   }
  ~TCmd_UnknownCMD(){}
   TCmd*       CMD_Execute(void* p1 = 0,void* p2 = 0);
   
private: 
};
//=============================================================================
// 
//==================================== 
class TCmd_Data:public TCmd_msg
{ 
public:
  TCmd_Data(TCmd_msg *baseCMD, string cmdText)
  {
     CmdText = cmdText;  
     BaseCMD = baseCMD;
   }
  ~TCmd_Data(){}
   TCmd*        CMD_Execute(void* p1 = 0,void* p2 = 0);
   
private: 
};
//=============================================================================
// 
//==================================== 
class TCmd_GetData:public TCmd_msg
{ 
public:
  TCmd_GetData(TCmd_msg *baseCMD, string cmdText)
  {
     CmdText = cmdText;  
     BaseCMD = baseCMD;
   }
  ~TCmd_GetData(){}
   TCmd*        CMD_Execute(void* p1 = 0,void* p2 = 0);
   
private: 
};
//=============================================================================
// Установка Калибровочных коэффициэнтов
//==================================== 
class TCmd_SetCalibr:public TCmd_msg
{ 
public:
  TCmd_SetCalibr(TCmd_msg *baseCMD, string cmdText)
  {
     CmdText = cmdText;  
     BaseCMD = baseCMD;
   }
  ~TCmd_SetCalibr(){}
   TCmd*        CMD_Execute(void* p1 = 0,void* p2 = 0);
   
private: 
};
//=============================================================================
// Чтение Коэффициэнтов
//==================================== 
class TCmd_GetCalibr:public TCmd_msg
{ 
public:
  TCmd_GetCalibr(TCmd_msg *baseCMD, string cmdText)
  {
     CmdText = cmdText;  
     BaseCMD = baseCMD;
   }
  ~TCmd_GetCalibr(){}
   TCmd*        CMD_Execute(void* p1 = 0,void* p2 = 0);
   
private: 
};
//=============================================================================
// Установка конфигурации
//==================================== 
class TCmd_SetCfg:public TCmd_msg
{ 
public:
   TCmd_SetCfg(TCmd_msg *baseCMD, string cmdText)
   { 
    CmdText = cmdText;  
    BaseCMD = baseCMD;
  }
  ~TCmd_SetCfg(){}
   TCmd*  CMD_Execute(void* p1 = 0,void* p2 = 0);
   
private: 
};
//=============================================================================
// Чтение конфигурации
//==================================== 
class TCmd_GetCfg:public TCmd_msg
{ 
public:
   TCmd_GetCfg(TCmd_msg *baseCMD, string cmdText)
   { 
    CmdText = cmdText;  
    BaseCMD = baseCMD;
  }
  ~TCmd_GetCfg(){}
   TCmd*  CMD_Execute(void* p1 = 0,void* p2 = 0);
   
private: 
};
//=============================================================================
// 
//==================================== 
class TCmd_UB:public TCmd_msg
{ 
public:
  TCmd_UB(TCmd_msg *baseCMD, string cmdText)
   { 
     CmdText = cmdText;     
     BaseCMD = baseCMD;
   }
  ~TCmd_UB(){}
   TCmd*  CMD_Execute(void* p1 = 0,void* p2 = 0);
   
private: 
};
//=============================================================================
// 
//==================================== 
class TCmd_UD:public TCmd_msg
{ 
public:
  TCmd_UD(TCmd_msg *baseCMD, string cmdText)
   { 
     CmdText = cmdText;     
     BaseCMD = baseCMD;
   }
  ~TCmd_UD(){}
   TCmd*  CMD_Execute(void* p1 = 0,void* p2 = 0);
   
private: 
};
//=============================================================================
// 
//==================================== 
class TCmd_UE:public TCmd_msg
{ 
public:
  TCmd_UE(TCmd_msg *baseCMD, string cmdText)
   { 
     CmdText = cmdText;     
     BaseCMD = baseCMD;
   }
  ~TCmd_UE(){}
   TCmd*  CMD_Execute(void* p1 = 0,void* p2 = 0);
   
private: 
};
//*****************************************************************************
//*****************************************************************************
//*****************************************************************************
//=============================================================================
// 
//====================================
class TVirtualInput:public IInput
{
public:
  TVirtualInput(Byte et, Byte en, Byte pt, Byte pn, Byte index, bool timerFlag=false):Value(0),Et(et),En(en),Pt(pt),Pn(pn),InIndex(index)
  { 
    IsVirtual = true;
    TimerFlag = timerFlag;
  }
  
  Byte GetValue()         { return Value;}
  void SetValue(Byte val) { Value = val;}
  
  Byte Et;
  Byte En;
  Byte Pt;
  Byte Pn;
  Byte InIndex;
  
private:
  Byte Value;
};
//*****************************************************************************
//*****************************************************************************
//*****************************************************************************
class TReleObject
{ 
public:
  TReleObject(Byte *port, Byte bit, bool inv, bool lock);
  TReleObject(Byte *port, Byte bit, bool inv, bool lock, bool fstate);
 ~TReleObject();
  
  bool GetState(void);

  string GetValueString(void);
  
  void SwitchON(void);
  void SwitchOFF(void);
  void SetManual(bool Block,bool Inversion);
  
  bool Value;
  bool HValue;
private:
  bool Inv; //Инверсия
  bool HOLD;//блокировка состояния
  Byte  BIT;
  Byte *PORT;
  
  TSimplPtype Pt;
};
//*****************************************************************************
//*****************************************************************************
//*****************************************************************************
//int (Screen::*pmf2)()
//*****************************************************************************

class TEvent
{
public:
  TEvent(PFB _func)
  {
    Function  = _func;    
    Parameter = NULL;
  }
  
  TEvent(PFB _func, void* p)
  {
    Function  = _func;    
    Parameter = p;
  }
  
  bool Execute()
  {
    bool retValue = false;
    if(this->Function != NULL )
    {
      if(Function(Parameter))
      {
        if(Parameter != NULL)
        {
          //delete Parameter;
          Parameter = NULL;
        }
        retValue = true;
      }
    }
    return retValue;
  }
  void* Parameter;
  
  
private:  
  PFB            Function;
};
//*****************************************************************************
//*****************************************************************************
//*****************************************************************************
class TEventManager:  public TFastTimer
{
public:
  TEventManager();

  
  void FASE_ExecuteEventProcess()
  {
    if( GetTimValue(0) <= 0 )
    {
      if(CurrEvent == NULL)
      { CurrEvent = Events.pop();}
      if(CurrEvent != NULL)
      {
        if(CurrEvent->Execute() == true )
        {
          delete CurrEvent;
          CurrEvent = NULL;
        }
      }
      SetTimer( 0, 1 );
    }
  }
  TFifo<TEvent*>  Events;
  TEvent          *CurrEvent;
};
//=================================================
#endif