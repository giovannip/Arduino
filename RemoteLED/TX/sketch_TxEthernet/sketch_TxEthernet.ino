#include <EEPROM.h>
#include <SPI.h>
#include "RF24.h"
#include <Ethernet.h>

const int LedPin = 2;

const char LedOn  = 'l';
const char LedOff = 'd';
const char AutoOn = 'a';
const char AutoOff = 'q';

//Gambi
const char SetClient1 = 'r';
const char SetClient2 = 't';

//const uint64_t pipe  = 0xE8E8F0F0E1LL;
//const uint64_t pipes[3] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };
const uint64_t pipeServerToClient  = 0xE8E8F0F0E1LL;
const uint64_t pipeClientToServer  = 0xF0F0F0F0E1LL;
const int radioChannel = 10;

byte myId; // Dump
byte myState;
byte targetId;
byte autoLoop;

byte SendedPackage[2]; //Gambi -> pq struct nao fuincionou
byte RecivedPackage[3];
// [0] Id
// [1] State
 
RF24 radio(8,7);

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED }; //physical mac address
byte ip[] = { 192, 168, 0, 177 }; // ip in lan
byte gateway[] = { 192, 168, 0, 1 }; // internet access via router
byte subnet[] = { 255, 255, 255, 0 }; //subnet mask
EthernetServer server(80); //server port

String readString;

void PackData()
{
	Serial.print("PackData -> ");
	Serial.print(SendedPackage[0]);
	Serial.print(" : ");
	Serial.println(SendedPackage[1]);
	SendedPackage[0] = targetId;
	SendedPackage[1] = myState;
}

void ReadConfig()
{
	Serial.println("ReadConfig");

	myId     = EEPROM.read(0);
	myState  = EEPROM.read(1);
	targetId = EEPROM.read(2);
	autoLoop = EEPROM.read(3);
	
	Serial.print("myId        -> ");
	Serial.println(myId);
	Serial.print("myState   -> ");
	Serial.println(myState);
	Serial.print("targetId    -> ");
	Serial.println(targetId);
	Serial.print("autoLoop  -> ");
	Serial.println(autoLoop);
}

void WriteConfig()
{
	Serial.println("WriteConfig");
	
	EEPROM.write(0, myId);
	EEPROM.write(1, myState);
	EEPROM.write(2, targetId);
	EEPROM.write(3, autoLoop);
	
	Serial.print("myId        -> ");
	Serial.println(myId);
	Serial.print("myState   -> ");
	Serial.println(myState);
	Serial.print("targetId    -> ");
	Serial.println(targetId);
	Serial.print("autoLoop  -> ");
	Serial.println(autoLoop);
}

void ApplyLedState()
{
	Serial.println("ApplyLedState");
	digitalWrite(LedPin, myState);
}

void SendData()
{
	Serial.println("SendData");
	int ntry = 3;
	bool done = false;
	while (!done && ntry > 0)
	{
		done = radio.write( SendedPackage, sizeof(SendedPackage) );
		ntry--;
	}
	radio.startListening();
	Serial.print("SendData ok -> ");
	Serial.println(done);
}

 
void setup(void)
{
	pinMode(LedPin, OUTPUT);

	ApplyLedState();
	
	Serial.begin(9600);
	
	radio.begin();
	radio.setChannel(radioChannel);
	radio.openWritingPipe(pipeServerToClient);
	radio.openReadingPipe(1,pipeClientToServer);
		
	radio.startListening();
	
	ReadConfig();
	
	Ethernet.begin(mac, ip, gateway, subnet);
	server.begin();
	//the pin for the servo co
	//enable serial data print
	
	Serial.print("server is at ");
	Serial.println(Ethernet.localIP());
}
 
void easyGo()
{
	PackData();
	SendData();
	ApplyLedState();
}

void loop(void)
{
	EthernetClient client = server.available();
	if (client) 
	{
		while (client.connected()) 
		{
			if (client.available()) 
			{
				char c = client.read();

				//read char by char HTTP request
				if (readString.length() < 100) 
				{
					//store characters to string
					readString += c;
					//Serial.print(c);
				}

				//if HTTP request has ended
				if (c == '\n') 
				{

					///////////////
					Serial.println(readString); //print to serial monitor for debuging

					client.println("HTTP/1.1 200 OK"); //send new page
					client.println("Content-Type: text/html");
					client.println();

					client.println("<HTML>");
					client.println("<HEAD>");
					client.println("<meta name='apple-mobile-web-app-capable' content='yes' />");
					client.println("<meta name='apple-mobile-web-app-status-bar-style' content='black-translucent' />");
					client.println("<link rel='stylesheet' type='text/css' href='http://homeautocss.net84.net/a.css' />");
					client.println("<TITLE>Home Automation</TITLE>");
					client.println("</HEAD>");
					client.println("<BODY>");
					client.println("<H1>Home Automation</H1>");
					client.println("<hr />");
					client.println("<br />");

					client.println("<a href=\"/?SetClient1\"\">Set Client Vermelho</a>");
					client.println("<a href=\"/?SetClient2\"\">Set Client Verde</a><br /><br /><br />");        
					client.println("<a href=\"/?LedOn\"\">Led On</a>");
					client.println("<a href=\"/?LedOff\"\">Led Off</a><br /><br /><br />");        
					client.println("<a href=\"/?AutoOn\"\">Auto Play On</a>");
					client.println("<a href=\"/?AutoOff\"\">Auto Play Off</a><br /><br /><br />");        

					client.println("</BODY>");
					client.println("</HTML>");

					delay(1);
					//stopping client
					client.stop();

					byte change = 1;
					if(readString.indexOf("?SetClient1") > 0)
						targetId = 1;
					else if(readString.indexOf("?SetClient2") > 0)
						targetId = 2;
					else if(readString.indexOf("?LedOn") > 0) 
							myState = HIGH;
					else if(readString.indexOf("?LedOff") > 0)
						myState = LOW;
					else if(readString.indexOf("?AutoOn") > 0)
						autoLoop = HIGH;
					else if(readString.indexOf("?AutoOff") > 0)
						autoLoop = LOW;
					else
						change = 0;

					if (change)
					{
						WriteConfig();
						easyGo();
					}
					
					readString="";
				}
			}
		}
	}

	if (autoLoop == HIGH)
	{	
		myState = !myState;

		easyGo();
		
		delay(500);
	}
	if (radio.available())
	{
		Serial.println("radio.available");
	
		int ntry = 3;
		bool done = false;
		while (!done && ntry > 0)
		{
			done = radio.read( RecivedPackage, sizeof(RecivedPackage) );
			ntry--;
		}
		if (RecivedPackage[0] != myId)
		{
			myState = RecivedPackage[1];
			ApplyLedState();
		}
	}
}

void serialEvent() 
{
	char inChar = (char)Serial.read(); 
	
	if (inChar == 'm')
	{
		myId = 99;
		WriteConfig();
	}
	else if (inChar == SetClient1)
		targetId = 1;
	else if (inChar == SetClient2) 
		targetId = 2;
	else if (inChar == LedOn) 
			myState = HIGH;
	else if (inChar == LedOff) 
		myState = LOW;
	else if (inChar == AutoOn) 
		autoLoop = HIGH;
	else if (inChar == AutoOff) 
		autoLoop = LOW;
	else
		return;

	WriteConfig();
	easyGo();

}
