

//******************************************************************************
//**
//**
//******************************************************************************
#ifndef TIPE_DEF_H
  #define TIPE_DEF_H
//******************************************************************************
//  #include <string>
//  #include <vector>
//  #include <Classes_def.h>
  //#include <stm32l1xx.h>
//******************************************************************************
typedef unsigned char  Byte;
typedef unsigned short   Word;
typedef unsigned int     DWord;
typedef unsigned short   Short;
//******************************************************************************
typedef signed char   sByte;
typedef signed short  sWord;
typedef signed long   sDWord;
//******************************************************************************
#pragma pack(1)
typedef union
{
    DWord Data_l;
    Byte Data_b[4];
} Blong;
//******************************************************************************
typedef union
{
    Word Data_s;
    Byte Data_b[2];
} Bshort;
//******************************************************************************
typedef union
{
    float Data_f;
    Byte Data_b[4];
} Bfloat;
#pragma pack()
//******************************************************************************
typedef unsigned int  HANDLE;
//****************************************************************************** 
typedef enum  
{
    COM0  = 1,
    COM1,
    COM2,
    COM3
}Port;
//******************************************************************************
typedef enum  
{
    STX_IND = 0,
    PASSWORD_IND,
    ADDR_IND,
    DATA_IND,
    TIME_IND,
    CMD_IND,
    ERSTAT_IND,
    CMD_DATA_IND,
    CRC_IND,
    ETX_IND
}PROTO_IND;
//******************************************************************************
typedef enum  
{
    RX_CMD = 0,
    EXE_CMD,
    TX_CMD,
    PARSING_CMD,
    PREPARE_CMD,
    WAIT_MES,
    RESET_CMD 
}FASE_CMD;
//******************************************************************************
typedef enum  
{
    OK_IND = 0,
    UNKNOWN_ER_IND,
    APPL_CRC_ERROR_IND,
}ERROR_IND;
//****************************************************************************** 
typedef enum  
{
    BOOTLOADER  = 0,
    APPLICATION
}BOOT_STAT;
//****************************************************************************** 
enum DATA_TASK_TIMER {TASK_PERIUD_TIMER = 0, TASK_MAIN_TIMER};
//******************************************************************************
enum TIMERS  { PERIUD_TIMER = 0, COMMON_TIMER, ADDITIONAL_TIMER, LONG_TIMER, ALARM_TIMER};
//******************************************************************************
typedef enum  
{
    DETECT_STATES = 0,
    FORMING_MESSAGE
}DATCHIK_MNG;
//******************************************************************************
typedef enum  
{
    DO_OVERSAMPL = 0,
    DO_DETECT_AKB,
    DO_FEEDBACK
}FASE_FEEDBACK;
//******************************************************************************
typedef enum  
{
    TA0  = 0,
    TA1,
    TB
}TimerType;
//******************************************************************************
typedef enum  { CHARGE = 0, DISCHARGE, FULL_CHARGE  } TAkbState;
//******************************************************************************
enum CLIMAT_STATES { MANAGE_AKB = 0, MANAGE_MAIN};
//******************************************************************************
typedef enum { FULL_POWER = 0, HALF_FULL_POWER, HALF_POWER, MIN_POWER, OFF_POWER}TPowerState;
//******************************************************************************
enum POWER_MNG_STATES { DETECT_STATE = 0, CREATE_MESSAGE, MANAGE_LOADS};
//******************************************************************************
//******************************************************************************
//Внутренние статусы сигнализации
//******************************************************************************
enum ALARM_STATES
{
  STATE_SECURE=0,
  STATE_MD_DOOR,
  STATE_ANY_ALARM,
  STATE_UNSECURE,
  STATE_READY_TO_SECURE  
};
//******************************************************************************
//Типы сработок
//******************************************************************************
enum ALARM_TYPES
{
  N0_ALARM=0,        //ничего не сработало
  ONLY_DOOR,         //Открыта дверка
  ONLY_MOVE,         //Сработал датчик движения
  ONLY_DOOR_AND_MOVE,//Датчик движения и Дверка
//  DOOR_MOVE_OTHER,   //
  ONLY_OTHER         //
};
//******************************************************************************
#pragma pack(1)
typedef struct Time 
{ 
  int Year, Month, DayOfWeek, Day, Hour, Minute, Second; 
}TRtcTime;
#pragma pack()

//******************************************************************************
struct T_ADC_AKB
{  
  Word  ADCMEM;   
  DWord  AKB;
  DWord  AKB_OUT;
};
//******************************************************************************
//Параметры протокола устройства
//******************************************************************************

#pragma pack(1)
typedef struct __Data_Params1
{
        Byte  DevAddress[3];//адрес устройства

}Data_Params1_t;
#pragma pack()

#pragma pack(1)
struct Data_Params
{
        Byte  DevAddress[3];//адрес устройства
        Byte  PassWord[9];      
        Byte  Unit_Type[9];
        
        DWord MAIN_timeout;   //межбайтовый таймаут 
        DWord MAIN_BOUD_RATE; //скорость передачи     
        
        Byte bReserved[20];
        Word wReserved[10];
        
        long Serial;
        
        Byte CRC_SUM;
        Byte  KeyByte;      
};
#pragma pack()

#pragma pack(1)
//******************************************************************************
struct Boot_Params
{
        Byte  BootStatus;
        Word  CurVersion;
        
        DWord  AppCurrAdr;        
 
        DWord AppAdrStart;
        Word Segment_Count;
        Word AppCRC;
        
        Word SerialNumber;
        Word Reserved1;
        Word Reserved2;
        Word Reserved3;
        Word Reserved4;
        
        Byte WriteRamVectorsFlag;
        
        Word UsartIntVector;
        Word Timer0B1IntVector;
        Word Timer1A1IntVector;  
        Word UnmIntVector;
     
        Byte  KeyByte;      
};
#pragma pack()
//******************************************************************************


//******************************************************************************
//Параметры Калибровки устройства
//******************************************************************************
#pragma pack(1)
struct TCalibrListConfig
{
        float CalVolt[8];
        bool IsFistStart;                 
        Word  CRC_SUM;
        Byte  KeyByte;         
};
#pragma pack()
//******************************************************************************
//Параметры Работы устройства
//******************************************************************************
#pragma pack(1)
struct TTaskListConfig
{
        Word          AdcMultiplier;
        Word          ReglamentTime;        //Период регламента   
        float         CalVolt[8];
        
        bool          IsFistStart;         //
        Word          CRC_SUM;
        Byte          KeyByte;      
};
#pragma pack()


enum DETECTING_RESULT{NO_RESULT=0,MAX_DETECTED,MIN_DETECTED};
//******************************************************************************
//******************************************************************************
typedef struct commtim
{
  commtim()
  {
    ReadIntervalTimeout         = 0;
    ReadTotalTimeout            = 0;
    ReadTotalTimeoutMultiplier  = 5;
  }
    DWord ReadIntervalTimeout;
    DWord ReadTotalTimeout;
    Word ReadTotalTimeoutMultiplier;
}COMMTIMEOUTS;
//******************************************************************************
typedef void (*PFV)(void* p );

typedef bool (*PFB)(void* p );
//******************************************************************************
//******************************************************************************
//=============================================================================
#endif // TIPE_DEF_H


#define __HAL_ADC_CLEAR_FLAG(__HANDLE__, __FLAG__)                             \
  (((__HANDLE__)->SR) = ~(__FLAG__))
    
    
    
//******************************************************************************
//******************************************************************************
