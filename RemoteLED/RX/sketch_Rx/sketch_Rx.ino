#include <EEPROM.h>
#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
//#include <avr/eeprom.h>

const int LedPin = 2;

//Gambi
const char SetClient1 = 'r';
const char SetClient2 = 't';

//const uint64_t pipe  = 0xE8E8F0F0E1LL;
//const uint64_t pipes[3] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };
const uint64_t pipeServerToClient  = 0xE8E8F0F0E1LL;
const uint64_t pipeClientToServer  = 0xF0F0F0F0E1LL;
const int radioChannel = 10;


byte myId;
byte myState;

byte RecivedPackage[3]; //Gambi -> pq struct nao fuincionou
byte SendedPackage[2];
// [0] Id
// [1] State

RF24 radio(8,7);

void ReadConfig()
{
	Serial.println("ReadConfig");

	myId    = EEPROM.read(0);
	myState = EEPROM.read(1);

	Serial.println(myId);
	Serial.println(myState);
}

void WriteConfig()
{
	Serial.println("WriteConfig");

	EEPROM.write(0, myId);
	EEPROM.write(1, myState);

	Serial.println(myId);
	Serial.println(myState);
}

void ApplyLedState()
{
	digitalWrite(LedPin, myState);
}

bool UpdateMyData()
{
	Serial.println("UpdateMyData");
	Serial.println(RecivedPackage[0]);
	Serial.println(RecivedPackage[1]);
	Serial.println(RecivedPackage[2]);
	
	if (RecivedPackage[0] != myId)
		return false;

	myId = RecivedPackage[0];
	myState = RecivedPackage[1];
	return true;
}

void setup(void)
{
	Serial.begin(9600);

	radio.begin();
	radio.setChannel(radioChannel);
	radio.openReadingPipe(1,pipeServerToClient);
	radio.openWritingPipe(pipeClientToServer);
	
	radio.startListening();

	pinMode(LedPin, OUTPUT);

	ReadConfig();
	ApplyLedState();	
}

void loop(void)
{
	if (radio.available())
	{
		Serial.println("radio.available");
	
		// Dump the payloads until we've gotten everything
		bool done = false;
		while (!done)
		{
			// Fetch the payload, and see if this was the last one.
			done = radio.read( RecivedPackage, sizeof(RecivedPackage) );
		}
		
		//recebimento
		PackData();
		SendData();

		if (UpdateMyData())
		{
			Serial.println("UpdateMyData true");
			
			ApplyLedState();
			WriteConfig();
		}
	}
}

void PackData()
{
	Serial.print("PackData -> ");
	Serial.print(SendedPackage[0]);
	Serial.print(" : ");
	Serial.println(SendedPackage[1]);
	SendedPackage[0] = myId;
	SendedPackage[1] = 1; //pensar o que nao seria OK
}

void SendData()
{
	Serial.println("SendData");
	radio.write( SendedPackage, sizeof(SendedPackage) );
}

void serialEvent() 
{
	char inChar = (char)Serial.read(); 
	
	if (inChar == SetClient1)
	{
		myId = 1;
		WriteConfig();
	}
	else if (inChar == SetClient2) 
	{
		myId = 2;
		WriteConfig();
	}
}
