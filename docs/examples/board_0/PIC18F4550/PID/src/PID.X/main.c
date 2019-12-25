#include "config.h"
#include "lcd.h"
#include  <math.h>
#include  <stdio.h>

#define T 0.1
#define kp 1.0
#define ki 2.0
#define kd 3.0
#define N 1000.0

float in[2], out, ap, ai[2], ad[2];
float ref, output;

void setup(void) {
    ap = 0;
    ai[0] = 0;
    ai[1] = 0;
    ad[0] = 0;
    ad[1] = 0;
    in[0] = 0;
    in[1] = 0;

    analogWrite(_RC2, 128);
    //Configura TMR0
    T0CONbits.T08BIT = 0;
    T0CONbits.T0CS = 0;
    T0CONbits.PSA = 0;
    T0CONbits.T0PS2 = 0; //divide por 16
    T0CONbits.T0PS1 = 1;
    T0CONbits.T0PS0 = 1;
    TMR0H = 0xE7; //59286=0xE796
    TMR0L = 0x96; //100ms=1us*16*(65536-59286)
    INTCONbits.GIE = 1;
    INTCONbits.T0IE = 1;
    INTCONbits.T0IF = 0;
    T0CONbits.TMR0ON = 1;

    lcd_init();
    lcd_cmd(L_NCR);

}
char buffer[10];

void loop(void) {

    lcd_cmd(L_L1);
    if (digitalRead(_RB0))
        lcd_str("Open ");
    else
        lcd_str("PID  ");

    lcd_str("ref: ");
    sprintf(buffer, "%5.2f", ref);
    lcd_str(buffer);
    lcd_cmd(L_L2);
    lcd_str("     out: ");
    sprintf(buffer, "%5.2f", output);
    //sprintf(buffer, "%5.2f", in[0]);
    lcd_str(buffer);

}

void interrupt isr(void) {
    if (INTCONbits.T0IF)//acionado a cada 0.1 s
    {
        TMR0H = 0xE7;
        TMR0L = 0x96;
        ref = 5.0 * analogRead(_AN0) / 1023.0;
        output = 5.0 * analogRead(_AN1) / 1023.0;

        in[1] = in[0];
        in[0] = ref - output;

        if (!digitalRead(_RB0)) {


            ap = kp * in[0];
            if (fabs(out) < 2.5) {
                ai[1] = ai[0];
                ai[0] = (ki * in[0] * T) + ai[1];
            }
            ad[1] = ad[0];
            ad[0] = ((kd * N * (in[0] - in[1])) + ad[1]) / (1 + (N * T));
            out = ap + ai[0] + ad[0];
            if (out > 2.5)out = 2.5;
            if (out < -2.5)out = -2.5;

            analogWrite(_RC2, (out * 51) + 127.5);
        } else {
            analogWrite(_RC2, ref * 255.0 / 5.0);
        }
        INTCONbits.T0IF = 0;
    }
}
