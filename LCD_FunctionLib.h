/* Microchip Technology Inc. and its subsidiaries.  You may use this software 
 * and any derivatives exclusively with Microchip products. 
 * 
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS".  NO WARRANTIES, WHETHER 
 * EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED 
 * WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A 
 * PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION 
 * WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION. 
 *
 * IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
 * INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
 * WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS 
 * BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE 
 * FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS 
 * IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF 
 * ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE 
 * TERMS. 
 */

/* 
 * File:   
 * Author: 
 * Comments:
 * Revision history: 
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef LCD_FUNCTIONLIB
#define	LCD_FUNCTIONLIB

#include "LCD_ConfigLib.h"

#define D() __delay_ms(500);


void LCD_Initialize()
{
    LCD_Config();

    /*// power up the LCD
    LCD_PWR = 1;*/

    LCD_pause(LCD_StartupTime);
    LCD_RS=LCD_RW=0;

    LCD_pause(LCD_shortDelayTime);
    
    LCDQuickWriteNibble(0x3);
    LCD_pause(5000);
    LCDQuickWriteNibble(0x3);
    LCD_pause(150);
    LCDQuickWriteNibble(0x3);
    LCDQuickWriteNibble(0x2);

    // set interface size, # of lines and font
    LCDPutCmd(FUNCTION_SET4bit);

    // turn off display and sets up cursor
    LCDPutCmd(DISPLAY_OFF);
    
    //set display clear
    LCDPutCmd(LCD_CLEAR);
    
    // set cursor movement direction
    LCDPutCmd(ENTRY_MODE);
}

void LCD_Clear(){
    LCDPutCmd(LCD_CLEAR);
    LCDPutCmd(LCD_HOME);
    LCD_pause(LCD_longDelayTime);//Wait to clear display
}

void LCD_Config(){
    ANSELB=0;
    TRISB=0xF0; //RB0-RB4 to DB4-DB7
    PORTB=0x00;
    
    ANSELCbits.ANSC6=0;
    ANSELCbits.ANSC7=0;
    ANSELEbits.ANSE2=0;
    TRISCbits.TRISC6=0;
    TRISCbits.TRISC7=0;
    TRISEbits.TRISE2=0;
    NOP();
    
    LCD_RW=0;  //mode WRITE
    LCD_RS=0; //ROM select
    LCD_EN=0; //Not enable    
}

void LCD_pause(unsigned int t){
    while(t--)__delay_us(1);
}


void LCD_Enable(){
	LCD_EN  = 1; 
    LCD_pause(LCD_enableDelayTime);
    LCD_EN  = 0; 
}

void LCDQuickWriteNibble(char data_nibble){
    NOP();
    PORTB=(data_nibble & 0x0F);
    NOP();
    LCD_Enable();
}


void LCDWriteNibble(char data_nibble,char rs)
{
    // set data/instr bit to 0 = insructions; 1 = data
    LCD_RS = rs;
    // RW - set write mode
    LCD_RW = 0;
    //EN ready
    LCD_pause(LCD_shortDelayTime);
    
    LCD_EN  = 1; 
    PORTB=(data_nibble & 0x0F);
    LCD_Enable();  
}

void LCDPutChar(char ch)
{
    //Send higher nibble first MSB
    LCDWriteNibble((ch>>4),DAT);
    // Now send the low nibble LSB
    LCD_pause(LCD_shortDelayTime); //wait to process data
    LCDWriteNibble((ch&0x0F),DAT);
    
    LCD_pause(LCD_longDelayTime);//wait to print char
}

    
void LCDPutCmd(char cmd)
{
    //Send higher nibble first MSB
    LCDWriteNibble((cmd>>4),ISR);
    // Now send the low nibble LSB
    LCD_pause(LCD_shortDelayTime); //wait to process data
    LCDWriteNibble((cmd&0x0F),ISR);
    
    LCD_pause(LCD_shortDelayTime); //wait to execute cmd
}

 
void LCDPutStr(const char *str)
{
    char i=0;
    
    // While string has not been fully traveresed
    while (str[i])
    {
        // Go display current char
        LCDPutChar(str[i++]);
    }
    
}

void LCDGoto(char pos,char ln)
{
    // if incorrect line or column
    if ((ln > (NB_LINES-1)) || (pos > (NB_COL-1)))
    {
        // Just do nothing
        return;
    }

    // LCD_Goto command
    LCDPutCmd((ln == 1) ? (0xC0 | pos) : (0x80 | pos));

    // Wait for the LCD to finish
    LCD_pause(LCD_shortDelayTime);
}

#endif

