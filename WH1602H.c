/*
 * File:   WH1602H.c
 * Author: kater
 *
 * Created on 3. listopadu 2019, 15:21
 */
#include "PICF18LF46K22_ConfigSFR.h"
#include "LCD_EngineLibV2.h"

#include <xc.h>

void InitDevice(){
    OSCCON=0b01111100; //osc setting, 16 MHz, internal by FOSCH
    
    LCD_Init();    
}
void ClearDevice(){
    LCD_Clear();
}
void TestDevice(){
    //Lcd_Set_Cursor(1, 0);
    LCD_On();
    LCD_Print_Char('A');
    //Lcd_Print_String("HELLO WORLD!");
    return;
}

void main(void) {
    InitDevice();
    ClearDevice();
    TestDevice();
    
    
    while(1);
    return;
}
