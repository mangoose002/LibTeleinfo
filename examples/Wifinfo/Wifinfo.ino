// **********************************************************************************
// ESP8266 Teleinfo WEB Server
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
//       Version 1.0.5
//       Librairie LibTeleInfo : Allocation statique d'un tableau de stockage 
//           des variables (50 entrées) afin de proscrire les malloc/free
//           pour éviter les altérations des noms & valeurs
//       Modification en conséquence des séquences de scanning du tableau
//       ATTENTION : Nécessite probablement un ESP-8266 type Wemos D1,
//        car les variables globales occupent 42.284 octets
//
//       Version 1.0.5a (11/01/2018)
//       Permettre la mise à jour OTA à partir de fichiers .ino.bin (Auduino IDE 1.8.3)
//       Ajout de la gestion d'un switch (Contact sec) relié à GND et D5 (GPIO-14)
//          Décommenter le #define SENSOR dans Wifinfo.h
//          Pour être utilisable avec Domoticz, au moins l'URL du serveur et le port
//          doivent être renseignés dans la configuration HTTP Request, ainsi que 
//          l'index du switch (déclaré dans Domoticz)
//          L'état du switch (On/Off) est envoyé à Domoticz au boot, et à chaque
//            changement d'état
//       Note : Nécessité de flasher le SPIFFS pour pouvoir configurer l'IDX du switch
//              et flasher le sketch winfinfo.ino.bin via interface Web
//       Rendre possible la compilation si define SENSOR en commentaire
//              et DEFINE_DEBUG en commentaire (aucun debug, version Production...)
//
//       Version 1.0.6 (04/02/2018) Branche 'syslog' du github
//		      Ajout de la fonctionnalité 'Remote Syslog'
//		        Pour utiliser un serveur du réseau comme collecteur des messages Debug
//            Note : Nécessité de flasher le SPIFFS pour pouvoir configurer le remote syslog
//          Affichage des options de compilation sélectionnées dans l'onglet 'Système'
//            et au début du Debug + syslog éventuels
// **********************************************************************************
// Modifié par marc PRIEUR 2019-03-21
//###################################includes##################################  

//version wifinfo syslog d'origine:352 392 bytes

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <EEPROM.h>

#include "Wifinfo.h"
#include "myNTP.h"
#include "mySyslog.h"
#include "myOTA.h"


#include "simuTempo.h"
#include "LibTeleinfo.h"

#include "leds.h"
#include "webServer.h"
#include "webclient.h"
#include "config.h"
#include "myWifi.h"

//###################################création des objets##################################  

TInfo TINFO;

lesLeds LESLEDS;
#ifdef SIMUTRAMETEMPO
	SimuTempo SIMU_TEMPO;
#endif 

Ticker Tick_emoncms;
Ticker Tick_jeedom;
Ticker Tick_httpRequest;

myntp NTP; 
 
myOTA MYOTA;
webServer WEBSERVER;
configuration CONFIGURATION;
#ifdef SYSLOG
mySyslog MYSYSLOG;
#endif
myWifi WIFI;

webClient WEBCLIENT;

/* ======================================================================
Function: ADPSCallback
Purpose : called by library when we detected a ADPS on any phased
Input   : phase number
			0 for ADPS (monophase)
			1 for ADIR1 triphase
			2 for ADIR2 triphase
			3 for ADIR3 triphase
Output  : -
Comments: should have been initialised in the main sketch with a
		  tinfo.attachADPSCallback(ADPSCallback())
====================================================================== */
void ADPSCallback(uint8_t phase)
{
	// Monophasé
	if (phase == 0) {
		Debugln(F("ADPS"));
	}
	else {
		Debug(F("ADPS Phase "));
		Debugln('0' + phase);
	}
}

/* ======================================================================
Function: NewFrame
Purpose : callback when we received a complete teleinfo frame
Input   : linked list pointer on the concerned data
Output  : -
Comments: it's called only if one data in the frame is different than
		  the previous frame
====================================================================== */
void UpdatedFrame(ValueList * me)
{
	// Light the RGB LED (purple)
	//if (CONFIGURATION.config.config & CFG_RGB_LED) {
		LESLEDS.LedRGBON(COLOR_MAGENTA);
		LESLEDS.tempoLedOff();
		// led off after delay
		//rgb_ticker.once_ms(BLINK_LED_MS, LedOff, RGB_LED_PIN);
	//}
	DebuglnF("UpdatedFrame received");

  /*
	// Got at least one ?
	if (me) {
	  WiFiUDP myudp;
	  IPAddress ip = WiFi.localIP();

	  // start UDP server
	  myudp.begin(1201);
	  ip[3] = 255;

	  // transmit broadcast package
	  myudp.beginPacket(ip, 1201);

	  // start of frame
	  myudp.write(TINFO_STX);

	  // Loop thru the node
	  while (me->next) {
		me = me->next;
		// prepare line and write it
		sprintf_P( buff, PSTR("%s %s %c\n"),me->name, me->value, me->checksum );
		myudp.write( buff);
	  }

	  // End of frame
	  myudp.write(TINFO_ETX);
	  myudp.endPacket();
	  myudp.flush();

	}
  */
}

/* ======================================================================
Function: Task_emoncms
Purpose : callback of emoncms ticker
Input   : 
Output  : -
Comments: Like an Interrupt, need to be short, we set flag for main loop
====================================================================== */
volatile boolean task_emoncms = false;
void Task_emoncms()
{
  task_emoncms = true;
}

/* ======================================================================
Function: Task_jeedom
Purpose : callback of jeedom ticker
Input   : 
Output  : -
Comments: Like an Interrupt, need to be short, we set flag for main loop
====================================================================== */
volatile boolean task_jeedom = false;
void Task_jeedom()
{
  task_jeedom = true;
}

/* ======================================================================
Function: Task_httpRequest
Purpose : callback of http request ticker
Input   : 
Output  : -
Comments: Like an Interrupt, need to be short, we set flag for main loop
====================================================================== */
volatile boolean task_httpRequest = false;
void Task_httpRequest()
{
  task_httpRequest = true;
}

/* ======================================================================
Function: NewFrame
Purpose : callback when we received a complete teleinfo frame
Input   : linked list pointer on the concerned data
Output  : -
Comments: -
====================================================================== */
//volatile boolean nouvelleTrame = false;
volatile boolean getImax = false;
void NewFrame(ValueList * me)
{
	// Light the RGB LED 
	if (CONFIGURATION.config.config & CFG_RGB_LED) {
		LESLEDS.LedRGBON(COLOR_GREEN);
		LESLEDS.tempoLedOff();
		// led off after delay
		//rgb_ticker.once_ms((uint32_t)BLINK_LED_MS, LedOff, (int)RGB_LED_PIN);
	}
	DebuglnF("NewFrame received");
	//nouvelleTrame = true;
//	getImax = true;
}

/* ======================================================================
Function: DataCallback
Purpose : callback when we detected new or modified data received
Input   : linked list pointer on the concerned data
		  value current state being TINFO_VALUE_ADDED/TINFO_VALUE_UPDATED
Output  : -
Comments: -
====================================================================== */
//void DataCallback(ValueList * me, uint8_t flags)
//{

	// This is for simulating ADPS during my tests
	// ===========================================
	/*
	static uint8_t test = 0;
	// Each new/updated values
	if (++test >= 20) {
	  test=0;
	  uint8_t anotherflag = TINFO_FLAGS_NONE;
	  ValueList * anotherme = tinfo.addCustomValue("ADPS", "46", &anotherflag);

	  // Do our job (mainly debug)
	  DataCallback(anotherme, anotherflag);
	}
	Debugf("%02d:",test);
	*/
	// ===========================================

  /*
	// Do whatever you want there
	Debug(me->name);
	Debug('=');
	Debug(me->value);

	if ( flags & TINFO_FLAGS_NOTHING ) Debug(F(" Nothing"));
	if ( flags & TINFO_FLAGS_ADDED )   Debug(F(" Added"));
	if ( flags & TINFO_FLAGS_UPDATED ) Debug(F(" Updated"));
	if ( flags & TINFO_FLAGS_EXIST )   Debug(F(" Exist"));
	if ( flags & TINFO_FLAGS_ALERT )   Debug(F(" Alert"));

	Debugln();
  */
//}
//###################################SETUP##################################  
/* ======================================================================
Function: setup
Purpose : Setup I/O and other one time startup stuff
Input   : -
Output  : - 
Comments: -
====================================================================== */
void setup() {

  system_update_cpu_freq(160);

#ifdef SYSLOG
  MYSYSLOG.setSYSLOGselected();
  MYSYSLOG.clearLinesSyslog();
#endif

#ifdef DEBUGSERIAL
	   DEBUG_SERIAL.begin(115200);		//pour test recup SPI
  while (!Serial) {
	  ; // wait for serial port to connect. Needed for native USB port only
  }
#endif

  LESLEDS.InitLed();
  CONFIGURATION.initConfig();
  WIFI.WifiHandleConn(true);

#ifdef SYSLOG
  MYSYSLOG.sendSyslog();
#endif

  WEBSERVER.initSpiffs();

  NTP.UpdateSysinfo(true, true);

  WEBSERVER.initServeur();
 
  CONFIGURATION.showConfig();
  #ifdef  TELEINFO_RXD2 
    DebuglnF("Fin des traces consoles voir avec syslog,la teleinfo est recue sur RXD2");
	DebuglnF("Changement de vitesse des traces consoles voir avec syslog,la teleinfo est maintenant recu sur RXD0 a la place de l'USB(OU RXD2 si Serial.swap())");
  #else
    DebuglnF("Les traces consoles continue de fonctionner sur TXD0 à 1200 bauds ou VITESSE_SIMUTRAMETEMPO si SIMUTRAMETEMPO");
	DebuglnF("La teleinfo est recue sur RXD0");
  #endif
	#ifdef MODE_HISTORIQUE
		#ifdef SIMUTRAMETEMPO
			#ifdef TELEINFO_RXD2
				DebuglnF("Pour la simulation:strapper D4(TXD1) et D7(RXD2)");
			#else
				DebuglnF("Pour la simulation:strapper D4(TXD1) et D9(RXD0");
			#endif

			const int VITESSE_SIMUTRAMETEMPO = 115200;

			Serial.begin(VITESSE_SIMUTRAMETEMPO); 
			Serial.setRxBufferSize(1024);
		#else
			Serial.begin(1200, SERIAL_7E1);
			DebuglnF("Serial.begin");
		#endif
	#else
		Serial.begin(9600, SERIAL_7E1);       //5.3.5. Couche physique document enedis Enedis-NOI-CPT_54E.pdf 
	#endif
#ifdef TELEINFO_RXD2
	Serial.swap();  // reception teleinfo sur rxd2 sinon passe la reception teleinfo sur rx0 pour recuperer rx2 pour mosi
						//fonctionne correctement sur rx0, il faut juste penser à enlever le strap de la simulation si present
						//pour programmer ou debuguer via la console, pas de pb en OTA.
#endif

	#ifdef SIMUTRAMETEMPO
	  SIMU_TEMPO.initSimuTrameTempo();
	#endif

#ifdef SIMUTRAMETEMPO
	SerialSimu.begin(VITESSE_SIMUTRAMETEMPO);	//19200, SERIAL_7E1
#endif
#ifdef MODE_HISTORIQUE
  TINFO.init(true);
#else
  TINFO.init(false);
#endif
  TINFO.attachADPS(ADPSCallback);
  TINFO.attachNewFrame(NewFrame);
  TINFO.attachUpdatedFrame(UpdatedFrame);
  LESLEDS.LedRGBOFF();

  // Emoncms Update if needed
  if (CONFIGURATION.config.emoncms.freq)
    Tick_emoncms.attach(CONFIGURATION.config.emoncms.freq, Task_emoncms);

  // Jeedom Update if needed
  if (CONFIGURATION.config.jeedom.freq)
    Tick_jeedom.attach(CONFIGURATION.config.jeedom.freq, Task_jeedom);

  // HTTP Request Update if needed
  if (CONFIGURATION.config.httpReq.freq)
    Tick_httpRequest.attach(CONFIGURATION.config.httpReq.freq, Task_httpRequest);

  uint8_t timeout = 5;
  while (WIFINOOKOU && timeout)
  {
	  delay(500);
	  DebugF(".");
	  --timeout;
  }
#ifdef AVEC_NTP 
  NTP.init();
#endif

}


//###################################LOOP##################################  

/* ======================================================================
Function: loop
Purpose : infinite loop main code
Input   : -
Output  : - 
Comments: -
====================================================================== */

void loop()
{
static unsigned long start = 0;
static unsigned long duree = 0;
static unsigned long dureeMax = 0;

  start = millis();
 
//************************************************1 fois par seconde*************************************************************************************
  // Only once task per loop, let system do its own task
  if (NTP.getCycle1Seconde()) {
	  WEBSERVER.handleClient();
	  MYOTA.handle();

#ifdef AVEC_NTP
	  NTP.refreshTimeIfMidi();		//si midi et wifi :remise a l'heure
#endif
	  NTP.UpdateSysinfo(false, false);

	  WIFI.testWifi();

#ifdef SIMUTRAMETEMPO
  SIMU_TEMPO.traite1Trame(NTP.getSeconds1970());
#endif
DebugF("dureeMax:");
  Debugln((long)dureeMax);
  dureeMax = 0;

  }
  //*************************************************************************************************************************************
	else if (task_emoncms) { 
    WEBCLIENT.emoncmsPost(); 
    task_emoncms=false; 
  } else if (task_jeedom) { 
	  WEBCLIENT.jeedomPost();
	task_jeedom=false;
  } else if (task_httpRequest) { 
	  WEBCLIENT.httpRequest();
    task_httpRequest=false;
  } 
	if (TINFO.getReinit())
	{
 		WEBSERVER.incNb_reinit();    //account of reinit operations, for system infos
#ifdef MODE_HISTORIQUE
		TINFO.init(true);//Clear ListValues, buffer, and wait for next STX
#else
		TINFO.init(false);
#endif
	} 
	else
	{
	  if ( Serial.available() )
	    { 
				char c = Serial.read();          //pour test recup SPI
				TINFO.process(c);
		}
	}
	unsigned long temp = millis();
	duree = temp - start;	
	if (duree > dureeMax)
  	  dureeMax = duree;
 }   //loop
