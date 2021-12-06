// *************************************************             
// Инициализация                            
// *************************************************
//=================================================
#include <type_define.h>
#include <params_def.h>
#include <init.h>
#include <stm32f10x.h>
#include <Classes.h>
#include <stm32f10x.h>
#include <stm32f10x_tim.h>
//=================================================
void SYSTEM_INIT(void)
{ 
    __disable_interrupt();
    Init_WDT();   //=== Инициализация WatchDog ===          
    Init_Ports(); //=== Инициализация портов        

    CLEAR_WDT;
    Init_OSC();   //=== инициализация тактовой частоты 
    CLEAR_WDT;    
    

    Init_Flash_Const(); 
    Init_Task_Flash_Const();

    Init_TIM7();
    Init_SysTick(); 
    Init_ADC12();
}
//=================================================
//======= Функции инициализации ===================
//=================================================
void Init_Ports(void)
{
//GPIOA->IDR = 0;
//GPIOA->ODR = 0;
//GPIOA->MODER = 0;
//GPIOA->OTYPER = 0;
//GPIOA->OSPEEDR = 0;
//
//GPIOB->ODR = 0;
//GPIOB->IDR = 0;
//GPIOB->MODER = 0;
//GPIOB->OTYPER = 0;
//GPIOB->OSPEEDR = 0;
//
//GPIOC->IDR = 0;
//GPIOC->ODR = 0;
//GPIOC->MODER = 0;
//GPIOC->OTYPER = 0;
//GPIOC->OSPEEDR = 0;
//
RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
RCC->APB2ENR |= RCC_APB2ENR_IOPAEN; // Разрешить тактирование PORTA.
RCC->APB2ENR |= RCC_APB2ENR_IOPBEN; // Разрешить тактирование PORTA.
RCC->APB2ENR |= RCC_APB2ENR_IOPCEN; // Разрешить тактирование PORTA..




}
//=================================================
void Init_OSC(void)// тактовой частоты                     
{
  //Turn ON HSI
  RCC->CR|=RCC_CR_HSION;
  
  //Wait until it's stable 
  while (!(RCC->CR & RCC_CR_HSIRDY));
  
  //Switch to HSI as SYSCLK
  //PLL input = HSI
  //PLL division factor = 2
  //PLL multiplication factor = 4
  RCC->CFGR|=RCC_CFGR_HPRE_DIV1 | RCC_CFGR_PLLMULL_0;
  
  //Turn PLL on
  RCC->CR|=RCC_CR_PLLON;
  
  //Wait PLL to stabilize
  while (!(RCC->CR & RCC_CR_PLLRDY));
  
  //Setting up flash for high speed
//  FLASH->ACR=FLASH_ACR_ACC64;
//  FLASH->ACR|=FLASH_ACR_LATENCY;
//  FLASH->ACR|=FLASH_ACR_PRFTEN;
  
  //Set PLL as SYSCLK
  RCC->CFGR|=RCC_CFGR_SW_0 | RCC_CFGR_SW_0;
  
  //Turn off MSI
//  RCC->CR&=~RCC_CR_MSION;

}    
//=================================================
void Init_WDT(void)
{
// Параметр tw от 6.918мс до 28339мс  
  int tw = 950;        //~1s
  IWDG->KR=0x5555;      // Ключ для доступа к таймеру
  IWDG->PR=7;           // Обновление IWDG_PR делитель
  IWDG->RLR=tw*40/256;  // Загрузить регистр перезагрузки
  IWDG->KR=0xAAAA;      // Перезагрузка
  IWDG->KR=0xCCCC;      // Пуск таймера
  CLEAR_WDT;
}
//=================================================

void Init_TIM7(void)
{
  RCC->APB1ENR |= RCC_APB1ENR_TIM7EN; // Разрешить тактирование TIM6
  TIM7->PSC = SMCLK; // Настраиваем делитель что таймер тикал 1000 раз в секунду
  TIM7->ARR = 1000; // Чтоб прерывание случалось раз в секунду
  TIM7->DIER |= TIM_DIER_UIE; //разрешаем прерывание от таймера
  TIM7->CR1 |= TIM_CR1_CEN; // Начать отсчёт!
    
  NVIC_EnableIRQ(TIM7_IRQn); //Разрешение TIM6_DAC_IRQn прерывания  
}

//=================================================
//Таймер раз в милисекунду
//=================================================

void Init_SysTick(void)
{
if (SysTick_Config(8000000 / 1000))	 // задать прерывани¤ от системного таймера раз в 1мс
  {
   while (1);									 // остановка в случае ошибки
  }
}
//=================================================


void Init_SysTick(long val)
{
//RCC->APB1ENR |= RCC_APB2ENR_SYSCFGEN; // Разрешить тактирование SYSTICK
//
//  SysTick->LOAD  = (uint32_t)(SMCLK);                         /* set reload register */
//  NVIC_SetPriority (SysTick_IRQn, (1UL << __NVIC_PRIO_BITS) - 1UL); /* set Priority for Systick Interrupt */
//  SysTick->VAL   = 0UL;                                             /* Load the SysTick Counter Value */
//  SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk |
//                   SysTick_CTRL_TICKINT_Msk   |
//                   SysTick_CTRL_ENABLE_Msk;                         /* Enable SysTick IRQ and SysTick Timer */
}
//=================================================

void Init_ADC12(void)
{  

RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
GPIOA->CRL = 0;  
ADC1->CR2    |= ADC_CR2_CONT;
ADC1->CR2    |= ADC_CR2_EXTTRIG;// External trigger conversion mode for regular channels

ADC1->CR2    |= ADC_CR2_TSVREFE;                // Включение температурного сенсора

ADC1->SQR1   = 0x0;

//1 start by Vref 
ADC1->SMPR1 = (DWord)ADC_SampleTime_384Cycles << (3 * 0x7);
ADC1->SQR3 = 0x11;

ADC1->CR2    |= ADC_CR2_CAL;                    // Калибровка


}
//=================================================
void Start_ADC12(void)
{  
ADC1->CR2 |= ADC_CR2_ADON;
ADC1->CR1 |= ADC_CR1_EOCIE;
ADC1->CR2 |= ADC_CR2_EXTTRIG;
ADC1->CR2 |=  ADC_CR2_EXTSEL;
ADC1->CR2 |= ADC_CR2_SWSTART; 
NVIC_EnableIRQ(ADC1_IRQn);
}
//=================================================
void Stop_ADC12(void)
{
  ADC1->CR2 &= ~ADC_CR2_ADON;
  ADC1->SR &= ~ADC_SR_EOC;
  ADC1->CR1 &= ~ADC_CR1_EOCIE;
  ADC1->CR2 &= ~ADC_CR2_SWSTART; 
  NVIC_DisableIRQ(ADC1_IRQn);
  
}
//=================================================
void Init_Flash_Const(void)
{
Word CNT = 1000;

   while(CNT--)
   {
     TTimer::wait_1ms(100);
      CLEAR_WDT;
      TObject::PtrToBootConstants = (Boot_Params const*)&BOOT_PARAMS;//0x1900;
      TObject::PtrToConstants = (Data_Params const*)&PARAMS;
      
      if (TObject::PtrToConstants->KeyByte != 0xAA)
      {    
        TObject::PtrToConstants = (Data_Params const*)&TMP_PARAMS;
        if(TObject::PtrToConstants->KeyByte == 0xAA)
        {
          if( TObject::RestoreMemParams() == true )   
            continue;
        }
      }
      else
        return;            
   }
   TObject::RestoreDefault();             
   TObject::Soft_Reset();
}
//=================================================
//#pragma optimize=z none
void Init_Task_Flash_Const(void)          //
{
Word CNT = 1000;

   while(CNT--)
   {
     TTimer::wait_1ms(100);
      CLEAR_WDT;
      TObjectTask::TaskListConfig = &TASK_PARAMS;
      Word crc = TObjectTask::CalcCRC(TObjectTask::TaskListConfig);
      bool firstStart = TObjectTask::TaskListConfig->IsFistStart;
      if(TObjectTask::TaskListConfig->KeyByte == 0xAA && firstStart !=0)
      {
        TTaskListConfig taskListConfig = *TObjectTask::TaskListConfig;
        taskListConfig.IsFistStart = 0; 
        TObjectTask::Save_TaskConfigInFlash(&taskListConfig);
      }
      if ( TObjectTask::TaskListConfig->KeyByte != 0xAA || TObjectTask::TaskListConfig->CRC_SUM !=  crc )
      {    
        TObjectTask::TaskListConfig = &TMP_TASK_PARAMS;
        crc = TObjectTask::CalcCRC(TObjectTask::TaskListConfig);
        if(TObjectTask::TaskListConfig->KeyByte == 0xAA && TObjectTask::TaskListConfig->CRC_SUM == crc)
        {
          if( TObjectTask::RestoreMemParams() == true )   
          {  continue;}
        }
      }
      else
      { return;}                             // если все ок - выход
   }
   TObjectTask::RestoreDefault();            // установка по-умолчанию
   TObject::Soft_Reset();

}//TaskListConfig

