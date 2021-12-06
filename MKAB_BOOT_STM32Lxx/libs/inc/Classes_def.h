#ifndef CLASSES_DEF_H
  #define CLASSES_DEF_H
    //*****************************************************************************
    //#define BOOT_MEM_START        0x1080 //адрес начала памяти констант(загрузчик)
    //#define TMP_BOOT_MEM_START    0x1000 //адрес начала временной памяти констант(загрузчик)
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
    //#define ERROR               (-1)
    //====================================
    #define STX           0x02
    #define ETX           0x03
    //=============================================================================
    // TCmd_msg
    //====================================
    const unsigned char ERROR_MASS[][10]=
    {  
        {"OK"},//
        {"UNK_ER"},//
        {"AppCrcErr"},
    };
//=============================================================================
// TObjectFunctions
//====================================
#define MAX_MESS_IN_USB    1
#define MAX_MESS_READY_SEND 2  
#define MAX_MESS_IN_ALTERNATE  1
#define MAX_MESS_IN_MAIN       1
#define USB_RS485_ACTIV_WAIT_TIMER 0
//=============================================================================
//====================================
#define MAX_ADC_INPUTS          8    
#define ADC_TIMER_PERIUD        1 // период включения АЦП в cек
//=============================================================================    
#define DEVICE_CURR_VER  5      //
#define DEVICE_SUB_VER   0      //Подверсия устройства.    
#define REPEAT_RECV_COUNT       600     //!!!!! зависит от USB_Timeout
//=============================================================================
//====================================    
#define MAX_ETYPES 5
//====================================       
#define BOOTLOADER_CURR_VER         50
//#define MEM_FIRM_START 0x08005C00
//#define MEM_FIRM_END   0x0800DC00
#define PAGE_BLOCK_LEN 256


#endif
