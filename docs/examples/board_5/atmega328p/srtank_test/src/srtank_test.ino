
#include <LiquidCrystal.h>

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

//entradas digitais
#define _VENT 0x00
#define _VSAI 0x01
#define _AQUE 0x02
#define _RESF 0x03
#define _AGIT 0x04
//saídas digitais
#define _BALT 0x10
#define _BBAI 0x11
#define _TMIN 0x12
#define _TMAX 0x13
//entradas analógicas
#define _ATMI 0x20
#define _ATMA 0x21
//saídas analógicas
#define _VOLU 0x30
#define _TEMP 0x31


#define _LIGA 0x01
#define _DESLIGA 0x00
unsigned int valori;

void setup(void) {
    Serial.begin(9600);
    lcd.begin(16, 2);
    lcd.setCursor(0, 0);
    lcd.print(" Connect srtank");
    lcd.setCursor(0, 1);
    lcd.print(" and press PD6");

    pinMode(6,INPUT);
    
    while(digitalRead(6) == 0)
    {
        delay(1);
    }
    
    Serial.write(0xFF); //sincroniza comunicação
    Serial.write(0xFF);
    Serial.write(0xFF);

    Serial.write(_ATMA);
    valori = 28;
    Serial.write((valori & 0xFF00) >> 8);
    Serial.write(valori & 0x00FF);
}

unsigned char valorh, valorl;
char buff[6];

void volume(void) {
    Serial.write(_VOLU);
    while(!Serial.available());
    valorh = Serial.read();
    while(!Serial.available());
    valorl = Serial.read();
    valori = (valorh << 8) | valorl;
    lcd.setCursor(0, 1);
    lcd.print("v=");
    lcd.print(valori);
    lcd.print("l  t=");
    
    Serial.write(_TEMP);
    while(!Serial.available());
    valorh = Serial.read();
    while(!Serial.available());
    valorl = Serial.read();
    valori = (valorh << 8) | valorl;
    lcd.print(valori);
    lcd.print("C ");
}


unsigned char valor;

void loop(void) {
    Serial.write(_VENT);
    Serial.write(_LIGA);

    Serial.write(_VSAI);
    Serial.write(_DESLIGA);

    lcd.setCursor(0, 0);
    lcd.print("Filling ");
    //delay(1000);

    do {
        Serial.write(_BALT);
        while(!Serial.available());
        valor = Serial.read();
        volume();
    } while (valor == 0);

    Serial.write(_VENT);
    Serial.write(_DESLIGA);

    Serial.write(_AQUE);
    Serial.write(_LIGA);

    lcd.setCursor(0, 0);
    lcd.print("Heating ");
    //delay(1000);

    do {
        Serial.write(_TMAX);
        while(!Serial.available());
        valor = Serial.read();
        volume();
    } while (valor == 0);

    Serial.write(_AQUE);
    Serial.write(_DESLIGA);

    Serial.write(_VSAI);
    Serial.write(_LIGA);

    lcd.setCursor(0, 0);
    lcd.print("Emptying");
    //delay(1000);

    do {
        Serial.write(_BBAI);
        while(!Serial.available()); 
        valor = Serial.read();
        volume();
    } while (valor == 1);
}
