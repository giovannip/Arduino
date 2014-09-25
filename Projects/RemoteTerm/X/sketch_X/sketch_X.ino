#include <EEPROM.h>
#include <SPI.h>
#include "RF24.h"

const int sensorPin = A0;
const int isTerminalPin = 9;

bool isTerminal;

//Gambi
const uint64_t pipeServerToClient  = 0xE8E8F0F0E1LL;
const uint64_t pipeClientToServer  = 0xF0F0F0F0E1LL;

const int radioChannel = 10;

int temperatureRaw;
float temperature = 0;

int SendedPackage	[2]; //Gambi -> pq struct nao fuincionou
int RecivedPackage	[3]; //Gambi -> so uso 2, mas so funcionou com 3 wtf???

RF24 radio(8,7);

void calcTemp()
{
        temperature = temperatureRaw * 0.48828125;
}

void SendData()
{
	SendedPackage[0] = temperatureRaw;
	SendedPackage[1] = temperature;

	int ntry = 3;
	bool done = false;
	while (!done && ntry > 0)
	{
		done = radio.write( SendedPackage, sizeof(SendedPackage) );
		ntry--;
	}
        /*
        if(done)
        	Serial.println("\tSendData OK");
        else
        	Serial.println("\tSendData NOK");
        */
}

void ReadSensor()
{
        temperatureRaw = analogRead(sensorPin);
        calcTemp();
	Serial.print("Term.Temperature:");
	Serial.println(temperature);
}

void setup(void)
{
	Serial.begin(9600);
	
	pinMode(isTerminalPin, INPUT);
	isTerminal = digitalRead(isTerminalPin);

	radio.begin();
	radio.setChannel(radioChannel);
	if (isTerminal)
	{
                Serial.println("isTerminal");
		//radio.openReadingPipe(1,pipeClientToServer);
		radio.openWritingPipe(  pipeServerToClient);
	}
	else
	{
                Serial.println("non.Terminal");
		radio.openReadingPipe(1,pipeServerToClient);
		//radio.openWritingPipe(  pipeClientToServer);
        	radio.startListening();
	}
}

void loop(void)
{
	if (isTerminal)
	{	
		ReadSensor();
		SendData();
        	delay(500);
	}
	else
	{
		if (radio.available())
		{
			//Serial.println("radio.available");
		
			int ntry = 3;
			bool done = false;
			while (!done && ntry > 0)
			{
				done = radio.read( RecivedPackage, sizeof(RecivedPackage) );
				ntry--;
			}
                        temperatureRaw = RecivedPackage[0];
                        calcTemp();			

			Serial.print("Serv.Temperature:");
			Serial.println(temperature);
		}
                /*
                else
                {
			Serial.println("radio.non.available");
                }
                */
	}
}
