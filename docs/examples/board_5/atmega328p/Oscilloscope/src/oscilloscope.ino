/* 
This is my version of the Arduino oscilloscope code.  This file is strictly the display code for 
one analog channel.  It will soon be merged into my data logger project.

This is shared so others may incorporate the display code into their projects.  It has been modified to 
play nice with the Adafruit 5110 LCD library.

This is a work in progress but hopefully it will help someone else by providing
a base to start and work from.

Please check out my Youtube videos here and consider a thumbs up if this helped you!
Youtube : http://www.youtube.com/mkmeorg

Full Arduino Tutorial Playlist here: https://www.youtube.com/playlist?list=PLxyM2a_cfnzjXms2WRO9OdAr-_7K-Ve2Z

Original code comment text:
###########################################################
  Title:       Arduino Uno 3V Oscilloscope
  Purpose:     Use a Nokia 5110 LCD screen with the arduino
  Created by:  Brian O'Dell from the Fileark Arduino Oscilloscope.
  Note:        Please reuse, repurpose, and redistribute this code.
  Note:        This code uses the Adafruit PDC8544 LCD library  
###########################################################
https://github.com/Megamemnon/Arduino-Oscilloscope/blob/master/Uno3VOScope.ino
 */
#include "Adafruit_GFX.h"
//#include "PCD8544.h"
#include "Adafruit_PCD8544.h"

// pin 7 - Serial clock out (SCLK)
// pin 6 - Serial data out (DIN)
// pin 5 - Data/Command select (D/C)
// pin 4 - LCD chip select (CS)
// pin 3 - LCD reset (RST)
Adafruit_PCD8544 display = Adafruit_PCD8544(7, 6, 5, 4, 3);

//Analog Pins
int channelAI = A0;      // probe
int delayAI = A1;       // delay potentiometer

float delayVariable =5; // 10 seems good for decent update- bit fast
float scale = 0;
int xCounter = 0;
int yPosition = 0;
int readings[85];
int counter = 0;

void setup() {
analogWrite(11,220);// PWM of LCD backlight but ebay unit is backwards- 
  //must go high + cycle to dim 
  //Very Dim=230
  
   
  Serial.begin(128000); // opens serial port, sets data rate to 9600 bps
  Serial.println("CLEARDATA");
  Serial.println("LABEL,TIME,A0,A1,A2,A3,A4,A5");
  
  display.begin();//Display code
  display.setContrast(50);//Nokia 5110 works best at 50- no more flicker
  delay(1000);
  display.clearDisplay();     // clears the screen and buffer
  display.setTextSize(1);     // set text size
  display.setTextColor(BLACK);
  //delay(1000);
  
  
   // Splash Personal- taken from example code
  display.setTextSize(1);
  display.setTextColor(BLACK);
  display.setCursor(0,0);
  display.println("   Eric's");
  display.println("");
  display.println("   Ardruino");
  display.println("     Data");
  display.println("    Logger");
  display.display();
  delay(1000);
  display.clearDisplay();     // clears the screen and buffer
 
}


void loop() 
{  
  
  //delayVariable = analogRead(delayAI);
  //delayVariable = (delayVariable/10);
  scale = 47.0/1023.0; 
  // commented out above delay items as no pot/encoder installed yet
  
  
  //record readings
  for(xCounter = 0; xCounter < 85; xCounter += 1)
    {                                 
      yPosition = analogRead(channelAI);
      readings[xCounter] = (yPosition*scale);
      delay (delayVariable);
    }
  
  display.clearDisplay();

  //Draw Voltage Ref Lines
  display.drawLine( 10, 0, 10, 47, BLACK);
  display.drawLine( 5, 47-(.166 *1023.0 * scale), 10, 47-(.166 *1023.0 * scale), BLACK);
  display.drawLine( 0, 47-(.33 *1023.0 * scale), 10, 47-(.33 *1023.0 * scale), BLACK);
  display.drawLine( 5, 47-(.5 *1023.0 * scale), 10, 47-(.5 *1023.0 * scale), BLACK);
  display.drawLine( 0, 47-(.66 *1023.0 * scale), 10, 47-(.66 *1023.0 * scale), BLACK);
  display.drawLine( 5, 47-(.84 *1023.0 * scale), 10, 47-(.84 *1023.0 * scale), BLACK);

  for(xCounter = 0; xCounter < 85; xCounter += 1)
    {
       display.drawPixel(xCounter, 47-readings[xCounter], BLACK);
       if(xCounter>1){
         display.drawLine(xCounter-1, 47-readings[xCounter-1], xCounter, 47-readings[xCounter], BLACK);
       }
    }
  
  display.display();     
} 
