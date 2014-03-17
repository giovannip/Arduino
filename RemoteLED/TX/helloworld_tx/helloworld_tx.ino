#include <RF24Network.h>
#include <RF24.h>
#include <SPI.h>

// nRF24L01(+) radio attached using Getting Started board 
RF24 radio(9,10);

// Network uses that radio
RF24Network network(radio);

// Address of our node
const uint16_t this_node = 1;

// Address of the other node
const uint16_t other_node = 0;

// How often to send 'hello world to the other unit
const unsigned long interval = 2000; //ms

int lstate;

// Structure of our payload
struct payload_t
{
  int state;
};

void setup(void)
{
	lstate = LOW;
	pinMode(2, OUTPUT);
	digitalWrite(2, LOW);
	Serial.begin(57600);
 
	SPI.begin();
	radio.begin();
	network.begin(/*channel*/ 90, /*node address*/ this_node);
}

void loop(void)
{
	// Pump the network regularly
	network.update();

	lstate = !lstate;

	Serial.print("Sending...");
	payload_t payload = { lstate };
	RF24NetworkHeader header(/*to node*/ other_node);
	bool ok = network.write(header,&payload,sizeof(payload));
	
	digitalWrite(2, payload.state);
	
	if (ok)
		Serial.println("ok.");
	else
		Serial.println("failed.");
	delay(interval);
}
// vim:ai:cin:sts=2 sw=2 ft=cpp
