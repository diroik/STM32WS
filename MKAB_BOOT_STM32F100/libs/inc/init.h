#include <Classes.h>
#include <AdvClasses.h>

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
 
 void Init_Flash_Const(void);
 void SwitchToApplication(void);
 void Init_Calibr_Flash_Const(void);
 void Init_BOR(void);
 void Init_InputContacts(void);
 void Init_ADC12();
 void Start_ADC12(); 
 void Stop_ADC12();  
 void Init_TIMER_B(long val);
 void nvic_set_vector_table(uint32_t Offset);
 void jump_to_application (uint32_t ApplicationAddress);
 void reset_all_periph (void);
//=================================================
 
 
Byte                TCmd::Fase            = 0;
Word                TCmd::ErrorCNT        = 0;
Word                TCmd::LastErrorIndex  = 0;
string              TCmd::LastCMD         = "";
vector<string>      TCmd::LastData;
string              TCmd::Addr            = "";
bool                TCmd::ResetFlg        = false; 
Tlist<TCmd*>TCmd::CMD_Messages;  


#endif