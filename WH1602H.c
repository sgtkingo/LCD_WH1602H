/*
 * File:   WH1602H.c
 * Author: kater
 *
 * Created on 3. listopadu 2019, 15:21
 */
#include "PICF18LF46K22_ConfigSFR.h"
#include "LCD_Engine.h"

void InitDevice(){
    OSCCON=0b01111100; //osc setting, 16 MHz, internal by FOSCH  
    ANSELD=0x00;
    TRISD=0x00;
    
    LCD_Initialize();
    __delay_ms(10);
}

void ClearDevice(){
    PORTD=0x00;
    LCD_Clear();
    LCDGoto(0,0);
}
void TestDevice(){
    LCDPutStr("Hello Word");
    __delay_ms(1000);
}

void main(void) {
    InitDevice();
    ClearDevice();
    TestDevice();
    __delay_ms(100);
    while(1);
}


