#ifndef FLASH_MAP_H
#define FLASH_MAP_H
#include <type_define.h>


#pragma location=0x08005400
  __root const Data_Params  PARAMS = 
  {
  .DevAddress         = "11",
  .PassWord           = "TM_DP485",       
  .Unit_Type          = "MKAB-MGF", 

  .MAIN_BOUD_RATE     = 38400,
  .MAIN_timeout       = 500,

  .wReserved[0]       = 26, // et
  .wReserved[1]       = 1,  // en
  .wReserved[2]       = 1,  // pn

  .Serial             = 1999999999,
  .CRC_SUM            = 0xFF,
  .KeyByte            = 0xAA
  };

  #pragma location = 0x08005800 
  __root const Data_Params  TMP_PARAMS = 
  {
  .DevAddress         = "11",
  .PassWord           = "TM_DP485",    
  .Unit_Type          = "MKAB-MGF", 

  .wReserved[0]       = 26, // et
  .wReserved[1]       = 1,  // en
  .wReserved[2]       = 1,  // pn

  .MAIN_BOUD_RATE     = 38400,
  .MAIN_timeout       = 500,

  .Serial             = 1999999999,
  .CRC_SUM                = 0xFF,
  .KeyByte            = 0xAA
  };
/*****************************************************************************/
  #pragma location =0x08005C00
  __root const TTaskListConfig  TASK_PARAMS = 
  {       
  .AdcMultiplier      = 4096,
  .CalVolt={
              1.0, //U1 0
              1.0, //U2 1
              1.0, //U3 2
              1.0, //U4 3
              1.0, //T1 4
              1.0, //T2 5
              1.0, //T3 6
              1.0  //T4 7
            },  
  .ReglamentTime      = 600,
  .IsFistStart        = true,
  .CRC_SUM            = 0x6A,
  .KeyByte            = 0xAA
  };     

  #pragma location =0x08006000 
  __root const TTaskListConfig  TMP_TASK_PARAMS = 
  {    
  .AdcMultiplier      = 4096,
  .CalVolt={
              1.0, //U1 0
              1.0, //U2 1
              1.0, //U3 2
              1.0, //U4 3
              1.0, //T1 4
              1.0, //T2 5
              1.0, //T3 6
              1.0  //T4 7
            },


  .ReglamentTime      = 600,
  .IsFistStart        = true,
  .KeyByte            = 0xAA
  };
      
      
      
      




  #pragma location=0x0800F400
  __root const Boot_Params  BOOT_PARAMS = 
  {
  .BootStatus      = 0x00,
  .CurVersion      = BOOTLOADER_CURR_VER, 
  .AppAdrStart     = 0xFFFFFFFF,
  .AppCurrAdr      = 0xFFFFFFFF,
  .Segment_Count   = 0x00,
  .AppCRC          = 0xFFFF,

  .WriteRamVectorsFlag    = 0xFF,
  .UsartIntVector         = 0xFFFF,
  .Timer0B1IntVector      = 0xFFFF,
  .Timer1A1IntVector      = 0xFFFF,
  .UnmIntVector           = 0xFFFF,
  .KeyByte         = 0xAA  
  };

  #pragma location=0x0800F800
  __root const Boot_Params  TMP_BOOT_PARAMS = 
  {
  .BootStatus      = 0x00,
  .CurVersion      = BOOTLOADER_CURR_VER, 
  .AppAdrStart     = 0xFFFFFFFF,
  .AppCurrAdr      = 0xFFFFFFFF,
  .Segment_Count   = 0x00,
  .AppCRC          = 0xFFFF,

  .WriteRamVectorsFlag    = 0xFF,
  .UsartIntVector         = 0xFFFF,
  .Timer0B1IntVector      = 0xFFFF,
  .Timer1A1IntVector      = 0xFFFF,
  .UnmIntVector           = 0xFFFF,  
    .KeyByte              = 0xFF
  };
#endif

