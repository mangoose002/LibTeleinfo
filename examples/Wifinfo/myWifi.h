// **********************************************************************************
// ESP8266 Traitement wifi
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
//		-intégré le code dans la classe myWifi myWifi.h
//
//Using library ESP8266WiFi version 1.0
//Using library ESP8266mDNS version 0.0.0
//
//********************************************************************************

#ifndef myWifi_h
#define myWifi_h

#include <Arduino.h>
#include "Wifinfo.h"


class myWifi
{
public:
	bool WiFiOn(void);
	void WiFiOff(void);
	void WIFI_printStatus(void);
	int WifiHandleConn(boolean setup);
	void testWifi(void);
	int getNb_reconnect(void) const;
	bool getWifi(void);
private:
	void on(void);
	void off(void);
	bool wifi = false;
	int nb_reconnect = 0;
};
extern myWifi WIFI;
#endif
