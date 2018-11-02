#include"config.h"
#include"lcd.h"
#include"itoa.h"

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
    pinMode(_RC7, INPUT);
    lcd_init();
    lcd_cmd(L_CLR);
    lcd_cmd(L_NCR);
    lcd_cmd(L_L1);
    lcd_str(" Connect srtank");
    lcd_cmd(L_L2);
    lcd_str(" and press RB0");
    
    while(digitalRead(_RB0) == 1)
    {
        delay(1);
    }
    
    Serial_write(0xFF); //sincroniza comunicação
    Serial_write(0xFF);
    Serial_write(0xFF);

    Serial_write(_ATMA);
    valori = 28;
    Serial_write((valori & 0xFF00) >> 8);
    Serial_write(valori & 0x00FF);
}

unsigned char valorh, valorl;
char buff[6];

void volume(void) {
    Serial_write(_VOLU);
    valorh = Serial_read(0);
    valorl = Serial_read(0);
    valori = (valorh << 8) | valorl;
    lcd_cmd(L_L2);
    lcd_str("v=");
    lcd_str(itoa(valori, buff)+1);
    lcd_str("l  t=");
    
    Serial_write(_TEMP);
    valorh = Serial_read(0);
    valorl = Serial_read(0);
    valori = (valorh << 8) | valorl;
    lcd_str(itoa(valori, buff)+3);
    lcd_str("C");
}


unsigned char valor;

void loop(void) {
    Serial_write(_VENT);
    Serial_write(_LIGA);

    Serial_write(_VSAI);
    Serial_write(_DESLIGA);

    lcd_cmd(L_L1);
    lcd_str("Filling ");
    //delay(1000);

    do {
        Serial_write(_BALT);
        valor = Serial_read(0);
        volume();
    } while (valor == 0);

    Serial_write(_VENT);
    Serial_write(_DESLIGA);

    Serial_write(_AQUE);
    Serial_write(_LIGA);

    lcd_cmd(L_L1);
    lcd_str("Heating ");
    //delay(1000);

    do {
        Serial_write(_TMAX);
        valor = Serial_read(0);
        volume();
    } while (valor == 0);

    Serial_write(_AQUE);
    Serial_write(_DESLIGA);

    Serial_write(_VSAI);
    Serial_write(_LIGA);

    lcd_cmd(L_L1);
    lcd_str("Emptying");
    //delay(1000);

    do {
        Serial_write(_BBAI);
        valor = Serial_read(0);
        volume();
    } while (valor == 1);
}