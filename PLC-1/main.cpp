/**********************************************/
/*              TM 2019                       */
/*                                            */
/* fable2000@rambler.ru                       */
/* droik@techmonitor.ru                       */
/**********************************************/
#include <device.h>
#include <intrinsics.h>
#include <type_define.h>
#include <params_def.h>
#include <Init.h>
#include <Classes.h>
#include <rdso.h>
//******************************************************************************
//**  Global vareables
//******************************************************************************  
Word64                  SecCount;   
long long               MilSecCount;

Data_Params             *PtrToConstants;
Boot_Params             *PtrToBootConstants;

TBuffer                 RS485Buffer(MAX_BUF_LEN);         //
TBuffer                 PlcBuffer(MAX_BUF_LEN);         //
 
TDispatcher             OS_Dispatcher;

bool                    Rs485Led_Ready      = false;
sWord                   Rs485LedCounter     = 0;  
bool                    PlcLed_Ready      = false;
sWord                   PlcLedCounter     = 0;  

bool                    NeedReload    = false;

TFifo<TBuffer*>          FromPlcToRS485(10);
TFifo<TBuffer*>          FromRS485ToPlc(10);


TUsartPort0             RS485Port(8);
TUsartPort3             PlcPort(8);
TSpiPort0               SpiPort; 
//debug

//USART_TypeDef* USART1_DEB;
//USART_TypeDef* USART3_DEB;

//******************************************************************************
//**  Functions prototypes
//******************************************************************************
void InitRS485Objects(void);
void InitPlcObjects(void);
void InitSpiObjects(void);
void InitLedObjects(void);
//***************************
void RS485Process(void  *Par = NULL);
void PlcProcess(void    *Par = NULL);
void LedProcess(void    *Par = NULL);
//******************************************************************************
void      PushBufToPlc(TBuffer *Buffer);
TBuffer*  PopBufFromPlc(void);
void      PushBufToRS485(TBuffer *Buffer);
TBuffer*  PopBufFromRS485(void);
//******************************************************************************
Byte CalcNEr(Byte num1, Byte num2, Byte num3);
int DePack(TBuffer *Buffer);
int Pack(TBuffer *Buffer);
//******************************************************************************
//  Programm start
//******************************************************************************
int main( void )
{
  SYSTEM_INIT();
  //debug init
  //USART1_DEB = USART1;
  //USART3_DEB = USART3;
  //
  InitLedObjects();
  InitRS485Objects();
  InitSpiObjects();
  InitPlcObjects();
  
  OS_Dispatcher.CreateProcess(RS485Process);                
  OS_Dispatcher.CreateProcess(PlcProcess);                
  OS_Dispatcher.CreateProcess(LedProcess);      
  
  CLEAR_WDT; 
  __enable_interrupt();

  for(;;)
  {    
    OS_Dispatcher.Scheduler();                          
    CLEAR_WDT;
  } 
  
  //return 0;
}
//******************************************************************************
// Dispatcher Process Functions
//******************************************************************************
// RS485Process
//******************************************************************************
void RS485Process(void *Par)
{ 
  volatile static Byte Fase = 0;
  switch(Fase)
  {
    case RX_CMD:
      RS485Buffer.DataLen = RS485Port.Recv( RS485Buffer.BUF, PtrToConstants->DATA[BYTES_CNT], RS485Buffer.MaxSize );
      if(RS485Buffer.DataLen > PtrToConstants->DATA[MIN_BYTE_LEN])
      {
          Rs485LedCounter = RS485Buffer.DataLen;
          if(TBuffer::ParsingAnswer(RS485Buffer.BUF, RS485Buffer.DataLen))
          {
            RS485Buffer.DataLen = TBuffer::GetDataFroMessage(RS485Buffer.BUF, RS485Buffer.DataLen);
            if(RS485Buffer.DataLen > 0){
              Fase = TX_CMD;
              break;
            }
          }
          else{
            PushBufToPlc(&RS485Buffer);
          }
      }
      else if(RS485Buffer.DataLen == 0)
      {//timeout
        Fase = WAIT_MES;        
      }
      break;
      
    case WAIT_MES:
      {
        TBuffer* buff = PopBufFromPlc();
        if(buff != NULL)
        {
          TBuffer::CopyBuffer(&RS485Buffer, buff);
          delete buff;
          Fase = TX_CMD;
        }
        else{
          Fase = RX_CMD;
        }
      }
      break;
      
    case TX_CMD:
      if( RS485Port.Send(RS485Buffer.BUF, RS485Buffer.DataLen, false) > 0)
      {
        Rs485LedCounter = RS485Buffer.DataLen;
        Fase = RX_CMD;
      }
      break;
    
    default:
      Fase = RX_CMD;
      break;
  };
}
//******************************************************************************
void PushBufToPlc(TBuffer *Buffer)
{
  if(Buffer != NULL && Buffer->DataLen > 0 && !FromRS485ToPlc.full())
  {
    TBuffer *newBuff = new TBuffer(Buffer->DataLen);
    TBuffer::CopyBuffer(newBuff, Buffer);
    FromRS485ToPlc.push(newBuff);
  }
}
//******************************************************************************
TBuffer* PopBufFromPlc(void)
{
  if(!FromPlcToRS485.empty())
    return FromPlcToRS485.pop();
  else
    return NULL;
}
//******************************************************************************
// PLCProcess
//******************************************************************************
void PlcProcess(void *Par)
{ 
  volatile static Byte Fase = 0;
  switch(Fase)
  {
    case RX_CMD:
      PlcBuffer.DataLen = PlcPort.Recv( PlcBuffer.BUF, PtrToConstants->DATA[PLC_BYTES_CNT], PlcBuffer.MaxSize );
      if(PlcBuffer.DataLen > PtrToConstants->DATA[PLC_MIN_BYTE_LEN])
      {
         PlcBuffer.DataLen = DePack(&PlcBuffer);
         if(PlcBuffer.DataLen > 0)
         {
           PlcLedCounter = PlcBuffer.DataLen;
           if(TBuffer::ParsingAnswer(PlcBuffer.BUF, PlcBuffer.DataLen))
           {
              PlcBuffer.DataLen = TBuffer::GetDataFroMessage(PlcBuffer.BUF, PlcBuffer.DataLen);
              if(PlcBuffer.DataLen > 0)
              {
                PlcBuffer.DataLen = Pack(&PlcBuffer);
                Fase = TX_CMD;
                break;
              }
           }
           else{
              PushBufToRS485(&PlcBuffer);
           }
         }
         else
         {
           Fase = WAIT_MES;
         }
      }
      else if (PlcBuffer.DataLen == 0)//timeout
      {
        //P1OUT ^= BIT1;
          Fase = WAIT_MES;
      }
      else
      {
          if(!FromRS485ToPlc.empty())
          {
            PlcPort.SetTimer(RX_BYTE_TIMEOUT, 0); // easy RecvOff if need send
          }
      }
      break;
      
    case WAIT_MES:
      {
        TBuffer* buff = PopBufFromRS485();
        if(buff != NULL)
        {
          TBuffer::CopyBuffer(&PlcBuffer, buff);
          delete buff;
          PlcBuffer.DataLen = Pack(&PlcBuffer);
          Fase = TX_CMD;
        }
        else{
          Fase = RX_CMD;
        }
      }
      break;
      
    case TX_CMD:
      if( PlcPort.Send(PlcBuffer.BUF, PlcBuffer.DataLen, false) > 0)
      {
        PlcLedCounter = PlcBuffer.DataLen;
        Fase = RX_CMD;
      }
      break;
    
    default:
      Fase = RX_CMD;
      break;
  };
}
//******************************************************************************
int Pack(TBuffer *Buffer)
{
  int ret = 0;
  if( Buffer != NULL && Buffer->DataLen > 0 )
  {
     int NEr = (int)(Buffer->DataLen*0.25);
     ret = Buffer->DataLen + NEr*2;     
     c_form(NEr, ret);
     CLEAR_WDT;
     c_code((char*)Buffer->BUF);  
     ret = TBuffer::ByteStuffing(Buffer->BUF, ret, 0xAA, 0xBB);
     CLEAR_WDT;
     for(int i=ret-1; i >=0; i--)
     {
       Buffer->BUF[i+7] = Buffer->BUF[i];
     }
     ret+=7;
     Buffer->BUF[0] = 0xFF;
     Buffer->BUF[1] = 0xFF;
     Buffer->BUF[2] = 0xAA;
     Buffer->BUF[3] = 0xAA;
     Buffer->BUF[4] = NEr;
     Buffer->BUF[5] = 0xFF;
     Buffer->BUF[6] = NEr;
     
     Buffer->BUF[ret] = 0xFF;
     Buffer->BUF[ret+1] = NEr;
     Buffer->BUF[ret+2] = 0xFF;
     Buffer->BUF[ret+3] = 0xAA;
     Buffer->BUF[ret+4] = 0xAA;
     ret+=5;
     CLEAR_WDT;
  }
  return ret;
}

//******************************************************************************
int DePack(TBuffer *Buffer)
{
  int ret = 0;
  if( Buffer != NULL && Buffer->DataLen > 10 )
  {
    Word Len = Buffer->DataLen;
    CLEAR_WDT;
    int start = -1;
    int end = -1;

    for(int i = 0; i < Len; i++)
    {
      if(Buffer->BUF[i] == 0xAA && i < (Len-2) && Buffer->BUF[i+1] == 0xAA)
      {
        start = i+2;
        break;        
      }
    }
    if(start >=0)
    {
      CLEAR_WDT;
       for(int i=Len-1; i >= 0; i--)
       {
          if(Buffer->BUF[i] == 0xAA && i > 1 && Buffer->BUF[i-1] == 0xAA)
          {
            end = i-1;
            break;        
          }
       }
       if(end > 0 && (end-start) > 6 )
       {
          Word newLen = (end-start)-6;
          Byte num1 = Buffer->BUF[start];
          Byte num2 = Buffer->BUF[start+2];
          Byte num3 = Buffer->BUF[end-1];

          Byte NEr = CalcNEr(num1, num2, num3);
          ret = TBuffer::DeByteStuffing(&Buffer->BUF[start+3], newLen, 0xAA, 0xBB);
          for(int i = 0; i < ret; i++)
          {
            Buffer->BUF[i] = Buffer->BUF[start+3+i];
          }
          c_form(NEr, ret);
          int nErr = c_decode((char*)Buffer->BUF);
          ret -=NEr*2;
          if(ret < 0)
            ret = 0;
       }
    }
    CLEAR_WDT;
  }
  return ret;
}
//******************************************************************************
Byte CalcNEr(Byte num1, Byte num2, Byte num3)
{
  return num1 == num2 ? num1 : (num2 == num3 ? num2 : (num1 == num3 ? num3 : 0));
}
//******************************************************************************
void PushBufToRS485(TBuffer *Buffer)
{
  if(Buffer != NULL && Buffer->DataLen > 0 && !FromPlcToRS485.full())
  {
    TBuffer *newBuff = new TBuffer(Buffer->DataLen);
    TBuffer::CopyBuffer(newBuff, Buffer);
    //TBuffer::ClrBUF(Buffer->BUF, Buffer->DataLen);
    FromPlcToRS485.push(newBuff);
  }
}
//******************************************************************************
TBuffer* PopBufFromRS485(void)
{
  if(!FromRS485ToPlc.empty())
    return FromRS485ToPlc.pop();
  else
    return NULL;
}
//******************************************************************************
// LedProcess
//******************************************************************************
void LedProcess(void *Par)
{ 
    if(Rs485Led_Ready)
    {
      Rs485Led_Ready = false;
      if(Rs485LedCounter > 0)
      {
        Rs485LedCounter--;
        GPIOC->ODR ^= GPIO_PIN_13; 
      }
    }
    if(PlcLed_Ready)
    {
      PlcLed_Ready = false;
      if(PlcLedCounter > 0)
      {
        PlcLedCounter--;
        GPIOC->ODR ^= GPIO_PIN_15; 
      }
      else{
        GPIOC->ODR &= ~GPIO_PIN_15; 
      }
    }
    
    
    
    /*
    TBuffer TestBuffer(50);
    int len = 30;
    for(int i=0; i < len; i++) 
    {
      TestBuffer.BUF[i]=i;
      if(i%5 == 0)
        TestBuffer.BUF[i] = 0xAA;
    }
    TestBuffer.DataLen = len;  
    
    TestBuffer.DataLen = Pack(&TestBuffer);
    TestBuffer.DataLen = DePack(&TestBuffer);
    
    
    TestBuffer.DataLen = TBuffer::ByteStuffing(TestBuffer.BUF, TestBuffer.DataLen, 0xAA, 0xBB);
    //TestBuffer.DataLen = TBuffer::DeByteStuffing(TestBuffer.BUF, TestBuffer.DataLen, 0xAA, 0xBB);
    */
    
    /*
    char buf[255];
    int NEr = 10;
    c_form(NEr, 255); 
    for(int i=0;i< (255-NEr); i++) 
      buf[i]=i;
    CLEAR_WDT;
    long long startms = MilSecCount;
    c_code(buf);                    
    long long codems = MilSecCount - startms;
    
    //CLEAR_WDT;
    for (int i = 0; i < NEr/5; i++)   
        buf[3 + 4 * i]++;
    startms = MilSecCount;
    int nErr = c_decode(buf);       
    long long decodems = MilSecCount - startms;
    
        for (int i = 0; i < 10000; i++)   
          asm("nop");   
    startms = 0;
    
    */
}
//******************************************************************************
//******************************************************************************
//******************************************************************************
// Initialization functions
//******************************************************************************
//
//******************************************************************************
#pragma inline
inline void InitRS485Objects()
{
    Word timeout = PtrToConstants->DATA[TIME_OUT];
    RS485Port.open(COM1, PtrToConstants->DATA[BOUD_RATE], timeout);
}
//******************************************************************************
//
//******************************************************************************
#pragma inline
inline void InitPlcObjects()
{
    Word timeout = PtrToConstants->DATA[PLC_TIME_OUT];
    PlcPort.open(COM3, PtrToConstants->DATA[PLC_BOUD_RATE], timeout);
}
//******************************************************************************
//
//******************************************************************************
void InitSpiObjects()
{
    CLEAR_WDT;
    SpiPort.params = PtrToConstants;
    SpiPort.open(SPI1, 2400, 50);
    SpiPort.write_register();
    SpiPort.read_register(); 
    SpiPort.close();
}
//******************************************************************************
//
//******************************************************************************
#pragma inline
inline void InitLedObjects()
{
     GPIOC->ODR |= GPIO_PIN_13; 
}
//*****************************************************************************