
#include <NECIRrcv.h>
#include <avr/eeprom.h>
#define IRPIN 4    // pin that IR detector is connected to
#define ARRAYMAX 20

NECIRrcv ir(IRPIN) ;

unsigned long ircodes[ARRAYMAX];
\
void setup()
{
  Serial.begin(9600) ;
  Serial.println("NEC IR code reception") ;
  ir.begin() ;
  eeprom_read_block((void*)&ircodes, (void*)0, sizeof(ircodes));

}

void loop()
{
  unsigned long ircode ;
  int i, lastPos, Pos;
  
  while (ir.available()) 
  {
    ircode = ir.read() ;
    
    Pos = lastPos = -1;
    for ( i = 0 ; ( ( i < ARRAYMAX ) && ( ( lastPos == -1 ) || ( Pos == -1 ) ) ) ; i++ )
    {
      if ( ( ircodes[i] == 0 ) && ( lastPos == -1 )  )
        lastPos = i;
      else if ( ( ircodes[i] == ircode ) && ( Pos == -1 ) )
        Pos = i;
    }
    if ( Pos == -1 )
    {
      ircodes[lastPos] = ircode;
      Serial.print("got new code: 0x") ;
      Serial.println(ircode,HEX) ;
    }
    else
    {
      Serial.print("------------ same: 0x") ;
      Serial.println(ircode,HEX) ;
    }
  }
  eeprom_write_block((const void*)&ircodes, (void*)0, sizeof(ircodes));
}
