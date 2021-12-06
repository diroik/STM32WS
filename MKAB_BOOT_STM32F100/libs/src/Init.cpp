// *************************************************             
// Инициализация                            
// *************************************************
//=================================================
#include <type_define.h>
#include <params_def.h>
#include <init.h>
#include <stm32f10x.h>
#include <Classes.h>
//#include <flash_map.h>
   

//=================================================
void SYSTEM_INIT(void)
{ 
    nvic_set_vector_table(OFFSET_BOOTLOADER);
    reset_all_periph();

    Init_Flash_Const();   
    
    Init_WDT();   //=== Инициализация WatchDog ===          
    Init_Ports(); //=== Инициализация портов        

    CLEAR_WDT;
    Init_OSC();   //=== инициализация тактовой частоты 
    //TTimer::wait_1ms(5);
    CLEAR_WDT;    
    
__enable_interrupt();
    //Init_BOR();


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
//uint8_t tw = 200;
//IWDG->KR=0x5555; // Ключ для доступа к таймеру
//IWDG->PR=7; // Обновление IWDG_PR делитель
//IWDG->RLR=tw*40/256; // Загрузить регистр перезагрузки
//IWDG->KR=0xAAAA; // Перезагрузка
//IWDG->KR=0xCCCC; // Пуск таймера
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
     TTimer::wait_1ms(100);
      CLEAR_WDT;
      TObject::PtrToBootConstants = (Boot_Params const*)&BOOT_PARAMS;//0x1900;
      TObject::PtrToConstants = (Data_Params const*)&PARAMS;
      //TObject::RestoreDefault();              // Установка поумолчания
      
      if (TObject::PtrToConstants->KeyByte != 0xAA)
      {    
        TObject::PtrToConstants = (Data_Params const*)&TMP_PARAMS;
        if(TObject::PtrToConstants->KeyByte == 0xAA)
        {
          if( TObject::RestoreMemParams() == true )   
          {
            continue;
          }
        }
        else
        {
          TObject::RestoreDefault();              // onoaiiaea ii-oiie?aie?
          TObject::Soft_Reset();             
        }
      }
      else
      { 
        return;
      }                            // anee ana ie - auoia
   }
   TObject::RestoreDefault();              // onoaiiaea ii-oiie?aie?
   TObject::Soft_Reset();
}






//--------------------------------------------------------------------------------------------------
// Установить смещение для таблица векторово во FLASH памяти
//--------------------------------------------------------------------------------------------------
void nvic_set_vector_table(uint32_t Offset)
{ 
__set_PRIMASK(1);           //disable all system interrupts
  SCB->VTOR = NVIC_VectTab_FLASH | (Offset & (uint32_t)0x1FFFFF80);
__set_PRIMASK(0);           //enable all system
}


void reset_all_periph (void)
{
USART1->CR1 = 0;
USART1->BRR = 0;
USART1->CR2 = 0;
    
GPIOA->CRL = 0;
GPIOA->CRH = 0;
GPIOA->IDR = 0;
GPIOA->ODR = 0;


GPIOB->CRL = 0;							
GPIOB->CRH = 0;
GPIOB->ODR = 0;
GPIOB->IDR = 0;

GPIOC->CRL = 0;										
GPIOC->CRH = 0;
GPIOC->IDR = 0;
GPIOC->ODR = 0;

FLASH->SR = 0;


RCC->APB1ENR = 0x00000;						// GPIO   dissable
RCC->APB2ENR = 0x00000;						// USART2   dissable  
  
}