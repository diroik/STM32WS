// *************************************************             
// Инициализация                            
// *************************************************
//=================================================
#include <type_define.h>
#include <params_def.h>
#include <init.h>
#include <stm32l1xx.h>
#include <stm32l1xx_rcc.h>
#include <stm32l1xx_gpio.h>
#include <stm32l1xx_adc.h>
#include <stm32l1xx_rcc.h>
#include <Classes.h>
#include <string.h>
//=================================================
void SYSTEM_INIT(void)
{ 
  RCC_ClocksTypeDef RCC_ClockFreq;
    __disable_interrupt();

    Init_OSC();   //=== инициализация тактовой частоты 
    Init_WDT();   //=== Инициализация WatchDog ===         
    
    Init_Ports(); //=== Инициализация портов        
    //GPIOC->ODR |= GPIO_ODR_ODR_13;//13    
    RCC_GetClocksFreq(&RCC_ClockFreq);
    
    CLEAR_WDT;    
    Init_Flash_Const(); 

    Init_TIM7();
    Init_SysTick(); 
    Init_ADC12();
}
//=================================================
//======= Функции инициализации ===================
//=================================================
void Init_Ports(void)
{
  GPIO_InitTypeDef        GPIO_InitStructure;

  /* GPIOA Periph clock enable for ADC*/
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
  
  /* GPIOB Periph clock enable for USART*/
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
  
    /* GPIOC Periph clock enable */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);


  /* Configure PD12 output pushpull mode */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
}
//=================================================
void Init_OSC(void)// тактовой частоты                     
{
  //Turn ON HSI
  RCC->CR|=RCC_CR_HSION;
  
  //Wait until it's stable 
  while (!(RCC->CR & RCC_CR_HSIRDY));
  
  
  RCC->CSR|= RCC_CSR_LSION;
  while (!(RCC->CSR & RCC_CSR_LSIRDY));
  
  //Switch to HSI as SYSCLK
  //PLL input = HSI
  //PLL division factor = 2
  //PLL multiplication factor = 4
  RCC->CFGR|=RCC_CFGR_HPRE_DIV2 | RCC_CFGR_PLLMUL3;// | RCC_CFGR_MCO_SYSCLK;//RCC_CFGR_PLLMULL_0
  
  //Turn PLL on
  //RCC->CR|= RCC_CR_PLLON;//RCC_PLL_NONE;// RCC_CR_PLLON;
  //RCC->CR&= ~RCC_CR_PLLON;//PLL is not configured 
  
  //Wait PLL to stabilize
  //while (!(RCC->CR & RCC_CR_PLLRDY));

  FLASH_SetLatency(FLASH_Latency_0);
  
  //Set PLL as SYSCLK
  //RCC->CFGR|= RCC_CFGR_SW_HSI;// RCC_CFGR_SW_0 | RCC_CFGR_SW_0;

   /* Select HSI as system clock source */
    RCC_SYSCLKConfig(RCC_CFGR_SW_HSI);
}    
//=================================================
void Init_WDT(void)
{
// Параметр tw от 6.918мс до 28339мс  
  int tw = 950;        //~1s
  IWDG->KR=0x5555;      // Ключ для доступа к таймеру
  IWDG->PR=7;           // Обновление IWDG_PR делитель
  IWDG->RLR=tw*37/256;  // Загрузить регистр перезагрузки
  IWDG->KR=0xAAAA;      // Перезагрузка
  IWDG->KR=0xCCCC;      // Пуск таймера
}
//=================================================

void Init_TIM7(void)
{
  RCC->APB1ENR |= RCC_APB1ENR_TIM7EN; // Разрешить тактирование TIM6
  TIM7->PSC = SMCLK; // Настраиваем делитель что таймер тикал 1000 раз в секунду
  TIM7->ARR = 1015; // Чтоб прерывание случалось раз в секунду //+15 это корректировка 
  TIM7->DIER |= TIM_DIER_UIE; //разрешаем прерывание от таймера
  TIM7->CR1 |= TIM_CR1_CEN; // Начать отсчёт!
    
  NVIC_EnableIRQ(TIM7_IRQn); //Разрешение TIM7_DAC_IRQn прерывания  
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
void DMA_Config(void)
{
  /*
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
  DMA_DeInit(DMA1_Channel1);
  DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_ADDRESS;
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&ADC_ConvertedValue;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
  DMA_InitStructure.DMA_BufferSize = 1;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
  DMA_Init(DMA1_Channel1, &DMA_InitStructure);
  
  DMA_Cmd(DMA1_Channel1, ENABLE);
  */
}
//=================================================
void Init_ADC12(void)
{   
  GPIO_InitTypeDef PORT_init_struct;
  ADC_InitTypeDef ADC_InitStructure;
  memset(&PORT_init_struct, 0, sizeof(GPIO_InitTypeDef));
  memset(&ADC_InitStructure, 0, sizeof(ADC_InitTypeDef));
    
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
  
  PORT_init_struct.GPIO_Pin     = GPIO_Pin_0;
  PORT_init_struct.GPIO_Mode    = GPIO_Mode_AIN;
  PORT_init_struct.GPIO_PuPd    = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOA, &PORT_init_struct);
  
  PORT_init_struct.GPIO_Pin     = GPIO_Pin_1;
  PORT_init_struct.GPIO_Mode    = GPIO_Mode_AIN;
  PORT_init_struct.GPIO_PuPd    = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOA, &PORT_init_struct);
  
  PORT_init_struct.GPIO_Pin     = GPIO_Pin_2;
  PORT_init_struct.GPIO_Mode    = GPIO_Mode_AIN;
  PORT_init_struct.GPIO_PuPd    = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOA, &PORT_init_struct);
  
  PORT_init_struct.GPIO_Pin     = GPIO_Pin_3;
  PORT_init_struct.GPIO_Mode    = GPIO_Mode_AIN;
  PORT_init_struct.GPIO_PuPd    = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOA, &PORT_init_struct);
  
  PORT_init_struct.GPIO_Pin     = GPIO_Pin_4;
  PORT_init_struct.GPIO_Mode    = GPIO_Mode_AIN;
  PORT_init_struct.GPIO_PuPd    = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOA, &PORT_init_struct);
  
  PORT_init_struct.GPIO_Pin     = GPIO_Pin_5;
  PORT_init_struct.GPIO_Mode    = GPIO_Mode_AIN;
  PORT_init_struct.GPIO_PuPd    = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOA, &PORT_init_struct);
  
  PORT_init_struct.GPIO_Pin     = GPIO_Pin_6;
  PORT_init_struct.GPIO_Mode    = GPIO_Mode_AIN;
  PORT_init_struct.GPIO_PuPd    = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOA, &PORT_init_struct);
  
  PORT_init_struct.GPIO_Pin     = GPIO_Pin_7;
  PORT_init_struct.GPIO_Mode    = GPIO_Mode_AIN;
  PORT_init_struct.GPIO_PuPd    = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOA, &PORT_init_struct);
  

  //ADC->CCR |= ADC_CCR_ADCPRE_0 | ADC_CCR_TSVREFE;
  ADC_InitStructure.ADC_Resolution              = ADC_Resolution_12b;
  ADC_InitStructure.ADC_ScanConvMode            = DISABLE;
  ADC_InitStructure.ADC_ContinuousConvMode      = ENABLE;//DISABLE
  ADC_InitStructure.ADC_ExternalTrigConvEdge    = ADC_ExternalTrigConvEdge_None;
  ADC_InitStructure.ADC_ExternalTrigConv        = 0;        
  ADC_InitStructure.ADC_DataAlign               = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NbrOfConversion         = 1;
  ADC_Init(ADC1, &ADC_InitStructure);
  //ADC1->SQR1   = 0x0;

  ADC_RegularChannelConfig(ADC1, ADC_Channel_Vrefint,   1, ADC_SampleTime_384Cycles); 
  /*
  ADC_RegularChannelConfig(ADC1, ADC_Channel_0,         2, ADC_SampleTime_384Cycles);
  ADC_RegularChannelConfig(ADC1, ADC_Channel_1,         3, ADC_SampleTime_384Cycles);
  ADC_RegularChannelConfig(ADC1, ADC_Channel_2,         4, ADC_SampleTime_384Cycles);
  ADC_RegularChannelConfig(ADC1, ADC_Channel_3,         5, ADC_SampleTime_384Cycles);
  ADC_RegularChannelConfig(ADC1, ADC_Channel_4,         6, ADC_SampleTime_384Cycles);
  ADC_RegularChannelConfig(ADC1, ADC_Channel_5,         7, ADC_SampleTime_384Cycles);
  ADC_RegularChannelConfig(ADC1, ADC_Channel_6,         8, ADC_SampleTime_384Cycles);
  ADC_RegularChannelConfig(ADC1, ADC_Channel_7,         9, ADC_SampleTime_384Cycles);*/

  ADC_TempSensorVrefintCmd(ENABLE); 
  ADC_Cmd(ADC1, ENABLE);

}
//=================================================
void Start_ADC12(void)
{  
  
    /* Enable ADC1 */
  //ADC_Cmd(ADC1, ENABLE);

  /* Start ADC1 Software Conversion */ 
  //ADC_SoftwareStartConv(ADC1);
  
  ADC1->CR2 |= ADC_CR2_ADON;
  ADC1->CR1 |= ADC_CR1_EOCIE;

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
      //TObject::PtrToBootConstants = (Boot_Params const*)&BOOT_PARAMS;//0x1900;
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
