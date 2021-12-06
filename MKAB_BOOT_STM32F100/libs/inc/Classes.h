#ifndef CLASSES_H
#define CLASSES_H
//=============================================================================
//#include <init.h>
#include <Classes_def.h>
#include <type_define.h>
#include <string>
#include <list>
#include <vector>
#include <deque>
#include <map>
#include <params_def.h>

#define ADDR_BOOTLOADER   ((uint32_t)0x08000000)
#define OFFSET_BOOTLOADER ((uint32_t)0x0)


  class TDateTime;
  class TBuffer;
  class TTimer;   
 
  
//******************************************************************************
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
  int          PType;
  int          PName;
  vector<string>  Value;
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
  int          EType;
  int          EName;
  vector<TPt>  Ptypes;
};
//=============================================================================
// 
//==================================== 



  
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
   Tlist_item( elemType value ):_value(value),_next(0),_back(0)
   {
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
        bump_down_size() ;
        delete ptr;
     }  
   }
/*********************************************************************************/
   void         pop_back();
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
  
  
  
//=============================================================================
// TObject declaration
//====================================
class TObject
{
public:
       static void   SetDataTime( TDateTime &dt, string &date, string &time );
       static bool   Save_ParamsInInfo( Data_Params  *Params  );
       static bool   Save_ParamsInInfoA(  Boot_Params  *BootParams  );
       static bool   VerifyMemory(  Byte const* DestAddr, Byte* ScrAddr, Word Len  );

       static bool   RestoreMemParams(void); 
       static bool   RestoreBootParams(void);
       static void   RestoreDefault(void);  
       static void   RestoreBootDefault(void);
       static bool   WriteByteToFlash(  Byte data, Byte const*Addr );
       static bool   WriteWordToFlash( Word data, Word const*Addr );
       static bool   WriteByteToFlash_MY(Byte const * PageAddr, Word size_wr, Byte* ram_buf);
       static bool   WriteBlockToFlash( Word* data, Word const*Addr );
       
       static void   Soft_Reset(void);
       static void   DirectJump(DWord Addr);
       
       static const  Data_Params *PtrToConstants;
       static const  Boot_Params *PtrToBootConstants;      
       static void   WriteHexBufToPage( Byte const* PageAddr,              Byte *HexBuf, Word BufLen );
       static void   WriteLongHexBufToPage( Byte const* PageAddr, Byte *HexBuf, Word BufLen  );
       static Word   ReturnFlashCRC(  Byte const* PageAddr,               Word BufLen  );
       static Word   ReturnLongFlashCRC(  Byte const* PageAddr, Word BufLen  );
       
private: 
       static void  WriteBufToPage(Byte const*PageAddr, Byte *Buf, Word BufLen);  
       static void  UnlockFlash (void);
       static bool  ClrFlashSegment(Byte const*SegAddr);         
};

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

        static Word   Crc(Byte const* pcBlock, Word len);
        static Word   AppCrc(DWord StartAddr, Word BlockCNT);
        static void   XOR_Crypt(Byte* in, Word inlen, string pass);
        static Word   ByteToShort(Byte* Buf);
        static Word   DeByteToShort(Byte* Buf);
        static DWord  ByteToLong(Byte* Buf);
        static void   ClrBUF(Byte *BUF, Word Len);
        static sWord  HexToByte(Byte *ptr);
        static Word   HexToWord(Byte *ptr);
        static DWord  HexToDWord(Byte *ptr);
        static DWord  HexToDWord_(Byte *ptr); 
 
        static void   ByteToHex(Byte *HEX, Byte BYTE);
        static void   WordToHex(Byte *HEX, Word WORD);
        static string WordToHex(Word WORD);
        static string DWordToHex(DWord Value);        
        static void   DWordToHex(Byte *HEX, DWord DWORD);
 
        static Byte   FloatToString(Byte *ReturnBUF, float Val, Byte AllSymb=6, Byte AfteComma=3);//string
        static string FloatToString(float Val, Byte AllSymb = 0, Byte AfteComma = 2);
        static Byte   IntToString(Byte *ReturnBUF, int Val, Byte AllSymb=4);       
        static string IntToString(int Val,Byte AllSymb=0);
        static string WordToString(Word Val, Byte AllSymb = 2);
        static string LongToString(long Val,Byte AllSymb=0);        
        
        static Word   CopyString(Byte* DestStr, Byte* SrcStr);
        static bool   StrCmp(Byte const* Str1, Byte const* Str2);
        static void   Split(string& InStr, string separator, vector<string>& results, int Len = -1);
        
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
  virtual sDWord GetTimValue(Byte TimNamber); 
  
  static void   wait_1ms(Word cnt); 
  static void   wait_1us(Word cnt); 
  
private:
  DWord* Timer;
  Byte  TimerCNT;
};
//=============================================================================
// TObjectFunctions declaration
// —татический класс 
//====================================
class TObjectFunctions
{
public:
       static TTimer  ObjectFunctionsTimer;    
       
};

//=============================================================================
// TComPort declaration  Ѕазовый интерфейсный класс
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
        
        virtual  bool    SendByte(Byte byte);        
        virtual  bool    RecvByte(Byte *byte); 
        virtual  bool    CloseHandle(HANDLE commPortHandle); 


        Byte            GetRxDataByte();
        void            SetTxIntEnable(bool state);        
        
        COMMTIMEOUTS     time_outs;    
           
protected:
        virtual         void    RX_Ready(void)  = 0;
        virtual         void    TX_Ready(void)  = 0;            
        virtual         void    RX_Disable(void)  = 0; 

        void            InitPort(DWord BAUDRATE);
        bool            GetTxIntFlag(void);
        void            SetTxIntFlag(bool state);

        void            SetRxIntEnable(bool state);

        void            SetTxDataByte(Byte data);
        bool            UsartIsBusy(void);
        
        volatile bool         DataReceiving;                           
        volatile HANDLE       commPortHandle;
        volatile Word         ByteIndex; 
        
        TFifo<Byte> *DATA_Rx;
        volatile Byte         DATA_Tx; 
        volatile Byte             *TX_BUF;
        volatile Byte             *RX_BUF;
        
private:
        volatile DWord            BaudRate;
        volatile bool             StxFlag; 
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
  
  DWord GetGlobalSeconds(void);
  TRtcTime    DateTime;   
private:

  DWord       GlobalSeconds;
  
  void IncMinute(void);
  void IncHour(void);
  void IncDay(void);
  void IncMonth(void);
  bool IsLeapYear(Word Year); 
};



extern const Data_Params  PARAMS;
extern const Boot_Params  BOOT_PARAMS;
extern const Data_Params  TMP_PARAMS;
#endif
