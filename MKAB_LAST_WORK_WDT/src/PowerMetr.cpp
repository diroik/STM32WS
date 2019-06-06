//=============================================================================
#include <PowerMetr.h>
//#include <PowerMetr.h>
//#include <flash_map.h>
//#include <sram_map.h>
//=============================================================================


#define vref                            2.5
#define fullsacle                       4095

extern T_ADC_AKB               AKB[];
extern int                     ADC_Counter;
//*****************************************************************************
//*** Класс  
//*****************************************************************************
//=============================================================================
// Constructor
//=======================================
TPowerMetrManager::TPowerMetrManager(Word period):TFastTimer(2,&SecCount),Period(period),Fase(DETECT_STATES)
{ 
  SetTimer(PERIUD_TIMER, 15);
  FirstInitFlg = true;
  //etalon = NULL;
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
void TPowerMetrManager::Init(Data_Params const * DataPtr, TTaskListConfig const *TaskPtr)
{ 
  TPowerMetr *DATA1 = new TPowerMetr();  
  Et = (DataPtr->wReserved[0] == 0 || DataPtr->wReserved[0] == 0xFFFF) ? 26:  DataPtr->wReserved[0];
  En = (DataPtr->wReserved[1] == 0 || DataPtr->wReserved[1] == 0xFFFF) ? 1:   DataPtr->wReserved[1];
  Pn = (DataPtr->wReserved[2] == 0 || DataPtr->wReserved[2] == 0xFFFF) ? 1:   DataPtr->wReserved[2];
  ADCReady=false;
 //Датчики 

  DATA1->VoltSensors.push_back(new TVoltageDetector(&AKB[4].AKB_OUT, &AKB[0].AKB_OUT, TaskPtr->CalVolt[0], 83, Pn,        fullsacle+1,  7.00));//U11
  DATA1->VoltSensors.push_back(new TVoltageDetector(&AKB[3].AKB_OUT, &AKB[0].AKB_OUT, TaskPtr->CalVolt[1], 83, Pn,        fullsacle+1, 13.012));//U12
  DATA1->VoltSensors.push_back(new TVoltageDetector(&AKB[2].AKB_OUT, &AKB[0].AKB_OUT, TaskPtr->CalVolt[2], 83, Pn,        fullsacle+1, 20.50));//U13
  DATA1->VoltSensors.push_back(new TVoltageDetector(&AKB[1].AKB_OUT, &AKB[0].AKB_OUT, TaskPtr->CalVolt[3], 83, Pn,        fullsacle+1, 25.00));//U14
  
  DATA1->TempSensors.push_back(new TVoltageDetector(&AKB[8].AKB_OUT, &AKB[0].AKB_OUT, TaskPtr->CalVolt[4], 83, (Pn+1),   fullsacle+1,  7.00));//U21
  DATA1->TempSensors.push_back(new TVoltageDetector(&AKB[7].AKB_OUT, &AKB[0].AKB_OUT, TaskPtr->CalVolt[5], 83, (Pn+1),   fullsacle+1, 13.012));//U22
  DATA1->TempSensors.push_back(new TVoltageDetector(&AKB[6].AKB_OUT, &AKB[0].AKB_OUT, TaskPtr->CalVolt[6], 83, (Pn+1),   fullsacle+1, 20.50));//U23
  DATA1->TempSensors.push_back(new TVoltageDetector(&AKB[5].AKB_OUT, &AKB[0].AKB_OUT, TaskPtr->CalVolt[7], 83, (Pn+1),   fullsacle+1, 25.00));//U24
  
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
    if(ADCReady)
    {
        float lastValue = 0.0;       
        foreach(IDetector* curr, PowerMetrs->VoltSensors)
        {
           float val = curr->GetValue();
           float divVal = val - lastValue;
           if(divVal < 0) divVal = 0.01;
           curr->U.CalcMinAverMax(divVal);  
           lastValue = val;
        };  
        lastValue = 0.0;  
        foreach(IDetector* curr, PowerMetrs->TempSensors)
        {
           float val = curr->GetValue();
           float divVal = val - lastValue;
           if(divVal < 0) divVal = 0.01;
           curr->U.CalcMinAverMax(divVal);  
           lastValue = val;
        }; 
        perflg = false;
        foreach(IDetector* curr, PowerMetrs->CurrSensors)
        {//ОШИБКА определения разряда!
           float val = curr->GetValue();
           if( val > -2.0 && val < 2.0) 
             val = 0.0;
           curr->U.CalcMinAverMax(val);  
           if(val <= -2.0)
             perflg = true;
        }; 
        ADCReady=false;
    }
    Fase = FORMING_MESSAGE;
}
//=============================================================================
// 
//======================================= 
void TPowerMetrManager::FASE_FormingMessageCMD(void *Par)
{
 Data_Params const      *ptr     = TObject::PtrToConstants; 
 Boot_Params const      *BOOTptr = TObject::PtrToBootConstants;
  
  if( Par != NULL)
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
         
         AddedStr +="\r\n>,"+TBuffer::WordToString(83)+','+TBuffer::WordToString(Pn)+',';
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
  }
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
IDetector::IDetector(DWord* adcmem, DWord* etalon, float koef, Byte ptype, Byte index, float mult):IParams(ptype, index, true), Koef(koef)
{ 
  ADCMEM          = adcmem;
  ADCETALON       = etalon;
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
//=============================================================================
// Constructor
//=======================================
TVoltageDetector::TVoltageDetector(DWord* adcmem, DWord* etalon, float koef, Byte ptype, Byte index, float mult, float del):IDetector(adcmem, etalon, koef, ptype, index, mult)
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
  //etalon1 = *ADCETALON;
  AverageValue = (1.2/ *ADCETALON) * (*ADCMEM);
  float U = AverageValue * Delitel;
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
  //etalon1 = *ADCETALON;
  AverageValue = (1.2/ *ADCETALON) * (*ADCMEM);;

  float U = AverageValue * Delitel;
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
//=============================================================================
