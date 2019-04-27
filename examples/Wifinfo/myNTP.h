// **********************************************************************************
// ESP8266 Traitement du temps
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
//		-intégré le code dans la classe myNTP myNTP.h
//		-Ajout mise à l'heure via NTP
//		-Using library Time-master version 1.5
//********************************************************************************


//https://github.com/ArduinoHannover/NTP/blob/master/NTP.cpp

#ifndef MYNTP_H
#define MYNTP_H

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include "Wifinfo.h" 

#ifndef byte
#define byte char
#endif

typedef struct
{
	String sys_uptime;
} _sysinfo;

void Task_1_Sec(void);

class myntp {
	private:
		const unsigned long seventyYears = 2208988800UL;
		const int NTP_PACKET_SIZE = 48;
		byte packetBuffer[48];
		IPAddress timeServerIP;
		uint32_t port = 2390;
		float timezone;
		WiFiUDP udpNTP;
		void send(void);
		unsigned char heureLocale = 0;
		unsigned char heureTU = 0;
		bool midi = false;
		bool minuit = false;
		const char* ntpServerName1 = "3.fr.pool.ntp.org";
		unsigned char secM;
		unsigned char minM;
		//byte hrM;
		unsigned char dayM;
		time_t seconds1970 = 0;
		bool refreshTime(void);
		void setTimezone(float);
		time_t get(void);
		time_t get(float);
	public:
		time_t getSeconds1970() const;
		myntp(void);
		void begin(void);
		void stop(void);
		void init(void);
		void refreshTimeIfMidi(void);
		bool getMinuit(void) const;
		void clrMinuit(void) ;
		unsigned char  getJour(void) const;
		unsigned char  getHeureTU(void) const;
		unsigned char  getHeureLocale(void) const;
		unsigned char  getMinutes(void)const;
		unsigned char  getSecondes(void)const;
		void UpdateSysinfo(bool first_call, bool show_debug);
		_sysinfo sysinfo;
		boolean getCycle1Seconde(void);
};
extern myntp NTP;
#endif
