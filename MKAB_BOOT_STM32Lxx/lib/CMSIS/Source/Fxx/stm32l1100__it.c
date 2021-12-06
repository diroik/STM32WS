/**
  ******************************************************************************
  * @file    SysTick/SysTick_Example/stm32f4xx_it.c 
  * @author  MCD Application Team
  * @version V1.6.1
  * @date    21-October-2015
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2015 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
//#include	"stm32f446_it.h"

//#include	"stdint.h"
#include <stm32f10x.h>
 
    
//void TIM6_IRQ (void);

//void TIM7_IRQ (void);



/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/



/* Private function prototypes -----------------------------------------------*/

/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
/******************************************************************************/
// Enable the IAR extensions for this source file.
#pragma language=extended
#pragma segment="CSTACK"


// Forward declaration of the default fault handlers.
void Reset_Handler(void);
void NMI_Handler(void);
void HardFault_Handler(void);
void IntDefaultHandler(void);
void MemManage_Handler(void);
void BusFault_Handler(void);
void UsageFault_Handler(void);
void SVC_Handler(void);
void DebugMon_Handler(void);
void PendSV_Handler(void);
void SysTick_Handler(void);

// External Interrupts

__weak void WWDG_IRQHandler(void);
__weak void PVD_IRQHandler(void);
__weak void TAMPER_IRQHandler(void);
__weak void RTC_IRQHandler(void);
__weak void FLASH_IRQHandler(void);
__weak void RCC_IRQHandler(void);
__weak void EXTI0_IRQHandler(void);
__weak void EXTI1_IRQHandler(void);
__weak void EXTI2_IRQHandler(void);
__weak void EXTI3_IRQHandler(void);
__weak void EXTI4_IRQHandler(void);
__weak void DMA1_Channel1_IRQHandler(void);
__weak void DMA1_Channel2_IRQHandler(void);
__weak void DMA1_Channel3_IRQHandler(void);
__weak void DMA1_Channel4_IRQHandler(void);
__weak void DMA1_Channel5_IRQHandler(void);
__weak void DMA1_Channel6_IRQHandler(void);
__weak void DMA1_Channel7_IRQHandler(void);
__weak void ADC1_IRQHandler(void);
__weak void EXTI9_5_IRQHandler(void);
__weak void TIM1_BRK_TIM15_IRQHandler(void);
__weak void TIM1_UP_TIM16_IRQHandler(void);
__weak void TIM1_TRG_COM_TIM17_IRQHandler(void);
__weak void TIM1_CC_IRQHandlerm(void);
__weak void TIM2_IRQHandler(void);
__weak void TIM3_IRQHandler(void);
__weak void TIM4_IRQHandler(void);
__weak void I2C1_EV_IRQHandler(void);
__weak void I2C1_ER_IRQHandler(void);
__weak void I2C2_EV_IRQHandler(void);
__weak void I2C2_ER_IRQHandler(void);
__weak void SPI1_IRQHandler(void);
__weak void SPI2_IRQHandler(void);
__weak void USART1_IRQHandler(void);
__weak void USART2_IRQHandler(void);
__weak void USART3_IRQHandler(void);
__weak void EXTI15_10_IRQHandler(void);
__weak void RTCAlarm_IRQHandler(void);
__weak void CEC_IRQHandler(void);
__weak void TIM6_DAC_IRQHandler(void);
__weak void TIM7_IRQHandler(void);

/* Private functions ---------------------------------------------------------*/



// The entry point for the application startup code.
//extern void	__iar_program_start		(void);
extern "C" void __iar_program_start( void );

// A union that describes the entries of the vector table.  The union is needed
// since the first entry is the stack pointer and the remainder are function
// pointers.
typedef union
{
    void (*pfnHandler)(void);
    void * ulPtr;
}
uVectorEntry;

// The vector table.  Note that the proper constructs must be placed on this to
// ensure that it ends up at physical address 0x0000.0000.
#pragma location=".intvec" 
__root const uVectorEntry __vector_table[] =
{
{ .ulPtr = __sfe( "CSTACK" ) },	// sfe(CSTACK) The initial stack pointer
Reset_Handler,
NMI_Handler,
HardFault_Handler,
MemManage_Handler,
BusFault_Handler,
UsageFault_Handler,
IntDefaultHandler,	// Reserved
IntDefaultHandler,	// Reserved
IntDefaultHandler,	// Reserved
IntDefaultHandler,	// Reserved
SVC_Handler,
DebugMon_Handler,
IntDefaultHandler,	// Reserved
PendSV_Handler,
SysTick_Handler,

// External Interrupts

WWDG_IRQHandler,
PVD_IRQHandler,
TAMPER_IRQHandler,
RTC_IRQHandler,
FLASH_IRQHandler,
RCC_IRQHandler,
EXTI0_IRQHandler,
EXTI1_IRQHandler,
EXTI2_IRQHandler,
EXTI3_IRQHandler,
EXTI4_IRQHandler,
DMA1_Channel1_IRQHandler,
DMA1_Channel2_IRQHandler,
DMA1_Channel3_IRQHandler,
DMA1_Channel4_IRQHandler,
DMA1_Channel5_IRQHandler,
DMA1_Channel6_IRQHandler,
DMA1_Channel7_IRQHandler,
ADC1_IRQHandler,
IntDefaultHandler,
IntDefaultHandler,
IntDefaultHandler,
IntDefaultHandler,
EXTI9_5_IRQHandler,
TIM1_BRK_TIM15_IRQHandler,
TIM1_UP_TIM16_IRQHandler,
TIM1_TRG_COM_TIM17_IRQHandler,
TIM1_CC_IRQHandlerm,
TIM2_IRQHandler,
TIM3_IRQHandler,
TIM4_IRQHandler,
I2C1_EV_IRQHandler,
I2C1_ER_IRQHandler,
I2C2_EV_IRQHandler,
I2C2_ER_IRQHandler,
SPI1_IRQHandler,
SPI2_IRQHandler,
USART1_IRQHandler,
USART2_IRQHandler,
USART3_IRQHandler,
EXTI15_10_IRQHandler,
RTCAlarm_IRQHandler,
CEC_IRQHandler,
IntDefaultHandler,
IntDefaultHandler,
IntDefaultHandler,
IntDefaultHandler,
IntDefaultHandler,
IntDefaultHandler,
IntDefaultHandler,
IntDefaultHandler,
IntDefaultHandler,
IntDefaultHandler,
IntDefaultHandler,
TIM6_DAC_IRQHandler,
TIM7_IRQHandler,
};


//void SysTick_Handler (void)
//{
//}


// This is the code that gets called when the processor first starts execution
// following a reset event.  Only the absolutely necessary set is performed,
// after which the application supplied entry() routine is called.  Any fancy
// actions (such as making decisions based on the reset cause register, and
// resetting the bits in that register) are left solely in the hands of the
// application.
#pragma call_graph_root="interrupt"         // interrupt category
void Reset_Handler (void)
{
    // Call the application's entry point.
	__iar_program_start ();
}

// This is the code that gets called when the processor receives a NMI.  This
// simply enters an infinite loop, preserving the system state for examination
// by a debugger.
#pragma call_graph_root="interrupt"         // interrupt category
static void NMI_Handler (void)
{
    // Enter an infinite loop.
    while (1)
    {
    }
}

// This is the code that gets called when the processor receives a fault
// interrupt.  This simply enters an infinite loop, preserving the system state
// for examination by a debugger.
#pragma call_graph_root="interrupt"         // interrupt category
static void HardFault_Handler (void)
{
    // Enter an infinite loop.
    while (1)
    {
    }
}

// This is the code that gets called when the processor receives an unexpected
// interrupt.  This simply enters an infinite loop, preserving the system state
// for examination by a debugger.
#pragma call_graph_root="interrupt"         // interrupt category
static void IntDefaultHandler (void)
{
    //
    // Go into an infinite loop.
    //
    while (1)
    {
    }
}


/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
#pragma call_graph_root="interrupt"         // interrupt category
void MemManage_Handler (void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
#pragma call_graph_root="interrupt"         // interrupt category
void BusFault_Handler (void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
#pragma call_graph_root="interrupt"         // interrupt category
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
#pragma call_graph_root="interrupt"         // interrupt category
void SVC_Handler (void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
#pragma call_graph_root="interrupt"         // interrupt category
void DebugMon_Handler (void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
#pragma call_graph_root="interrupt"         // interrupt category
void PendSV_Handler (void)
{
}

/******************************************************************************/
/*                 STM32F4xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f40xx.s/startup_stm32f427x.s/startup_stm32f429x.s).    */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/
#pragma weak WWDG_IRQHandler = IntDefaultHandler
#pragma weak PVD_IRQHandler = IntDefaultHandler
#pragma weak TAMPER_IRQHandler = IntDefaultHandler
#pragma weak RTC_IRQHandler = IntDefaultHandler
#pragma weak FLASH_IRQHandler = IntDefaultHandler
#pragma weak RCC_IRQHandler = IntDefaultHandler
#pragma weak EXTI0_IRQHandler = IntDefaultHandler
#pragma weak EXTI1_IRQHandler = IntDefaultHandler
#pragma weak EXTI2_IRQHandler = IntDefaultHandler
#pragma weak EXTI3_IRQHandler = IntDefaultHandler
#pragma weak EXTI4_IRQHandler = IntDefaultHandler
#pragma weak DMA1_Channel1_IRQHandler = IntDefaultHandler
#pragma weak DMA1_Channel2_IRQHandler = IntDefaultHandler
#pragma weak DMA1_Channel3_IRQHandler = IntDefaultHandler
#pragma weak DMA1_Channel4_IRQHandler = IntDefaultHandler
#pragma weak DMA1_Channel5_IRQHandler = IntDefaultHandler
#pragma weak DMA1_Channel6_IRQHandler = IntDefaultHandler
#pragma weak DMA1_Channel7_IRQHandler = IntDefaultHandler
#pragma weak ADC1_IRQHandler = IntDefaultHandler
#pragma weak EXTI9_5_IRQHandler = IntDefaultHandler
#pragma weak TIM1_BRK_TIM15_IRQHandler = IntDefaultHandler
#pragma weak TIM1_UP_TIM16_IRQHandler = IntDefaultHandler
#pragma weak TIM1_TRG_COM_TIM17_IRQHandler = IntDefaultHandler
#pragma weak TIM1_CC_IRQHandlerm = IntDefaultHandler
#pragma weak TIM2_IRQHandler = IntDefaultHandler
#pragma weak TIM3_IRQHandler = IntDefaultHandler
#pragma weak TIM4_IRQHandler = IntDefaultHandler
#pragma weak I2C1_EV_IRQHandler = IntDefaultHandler
#pragma weak I2C1_ER_IRQHandler = IntDefaultHandler
#pragma weak I2C2_EV_IRQHandler = IntDefaultHandler
#pragma weak I2C2_ER_IRQHandler = IntDefaultHandler
#pragma weak SPI1_IRQHandler = IntDefaultHandler
#pragma weak SPI2_IRQHandler = IntDefaultHandler
#pragma weak USART1_IRQHandler = IntDefaultHandler
#pragma weak USART2_IRQHandler = IntDefaultHandler
#pragma weak USART3_IRQHandler = IntDefaultHandler
#pragma weak EXTI15_10_IRQHandler = IntDefaultHandler
#pragma weak RTCAlarm_IRQHandler = IntDefaultHandler
#pragma weak CEC_IRQHandler = IntDefaultHandler
#pragma weak TIM6_DAC_IRQHandler = IntDefaultHandler
#pragma weak TIM7_IRQHandler = IntDefaultHandler


/****END OF FILE****/




