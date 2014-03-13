#include <SPI.h>
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
	radio.openWritingPipe(pipe);
}
 
void loop(void)
{
	if (val[0] == HIGH)
		val[0] = LOW;
	else
		val[0] = HIGH;
		
	digitalWrite(2, val[0]);
		
	radio.write( val, sizeof(val) );

        delay(500);
}
