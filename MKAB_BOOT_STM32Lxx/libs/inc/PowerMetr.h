#ifndef PWR_H
#define PWR_H
//======================
#include <type_define.h>
#include <Classes.h>
#include <AdvClasses.h>

  class TPowerMetrManager;
  class TPowerMetr;
  class TVoltageDetector;
  class TCurrentCalculator;
  class IDetector;
//=============================================================================
// 
// 
//==================================== 
class TPowerMetrManager:public TFastTimer
{  
public:
  TPowerMetrManager(Word period);
  ~TPowerMetrManager();
  
  void Init(Data_Params const * DataPtr, TTaskListConfig const *TaskPtr,TCalibrListConfig const *CalPtr);
  void FASE_DetectStates( void );
  void FASE_FormingMessageCMD( void *Par = NULL );//Значения Температуры

  bool ADCReady;
  Byte Fase;

  TPowerMetr*      PowerMetrs;
  bool FirstInitFlg;  
private:      
  Word      Period;
  Word      Et;
  Word      En;
  Word      Pn;
};
//=============================================================================
// 
// 
//==================================== 
class TPowerMetrParameter
{
public:
  TPowerMetrParameter(Byte pt, Byte pn, Tlist<string> &val):Ptype(pt), Pname(pn), Value(val)
  {}
  Byte Ptype;
  Byte Pname;
  Tlist<string> Value;
};
//==================================== 
class TPowerMetr:public TFastTimer
{  
public:
  TPowerMetr(void);
  ~TPowerMetr();
  Tlist<IDetector*>      VoltSensors;
  Tlist<IDetector*>      CurrSensors;
  Tlist<IDetector*>      TempSensors;
  
  bool          Modify;

private:         
};
//=============================================================================
//==================================== 
class IDetector: public IParams
{  
public:
  IDetector(DWord* adcmem, float koef, Byte ptype, Byte index, float mult);
  ~IDetector();
  
  virtual DETECTING_RESULT  DetectPorogs(float Value) = 0;
  virtual float             GetValue() = 0;
  virtual float             GetSimpleValue() = 0;
  
  
    string GetValueString(void)
    {
      string ReturnStr="";
      if(Visible)
      {
         ReturnStr = "";
         for( int i=0; i < Params.size();i++)
         {
            CLEAR_WDT;
            TMamStruct *CurrMam = Params[i];
            ReturnStr += TBuffer::FloatToString(CurrMam->Aver)+','+        
                         TBuffer::FloatToString(CurrMam->Min)+','+      
                         TBuffer::FloatToString(CurrMam->Max)+',';       
           CurrMam->ClrDivider();
         }
      }
        return ReturnStr;
    }
  
  float           Koef;  
  TMamStruct      U;
  
//private:         
  DWord*  ADCMEM;
  DWord   AverageValue;
  float   Value;

  long    Divider;
  
  float   Delitel;
  float   Multiplier;
};
//=============================================================================
//==================================== 
class TVoltageDetector: public IDetector
{  
public:
  TVoltageDetector(DWord* adcmem, float koef, Byte ptype, Byte index, float mult);
  TVoltageDetector(DWord* adcmem, float koef, Byte ptype, Byte index, float mult, float del);
  ~TVoltageDetector();
  
  DETECTING_RESULT  DetectPorogs(float Value);
  float             GetValue();
  float             GetSimpleValue( );

  
private:         

};
//=============================================================================
//==================================== 
class TCurrentDetector: public IDetector
{  
public:
  TCurrentDetector(DWord* adcmem, float koef, Byte ptype, Byte index, float mult, float par1 = 1);
  ~TCurrentDetector();
  
  DETECTING_RESULT  DetectPorogs(float Value);
  float             GetValue();
  float             GetSimpleValue( );

  
private:         
    float GetSens();
    float sens;
};
//=============================================================================
// 
//==================================== 
typedef void (TPowerMetr::*CPFV)(void* p);
//==================================== 
struct TPowerMetrEventParameters
{
   TPowerMetr*            Cond;    
   //CPFV                     Func;
   TPowerMetrParameter*   Params;
};


#endif  