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

const uint64_t pipe  = 0xE8E8F0F0E1LL;
const uint64_t pipes[3] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };

byte myId; // Dump
byte myState;
byte targetId;
byte autoLoop;

byte package[2]; //Gambi -> pq struct n�o fuincionou
// [0] Id
// [1] State
 
RF24 radio(8,7);

void PackData()
{
	Serial.print("PackData -> ");
	Serial.print(package[0]);
	Serial.print(" : ");
	Serial.println(package[1]);
	package[0] = targetId;
	package[1] = myState;
}

void ReadConfig()
{
	Serial.println("ReadConfig");

	myId     = EEPROM.read(0);
	myState  = EEPROM.read(1);
	targetId = EEPROM.read(2);
	autoLoop = EEPROM.read(3);
	
	Serial.println(myId);
	Serial.println(myState);
	Serial.println(targetId);
	Serial.println(autoLoop);
}

void WriteConfig()
{
	Serial.println("WriteConfig");
	
	EEPROM.write(0, myId);
	EEPROM.write(1, myState);
	EEPROM.write(2, targetId);
	EEPROM.write(3, autoLoop);
	
	Serial.println(myId);
	Serial.println(myState);
	Serial.println(targetId);
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
	radio.write( package, sizeof(package) );
}
 
void setup(void)
{
	pinMode(LedPin, OUTPUT);

	ApplyLedState();
	
	Serial.begin(9600);
	radio.begin();
	radio.setChannel(10);
	radio.openWritingPipe(pipe);

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
	if (autoLoop == LOW)
		return;
	
	myState = !myState;

	easyGo();
	
	delay(500);
}

void serialEvent() 
{
	char inChar = (char)Serial.read(); 
	
	if (inChar == SetClient1)
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
