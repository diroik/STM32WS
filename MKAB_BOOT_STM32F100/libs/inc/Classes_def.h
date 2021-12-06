#ifndef CLASSES_DEF_H
  #define CLASSES_DEF_H
    //*****************************************************************************
    #define BOOT_MEM_START        0x1080 //адрес начала памяти констант(загрузчик)
    #define TMP_BOOT_MEM_START    0x1000 //адрес начала временной памяти констант(загрузчик)
    #define CORE_CLK              SMCLK
    //=============================================================================
    // TObject
    //====================================
    //#define NULL  0
    //=============================================================================
    // TComPort
    //====================================
    #define INVALID_HANDLE_VALUE  0
    //таймауты:
    #define RX_BYTE_TIMEOUT       0x00      // между символами 
    #define RX_TOTAL_TIMEOUT      0x01      // на всю посылку   
    #define TX_BYTE_TIMEOUT       0x02  
    #define TX_TOTAL_TIMEOUT      0x03 
    
    #define MAX_BUF_LEN           1300
    #define PASSWORD_CNT          1
    #define ADDR_CNT              2
    #define CMD_CNT               3
    #define ERROR               (-1)
    //====================================
    #define STX           0x02
    #define ETX           0x03
    //=============================================================================
    // TCmd_msg
    //====================================
    const unsigned char ERROR_MASS[][16]=
    {  
        {"OK"},//
        {"PASW_ER"},        
        {"ID_ER"},
        {"CMD_ER"},//
        {"CRC_ER "},//
        {"UNKN_ERROR"},//
        {"NO_DATA"},
    };
//=============================================================================
// TObjectFunctions
//====================================
#define MAX_MESS_IN_USB    1
#define MAX_MESS_READY_SEND 2  
#define MAX_MESS_IN_ALTERNATE  3
#define MAX_MESS_IN_MAIN       5

#define MAX_DEV_ADDR        20
#define MAX_ECOUNTER_ADDR   5   
#define MAX_DATCHIKS        32
#define MAX_TERMO_DATCHIKS  8
#define MAX_CODES           40
    
#define USB_RS485_ACTIV_WAIT_TIMER 0
//=============================================================================
//====================================
#define MAX_ADC_INPUTS          15    
#define ADC_TIMER_PERIUD        10 // периуд включения АЦП в cек
//=============================================================================    
#define DEVICE_CURR_VER  3      //
#define DEVICE_SUB_VER   2      //Подверсия устройства.    
#define REPEAT_RECV_COUNT       600     //!!!!! зависит от USB_Timeout
//=============================================================================
//====================================    
#define MAX_ETYPES 5
//====================================       
#define STD_MODBUS_MSG_LEN  12
#define RESISTOR 1500.0
#define I_DIVIDER  2.0
#define I_SENS     0.042 // 25 мВ/A    
#define U_SENS_MAX 5.0
#define I_NULL_POROG  0.05
#define MAX_U         2.55 
#define MIN_U         2.45
    

#define BOOTLOADER_CURR_VER         4

#define MEM_FIRM_START 0x08005C00
#define MEM_FIRM_END   0x0800DC00

#define PAGE_BLOCK_LEN 256


#endif
