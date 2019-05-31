#ifndef FLASH_MAP_H
#define FLASH_MAP_H
#include <type_define.h>



      #pragma location=0x0800E800
      __root const Data_Params  PARAMS = 
      {
        .DevAddress         = "11",
        .PassWord           = "TM_DP485",       
        .Unit_Type          = "IAP", 

        .MAIN_BOUD_RATE     = 38400,
        .MAIN_timeout       = 500,
        
        .wReserved[0]       = 26, // et
        .wReserved[1]       = 1,  // en
        .wReserved[2]       = 1,  // pn
        
        .Serial             = 1999999999,
        .CRC_SUM            = 0xFF,
        .KeyByte            = 0xAA
      };

     #pragma location = 0x0800EC00 
     __root const Data_Params  TMP_PARAMS = 
      {
        .DevAddress         = "11",
        .PassWord           = "TM_DP485",    
        .Unit_Type          = "IAP", 

        .wReserved[0]       = 26, // et
        .wReserved[1]       = 1,  // en
        .wReserved[2]       = 1,  // pn

        .MAIN_BOUD_RATE     = 38400,
        .MAIN_timeout       = 500,
        
        .Serial             = 1999999999,
        .CRC_SUM                = 0xFF,
        .KeyByte            = 0xAA
      };
     
      #pragma location =0x0800F000 
      __root const TTaskListConfig  TASK_PARAMS = 
      {       
        .AdcMultiplier      = 1024,
        
        .ReglamentTime      = 600,
        .IsFistStart        = true,
        .CRC_SUM            = 0x6A,
        .KeyByte            = 0xAA
      };     
      
      #pragma location =0x0800F400 
      __root const TTaskListConfig  TMP_TASK_PARAMS = 
      {    
        .AdcMultiplier      = 1024,
        
        
        .ReglamentTime      = 600,
        .IsFistStart        = true,
        .KeyByte            = 0xAA
      };
      
      #pragma location =0x0800F800 
      __root const TCalibrListConfig  CALIBR_PARAMS = 
      {
        .CalVolt={
                    1.0, //U1 0
                    1.0, //Icharge
                    1.0, //Iload
                    1.0, //Hbat
                    0.99847, //Batt
                    1.013426, //Uin
                    1.0, //T3 6
                    1.0  //T4 7
                  },
        .IsFistStart=true,
        //.CRC_SUM           = 2351,
        .KeyByte           = 0xAA
        
      }; 
      
      #pragma location =0x0800FC00 
      __root const TCalibrListConfig  TMP_CALIBR_PARAMS = 
      {    
        .CalVolt={
                    1.0, //U1 0
                    1.0, //Icharge
                    1.0, //Iload
                    1.0, //Hbat
                    0.99847, //Batt
                    1.013426, //Uin
                    1.0, //T3 6
                    1.0  //T4 7
                  },
        .IsFistStart=true,
        .KeyByte           = 0xAA
      };
     
      
#endif

