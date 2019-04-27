// **********************************************************************************
// Traitement des leds
// **********************************************************************************
// Creative Commons Attrib Share-Alike License
// You are free to use/extend this library but please abide with the CC-BY-SA license:
// Attribution-NonCommercial-ShareAlike 4.0 International License
// http://creativecommons.org/licenses/by-nc-sa/4.0/
//
// For any explanation about teleinfo ou use , see my blog
// http://hallard.me/category/tinfo
//
// This program works with the Wifinfo board
// see schematic here https://github.com/hallard/teleinfo/tree/master/Wifinfo
//
// Written by Charles-Henri Hallard (http://hallard.me)
//
// History : V1.00 2015-06-14 - First release
//
// All text above must be included in any redistribution.
//
// Modifié par marc Prieur 04/2019
//
//Using library NeoPixelBus-master version 2.3.4
//
// **********************************************************************************

#ifndef LEDSRGBSERIAL_H
#define LEDSRGBSERIAL_H

#include <Arduino.h>
#include "Wifinfo.h"

// value for HSL color
// see http://www.workwithcolor.com/blue-color-hue-range-01.htm
#define COLOR_RED             0
#define COLOR_ORANGE         30
#define COLOR_ORANGE_YELLOW  45
#define COLOR_YELLOW         60
#define COLOR_YELLOW_GREEN   90
#define COLOR_GREEN         120
#define COLOR_GREEN_CYAN    165
#define COLOR_CYAN          180
#define COLOR_CYAN_BLUE     210
#define COLOR_BLUE          240
#define COLOR_BLUE_MAGENTA  275
#define COLOR_MAGENTA	    300
#define COLOR_PINK		    350
#define rgb_brightness       1        //50				//50 trop   25 trop 10 trop 0 eteint
#define BLINK_LED_MS   50 // 50 ms blink



#ifdef BLU_LED_PIN
#define LedBluON()  {digitalWrite(BLU_LED_PIN, 0);}
#define LedBluOFF() {digitalWrite(BLU_LED_PIN, 1);}
#else
#define LedBluON()  {}
#define LedBluOFF() {}
#endif
// GPIO 12 red led
#define LedRedON()  {digitalWrite(RED_LED_PIN, 1);}
#define LedRedOFF() {digitalWrite(RED_LED_PIN, 0);}

#define BLINK_LED_MS   50 // 50 ms blink
#define RGB_LED_PIN    14 
#define RED_LED_PIN    12

void LedOff(int led); 

struct etatLeds {
	unsigned char aujourdhui : 2;
	unsigned char demain : 2;
	unsigned char chauffage : 1;
	unsigned char cumulus : 1;
	unsigned char present : 1;
	unsigned char jourNuit : 1;
};

union leds {
	struct etatLeds etatCourant;
	unsigned char etat;
};
class lesLeds
{
public:
	lesLeds();
	void tempoLedOff(void);
	void InitLed(void);
	void LedRGBON(uint16_t hue);
	void LedRGBOFF(void);
private:
	Ticker rgb_ticker;
};
extern lesLeds LESLEDS;

#endif

