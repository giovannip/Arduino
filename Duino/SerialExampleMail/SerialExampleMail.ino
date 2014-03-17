#include <LiquidCrystal.h>

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

String meutexto;

//void main() {
//  setup();
//  while(true)
//    loop();
//}

void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
}

void loop(){
/*
  if (Serial.available()) {
    delay(100);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Olah: ");
    //lcd.setCursor(0, 1);
    while (Serial.available() > 0) {
      meutexto += (char)Serial.read();
    }
    lcd.print(meutexto);
    meutexto = "";
  }*/
  
}





















void serialEvent() {
  if (Serial.available()) {
    delay(100);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Mails");
    lcd.setCursor(0, 1);
    while (Serial.available() > 0) {
      lcd.write(Serial.read());
    }
  }
}
