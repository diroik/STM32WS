
//=============================================================================
#include <stm32f10x_tim.h>
#include <stm32f10x.h>
#include <type_define.h>
#include <params_def.h>
#include <AdvClasses.h>
#include <PowerMetr.h>
#include <Classes.h>

#define ADC_MEASUREMENT_COUNT 1024
#define CHANNEL_CNT 5
//=================================================
  //volatile static sByte adcCnt = 0;
volatile uint32_t system_timer = 0;	// системный счётчик времени в мс.
volatile static sByte adcCnt = 0;

 //-------индикация----------//
      extern  volatile uint8_t Load_Indicate;
      extern volatile bool Color_Indicate;
      extern uint8_t Bit_Counter;
      extern uint8_t Load_Indicate_Buffer;
//--------------------------//

//=============================================================================
/*******************************************************************************
* прерывание системного счётчика времени
*
*******************************************************************************/
extern const int LED_RED = 0x40;    //Маска пина Красного диода
extern const int LED_GREEN = 0x20;  //Маска пина Зелёного диода
extern const int TEST_PIN=0x08;    // Тестовый пин

extern T_ADC_AKB               AKB[];
extern int                     ADC_Counter;
extern bool                    AdcStartEnable;
extern float                    AdcMultiplier;
extern class TPowerMetrManager      *PowerMetrManager;
//extern class TTimer                  WdiTimer();

Byte cnt;

Byte CH_MASK[CHANNEL_CNT]={0,1,3,7,8};
uint16_t result[16];

void ADC1_IRQHandler (void)
{
  AKB[cnt].AKB += ADC1->DR;
  ADC1->SMPR2 = (uint32_t)ADC_SampleTime_384Cycles << (3 * cnt);
  ADC1->SQR3 = CH_MASK[cnt];
  ADC1->SMPR1 = 0;

  if(cnt == CHANNEL_CNT)   //4
  {
    ADC1->SMPR1 = (uint32_t)ADC_SampleTime_384Cycles << (3 * 0x7);
    ADC1->SQR3 = 0x11;
  }

  
  if(cnt++ >= CHANNEL_CNT)//!!!!     //4
  {
    cnt = 0;
    ADC_Counter ++;
    if(ADC_Counter < AdcMultiplier)          // AdcMultiplier
    {

    }
    else 
    {  
      AKB[0].AKB_OUT  = AKB[0].AKB;/*>>12;*/  AKB[0].AKB=0;//etalon adc17
      
      AKB[1].AKB_OUT  = AKB[1].AKB;/*>>12;*/   AKB[1].AKB=0;//Канал А0
      AKB[2].AKB_OUT  = AKB[2].AKB;/*>>12;*/   AKB[2].AKB=0;//Канал А1
      AKB[3].AKB_OUT  = AKB[3].AKB;/*>>12;*/   AKB[3].AKB=0;//Канал A3         
      AKB[4].AKB_OUT  = AKB[4].AKB;/*>>12;*/   AKB[4].AKB=0;//Канал А7          
      AKB[5].AKB_OUT  = AKB[5].AKB;/*>>12;*/   AKB[5].AKB=0;//Канал A8       
      ADC_Counter = 0;
      
      if(PowerMetrManager != NULL) 
      {
        PowerMetrManager->ADCReady=true;       
      }
      ADC_Counter=0;
      Stop_ADC12(); 
      
      //GPIOA->ODR&=~LED_RED; //GPIOB->ODR&=~TEST_PIN; 

      Start_ADC12();  
      
      //GPIOA->ODR ^=LED_RED; //GPIOB->ODR|=TEST_PIN;       /// add
     
      /*
      if(AdcStartEnable) 
      {
        Start_ADC12(); GPIOB->ODR|=TEST_PIN;     
      }*/
    }
  }
}

void TIM7_IRQHandler (void)
{
  IndicateLoad_Interrupt();      // загрузка индикации
 
  if(TIM7->SR & TIM_IT_Update)
  {
//    GPIOC->ODR ^= GPIO_ODR_ODR_8; 
    TIM7->SR &= ~TIM_SR_UIF; 			//сброс бита переполнения счётчика
  }
  
//  TB0CCR1 += RtcTimerDivider;
  //SystemTime.IncSecond();   //Ускорено в 10 раз!!!
  //SecCount.Inc();
  

  
  /*
  if(adcCnt-- <= 0)
  {
    adcCnt = ADC_TIMER_PERIUD;
    if(AdcStartEnable) 
    {
      //Start_ADC12();       ////// zak
    }
  }*/
}

void IndicateLoad_Interrupt(void)
{
  if (Bit_Counter>7) 
  {
    Load_Indicate_Buffer = Load_Indicate; 
    Bit_Counter=0;
  }
  
  if (Load_Indicate_Buffer&0x01)
  {
    if (Color_Indicate) 
    {
      GPIOA->ODR|=LED_RED;       // LED_RED ON
      GPIOA->ODR&=~LED_GREEN; 
    }
    else
    {
      GPIOA->ODR|=LED_GREEN;     // LED_GREEN ON
      GPIOA->ODR&=~LED_RED;
    }   
  }
  else
  {
      GPIOA->ODR &= ~LED_RED;      // OFF ALL
      GPIOA->ODR &= ~LED_GREEN;   
  }
  
  Load_Indicate_Buffer >>= 1; 
  Bit_Counter++;
}

void Indicate_Set(uint8_t load, bool color)
{
  Load_Indicate=load;
  Color_Indicate=color;
}

void SysTick_Handler (void)
{
  
//WdiTimer.DecTimer(0);   
}

/*#pragma vector = TIMER0_B1_VECTOR
void Timer0B1_ISR(void)//только TBIFG(досчитали до 0xFFFF)
{
  
  switch (TB0IV)
  {
    case T0_INT1:     // таймер  rtc
        TB0CCR1 += RtcTimerDivider;
        SystemTime.IncSecond();
        SecCount.Inc();

        if(adcCnt-- <= 0)
        {
          adcCnt = ADC_TIMER_PERIUD;
          if(AdcStartEnable) Start_ADC12();
        }
        
        break;
    default:
      break;    
  }
  //__low_power_mode_off_on_exit(); 
}*/
//=============================================================================
//=============================================================================
/*#pragma vector = TIMER1_A1_VECTOR
void Timer1_A1_ISR(void)
{
  
  switch (TA1IV)
  {
    case T_INT1:     // таймер общего назначения
      TA1CCR1  += 3686;  //1 msek          
      RS485Port.DecTimer(RX_BYTE_TIMEOUT);   
      WdiTimer.DecTimer(0);   
      break;
    default:  
      break;    
  }
  //__low_power_mode_off_on_exit(); 
}*/
//=============================================================================
//связь RS485
//=======================================

void USART1_IRQHandler(void)
{

  volatile static Byte data;
  uint8_t usart1_status; 
  usart1_status = USART1->SR;
    if (USART1->CR1 & USART_CR1_RXNEIE)
        {
        if(usart1_status & USART_SR_RXNE)
            {
            }
        }
            
    if (USART1->CR1 & USART_CR1_TXEIE)
        {
        if(usart1_status & USART_SR_TXE)
            {
            }
        }
}

//=============================================================================
//=======================================

