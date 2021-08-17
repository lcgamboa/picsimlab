//https://github.com/terryjmyers/PWM
#include <PWM.h>

int32_t frequency = 7812; //frequency (in Hz)

void setup() {
  InitTimersSafe();

  // put your setup code here, to run once:
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);

  SetPinFrequencySafe(9,  frequency);
  SetPinFrequencySafe(10, frequency);

  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);

}

int value;
void loop() {
  value = analogRead(A0);
  analogWrite(9, value);

  value = analogRead(A1);
  analogWrite(10, value);

  delay(100);
}
