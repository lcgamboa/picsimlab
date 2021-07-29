void setup() {

  // put your setup code here, to run once:
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
  pinMode(A3, INPUT);
  pinMode(A4, INPUT);
  pinMode(A5, INPUT);

  pinMode(3, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);

}

int value;
void loop() {
  value = analogRead(A0);
  analogWrite(3, value >> 2);
  
  value = analogRead(A1);
  analogWrite(5, value >> 2);

  value = analogRead(A2);
  analogWrite(6, value >> 2);

  value = analogRead(A3);
  analogWrite(9, value >> 2);

  value = analogRead(A4);
  analogWrite(10, value >> 2);

  value = analogRead(A5);
  analogWrite(11, value >> 2);

  delay(100);
}
