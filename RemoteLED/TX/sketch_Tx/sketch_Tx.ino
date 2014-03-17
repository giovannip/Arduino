#include <SPI.h>
#include "RF24.h"
 
int val[2];
int lauto;
 
RF24 radio(8,7);
 
const uint64_t pipe = 0xE8E8F0F0E1LL;
 
void setup(void)
{
	pinMode(2, OUTPUT);
	val[0] = LOW;
	lauto = HIGH;
	digitalWrite(2, val[0]);
	
	Serial.begin(9600);
	radio.begin();
	radio.setChannel(10);
	radio.openWritingPipe(pipe);
}
 
void loop(void)
{
	if (lauto == HIGH)
	{
		if (val[0] == HIGH)
			val[0] = LOW;
		else
			val[0] = HIGH;
			
		digitalWrite(2, val[0]);
			
		radio.write( val, sizeof(val) );

			delay(500);
	}
}


void serialEvent() 
{
	char inChar = (char)Serial.read(); 
	
	if (inChar == 'd') 
		val[0] = LOW;
	else if (inChar == 'l') 
		val[0] = HIGH;
	else if (inChar == 'a') 
		lauto = HIGH;
	else if (inChar == 'q') 
		lauto= LOW;
		
	digitalWrite(2, val[0]);
		
	radio.write( val, sizeof(val) );
}