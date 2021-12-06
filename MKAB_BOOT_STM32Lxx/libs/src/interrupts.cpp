
//=============================================================================
#include <stm32l1xx.h>
#include <type_define.h>
#include <params_def.h>
#include <AdvClasses.h>
//=================================================
  //volatile static sByte adcCnt = 0;
volatile uint32_t system_timer = 0;	// системный счётчик времени в мс.
volatile static sByte adcCnt = 0;
//=============================================================================
/*******************************************************************************
* прерывание системного счётчика времени
*
*******************************************************************************/
extern class TComPort0 RS485Port;
extern TDateTime SystemTime;
//=============================================================================
void TIM7_IRQHandler (void)
{
  if(TIM7->SR & TIM_IT_Update)
  {
//    GPIOC->ODR ^= GPIO_ODR_ODR_13;
    TIM7->SR &= ~TIM_SR_UIF; 			//сброс бита переполнения счётчика
  }
//  TB0CCR1 += RtcTimerDivider;
  SystemTime.IncSecond();
  TObjectFunctions::ObjectFunctionsTimer.DecTimer(0);
}
//=============================================================================
void SysTick_Handler (void)
{
  RS485Port.DecTimer(RX_BYTE_TIMEOUT); 
}
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

