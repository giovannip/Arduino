#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include <Servo.h> 

Servo myservo;  // create servo object to control a servo 
 
int val;    // variable to read the value from the analog pin 
String bufpot1;

 
int joystick[2];
 
RF24 radio(8,7);
const uint64_t pipe = 0xE8E8F0F0E1LL;
 
void setup(void)
{
  myservo.attach(5); 
  myservo.write(90); 
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
      done = radio.read( joystick, sizeof(joystick) );
      val = joystick[0];
      
              val = map(val, 0, 1023, 0, 180);     // scale it to use it with the servo (value between 0 and 180) 

        myservo.write(val);  
        
      Serial.println(joystick[0]);
delay(21);
    }
  }
  else
  {
    Serial.println("No radio available");
  }
}
