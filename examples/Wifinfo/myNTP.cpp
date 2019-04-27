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
//		-intégré le code dans la classe myNTP
//		-Ajout mise à l'heure via NTP
//		-Using library Time-master version 1.5
//********************************************************************************
#include <Arduino.h>
#include <Ticker.h>	
#include "Wifinfo.h"
#include "mySyslog.h"
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <Time.h>
#include <TimeLib.h>
#include "Timezone.h"    // https://github.com/JChristensen/Timezone
//#include "Ticker.h"		//erreur bizarre si <Ticker.h> au lieu de "Ticker.h" : Every_1_Sec deja défini dans print.h ?
#include "myNTP.h"

#define NO_DEBUGNTP
TimeChangeRule myDST = { "EDT", Fourth, Sun, Mar, 2, +120 };    //été = UTC + 2 hours
TimeChangeRule mySTD = { "EST", Fourth, Sun, Oct, 2, +60 };     //hiver = UTC + 1 hours
Timezone myTZ(myDST, mySTD);
Ticker Every_1_Sec;

/* ======================================================================
Function: UpdateSysinfo
Purpose : update sysinfo variables
Input   : true if first call
		  true if needed to print on serial debug
Output  : -
Comments: -
====================================================================== */
void myntp::UpdateSysinfo(bool first_call, bool show_debug)
{
	tmElements_t tm;
	this->seconds1970++;
	time_t utc = this->seconds1970; 
	breakTime(this->seconds1970, tm);
	int decalage = 0;
	decalage = myTZ.getOffset(utc);
	this->secM = tm.Second;
	this->minM = tm.Minute;
	this->heureTU = tm.Hour;
	this->dayM = tm.Day;

	this->heureLocale =(this->heureTU +decalage) % 24;

	char  buffer[132];
	sprintf_P(buffer, PSTR("%ld days %02d h %02d m %02d sec"), this->dayM, this->heureLocale, this->minM, this->secM);

	if (this->heureLocale == 0  && this->minM == 0 && this->secM == 0)   //minuit on pourrait ajouter un boolean pour valider le passage au cas ou on rate 1 seconde
	{
		this->minuit = true;
	}
	else if ((this->heureLocale == 12 ) && this->minM == 0 && this->secM == 0)	//midi on pourrait ajouter un boolean pour valider le passage au cas ou on rate 1 seconde
	{
		this->midi = true;
	}
	this->sysinfo.sys_uptime = buffer;
	return ;
}
void myntp::init() {
	WiFi.hostByName(this->ntpServerName1, this->timeServerIP);
	this->refreshTime();
}

void myntp::send() {
	// set all bytes in the buffer to 0
	memset(packetBuffer, 0, NTP_PACKET_SIZE);
	// Initialize values needed to form NTP request
	// (see URL above for details on the packets)
	this->packetBuffer[0] = 0b11100011;   // LI, Version, Mode
	this->packetBuffer[1] = 0;     // Stratum, or type of clock
	this->packetBuffer[2] = 6;     // Polling Interval
	this->packetBuffer[3] = 0xEC;  // Peer Clock Precision
	// 8 bytes of zero for Root Delay & Root Dispersion
	this->packetBuffer[12] = 49;
	this->packetBuffer[13] = 0x4E;
	this->packetBuffer[14] = 49;
	this->packetBuffer[15] = 52;

	// all NTP fields have been given values, now
	// you can send a packet requesting a timestamp:
	//int ret=
	udpNTP.beginPacket(timeServerIP, 123); //NTP requests are to port 123
#ifdef DEBUGNTP
	DebugF("NTP:send:ret de udp.beginPacket(1 ok): "); Debugln(ret);
#endif
	//size_t ret=
	udpNTP.write(packetBuffer, NTP_PACKET_SIZE);

	//ret=
	udpNTP.endPacket();

#ifdef DEBUGNTP
	DebugF("NTP:send:ret de udp.endPacket(1 ok): "); Debugln(ret);
#endif
}

void myntp::setTimezone(float timezone) {
	this->timezone = timezone;
}

time_t myntp::get() {
	return this->get(this->timezone);
}
time_t myntp::get(float timezone) {
#ifdef DEBUGNTP
	char  toprint[20];
	IPAddress ad;
	DebugF("NTP:get:_port: "); Debugln(_port);
	ad = _timeServerIP;
	sprintf(toprint, "%d.%d.%d.%d", ad[0], ad[1], ad[2], ad[3]);
	DebugF("NTP:get:IP address   : "); Debugln(toprint);
#endif	
	while (udpNTP.parsePacket());
	send();
	uint32_t start = millis();
	while (millis() - start < 3000) {  //marc remplace 1500 par 3000 ok
		int ret = udpNTP.parsePacket();
		if (ret >= NTP_PACKET_SIZE) {
			DebuglnF("serveur NTP OK: ");
			udpNTP.read(this->packetBuffer, NTP_PACKET_SIZE);
			unsigned long highWord = word(this->packetBuffer[40], this->packetBuffer[41]);
			unsigned long lowWord = word(this->packetBuffer[42], this->packetBuffer[43]);
			unsigned long secsSince1900 = highWord << 16 | lowWord;
			return secsSince1900 - seventyYears + timezone * SECS_PER_HOUR;
		}
		else
		{
			DebugF("parsePacket: "); Debugln(ret);
		}
		delay(1000);
	}
	DebuglnF("pb sur le serveurs NTP");
	return 0;
}


/* ======================================================================
Function: getSeconds1970
Purpose : return number of second since 01/01/1970
Comments : without NTP return number of second since power on

====================================================================== */

time_t myntp::getSeconds1970() const
{
	return this->seconds1970;
}

/* ======================================================================
Function: refreshTimeIfMidi
Purpose : call NTP for refresh time
Comments : if noom:refresh time and reset boolean midi
====================================================================== */
void myntp::refreshTimeIfMidi()
{
	if (this->midi)
	{
		this->refreshTime();
		this->midi = false;
	}
}

/* ======================================================================
Function: refreshTime
Purpose : call NTP for refresh time
Output  : -if ok:return second since 1970
			else return 0
Comments : for 5 test max if NOK
====================================================================== */
bool myntp::refreshTime()
{
	if (WIFIOK)
	{
		float _timezone = 0; 
		this->begin();
		for (int essai = 1; essai < 5; essai++)
		{
			DebugF("essai NTP:");Debugln(essai);
			time_t memoTime = this->seconds1970;
			this->seconds1970 = get(_timezone);
			if (this->seconds1970)
			{
				DebugF("Correction time:(New-Old");	Debugln(this->seconds1970 - memoTime);
				break;
			}
			delay(1000);
		}
		DebugF("seconds1970:");Debugln(this->seconds1970);
		this->seconds1970 -= 1;   //+1 en entrée de UpdateSysinfo

		this->UpdateSysinfo(false, true);
		this->stop();
		return false;
	}
	return true;
}

myntp::myntp() {
	Every_1_Sec.attach(1, Task_1_Sec);
}
///* ======================================================================
//Function: Task_1_Sec
//Purpose : update our second ticker
//Input   : -
//Output  : -
//Comments: return false by getCycle1Seconde-
//====================================================================== */
volatile boolean task_1_sec = false;
void Task_1_Sec(void)
{
	task_1_sec = true;
}


boolean myntp::getCycle1Seconde(void)
{
	if (task_1_sec)
	{
		task_1_sec = false;
		return true;
	}
	return false;
}




void myntp::begin() {
	udpNTP.begin(port);
}

void myntp::stop() {
	udpNTP.stop();
}
unsigned char myntp::getJour() const
{
	return this->dayM;
}
unsigned char myntp::getHeureLocale() const
{
	return this->heureLocale;
}
unsigned char myntp::getHeureTU() const
{
	return this->heureTU;
}
unsigned char  myntp::getMinutes()const
{
	return this->minM;
}
unsigned char  myntp::getSecondes()const
{
	return  this->secM;
}
bool myntp::getMinuit(void) const
{
	return this->minuit;
}
void myntp::clrMinuit(void) 
{
	this->minuit=false;
}