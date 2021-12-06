#ifndef INIT_H
    #define INIT_H
//=================================================
 void SYSTEM_INIT(void);
 void Init_Leds(void);
 void Init_Ports(void);
 void Init_OSC(void);
 void Init_WDT(void);

 void Init_TIM7(void);
 
 void Init_SysTick(void);
 void Init_SysTick(long val);
 
 void Init_Flash_Const(void);
 void Init_SRAM_Const(void) ;
 
 void Init_BOR(void);
 void Init_InputContacts(void);
 void DMA_Config(void);
 void Init_ADC12();
 void Start_ADC12(); 
 void Stop_ADC12();  
//=================================================
#endif