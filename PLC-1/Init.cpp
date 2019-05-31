// *************************************************             
// Инициализация                            
// *************************************************
//=================================================
#include <init.h>
#include <Classes.h>
#include <flash_map.h>
//=============================================================================
extern Data_Params* PtrToConstants;
extern Boot_Params* PtrToBootConstants;
//=================================================
void SYSTEM_INIT(void)
{ 
    __disable_interrupt();
    
    Init_WDT();   //=== Инициализация WatchDog ===          
    Init_Ports(); //=== Инициализация портов        

    CLEAR_WDT;
    Init_OSC();   //=== инициализация тактовой частоты 
    CLEAR_WDT;    
    //PtrToConstants = (Data_Params*)&PARAMS;
    Init_Flash_Const();
    Init_TIM7();
    Init_SysTick(); 
}
//=================================================
//======= Функции инициализации ===================
//=================================================
void Init_Ports(void)
{
RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
RCC->APB2ENR |= RCC_APB2ENR_IOPAEN; // Разрешить тактирование PORTA.
RCC->APB2ENR |= RCC_APB2ENR_IOPBEN; // Разрешить тактирование PORTA.
RCC->APB2ENR |= RCC_APB2ENR_IOPCEN; // Разрешить тактирование PORTA..
GPIOA->CRH = 0x00;
GPIOA->CRL = 0x00;

GPIOB->CRH = 0x00;
GPIOB->CRL = 0x00;

GPIOC->CRH = 0x00;
GPIOC->CRL = 0x00;

GPIOA->CRL |= GPIO_CRL_MODE4_1;                 //SPI_UART


GPIOB->CRH |= GPIO_CRH_MODE12_1;                 //REG_DATA

GPIOB->CRL |= GPIO_CRL_MODE0_1;                 //_RXTX

GPIOA->BRR |= GPIO_PIN_4;                                       //SPI_UART = 0 / SET SPI

GPIOC->CRH |= GPIO_CRH_MODE13_1;                 //LED1
GPIOC->CRH |= GPIO_CRH_MODE15_1;                 //LED2


GPIOC->BRR |= GPIO_PIN_13;                      // Включить светодиод Green  
GPIOC->BRR |= GPIO_PIN_15;                      // Включить светодиод Red


//GPIOB->ODR ^= GPIO_PIN_0;

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
  // Параметр tw 
  int tw = 950;// 950=~1000ms
  IWDG->KR=0x5555; // Ключ для доступа к таймеру
  IWDG->PR=7; // Обновление IWDG_PR делитель
  IWDG->RLR=tw*40/256; // Загрузить регистр перезагрузки
  IWDG->KR=0xAAAA; // Перезагрузка
  IWDG->KR=0xCCCC; // Пуск таймера
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
void Init_Flash_Const(void)
{
  Word CNT = 1000;
   while(CNT--)
   {
      TTimer::wait_1ms(10);//CLEAR_WDT in wait_1ms
      PtrToBootConstants = (Boot_Params*)BOOT_MEM_START;
      PtrToConstants = (Data_Params*)PARAMS_ADDR;
      
      if (PtrToConstants->KeyByte != 0xAA)
      {    
        PtrToConstants = (Data_Params*)TMP_PARAMS_ADDR;
        if(PtrToConstants->KeyByte == 0xAA)
        {
          if( TObject::RestoreMemParams(PtrToConstants) == true )   
          {  continue;}
        }
      }
      else
      { return;}                          
   }
   TObject::RestoreDefault();              
   TObject::Soft_Reset();
}
