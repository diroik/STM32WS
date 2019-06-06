//=============================================================================
#include <PowerMetr.h>
//#include <PowerMetr.h>
//#include <flash_map.h>
//#include <sram_map.h>
//=============================================================================


#define vref                            3.3
#define fullsacle                       4096//битность ацп, не менять!

extern T_ADC_AKB               AKB[];
extern int                     ADC_Counter;
extern float                   AdcMultiplier;
//*****************************************************************************
//*** Класс  
//*****************************************************************************
//=============================================================================
// Constructor
//=======================================
TPowerMetrManager::TPowerMetrManager(Word period):TFastTimer(2,&SecCount),Period(period),Fase(DETECT_STATES)
{ 
  SetTimer(PERIUD_TIMER, 2);
  FirstInitFlg = true;
}
//=============================================================================
// Destructor
//=======================================
TPowerMetrManager::~TPowerMetrManager()
{

}
//=============================================================================
//
//=======================================
void TPowerMetrManager::Init(Data_Params const * DataPtr, TTaskListConfig const *TaskPtr, TCalibrListConfig const *CalPtr)
{ 
  TPowerMetr *DATA1 = new TPowerMetr();  
  Et = (DataPtr->wReserved[0] == 0 || DataPtr->wReserved[0] == 0xFFFF) ? 26:  DataPtr->wReserved[0];
  En = (DataPtr->wReserved[1] == 0 || DataPtr->wReserved[1] == 0xFFFF) ? 1:   DataPtr->wReserved[1];
  Pn = (DataPtr->wReserved[2] == 0 || DataPtr->wReserved[2] == 0xFFFF) ? 1:   DataPtr->wReserved[2];
  ADCReady=false;
 //Датчики 
  //DATA1->CurrSensors.push_back(new TCurrentDetector(&AKB[0].AKB_OUT, CalPtr->CalVolt[8], 60, 1, fullsacle+1, CalPtr->CalVolt[9]));//I  
  DATA1->CurrSensors.push_back(new TCurrentDetector(&AKB[1].AKB_OUT, CalPtr->CalVolt[1], 60, 1, AdcMultiplier));//Icharge
  DATA1->CurrSensors.push_back(new TCurrentDetector(&AKB[2].AKB_OUT, CalPtr->CalVolt[2], 60, 1, AdcMultiplier));//Iload
  
  DATA1->VoltSensors.push_back(new TVoltageDetector(&AKB[3].AKB_OUT, CalPtr->CalVolt[3], 83, Pn,        AdcMultiplier,  (15.0+10.0)/10.0) );//U HBAT
  DATA1->VoltSensors.push_back(new TVoltageDetector(&AKB[4].AKB_OUT, CalPtr->CalVolt[4], 83, Pn,        AdcMultiplier,  (15.0+10.0)/10.0) );//U BAT_
  DATA1->VoltSensors.push_back(new TVoltageDetector(&AKB[5].AKB_OUT, CalPtr->CalVolt[5], 83, Pn,        AdcMultiplier,  (22.0+10.0)/10.0) );//U BAT_


  PowerMetrs  = DATA1;
  ADC_Counter = 0;
  CLEAR_WDT;
}
//=============================================================================
// 
//======================================= 
void TPowerMetrManager::FASE_DetectStates(void)
{
  CLEAR_WDT;
  static bool perflg = false;
  bool flag = false;
  
    if( this->GetTimValue(PERIUD_TIMER) <= 0 )
    {
      if(FirstInitFlg == true)
      {
        FirstInitFlg = false;
        flag     = true;
      }
      Word period = Period;
      if(perflg == true) period = Period/5;
      if( SystemTime.GetGlobalSeconds()%period  == 0)
      { 
        flag     = true;
        SetTimer( PERIUD_TIMER, 2 );
      }
      else
      { SetTimer( PERIUD_TIMER, 1 );}
      if(flag == true)
      {
         PowerMetrs->Modify = true;
      }
      else
      {//проверка на пороги и тп
      }
    }
    if(ADCReady)//Завершение преобразования АЦП
    {
        float lastValue = 0.0;       
        foreach(IDetector* curr, PowerMetrs->VoltSensors)
        {
           float val = curr->GetValue();
           curr->U.CalcMinAverMax(val);  
        };  

        foreach(IDetector* curr, PowerMetrs->CurrSensors)
        {//ОШИБКА определения разряда!
           float val = curr->GetValue();
           //if( val > -2.0 && val < 2.0) 
           //  val = 0.0;
           curr->U.CalcMinAverMax(val);  
        };
        
        ADCReady=false;
        CONVReady=true;     // флаг готовности для цикла main
    }
    Fase = FORMING_MESSAGE;
}
//=============================================================================
// 
//======================================= 
void TPowerMetrManager::FASE_FormingMessageCMD(void *Par)
{
 //Data_Params const      *ptr     = TObject::PtrToConstants; 
 //Boot_Params const      *BOOTptr = TObject::PtrToBootConstants;
  
  /*if( Par != NULL)
  {
    Tlist<TCmdMessage*> *Messages = (Tlist<TCmdMessage*>*)Par;
    TPowerMetr* DATA = PowerMetrs;
    if( Messages->size() < MAX_MESS_IN_MAIN)
    {
      string AddedStr = "";
      //string TempStr = "";  
      if(DATA->Modify == true )
      {
         AddedStr+=TBuffer::WordToString(Et)+',';
         AddedStr+=TBuffer::WordToString(En)+',';
         
         /*
         AddedStr +="\r\n>,"+TBuffer::WordToString(60)+','+TBuffer::WordToString(Pn)+',';
         foreach(IDetector* curr, DATA->CurrSensors)
         {
            AddedStr+= curr->GetValueString();
         } */
         
       /*  AddedStr +="\r\n>,"+TBuffer::WordToString(83)+','+TBuffer::WordToString(Pn)+',';
         foreach(IDetector* curr, DATA->VoltSensors)
         {
            AddedStr+= curr->GetValueString();
         }  
         AddedStr +="\r\n>,"+TBuffer::WordToString(83)+','+TBuffer::WordToString(Pn+1)+',';
         foreach(IDetector* curr, DATA->TempSensors)
         {
            AddedStr+= curr->GetValueString();
         } 
         
         Word addr = atoi((char const*)ptr->DevAddress);
         string Version ="0,2,";
         Version+="\r\n>,100,"+TBuffer::WordToString(addr)+","+
                          TBuffer::WordToString(DEVICE_CURR_VER)+","+
                            TBuffer::WordToString(DEVICE_SUB_VER)+","+
                              TBuffer::WordToString(BOOTptr->CurVersion)+",";
         
         TCmdMessage *message = new TCmdMessage();
         message->DateTime    = new TRtcTime();
         *message->DateTime   = SystemTime.GetTime();
         message->CMD         = "Data";
         message->Addr        = "";
         message->State       = " ";
         message->Data.push_back(AddedStr);
         message->Data.push_back(Version);         
         message->NeedCmdAnswer = true;
         Messages->push_back(message); 
         DATA->Modify         = false;
         
      }
    }
  }*/
  this->Fase = DETECT_STATES;
}

//*****************************************************************************
//*** Класс  
//*****************************************************************************
//=============================================================================
// Constructor
//=======================================
TPowerMetr::TPowerMetr(void):TFastTimer(1,&SecCount), Modify(false)
{ 
      //HValue=Value=false;
      //SwitchFase = 0;
      //PorogGesterezis = false;
}
//=============================================================================
// Destructor
//=======================================
TPowerMetr::~TPowerMetr()
{

}
//*****************************************************************************
//*** Класс  
//*****************************************************************************
//=============================================================================
// Constructor
//=======================================
IDetector::IDetector(DWord* adcmem, float koef, Byte ptype, Byte index, float mult):IParams(ptype, index, true), Koef(koef)
{ 
  ADCMEM          = adcmem;
  Value           = 0;
  Multiplier      = mult;
  AverageValue    = 0;
  Divider         = 0;
  Delitel         = 10;  
  
  Params.push_back(&U);
}
//=============================================================================
// Destructor
//=======================================
IDetector::~IDetector()
{

}
//*****************************************************************************
//*** Класс  
//*****************************************************************************
//=============================================================================
// Constructor
//=======================================
TVoltageDetector::TVoltageDetector(DWord* adcmem, float koef, Byte ptype, Byte index, float mult, float del):IDetector(adcmem, koef, ptype, index, mult)
{ 
    Delitel = del;
}
//=============================================================================
// Destructor
//=======================================
TVoltageDetector::~TVoltageDetector()
{

}
//=============================================================================
// 
//=======================================
float TVoltageDetector::GetValue( )
{
  AverageValue = *ADCMEM;
  float U = (vref*AverageValue*Delitel)/(fullsacle*(float)Multiplier*1.0);
  Value=U*Koef;
  if(Value < 0.1) Value = 0;
  Divider = 0;
  return Value;
}
//=============================================================================
// 
//=======================================
float TVoltageDetector::GetSimpleValue( )
{
  AverageValue = *ADCMEM;
  float U = (vref*AverageValue*Delitel)/(fullsacle*(float)Multiplier*1.0);
  Value=U;
  Divider = 0;
  return Value;
}
//=============================================================================
// 
//=======================================
DETECTING_RESULT TVoltageDetector::DetectPorogs(float Value)
{
  return NO_RESULT;
}
//*****************************************************************************
//*** Класс  
//*****************************************************************************
//=============================================================================
// Constructor
//=======================================
TCurrentDetector::TCurrentDetector(DWord* adcmem, float koef, Byte ptype, Byte index, float mult, float par1):IDetector(adcmem, koef, ptype, index, mult)
{ 
    sens = par1;
    Delitel = 1.0;
}
//=============================================================================
// Destructor
//=======================================
TCurrentDetector::~TCurrentDetector()
{

}
//=============================================================================
// 
//=======================================
float TCurrentDetector::GetValue( )
{
  AverageValue = *ADCMEM;
  float U = (vref*AverageValue*Delitel)/(fullsacle*(float)Multiplier*1.0);  
  float sens = GetSens();

  Value= U*Koef/sens;
  //Value= (U - (U_SENS_MAX/2.0))/(sens*Koef);
  
  Divider = 0;
  return Value;
}
//=============================================================================
// 
//=======================================
float TCurrentDetector::GetSimpleValue( )
{
  AverageValue = *ADCMEM;
  float U = (vref*AverageValue*Delitel)/(fullsacle*(float)Multiplier*1.0);
  Value=U;
  Divider = 0;
  return Value;
}
//=============================================================================
// 
//=======================================
float TCurrentDetector::GetSens()
{  
  float value = sens == 1 ? I_SENS : sens;
  return value;
}
//=============================================================================
// 
//=======================================
DETECTING_RESULT TCurrentDetector::DetectPorogs(float Value)
{
  return NO_RESULT;
}

