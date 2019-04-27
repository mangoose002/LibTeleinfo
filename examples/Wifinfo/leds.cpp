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

#include <Ticker.h>
#include <NeoPixelBus.h>
#include <Arduino.h>
#include "Wifinfo.h"
#include "mySyslog.h"
#include "LibTeleinfo.h"
#include "config.h"

#include "leds.h"

#ifdef RGB_LED_PIN
NeoPixelBus<NeoRgbFeature, NeoEsp8266BitBang800KbpsMethod> rgb_led(1, RGB_LED_PIN);
#endif


lesLeds::lesLeds()
{
}
/* ======================================================================
Function: LedOff
Purpose : callback called after led blink delay
Input   : led (defined in term of PIN)
Output  : -
Comments: -
====================================================================== */
void LedOff(int led)
{
#ifdef BLU_LED_PIN
	if (led == BLU_LED_PIN)
		LedBluOFF();
#endif
	if (led == RED_LED_PIN)
		LedRedOFF();
	if (led == RGB_LED_PIN)
	{
		if (CONFIGURATION.config.config & CFG_RGB_LED) {
			rgb_led.SetPixelColor(0, RgbColor(0));
			rgb_led.Show();
		}
	}
}


// Light off the RGB LED
#ifdef RGB_LED_PIN
/* ======================================================================
Function: LedRGBON
Purpose : Light RGB Led with HSB value
Input   : Hue (0..255)
		  Saturation (0..255)
		  Brightness (0..255)
Output  : -
Comments:
====================================================================== */
void lesLeds::LedRGBON(uint16_t hue)
{
	if (CONFIGURATION.config.config & CFG_RGB_LED) {
		// Convert to neoPixel API values
		// H (is color from 0..360) should be between 0.0 and 1.0
		// L (is brightness from 0..100) should be between 0.0 and 0.5
		RgbColor target = HslColor(hue / 360.0f, 1.0f, rgb_brightness * 0.005f);

		// Set RGB Led
		rgb_led.SetPixelColor(0, target);
		rgb_led.Show();
	}
}

/* ======================================================================
Function: LedRGBOFF
Purpose : light off the RGN LED
Input   : -
Output  : -
Comments: -
====================================================================== */
void lesLeds::LedRGBOFF(void)
{
	if (CONFIGURATION.config.config & CFG_RGB_LED) {
		rgb_led.SetPixelColor(0, RgbColor(0));
		rgb_led.Show();
	}
}

#endif

void lesLeds::tempoLedOff()
{
	//rgb_ticker.once_ms((unsigned int)BLINK_LED_MS, LedOff,(LES_LEDS_EN_SERIE)PIN_LED_RGB);
	rgb_ticker.once_ms(   (uint32_t)BLINK_LED_MS,LedOff, (int)RGB_LED_PIN);
}

void lesLeds::InitLed(void )
{
	rgb_led.Begin();
}
