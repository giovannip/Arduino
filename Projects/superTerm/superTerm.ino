#include <LiquidCrystal.h>

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
const int sensorPin = 0;
const String Cel = " 'C";
const String xluz = "Luz";
const int photocellPin = 1;
const int LEDDisplaypin = 10;

const int LEDRedpin = 9;
const int LEDGreenpin = 8;
const int LEDBluepin = 7;

const int Buttonpin = 6;
int ButtonpinResult = 0;
int State = 1;

void mdelay ( int t )
{
  int xt = t;  
  while ( xt > 0)
  {
    checkLight ();
    delay(1);
    xt--;
  }
}

void RGBLed (int pled)
{
  if ( pled < LEDBluepin || pled > LEDRedpin )
    exit;

  digitalWrite (LEDRedpin   , LOW);
  digitalWrite (LEDGreenpin , LOW);
  digitalWrite (LEDBluepin  , LOW);

  digitalWrite (pled   , HIGH);
}

void checkLight ()
{
  if ( digitalRead ( Buttonpin ) == HIGH )
    ButtonpinResult = 1;
//  else
  //  ButtonpinResult = 0;
    
  if ( ( analogRead(photocellPin) <= 50 ) || ButtonpinResult )
  {
    digitalWrite(LEDDisplaypin, HIGH);   
    if ( ButtonpinResult > 0 )
      ButtonpinResult--;
  }
  else
    digitalWrite(LEDDisplaypin, LOW);
}

void FTemperature ()
{
  float temperature;
  temperature = analogRead(sensorPin);    // reading analog sensor value
  temperature = temperature*0.488;   
  lcd.clear();
  lcd.print("Temperature !"); 
  lcd.setCursor(0, 1);

  if (temperature >= 30)
    RGBLed(LEDRedpin);
  else if (temperature >= 20)
    RGBLed(LEDGreenpin);
  else
    RGBLed(LEDBluepin);

  checkLight ();

  lcd.print(temperature);
  lcd.print(Cel);
  Serial.print(temperature);
  Serial.println(Cel);
  
  mdelay(500);  
}

void Fluz()
{
  int luz;
  luz = analogRead(photocellPin);
  lcd.clear();
  lcd.print("Luz         !"); 
  lcd.setCursor(0, 1);

  if (luz >= 700)
    RGBLed(LEDRedpin);
  else if (luz >= 200)
    RGBLed(LEDGreenpin);
  else
    RGBLed(LEDBluepin);

  checkLight ();

  lcd.print(luz);
  lcd.print(" - ");
  lcd.print(xluz);
  Serial.print(xluz);
  Serial.print(" - ");
  Serial.println ( analogRead(photocellPin) );

  mdelay(500);            
}

///////////////////////

void setup()
{
  int x;
  lcd.begin(16, 2);
  pinMode (LEDDisplaypin , OUTPUT);

  pinMode (LEDRedpin , OUTPUT);
  pinMode (LEDGreenpin , OUTPUT);
  pinMode (LEDBluepin , OUTPUT);

  pinMode (Buttonpin , INPUT);  
  
  Serial.begin(9600);

  checkLight ();

  lcd.setCursor(0, 0);
  lcd.print("    LOADING");

  lcd.setCursor(0, 1); 
  for ( int i = 0 ; i < 17 ; i++ )
  {
    x = ( i%3 ) + LEDBluepin;

    lcd.print("=");
    checkLight ();
    RGBLed (x);
    mdelay(150);
  }


}

void loop()
{
    if ( digitalRead ( Buttonpin ) == HIGH )
      State = ( State )?0:1;
    
    if ( State )
    {
      Serial.println ("FTemperature");
      FTemperature ();
    }
    else
    {
      Serial.println ("Fluz");
      Fluz();
    }
}

