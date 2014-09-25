#include <EEPROM.h>
#include <SPI.h>
#include "RF24.h"

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
}
 
void easyGo()
{
	PackData();
	SendData();
	ApplyLedState();
}

void loop(void)
{
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
