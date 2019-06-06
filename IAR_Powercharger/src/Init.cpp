// *************************************************             
// �������������                            
// *************************************************
//=================================================
#include <type_define.h>
#include <params_def.h>
#include <init.h>
#include <stm32f10x.h>
#include <Classes.h>
#include <stm32f10x.h>
#include <stm32f10x_tim.h>

#define ADC_Channel_0                              ((uint8_t)0x00)
#define ADC_Channel_1                              ((uint8_t)0x01)
#define ADC_Channel_2                              ((uint8_t)0x02)
#define ADC_Channel_3                              ((uint8_t)0x03)
#define ADC_Channel_4                              ((uint8_t)0x04)
#define ADC_Channel_5                              ((uint8_t)0x05)
#define ADC_Channel_6                              ((uint8_t)0x06)
#define ADC_Channel_7                              ((uint8_t)0x07)
#define ADC_Channel_8                              ((uint8_t)0x08)

#define ADC_SampleTime_4Cycles                     ((uint8_t)0x00)
#define ADC_SampleTime_9Cycles                     ((uint8_t)0x01)
#define ADC_SampleTime_16Cycles                    ((uint8_t)0x02)
#define ADC_SampleTime_24Cycles                    ((uint8_t)0x03)
#define ADC_SampleTime_48Cycles                    ((uint8_t)0x04)
#define ADC_SampleTime_96Cycles                    ((uint8_t)0x05)
#define ADC_SampleTime_192Cycles                   ((uint8_t)0x06)
#define ADC_SampleTime_384Cycles                   ((uint8_t)0x07)


    

//=================================================
void SYSTEM_INIT(void)
{ 
    Init_WDT();   //=== ������������� WatchDog ===          
    Init_Ports(); //=== ������������� ������        

    CLEAR_WDT;
    Init_OSC();   //=== ������������� �������� ������� 
    //TTimer::wait_1ms(10);
    CLEAR_WDT;    
    
__enable_interrupt();
    //Init_BOR();
    Init_Flash_Const(); 
    Init_Task_Flash_Const();
    Init_Calibr_Flash_Const();
    Init_SRAM_Const();
//__disable_interrupt();
    //Init_TIMER_A0();
    //Init_TIMER_A1();
    Init_TIM7();
    Init_SysTick(); 
    Init_ADC12();
}
//=================================================
//======= ������� ������������� ===================
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
RCC->APB2ENR |= RCC_APB2ENR_IOPAEN; // ��������� ������������ PORTA.
RCC->APB2ENR |= RCC_APB2ENR_IOPBEN; // ��������� ������������ PORTB.
RCC->APB2ENR |= RCC_APB2ENR_IOPCEN; // ��������� ������������ PORTC.
RCC->APB1ENR |= RCC_APB1ENR_DACEN;  //������������ ���

}
//=================================================
void Init_OSC(void)// �������� �������                     
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
// �������� tw �� 6.918�� �� 28339��  
//uint8_t tw = 200;
//IWDG->KR=0x5555; // ���� ��� ������� � �������
//IWDG->PR=7; // ���������� IWDG_PR ��������
//IWDG->RLR=tw*40/256; // ��������� ������� ������������
//IWDG->KR=0xAAAA; // ������������
//IWDG->KR=0xCCCC; // ���� �������
  CLEAR_WDT;
}
//=================================================

void Init_TIM7(void)
{
  RCC->APB1ENR |= RCC_APB1ENR_TIM7EN; // ��������� ������������ TIM6
  TIM7->PSC = SMCLK; // ����������� �������� ��� ������ ����� 1000 ��� � �������
  TIM7->ARR = 100; // ���� ���������� ��������� 10 ��� � �������
  TIM7->DIER |= TIM_DIER_UIE; //��������� ���������� �� �������
  TIM7->CR1 |= TIM_CR1_CEN; // ������ ������!
    
  NVIC_EnableIRQ(TIM7_IRQn); //���������� TIM6_DAC_IRQn ����������  
}

//=================================================
//������ ��� � �����������
//=================================================

void Init_SysTick(void)
{
if (SysTick_Config(16000000 / 1000))	 // ������ ��������� �� ���������� ������� ��� � 1��
  {
   while (1);									 // ��������� � ������ ������
  }
}
//=================================================
void Init_SysTick(long val)
{
//RCC->APB1ENR |= RCC_APB2ENR_SYSCFGEN; // ��������� ������������ SYSTICK
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
//������������ ���
RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
//���������� � ���� �������� ����� AIN
GPIOA->CRL&=~(GPIO_CRL_CNF0+GPIO_CRL_MODE0+GPIO_CRL_CNF1+GPIO_CRL_MODE1+GPIO_CRL_CNF3+GPIO_CRL_MODE3+GPIO_CRL_CNF7+GPIO_CRL_MODE7); // AIN0, AIN1, AIN3, AIN7
GPIOB->CRL&=~(GPIO_CRL_CNF0+GPIO_CRL_MODE0); // Uin 
ADC1->CR2    |= ADC_CR2_CONT; // Continuous conversion mode 
ADC1->CR2    |= ADC_CR2_EXTTRIG;//�������� ������ �������������� �� �������� ��������.

ADC1->CR2    |= ADC_CR2_TSVREFE;                // ��������� �������������� �������

ADC1->SQR1   = 0x0;     //1 ��������������

//1
ADC1->SMPR1 = (DWord)ADC_SampleTime_384Cycles << (3 * 0x7);
ADC1->SQR3 = 0x11;

ADC1->CR2    |= ADC_CR2_CAL;                    // ����������


}
//=================================================
void Start_ADC12(void)
{  
ADC1->CR2 |= ADC_CR2_ADON;//�������� ���
ADC1->CR1 |= ADC_CR1_EOCIE;//�������� ���������� �� ��������� ��������������
ADC1->CR2 |= ADC_CR2_EXTTRIG;//�������� ������ �������������� �� �������� ��������.
ADC1->CR2 |= ADC_CR2_EXTSEL;//��������� ����� ������� ��� �������"Software control bit"
ADC1->CR2 |= ADC_CR2_SWSTART;//��������� ��������������. ����� ��������� ������������.
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
      TObject::PtrToBootConstants = (Boot_Params const*)0x08007000;//0x1900;
      TObject::PtrToConstants = (Data_Params const*)&PARAMS;
      TObject::RestoreDefault();              // ��������� �����������
      
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
          //TObject::RestoreDefault();              // onoaiiaea ii-oiie?aie?
          //TObject::Soft_Reset();             
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
//=================================================
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
        else                                // ��������� ��-���������   
        { 
          //TObjectTask::RestoreDefault();
        }
      }
      else
      { return;}                             // ���� ��� �� - �����
   }
   TObjectTask::RestoreDefault();            // ��������� ��-���������
   TObject::Soft_Reset();

}//TaskListConfig
//=================================================
void Init_Calibr_Flash_Const(void)          //
{
Word CNT = 1000;

   while(CNT--)
   {
     TTimer::wait_1ms(100);
      CLEAR_WDT;
      TObjectCalibr::CalibrListConfig = &CALIBR_PARAMS;
      Word crc = TObjectCalibr::CalcCRC(TObjectCalibr::CalibrListConfig);
      bool firstStart = TObjectCalibr::CalibrListConfig->IsFistStart;
      if(TObjectCalibr::CalibrListConfig->KeyByte == 0xAA && firstStart !=0)
      {
        TCalibrListConfig CalibrListConfig = *TObjectCalibr::CalibrListConfig;
        CalibrListConfig.IsFistStart = 0; 
        TObjectCalibr::Save_CalibrConfigInFlash(&CalibrListConfig);
      }
      if ( TObjectCalibr::CalibrListConfig->KeyByte != 0xAA || TObjectCalibr::CalibrListConfig->CRC_SUM !=  crc )
      {    
        TObjectCalibr::CalibrListConfig = &TMP_CALIBR_PARAMS;
        crc = TObjectCalibr::CalcCRC(TObjectCalibr::CalibrListConfig);
        if(TObjectCalibr::CalibrListConfig->KeyByte == 0xAA && TObjectCalibr::CalibrListConfig->CRC_SUM == crc)
        {
          if( TObjectCalibr::RestoreMemParams() == true )   
          {  
            continue;
          }
        }
        else                                // ��������� ��-���������   
        { 
          //TObjectCalibr::RestoreDefault();
        }
      }
      else
      { 
        return;
      }                             // ���� ��� �� - �����
   }
   TObjectCalibr::RestoreDefault();            // ��������� ��-���������
   TObject::Soft_Reset();
}
//=================================================
void Init_SRAM_Const(void) 
{

}
