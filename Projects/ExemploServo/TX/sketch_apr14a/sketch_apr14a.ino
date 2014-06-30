#include <SPI.h>
#include "RF24.h"
 
int joystick[2];
 
RF24 radio(8,7);
 
const uint64_t pipe = 0xE8E8F0F0E1LL;
 
void setup(void)
{
  Serial.begin(9600);
  radio.begin();
  radio.openWritingPipe(pipe);
}
 
void loop(void)
{
  joystick[0] = analogRead(A0);
  
  radio.write( joystick, sizeof(joystick) );
 
}
