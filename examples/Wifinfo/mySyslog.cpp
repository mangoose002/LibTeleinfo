// **********************************************************************************
// ESP8266 Traitement syslog
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
//		-intégré le code dans la classe mySyslog mySyslog.cpp
//********************************************************************************
#include <Arduino.h>
#include "Wifinfo.h"
#include <FS.h>
#include <WiFiUdp.h>
#include <Syslog.h>
#include "config.h"
#include "mySyslog.h"

#ifdef SYSLOG
	WiFiUDP udpClient;
	Syslog syslog(udpClient, SYSLOG_PROTO_IETF);
#endif


/////////////////////////// uniquement si DEBUGSERIAL ou SYSLOG //////////
#ifdef MACRO
////// Versions polymorphes des appels au debugging
////// non liées au port Serial ou Serial1

	void mySyslog::convert(const __FlashStringHelper *ifsh)
	{
	  PGM_P p = reinterpret_cast<PGM_P>(ifsh);
	  this->plog=0;
	  while (1) {
		unsigned char c = pgm_read_byte(p++);
		if (c == 0) {
			this->logbuffer[plog]=0;
		  break;
		}
		this->logbuffer[plog]=c;
		this->plog++;
	  }
	}

#ifdef SYSLOG

void mySyslog::process_line(char *msg) {
	strcat(this->waitbuffer, msg);
	this->pending = strlen(this->waitbuffer);
	if (this->waitbuffer[this->pending - 1] == 0x0D || this->waitbuffer[this->pending - 1] == 0x0A) {
		//Cette ligne est complete : l'envoyer !
		for (uint i = 0; i < this->pending - 1; i++) {
			if (this->waitbuffer[i] <= 0x20)
				this->waitbuffer[i] = 0x20;
		}
		syslog.log(LOG_INFO, this->waitbuffer);
		delay(2 * this->pending);
		memset(this->waitbuffer, 0, 255);
		this->pending = 0;

	}
}
#endif

// Toutes les fonctions aboutissent sur la suivante :
void mySyslog::Myprint(char *msg) {

#ifdef DEBUGSERIAL
	DEBUG_SERIAL.print(msg);
#endif

#ifdef SYSLOG
	//if( SYSLOGusable ) 

	if (WIFIOK)
	{
		process_line(msg);
	}
	else if (this->SYSLOGselected) {
		//syslog non encore disponible
		//stocker les messages à envoyer plus tard
		this->in++;
		if (this->in >= 50) {
			//table saturée !
			this->in = 0;
		}
		if (this->lines[this->in]) {
			//entrée occupée : l'écraser, tant pis !
			free(lines[this->in]);
		}
		lines[this->in] = (char *)malloc(strlen(msg) + 2);
		memset(lines[this->in], 0, strlen(msg + 1));
		strcpy(lines[this->in], msg);
	}
#endif

}

void mySyslog::Myprint() {
	this->logbuffer[0] = 0;
	Myprint(this->logbuffer);
}
void mySyslog::Myprint(String msg) {
	sprintf(this->logbuffer, "%s", msg.c_str());
	Myprint(this->logbuffer);
}
void mySyslog::Myprint(const __FlashStringHelper *msg) {
	convert(msg);
	Myprint(this->logbuffer);
}
void mySyslog::Myprint(long i) {
	sprintf(this->logbuffer, "%ld", i);
	Myprint(this->logbuffer);
}
void mySyslog::Myprint(unsigned int i) {
	sprintf(this->logbuffer, "%d", i);
	Myprint(this->logbuffer);
}
void mySyslog::Myprint(int i) {
	sprintf(this->logbuffer, "%d", i);
	Myprint(this->logbuffer);
}
void mySyslog::Myprintln() {
	sprintf(this->logbuffer, "\n");
	Myprint(this->logbuffer);
}
void mySyslog::Myprintln(unsigned char *msg) {
	sprintf(this->logbuffer, "%s\n", msg);
	Myprint(this->logbuffer);
}
void mySyslog::Myprintln(String msg) {
	sprintf(this->logbuffer, "%s\n", msg.c_str());
	Myprint(this->logbuffer);
}
void mySyslog::Myprintln(const __FlashStringHelper *msg) {
	convert(msg);
	this->logbuffer[plog] = (char)'\n';
	this->logbuffer[plog + 1] = 0;
	Myprint(this->logbuffer);
}
void mySyslog::Myprintln(long i) {
	sprintf((char *)this->logbuffer, "%ld\n", i);
	Myprint(this->logbuffer);
}
void mySyslog::Myprintln(unsigned int i) {
	sprintf(this->logbuffer, "%d\n", i);
	Myprint(this->logbuffer);
}
void mySyslog::Myprintln(int i) {
	sprintf(this->logbuffer, "%d\n", i);
	Myprint(this->logbuffer);
}

void mySyslog::Myflush() {

}

#endif    //MACRO

#ifdef SYSLOG
	void mySyslog::configSyslog(void)
	{
		if (*CONFIGURATION.config.syslog_host) {
			SYSLOGselected = true;
			// Create a new syslog instance with LOG_KERN facility
			syslog.server(CONFIGURATION.config.syslog_host, CONFIGURATION.config.syslog_port);
			syslog.deviceHostname(CONFIGURATION.config.host);
			syslog.appName(APP_NAME);
			syslog.defaultPriority(LOG_KERN);
			memset(waitbuffer, 0, 255);
			pending = 0;
		}
		else {
			// SYSLOGusable=false;
			SYSLOGselected = false;
		}
	}
	void mySyslog::setSYSLOGselected(void)
	{
	SYSLOGselected = true;  //Par défaut, au moins stocker les premiers msg debug
	}

	void mySyslog::clearLinesSyslog(void)
	{
	for (int i = 0; i < 50; i++)
		lines[i] = 0;
	in = -1;
	out = -1;
	}

void mySyslog::sendSyslog(void)
{
	if (SYSLOGselected) {
		if (in != out && in != -1) {
			//Il y a des messages en attente d'envoi
			out++;
			while (out <= in) {
				process_line(lines[out]);
				free(lines[out]);
				lines[out] = 0;
				out++;
			}
			DebuglnF("syslog buffer empty");
		}
	}
	else {
		DebuglnF("syslog not activated !");
	}
}
#endif
