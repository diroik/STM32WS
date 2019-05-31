#include <type_define.h>
#include <stddef.h>
#include <params_def.h>
#include <Classes.h>
#include <AdvClasses.h>
#include <init.h>
#include <PowerMetr.h>

#define I_CHARGE_MAX 1000 //Миллиампер

#define U_IN_CONST 2
#define U_BAT_MIN 5.0
#define U_BAT_MAX 7.2
#define START_DAC 600
#define DAC_STEP  10
#define DELTA_U_MAX 0.1
#define DAC_STEP_PRECHARGE 100

typedef enum {
  STATE_START=0x01,
  STATE_PRECHARGE=0x2,
  STATE_CHARGE=0x03,
  STATE_FULL_CHARGE=0x04,
  STATE_DISCHARGE=0x05,
  STATE_FULL_DISCHARGE=0x06,
} T_APP_POWER_STATE;

//const uint16_t fw_type __attribute__((at(0x08080000)));
const int LED_GREEN = 0x20;  //Маска пина Зелёного диода
const int LED_RED = 0x40;    //Маска пина Красного диода
const int BAT_ON  = 0x04;    // Маска ключа "BAT_ON"
const int Balance_BAT=0x08;  //Маска разрядного ключа Верхней батареи
const int Balance_HBAT=0x10; //Маска разрядного ключа Нижней Батареи 

const int                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       TEST_PIN  = 0x08;    // Маска ключа "BAT_ON"
//******************************************************************************
//**  Глобальные переменные
//******************************************************************************  
        Word64                  SecCount;           
        long                    RtcTimerDivider = 32768;
        
        TCmd_msg                BaseCmd;        
        TCmd                   *ExecutableCMD;

        TPowerMetrManager      *PowerMetrManager = NULL;

        TDateTime               SystemTime;
        TDispatcher             OS_Dispatcher;   

        Tlist<TCmdMessage*>     CMDmessages;       

        T_ADC_AKB               AKB[MAX_ADC_INPUTS];
        int                     ADC_Counter     = 0;

        TTimer                  WdiTimer(1);
        bool                    AdcStartEnable  = false;
        
        float                   AdcMultiplier   = 1024;
        T_APP_POWER_STATE AppPowerState;
        Data_Params const *TObject::PtrToConstants;
        Boot_Params const *TObject::PtrToBootConstants;
        
        int dac=0;   //Значение ДАка (ОТ 0 ДО 4095)  
        int MODE;    //Режим работы- разряд (0) Заряд током до напряжения (1) Заряд напряжением со снижением тока (2) удержание напряжения.
        int LED_MODE;//Режим индикации, пока 0-заряд, зелёный, 1- разряд, красный....
        //-------индикация----------//
        volatile uint8_t Load_Indicate;
        volatile bool Color_Indicate;
        uint8_t Bit_Counter;
        uint8_t Load_Indicate_Buffer;
        //--------------------------//
    TTaskListConfig const *TObjectTask::TaskListConfig; 
    TCalibrListConfig const *TObjectCalibr::CalibrListConfig; 
    
    //Data_Params const      *PtrToConstants;
    //Boot_Params const      *PtrToBootConstants;
    
    //TTaskListConfig const  *TaskListConfig;  
    //TCalibrListConfig const *CalibrListConfig;
    
    
    
    

//******************************************************************************
//**  Прототипы ф-й
//******************************************************************************
void InitProtocolObjects(void);
void Init_PowerMetrManagerObjects(void);
void InitLEDProcess(void);
void SetLedRED(void);
void SetLedGREEN(void);
void InitDACProcess(void);
void InitADCProcess(void);
//******************************************************************************
void PowerMetrProcess(void    *Par = NULL);

void DACProcess(void    *Par = NULL);
void ADCProcess(void    *Par = NULL);
//*******************************************
void OnAnswerErrorExecute(void  *Par = NULL);
//******************************************************************************
void Do_STATE_START(void);
void Do_STATE_PRECHARGE(void);
void Do_STATE_CHARGE(void);
void Do_STATE_FULL_CHARGE(void);
void Do_STATE_DISCHARGE(void);
void Do_STATE_FULL_DISCHARGE(void);
//******************************************************************************
//  Начало программы
//******************************************************************************


#define VREFINT_CAL             ((uint16_t)0x1FF80078)

float temp;
float Icharge;// = PowerMetrManager->PowerMetrs->CurrSensors[0];
float Iload;// = PowerMetrManager->PowerMetrs->CurrSensors[1];    
float Hbatt;// = PowerMetrManager->PowerMetrs->VoltSensors[0];
float Batt;// = PowerMetrManager->PowerMetrs->VoltSensors[1];
float Uin;// = PowerMetrManager->PowerMetrs->VoltSensors[1];

string test;
__task main (void)
{
  temp = VREFINT_CAL;
  test = TBuffer::IntToString(123,3);
  
  SYSTEM_INIT();
  
  InitLEDProcess();
  InitDACProcess();
  Init_PowerMetrManagerObjects();
  OS_Dispatcher.CreateProcess(PowerMetrProcess);            // создание процесса   
  //LED PROCESS
  //ADC PROCESS
  //DAC PROCESS
  //CELL_PROCESS
  CLEAR_WDT; 
__enable_interrupt();
  //P8DIR |= BIT1;//wdi

  // float Icharge = PowerMetrManager->PowerMetrs->CurrSensors[0]->U.Value;
 //  float Iload = PowerMetrManager->PowerMetrs->CurrSensors[1]->U.Value;    
 //  float Hbatt = PowerMetrManager->PowerMetrs->VoltSensors[0]->U.Value;
 //  float  Batt = PowerMetrManager->PowerMetrs->VoltSensors[1]->U.Value;
  AdcStartEnable = true;   
  AppPowerState=STATE_START;
  Start_ADC12();
  for(;;)
  {    
    OS_Dispatcher.Scheduler();                              // переключение процессов
    CLEAR_WDT;
    
    if(WdiTimer.GetTimValue(0) <= 0)
    {
      WdiTimer.SetTimer(0, 100);
    }            
    if(PowerMetrManager->CONVReady)//Завершение преобразования АЦП
    { 
        Icharge = PowerMetrManager->PowerMetrs->CurrSensors[0]->U.Value*1000;//117mA
        Iload= PowerMetrManager->PowerMetrs->CurrSensors[1]->U.Value*1000;//16mA       
        Uin = PowerMetrManager->PowerMetrs->VoltSensors[2]->U.Value; // Uвх
        Batt = PowerMetrManager->PowerMetrs->VoltSensors[1]->U.Value; //4.55
        Hbatt = PowerMetrManager->PowerMetrs->VoltSensors[0]->U.Value; // пока ненадо, балансировки нет 
        Indicate_Process();        
        switch(AppPowerState)
        {     
          case STATE_START: 
            Do_STATE_START(); 
            break;           
          case STATE_PRECHARGE: 
            Do_STATE_PRECHARGE(); 
            break;            
          case STATE_CHARGE: 
            Do_STATE_CHARGE(); 
            break;
          case STATE_FULL_CHARGE: 
            Do_STATE_FULL_CHARGE();
            break;
          case STATE_DISCHARGE: 
            Do_STATE_DISCHARGE(); 
            break;
          case STATE_FULL_DISCHARGE: 
            Do_STATE_FULL_DISCHARGE(); 
            break;
          default: 
            Do_STATE_START(); 
            break;
        }        
        DAC->DHR12RD=dac;        
        PowerMetrManager->CONVReady=false;
    }  
  }
}

void Indicate_Process(void)
{
  if ((AppPowerState==STATE_PRECHARGE) || (AppPowerState==STATE_CHARGE) || (AppPowerState==STATE_FULL_CHARGE) || (AppPowerState==STATE_START))
  {
    if (Batt<U_BAT_MIN+0.7) {
      Indicate_Set(LED_LOAD_LIGHT, COLOR_GREEN);
      return;
    }
    if (Batt<U_BAT_MAX-0.7) {
      Indicate_Set(LED_LOAD_MIDDLE, COLOR_GREEN);
      return;
    }
    if (Batt<U_BAT_MAX) {
      Indicate_Set(LED_LOAD_HARD, COLOR_GREEN);
      return;
    }
  }
  
  if (AppPowerState==STATE_DISCHARGE || AppPowerState==STATE_FULL_DISCHARGE)
  {
    if (Batt<U_BAT_MIN+0.7) {
      Indicate_Set(LED_LOAD_LIGHT, COLOR_RED);
      return;
    }
    if (Batt<U_BAT_MAX-0.7) {
      Indicate_Set(LED_LOAD_MIDDLE, COLOR_RED);
      return;
    }
    if (Batt<U_BAT_MAX) {
      Indicate_Set(LED_LOAD_HARD, COLOR_RED); 
      return;
    }
  }
}

void Do_STATE_START(void)//V
{
  GPIOA->ODR|=BAT_ON;   // подхватываем микроконтроллер по питанию
  dac=START_DAC;  // обнуляем DAC
  if(Uin>U_IN_CONST)  AppPowerState=STATE_PRECHARGE;
  else if (Uin<U_IN_CONST)  AppPowerState=STATE_DISCHARGE;
}

void Do_STATE_PRECHARGE(void)
{
  //GPIOA->ODR^=LED_GREEN;     // Мигание зеленый
  //GPIOA->ODR&=~LED_RED; 
  if((Icharge<I_CHARGE_MAX) && (Batt<U_BAT_MAX))
     dac+=DAC_STEP_PRECHARGE;    // прирост DAC
  else if ((Icharge>I_CHARGE_MAX) || (Batt>U_BAT_MAX)) 
  {
     dac-=DAC_STEP_PRECHARGE; 
     AppPowerState=STATE_CHARGE;
  }
}

void Do_STATE_CHARGE(void)//V
{
  //GPIOA->ODR^=LED_GREEN;     // Мигание зеленый
  //GPIOA->ODR&=~LED_RED;      
  if((Icharge<I_CHARGE_MAX) && (Batt<U_BAT_MAX))
    dac+=DAC_STEP;    // прирост DAC
  else if ((Icharge>I_CHARGE_MAX) && (Batt<U_BAT_MAX)) 
    dac-=DAC_STEP;    // глушим DAC  
  if(Batt>U_BAT_MAX) 
  AppPowerState=STATE_FULL_CHARGE; 
  
  if (Uin<U_IN_CONST)  AppPowerState=STATE_DISCHARGE;
}

void Do_STATE_FULL_CHARGE(void)
{
  //GPIOA->ODR|=LED_GREEN;    // зеленый
  //GPIOA->ODR&=~LED_RED; 
  if (Uin<U_IN_CONST)  AppPowerState=STATE_DISCHARGE;

  if(Batt>U_BAT_MAX+DELTA_U_MAX) 
    dac-=DAC_STEP;    // глушим DAC
  else if (Batt<U_BAT_MAX-DELTA_U_MAX) 
    dac+=DAC_STEP;    // прирост DAC   
}

void Do_STATE_DISCHARGE(void)//V
{
  //GPIOA->ODR^=LED_RED;     // Мигание красный
  //GPIOA->ODR&=~LED_GREEN; 
  dac=START_DAC;                      
  if(Batt<U_BAT_MIN) AppPowerState=STATE_FULL_DISCHARGE; 
  if(Uin>U_IN_CONST) AppPowerState=STATE_PRECHARGE;
}

void Do_STATE_FULL_DISCHARGE(void)//V
{
   GPIOA->ODR&=~BAT_ON;   // выключаем самоподдержание питания контроллера питания
}



/*
void Do_STATE_START(void)//V
{
  GPIOA->ODR|=BAT_ON;   // подхватываем микроконтроллер по питанию
  dac=START_DAC;  // обнуляем DAC
  if(Icharge>Iload)  AppPowerState=STATE_CHARGE;
  else if (Icharge<Iload)  AppPowerState=STATE_DISCHARGE;
}

void Do_STATE_CHARGE(void)//V
{
  GPIOA->ODR^=LED_GREEN;     // Мигание зеленый
  GPIOA->ODR&=~LED_RED;      
  if((Icharge<I_CHARGE_MAX) && (Batt<U_BAT_MAX)) dac+=DAC_STEP;    // прирост DAC
  else if(Batt>U_BAT_MAX) AppPowerState=STATE_FULL_CHARGE; 
  
  if (Icharge<Iload)  AppPowerState=STATE_DISCHARGE;
}

void Do_STATE_FULL_CHARGE(void)
{
  GPIOA->ODR|=LED_GREEN;    // зеленый
  GPIOA->ODR&=~LED_RED; 
  if (Icharge<Iload)  AppPowerState=STATE_DISCHARGE;  
}

void Do_STATE_DISCHARGE(void)//V
{
  GPIOA->ODR^=LED_RED;     // Мигание красный
  GPIOA->ODR&=~LED_GREEN; 
  dac=START_DAC;                      
  if(Batt<U_BAT_MIN) AppPowerState=STATE_FULL_DISCHARGE; 
  if(Icharge>Iload)  AppPowerState=STATE_CHARGE;
}

void Do_STATE_FULL_DISCHARGE(void)//V
{
   GPIOA->ODR&=~BAT_ON;   // выключаем самоподдержание питания контроллера питания
}
*/

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
//*******************************************************************************************
//    LED INIT
//*******************************************************************************************
void InitLEDProcess(void)
{
  GPIOA->CRL=0;
 // GPIOB->CRL=0;
  GPIOA->CRL|=GPIO_CRL_MODE5_1+GPIO_CRL_MODE6_1+GPIO_CRL_MODE2_1;//Режим портов на выход
  GPIOA->ODR&=~LED_RED+LED_GREEN;//
  GPIOA->ODR|=BAT_ON;//
  
  //GPIOB->CRL|=GPIO_CRL_MODE3_1; //Режим портов тестовый пин на выход
  //GPIOB->ODR&=~TEST_PIN; 
  
  //GPIOB->CRL|=GPIO_CRL_MODE3_0+GPIO_CRL_MODE4_0;//Режим портов на выход
  //GPIOB->ODR=0;//
  
  
  //if(GPIOA->ODR==0x00000020)GPIOA->ODR= 0x00000040; else GPIOA->ODR= 0x00000020;//Тестовое моргание портом....
 
  //LEDGREEN
 }
//*******************************************************************************************
//    SET LED RED
//*******************************************************************************************
void SetLedRED(void)
{
    GPIOA->ODR&=~LED_GREEN; 
    GPIOA->ODR|=LED_RED;
}
//*******************************************************************************************
//    SET LED GREEN
//*******************************************************************************************
void SetLedGREEN(void)
{
    GPIOA->ODR&=~LED_RED;
    GPIOA->ODR|=LED_GREEN;
}
//*******************************************************************************************
//    LED PROC
//*******************************************************************************************
void LEDProcess(void    *Par)
{
    //если разряд - красный
    //если заряд  - зелёный
    //Емкость батареи показывается частотой мигания светодиода, чем чаще, тем меньше заряд.
}
//*******************************************************************************************
//    DAC INIT
//*******************************************************************************************
void InitDACProcess(void)
{
    //Режим 12 бит
    //Порт А 4й бит в режим AIN
    GPIOA->CRL&=~(GPIO_CRL_CNF4+GPIO_CRL_MODE4); //сбрасываем в ноль включаем Режим AIN
    DAC->CR|=DAC_CR_TSEL1+DAC_CR_BOFF1;
    DAC->CR|=DAC_CR_EN1;
    DAC->DHR12RD=START_DAC;
    DAC->SWTRIGR=DAC_SWTRIGR_SWTRIG1;     
    
}
//*******************************************************************************************
//    DAC PROC
//*******************************************************************************************
void DACProcess(void    *Par)
{
}
//*******************************************************************************************
//    ADC INIT
//*******************************************************************************************
void InitADCProcess(void)
{
}
//*******************************************************************************************
//    ADC PROC
//*******************************************************************************************
void ADCProcess(void    *Par)
{
}
//******************************************************************************
// 
//******************************************************************************
void Init_PowerMetrManagerObjects(void) 
{
  
  Data_Params       const *ptr1 = TObject::PtrToConstants;
  TTaskListConfig   const *ptr2 = TObjectTask::TaskListConfig;    
  TCalibrListConfig const *ptr3 = TObjectCalibr::CalibrListConfig;  
  
  TPowerMetrManager *Pm = new TPowerMetrManager(ptr2->ReglamentTime);
  Pm->Init(ptr1, ptr2, ptr3);
  PowerMetrManager = Pm;
}
//******************************************************************************
// niauoea aicieeaao eiaaa iao ioaaoa 
//******************************************************************************
void OnAnswerErrorExecute(void *Par)
{

}
//******************************************************************************
//
//******************************************************************************
