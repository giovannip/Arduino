#include <EEPROM.h>
#include <SPI.h>
#include "RF24.h"

const int LedPin = 2;

//Gambi
const char SetClient1			= 'r';
const char SetClient2			= 't';
const char SetServer			= 'm';

const char ResponseServerHIGH	= 's';
const char ResponseServerLOW	= 'w';
const char LedOn 				= 'l';
const char LedOff				= 'd';
const char AutoOn				= 'a';
const char AutoOff				= 'q';

const uint64_t pipeServerToClient  = 0xE8E8F0F0E1LL;
const uint64_t pipeClientToServer  = 0xF0F0F0F0E1LL;

const int radioChannel = 10;

byte isServer;

byte myId;
byte myState;
byte targetId;
byte autoLoop;

byte SendedPackage	[2]; //Gambi -> pq struct nao fuincionou
byte RecivedPackage	[3]; //Gambi -> so uso 2, mas so funcionou com 3 wtf???
const int KId		= 0; // [0]
const int KState	= 1; // [1]

RF24 radio(8,7);

void ReadConfig()
{
	Serial.println("ReadConfig");

	myId     = EEPROM.read(KId);
	myState  = EEPROM.read(KState);
	
	// TODO: Fazer direito
	isServer = myId == 99;
	
	if (isServer)
	{
		targetId = EEPROM.read(2); // TODO keys
		autoLoop = EEPROM.read(3);
	}
	
	Serial.print("myId        -> ");
	Serial.println(myId);
	Serial.print("myState   -> ");
	Serial.println(myState);
	
	if (isServer)
	{
		Serial.print("targetId    -> ");
		Serial.println(targetId);
		Serial.print("autoLoop  -> ");
		Serial.println(autoLoop);
	}
}

void WriteConfig()
{
	Serial.println("WriteConfig");
	
	EEPROM.write(KId, myId);
	EEPROM.write(KState, myState);
	
	if (isServer)
	{
		EEPROM.write(2, targetId); // TODO: keys
		EEPROM.write(3, autoLoop);
	}
	
	Serial.print("myId        -> ");
	Serial.println(myId);
	Serial.print("myState   -> ");
	Serial.println(myState);
	
	if (isServer)
	{
		Serial.print("targetId    -> ");
		Serial.println(targetId);
		Serial.print("autoLoop  -> ");
		Serial.println(autoLoop);
	}
}

void ApplyLedState()
{
	Serial.println("ApplyLedState");
	digitalWrite(LedPin, myState);
}

void PackData()
{
	Serial.print("PackData -> ");
	SendedPackage[KId] = isServer ? targetId : myId;
	SendedPackage[KState] = myState;
	Serial.print(SendedPackage[KId]);
	Serial.print(" : ");
	Serial.println(SendedPackage[KState]);
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

void UpdateMyData()
{
	Serial.println("UpdateMyData true");
	
	myState = RecivedPackage[KState];
	ApplyLedState();
	WriteConfig();
}

void setup(void)
{
	Serial.begin(9600);
	pinMode(LedPin, OUTPUT);
	
	radio.begin();
	radio.setChannel(radioChannel);
	if (isServer)
	{
		radio.openReadingPipe(1,pipeClientToServer);
		radio.openWritingPipe(  pipeServerToClient);
	}
	else
	{
		radio.openReadingPipe(1,pipeServerToClient);
		radio.openWritingPipe(  pipeClientToServer);
	}
	radio.startListening();

	ReadConfig();
	ApplyLedState();	
}

void loop(void)
{
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
		if (  ( isServer && RecivedPackage[KId] != myId)
		   || (!isServer && RecivedPackage[KId] == myId)  )
			UpdateMyData();
		
	}
	
	if (isServer && autoLoop == HIGH)
	{	
		myState = !myState;

		PackData();
		SendData();
		ApplyLedState();
		WriteConfig();
		
		delay(500);
	}
}

void serialEvent() 
{
	char inChar = (char)Serial.read(); 
	
	if (inChar == SetClient1)
	{
		myId = 1;
		if (isServer)
			targetId = 1;
		WriteConfig();
	}
	else if (inChar == SetClient2) 
	{
		myId = 2;
		if (isServer)
			targetId = 2;
		WriteConfig();
	}
	else if (inChar == SetServer) 
	{
		if (isServer)
		{
			myId = 99;
			WriteConfig();
		}
		else
			Serial.println("nao server");
	}
	else if (inChar == ResponseServerHIGH) 
	{
		myState = HIGH;
		PackData();
		SendData();
		ApplyLedState();
		WriteConfig();
	}
	else if (inChar == ResponseServerLOW) 
	{
		myState = LOW;
        PackData();
		SendData();
		ApplyLedState();
		WriteConfig();
	}
	else if (inChar == LedOn) 
	{
		myState = HIGH;
		PackData();
		SendData();
		ApplyLedState();
		WriteConfig();
	}
	else if (inChar == LedOff)
	{
		myState = LOW;
		PackData();
		SendData();
		ApplyLedState();
		WriteConfig();
	}
	else if (inChar == AutoOn) 
	{
		autoLoop = HIGH;
		PackData();
		SendData();
		ApplyLedState();
		WriteConfig();
	}
	else if (inChar == AutoOff)
	{
		autoLoop = LOW;
		PackData();
		SendData();
		ApplyLedState();
		WriteConfig();
	}
	else
	{
		Serial.println("Comando desconhecido");
	}

}
