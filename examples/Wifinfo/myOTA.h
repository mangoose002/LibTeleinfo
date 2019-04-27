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
//		-intégré le code dans la classe myOTA myOTA.h
//
//Using library ArduinoOTA version 1.0 
//
//********************************************************************************
#ifndef MYOTA_H
#define MYOTA_H
#include <Arduino.h>
#include <ArduinoOTA.h>
#include "Wifinfo.h"

class myOTA : public  ArduinoOTAClass
{
private:
	bool ota_blink;
public:
	myOTA(void);
	void configuration(void);
	bool getOta_blink(void);
	void notOta_blink(void);
	void setOta_blink(void);
};

extern myOTA MYOTA;

#endif