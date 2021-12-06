/**
  ******************************************************************************
  * @file    Project/STM32L1xx_StdPeriph_Templates/stm32l1xx_it.c 
  * @author  MCD Application Team
  * @version V1.2.0
  * @date    16-May-2014
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
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
//#include "stm32l1xx_it.h"
#include "stm32l1xx.h"
//#include "main.h"

/** @addtogroup Template_Project
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
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
__weak void TAMPER_STAMP_IRQHandler(void);//
__weak void RTC_WKUP_IRQHandler(void);//
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
__weak void LCD_IRQHandler(void);//
__weak void TIM9_IRQHandler(void);//
__weak void TIM10_IRQHandler(void);//
__weak void TIM11_IRQHandler(void);//
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
__weak void RTC_Alarm_IRQHandler(void);//
__weak void USB_FS_WKUP_IRQHandler(void);
__weak void TIM6_IRQHandler(void);
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
TAMPER_STAMP_IRQHandler,
RTC_WKUP_IRQHandler,
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
LCD_IRQHandler,
TIM9_IRQHandler,
TIM10_IRQHandler,
TIM11_IRQHandler,
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
RTC_Alarm_IRQHandler,
USB_FS_WKUP_IRQHandler,
TIM6_IRQHandler,
TIM7_IRQHandler
};

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


/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
#pragma call_graph_root="interrupt"         // interrupt category
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
#pragma call_graph_root="interrupt"         // interrupt category
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

#pragma call_graph_root="interrupt"         // interrupt category
void IntDefaultHandler (void)
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
void MemManage_Handler(void)
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
void BusFault_Handler(void)
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
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
#pragma call_graph_root="interrupt"         // interrupt category
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
#pragma call_graph_root="interrupt"         // interrupt category
void PendSV_Handler(void)
{
}

/******************************************************************************/
/*                 STM32L1xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32l1xx_xx.s).                                            */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */ 
#pragma weak WWDG_IRQHandler            = IntDefaultHandler
#pragma weak PVD_IRQHandler             = IntDefaultHandler
#pragma weak TAMPER_STAMP_IRQHandler    = IntDefaultHandler
#pragma weak RTC_WKUP_IRQHandler        = IntDefaultHandler
#pragma weak FLASH_IRQHandler           = IntDefaultHandler
#pragma weak RCC_IRQHandler             = IntDefaultHandler
#pragma weak EXTI0_IRQHandler           = IntDefaultHandler
#pragma weak EXTI1_IRQHandler           = IntDefaultHandler
#pragma weak EXTI2_IRQHandler           = IntDefaultHandler
#pragma weak EXTI3_IRQHandler           = IntDefaultHandler
#pragma weak EXTI4_IRQHandler           = IntDefaultHandler
#pragma weak DMA1_Channel1_IRQHandler   = IntDefaultHandler
#pragma weak DMA1_Channel2_IRQHandler   = IntDefaultHandler
#pragma weak DMA1_Channel3_IRQHandler   = IntDefaultHandler
#pragma weak DMA1_Channel4_IRQHandler   = IntDefaultHandler
#pragma weak DMA1_Channel5_IRQHandler   = IntDefaultHandler
#pragma weak DMA1_Channel6_IRQHandler   = IntDefaultHandler
#pragma weak DMA1_Channel7_IRQHandler   = IntDefaultHandler
#pragma weak ADC1_IRQHandler            = IntDefaultHandler
#pragma weak EXTI9_5_IRQHandler         = IntDefaultHandler
#pragma weak LCD_IRQHandler             = IntDefaultHandler
#pragma weak TIM9_IRQHandler            = IntDefaultHandler
#pragma weak TIM10_IRQHandler           = IntDefaultHandler
#pragma weak TIM11_IRQHandler           = IntDefaultHandler
#pragma weak TIM2_IRQHandler            = IntDefaultHandler
#pragma weak TIM3_IRQHandler            = IntDefaultHandler
#pragma weak TIM4_IRQHandler            = IntDefaultHandler
#pragma weak I2C1_EV_IRQHandler         = IntDefaultHandler
#pragma weak I2C1_ER_IRQHandler         = IntDefaultHandler
#pragma weak I2C2_EV_IRQHandler         = IntDefaultHandler
#pragma weak I2C2_ER_IRQHandler         = IntDefaultHandler
#pragma weak SPI1_IRQHandler            = IntDefaultHandler
#pragma weak SPI2_IRQHandler            = IntDefaultHandler
#pragma weak USART1_IRQHandler          = IntDefaultHandler
#pragma weak USART2_IRQHandler          = IntDefaultHandler
#pragma weak USART3_IRQHandler          = IntDefaultHandler
#pragma weak EXTI15_10_IRQHandler       = IntDefaultHandler
#pragma weak RTC_Alarm_IRQHandler       = IntDefaultHandler
#pragma weak USB_FS_WKUP_IRQHandler     = IntDefaultHandler
#pragma weak TIM6_IRQHandler            = IntDefaultHandler
#pragma weak TIM7_IRQHandler            = IntDefaultHandler

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
