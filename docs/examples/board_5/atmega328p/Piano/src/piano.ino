//Arduino Piano
/*
https://www.instructables.com/id/Arduino-Tutorial-Mini-Piano/

Visit the Channel for more interesting projects

https://www.youtube.com/channel/UCks-9JSnVb22dlqtMgPjrlg

*/

#define T_C 262
#define T_D 294
#define T_E 330
#define T_F 349
#define T_G 392
#define T_A 440
#define T_B 493

const int C = 10;
const int D = 9;
const int E = 8;
const int F = 7;
const int G = 6;
const int A = 5;
const int B = 4;

const int Buzz = 11;
const int LED = 13;

void setup()
{
  pinMode(LED, OUTPUT);
  pinMode(C, INPUT);
  //digitalWrite(C,HIGH);
  
  pinMode(D, INPUT);
  //digitalWrite(D,HIGH);
  
  pinMode(E, INPUT);
  //digitalWrite(E,HIGH);
  
  pinMode(F, INPUT);
  //digitalWrite(F,HIGH);
  
  pinMode(G, INPUT);
  //digitalWrite(G,HIGH);
  
  pinMode(A, INPUT);
  //digitalWrite(A,HIGH);
  
  pinMode(B, INPUT);
  //digitalWrite(B,HIGH);

   digitalWrite(LED,LOW);
}

void loop()
{
 
  while(digitalRead(C) == LOW)
  {
    tone(Buzz,T_C);
    digitalWrite(LED,HIGH);
  }

  while(digitalRead(D) == LOW)
  {
    tone(Buzz,T_D);
    digitalWrite(LED,HIGH);
  }

  while(digitalRead(E) == LOW)
  {
    tone(Buzz,T_E);
    digitalWrite(LED,HIGH);
  }

  while(digitalRead(F) == LOW)
  {
    tone(Buzz,T_F);
    digitalWrite(LED,HIGH);
  }

  while(digitalRead(G) == LOW)
  {
    tone(Buzz,T_G);
    digitalWrite(LED,HIGH);
  }

  while(digitalRead(A) == LOW)
  {
    tone(Buzz,T_A);
    digitalWrite(LED,HIGH);
  }

  while(digitalRead(B) == LOW)
  {
    tone(Buzz,T_B);
    digitalWrite(LED,HIGH);
  }

  noTone(Buzz);
  digitalWrite(LED,LOW);

}
