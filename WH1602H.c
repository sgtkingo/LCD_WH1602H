/*
 * File:   WH1602H.c
 * Author: kater
 *
 * Created on 3. listopadu 2019, 15:21
 */
#include "PICF18LF46K22_ConfigSFR.h"
#include "LCD_Engine4bit.h"

#define N 5

char NumberBuffer[N]={};

void InitDevice(){
    OSCCON=0b01111100; //osc setting, 16 MHz, internal by FOSCH     
    LCD_Initialize();
    __delay_ms(10);
}

void ClearDevice(){
    LCD_Clear();
}
void TestDevice(){
    LCDPutStr("Vono to zije! ");
    LCDGoto(0,1);
    
    convertNumber(1996, NumberBuffer,N);
    LCDPutStr(NumberBuffer);
    __delay_ms(1000);
}

void main(void) {
    InitDevice();
    ClearDevice();
    TestDevice();
    __delay_ms(100);
    while(1);
}


