/*
 * File:   lcd.c
 * Author: kater
 *
 * Created on 4. prosince 2019, 18:16
 */


#include "PICF18LF46K22_ConfigSFR.h"

// set up the timing for the LCD delays
#define LCD_enableDelayTime    100     // ~1ms
#define LCD_shortDelayTime     2000     // ~2ms
#define LCD_longDelayTime      5000     // ~5ms
#define LCD_StartupTime        15000    // ~15ms

// Command set for Hitachi 44780U LCD display controller
#define LCD_CLEAR           0x01
#define LCD_HOME            0x02
#define LCD_CURSOR_BACK     0x10
#define LCD_CURSOR_FWD      0x14
#define LCD_PAN_LEFT        0x18
#define LCD_PAN_RIGHT       0x1C
#define LCD_CURSOR_OFF      0x0C
#define LCD_CURSOR_ON       0x0E
#define LCD_CURSOR_BLINK    0x0F
#define LCD_CURSOR_LINE2    0xC0

// display controller setup commands from page 46 of Hitachi datasheet
#define FUNCTION_SET4bit    0x28          // 4 bit interface, 2 lines, 5x8 font
#define ENTRY_MODE          0x06          // increment mode
#define DISPLAY_SETUP       0x0C          // display on, cursor off, blink offd
#define DISPLAY_OFF         0x08          // display off, cursor off, blink offd

//----------------------------------------------------------------------
// Definitions specific to the PICDEM 2 Plus
// These apply to the Black (2011) version.
//----------------------------------------------------------------------

// single bit for selecting command register or data register
#define ISR        0
#define DAT         1

/*// These #defines create the pin connections to the LCD in case they are changed on a future demo board
#define LCD_D7     PORTBbits.RB3
#define LCD_D6     PORTBbits.RB2
#define LCD_D5     PORTBbits.RB1
#define LCD_D4     PORTBbits.RB0*/

//#define LCD_PWR      PORTBbits.RB0                      // LCD power pin
#define LCD_EN       PORTCbits.RC6                     // LCD enable
#define LCD_RW       PORTEbits.RE2                      // LCD read/write line
#define LCD_RS       PORTCbits.RC7                      // LCD register select line

#define NB_LINES    2                                   // Number of display lines
#define NB_COL      16                                  // Number of characters per line


void LCD_Config();
void LCD_Initialize();
void LCD_Clear();
void LCD_Config();
void LCD_Enable();
void LCDQuickWriteNibble(char data_nibble);
void LCDWriteNibble(char data_nibble,char rs);
void LCDPutCmd(char cmd);
void LCDPutChar(char ch);
void LCD_pause(unsigned int t);
void LCDGoto(char pos,char ln);
void LCDPutStr(const char *str);

void LCD_Initialize()
{
    LCD_Config();

    /*// power up the LCD
    LCD_PWR = 1;*/

    // required by display controller to allow power to stabilize
    LCD_pause(LCD_StartupTime);
    LCD_RS=LCD_RW=0;

    LCD_pause(LCD_shortDelayTime);
    
    LCDQuickWriteNibble(0x3);
    LCD_pause(5000);
    LCDQuickWriteNibble(0x3);
    LCD_pause(300);
    LCDQuickWriteNibble(0x3);
    LCDQuickWriteNibble(0x2);

    // set interface size, # of lines and font
    LCDPutCmd(FUNCTION_SET4bit);

    // turn off display and sets up cursor
    LCDPutCmd(DISPLAY_SETUP);
    
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

void LCD_Enable(){
	LCD_EN  = 1; 
    LCD_pause(LCD_enableDelayTime);
    LCD_EN  = 0; 
}

void LCDQuickWriteNibble(char data_nibble){
    NOP();
    PORTB=(data_nibble & 0x0F);
    //LCD_pause(100);
    LCD_Enable();
}


void LCDWriteNibble(char data_nibble,char rs)
{
    // set data/instr bit to 0 = insructions; 1 = data
    LCD_RS = rs;
    // RW - set write mode
    LCD_RW = 0;
    //EN ready
    LCD_EN  = 1; 
    PORTB=(data_nibble & 0x0F);
    LCD_Enable();  
}

void LCD_pause(unsigned int t){
    while(t--)__delay_us(1);
}

void LCDPutChar(char ch)
{
    LCD_pause(LCD_shortDelayTime);//wait to print char
    //Send higher nibble first MSB
    LCDWriteNibble((ch>>4),DAT);
    // Now send the low nibble LSB
    //LCD_pause(LCD_shortDelayTime); //wait to process data
    LCDWriteNibble((ch&0x0F),DAT);
 
}

void LCDPutCmd(char cmd)
{
    LCD_pause(LCD_shortDelayTime); //wait to execute cmd
    //Send higher nibble first MSB
    LCDWriteNibble((cmd>>4),ISR);
    // Now send the low nibble LSB
    //LCD_pause(LCD_shortDelayTime); //wait to process data
    LCDWriteNibble((cmd&0x0F),ISR);
    
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

void InitDevice(){
    OSCCON=0b01111100; //osc setting, 16 MHz, internal by FOSCH  
    ANSELD=0x00;
    TRISD=0x00;
}

void main(void) {
    InitDevice();
    PORTD=0xff;
    
    LCD_Initialize();
    __delay_ms(100);
    LCDPutChar('a');
    //LCD_Clear();
    //LCDGoto(0,0);
    //LCDPutStr("Hello Word");
    PORTD=0x55;
    while(1);
}
