/*
  Analog input, analog output, serial output

  Reads an analog input pin, maps the result to a range from 0 to 255 and uses
  the result to set the pulse width modulation (PWM) of an output pin.
  Also prints the results to the Serial Monitor.

  The circuit:
  - potentiometer connected to analog pin 0.
    Center pin of the potentiometer goes to the analog pin.
    side pins of the potentiometer go to +5V and ground
  - LED connected from digital pin 9 to ground

  created 29 Dec. 2008
  modified 9 Apr 2012
  by Tom Igoe

  This example code is in the public domain.

  http://www.arduino.cc/en/Tutorial/AnalogInOutSerial
*/

// These constants won't change. They're used to give names to the pins used:
const int analogInPinR = A0;  // Analog input pin that the potentiometer is attached to
const int analogInPinG = A1;  // Analog input pin that the potentiometer is attached to
const int analogInPinB = A2;  // Analog input pin that the potentiometer is attached to
const int analogOutPinR = 5; // Analog output pin that the LED is attached to
const int analogOutPinG = 6; // Analog output pin that the LED is attached to
const int analogOutPinB = 9; // Analog output pin that the LED is attached to

int sensorValueR = 0;        // value read from the pot
int sensorValueG = 0;        // value read from the pot
int sensorValueB = 0;        // value read from the pot
int outputValueR = 0;        // value output to the PWM (analog out)
int outputValueG = 0;        // value output to the PWM (analog out)
int outputValueB = 0;        // value output to the PWM (analog out)

void setup() {
  // initialize serial communications at 9600 bps:
  Serial.begin(9600);
}

void loop() {
  // read the analog in value:
  sensorValueR = analogRead(analogInPinR);
  // map it to the range of the analog out:
  outputValueR = map(sensorValueR, 0, 1023, 0, 255);
  // change the analog out value:
  analogWrite(analogOutPinR, outputValueR);

  // read the analog in value:
  sensorValueG = analogRead(analogInPinG);
  // map it to the range of the analog out:
  outputValueG = map(sensorValueG, 0, 1023, 0, 255);
  // change the analog out value:
  analogWrite(analogOutPinG, outputValueG);

  // read the analog in value:
  sensorValueB = analogRead(analogInPinB);
  // map it to the range of the analog out:
  outputValueB = map(sensorValueB, 0, 1023, 0, 255);
  // change the analog out value:
  analogWrite(analogOutPinB, outputValueB);

  // print the results to the Serial Monitor:
  Serial.print("sensor = ");
  Serial.print(sensorValueR);
  Serial.print(",");
  Serial.print(sensorValueG);
  Serial.print(",");
  Serial.print(sensorValueB);
  Serial.print("\t output = ");
  Serial.print(outputValueR);
  Serial.print(",");
  Serial.print(outputValueG);
  Serial.print(",");
  Serial.println(outputValueB);
  
  // wait 2 milliseconds before the next loop for the analog-to-digital
  // converter to settle after the last reading:
  delay(2);
}
