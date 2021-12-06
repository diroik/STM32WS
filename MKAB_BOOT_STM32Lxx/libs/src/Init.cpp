// *************************************************             
// �������������                            
// *************************************************
//=================================================
#include <type_define.h>
#include <params_def.h>
#include <init.h>
#include <stm32l1xx.h>
#include <stm32l1xx_rcc.h>
#include <stm32l1xx_gpio.h>
#include <Classes.h>
//#include <flash_map.h>
//=================================================
void SYSTEM_INIT(void)
{ 
    //nvic_set_vector_table(OFFSET_BOOTLOADER);
    //reset_all_periph();
    Init_OSC();   //=== ������������� �������� ������� 
    Init_WDT();   //=== ������������� WatchDog ===          
    Init_Ports(); //=== ������������� ������        
    CLEAR_WDT;
    Init_Flash_Const();   
    //TTimer::wait_1ms(5);
    CLEAR_WDT;    
    
//__enable_interrupt();
    //Init_BOR();

    Init_TIM7();
    Init_SysTick(); 
}
//=================================================
//======= ������� ������������� ===================
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
void Init_OSC(void)// �������� �������                     
{
    //RCC_HSICmd(ENABLE);
  
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
  RCC->CFGR|=RCC_CFGR_HPRE_DIV2 | RCC_CFGR_PLLMUL3;//RCC_CFGR_PLLMULL_0
  
  //Turn PLL on
  ///RCC->CR|= RCC_CR_PLLON;//RCC_PLL_NONE;// RCC_CR_PLLON;
  
  //Wait PLL to stabilize
  ///while (!(RCC->CR & RCC_CR_PLLRDY));
  
  //Setting up flash for high speed
//  FLASH->ACR=FLASH_ACR_ACC64;
//  FLASH->ACR|=FLASH_ACR_LATENCY;
//  FLASH->ACR|=FLASH_ACR_PRFTEN;
  FLASH_SetLatency(FLASH_Latency_0);
  
  //Set PLL as SYSCLK
  RCC->CFGR|= RCC_CFGR_SW_HSI;
  
  //Turn off MSI
//  RCC->CR&=~RCC_CR_MSION;

}      
//=================================================
void Init_WDT(void)
{
// �������� tw �� 6.918�� �� 28339��  
  int tw = 950;        //~1s
  IWDG->KR=0x5555;      // ���� ��� ������� � �������
  IWDG->PR=7;           // ���������� IWDG_PR ��������
  IWDG->RLR=tw*37/256;  // ��������� ������� ������������
  IWDG->KR=0xAAAA;      // ������������
  IWDG->KR=0xCCCC;      // ���� �������
  CLEAR_WDT;
}
//=================================================

void Init_TIM7(void)
{
  RCC->APB1ENR |= RCC_APB1ENR_TIM7EN; // ��������� ������������ TIM6
  TIM7->PSC = SMCLK; // ����������� �������� ��� ������ ����� 1000 ��� � �������
  TIM7->ARR = 1015; // ���� ���������� ��������� ��� � �������
  TIM7->DIER |= TIM_DIER_UIE; //��������� ���������� �� �������
  TIM7->CR1 |= TIM_CR1_CEN; // ������ ������!  
  NVIC_EnableIRQ(TIM7_IRQn); //���������� TIM6_DAC_IRQn ����������  
}

//=================================================
//������ ��� � �����������
//=================================================

void Init_SysTick(void)
{
if (SysTick_Config(8000000 / 1000))	 // ������ ��������� �� ���������� ������� ��� � 1��
  {
   while (1);									 // ��������� � ������ ������
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
   TObject::RestoreDefault();              // onoaiiaea ii-oiie?aie?
   TObject::Soft_Reset();
}
//--------------------------------------------------------------------------------------------------
// ���������� �������� ��� ������� ��������� �� FLASH ������
//--------------------------------------------------------------------------------------------------
void nvic_set_vector_table(uint32_t Offset)
{ 
  __set_PRIMASK(1);           //disable all system interrupts
    SCB->VTOR = NVIC_VectTab_FLASH | (Offset & (uint32_t)0x1FFFFF80);
  __set_PRIMASK(0);           //enable all system
}
//--------------------------------------------------------------------------------------------------
void reset_all_periph (void)
{
    USART1->CR1 = 0;
    USART1->BRR = 0;
    USART1->CR2 = 0;

    GPIOA->MODER = 0;
    GPIOA->PUPDR = 0;
    GPIOA->IDR = 0;
    GPIOA->ODR = 0;

    GPIOB->MODER = 0;
    GPIOB->PUPDR = 0;
    GPIOB->ODR = 0;
    GPIOB->IDR = 0;

    GPIOC->MODER = 0;
    GPIOC->PUPDR = 0;
    GPIOC->IDR = 0;
    GPIOC->ODR = 0;

    FLASH->SR = 0;
    RCC->APB1ENR = 0x00000;						// GPIO   dissable
    RCC->APB2ENR = 0x00000;						// USART2   dissable  
}
//--------------------------------------------------------------------------------------------------