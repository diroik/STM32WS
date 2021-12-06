#ifndef FLASH_MAP_H
#define FLASH_MAP_H
#include <type_define.h>


/*****************************************************************************/
#pragma location=0x08080000
  __root const Data_Params  PARAMS = 
  {
  .DevAddress         = "11",
  .PassWord           = "TM_DP485",       
  .Unit_Type          = "MKAB-12", 
  .MAIN_BOUD_RATE     = 38400,
  .MAIN_timeout       = 500,
  .wReserved[0]       = 26, // et
  .wReserved[1]       = 1,  // en
  .wReserved[2]       = 1,  // pn
  .Serial             = 1999999999,
  
  .AdcMultiplier      = 4096,
  .CalVolt=
  {
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
  
  .BOOT_PARAMS=
  {
    .BootStatus      = 0x01,
    .CurVersion      = 0xFF, 
    .AppAdrStart     = 0xFFFFFFFF,
    .AppCurrAdr      = 0xFFFFFFFF,
    .Segment_Count   = 0x00,
    .AppCRC          = 0xFFFF,

    .WriteRamVectorsFlag    = 0xFF,
    .UsartIntVector         = 0xFFFF,
    .Timer0B1IntVector      = 0xFFFF,
    .Timer1A1IntVector      = 0xFFFF,
    .UnmIntVector           = 0xFFFF,
  },
  
  
  .CRC_SUM            = 0xFF,
  .KeyByte            = 0xAA
  };

  #pragma location = 0x08080200 
  __root const Data_Params  TMP_PARAMS = 
  {
  .DevAddress         = "11",
  .PassWord           = "TM_DP485",    
  .Unit_Type          = "MKAB-12", 
  .wReserved[0]       = 26, // et
  .wReserved[1]       = 1,  // en
  .wReserved[2]       = 1,  // pn
  .MAIN_BOUD_RATE     = 38400,
  .MAIN_timeout       = 500,
  .Serial             = 1999999999,
  
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
  .BOOT_PARAMS=
  {
    .BootStatus      = 0x01,
    .CurVersion      = 0xFF, 
    .AppAdrStart     = 0xFFFFFFFF,
    .AppCurrAdr      = 0xFFFFFFFF,
    .Segment_Count   = 0x00,
    .AppCRC          = 0xFFFF,

    .WriteRamVectorsFlag    = 0xFF,
    .UsartIntVector         = 0xFFFF,
    .Timer0B1IntVector      = 0xFFFF,
    .Timer1A1IntVector      = 0xFFFF,
    .UnmIntVector           = 0xFFFF,
  },
  
  .CRC_SUM                = 0xFF,
  .KeyByte            = 0xAA
  };
/*****************************************************************************/
  /*
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
  .CRC_SUM            = 0xFF,
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
  .CRC_SUM            = 0xFF,
  .KeyByte            = 0xAA
  };*/
/*****************************************************************************/
//  #pragma location =0x0800EC00 
//  __root const TCalibrListConfig  CALIBR_PARAMS = 
//  {
//  .CalVolt={
//              1.0, //U1 0
//              1.0, //U2 1
//              1.0, //U3 2
//              1.0, //U4 3
//              1.0, //T1 4
//              1.0, //T2 5
//              1.0, //T3 6
//              1.0  //T4 7
//            },
//  .IsFistStart=true,
//  //.CRC_SUM           = 2351,
//  .KeyByte           = 0xAA
//
//  }; 
//  #pragma location =0x0800F000 
//  __root const TCalibrListConfig  TMP_CALIBR_PARAMS = 
//  {    
//  .CalVolt={
//              1.0, //U1 0
//              1.0, //U2 1
//              1.0, //U3 2
//              1.0, //U4 3
//              1.0, //T1 4
//              1.0, //T2 5
//              1.0, //T3 6
//              1.0  //T4 7
//            },
//  .IsFistStart=true,
//  .KeyByte           = 0xAA
//  };
//      
/*****************************************************************************/
  /*
  #pragma location=0x08005C00
  __root const Boot_Params  BOOT_PARAMS = 
  {
  .BootStatus      = 0x01,
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
*/
      
#endif

