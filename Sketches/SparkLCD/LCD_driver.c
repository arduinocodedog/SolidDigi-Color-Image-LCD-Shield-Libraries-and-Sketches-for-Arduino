//************************************************************************
//
//				LCD_driver.c: Interface for Nokia LCD
//
//************************************************************************
//************************************************************************
//*	Edit History
//*		<MLS>	= Mark Sproul msproul -at- jove.rutgers.edu
//************************************************************************
//*	Apr  2,	2010	<MLS> I received my Color LCD Shield sku: LCD-09363 from sparkfun
//*	Apr  2,	2010	<MLS> The code was written for WinAVR, I modified it to compile under Arduino
//*	Apr  3,	2010	<MLS> Changed LCDSetPixel to make it "RIGHT" side up
//*	Apr  3,	2010	<MLS> Made LCDSetPixel public
//*	Apr  3,	2010	<MLS> Working on MEGA, pin defs in nokia_tester.h
//*	Apr  4,	2010	<MLS> Removed delays from LCDCommand & LCDCommand, increased speed by 75%
//************************************************************************
#include "nokia_tester.h"
#include "LCD_driver.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#ifdef _USE_ARDUINO_FOR_NOKIEA_
#include	"Arduino.h"
#endif

//************************************************************************
//
//				"Private" Function Definitions
//
//************************************************************************


//************************************************************************
//
//				Global Variables for LCD driver
//
//************************************************************************
//static char x_offset = 0;
//static char y_offset = 0;

//The spark logo array defines the SparkFun logo that is displayed during startup on the splash screen.  The array may
//be changed to display a different image at startup or removed altogether.  The array is local to the LCD driver and
//can not be used outside of this file.
static char logo_spark[1120] =	{
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x78,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xf0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0xe0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0xe0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0xf0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xfb,0x80,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xff,0x80,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7f,0x80,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0c,0x3f,0xc0,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1c,0x3f,0xc0,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3c,0x7f,0xc0,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3f,0xff,0x80,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3f,0xff,0x80,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3f,0xff,0x80,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3f,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3f,0xfe,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3f,0xfc,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3f,0xe0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3e,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3c,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x38,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x30,0x0f,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x0e,0x20,0x1f,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x1e,0x00,0x3f,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x1e,0x00,0x3c,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x1e,0x00,0x3c,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x0f,0xe0,0x9f,0x01,0xfc,0x09,0x9e,0x1e,0x7f,0x70,0x73,0x9f,0x00,0x00,0x00,0x00,
  0x3f,0xf1,0xff,0x87,0xfe,0x3f,0xde,0x3d,0xff,0x78,0xf3,0xff,0x80,0x00,0x00,0x00,
  0x3c,0xf9,0xff,0xc7,0xdf,0x3f,0xde,0x79,0xff,0x78,0xf3,0xff,0xc0,0x00,0x00,0x00,
  0x78,0x79,0xc3,0xcf,0x0f,0x3f,0x1c,0xf0,0x3c,0x78,0xf3,0xe3,0xc0,0x00,0x00,0x00,
  0x7c,0x01,0xc1,0xe0,0x0f,0x3e,0x1f,0xe0,0x3c,0x78,0xf3,0xc3,0xc0,0x00,0x00,0x00,
  0x3f,0xc1,0x81,0xe0,0x3f,0x3c,0x1f,0xe0,0x3c,0x78,0xf3,0xc1,0xc0,0x00,0x00,0x00,
  0x1f,0xf1,0x81,0xe3,0xff,0x3c,0x1f,0xe0,0x3c,0x78,0xf3,0xc1,0xc0,0x00,0x00,0x00,
  0x07,0xf9,0x81,0xe7,0xef,0x3c,0x1f,0xf0,0x3c,0x78,0xf3,0xc1,0xc0,0x00,0x00,0x00,
  0x00,0xf9,0x81,0xef,0x07,0x3c,0x1e,0xf8,0x3c,0x78,0xf3,0xc1,0xc0,0x00,0x00,0x00,
  0x78,0x79,0xc1,0xef,0x0f,0x3c,0x1e,0x78,0x3c,0x78,0xf3,0xc1,0xc0,0x00,0x00,0x00,
  0x78,0x79,0xe3,0xcf,0x0f,0x3c,0x1e,0x3c,0x3c,0x7c,0xf3,0xc1,0xc0,0x00,0x00,0x00,
  0x3f,0xf9,0xff,0xcf,0xff,0x3c,0x1e,0x3e,0x3c,0x7f,0xf3,0xc1,0xcf,0x00,0x00,0x00,
  0x1f,0xf1,0xff,0x87,0xff,0x3c,0x1e,0x1e,0x3c,0x3f,0xf3,0xc1,0xc7,0x00,0x00,0x00,
  0x07,0xc1,0x9e,0x03,0xe0,0x00,0x00,0x02,0x00,0x0e,0x20,0x00,0x00,0x00,0x00,0x00,
  0x00,0x01,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x01,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x03,0x80,0x00,0x00,0x00,0xc0,0x00,0x00,0x18,0x00,0x00,0x08,0x08,0x00,0x00,
  0x00,0x01,0x87,0xc3,0x03,0xe0,0xe1,0xf0,0xf8,0x3e,0x33,0x08,0x3e,0x1e,0x00,0x00,
  0x00,0x01,0x86,0x03,0x03,0x01,0xb0,0xe0,0xdc,0x66,0x3b,0x08,0x66,0x32,0x00,0x00,
  0x00,0x00,0x87,0xc3,0x03,0xe1,0x80,0x40,0xd8,0x63,0x3b,0x08,0x60,0x3c,0x00,0x00,
  0x00,0x00,0x87,0x83,0x03,0xc1,0x80,0x40,0xf8,0x63,0x3f,0x08,0x60,0x0e,0x00,0x00,
  0x00,0x00,0x06,0x03,0x03,0x01,0xb0,0x40,0xd8,0x66,0x37,0x08,0x66,0x32,0x00,0x00,
  0x00,0x00,0x07,0xc3,0xe3,0xe0,0xe0,0x40,0xc8,0x3e,0x33,0x08,0x3e,0x3e,0x00,0x00,
  0x00,0x00,0x07,0xc3,0xe3,0xe0,0xe0,0x40,0x88,0x3c,0x33,0x08,0x3c,0x1e,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,};


//************************************************************************
//Usage: LCDClear(black);
//Inputs: char color: 8-bit color to be sent to the screen.
//Outputs: None
//Description: This function will clear the screen with "color" by writing the
//				color to each location in the RAM of the LCD.
//************************************************************************
void LCDClear(int color)
{
  color = invertbits(color);
  
  unsigned int i;
  LCDCommand(P_PASET);
  LCDData(0);
  LCDData(131);

  LCDCommand(P_CASET);
  LCDData(0);
  LCDData(131);

  LCDCommand(P_RAMWR);

  for (i=0; i < (132*132)/2; i++)
  {
    LCDData((color >> 4) & 0x00FF);
    LCDData(((color & 0x0F) << 4) | (color >> 8));
    LCDData(color & 0x0FF);		// nop(EPSON)
  }

  //	x_offset = 0;
  //	y_offset = 0;
}


//************************************************************************
void	delay_us(int microSecs)
{
  delayMicroseconds(microSecs);
}

//************************************************************************
void	delay_ms(int millisecs)
{
  //	delay(millisecs);
  //	delayMicroseconds(millisecs * 1000);
}


//************************************************************************
//Usage: LCDCommand(RAMWR);
//Inputs: char data - character command to be sent to the LCD
//Outputs: None
//************************************************************************
void LCDCommand(unsigned char data)
{
  char jj;
  cbi(LCD_PORT_CS, CS);		// enable chip, p0.20 goes low
  //delay_us(1);
  cbi(LCD_PORT_SCK, LCD_SCK);		// send clock pulse
  cbi(LCD_PORT_DIO, DIO);		// output low on data out (9th bit low = command), p0.19
  //delay_us(1);
  delay_us(1);
  sbi(LCD_PORT_SCK, LCD_SCK);
  //delay_us(1);

  for (jj = 0; jj < 8; jj++)
  {
    cbi(LCD_PORT_SCK, LCD_SCK);		// send clock pulse
    if ((data & 0x80) == 0x80)
    {
      sbi(LCD_PORT_DIO, DIO);
      data <<= 1;
    }
    else
    {
      cbi(LCD_PORT_DIO, DIO);
      data <<= 1;
    }
    sbi(LCD_PORT_SCK, LCD_SCK);
    cbi(LCD_PORT_SCK, LCD_SCK);	
  }
  sbi(LCD_PORT_CS, CS);			// disable
}

//************************************************************************
//Usage: LCDData(RAMWR);
//Inputs: char data - character data to be sent to the LCD
//Outputs: None
//************************************************************************
void LCDData(unsigned char data)
{
  char j;

  cbi(LCD_PORT_CS, CS);			// enable chip, p0.20 goes low
  //delay_us(1);
  cbi(LCD_PORT_SCK, LCD_SCK);			// send clock pulse
  sbi(LCD_PORT_DIO, DIO);			// output high on data out (9th bit high = data), p0.19
  sbi(LCD_PORT_SCK, LCD_SCK);			// send clock pulse
  //delay_us(1);
  for (j = 0; j < 8; j++)
  {
    cbi(LCD_PORT_SCK, LCD_SCK);		// send clock pulse
    if ((data & 0x80) == 0x80)
    {
      sbi(LCD_PORT_DIO, DIO);
      data <<= 1;
    }
    else
    {
      cbi(LCD_PORT_DIO, DIO);
      data <<= 1;
    }
    sbi(LCD_PORT_SCK, LCD_SCK);
    cbi(LCD_PORT_SCK, LCD_SCK);
  }
  sbi(LCD_PORT_CS, CS);		// disable
}

//************************************************************************
//Usage: LCDInit();
//Inputs: None
//Outputs: None
//Description:  Initializes the LCD regardless of if the controlller is an EPSON or PHILLIPS.
//************************************************************************
void LCDInit(void)
{
  cbi(LCD_PORT_RES, LCD_RES);		//output_low (LCD_RESET);
  delay_us(200);
  sbi(LCD_PORT_RES, LCD_RES);		//output_high (LCD_RESET);
  delay_us(200);

  LCDCommand(P_SLEEPOUT);	//sleep out(PHILLIPS)
  LCDCommand(P_INVON);	// invert display mode(PHILLIPS)
  LCDCommand(P_COLMOD);	// Set Color Mode(PHILLIPS)
  LCDData(0x03);	
  LCDCommand(P_MADCTL);	//Memory Access Control(PHILLIPS)
  LCDData(0xC0);
  LCDCommand(P_SETCON);	// Set Contrast(PHILLIPS)
  LCDData(0x30);	
  LCDCommand(P_DISPON);	// display on(PHILLIPS)
}


//************************************************************************
//Usage: LCDPrintLogo();
//Inputs: None
//Outputs: None
//Description: Prints the logo_spark array to the LCD.
//************************************************************************
void LCDPrintLogo(void)
{
  int x = 4, y = 25, logo_ix = 0, z;
  char logo;

  for (logo_ix = 0; logo_ix < 1120; logo_ix++)
  {
    logo = logo_spark[logo_ix];
    for (z = 0; z < 8; z++)
    {
      if ((logo & 0x80) == 0x80)
      {
        //	LCDSetPixel(RED, y, x);
        LCDSetPixel(RED, y, x);
      }
      x++;
      if (x == 132)
      {
        x = 4;
        y++;
      }

      logo <<= 1;
    }
  }

}

//************************************************************************
//Usage: LCDSetPixel(white, 0, 0);
//Inputs: unsigned char color - desired color of the pixel
//			unsigned char x - Page address of pixel to be colored
//			unsigned char y - column address of pixel to be colored
//Outputs: None
//Description: Sets the starting page(row) and column (x & y) coordinates in ram,
//				then writes the colour to display memory.	The ending x & y are left
//				maxed out so one can continue sending colour data bytes to the 'open'
//				RAMWR command to fill further memory.	issuing any red command
//				finishes RAMWR.
//**NOTE** Because this function is static, it is essentially a "private" function
//		and can only be used within this file!
//*	Apr  3,	2010	<MLS> Made LCDSetPixel public
//************************************************************************
void LCDSetPixel(int color, unsigned char x, unsigned char y)
{
  //y	=	(COL_HEIGHT - 1) - y;
  //x       =       (ROW_LENGTH - 1) - x;

  color = invertbits(color);

  LCDCommand(P_PASET);	// page start/end ram
  LCDData(x);
  LCDData(ENDPAGE);

  LCDCommand(P_CASET);	// column start/end ram
  LCDData(y);
  LCDData(ENDCOL);

  LCDCommand(P_RAMWR);	// write

  LCDData((unsigned char)((color>>4)&0x00FF));
  LCDData((unsigned char)(((color&0x0F)<<4)|0x00));
}

//**********************************************************************************
// The SOLID Shield is a bit different than the Sparkfun Shield.
// One BIG difference is that the color 'bits' are inverted 1 = 0, 0 = 1.
// This little hack takes care of that (as needed).
int invertbits(int num)
{
  return ~num;  
}



