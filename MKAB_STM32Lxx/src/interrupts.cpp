
//=============================================================================
#include <stm32l1xx.h>
#include <stm32l1xx_adc.h>
#include <type_define.h>
#include <params_def.h>
#include <AdvClasses.h>
#include <PowerMetr.h>
#include <Classes.h>
//=================================================
  //volatile static sByte adcCnt = 0;
//static sByte adcCnt = 0;
static int adcChanalIndex      = 0;
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

//Byte cnt = 0;
//=============================================================================
void ADC1_IRQHandler (void)
{ 
    AKB[adcChanalIndex].AKB += ADC1->DR; 
    if(ADC_Counter++ >= AdcMultiplier)
    {   
      AKB[adcChanalIndex].AKB_OUT  = AKB[adcChanalIndex].AKB; AKB[adcChanalIndex].AKB=0;//etalon adc17
      Stop_ADC12();

      if(++adcChanalIndex > MAX_ADC_INPUTS)
      {
        adcChanalIndex = 0;
        if(PowerMetrManager != NULL) {
          PowerMetrManager->ADCReady = true;       
        }
      }
      ADC_Counter = 0;
    }
}
//=============================================================================
void TIM7_IRQHandler (void)
{
  if(TIM7->SR & TIM_IT_Update)
  {
    TIM7->SR &= ~TIM_SR_UIF; 			//сброс бита переполнения счётчика

    
    GPIOC->ODR ^= GPIO_ODR_ODR_13;//13 
  }

  SystemTime.IncSecond();
  SecCount.Inc();

    if(AdcStartEnable == true) 
    {
      if(ADC_Counter == 0)
      {
        if(adcChanalIndex == 0)
        {
            ADC1->SMPR2 = (DWord)ADC_SampleTime_384Cycles << 21; 
            ADC1->SMPR3 = 0;
            ADC1->SQR5 = ADC_Channel_Vrefint;
        }
        else
        {
          ADC1->SMPR2 = 0; 
          ADC1->SMPR3 = (uint32_t)ADC_SampleTime_384Cycles << (3 * (adcChanalIndex-1));
          ADC1->SQR5 = adcChanalIndex - 1;//SQR5
        }
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


