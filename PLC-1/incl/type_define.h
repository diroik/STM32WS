//******************************************************************************
//**
//**
//******************************************************************************
#ifndef TIPE_DEF_H
  #define TIPE_DEF_H
//******************************************************************************
#define PARITY_NONE 0
#define PARITY_ODD  1// нечётный
#define PARITY_EVEN 2// чётный
#define SMCLK    8000


#define BOOT_MEM_START        0x0800F400 //адрес начала памяти констант(загрузчик)
#define TMP_BOOT_MEM_START    0x0800F800 //адрес начала временной памяти констант(загрузчик)
#define PARAMS_ADDR           0x0800EC00
#define TMP_PARAMS_ADDR       0x0800F000
#define CORE_CLK              SMCLK

#define STX ':'
#define ETX 0x0A

#define START_COIL_STATUS_ADDR          100
#define START_DISCRETE_INPUTS_ADDR      200
#define START_HOLDIG_REGISTERS_ADDR     300
#define START_INPUT_REGISTERS_ADDR      400

#define BAUD_RATE_IND   0
#define DATA_BITS_IND   1
#define STOP_BITS_IND   2
#define PARITY_MODE_IND 3
#define TIMEOUT_IND     4

#define RESTART_TIMEOUT 5000 //5 sec
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
typedef enum  
{
    BOOT_FLG  = 0,
    DEV_ADDR,           //1
    TIME_OUT,           //2
    BOUD_RATE,          //3
    BYTES_CNT,          //4
    PLC_TIME_OUT,       //5
    PLC_BOUD_RATE,      //6
    PLC_BYTES_CNT,      //7
    MIN_BYTE_LEN,       //8
    PLC_MIN_BYTE_LEN,   //9
    DEV_CURR_VER,       //10
    DEV_SUB_VER,        //11
    SERIAL_LOW,         //12
    SERIAL_HI           //13

}PARAMETERS;
//******************************************************************************
struct Data_Params
{
        Word DATA[20];
    
        Word transnite_plc_teimeout_m;
        Word transnite_plc_teimeout_s;
        Word resive_byte_temeout;  
        Word transmite_485_temeout;
        
        Word size_pack_plc;
        Word master_slave;              // 0 - slave, 1- master
        
        Word my_address;
        
        Word filter;                    // 0 - off, 1- on
        Word sensitive;                 // 0 - off, 1- on
        Word deviation;                 // 0 - off, 1- on
        Word baudrate;
        Word version;
        Word ping_timer;
        
        
        Byte CRC_SUM;
        Byte KeyByte;        
};
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

//enum DETECTING_RESULT{NO_RESULT=0,MAX_DETECTED,MIN_DETECTED};
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
