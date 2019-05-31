#ifndef FLASH_MAP_H
#define FLASH_MAP_H

#include <type_define.h>
//******************************************************************************
// Main parameters
//******************************************************************************
      #pragma location=PARAMS_ADDR
      __root const Data_Params  PARAMS = 
      {
        .DATA[BOOT_FLG] = 0,
#ifdef UM_DEV   
        .DATA[DEV_ADDR] = 2,
#else
        .DATA[DEV_ADDR] = 1,
#endif 
        .DATA[TIME_OUT]         = 50,
        .DATA[BOUD_RATE]        = 9600,
        .DATA[BYTES_CNT]        = 200,
        .DATA[MIN_BYTE_LEN]     = 3,
        
        .DATA[PLC_TIME_OUT]     = 200,
        .DATA[PLC_BOUD_RATE]    = 600,
        .DATA[PLC_BYTES_CNT]    = 250,
        .DATA[PLC_MIN_BYTE_LEN] = 10,
        
        
        .DATA[DEV_CURR_VER]       = 4,
        .DATA[DEV_SUB_VER]        = 0,
        
        .DATA[SERIAL_LOW]       = 1,
        .DATA[SERIAL_HI]        = 0,
                
        .filter         = 1,                    // 0 - off, 1- on
        .sensitive      = 1,                 // 0 - off, 1- on
        .deviation      = 1,                 // 0 - off, 1- on

        .CRC_SUM                = 0xFF,
        .KeyByte                = 0xAA
      };
//******************************************************************************
      #pragma location=TMP_PARAMS_ADDR
      __root const Data_Params TMP_PARAMS = 
      {
        .DATA[BOOT_FLG] = 0,
#ifdef UM_DEV   
        .DATA[DEV_ADDR] = 2,
#else
        .DATA[DEV_ADDR] = 1,
#endif 
        .DATA[TIME_OUT]         = 50,
        .DATA[BOUD_RATE]        = 9600,
        .DATA[BYTES_CNT]        = 200,
        .DATA[MIN_BYTE_LEN]     = 3,
        
        .DATA[PLC_TIME_OUT]     = 200,
        .DATA[PLC_BOUD_RATE]    = 600,
        .DATA[PLC_BYTES_CNT]    = 250,
        .DATA[PLC_MIN_BYTE_LEN] = 10,
        
        
        .DATA[DEV_CURR_VER]     = 4,
        .DATA[DEV_SUB_VER]      = 0,
        
        .DATA[SERIAL_LOW]       = 1,
        .DATA[SERIAL_HI]        = 0,
                
        .filter         = 1,                    // 0 - off, 1- on
        .sensitive      = 1,                 // 0 - off, 1- on
        .deviation      = 1,                 // 0 - off, 1- on

        .CRC_SUM                = 0xFF,
        .KeyByte                = 0xAA
      };
//******************************************************************************
// bootloader parameters    
//******************************************************************************
    #pragma location=BOOT_MEM_START
    __root const Boot_Params BOOT_PARAMS = 
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
//******************************************************************************
    #pragma location=TMP_BOOT_MEM_START 
    __root const Boot_Params  TMP_BOOT_PARAMS @TMP_BOOT_MEM_START = 
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
//******************************************************************************
#endif

