//************************************************************************
//					Nokia Shield
//************************************************************************
//*	Edit History
//*		<MLS>	= Mark Sproul msproul -at- jove.rutgers.edu
//************************************************************************
//*	Apr  2,	2010	<MLS> I received my Color LCD Shield sku: LCD-09363 from sparkfun
//*	Apr  2,	2010	<MLS> The code was written for WinAVR, I modified it to compile under Arduino
//*	Apr  3,	2010	<MLS> Changed LCDSetPixel to make it "RIGHT" side up
//************************************************************************
//NOTE:In addition to red, green, blue,the rest of the color is not correct, your adjusted according to PCF8833

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>


#include"Arduino.h"
#include"HardwareSerial.h"


//************************************************************************
//					External Component Libs
//************************************************************************
#include "LCD_driver.h"
#include "nokia_tester.h"
#define	kSwitch1_PIN	3
#define	kSwitch2_PIN	4
#define	kSwitch3_PIN	5
unsigned char count;
//************************************************************************
//					Main Code
//************************************************************************
void	setup()
{
int	frameCounter;
unsigned long	startTime;

	Serial.begin(9600);
	Serial.println("Initializing");
	ioinit();
	Serial.println("ioinit. Done");
	LCDInit();			//Initialize the LCD
	Serial.println("LCDInit. Done");
	LCDClear(WHITE);
	Serial.print("Logo...");
	LCDPrintLogo(BLACK);
}

//************************************************************************
//					Loop
//************************************************************************
void	loop()
{
int	s1, s2, s3;

	s1=!digitalRead(kSwitch1_PIN);
	s2=!digitalRead(kSwitch2_PIN);
	s3=!digitalRead(kSwitch3_PIN);

	if (s1)
	{
            if(count == 0)
            {
		LCDClear(RED);
            }
            if(count == 1)
            {
		LCDClear(BLUE);
            }
            if(count == 2)
            {
		LCDClear(GREEN);
            }
            count++;
            if(count == 3)
            {
                count = 0;
            }
		LCDPrintLogo(BLACK);
	}
	else 
	{
		if (s2)
		{
			LCDClear(BLACK);
				if(count == 0)
				{
			LCDPrintLogo(RED);
				}
				if(count == 1)
				{
			LCDPrintLogo(BLUE);
				}
				if(count == 2)
				{
			LCDPrintLogo(GREEN);
				}
				count++;
				if(count == 3)
				{
					count = 0;
				}
		}
		else 
		{
			if (s3)
			{
					LCDClear(WHITE);

				LCDPrintLogo(BLACK);
			}
		}
	}
	delay(200);
}

//************************************************************************
void ioinit(void)
{
	//*	setup the switches for input
	pinMode(kSwitch1_PIN, INPUT);
	pinMode(kSwitch2_PIN, INPUT);
	pinMode(kSwitch3_PIN, INPUT);

	//*	set the pull up resisters
	digitalWrite(kSwitch1_PIN, HIGH);
	digitalWrite(kSwitch2_PIN, HIGH);
	digitalWrite(kSwitch3_PIN, HIGH);


	//*	do the LCD control lines
	pinMode(CS_PIN,	OUTPUT);
	pinMode(DIO_PIN,OUTPUT);
	pinMode(SCK_PIN,OUTPUT);
	pinMode(LCD_RES_PIN,OUTPUT);
}



//************************************************************************
//General short delays
void delay_ms(int x)
{
	for (; x > 0 ; x--)
		delay_us(1000);
}

//************************************************************************
//General short delays
void delay_us(int x)
{	
	TIFR2	=	(1<<TOV2); //Clear any interrupt flags on Timer2
	TCNT2= 240; //Setting counter to 239 will make it 16 ticks until TOV is set. .0625uS per click means 1 uS until TOV is set
	while(x>0)
	{
		while( (TIFR2 & (1<<TOV2)) == 0);
		TIFR2	=	(1<<TOV2); //Clear any interrupt flags on Timer2
		TCNT2=240;
		x--;
	}
}

