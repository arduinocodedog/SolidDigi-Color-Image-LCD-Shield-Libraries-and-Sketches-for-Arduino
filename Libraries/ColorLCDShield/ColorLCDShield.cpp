/*
  LCDShield.cpp - Arduino Library to control a Nokia 6100 LCD, 
  specifically that found on SparkFun's Color LCD Shield.
  This code should work for both Epson and Phillips display drivers 
  normally found on the Color LCD Shield.
	
  License: CC BY-SA 3.0: Creative Commons Share-alike 3.0. Feel free 
  to use and abuse this code however you'd like. If you find it useful
  please attribute, and SHARE-ALIKE!
  
  This is based on code by Mark Sproul, and Peter Davenport.
  Thanks to Coleman Sellers and Harold Timmis for help getting it to work with the Phillips Driver 7-31-2011
*/

#include "ColorLCDShield.h"

//extern "C" {
	//#include "wiring.h"
//}

LCDShield::LCDShield()
{

#if defined(__AVR_ATmega2560__) || defined(__AVR_ATmega1280__)
	DDRB = ((1<<DIO)|(1<<LCD_SCK));     //Set DIO and SCK pins on PORTB as outputs
	DDRH = ((1<<CS)|(1<<LCD_RES));  //Set CS and RES pins PORTH as outputs
#else
	DDRB = ((1<<CS)|(1<<DIO)|(1<<LCD_SCK)|(1<<LCD_RES));  //Set the control pins as outputs
#endif

	DDRD	=	0x00;
	PORTD	=	0xFF;
}

void LCDShield::LCDCommand(unsigned char data)
{
	char jj;
	cbi(LCD_PORT_CS, CS);		// enable chip, p0.20 goes low
	//delay_us(1);
    cbi(LCD_PORT_SCK, LCD_SCK);	// send clock pulse
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

void LCDShield::LCDData(unsigned char data)
{
	char j;

	cbi(LCD_PORT_CS, CS);			// enable chip, p0.20 goes low
	//delay_us(1);
    cbi(LCD_PORT_SCK, LCD_SCK);		// send clock pulse
	sbi(LCD_PORT_DIO, DIO);			// output high on data out (9th bit high = data), p0.19
	sbi(LCD_PORT_SCK, LCD_SCK);		// send clock pulse
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

void LCDShield::init()
{
	cbi(LCD_PORT_RES, LCD_RES);		//output_low (LCD_RESET);
	delay_us(200);
	sbi(LCD_PORT_RES, LCD_RES);		//output_high (LCD_RESET);
	delay_us(200);
	
	LCDCommand(SLEEPOUT);	//sleep out(PHILLIPS)
	LCDCommand(INVON);		// invert display mode(PHILLIPS)
    LCDCommand(COLMOD);		// Set Color Mode(PHILLIPS)
	LCDData(0x03);	
	LCDCommand(MADCTL);		//Memory Access Control(PHILLIPS)
	LCDData(0xC0);
	LCDCommand(SETCON);		// Set Contrast(PHILLIPS)
	LCDData(0x30);	
	LCDCommand(DISPON);		// display on(PHILLIPS)
}

void LCDShield::clear(int color)
{
	color = ~color;	// for SOLID LCD
	
	LCDCommand(PASETP);
	LCDData(0);
	LCDData(131);

	LCDCommand(CASETP);
	LCDData(0);
	LCDData(131);

	LCDCommand(RAMWRP);

	for(unsigned int i=0; i <(132*132)/2; i++)
	{
		LCDData((color>>4)&0x00FF);
		LCDData(((color&0x0F)<<4)|(color>>8));
		LCDData(color&0x0FF);
	}

	x_offset = 0;
	y_offset = 0;
}

void LCDShield::contrast(char setting)
{
	LCDCommand(SETCON);      // electronic volume, this is the contrast/brightness(EPSON)
	LCDData(setting);        // volume (contrast) setting - course adjustment,  -- original was 24

	LCDCommand(NOP);         // nop(EPSON)
}

void LCDShield::setPixel(int color, unsigned char x, unsigned char y)
{
	//y = (COL_HEIGHT - 1) - y;
	//x = (ROW_LENGTH - 1) - x;
	
	color = ~color;  // for SOLID LCD

	LCDCommand(PASETP); // page start/end ram
	LCDData(x);
	LCDData(x);

	LCDCommand(CASETP); // column start/end ram
	LCDData(y);
	LCDData(y);

	LCDCommand(RAMWRP); // write

	LCDData((unsigned char)((color>>4)&0x00FF));
	LCDData((unsigned char)(((color&0x0F)<<4)|0x00));
}

void LCDShield::setCircle (int x0, int y0, int radius, int color)
{
	int f = 1 - radius;
	int ddF_x = 0;
	int ddF_y = -2 * radius;
	int x = 0;
	int y = radius;

	setPixel(color, x0, y0 + radius);
	setPixel(color, x0, y0 - radius);
	setPixel(color, x0 + radius, y0);
	setPixel(color, x0 - radius, y0);

	while(x < y)
	{
		if(f >= 0)
		{
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x + 1;

		setPixel(color, x0 + x, y0 + y);
		setPixel(color, x0 - x, y0 + y);
		setPixel(color, x0 + x, y0 - y);
		setPixel(color, x0 - x, y0 - y);
		setPixel(color, x0 + y, y0 + x);
		setPixel(color, x0 - y, y0 + x);
		setPixel(color, x0 + y, y0 - x);
		setPixel(color, x0 - y, y0 - x);
	}
}

void LCDShield::setChar(char c, int x, int y, int fColor, int bColor)
{
	// y	=	(COL_HEIGHT - 1) - y; // make display "right" side up
	// x	=	(ROW_LENGTH - 2) - x;
	
	fColor = ~fColor;
	bColor = ~bColor;

	int             i,j;
	unsigned int    nCols;
	unsigned int    nRows;
	unsigned int    nBytes;
	unsigned char   PixelRow;
	unsigned char   Mask;
	unsigned int    Word0;
	unsigned int    Word1;
	unsigned char   *pFont;
	unsigned char   *pChar;

	// get pointer to the beginning of the selected font table
	pFont = (unsigned char *)FONT8x16;
	// get the nColumns, nRows and nBytes
	nCols = *pFont;
	nRows = *(pFont + 1);
	nBytes = *(pFont + 2);
	// get pointer to the last byte of the desired character
	pChar = pFont + (nBytes * (c - 0x1F)) + nBytes - 1;
	
	// Row address set (command 0x2B)
	LCDCommand(PASETP);
	LCDData(x);
	LCDData(x + nRows - 1);
	// Column address set (command 0x2A)
	LCDCommand(CASETP);
	LCDData(y);
	LCDData(y + nCols - 1);

	// WRITE MEMORY
	LCDCommand(RAMWRP);
	// loop on each row, working backwards from the bottom to the top
	//pChar+=nBytes-1;  // stick pChar at the end of the row - gonna reverse print on phillips
	for (i = nRows - 1; i >= 0; i--) {
		// copy pixel row from font table and then decrement row
		PixelRow = *pChar++;
		// loop on each pixel in the row (left to right)
		// Note: we do two pixels each loop
		Mask = 0x80;  // <- opposite of epson
		for (j = 0; j < nCols; j += 2) 
		{
			// if pixel bit set, use foreground color; else use the background color
			// now get the pixel color for two successive pixels
			if ((PixelRow & Mask) == 0)
				Word0 = bColor;
			else
				Word0 = fColor;
			Mask = Mask >> 1; // <- opposite of epson
			if ((PixelRow & Mask) == 0)
				Word1 = bColor;
			else
				Word1 = fColor;
			Mask = Mask >> 1; // <- opposite of epson
			// use this information to output three data bytes
			LCDData((Word0 >> 4) & 0xFF);
			LCDData(((Word0 & 0xF) << 4) | ((Word1 >> 8) & 0xF));
			LCDData(Word1 & 0xFF);
		}
	}
}

void LCDShield::setStr(char *pString, int x, int y, int fColor, int bColor)
{
	//x = x + 16;
	//y = y + 8;
	// loop until null-terminator is seen
	while (*pString != 0x00) {
		// draw the character
		setChar(*pString++, x, y, fColor, bColor);
		// advance the y position
		y = y + 8;
		// bail out if y exceeds 131
		if (y > 131) break;
	}
}

void LCDShield::setLine(int x0, int y0, int x1, int y1, int color)
{
	int dy = y1 - y0; // Difference between y0 and y1
	int dx = x1 - x0; // Difference between x0 and x1
	int stepx, stepy;

	if (dy < 0)
	{
		dy = -dy;
		stepy = -1;
	}
	else
		stepy = 1;

	if (dx < 0)
	{
		dx = -dx;
		stepx = -1;
	}
	else
		stepx = 1;

	dy <<= 1; // dy is now 2*dy
	dx <<= 1; // dx is now 2*dx
	setPixel(color, x0, y0);

	if (dx > dy) 
	{
		int fraction = dy - (dx >> 1);
		while (x0 != x1)
		{
			if (fraction >= 0)
			{
				y0 += stepy;
				fraction -= dx;
			}
			x0 += stepx;
			fraction += dy;
			setPixel(color, x0, y0);
		}
	}
	else
	{
		int fraction = dx - (dy >> 1);
		while (y0 != y1)
		{
			if (fraction >= 0)
			{
				x0 += stepx;
				fraction -= dy;
			}
		y0 += stepy;
		fraction += dx;
		setPixel(color, x0, y0);
		}
	}
}

void LCDShield::setRect(int x0, int y0, int x1, int y1, unsigned char fill, int color)
{
	// check if the rectangle is to be filled
	if (fill == 1)
	{
		int xDiff;
	
		if(x0 > x1)
			xDiff = x0 - x1; //Find the difference between the x vars
		else
			xDiff = x1 - x0;
	
		while(xDiff > 0)
		{
			setLine(x0, y0, x0, y1, color);
		
			if(x0 > x1)
				x0--;
			else
				x0++;
		
			xDiff--;
		}

	}
	else 
	{
		// best way to draw an unfilled rectangle is to draw four lines
		setLine(x0, y0, x1, y0, color);
		setLine(x0, y1, x1, y1, color);
		setLine(x0, y0, x0, y1, color);
		setLine(x1, y0, x1, y1, color);
	}
}

void LCDShield::printLogo(void)
{
	int x = 4, y = 25, logo_ix = 0, z;
	char logo;

	for (logo_ix = 0; logo_ix < 1120; logo_ix++)
	{
		logo = logo_spark[logo_ix];
		for (z = 0; z < 8; z++)
		{
			if ((logo & 0x80) == 0x80) setPixel(RED, y, x);
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

void LCDShield::off(void)
{
	LCDCommand(DISPOFF);
}

void LCDShield::on(void)
{
	LCDCommand(DISPON);
}

void LCDShield::delay_us(int microSecs)
{
	delayMicroseconds(microSecs);
}