
#include <stm32f10x.h>

#ifndef IO_CFG_H
#define IO_CFG_H

#define  CRITICAL_SEC_ON  __disable_interrupt();
#define  CRITICAL_SEC_OFF __enable_interrupt();

// ----- константы для загрузки индикации-----

#define COLOR_GREEN false
#define COLOR_RED true
#define LED_LOAD_LIGHT           0x01
#define LED_LOAD_MIDDLE          0x05
#define LED_LOAD_HARD            0x15
#define LED_LOAD_CHARGE_HOLD     0x3F

// -------------------------------------------

/*
// ----- константы параметров заряда разряда -

#define I_CHARGE_MAX 1000 //Миллиампер
#define U_IN_CONST 2
#define U_BAT_MIN 5.0
#define U_BAT_MAX 7.2
#define START_DAC 600
#define DAC_STEP  10
#define DAC_STEP_MIN  2
#define DELTA_U_MAX 0.1
#define DAC_STEP_PRECHARGE 100
#define I_TRANSFER_TO_STORAGE 5   // ток перехода в STORAGE
#define U_STORAGE_HOLD 6.5        // напряжение поддержания в состоянии STORAGE

// -------------------------------------------

// ------------ состояния переходов ----------

typedef enum {
  STATE_START=0x01,
  STATE_PRECHARGE=0x2,
  STATE_CHARGE=0x03,
  STATE_FULL_CHARGE=0x04,
  STATE_DISCHARGE=0x05,
  STATE_FULL_DISCHARGE=0x06,
  STATE_STORAGE=0x07,
  STATE_OVER_CHARGE=0x08,
} T_APP_POWER_STATE;

// --------------------------------------------

*/

// -------------------------------------------
void IndicateLoad_Interrupt(void);
void Indicate_Set(uint8_t load, bool color);
void Indicate_Process(void);
//= IO_CFG =============================================
//      #define P1_PORT   GPIOA.ODR
//      #define P1_DDR    GPIOA.DDR
//      #define P1_PIN    GPIOA.IDR
//      //#define P1_SEL    PC_CR1
//      //#define P1_SEL2   PC_CR2
//    
//      #define P2_PORT   GPIOB.ODR
//      #define P2_DDR    GPIOB.DDR
//      #define P3_PIN    GPIOB.IDR
//      //#define P3_SEL    PD_CR1
//      //#define P4_SEL2   PD_CR2
//= OSC ================================================
      #define SMCLK    16000
//= WatchDog Timer =====================================
    /**** Макросы для WDT ****/
    #define CLEAR_WDT    {}    //IWDG->KR=0xAAAA; // Перезагрузка
    #define HOLD_WDT     {} //WDTCTL = (WDTPW | WDTHOLD)  

/*
//======= USART0 =======================================
      #define UART_DATA_TX0     UCA0TXBUF  
      #define UART_DATA_RX0     UCA0RXBUF
      #define TX0_PORT          4
      #define RX0_PORT          5
      #define UART_BUS0_DIR     P3_DDR
      #define UART_BUS0_EN      P3_SEL

      #define U0BR0             UCA0BR0
      #define U0BR1             UCA0BR1
      #define U0MCTL            UCA0MCTL

      #define TX0_INT_FLAG      (bool)(UCA0IFG & UCTXIFG) //(bool)(IFG1 & UTXIFG0)

      #define TX0_INT_ENABLE    UCA0IE  |=  UCTXIE  //  enable tx int
      #define TX0_INT_DISABLE   UCA0IE  &= ~UCTXIE //  disable tx int

      #define RX0_INT_ENABLE    UCA0IE  |= UCRXIE;    
      #define RX0_INT_DISABLE   UCA0IE  &= ~UCRXIE;
                                                                 
      #define USART0_BUSY        (bool)(UCA0STAT & UCBUSY)    
//======= USART1 =======================================
      #define UART_DATA_TX1     UCA1TXBUF  
      #define UART_DATA_RX1     UCA1RXBUF
      #define TX1_PORT          6
      #define RX1_PORT          7
      #define UART_BUS1_DIR     P5_DDR
      #define UART_BUS1_EN      P5_SEL

      #define U1BR0             UCA1BR0
      #define U1BR1             UCA1BR1
      #define U1MCTL            UCA1MCTL

      #define TX1_INT_FLAG      (bool)(UCA1IFG & UCTXIFG) 

      #define TX1_INT_ENABLE    UCA1IE  |=  UCTXIE  //  enable tx int
      #define TX1_INT_DISABLE   UCA1IE  &= ~UCTXIE //  disable tx int

      #define RX1_INT_ENABLE    UCA1IE  |= UCRXIE;    
      #define RX1_INT_DISABLE   UCA1IE  &= ~UCRXIE;
                                                                 
      #define USART1_BUSY        (bool)(UCA1STAT & UCBUSY)  
//======= USART2 =======================================
      #define UART_DATA_TX2     UCA2TXBUF  
      #define UART_DATA_RX2     UCA2RXBUF
      #define TX2_PORT          4
      #define RX2_PORT          5
      #define UART_BUS2_DIR     P9_DDR
      #define UART_BUS2_EN      P9_SEL

      #define U2BR0             UCA2BR0
      #define U2BR1             UCA2BR1
      #define U2MCTL            UCA2MCTL

      #define TX2_INT_FLAG      (bool)(UCA2IFG & UCTXIFG) 

      #define TX2_INT_ENABLE    UCA2IE  |=  UCTXIE  //  enable tx int
      #define TX2_INT_DISABLE   UCA2IE  &= ~UCTXIE //  disable tx int             

      #define RX2_INT_ENABLE    UCA2IE  |= UCRXIE;    
      #define RX2_INT_DISABLE   UCA2IE  &= ~UCRXIE;
                                                                 
      #define USART2_BUSY        (bool)(UCA2STAT & UCBUSY)  
//======= USART3 =======================================
      #define UART_DATA_TX3     UCA3TXBUF  
      #define UART_DATA_RX3     UCA3RXBUF
      #define TX3_PORT          4
      #define RX3_PORT          5
      #define UART_BUS3_DIR     P10_DDR
      #define UART_BUS3_EN      P10_SEL

      #define U3BR0            UCA3BR0
      #define U3BR1             UCA3BR1
      #define U3MCTL            UCA3MCTL

      #define TX3_INT_FLAG      (bool)(UCA3IFG & UCTXIFG) 

      #define TX3_INT_ENABLE    UCA3IE  |=  UCTXIE  //  enable tx int
      #define TX3_INT_DISABLE   UCA3IE  &= ~UCTXIE //  disable tx int

      #define RX3_INT_ENABLE    UCA3IE  |= UCRXIE;    
      #define RX3_INT_DISABLE   UCA3IE  &= ~UCRXIE;
                                                                 
      #define USART3_BUSY        (bool)(UCA3STAT & UCBUSY)  
//======= TIMER0 =======================================
      #define T0_IV              TAIV
      #define T0_INT1            0x02  	//Захват/сравнение 1  	TACCR1 CCIFG  	
      #define T0_INT2            0x04 	//Захват/сравнение 2 	TACCR2 CCIFG 	  
      #define T0_INT3            0x0A 	//Переполнение таймера 	TAIFG
//======= TIMER1 =======================================
      #define T_INT1            0x02  	//Захват/сравнение 1  	TBCCR1 CCIFG  	
      #define T_INT2            0x04 	//Захват/сравнение 2 	TBCCR2 CCIFG 	
      #define T_INT3            0x06 	//Захват/сравнение 3 	TBCCR3 CCIFG 	
      #define T_INT4            0x08 	//Захват/сравнение 4 	TBCCR4 CCIFG 	
      #define T_INT5            0x0A 	//Захват/сравнение 5 	TBCCR5 CCIFG
      #define T_INT6            0x0C 	//Захват/сравнение 6 	TBCCR6 CCIFG
      #define T_INT7            0x0E 	//Переполнение таймера 	
//======================================================
//======================================================
*/
#endif//IO_CFG_H
