#ifndef ADV_CLASSES_H
#define ADV_CLASSES_H
//=================================================
#include <Classes.h>
#include <vector>
 
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
         static   void     DATA_RX_TX(void);   //USB    
};
//=============================================================================
// TComPort1 declaration
// Реализация СОМ порта процессора
//==================================== 
class TCmd
{ 
public:
   TCmd();
  ~TCmd();
  
  virtual TCmd* CMD_Execute(void* p1 = 0,void* p2 = 0);
  virtual TCmd* CMD_Recv( void* p1 = 0,void* p2 = 0 );
  virtual TCmd* CMD_Send( void* p1 = 0,void* p2 = 0 ); 
  
  Byte            GetFase(void);
  void            SetFase(Byte fase);
  void            AddToCMD_Messages(TCmd* msg);
  
  string          CmdText;  
  static Word     ErrorCNT; 
  static bool     ResetFlg;
 
protected:  
  
  TCmd*       ParsingAnswer( TBuffer *BufferModem );
  string         CreateCMD(string CMD, string INDICATION, vector<string> Equpments, TRtcTime *DateTime = NULL);
  TCmd*       GetCMDbyKey(string key);
  
  static vector<TEt> GetTypes(vector<string> Data);
  static sWord       FindEtPtStrings(const vector<string> &InData, vector<TEt*> &Equpments);
  
  static Word                 LastErrorIndex;  
  static string               LastCMD;
  static vector<string>       LastData;
  static string               Addr;
  static Byte                 Fase; 
  
private:
  
  static Tlist<TCmd*> CMD_Messages;  
};


//=============================================================================
// 
//==================================== 
class TCmdErrorCMD:public TCmd
{ 
public:
  TCmdErrorCMD(string cmdText)
  { CmdText = cmdText;}
  ~TCmdErrorCMD(){}
 
  TCmd*     CMD_Execute(void* p1 = 0,void* p2 = 0);  

protected:  
private:
};

//=============================================================================
// 
//==================================== 
class TCmdUB:public TCmd
{ 
public:
   TCmdUB(string cmdText)
   { CmdText = cmdText;}
  ~TCmdUB(){}
 
  TCmd*     CMD_Execute(void* p1 = 0,void* p2 = 0);  

protected:  
private:
};
//=============================================================================
// 
//==================================== 
class TCmdUD:public TCmd
{ 
public:
   TCmdUD(string cmdText)
   { CmdText = cmdText;}
  ~TCmdUD(){}
 
  TCmd*     CMD_Execute(void* p1 = 0,void* p2 = 0);  

protected:  
private:
};
//=============================================================================
// 
//==================================== 
class TCmdUE:public TCmd
{ 
public:
   TCmdUE(string cmdText)
   { CmdText = cmdText;}
  ~TCmdUE(){}
 
  TCmd*     CMD_Execute(void* p1 = 0,void* p2 = 0);  

protected:  
private:
};
//=============================================================================
// 
//==================================== 
class TCmdGV:public TCmd
{ 
public:
  TCmdGV(string cmdText)
  {CmdText = cmdText;}
  ~TCmdGV(){}
  
  TCmd*     CMD_Execute(void* p1 = 0,void* p2 = 0);  

protected:  
private:
};
//=============================================================================
// 
//==================================== 
class TCmdGD:public TCmd
{ 
public:
  TCmdGD(string cmdText)
  {CmdText = cmdText;}
  ~TCmdGD(){}
  
  TCmd*     CMD_Execute(void* p1 = 0,void* p2 = 0);  

protected:  
private:
};
//=============================================================================
//TLedObj declaration
// Мигалка
//=============================================================================
class TLedObj:public TTimer
{
public:
  TLedObj():TTimer(1),AlarmsStatus(0),BlinkMode(false)
  {}
  void SetRedLed(void);
  void SetGreenLed(void);
  void SetBlinkLeds(void);
  
  bool  BlinkMode;
  DWord AlarmsStatus;
}; 

#endif