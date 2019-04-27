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
//		-intégré le code dans la classe mySyslog mySyslog.h
//********************************************************************************

#ifndef MYSYSLOG_H
#define MYSYSLOG_H
#include <Arduino.h>
#include <FS.h>
#include "Wifinfo.h"


#ifdef MACRO
class mySyslog 
{
public:
	void configSyslog(void);
	void setSYSLOGselected(void);
	void clearLinesSyslog(void);
	void sendSyslog(void);
	void Myprint(char *msg);
	void Myprint(void);
	void Myprint(String msg);
	void Myprint(const __FlashStringHelper *msg);
	void Myprint(unsigned int i);
	void Myprintln(void);
	void Myprintln(long);
	void Myprintln(int);
	void Myprint(long i);
	void Myprint(int i);
	void Myprintln(unsigned char *msg);
	void Myprintln(String msg);
	void Myprintln(const __FlashStringHelper *msg);
	void Myprintln(unsigned int i);
	void Myflush(void);
	void convert(const __FlashStringHelper *ifsh);
	void process_line(char *msg);
private:

#ifdef MACRO
	//// Versions polymorphes des appels au debugging
	//// non liées au port Serial ou Serial1
	char logbuffer[255];

#ifdef SYSLOG
	volatile boolean SYSLOGselected = false;
	char waitbuffer[255];
	char *lines[50];
	int in = -1;
	int out = -1;
	unsigned int pending = 0;
#endif

	int plog = 0;
	FS* _fs;    //Pointeur objet File System
#endif

};

#define Debug(x)    MYSYSLOG.Myprint(x)
#define Debugln(x)   MYSYSLOG.Myprintln(x)
#define DebugF(x)    MYSYSLOG.Myprint(F(x))
#define DebuglnF(x)  MYSYSLOG.Myprintln(F(x))
#define Debugflush()   MYSYSLOG.Myflush()
#else
#define Debug(x)    {}
#define Debugln(x)  {}
#define DebugF(x)   {}
#define DebuglnF(x) {}
#define Debugf(...) {}
#define Debugflush()  {}
#endif
#ifdef SYSLOG
extern mySyslog MYSYSLOG;
#endif
#endif