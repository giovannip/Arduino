#include <EEPROM.h>
#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
//#include <avr/eeprom.h>

const int LedPin = 2;

//Gambi
const char SetClient1 = 'r';
const char SetClient2 = 't';

const uint64_t pipe  = 0xE8E8F0F0E1LL;
const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };

byte myId;
byte myState;

byte package[3]; //Gambi -> pq struct n�o fuincionou
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
	Serial.println(package[0]);
	Serial.println(package[1]);
	Serial.println(package[2]);
	
	if (package[0] != myId)
		return false;

	myId = package[0];
	myState = package[1];
	return true;
}

void setup(void)
{
	Serial.begin(9600);

	radio.begin();
	radio.setChannel(10);
	radio.openReadingPipe(1,pipe);
		
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
			done = radio.read( package, sizeof(package) );
		}

		if (UpdateMyData())
		{
			Serial.println("UpdateMyData true");
			
			ApplyLedState();
			WriteConfig();
		}
	}
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
