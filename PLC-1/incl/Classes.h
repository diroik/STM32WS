#ifndef CLASSES_H
#define CLASSES_H
//=============================================================================
#include <Classes_def.h>
#include <type_define.h>
//#include <flash_map.h>
#include <string>
#include <list>
#include <vector>
#include <deque>
#include <map>
#include <params_def.h>
//******************************************************************************
#define ADDR_BOOTLOADER   ((uint32_t)0x08000000)
#define OFFSET_BOOTLOADER ((uint32_t)0x0)
#define MAX_CYCLE 1000
#define __HAL_SPI_GET_FLAG_MY(__HANDLE__, __FLAG__) ((((__HANDLE__)->SR) & (__FLAG__)) == (__FLAG__))
//******************************************************************************
class TDateTime;
class TBuffer;
class TTimer;   
//******************************************************************************
using namespace std; 
//******************************************************************************
 //=============================================================================
//=============================================================================
// Class_Templates
//====================================
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
   inline Type pop()
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
   inline void push(const Type &elem)
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
   inline bool empty() const
   {
     return (size == 0);
   }
//=============================================================================
   inline bool full() const
   {
     return (size == max_size);
   }
//=============================================================================
   void clear(void)
   {
      while(size)
      {
        Buf[size-- -1] = 0;
        CLEAR_WDT;
      }
      front_ind = end_ind = 0;
   }
//=============================================================================
  inline Type operator[]( int ix ) const
  {
    Type tmp = (Type)0;
    if (size > 0 && ix < size)
    {
      unsigned int div_index = (front_ind + ix);
      unsigned int calc_index = div_index >= max_size ? (div_index - max_size) : div_index;
      tmp = Buf[calc_index];
    }
    return tmp;
  }
//=============================================================================
  unsigned int        size;   
  
private: 
  unsigned int        max_size;
  Type                *Buf;
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
   
    while ( plist && ( plist->value() == value) )//???? ? ?????? ??????
    {
      CLEAR_WDT;
      plist = plist->next();
      pop_front();
      ++elem_cnt;
    }
    
    if ( ! plist )  //???? ?????? ???? ?? ??????? ???-?? ????????
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
public:
  
       static bool   Save_ParamsInInfo(Data_Params  *Params);
       static bool   VerifyMemory(  Byte const* DestAddr, Byte* ScrAddr, Word Len  );

       static bool   RestoreMemParams(Data_Params *ptr); 
       static void   RestoreDefault(void);  
       static void   Soft_Reset(void);
       
       static bool   WriteByteToFlash(Byte const * PageAddr, Word size_wr, Byte* ram_buf);
       static void   WriteHexBufToPage( Byte const* PageAddr, Byte *HexBuf, Word BufLen );
       static Word   ReturnFlashCRC(  Byte const* PageAddr,  Word BufLen  );
       static Word   ReturnLongFlashCRC(  Byte const* PageAddr, Word BufLen  );
       static void   WriteBufToPage(Byte const*PageAddr, Byte *Buf, Word BufLen);  
       
private: 
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
//        sWord    DataLenRX;
//        sWord    DataLenTX;

        static Word   Crc(Byte *pcBlock, Word len);
        static Byte   CrcByte(Byte const* pcBlock, Word len);
        static Byte   CycleCrcByte(Byte const* pcBlock, Byte len, int cur);

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
        static Word   CopyBuffer(TBuffer* Dest, TBuffer* Src);
        static Word   CopyBuffer(Byte* Dest, Byte* Src, Word DataLen);
        static bool   StrCmp(Byte const* Str1, Byte const* Str2);
        static void   Split(string& InStr, string separator, vector<string>& results, int Len = -1);
        
        static int     find_first_of(TBuffer *BUFFER, Byte simb);
        static int     find_first_of(Byte *BUFFER, Word Len, Byte simb);
        static int     find_last_of(TBuffer *BUFFER, Byte simb);     
        static Word    clear_all_nulls(char *_ptr, Word _size);
        
        static bool    ParsingAnswer( Byte *BUF, Word Len);
        static int     GetDataFroMessage(Byte *Buffer, Word DataLen);
        
        static int     ByteStuffing(Byte *BUF, Word Len, Byte flg, Byte nflg)
        {
            Word ret = Len;
            Word len = 0;
            CLEAR_WDT;
            for(int i = 0; i < Len; i++)
            {
              if(BUF[i] == flg)
                len++;
            }
            if(len > 0)
            {
              CLEAR_WDT;
              Byte *TMP_BUF = new Byte[Len+len];
              CLEAR_WDT;
              int newind = 0;
              for(int i = 0; i < Len; i++)
              {
                if(BUF[i] == flg)
                {
                  TMP_BUF[newind++] = flg;
                  TMP_BUF[newind++] = nflg;
                }
                else
                {
                  TMP_BUF[newind++] = BUF[i];
                }
              }
              CLEAR_WDT;
              for(int i = 0; i < newind; i++)
              {
                BUF[i] = TMP_BUF[i];
              }
              delete[] TMP_BUF;
              CLEAR_WDT;
              
              ret = newind;
            }
            return ret;
        }
        
        static int DeByteStuffing(Byte *BUF, Word Len, Byte flg, Byte nflg)
        {
            Word ret = Len;
            CLEAR_WDT;
            Byte *TMP_BUF = new Byte[Len];
            CLEAR_WDT;
            int newind = 0;
            for(int i = 0; i < Len; i++)
            {
              TMP_BUF[newind++] = BUF[i];
              if(BUF[i] == flg && i < (Len-1) && BUF[i+1] == nflg){
                i++;
              }
            }
            CLEAR_WDT;
            for(int i = 0; i < newind; i++)
            {
              BUF[i] = TMP_BUF[i];
            }
            delete[] TMP_BUF;
            CLEAR_WDT;
            ret = newind;
            return ret;
        }
        
        
        
private:
        static Byte   CheckSum(Byte *BUF, Word Len);
  
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
// INT64 COUNTER
//==================================== 
class Word64
{
public:
  Word64()
  {
    value = 0;
  }
  ~Word64(){}
  inline void Inc()
  {
    value++;
  }
  
  inline Word64 Add(DWord Value)
  {
    value += Value;
    return *this;
  }
  
  inline bool Compare(Word64 *VAL)// true VAL>=   false <
  {
    return value == VAL->value;
  }
  
  inline Word64& operator=( const Word64 &newval ) 
  {
    value = newval.value;
    return *this;
  }
  
  
private:
  long long value;
};
//=============================================================================
// TUsartPort declaration  Базовый интерфейсный класс
//==================================== 
class TSpiPort
{
public:
        TSpiPort();
       ~TSpiPort();
      
	virtual HANDLE  open(SPI_TypeDef* SPIx, DWord baudRate, DWord ReadIntervalTimeout);
        bool            close();
        bool            enabled; 
        
        void            read_register();
        void            write_register();
        Data_Params    *params;
        
protected:
  
        virtual  bool           CloseHandle(HANDLE spiPortHandle); 
        volatile HANDLE         spiPortHandle;
};
//=============================================================================
// TUsartPort declaration  Базовый интерфейсный класс
//==================================== 
class TUsartPort:public TTimer
{
public:
        TUsartPort();
       ~TUsartPort();
      
	virtual HANDLE  open(Port comport, DWord baudRate, DWord ReadIntervalTimeout);//USART_TypeDef* USARTx
        bool            close();
        void            SetTimeOuts(COMMTIMEOUTS timeouts);
        void            GetTimeOuts(COMMTIMEOUTS &timeouts); 
        bool            TestStxFlag(void);
        void            ClrStxFlag(void);   
        
        virtual sWord   Recv(Byte *Buf, Word Len, Word MaxLen);
        virtual sWord   RecvTo(Byte *Buf, Word MaxLen, Byte StopSymbol);
        virtual sWord   Send(Byte *Buf, sWord Len, bool separate = true);    
        
        
        void            PushByteToDATA_Rx(Byte Data);
        sWord           PopByteDATA_Rx();        
        bool            Busy();        
        void            ClearRxBUF();
        void            ClrByteIndex(void);
        bool            enabled;  
        Word            CRC_SUM;
        
        virtual  bool    SendByte(Byte byte);        
        virtual  bool    RecvByte(Byte *byte); 
        virtual  bool    CloseHandle(HANDLE usartPortHandle); 


        Byte            GetRxDataByte();
        void            SetTxIntEnable(bool state);        
        
        COMMTIMEOUTS     time_outs;    
        Byte             CntOfAck;
        
        TFifo<Byte> *DATA_Rx;
        volatile Byte           DATA_Tx; 
        volatile Byte             *TX_BUF_USART;
        volatile Byte             *RX_BUF_USART;

        Byte                      State_Usart;
        bool                      GetTxIntFlag();
        
protected:
        virtual         void    RX_Ready(void)  = 0;
        virtual         void    TX_Ready(void)  = 0;            
        virtual         void    RX_Disable(void)  = 0; 

        void            InitPort(DWord BAUDRATE);  
        void            SetTxIntFlag(bool state);
        void            SetRxIntEnable(bool state);
        void            SetTxDataByte(Byte data);
        bool            UsartIsBusy();
        
        volatile bool         DataReceiving;                           
        volatile HANDLE       usartPortHandle;
        volatile Word         ByteIndex; 
        

        
private:
        volatile DWord            BaudRate;
        volatile bool             StxFlag; 
 };
//=============================================================================
// TUsartPort0 declaration
// реализация СОМ порта процессора
//==================================== 
class TUsartPort0: public TUsartPort
{  
public:
  	TUsartPort0(Word IntBufSize);
       ~TUsartPort0();
               
         void     RX_Ready(void);
         
private:         
         void     TX_Ready(void); 
         void     RX_Disable(void);         
         static   void     DATA_RX_TX(void);   //USB    
};
//=============================================================================
// TUsartPort0 declaration
// реализация СОМ порта процессора
//==================================== 
class TUsartPort3: public TUsartPort
{  
public:
  	TUsartPort3(Word IntBufSize);
       ~TUsartPort3();
               
         void     RX_Ready(void);
         
private:         
         void     TX_Ready(void); 
         void     RX_Disable(void);         
         static   void     DATA_RX_TX(void);   //USB    
};
//=============================================================================
// TSpiPort0 declaration
// реализация
//==================================== 
class TSpiPort0: public TSpiPort
{  
public:
  	TSpiPort0();
       ~TSpiPort0();
               
};
//=============================================================================
#endif
