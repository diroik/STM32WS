
//=============================================================================
#include <stm32f10x_tim.h>
#include <stm32f10x.h>
#include <type_define.h>
#include <params_def.h>
#include <AdvClasses.h>
#include <PowerMetr.h>
#include <Classes.h>
//=================================================
  //volatile static sByte adcCnt = 0;
volatile static sByte adcCnt = 0;
//=============================================================================
/*******************************************************************************
* прерывание системного счётчика времени
*
*******************************************************************************/
extern class TComPort0 RS485Port;
extern T_ADC_AKB               AKB[];
extern int                     ADC_Counter;
extern bool                    AdcStartEnable;
extern Word                    AdcMultiplier;
extern class TPowerMetrManager      *PowerMetrManager;
//extern class TTimer                  WdiTimer();

Byte cnt = 0;
//=============================================================================
void ADC1_IRQHandler (void)
{
  AKB[cnt].AKB += ADC1->DR;
  ADC1->SMPR2 = (uint32_t)ADC_SampleTime_384Cycles << (3 * cnt);
  ADC1->SQR3 = cnt;
  ADC1->SMPR1 = 0;

  if(cnt == 8)
  {
    ADC1->SMPR1 = (uint32_t)ADC_SampleTime_384Cycles << (3 * 0x7);
    ADC1->SQR3 = 0x11;
  }

  cnt++;
  if(cnt >= 9)//!!!!
  {
    cnt = 0;
    ADC_Counter ++;
    if(ADC_Counter < AdcMultiplier)
    {

    }
    else 
    {  
      AKB[0].AKB_OUT  = AKB[0].AKB;/*>>12;*/  AKB[0].AKB=0;//etalon adc17
      
      AKB[1].AKB_OUT  = AKB[1].AKB;/*>>12;*/  AKB[1].AKB=0;
      AKB[2].AKB_OUT  = AKB[2].AKB;/*>>12;*/  AKB[2].AKB=0;
      AKB[3].AKB_OUT  = AKB[3].AKB;/*>>12;*/  AKB[3].AKB=0;         
      AKB[4].AKB_OUT  = AKB[4].AKB;/*>>12;*/  AKB[4].AKB=0;          
      AKB[5].AKB_OUT  = AKB[5].AKB;/*>>12;*/  AKB[5].AKB=0;
      AKB[6].AKB_OUT  = AKB[6].AKB;/*>>12;*/  AKB[6].AKB=0;
      AKB[7].AKB_OUT  = AKB[7].AKB;/*>>12;*/  AKB[7].AKB=0;
      AKB[8].AKB_OUT  = AKB[8].AKB;/*>>12;*/  AKB[8].AKB=0;
      ADC_Counter = 0;
      
      if(PowerMetrManager != NULL) 
      {
        PowerMetrManager->ADCReady=true;       
      }
      ADC_Counter=0;
      Stop_ADC12();
    }
  }
}
//=============================================================================
void TIM7_IRQHandler (void)
{
  if(TIM7->SR & TIM_IT_Update)
  {
//    GPIOC->ODR ^= GPIO_ODR_ODR_8; 
    TIM7->SR &= ~TIM_SR_UIF; 			//сброс бита переполнения счётчика
  }
  
//  TB0CCR1 += RtcTimerDivider;
  SystemTime.IncSecond();
  SecCount.Inc();

  if(adcCnt-- <= 0)
  {
    adcCnt = ADC_TIMER_PERIUD;
    if(AdcStartEnable) 
    {
      Start_ADC12();
    }
  }
}
//=============================================================================
//=======================================
void SysTick_Handler (void)
{
  
  //WdiTimer.DecTimer(0);   
  RS485Port.DecTimer(RX_BYTE_TIMEOUT); 
}
//=============================================================================
//связь RS485
//=======================================
void USART1_IRQHandler(void)
{

  volatile static Byte data;
  Byte usart1_status; 
  usart1_status = USART1->SR;

    if (USART1->CR1 & USART_CR1_RXNEIE)
        {
        if(usart1_status & USART_SR_RXNE)
            {
            data = RS485Port.GetRxDataByte(); 
            RS485Port.PushByteToDATA_Rx(data);//к себе на обработку
            }
        }
            
    if (USART1->CR1 & USART_CR1_TXEIE)
        {
        if(usart1_status & USART_SR_TXE)
            {
            RS485Port.SetTxIntEnable(false);         
            //RS485Port.SetTxDataByte(RS485Port.DATA_Tx); 
            }
        }
}

//=============================================================================
//=======================================


