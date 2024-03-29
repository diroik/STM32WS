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
// ���������� ��� ����� ����������
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
//*****************************************************************************
//*****************************************************************************
//*****************************************************************************
//*****************************************************************************
//=============================================================================
// TCmd_msg declaration
// ������� ����� ��� ��������� ������ � ��������� ��
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
               PasswordPtr                = NULL;
               AddrPtr                    = NULL;
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
  //void                    SendRESET(void* p1 = 0,void* p2 = 0);
  void                    ClearErrorCount(void);
    
  string                  CmdText;
  Tlist<TCmdMessage*>    *MainMessages;
  Byte                    LastErrorIndex;  
  string                  LastCMD;
  string                  LastState;
  string                  ID;
  string                  Addr;
  Tlist<string>          LastData;  
  char                   *PasswordPtr;
  char                   *AddrPtr;
  
protected: 

  void                    SetNeedReset(void);  
  void                    SetNeedAnswer(void);
  
  Word                    CreateCMD(TBuffer *Buffer, string CMD, string INDICATION, Tlist<string> &Data, bool WithQuotes = true, TRtcTime *DateTime=NULL, string Passw="", string Addr="");
  TCmd *                  ParsingAnswer( TBuffer *Buffer );  

  bool                    PushToMainMessages(string& Addr, string CMD);
  
  void                    ClearMainAndCurrMessages(void);
  void                    ClearLastData(void);
  void                    PushBackToLastData(string data); 
  
  //bool                    CMD_CmpCRC(Byte  *RcvCRC, Word &CRC_SUM);
  //Tlist<TEt>              GetTypes( Tlist<string> &Data );
  void                    GetTypes(Tlist<string> &Data, Tlist<TEt*> &ReturnValue);  
  TCmd_msg               *BaseCMD; 
private:   
  
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
// ��������� ������������
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
// ������ ������������
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
//=================================================
#endif