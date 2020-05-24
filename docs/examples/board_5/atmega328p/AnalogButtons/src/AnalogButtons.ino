#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define ANALOG_PIN A0
LiquidCrystal_I2C lcd(0x27, 20, 4); // set the LCD address to 0x27 for a 16 chars and 2 line display

void setup() {
  lcd.init();                      // initialize the lcd
  lcd.backlight();
  lcd.clear();
  lcd.home();
  lcd.print("Press Buttons");
}

/*
  0      0
  0.6    123
  1.2    246
  1.9    389
  2.5    512
  3.1    634
  3.8    777
  4.4    900
*/


void loop() {
  unsigned int value = analogRead(ANALOG_PIN);
  
  if ((value >= 0) && (value < 50))
  {
    lcd.setCursor(0, 1);
    lcd.print("Button 1 Press");
  }
  else if ((value > 73) && (value < 173))
  {
    lcd.setCursor(0, 1);
    lcd.print("Button 2 Press");
  }
  else if ((value > 196) && (value < 296))
  {
    lcd.setCursor(0, 1);
    lcd.print("Button 3 Press");
  }
  else if ((value > 339) && (value < 439))
  {
    lcd.setCursor(0, 1);
    lcd.print("Button 4 Press");
  }
  else if ((value > 462) && (value < 562))
  {
    lcd.setCursor(0, 1);
    lcd.print("Button 5 Press");
  }
  else if ((value > 584) && (value < 684))
  {
    lcd.setCursor(0, 1);
    lcd.print("Button 6 Press");
  }
  else if ((value > 727) && (value < 827))
  {
    lcd.setCursor(0, 1);
    lcd.print("Button 7 Press");
  }
  else if ((value > 850) && (value < 950))
  {
    lcd.setCursor(0, 1);
    lcd.print("Button 8 Press");
  }
  else
  {
    lcd.setCursor(0, 1);
    lcd.print("               ");
  }
}


