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
#define EN PORTCbits.RC6 //ENABLE SIGNAL for data (0-> send data, 1-process data and wait, 0->end)
#define RW PORTEbits.RE2 //R/W CONTROL R-1, W-0
#define RS PORTCbits.RC7 //RS CONTROL, INSTRUCTION-0, DATA-1

void LCD_Config();
void LCD_Init();
void LCD_Clear();
void LCD_Wait();

void LCD_Wait(){
    SWITCH_BF=1;
    NOP();
    
    RS=0;
    NOP();
    RW=1;
    NOP();
    //delay_ms(1)
    while(BF)NOP();
    
    RW=0;
    NOP();
    SWITCH_BF=0;
    NOP();
}

void Lcd_SetBit(char data_bit) //Based on the Hex value Set the Bits of the Data Lines
{
	if(data_bit& 1) 
		D4 = 1;
	else
		D4 = 0;

	if(data_bit& 2)
		D5 = 1;
	else
		D5 = 0;

	if(data_bit& 4)
		D6 = 1;
	else
		D6 = 0;

	if(data_bit& 8) 
		D7 = 1;
	else
		D7 = 0;
}

void Lcd_Cmd(char a)
{
	RS = 0;           
	Lcd_SetBit(a); //Incoming Hex value
	EN  = 1;         
    __delay_ms(5);
    EN  = 0;         
}

void LCD_CLEAR()
{
	Lcd_Cmd(0); //Clear the LCD
	Lcd_Cmd(1); //Move the cursor to first position
}

void Lcd_Set_Cursor(char a, char b)
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
}

void LCD_Init()
{
  LCD_Config();
  
  Lcd_SetBit(0x00);
  //for(int i=1065244; i<=0; i--)  NOP(); 
  __delay_ms(50);
  Lcd_Cmd(0x03);
  __delay_ms(5);
  //NOP();
  Lcd_Cmd(0x03);
  __delay_ms(10);
  //NOP();
  Lcd_Cmd(0x03); 
  Lcd_Cmd(0x02); //02H is used for Return home -> Clears the RAM and initializes the LCD
  Lcd_Cmd(0x02); //02H is used for Return home -> Clears the RAM and initializes the LCD
  Lcd_Cmd(0x08); //Select Row 1
  Lcd_Cmd(0x00); //Clear Row 1 Display
  Lcd_Cmd(0x0C); //Select Row 2
  Lcd_Cmd(0x00); //Clear Row 2 Display
  Lcd_Cmd(0x06);
}

void LCD_Config(){
    ANSELB=0;
    ANSELEbits.ANSE2=0;
    ANSELCbits.ANSC6=0;
    ANSELCbits.ANSC7=0;
    
    TRISB=0x00;
    TRISEbits.RE2=0;
    TRISCbits.RC6=0;
    TRISCbits.RC7=0;
    
    PORTB=0x00;
    RW=0;  //mode WRITE
    RS=0; //ROM select
    EN=0; //Not enable
}

void Lcd_Print_Char(char data)  //Send 8-bits through 4-bit mode
{
   char Lower_Nibble,Upper_Nibble;
   Lower_Nibble = data&0x0F;
   Upper_Nibble = data&0xF0;
   RS = 1;             // => RS = 1
   Lcd_SetBit(Upper_Nibble>>4);             //Send upper half by shifting by 4
   EN = 1;
   //NOP();
   //for(int i=2130483; i<=0; i--)  NOP(); 
   __delay_ms(100);
   //NOP();
   EN = 0;
   NOP();
   Lcd_SetBit(Lower_Nibble); //Send Lower half
   EN = 1;
   NOP();
   //for(int i=2130483; i<=0; i--)  NOP();
   __delay_ms(100);
   //NOP();
   EN = 0;
}

void Lcd_Print_String(char *a)
{
	int i;
	for(i=0;a[i]!='\0';i++)
	   Lcd_Print_Char(a[i]);  //Split the string using pointers and call the Char function 
}

#endif	/* LCD_ENGINELIB */

