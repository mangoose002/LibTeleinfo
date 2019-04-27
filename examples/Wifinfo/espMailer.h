// **********************************************************************************
// ESP8266 Pilotage VMC et TEMPO->Emission mail pour enregistrement
// **********************************************************************************
//https://github.com/ArduinoHannover/ESPMailer
//file espMailer.h
// **********************************************************************************
#include <Arduino.h>

#ifndef MAIL_H
#define MAIL_H

#ifndef null
#define null NULL
#endif

		
enum AUTH {
	PLAIN,
	LOGIN,
	XOAUTH2,	// Not supported
	NTLM,		// Not supported
	CRAM_MD5	// Not supported
};

class ESPMailer {
	private:
		const char* _b64_alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
		const char* _delim = "\n";
		
		WiFiClient _smtp;
		
		char* _from = NULL;
		char* _fromName = NULL;
		char* _to = NULL;
		char* _toNames = NULL;
		char* _cc = NULL;
		char* _ccNames = NULL;
		char* _bcc = NULL;
		char* _bccNames = NULL;
		
		boolean html = false;
		float _timezone = 0;
		int8_t do_debug = 0;
		
		boolean sendCMD(const char*, uint16_t);
		void a3_to_a4(unsigned char*, unsigned char*);
		int base64_encode(char*, const char*, int);
		char* append(char*&, const char*);
		bool SMTPTo(char*);
		void HeaderTo(const char*, char*, char*);
		String last_reply;
	public:
		ESPMailer(void);
		~ESPMailer(void);
		
		void setFrom(const char*);
		void setFrom(const char*, const char*);
		void addAddress(const char*);
		void addAddress(const char*, const char*);
		void addCC(const char*);
		void addCC(const char*, const char*);
		void addBCC(const char*);
		void addBCC(const char*, const char*);
		void setTimezone(float);
		void setDebugLevel(uint8_t);
		boolean isHTML(void);
		boolean isHTML(boolean);
		//boolean send();
		//marc ajout parametre
		boolean send(time_t tm);
		//boolean send();
		String Host;
		//marc
		uint16_t Port = 587;
		boolean SMTPAuth = true;
		/*uint16_t Port = 25;
		boolean SMTPAuth = false;*/

		String Username;
		String Password;
		String Subject;
		String Body;
		uint16_t Timeout = 300;
		String AltBody;
		AUTH AuthType = PLAIN;
		
		boolean TLS = false;
};
#endif
