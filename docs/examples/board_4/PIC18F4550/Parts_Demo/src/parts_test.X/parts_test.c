#include<xc.h>
#include"servo.h"
#include"atraso.h"
#include"adc.h"
#include"lcd.h"
#include"itoa.h"

void setup() {
    servo_init();
    adc_init();
    lcd_init();
    lcd_cmd(L_NCR);
    lcd_str("Parts Demo");
}

unsigned int value;
char buff[6];

int i=0;
int angle;
void sleep(void) {
   
    if(i < 50)
    {
        i=0;
        value = adc_sample(0);
        angle= value * 180l / 1023;
        servo_pos(angle);
        lcd_cmd(L_L2);
        lcd_str(itoa(value, buff));
        lcd_cmd(L_L2+8);
        lcd_str(itoa(angle, buff));
    }
    else
    {
        i++;
    }
        //atraso_ms(1);
    
}

#define NSTEPS 50

void loop() {
    int i;

    for (i = 0; i < NSTEPS; i++) {
        PORTB = 0x10 | (PORTB & 0x0F);
        sleep();
        PORTB = 0x20 | (PORTB & 0x0F);
        sleep();
        PORTB = 0x40 | (PORTB & 0x0F);
        sleep();
        PORTB = 0x80 | (PORTB & 0x0F);
        sleep();
    }

    for (i = 0; i < NSTEPS; i++) {
        PORTB = 0x80 | (PORTB & 0x0F);
        sleep();
        PORTB = 0x40 | (PORTB & 0x0F);
        sleep();
        PORTB = 0x20 | (PORTB & 0x0F);
        sleep();
        PORTB = 0x10 | (PORTB & 0x0F);
        sleep();
    }

    for (i = 0; i < NSTEPS; i++) {
        PORTB = 0x10 | (PORTB & 0x0F);
        sleep();
        PORTB = 0x30 | (PORTB & 0x0F);
        sleep();
        PORTB = 0x20 | (PORTB & 0x0F);
        sleep();
        PORTB = 0x60 | (PORTB & 0x0F);
        sleep();
        PORTB = 0x40 | (PORTB & 0x0F);
        sleep();
        PORTB = 0xC0 | (PORTB & 0x0F);
        sleep();
        PORTB = 0x80 | (PORTB & 0x0F);
        sleep();
        PORTB = 0x90 | (PORTB & 0x0F);
        sleep();
    }

    for (i = 0; i < NSTEPS; i++) {
        PORTB = 0x90 | (PORTB & 0x0F);
        sleep();
        PORTB = 0x80 | (PORTB & 0x0F);
        sleep();
        PORTB = 0xC0 | (PORTB & 0x0F);
        sleep();
        PORTB = 0x40 | (PORTB & 0x0F);
        sleep();
        PORTB = 0x60 | (PORTB & 0x0F);
        sleep();
        PORTB = 0x20 | (PORTB & 0x0F);
        sleep();
        PORTB = 0x30 | (PORTB & 0x0F);
        sleep();
        PORTB = 0x10 | (PORTB & 0x0F);
        sleep();
    }
}
