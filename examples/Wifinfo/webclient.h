// **********************************************************************************
// ESP8266 Teleinfo WEB Client, routine include file
// **********************************************************************************
// Creative Commons Attrib Share-Alike License
// You are free to use/extend this library but please abide with the CC-BY-SA license:
// Attribution-NonCommercial-ShareAlike 4.0 International License
// http://creativecommons.org/licenses/by-nc-sa/4.0/
//
// For any explanation about teleinfo ou use, see my blog
// http://hallard.me/category/tinfo
//
// This program works with the Wifinfo board
// see schematic here https://github.com/hallard/teleinfo/tree/master/Wifinfo
//
// Written by Charles-Henri Hallard (http://hallard.me)
//
// History : V1.00 2015-12-04 - First release
//
// All text above must be included in any redistribution.
//
// Modifi� par marc Prieur 2019
//		-int�gr� le code dans la classe webClient webClient.cpp  webClient.h
//
// Using library ESP8266HTTPClient version 1.1
//
// **********************************************************************************

#ifndef WEBCLIENT_H
#define WEBCLIENT_H

#include <Arduino.h> 
#include "Wifinfo.h"

#define WC_TABNAME_SIZE	105

class webClient
{
public:

	boolean emoncmsPost(void);
	boolean jeedomPost(void);
	boolean httpRequest(void);
	//boolean UPD_switch(void);
	String  build_emoncms_json(void);
	bool validate_value_name(String name);

	
private:
	boolean httpPost(char * host, uint16_t port, char * url);
	//List of authorized value names in Teleinfo, to detect polluted entries
	const String tabnames[WC_TABNAME_SIZE] = {
		"ADCO" , "OPTARIF" , "ISOUSC" , "BASE", "HCHC" , "HCHP",
 		"IMAX" , "IINST" , "PTEC", "PMAX", "PAPP", "HHPHC" , "MOTDETAT" , "PPOT",
 		"IINST1" , "IINST2" , "IINST3", "IMAX1" , "IMAX2" , "IMAX3" ,
		"EJPHN" , "EJPHPM" , "BBRHCJB" , "BBRHPJB", "BBRHCJW" , "BBRHPJW" , "BBRHCJR" ,
		"BBRHPJR" , "PEJP" , "DEMAIN" , "ADPS" , "ADIR1", "ADIR2" , "ADIR3",
		//FOR STANDARD TYPE
		"ADSC", "VTIC", "DATE", "NGTF", "LTARF", "EAST","EASF01","EASF02","EASF03","EASF04","EASF05",
		"EASF06", "EASF07", "EASF08", "EASF09", "EASF10", "EASD01", "EASD02", "EASD03", "EASD04", "EAIT",
		"ERQ1", "ERQ2", "ERQ3", "ERQ4", "IRMS1", "IRMS2", "IRMS3", "URMS1", "URMS2", "URMS3",
		"PREF","PCOUP", "SINSTS","SINSTS1","SINSTS2","SINSTS3","SMAXSN","SMAXSN1","SMAXSN2","SMAXSN3",
		"SMAXSN-1","SMAXSN1-1","SMAXSN2-1","SMAXSN3-1","SINSTI","SMAXIN","SMAXIN-1","CCASN","CCASN-1",
		"CCAIN","CCAIN-1","UMOY1","UMOY2","UMOY3","STGE","DPM1","FPM1","DPM2","FPM2","DPM3","FPM3",
		"MSG1","MSG2","PRM","RELAIS","NTARF","NJOURF","NJOURF+1","PJOURF+1","PPOINTE"
	};
};

extern webClient WEBCLIENT;
#endif
