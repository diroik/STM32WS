#ifndef CLASSES_H
#define CLASSES_H
//=============================================================================
#include <init.h>
//#include <Class_Tamplate.h>
#include <Classes_def.h>
#include <type_define.h>
#include <intrinsics.h>
#include <params_def.h>


#include <stdlib.h>
#include <math.h>
#include <typeinfo>
#include <new>
#include <string>

using std::string;


  class TDateTime;
  class TBuffer;
  class TTimer; 
  class TPtype;
  class IDatchik;  
  class TDatchik;  
  class TEquipment;
  class TPt;
  class TEt;  
  
#define foreach(new_var, list)\
  list.set_interator(0);\
        while(new_var = list.foreach_next())
          


#define NVIC_VectTab_FLASH           ((uint32_t)0x08000000)


extern const Boot_Params  BOOT_PARAMS;
//=============================================================================
//*****************************************************************************
//*** Class TFifo
//*****************************************************************************
//=============================================================================
// TFifo declaration
//==================================== 
template <class Type>
class TFifo
{ 
public:
  
//=============================================================================
  TFifo(unsigned int sz):max_size(sz),size(0),front_ind(0),end_ind(0)
  {
      Buf = new Type[sz];
  }

//=============================================================================
  ~TFifo()
  {
      clear();
      delete[] Buf;
  }
  
//=============================================================================
    Type       pop()
   {
      Type tmp = (Type)0;
      if (size > 0)
      {
        tmp = Buf[front_ind];
        if (++front_ind == max_size)
            front_ind = 0;
        size--;
      }
      else
      {
        front_ind = end_ind = 0;
      }
      return tmp;
   }
   
//=============================================================================
    void       push(const Type &elem)
   {
      Buf[end_ind] = elem;
      if (++end_ind == max_size)
        end_ind = 0;
      
      if ( size == max_size)
      {
        if (++front_ind == max_size)
          front_ind = 0;
      }
      else
        size++;     
   }

//=============================================================================
    bool       empty() const
   {
     return (size == 0);
   }
   
//=============================================================================
    void       clear(void)
   {
    while(size)
      {
      Buf[size-- -1] = 0;
      CLEAR_WDT;
      }
    front_ind = end_ind = 0;
   }
   
  unsigned int        size;   
private: 
  unsigned int        max_size;
  
  Type        *Buf;
  unsigned int        front_ind;
  unsigned int        end_ind;
};


////////////////////////////////////////////////////////////////////////
// EEANN Tlist_item
////////////////////////////////////////////////////////////////////////


template <class elemType>
class Tlist_item 
{
public:
/*********************************************************************************/
   Tlist_item( elemType value ):_next(0),_back(0)
   {
     _value = value;//new elemType(value);
   }
/*********************************************************************************/
   #pragma diag_suppress=Pe414 
   ~Tlist_item(  )
   {
     //delete (elemType*)_value;
     //free(_value);
     
     string flag = typeid(_value).name();
     //flag = "";
     if(typeid(_value) == typeid(TEt*))
     {
       long pd = (long)&_value;
       if(pd > 0)
       {
         TEt* ptr = *((TEt**)pd);
         if(ptr > 0)
           delete ptr;
       }
     }
     else if(typeid(_value) == typeid(TPt*))
     {
       long pd = (long)&_value;
       if(pd > 0)
       {
         TPt* ptr = *((TPt**)pd);
         if(ptr > 0)
           delete ptr;
       }
     }
     
   }
/*********************************************************************************/
    elemType     value()
   {
     return _value;
   }
/*********************************************************************************/
    void         value(elemType new_value)
   {
     _value = new_value; 
   }
/*********************************************************************************/
    Tlist_item*  next()
   {
   return _next;  
   }
/*********************************************************************************/
    Tlist_item*  prev()
   {
   return _back;
   }
/*********************************************************************************/
    elemType     *ptr()
   {
    return &_value;
   }
/*********************************************************************************/
    void         next(Tlist_item<elemType>* ptr)
   {
    _next = ptr;
   }
/*********************************************************************************/
    void         prev(Tlist_item<elemType>* ptr)
   {
    _back = ptr;
   }
/*********************************************************************************/
   
private:
   elemType _value;
   Tlist_item *_next;
   Tlist_item *_back;
};
////////////////////////////////////////////////////////////////////////
// EEANN Tlist
////////////////////////////////////////////////////////////////////////
template <class elemType>
class Tlist 
{
public:
/*********************************************************************************/
   Tlist():_at_front(0),_at_end(0),_at_interator(0),_size(0),interator(0)
   {
   }
/*********************************************************************************/
   Tlist(const Tlist<elemType> &rhs):_at_front(0),_at_end(0),_size(0)
   {
      this->insert_all(rhs);   
   }
/*********************************************************************************/
   ~Tlist()
   {
      clear();
   }
/*********************************************************************************/
    elemType operator[]( int ix ) const
   {
    if( ix >= _size)
      return _at_end->value();
    
    Tlist_item<elemType> *tmp;
    tmp = _at_front;
    //HOLD_WDT;
    for(int i=0;i<ix;i++)
    {     
      tmp = tmp->next();
    }
    CLEAR_WDT;
    return  tmp->value();
    
   }
/*********************************************************************************/
    Tlist<elemType> &operator=( const Tlist<elemType> &newlist ) 
   {
    this->clear();
    HOLD_WDT;
    for( int i=0; i < newlist._size; i++)
    { 
      this->push_back( newlist[i] );
    }
    CLEAR_WDT;
    return *this;
   }
/*********************************************************************************/
    int          size()
   {
    return this->_size;  
   }
/*********************************************************************************/
    bool         empty()
   {
    if ( _size == 0)
      return true;
    else
      return false;
   }
/*********************************************************************************/
    void         push_front(elemType value)
   {
    Tlist_item<elemType> *ptr;
    ptr = new Tlist_item<elemType>( value );
    if ( !_at_front )
      _at_front = _at_end = ptr;
    else 
    {
      ptr->next( _at_front );
      _at_front->prev(ptr);
      _at_front = ptr;
     }
    bump_up_size();
   }
/*********************************************************************************/
    void         push_back(elemType value)
   {
    Tlist_item<elemType> *ptr;
    ptr = new Tlist_item<elemType>( value );    
    if ( _at_end == 0)
    {   _at_end = _at_front = ptr;}
    else 
    {
        _at_end->next(ptr);
        ptr->prev(_at_end);
        _at_end = ptr;
    }
    bump_up_size();
   }
/*********************************************************************************/
    void         pop_front()
   {
   Tlist_item<elemType> *ptr; 
     if ( _at_front ) 
     {
        ptr = _at_front;
        if (_size == 1)
          _at_front = _at_end = 0;
        else   
          _at_front = _at_front->next();
        bump_down_size();
        delete ptr;
     }  
   }
/*********************************************************************************/
   //void         pop_back();
/*********************************************************************************/
    bool         is_front(elemType elem)
   {
    return *_at_front->ptr() == elem;
   }
/*********************************************************************************/
    bool         is_back(elemType elem)
   {
   return *_at_end->ptr() == &elem;
   }
/*********************************************************************************/
    elemType*    front()
   {
   return _at_front->ptr();
   }
/*********************************************************************************/
    elemType*    back()
   {
     return _at_end->ptr();
   }
/*********************************************************************************/
     int          remove( elemType value )
   {
   Tlist_item<elemType>   *plist = _at_front;
   int                    elem_cnt = 0;
   
    while ( plist && ( plist->value() == value) )//если в начале списка
    {
      CLEAR_WDT;
      plist = plist->next();
      pop_front();
      ++elem_cnt;
    }
    
    if ( ! plist )  //если список пуст то возврат кол-ва удалений
      return elem_cnt;
    
    Tlist_item<elemType> *prev = plist;
    plist = plist->next();
    while ( plist ) 
    {
      CLEAR_WDT;
      if ( plist->value() == value ) 
      {
        prev->next( plist->next() );
        delete plist;
        ++elem_cnt;
        bump_down_size();
        plist = prev->next();
        if ( ! plist ) 
        {
          _at_end = prev;
          return elem_cnt;
        }
       }
       else 
       {
          prev = plist;
          plist = plist->next();
       }
    }
    return elem_cnt;
   }
/*********************************************************************************/
     void         clear ()
   {
    while ( _at_front )
    {
      CLEAR_WDT;
      pop_front();
    }
    _size = 0;
    _at_front = _at_end = 0;
   }
/*********************************************************************************/
   void         concat( const Tlist<elemType> &i1 )
   {
     Tlist_item<elemType> *ptr = i1._at_front;
     while ( ptr ) 
     {
        CLEAR_WDT;
        push_back( ptr->value() );
        ptr = ptr->next();
     }
   }
/*********************************************************************************/
    void         reverse()
   {
     Tlist_item<elemType> *ptr = _at_front;
     Tlist_item<elemType> *prev = 0;
     _at_front = _at_end;
     _at_end = ptr;
     while ( ptr != _at_front )
    {
       CLEAR_WDT;
        Tlist_item<elemType> *tmp = ptr->next();
        ptr->next( prev );
        prev = ptr;
        ptr = tmp;
     }
    _at_front->next( prev );
   }
/*********************************************************************************/
    Tlist_item<elemType>*  find(elemType value)
   {
   Tlist_item<elemType> *ptr = _at_front;
   while ( ptr )
   {
     CLEAR_WDT;
      if ( ptr->value() == value )
      break;
      ptr = ptr->next();
   }
   return ptr;
   
   }
/*********************************************************************************/
    elemType     next(elemType elem)
   {
    if(_size==0)
      return 0;
    
    Tlist_item<elemType>* findelem = find(elem);
    if(findelem != NULL)
      return findelem->next()->value();
    else
      return 0;
   }
/*********************************************************************************/
    elemType     prev(elemType elem)
   {
    if(_size==0)
      return 0;
    
    Tlist_item<elemType>* findelem = find(elem);
    if(findelem != NULL)
      return findelem->prev()->value();
    else
      return 0;
   }
/*********************************************************************************/
    elemType     foreach_next()
   {
    if(_size==0)
      return 0;
    
    if ( interator++ > (_size-1) )
    {  
      interator = 0; 
      _at_interator =  _at_front;
      return 0;
    }
    Tlist_item<elemType> *tmp = _at_interator;
    _at_interator = _at_interator->next();
    return  tmp->value();
   }
/*********************************************************************************/
    void         set_interator(int value)
   {
    interator = value;
    if ( interator > (_size-1) )
    {  interator = 0; }
    
    Tlist_item<elemType> *tmp = _at_front;
    HOLD_WDT;
    for(int i=0; i<interator; i++)
    {  
      tmp = tmp->next();
    }
    CLEAR_WDT;
    _at_interator = tmp;
   }
/*********************************************************************************/  
    
private:  
/*********************************************************************************/
   void insert_all( const Tlist<elemType> &rhs )
  {
     Tlist_item<elemType> *pt = rhs._at_front;
     while ( pt ) 
     {
       CLEAR_WDT;
        this->push_back( pt->value() );
        pt = pt->next();
     }
  }
/*********************************************************************************/
   void bump_up_size()
  {
      ++(this->_size); 
  }
/*********************************************************************************/
   void bump_down_size()
  {
  --(this->_size); 
  }
  
  
/*********************************************************************************/
  int                   interator;
  Tlist_item<elemType>  *_at_front;
  Tlist_item<elemType>  *_at_end;
  Tlist_item<elemType>  *_at_interator;
  int                   _size;
  
};
//=============================================================================
// TObject declaration
//====================================
class TObject
{
    friend class TObjectTask;
    friend class TObjectCalibr;
public:
       static TRtcTime *GetRtcTime(string &date, string &time);
       static void   SetDataTime(TDateTime &dt, string &date, string &time);
       static bool   Save_ParamsInInfo(Data_Params  *Params);
       static bool   VerifyMemory(Byte const* DestAddr, Byte* ScrAddr,Word Len); 

       static bool   RestoreMemParams(void); 
       static void   RestoreDefault(void);  
       static bool   WriteByteToFlash(Byte data, Byte const *Addr);
       static bool   WriteByteToFlash_MY(Byte const * PageAddr, Word size_wr, Byte* ram_buf);
       static bool   WriteWordToFlash( Word data, Word const *Addr );
       static bool   WriteBlockToFlash( Word* data, Word const *Addr );
       
       static void   Soft_Reset(void);
       static void   DirectJump(Word Addr);
       
       static const  Data_Params *PtrToConstants;
       static const  Boot_Params *PtrToBootConstants; 
       static void   WriteHexBufToPage(Byte const * PageAddr, Byte *HexBuf, Word BufLen);
       static void   WriteLongHexBufToPage(Byte const * PageAddr, Byte *HexBuf, Word BufLen);
       static Word   ReturnFlashCRC(Byte const * PageAddr, Word BufLen);
       static Word   ReturnLongFlashCRC(Byte const * PageAddr, Word BufLen);
       static void   WriteBufToPage(Byte const * PageAddr, Byte *Buf, Word BufLen);
       static void   SimpleWriteBufToPage(Byte const * PageAddr, Byte *Buf, Word BufLen); 
       static bool   ClrFlashSegment(Byte const *SegAddr);
       
private: 
 

       static void  UnlockFlash (void);
};
//Data_Params const *TObject::PtrToConstants;
//Boot_Params const *TObject::PtrToBootConstants;
//=============================================================================
// TObjectTask declaration
// Статический класс 
//====================================
class TObjectTask
{
public:
       static bool   Save_TaskConfigInFlash(TTaskListConfig *taskListConfig);
       static void   RestoreDefault(void);      
       static bool   RestoreMemParams(void);
       static Word   CalcCRC(TTaskListConfig const*);       
       static        TTaskListConfig const *TaskListConfig;       
};
//TTaskListConfig const *TObjectTask::TaskListConfig; 
//=============================================================================
// TObjectCalibr declaration
// Статический класс 
//====================================
class TObjectCalibr
{
public:
       static bool   Save_CalibrConfigInFlash(TCalibrListConfig *calibrListConfig);
       static void   RestoreDefault(void);      
       static bool   RestoreMemParams(void);
       static Word   CalcCRC(TCalibrListConfig const*);       
       static        TCalibrListConfig const *CalibrListConfig;       
};
// TCalibrListConfig const *TObjectCalibr::CalibrListConfig;  
//=============================================================================
// TDispatcher declaration
//====================================
class TDispatcher
{
public:
     TDispatcher();
    ~TDispatcher();
     
     void CreateProcess(PFV Process);
     void Scheduler();
private: 
     Tlist<PFV> PocessList;       
};
//=============================================================================
// TBuffer declaration
//====================================
class TBuffer
{
public:         
        TBuffer(Word Size):DataLen(0),MaxSize(Size)
        {
          //HOLD_WDT;
          CLEAR_WDT;
          BUF = new Byte[Size];
          CLEAR_WDT;
        }
       ~TBuffer()
        { 
          delete[] BUF;
        }
        
        Byte    *BUF;
        Word     MaxSize;
        sWord    DataLen;

        static Word   Crc16(Byte * pcBlock, Word len);
        static Word   Crc(Byte const * pcBlock, Word len);
        static Word   AppCrc(DWord StartAddr, Word BlockCNT);
        static void   XOR_Crypt(Byte* in, Word inlen, string pass);
        static Word   ByteToShort(Byte* Buf);
        static Word   DeByteToShort(Byte* Buf);
        static DWord  ByteToLong(Byte* Buf);
        static void   ClrBUF(Byte *BUF, Word Len);
        static sWord  HexToByte(Byte *ptr);
        static Word   HexToWord(Byte *ptr);
//        static DWord  HexToDWord(Byte *ptr);
 
        static void   ByteToHex(Byte *HEX, Byte BYTE);
        static void   WordToHex(Byte *HEX, Word WORD);
        static void   DWordToHex(Byte *HEX, DWord DWORD);
        
        //static Byte    FloatToString(Byte *ReturnBUF, float Val, Byte AllSymb=6, Byte AfteComma=2);//string
        static string  FloatToString(float Val, Byte AllSymb = 0, Byte AfteComma = 2);
        //static Byte    IntToString(Byte *ReturnBUF, int Val, Byte AllSymb=4);       
        static string  IntToString(int Val,Byte AllSymb=0);
        //static string  NewIntToString(int Val);
        static string  WordToString(Word Val, Byte AllSymb = 0);
        //static Word    NewStringToInt(string Val);
        static string  LongToString(long Val,Byte AllSymb=0);     
        
        static string  DateTimeToString(TRtcTime *datetime);
        static TRtcTime       *StringToDateTime(string strdt);
        
        static Word   CopyString(Byte* DestStr, Byte* SrcStr);
        static bool   StrCmp(Byte const* Str1, Byte const* Str2);
        static void   Split(string& InStr, string separator, Tlist<string>& results, int Len = -1);
        static void   Split(Byte *BUFFER, Word Size, string separator, Tlist<string>& results, Word Len = -1);
        
        static int     find(TBuffer *BUFFER, string str);
        static int     find(Byte *BUFFER, Word Len, string str);
        static int     find_first_of(TBuffer *BUFFER, Byte simb);
        static int     find_first_of(Byte *BUFFER, Word Len, Byte simb);
        static int     find_last_of(TBuffer *BUFFER, Byte simb);    
        static Word    clear_all_nulls(char *_ptr, Word _size);
        
private:
};
//=============================================================================
// TTimer declaration
//==================================== 
class TTimer
{ 
public:
  TTimer(Byte TimNum);
  ~TTimer();

  virtual bool  SetTimer(Byte TimNamber, DWord Value);
  virtual void  DecTimer(Byte TimNamber);
  virtual sDWord  GetTimValue(Byte TimNamber); 
  
  static void   wait_1ms(Word cnt); 
  static void   wait_1us(Word cnt);   
private:
  DWord* Timer;
  Byte   TimerCNT;
};
//=============================================================================
// INT64 COUNTER
//==================================== 
class Word64
{
public:
  Word64()
  {
    this->HI = 0;
    this->LO = 0;
  }
  ~Word64(){}
  void Inc()
  {
    DWord TmpVal=this->LO;
    this->LO++;
    if(this->LO<TmpVal) this->HI++;
  }
  Word64 Add(DWord Value)
  {
    DWord TmpVal=this->LO;
    LO+=Value;
    if(this->LO<TmpVal) 
      this->HI++;
    return *this;
  }
  bool Compare(Word64 *VAL)// true VAL>=   false <
  {
    if(this->HI > VAL->HI)
    
      return false;
    else
      if(this->LO >= VAL->LO)
        return false;
    return true;
  }
  Word64& operator=( const Word64 &newval ) 
  {
    this->HI = newval.HI;
    this->LO = newval.LO;
    return *this;
  }
  
  
private:
DWord HI,LO;
};
//=============================================================================
// TTimer declaration
//==================================== 
class TFastTimer
{ 
public:
  TFastTimer(Byte CNT, Word64* GLT)
  {
    //HOLD_WDT;
    TimerCNT=CNT;
    Timer = new Word64[TimerCNT]; 
    for(Word i = 0; i < TimerCNT; i++)
    { 
      CLEAR_WDT;
      GlobalTimer = GLT;      
      SetTimer(i,0);
    }

  } 
  ~TFastTimer(){}
  bool  SetTimer(Byte TimNam, DWord Value)
  {
      bool flag     = false;
      //Word old_SR   = __get_SR_register();
      //if(old_SR & GIE)
      //{flag = true;}
      
      __disable_interrupt(); 
      bool state;
      if (TimNam<TimerCNT)
      {
         Timer[TimNam] = *GlobalTimer;
         Timer[TimNam].Add(Value);
         state = true;
      }
      else
      {  state = false;}
      if(flag)
      {__enable_interrupt(); }
      return state;
  }
  bool  GetTimValue(Byte TimNam) //0 - Timer сработал 1 - ещё считает
  {
      bool flag     = false;
//      Word old_SR   = __get_SR_register();
//      if(old_SR & GIE)
//      {flag = true;}
    __disable_interrupt(); 
      bool Val;
      if (TimNam<TimerCNT)
      {  Val = GlobalTimer->Compare(&Timer[TimNam]);}
      else
      {  Val = false;}
      if(flag)
      {__enable_interrupt();} 
      return Val;
  }
  
  
private:
  Word64* GlobalTimer;
  Word64* Timer;
  Byte   TimerCNT;
};


//=============================================================================
// TObjectFunctions declaration
// Статический класс 
//====================================
class TObjectFunctions
{
public:
       static void   ReleON(int rele);
       static void   ReleOFF(int rele);

       static TFastTimer ObjectFunctionsTimer;    
       
};
//=============================================================================
// TComPort declaration  Базовый интерфейсный класс
//==================================== 
class TComPort:public TTimer
{
public:
        TComPort();
       ~TComPort();
      
	virtual HANDLE  open(Port port, DWord baudRate, DWord ReadIntervalTimeout);
        bool            close();
        void            SetTimeOuts(COMMTIMEOUTS timeouts);
        void            GetTimeOuts(COMMTIMEOUTS &timeouts); 
        bool            TestStxFlag(void);
        void            ClrStxFlag(void);   
        
       // virtual sWord   Recv(Byte *Buf, Word MaxLen);
        virtual sWord   Recv(Byte *Buf, Word Len, Word MaxLen);
        virtual sWord   Recv(Byte *Buf, Word Len, Word MaxLen, Word timeout, bool separate);
        virtual sWord   RecvTo(Byte *Buf, Word MaxLen, Byte StopSymbol);
        virtual sWord   Send(Byte *Buf, Word Len);  
        virtual sWord   Send(Byte *Buf, Word Len, bool separate);    
        
        
        void            PushByteToDATA_Rx(Byte Data);
        sWord           PopByteDATA_Rx();        
        bool            Busy();        
        void            ClearRxBUF();
        void            ClrByteIndex(void);
        bool            enabled;  
        Word            CRC_SUM;
        
        virtual bool    SendByte(Byte byte);        
        virtual bool    RecvByte(Byte *byte); 
        virtual         bool    CloseHandle(HANDLE commPortHandle); 
        
        COMMTIMEOUTS     time_outs;    
           
        void            SetTxDataByte(Byte data);
        Byte            GetRxDataByte();
        void            SetTxIntEnable(bool state);
        
        TFifo<Byte>      *DATA_Rx;
        Byte             DATA_Tx;  
                
protected:
        virtual         void    RX_Ready(void)  = 0;
        virtual         void    TX_Ready(void)  = 0;            
        virtual         void    RX_Disable(void)  = 0; 

        void            InitPort(DWord BAUDRATE);
        bool            GetTxIntFlag(void);
        void            SetTxIntFlag(bool state);
        
        void            SetRxIntEnable(bool state);

        Short           GetIntVector();

        bool            UsartIsBusy(void);
        
        bool             DataReceiving;                           
        HANDLE           commPortHandle;
        Word             ByteIndex; 
        
 
        
private:
                 bool             StxFlag; 
                 Byte             RX_PORT;
                 Byte             TX_PORT; 
        volatile Byte             *CTL_REGISTER0;
        volatile Byte             *CTL_REGISTER1;
        volatile Byte             *MCTL_REGISTER;
        volatile Byte             *BAUD_RATE0;
        volatile Byte             *BAUD_RATE1;
        
        volatile Byte             *BUS_DIR_REGISTER;
        volatile Byte             *BUS_ENABLE_REGISTER;         
        volatile Byte             *BUS_REN_REGISTER;
        volatile Byte             *BUS_OUT_REGISTER;       
        
        volatile Byte             *TX_BUF;
        volatile Byte             *RX_BUF; 
        volatile Byte             *IE_REGISTER;
        volatile Byte             *IFG_REGISTER;
        volatile Byte             *STATUS_REGISTER;   
        volatile Short            *IVECTOR_REGISTER;
};
//=============================================================================
// 
// 
//==================================== 
class TDateTime
{ 
public:
  TDateTime(); 
  TDateTime(TRtcTime time);
 ~TDateTime();
  
  TRtcTime  GetTime(void);
  void      SetTime(TRtcTime dateTime);
  void      IncSecond(void);
  DWord     GetGlobalSeconds(void);
  long      GetFullGlobalSecond(void);
  TRtcTime  DateTime;
  void      SetSynchAct(void);
  bool      GetSynchAct(void);
  DWord     GetAbsTime(void);
private:
  bool        TimeSynchIsActual;
  DWord       GlobalSeconds;
  
  void IncMinute(void);
  void IncHour(void);
  void IncDay(void);
  void IncMonth(void);
  bool IsLeapYear(Word Year); 
};
//=============================================================================
// TCmd_msg declaration
// базовый класс для текстовых команд в протоколе ТМ
//==================================== 
class TCmd
{ 
public:      
   virtual          ~TCmd()  { } 
   virtual TCmd*      GET_Message(void* p1 = 0,void* p2 = 0) = 0;   
   virtual TCmd*      CMD_Execute(void* p1 = 0,void* p2 = 0) = 0;
   virtual TCmd*      CMD_Recv( void* p1,void* p2 )          = 0;
   virtual TCmd*      CMD_Send( void* p1,void* p2 )          = 0; 
   virtual Byte       GetFase(void)                          = 0;
   virtual void       SetFase(Byte NewFase)                  = 0;
   virtual bool       NeedReset(void)                        = 0;
   virtual bool       NeedAnswer(void)                       = 0;
   
protected:     
          Word      GetAddress(); 
};
//=============================================================================
// 
//==================================== 
class TPt
{
public:
  TPt(int pType, int eName)
  {
     this->PType = pType;
     this->PName = eName;
  }
  
  ~TPt()
  {
    Value.clear();
  }
  
  int          PType;
  int          PName;
  Tlist<string>  Value;
};
//=============================================================================
// 
//==================================== 
class TEt
{
public:
  TEt(int eType, int eName)
  {
     this->EType = eType;
     this->EName = eName;
  }
  
  ~TEt()
  {
    Ptypes.clear();
  }
  
  int          EType;
  int          EName;
  Tlist<TPt*>  Ptypes;
};
//=============================================================================
// 
//==================================== 
class TCmdMessage
{
public:
  TCmdMessage():DateTime(NULL),NeedCmdAnswer(false),CMD("Data"), Addr(""),Password(""),State(" "),WithQuotes(true)
  {}
  
  ~TCmdMessage()
  {
    Data.clear();
  }
  
  string         CMD;
  string         Password; 
  string         Addr;  
  string         State;  
  TRtcTime      *DateTime;

  bool           NeedCmdAnswer;
  bool           WithQuotes;  
  
  Tlist<string>  Data;  
};

//=============================================================================
//=============================================================================
  class TSimplPtype
  {
  public: 
    TSimplPtype(Byte Pt, Byte Pn):Ptype(Pt),Pname(Pn)
    {}
   ~TSimplPtype(){}
   
    Byte Ptype;
    Byte Pname;   
  };  
//=============================================================================
//=============================================================================
  class TMamStruct
  {
  public: 
   TMamStruct()
    {
       Max=Min=Aver    = 0;      
       Min     = 0;
       Aver    = 0;
       Value   = 0;  
       Divider = 0;
    }
  ~TMamStruct(){}
  
  /* TMamStruct &operator=( const TMamStruct& )
   {
     
     return *this;
   }*/
  
   void CalcMinAverMax(float newVal)
   {
     if(Divider == 0)
     {
       this->Max = newVal;
       this->Min = newVal;
     }
     Divider++;
     if(newVal > this->Max)
     {  this->Max = newVal;}
     if(newVal < this->Min)
     {  this->Min = newVal;} 
     
     this->Aver   = ( (float)((this->Aver*(Divider-1))  + newVal )) / ((float)Divider); 
     this->Value  =  newVal;     
   }
   void ClrDivider()////////////////////////
   {  
     this->Divider  = 0;
     
   /*  this->Aver  = 0;  
     this->Max  = 0; 
     this->Min  = 0; */
   }
   
   void ClrValue()////////////////////////
   {  
     this->Divider  = 0;
     this->Aver     = 0;  
     this->Max      = 0; 
     this->Min      = 0;
     this->Value    = 0;     
   }
   
   float Max;    
   float Min;
   float Aver;
   float Value; 
  private:  
   long  Divider;
  }; 
  
//=============================================================================
// 
//==================================== 
class IInput
{
public:
  
  virtual ~IInput() { } 
  virtual Byte GetValue(void) = 0; 
  
  bool    IsVirtual;    
  bool    TimerFlag;
private:
};
//=============================================================================
// 
//==================================== 
class TInput:public IInput
{
public:
  TInput(Byte *port, Byte bit)
  {
    BIT   =   bit;
    PORT  =   port;//in
    port  +=  4;
    *port &= ~BIT; //dir
    IsVirtual = false;
    TimerFlag = false;
    Inv       = false;
  }
  TInput(Byte *port, Byte bit, bool inv)
  {
    BIT   =   bit;
    PORT  =   port;//in
    port  +=  4;
    *port &= ~BIT; //dir
    IsVirtual = false;
    TimerFlag = false;
    Inv       = inv;
  }
  Byte GetValue()
  { return ((bool)( (*PORT)&BIT ))^Inv; }
  
private:
  Byte  BIT;
  Byte *PORT;
  bool  Inv;
};
//==================================== 
class TMkkoInput:public IInput
{
public:
  TMkkoInput(Byte *port, Byte bit, Word porog)
  {
    BIT   =   bit;
    PORT  =   port;//in
    port  +=  4;
    *port &= ~BIT; //dir
    _porog = porog;
    _value = '0';
    _count = 0;
    _counter = 0;
    _lastcounter = 0;
    _clbKoef = 0;
    Index = 0;
  }
  
  void DetectValue(void)
  {
    if( !GetBoolValue() ) 
    {++_counter;}
    else
    { _counter = _counter;}
    if( ++_count >= 1000)
    {
      _counter +=_clbKoef;
      if(_counter > 1128) _counter = 0;
      if( _counter >= _porog)
      {  _value = 1;}
      else
      {  _value = 0;}
       
      _lastcounter = _counter;
      _count = 0;
      _counter = 0;
    }
  }
  
  void SetClb(sByte clb)
  {
    _clbKoef = clb;
  }
  
  string GetCString(void)
  {
    return TBuffer::WordToString(_lastcounter);
  }
  
  Byte GetValue() { return _value; }
  
  Byte Index;
  
private:
  
  bool GetBoolValue() { return ((bool)( (*PORT)&BIT )); }
  
  Byte  BIT;
  Byte *PORT;
  
  Byte _value;
  Word _count;
  Word _counter;
  Word _lastcounter;
  Word _porog;
  
  sByte _clbKoef;
};
//=============================================================================
//=============================================================================
  class IParams
  {
  public:
    IParams(Byte Pt, Byte Pn,bool IsVisible):Pt(Pt, Pn),Visible(IsVisible),size(0),Period(0)
    {}
    
    ~IParams()
    {
      for(int i = 0; i < Params.size(); i++)
      {
        CLEAR_WDT;
        delete Params[i];
      }
      Params.clear();
    }
    
    string GetCfgString(void)
    {
      return string("\r\n>,"+
                  TBuffer::WordToString(Pt.Ptype)+','+
                  TBuffer::WordToString(Pt.Pname)+','+     
                  TBuffer::WordToString(size)+','+
                  TBuffer::WordToString(Period));
    }
    string GetValueString(void)
    {
      string ReturnStr="";
      if(Visible)
      {
         ReturnStr = "\r\n>,"+
                        TBuffer::WordToString(Pt.Ptype)+','+
                        TBuffer::WordToString(Pt.Pname)+',';      
         for( int i=0; i < Params.size();i++)
         {
            CLEAR_WDT;
            TMamStruct *CurrMam = Params[i];
            ReturnStr += TBuffer::FloatToString(CurrMam->Aver)+','+        
                         TBuffer::FloatToString(CurrMam->Min)+','+      
                         TBuffer::FloatToString(CurrMam->Max)+',';       
           CurrMam->ClrDivider();
         }
      }
        return ReturnStr;
    }
    TSimplPtype Pt;
    Byte size;
    Word Period;
    bool Visible;
    Tlist<TMamStruct*> Params;
  };
//=============================================================================
// 
//==================================== 
class IDatchik
{
public:
  IDatchik()
  { Ptype       = NULL;}
  virtual ~IDatchik(){}
  virtual string GetStringValue(void) = 0;
  virtual string GetStringCfgValue(void) = 0;
  
  TPtype     *Ptype;
 // Byte       *SetDataPtr;
 // Byte       *CfgPtr;
};
//=============================================================================
// 
//==================================== 
class TDatchik:public IDatchik
{
public:
  TDatchik(TPtype* ptype, TInput* pDryInput, Byte pPrPlace);
  TDatchik(TPtype* ptype, TInput* pDryInput, Byte pPrPlace, bool inv, bool StopWath);
 ~TDatchik();
 
   string GetStringValue(void);
   string GetStringCfgValue(void);

   Byte   GetValue(void);
   void   SetOffInput(void);
   void   SetONInput(void);
   void   SetManualState(bool Ena,bool Inv);   
   
   Byte        PrPlace;   
   Byte        Inversia;
   Byte        StopWathSendOFF;//Признак "Прикратить наблюдение за данным каналом, читаться отсутствием события"

   Byte        Value;  
   Byte        HValue;  
private:  
   TInput      *DryInput;
};
//==================================== 
class TAutomatDevice:public IDatchik
{
public:
  TAutomatDevice(TPtype* ptype, Byte pn, TMkkoInput **glInp, Byte ia, Byte ib, Byte ic, Byte oa, Byte ob, Byte oc);
  ~TAutomatDevice();
  
   Byte   GetValue(void);
  
   string GetStringValue(void);
   string GetStringCfgValue(void);

  Byte Value;  
  Byte HValue;
  
private: 
  
  Byte        _parName;  
  
  Byte _type;
  Byte _outState;
  
  TMkkoInput *_inA;
  TMkkoInput *_inB;
  TMkkoInput *_inC;
  
  TMkkoInput *_outA;
  TMkkoInput *_outB;
  TMkkoInput *_outC;
};
//=============================================================================
// 
//==================================== 
class TPtype
{
public:
  TPtype(TEquipment *EType, Byte pType, Byte pName, Byte pSize, Word pPer)
  {
    this->Etype  = EType;
    this->PType  = pType;
    this->PName  = pName;
    this->Size   = pSize;   
    this->Period = pPer;
    this->Modify = false;    
   }
  
  TPtype(TEquipment *EType, Byte pType, Byte pName, Word pPer)
  {
    this->Etype  = EType;
    this->PType  = pType;
    this->PName  = pName;
    this->Modify = false;
    this->Size   = 1;   
    this->Period = pPer;
   }
  
   ~TPtype()
   {
      for(int i = 0; i < Datchiks.size(); i++)
      {
        CLEAR_WDT;
        delete Datchiks[i];
      }
      Datchiks.clear();
   }
   TEquipment      *Etype;
   Byte             PType;
   Byte             PName;
   Byte             Size;
   Word             Period;   
   bool             Modify;  
   Tlist<IDatchik*> Datchiks;
};
//=============================================================================
// 
//==================================== 
class TEquipment
{
public:
  TEquipment(Byte eType, Byte eName)
  {
     EType = eType;
     EName = eName;
  }
  
 ~TEquipment()
  {
    for(int i = 0; i < Ptypes.size(); i++)
    {
      delete Ptypes[i];
    }
    Ptypes.clear();
  }
  
  Byte           EType;
  Byte           EName;
  Tlist<TPtype*> Ptypes;
};
//=============================================================================
/*  class TString
  {
  public: 
    TString();
    TString(TString const &str);
    TString(string const &str);
   ~TString();

    const char *c_str(void);
    Word        size(void);
    void        clear();
    void        clear_all_nulls();
    bool        empty();
    
    TString    &operator=( const TString& newstr );
    
  private:
    Word        _size;        
    char        *_ptr;
  };  */
//=============================================================================
  
extern Word64                  SecCount;           
extern long                    RtcTimerDivider;
extern TDateTime               SystemTime;



extern const Data_Params  PARAMS;
extern const Data_Params  TMP_PARAMS;
extern const TTaskListConfig  TASK_PARAMS;
extern const TTaskListConfig  TMP_TASK_PARAMS;
//=============================================================================
//=============================================================================
#endif


extern const Data_Params  PARAMS;
extern const Data_Params  TMP_PARAMS;
extern const TTaskListConfig  TASK_PARAMS;
extern const TTaskListConfig  TMP_TASK_PARAMS;


#define ADC_Channel_0                              ((uint8_t)0x00)
#define ADC_Channel_1                              ((uint8_t)0x01)
#define ADC_Channel_2                              ((uint8_t)0x02)
#define ADC_Channel_3                              ((uint8_t)0x03)
#define ADC_Channel_4                              ((uint8_t)0x04)
#define ADC_Channel_5                              ((uint8_t)0x05)
#define ADC_Channel_6                              ((uint8_t)0x06)
#define ADC_Channel_7                              ((uint8_t)0x07)
#define ADC_Channel_17                             ((uint8_t)0x11)


#define ADC_SampleTime_4Cycles                     ((uint8_t)0x00)
#define ADC_SampleTime_9Cycles                     ((uint8_t)0x01)
#define ADC_SampleTime_16Cycles                    ((uint8_t)0x02)
#define ADC_SampleTime_24Cycles                    ((uint8_t)0x03)
#define ADC_SampleTime_48Cycles                    ((uint8_t)0x04)
#define ADC_SampleTime_96Cycles                    ((uint8_t)0x05)
#define ADC_SampleTime_192Cycles                   ((uint8_t)0x06)
#define ADC_SampleTime_384Cycles                   ((uint8_t)0x07)