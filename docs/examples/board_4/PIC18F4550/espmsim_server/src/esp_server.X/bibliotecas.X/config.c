
// PIC18F4550 Configuration Bit Settings

// 'C' source line config statements

// CONFIG1L
#pragma config PLLDIV = 2       // PLL Prescaler Selection bits (Divide by 2 (8 MHz oscillator input))
#pragma config CPUDIV = OSC1_PLL2// System Clock Postscaler Selection bits ([Primary Oscillator Src: /1][96 MHz PLL Src: /2])
#pragma config USBDIV = 2       // USB Clock Selection bit (used in Full-Speed USB mode only; UCFG:FSEN = 1) (USB clock source comes from the 96 MHz PLL divided by 2)

// CONFIG1H
#pragma config FOSC = HS        // Oscillator Selection bits (HS oscillator (HS))
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enable bit (Fail-Safe Clock Monitor disabled)
#pragma config IESO = OFF       // Internal/External Oscillator Switchover bit (Oscillator Switchover mode disabled)

// CONFIG2L
#pragma config PWRT = OFF       // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOR = OFF        // Brown-out Reset Enable bits (Brown-out Reset disabled in hardware and software)
#pragma config BORV = 3         // Brown-out Reset Voltage bits (Minimum setting 2.05V)
#pragma config VREGEN = OFF     // USB Voltage Regulator Enable bit (USB voltage regulator disabled)

// CONFIG2H
#pragma config WDT = OFF        // Watchdog Timer Enable bit (WDT disabled (control is placed on the SWDTEN bit))
#pragma config WDTPS = 32768    // Watchdog Timer Postscale Select bits (1:32768)

// CONFIG3H
#pragma config CCP2MX = ON      // CCP2 MUX bit (CCP2 input/output is multiplexed with RC1)
#pragma config PBADEN = ON      // PORTB A/D Enable bit (PORTB<4:0> pins are configured as analog input channels on Reset)
#pragma config LPT1OSC = OFF    // Low-Power Timer 1 Oscillator Enable bit (Timer1 configured for higher power operation)
#pragma config MCLRE = ON       // MCLR Pin Enable bit (MCLR pin enabled; RE3 input pin disabled)

// CONFIG4L
#pragma config STVREN = OFF     // Stack Full/Underflow Reset Enable bit (Stack full/underflow will not cause Reset)
#pragma config LVP = OFF        // Single-Supply ICSP Enable bit (Single-Supply ICSP disabled)
#pragma config ICPRT = OFF      // Dedicated In-Circuit Debug/Programming Port (ICPORT) Enable bit (ICPORT disabled)
#pragma config XINST = OFF      // Extended Instruction Set Enable bit (Instruction set extension and Indexed Addressing mode disabled (Legacy mode))

// CONFIG5L
#pragma config CP0 = OFF        // Code Protection bit (Block 0 (000800-001FFFh) is not code-protected)
#pragma config CP1 = OFF        // Code Protection bit (Block 1 (002000-003FFFh) is not code-protected)
#pragma config CP2 = OFF        // Code Protection bit (Block 2 (004000-005FFFh) is not code-protected)
#pragma config CP3 = OFF        // Code Protection bit (Block 3 (006000-007FFFh) is not code-protected)

// CONFIG5H
#pragma config CPB = OFF        // Boot Block Code Protection bit (Boot block (000000-0007FFh) is not code-protected)
#pragma config CPD = OFF        // Data EEPROM Code Protection bit (Data EEPROM is not code-protected)

// CONFIG6L
#pragma config WRT0 = OFF       // Write Protection bit (Block 0 (000800-001FFFh) is not write-protected)
#pragma config WRT1 = OFF       // Write Protection bit (Block 1 (002000-003FFFh) is not write-protected)
#pragma config WRT2 = OFF       // Write Protection bit (Block 2 (004000-005FFFh) is not write-protected)
#pragma config WRT3 = OFF       // Write Protection bit (Block 3 (006000-007FFFh) is not write-protected)

// CONFIG6H
#pragma config WRTC = OFF       // Configuration Register Write Protection bit (Configuration registers (300000-3000FFh) are not write-protected)
#pragma config WRTB = OFF       // Boot Block Write Protection bit (Boot block (000000-0007FFh) is not write-protected)
#pragma config WRTD = OFF       // Data EEPROM Write Protection bit (Data EEPROM is not write-protected)

// CONFIG7L
#pragma config EBTR0 = OFF      // Table Read Protection bit (Block 0 (000800-001FFFh) is not protected from table reads executed in other blocks)
#pragma config EBTR1 = OFF      // Table Read Protection bit (Block 1 (002000-003FFFh) is not protected from table reads executed in other blocks)
#pragma config EBTR2 = OFF      // Table Read Protection bit (Block 2 (004000-005FFFh) is not protected from table reads executed in other blocks)
#pragma config EBTR3 = OFF      // Table Read Protection bit (Block 3 (006000-007FFFh) is not protected from table reads executed in other blocks)

// CONFIG7H
#pragma config EBTRB = OFF      // Boot Block Table Read Protection bit (Boot block (000000-0007FFh) is not protected from table reads executed in other blocks)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include "config.h"




void setup();
void loop();

void main()
{
    ADCON1=0x0C;//AN0,AN1 e AN2 ativados
    TRISA=0xC7; //0b11000111    
    TRISB=0x07;
    TRISC=0x01;
    TRISD=0x00;
    TRISE=0x00; 
    
    adc_init();
    
    PWM1_Init(5000);
   
    
    serial_init();
    
    setup();
    
    
    
    while(1)
    {
        loop();
    }
}



unsigned char digitalRead(unsigned char pin)
{
    unsigned char val = 1<<(0x0F & pin);
    switch((pin & 0xF0)>>4)
    {
       case 0:
         return (PORTA & val) == val;
       case 1:
         return (PORTB & val) == val;  
       case 2:
         return (PORTC & val) == val;
        case 3:
         return (PORTD & val) == val;  
       case 4:
         return (PORTE & val) == val;      
    }
    return 0;
}

void pinMode(unsigned char pin, unsigned char value)
{
    unsigned char val = 1<<(0x0F & pin);
    switch((pin & 0xF0)>>4)
    {
       case 0:
         if(value)
             TRISA|=val;
         else
             TRISA&=~val;
         break;
       case 1:
         if(value)
             TRISB|=val;
         else
             TRISB&=~val;
         break;
       case 2:
         if(value)
             TRISC|=val;
         else
             TRISC&=~val;
         break;
       case 3:
         if(value)
             TRISD|=val;
         else
             TRISD&=~val;
         break;
       case 4:
         if(value)
             TRISE|=val;
         else
             TRISE&=~val;
         break;
    }
}


void digitalWrite(unsigned char pin, unsigned char value)
{
    unsigned char val = 1<<(0x0F & pin);
    switch((pin & 0xF0)>>4)
    {
       case 0:
         if(value)
             PORTA|=val;
         else
             PORTA&=~val;
         break;
       case 1:
         if(value)
             PORTB|=val;
         else
             PORTB&=~val;
         break;
       case 2:
         if(value)
             PORTC|=val;
         else
             PORTC&=~val;
         break;
       case 3:
         if(value)
             PORTD|=val;
         else
             PORTD&=~val;
         break;
       case 4:
         if(value)
             PORTE|=val;
         else
             PORTE&=~val;
         break;
    }
}


void analogWrite(unsigned char pin, unsigned char value)
{
    if(pin == 0x22) //RC2
    {
      PWM1_Start();
      PWM1_Set_Duty(value);
    }
}