#include <type_define.h>
#include <stddef.h>
#include <params_def.h>
#include <Classes.h>
#include <AdvClasses.h>
#include <init.h>
#include <PowerMetr.h>
//#include <typeinfo>

//const uint16_t fw_type __attribute__((at(0x08080000)));

//******************************************************************************
//**  Глобальные переменные
//******************************************************************************  
        Word64                  SecCount;           
        long                    RtcTimerDivider = 32768;
        
        TBuffer                 Buffer(256);         //iniaaiie aooa? aaiiuo
        TComPort0               RS485Port(4);//128 

        TCmd_msg                BaseCmd;        
        TCmd                   *ExecutableCMD;

        TPowerMetrManager      *PowerMetrManager = NULL;

        TDateTime               SystemTime;
        TDispatcher             OS_Dispatcher;   

        Tlist<TCmdMessage*>     CMDmessages;       

        T_ADC_AKB               AKB[MAX_ADC_INPUTS+1];
        int                     ADC_Counter     = 0;

        //TTimer                  WdiTimer(1);
        bool                    AdcStartEnable  = false;
        
        Word                    AdcMultiplier   = 4096;
    
    Data_Params const *TObject::PtrToConstants;
    Boot_Params const *TObject::PtrToBootConstants;
    
    TTaskListConfig const *TObjectTask::TaskListConfig; 
    TCalibrListConfig const *TObjectCalibr::CalibrListConfig; 
//******************************************************************************
//**  Прототипы ф-й
//******************************************************************************
void InitProtocolObjects(void);
void InitRS485Objects(void);
void InitRS485ProtocolObjects();
void Init_PowerMetrManagerObjects(void);
//******************************************************************************
void RS485Process(void        *Par = NULL);
void PowerMetrProcess(void    *Par = NULL);
//*******************************************
void GenerateDUMMY_NS_command(void *Par);

typedef  void (*pFunction)(void);
pFunction Jump_To_Application;


//******************************************************************************
//  Начало программы
//******************************************************************************

//#pragma optimize=z none
int main (void)
{
  __set_PRIMASK(1);           //disable all system interrupts
  SCB->VTOR = NVIC_VectTab_FLASH | (0x08006400 & (uint32_t)0x1FFFFF80);
  __set_PRIMASK(0);           //enable all system
 
  SYSTEM_INIT();
  InitRS485Objects();
  Init_PowerMetrManagerObjects();
  OS_Dispatcher.CreateProcess(RS485Process);                // создание процесса   RS485
  OS_Dispatcher.CreateProcess(PowerMetrProcess);            // создание процесса   
  CLEAR_WDT; 
 
__enable_interrupt();
  AdcStartEnable = true;  
  
  GenerateDUMMY_NS_command(BaseCmd.MainMessages);
  for(;;)
  {    
    OS_Dispatcher.Scheduler();                              // переключение процессов
    CLEAR_WDT;
  }
}
//******************************************************************************
// RS485Process
//******************************************************************************
void RS485Process(void *Par)
{ 

    switch( ExecutableCMD->GetFase() )
    {   
      case WAIT_MES:
        ExecutableCMD = ExecutableCMD->GET_Message(&RS485Port,&Buffer);
        break;   
      case RX_CMD:
        ExecutableCMD = ExecutableCMD->CMD_Recv(&RS485Port,&Buffer);
        break;
      case EXE_CMD:
        ExecutableCMD = ExecutableCMD->CMD_Execute(&RS485Port,&Buffer);
        break;   
      case TX_CMD:
        ExecutableCMD =  ExecutableCMD->CMD_Send(&RS485Port,&Buffer);
        break;       
      default:
        TObject::Soft_Reset();
    }    
    if(ExecutableCMD->NeedReset() == true && ExecutableCMD->NeedAnswer() == false) 
    { 
      TTimer::wait_1ms(1000);
      ExecutableCMD->SetFase(99);
    }  
}

//******************************************************************************
// 
//******************************************************************************
void PowerMetrProcess(void *Par)
{
  if( PowerMetrManager != NULL )
  {
      switch( PowerMetrManager->Fase )
      {
        case DETECT_STATES:
          PowerMetrManager->FASE_DetectStates();
          break;     
        case FORMING_MESSAGE:
          PowerMetrManager->FASE_FormingMessageCMD( BaseCmd.MainMessages );
          break;           
        default:
          TObject::Soft_Reset();
      }
  }
}
//******************************************************************************
//******************************************************************************
//******************************************************************************
//
//******************************************************************************
void InitRS485Objects()
{
    Data_Params const *ptr             = TObject::PtrToConstants;
    RS485Port.open(COM0, TObject::PtrToConstants->MAIN_BOUD_RATE, TObject::PtrToConstants->MAIN_timeout);
   
    //BaseCmd.AddToCMD_Messages(new TCmd_ERROR(&BaseCmd,"ERROR"));
    BaseCmd.AddToCMD_Messages(new TCmd_UnknownCMD(&BaseCmd, "UnknownCMD"));  
    BaseCmd.AddToCMD_Messages(new TCmd_Data(&BaseCmd,"Data"));    
    BaseCmd.AddToCMD_Messages(new TCmd_SetCfg(&BaseCmd,"SetCfg"));  
    BaseCmd.AddToCMD_Messages(new TCmd_GetCfg(&BaseCmd,"GetCfg")); 
    BaseCmd.AddToCMD_Messages(new TCmd_GetData(&BaseCmd,"GetData"));  
    //BaseCmd.AddToCMD_Messages(new TCmd_GetCalibr(&BaseCmd,"GetCalibr"));  
    //BaseCmd.AddToCMD_Messages(new TCmd_SetCalibr(&BaseCmd,"SetCalibr"));  
    //BaseCmd.AddToCMD_Messages(new TCmd_UB(&BaseCmd,"UB"));
   
    CLEAR_WDT;  
    BaseCmd.PasswordPtr         = (char*)ptr->PassWord;
    BaseCmd.AddrPtr             = (char*)ptr->DevAddress;
    BaseCmd.MainMessages        = &CMDmessages;
    BaseCmd.MainMessages->clear();  
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
void Init_PowerMetrManagerObjects(void) 
{
  
  Data_Params       const *ptr1 = TObject::PtrToConstants;
  TTaskListConfig   const *ptr2 = TObjectTask::TaskListConfig;    
  
  TPowerMetrManager *Pm = new TPowerMetrManager(ptr2->ReglamentTime);
  Pm->Init(ptr1, ptr2);
  PowerMetrManager = Pm;
}
//******************************************************************************
//
//******************************************************************************
void GenerateDUMMY_NS_command(void *Par)
{
  
 Data_Params const      *ptr     = TObject::PtrToConstants; 
 if( Par != NULL)
  {
    Tlist<TCmdMessage*> *Messages = (Tlist<TCmdMessage*>*)Par;
           Word addr = atoi((char const*)ptr->DevAddress);
           TCmdMessage *message = new TCmdMessage();
           message->DateTime = new TRtcTime();
           *message->DateTime = SystemTime.GetTime();
           message->CMD      = "Data";
           message->Addr     = "";
           message->State    = " ";
           message->Data.push_back("0,0,>,0,0,MKAB_RESET a="+TBuffer::WordToString(addr));
           message->NeedCmdAnswer = false;
           
           Messages->push_back(message);    
  }
}
//******************************************************************************
//******************************************************************************
//*****************************************************************************