#ifndef CLASSES_DEF_H
  #define CLASSES_DEF_H
    //=============================================================================
    #define INVALID_HANDLE_VALUE  0
    //таймауты:
    #define RX_BYTE_TIMEOUT       0x00      // между символами 
    #define RX_TOTAL_TIMEOUT      0x01      // на всю посылку   
    #define TX_BYTE_TIMEOUT       0x02  
    #define TX_TOTAL_TIMEOUT      0x03 
    
    #define MAX_BUF_LEN           300//
    #define ERROR               (-1)
    //====================================
    #define STX ':'
    #define ETX 0x0A
//=============================================================================    
#define DEVICE_CURR_VER  1      //
#define DEVICE_SUB_VER   0      //Подверсия устройства.    
//=============================================================================
#define STD_MODBUS_MSG_LEN  12
#define BOOTLOADER_CURR_VER 1
//=============================================================================
#define MEM_FIRM_START 0x08005C00
#define MEM_FIRM_END   0x0800DC00
#define PAGE_BLOCK_LEN 256
//=============================================================================
//состояния приема.
#define FIRST_BYTE
#define NEXT_BYTE
#define ASK_RECV 0
#define ASK_TRAN 1
#define RECV_OFF 0
#define RECV_ON 1

#define CNT_PACKEG 10
#define WAIT_PACKET 44
#define TRANSMITE_PLC 1
#define RECEIVE_ASC_PLC 2
#define TRANSMITE_ASC_PLC 3
#define NONE 4
#define RECIVE_PACKET 5
#define START_N 50
#define PING 140

//#define SIZE_PACK_PLC 3
//#define TRANSMITE_PLC_TIMEOUT_M 3000
//#define TRANSMITE_PLC_TIMEOUT_S 7000
//#define RESIVE_BYTE_TIMEOUT 100
//#define TRANSMITE_485_TIMEOUT 300

#define MASTER 1
#define SLAVE 0

#define PING_FREE 0
#define PING_SEND 1
#define PING_OFF  2

#define GPIO_PIN_0                 ((uint16_t)0x0001)  /* Pin 0 selected    */
#define GPIO_PIN_1                 ((uint16_t)0x0002)  /* Pin 1 selected    */
#define GPIO_PIN_2                 ((uint16_t)0x0004)  /* Pin 2 selected    */
#define GPIO_PIN_3                 ((uint16_t)0x0008)  /* Pin 3 selected    */
#define GPIO_PIN_4                 ((uint16_t)0x0010)  /* Pin 4 selected    */
#define GPIO_PIN_5                 ((uint16_t)0x0020)  /* Pin 5 selected    */
#define GPIO_PIN_6                 ((uint16_t)0x0040)  /* Pin 6 selected    */
#define GPIO_PIN_7                 ((uint16_t)0x0080)  /* Pin 7 selected    */
#define GPIO_PIN_8                 ((uint16_t)0x0100)  /* Pin 8 selected    */
#define GPIO_PIN_9                 ((uint16_t)0x0200)  /* Pin 9 selected    */
#define GPIO_PIN_10                ((uint16_t)0x0400)  /* Pin 10 selected   */
#define GPIO_PIN_11                ((uint16_t)0x0800)  /* Pin 11 selected   */
#define GPIO_PIN_12                ((uint16_t)0x1000)  /* Pin 12 selected   */
#define GPIO_PIN_13                ((uint16_t)0x2000)  /* Pin 13 selected   */
#define GPIO_PIN_14                ((uint16_t)0x4000)  /* Pin 14 selected   */
#define GPIO_PIN_15                ((uint16_t)0x8000)  /* Pin 15 selected   */
#define GPIO_PIN_All               ((uint16_t)0xFFFF)  /* All pins selected */

#define GPIO_PIN_MASK              0x0000FFFFU /* PIN mask for assert test */


#endif
