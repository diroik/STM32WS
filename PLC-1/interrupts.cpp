
//=============================================================================
#include <device.h>
#include <type_define.h>
#include <params_def.h>
#include <Classes.h>
//=============================================================================
extern Word64         SecCount; 
extern long long      MilSecCount;
extern TUsartPort0    RS485Port;
extern TUsartPort3    PlcPort;
extern bool           PlcLed_Ready;
extern sWord          PlcLedCounter;  
extern bool           Rs485Led_Ready;
extern sWord          Rs485LedCounter;  
//=============================================================================
/*******************************************************************************
* прерывание системного счётчика времени
*******************************************************************************/
void TIM7_IRQHandler (void)
{
  static int led_blink_cnt = 0;
    if(TIM7->SR & TIM_IT_Update)
    {
      TIM7->SR &= ~TIM_SR_UIF; 			//сброс бита переполнения счётчика
    }
    
    SecCount.Inc();
    if(led_blink_cnt++ >=5 )
    {
      led_blink_cnt = 0;
      if(Rs485LedCounter < 1)
        GPIOC->ODR ^= GPIO_PIN_13;                 //green led
    }
}
//=============================================================================
//
//=======================================
void SysTick_Handler (void)
{
  static int rx_tx_led_timer = 0;
    MilSecCount++;
    RS485Port.DecTimer(RX_BYTE_TIMEOUT); 
    PlcPort.DecTimer(RX_BYTE_TIMEOUT); 
    if( rx_tx_led_timer++ > 30){
      rx_tx_led_timer = 0;
      Rs485Led_Ready = true;
      PlcLed_Ready = true;
    }
}
//=============================================================================
//связь USART1COMPort = 485
//=======================================
void USART1_IRQHandler(void)
{
  volatile static Byte data;
  if(USART1->SR & USART_SR_RXNE)
  {
    data = RS485Port.GetRxDataByte(); 
    RS485Port.PushByteToDATA_Rx(data);//к себе на обработку
  }
  else if(USART1->SR & USART_SR_TXE)
  {     
    RS485Port.SetTxIntEnable(false);
  }
}
//=============================================================================
// связь USART3COMPort = PLC
//=======================================
void USART3_IRQHandler(void)
{
  volatile static Byte data;
  if(USART3->SR & USART_SR_RXNE)
  {
    data = PlcPort.GetRxDataByte(); 
    PlcPort.PushByteToDATA_Rx(data);//к себе на обработку
  }
  else if(USART3->SR & USART_SR_TXE)
  {
    PlcPort.SetTxIntEnable(false);
  }
}
//=============================================================================
//
//=======================================
void SPI1_IRQHandler(void)
{
  
}
//=============================================================================