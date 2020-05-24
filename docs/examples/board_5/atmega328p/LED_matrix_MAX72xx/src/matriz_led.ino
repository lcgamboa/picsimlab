/*
   Exemplo PONG
   Livro Arduino Básico de Michael McRoberts
*/
 
//inclui biblioteca LedControl:http://playground.arduino.cc/Main/LedControl
#include "LedControl.h"
 
LedControl myMatrix = LedControl(12, 11, 10, 1); // cria uma instância de uma Matriz
                                                 //pino 12 - DIN
                                                 //pino 11 - CLK
                                                 //pino 10 - CS
int column = 1, row = random(8)+1;  // decide em que ponto a bola deve iniciar
int directionX = 1, directionY = 1; // certifica-se de que ela vai primeiro da esquerda para a direita
int paddle1 = 5, paddle1Val;        // pino e valor do potenciômetro
int speed = 300;                    // velocidade
int counter = 0, mult = 10;         // variáveis auxiliares para controle de velocidade
 
 
void setup() {
  myMatrix.shutdown(0, false); // habilita o display
  myMatrix.setIntensity(0, 1); // define o brilho dos leds
  myMatrix.clearDisplay(0);    // limpa o display
  randomSeed(analogRead(0));   //
}
 
 
void loop() {
  
  paddle1Val = analogRead(paddle1);              //le valor do potenciometro        
  paddle1Val = map(paddle1Val, 200, 1024, 1,6);  //mapeia valor para  entre 1 e 6
 
  column += directionX;     //atualiza valor da coluna
  row += directionY;        //atualiza valor da linha
 
  //verifica se boa atingiu a raquete
  if (column == 6 && directionX == 1 && (paddle1Val == row || paddle1Val+1 == row 
|| paddle1Val+2 == row))
  {
    directionX = -1; //rebate bola
  }
 
  //verica se bola atingiu paredes: topo, base ou lateral esquerda
  if (column == 0 && directionX == -1 ) {directionX = 1;}
  if (row == 7 && directionY == 1 ) {directionY = -1;}
  if (row == 0 && directionY == -1 ) {directionY = 1;}
 
  //caso tenha atingido a coluna 7, significa que ultrapassou a raquete
  if (column == 7) { oops(); }  //chama a função oops, para finalizar o jogo
 
 
  myMatrix.clearDisplay(0); // limpa a tela para o próximo quadro de animação
 
  myMatrix.setLed(0, column, row, HIGH);            //desenha bola na linha e coluna atual
 
  //desenha raquete na coluna 7
  myMatrix.setLed(0, 7, paddle1Val, HIGH);            
  myMatrix.setLed(0, 7, paddle1Val+1, HIGH);
  myMatrix.setLed(0, 7, paddle1Val+2, HIGH);
 
  if (!(counter % mult)) {speed -= 5; mult * mult;}
  delay(speed);
  counter++;
}
 
void oops() 
{
  for (int x=0; x<3; x++) 
  {
    myMatrix.clearDisplay(0);
    delay(250);
    for (int y=0; y<8; y++) 
    {
      myMatrix.setRow(0, y, 255);
    }
    delay(250);
  }
  // reinicia todos os valores
  counter=0; 
  speed=300;
  column=1;
  row = random(8)+1; // escolhe uma nova posição inicial
}
