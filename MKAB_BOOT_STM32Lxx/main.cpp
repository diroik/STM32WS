#include <type_define.h>
#include <stddef.h>
#include <params_def.h>
#include <Classes.h>
#include <AdvClasses.h>

void SYSTEM_INIT(void);
//******************************************************************************
//**  √лобальные переменные  
    
TBuffer                     Buffer(1200);  
TDispatcher                 OS_Dispatcher;
TComPort0                   RS485Port(8);        
TCmd                        BaseCmd;
TCmd                        *ExecutableCMD;        
TDateTime                   SystemTime;
DWord                       ErrorBitCNT = 0;

Data_Params const       *TObject::PtrToConstants;
//Boot_Params const       *TObject::PtrToBootConstants;
TTimer                  TObjectFunctions::ObjectFunctionsTimer(2);  

//******************************************************************************
//**  ѕрототипы ф-й
//******************************************************************************
void InitProtocolObjects(void);
void InitRS485Objects(void);
void InitRS485ProtocolObjects();
//******************************************************************************
void RS485Process(void        *Par = NULL);
//*******************************************
void OnAnswerErrorExecute(void  *Par = NULL);
void GenerateDUMMY_NS_command(void *Par);
//******************************************************************************
//  Ќачало программы
//******************************************************************************
//    #pragma location=0x0800F400
//  __root uint32_t status;    
typedef void (*pFunction)(void);
volatile pFunction Jump_To_Application;
volatile __IO uint32_t addres_to_jump = 0;

#pragma optimize=z none
void main (void)
{
volatile __IO uint32_t JumpAddress = 0;

USART1->CR1 = 0;
USART1->BRR = 0;
USART1->CR2 = 0;
    
//GPIOA->CRL = 0;
//GPIOA->CRH = 0;
GPIOA->MODER = 0;
GPIOA->PUPDR = 0;
GPIOA->IDR = 0;
GPIOA->ODR = 0;


//GPIOB->CRL = 0;							
//GPIOB->CRH = 0;
GPIOB->MODER = 0;
GPIOB->PUPDR = 0;
GPIOB->ODR = 0;
GPIOB->IDR = 0;

//GPIOC->CRL = 0;										
//GPIOC->CRH = 0;
GPIOC->MODER = 0;
GPIOC->PUPDR = 0;
GPIOC->IDR = 0;
GPIOC->ODR = 0;

FLASH->SR = 0;

RCC->APB1ENR = 0x00000;						// GPIO   dissable
RCC->APB2ENR = 0x00000;						// USART2   dissable  


addres_to_jump = PARAMS.BOOT_PARAMS.AppAdrStart;
if(PARAMS.BOOT_PARAMS.BootStatus == 1)
{
  if (((*(__IO uint32_t*)addres_to_jump) & 0x2FFE0000 ) == 0x20000000)           // Test if user code is programmed starting from address "ApplicationAddress"
  { 
    JumpAddress = *(__IO uint32_t*) (addres_to_jump + 4);   // исполнительный адрес пользовательского приложени€
    Jump_To_Application = (pFunction) JumpAddress;
    __set_MSP(*(__IO uint32_t*) addres_to_jump);            // инициализируем указатель стека в пользовательском приложении
    Jump_To_Application();                                     // передаем управление пользовательскому приложению
  }
}

SYSTEM_INIT();
InitRS485Objects();
OS_Dispatcher.CreateProcess(RS485Process);                // создание процесса   RS485
CLEAR_WDT; 
__enable_interrupt();

for(;;)
  {    
    OS_Dispatcher.Scheduler();                              // переключение процессов
  }
}
//******************************************************************************
// RS485Process
//******************************************************************************
void RS485Process(void *Par)
{ 

    switch( ExecutableCMD->GetFase() )
    {   
      case RX_CMD:
        if(TCmd::ResetFlg == true){ 
          ExecutableCMD->SetFase(99);
        }
        ExecutableCMD = ExecutableCMD->CMD_Recv(&RS485Port,&Buffer);
        break;
      case EXE_CMD:
        ExecutableCMD = ExecutableCMD->CMD_Execute(&RS485Port,&Buffer);
        break;   
      case TX_CMD:
        ExecutableCMD = ExecutableCMD->CMD_Send(&RS485Port,&Buffer);
        break;       
      default:
        TObject::Soft_Reset();
    }
}
//******************************************************************************
//
//******************************************************************************
void InitRS485Objects()
{
 
  RS485Port.open(COM0, TObject::PtrToConstants->MAIN_BOUD_RATE, TObject::PtrToConstants->MAIN_timeout);
  CLEAR_WDT;
  BaseCmd.AddToCMD_Messages(new TCmdErrorCMD("ERROR") );
  BaseCmd.AddToCMD_Messages(new TCmdUB("UB") );
  BaseCmd.AddToCMD_Messages(new TCmdUD("UD") );
  BaseCmd.AddToCMD_Messages(new TCmdUE("UE") );
  //BaseCmd.AddToCMD_Messages(new TCmdGV("GetCfg") );
  BaseCmd.AddToCMD_Messages(new TCmdGD("GetData") );
  CLEAR_WDT;
  ExecutableCMD = &BaseCmd;
}
//******************************************************************************
//
//******************************************************************************
void InitProtocolObjects()
{
}
//******************************************************************************
// 
//******************************************************************************
//
//******************************************************************************
void OnAnswerErrorExecute(void *Par)
{

}
//******************************************************************************
//
//******************************************************************************
void GenerateDUMMY_NS_command(void *Par)
{

}
//******************************************************************************
//******************************************************************************
//*****************************************************************************