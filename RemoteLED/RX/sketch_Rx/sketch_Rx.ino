#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
 
int val[2];
 
RF24 radio(8,7);
const uint64_t pipe = 0xE8E8F0F0E1LL;
 
void setup(void)
{
	pinMode(2, OUTPUT);
	val[0] = LOW;
	digitalWrite(2, val[0]);
	
	Serial.begin(9600);
	radio.begin();
	radio.openReadingPipe(1,pipe);
	radio.startListening();
}
 
void loop(void)
{
	if ( radio.available() )
	{
		// Dump the payloads until we've gotten everything
		bool done = false;
		while (!done)
		{
			// Fetch the payload, and see if this was the last one.
			done = radio.read( val, sizeof(val) );
			
			digitalWrite(2, val[0]);
			
			Serial.println(val[0]);
			delay(21);
		}
	}
	else
	{
		Serial.println("No radio available");
	}
}
