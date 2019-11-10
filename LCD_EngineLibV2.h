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
#ifndef LCD_ENGINELIB
#define	LCD_ENGINELIB

#include <xc.h> // include processor files - each processor file is guarded.  
/*HD44780 standard */
#define SWITCH_BF TRISBbits.RB7 //switch BF ON/OFF

//DATA SIGNAL PINS BCD MODE
#define D4 PORTBbits.RB4
#define D5 PORTBbits.RB5
#define D6 PORTBbits.RB6
#define D7 PORTBbits.RB7

//CONTROL SIGNAL PING
#define BF PORTBbits.RB7 //BF- ReadyFlag (DB7)
#define RW PORTCbits.RC6 //R/W CONTROL R-1, W-0 
#define EN PORTAbits.RA5 //ENABLE SIGNAL for data (0-> send data, 1-process data and wait, 0->end)
#define RS PORTCbits.RC7 //RS CONTROL, INSTRUCTION-0, DATA-1

enum instructions { CRL_DISPLAY=0x01, CURS_HOME=0x02, ENTRY_MODE_INS=0x06, ON_DISPLAY=0x0F, OFF_DISPLAY=0x08,
    CURS_SHIFT_RIGHT=0x14, CURS_SHIFT_LEFT=0x10, FCE_SET_4B16D5x7=0x28, CGRAM_ADDR=0x40, DDRAM_ADDR=0x80 };

void LCD_Config();
void LCD_Init();
void LCD_Clear();
void LCD_Wait();
void LCD_On();

void LCD_SetNibble(char data_nibble);
void EnableCycle();
void LCD_Cmd(char isr);
void LCD_ShortCmd(char isr);
void LCD_Data(char data);

//void LCD_Set_Cursor(char line, char pos);
void LCD_Print_Char(char data);
void LCD_Print_String(char *s);

void LCD_Wait(){
    SWITCH_BF=1;
    NOP();
    
    RS=0;
    NOP();
    RW=1;
    NOP();
    EN = 1;
    __delay_us(100);
    
    while(BF)NOP();
    
    EN = 0;
    RW=0;
    NOP();
    SWITCH_BF=0;
    NOP();
}

void LCD_SetNibble(char data_nibble) //Based on the Hex value Set the Bits of the Data Lines
{
    if(data_nibble& 1) 
		D4 = 1;
	else
		D4 = 0;
    NOP();
    
	if(data_nibble& 2)
		D5 = 1;
	else
		D5 = 0;
    NOP();
    
	if(data_nibble& 4)
		D6 = 1;
	else
		D6 = 0;
    NOP();
    
	if(data_nibble& 8) 
		D7 = 1;
	else
        D7=0;
    NOP();
}

void EnableCycle(){
	EN  = 1; 
    __delay_ms(5);
    EN  = 0;    
}

void LCD_ShortCmd(char isr){
    RW=0;
    LCD_SetNibble(isr&0x0F); //LSB ONLY
    NOP();
    RS=0;
    NOP();
    EnableCycle();
}
void LCD_Cmd(char isr)
{
    RW=0;
	LCD_SetNibble(isr>>4); //MSB
    NOP();
    RS = 0;  
    NOP();
    EnableCycle();
    NOP();  
    LCD_SetNibble(isr&0x0F); //LSB
    NOP();
    EnableCycle();
}

void LCD_Data(char data)
{
    RW=0;
	LCD_SetNibble(data>>4); //MSB
    NOP();
    RS = 1;  
    NOP();
    EnableCycle();    
    NOP();
    LCD_SetNibble(data&0x0F); //LSB
    EnableCycle();
    LCD_Wait();      
}

void LCD_Clear()
{
	LCD_Cmd(CRL_DISPLAY); //Clear the LCD
    LCD_Wait();
	LCD_Cmd(CURS_HOME); //Move the cursor to first position
    LCD_Wait();
}

/*void LCD_Set_Cursor(char line, char pos)
{
	char temp,z,y;
	if(a== 1)
	{
	  temp = 0x80 + b - 1; //80H is used to move the cursor
		z = temp>>4; //Lower 8-bits
		y = temp & 0x0F; //Upper 8-bits
		Lcd_Cmd(z); //Set Row
		Lcd_Cmd(y); //Set Column
	}
	else if(a== 2)
	{
		temp = 0xC0 + b - 1;
		z = temp>>4; //Lower 8-bits
		y = temp & 0x0F; //Upper 8-bits
		Lcd_Cmd(z); //Set Row
		Lcd_Cmd(y); //Set Column
	}
}*/

void LCD_Init() //4bit init
{
  LCD_Config();
  NOP();
  __delay_ms(20);
  NOP();  
  RS=RW=0;
  NOP();
  LCD_ShortCmd(0x03);
  __delay_ms(5);
  LCD_ShortCmd(0x03);
  __delay_ms(1);
  LCD_ShortCmd(0x03);
  NOP();
  LCD_ShortCmd(0x02);
  NOP();
  LCD_Cmd(FCE_SET_4B16D5x7);
  NOP();
  LCD_Cmd(OFF_DISPLAY);
  NOP();
  LCD_Cmd(CRL_DISPLAY);
  NOP();
  LCD_Cmd(ENTRY_MODE_INS);
  NOP();
  LCD_Wait();
}

void LCD_Config(){
    ANSELB=0;
    ANSELAbits.ANSA5=0;
    ANSELEbits.ANSE2=0;
    ANSELCbits.ANSC6=0;
    ANSELCbits.ANSC7=0;
    
    TRISB=0x00;
    TRISAbits.RA5=0;
    TRISEbits.RE2=0;
    TRISCbits.RC6=0;
    TRISCbits.RC7=0;
    
    PORTB=0x00;
    RW=0;  //mode WRITE
    RS=0; //ROM select
    EN=0; //Not enable    
}

void LCD_On(){
    LCD_Cmd(ON_DISPLAY);
    LCD_Wait();
}

void LCD_Print_Char(char data)  //Send 8-bits through 4-bit mode
{
    LCD_Data(data);
}

void LCD_Print_String(char *s)
{
	int i;
	for(i=0;s[i]!='\0';i++)
	   LCD_Print_Char(s[i]);  //Split the string using pointers and call the Char function 
}

#endif	/* LCD_ENGINELIB */

