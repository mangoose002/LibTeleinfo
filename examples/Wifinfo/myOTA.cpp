// **********************************************************************************
// ESP8266 traitement OTA
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
// Modifié par Dominique DAMBRAIN 2017-07-10 (http://www.dambrain.fr)
//
// Modifié par marc PRIEUR 2019-03-21 ()
//		-intégré le code dans la classe myOTA myOTA.cpp
//
//Using library ArduinoOTA version 1.0 
//
//********************************************************************************
#include <Arduino.h>
#include <ArduinoOTA.h>
#include <functional>

#include "Wifinfo.h"
#include "mySyslog.h"
#include "config.h"
#include "leds.h"
#include "myNTP.h"
#include "myOTA.h"

myOTA::myOTA() : ArduinoOTAClass()
{
	  this->onStart([]() { 
	
		LESLEDS.LedRGBON(COLOR_MAGENTA);
	
	    DebuglnF("Update Started");
		MYOTA.setOta_blink();
	  });

	this->onEnd([]() {

	  LESLEDS.LedRGBOFF();

    DebuglnF("Update finished : restarting");
  });

	this->onProgress([](unsigned int progress, unsigned int total) {

	  if (MYOTA.getOta_blink()) {
		  LESLEDS.LedRGBON(COLOR_MAGENTA);
	  }
	  else {
		  LESLEDS.LedRGBOFF();
	  }


	  MYOTA.notOta_blink();
	  char  buffer[132];
	sprintf(buffer, "Progress: %u%%\n", (progress / (total / 100)));
	Debugln(buffer);
    //Serial.printf("Progress: %u%%\n", (progress / (total / 100)));
  });

	this->onError([](ota_error_t error) {

	LESLEDS.LedRGBON(COLOR_RED);

#ifdef DEBUGSERIAL
	char  buffer[132];
    sprintf(buffer,"Update Error[%u]: ", error);
    Debugln(buffer);
    if (error == OTA_AUTH_ERROR) DebuglnF("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) DebuglnF("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) DebuglnF("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) DebuglnF("Receive Failed");
    else if (error == OTA_END_ERROR) DebuglnF("End Failed");
	delay(2000);
#endif
    ESP.restart(); 
  });
}


void myOTA::configuration(void)
{
	this->setPort(CONFIGURATION.config.ota_port);
	this->setHostname(CONFIGURATION.config.host);
	this->setPassword(CONFIGURATION.config.ota_auth);
	this->begin();
	// just in case your sketch sucks, keep update OTA Available
// Trust me, when coding and testing it happens, this could save
// the need to connect FTDI to reflash
// Usefull just after 1st connexion when called from setup() before
// launching potentially buggy main()
	for (uint8_t i = 0; i <= 10; i++) {

		LESLEDS.LedRGBON(COLOR_MAGENTA);

		delay(100);

		LESLEDS.LedRGBOFF();

		delay(200);
		this->handle();
	}
}

bool myOTA::getOta_blink(void)
{
	return ota_blink;
}
void myOTA::setOta_blink(void)
{
	LESLEDS.LedRGBON(COLOR_MAGENTA);
	ota_blink = true;
}
void myOTA::notOta_blink(void)
{
	ota_blink = !ota_blink;
}

