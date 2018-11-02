
#define __16f628a
#include "xc.h"
#include "tsmtypes.h"




// CONFIG
#pragma config FOSC = HS        // Oscillator Selection bits (HS oscillator: High-speed crystal/resonator on RA6/OSC2/CLKOUT and RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = ON       // RA5/MCLR/VPP Pin Function Select bit (RA5/MCLR/VPP pin function is MCLR)
#pragma config BOREN = OFF      // Brown-out Detect Enable bit (BOD disabled)
#pragma config LVP = OFF        // Low-Voltage Programming Enable bit (RB4/PGM pin has digital I/O function, HV on MCLR must be used for programming)
#pragma config CPD = OFF        // Data EE Memory Code Protection bit (Data memory code protection off)
#pragma config CP = OFF         // Flash Program Memory Code Protection bit (Code protection off)


// If KHZ is not specified by the makefile, assume it to be 4 MHZ
#ifndef KHZ
#define KHZ 4000
#endif

// These are fixed.  The 16f628a can only use these as transmit and recieve.
#define TX_PORT	2
#define RX_PORT	1
#define TX_BIT	(1<<TX_PORT)
#define RX_BIT	(1<<RX_PORT)


// Twiddle these as you like BUT remember that not all values work right!
// See the datasheet for what values can work with what clock frequencies.
#define	BAUD 19200
#define BAUD_HI 1

// This section calculates the proper value for SPBRG from the given
// values of BAUD and BAUD_HI.  Derived from Microchip's datasheet.
#if	(BAUD_HI == 1)
#define	BAUD_FACTOR	(16L*BAUD)
#else
#define	BAUD_FACTOR	(64L*BAUD)
#endif

#define SPBRG_VALUE	(unsigned char)(((KHZ*1000L)-BAUD_FACTOR)/BAUD_FACTOR)



 #define spi_clk   RB7   // pino de clock
 #define spi_dta   RB6   // pino de dados
 #define spi_cs    RB5   // pino de seleção
 #define spi_reset RB4   // pino de reset


 #define CS0 spi_cs=0;
 #define CS1 spi_cs=1;
 #define CLK0 spi_clk=0;
 #define CLK1 spi_clk=1;
 #define SDA0 spi_dta=0;
 #define SDA1 spi_dta=1;
 #define RESET0 spi_reset=0;
 #define RESET1 spi_reset=1;

 byte n;
 byte s1,s2;
 byte r,g,b,x,y;


static void sendCMD(byte cmd);
static void sendData(byte cmd);
static void shiftBits(byte b);
static void setPixel(byte r,byte g,byte b);



static unsigned char c,d,e;

static void delay_ms(unsigned char loop)
{
  for(c=0;c<loop;c++)
     for(d=0;d<255;d++)
		for(e=0;e<4;e++);
   
}

static void shiftBits(byte b) {

  CLK0
  if ((b&128)!=0) SDA1 else SDA0
  CLK1

  CLK0
  if ((b&64)!=0) SDA1 else SDA0
  CLK1

  CLK0
  if ((b&32)!=0) SDA1 else SDA0
  CLK1

  CLK0
  if ((b&16)!=0) SDA1 else SDA0
  CLK1

  CLK0
  if ((b&8)!=0) SDA1 else SDA0
  CLK1

  CLK0
  if ((b&4)!=0) SDA1 else SDA0
  CLK1

  CLK0
  if ((b&2)!=0) SDA1 else SDA0
  CLK1

  CLK0
  if ((b&1)!=0) SDA1 else SDA0
  CLK1

}

//envia cmd
static void sendCMD(byte datax) {

  CLK0
  SDA0                                                 //1 para cmd
  CLK1

  shiftBits(datax);
}



static void sendData(byte datax) {

  CLK0
  SDA1                                                 //1 for param
  CLK1

  shiftBits(datax);
  
}

static void setPixel(byte r_,byte g_,byte b_) {
  r=r_;
  g=g_;
  b=b_;
  if (n==0) {
    s1=(r & 0xF0) | (g>>4);
    s2=(b & 0xF0);
    n=1;
  } else {
    n=0;
    sendData(s1);
    sendData(s2|(r>>4));
    sendData((g&0xF0) | (b>>4));
  }
  
}

static void putc(unsigned char datax)
{
  	while(!TRMT);	// Wait until we're free to transmit
	TXREG=datax;
}

static unsigned char getc(void)
{
	while(!RCIF);	// Wait until data recieved
	return RCREG;	// Store for later
}


static void
ponto(byte x_, byte y_,byte r_,byte g_,byte b_)
{
 x=x_;
 y=y_;
 r=r_;
 g=g_;
 b=b_;
//Column Adress Set
  sendCMD(0x2A);
  sendData(x);
  sendData(x);
  //Page Adress Set
  sendCMD(0x2B);
  sendData(y);
  sendData(y);
 //Memory Write
  sendCMD(0x2C);
   
  sendData((r & 0xF0)| (g>>4));
  sendData((b & 0xF0)| (r>>4));
  sendData((g & 0xF0)| (b>>4));
}

unsigned int i;
unsigned char ix;

void main()
{
 
 
    n=0; 
   
    TRISB=0;
	TRISB=TX_BIT|RX_BIT;	// These need to be 1 for USART to work

    SPBRG=SPBRG_VALUE ;	// Baud Rate register, calculated by macro
//    SPBRG=25;
	BRGH=BAUD_HI;

	SYNC=0;			// Disable Synchronous/Enable Asynchronous
	SPEN=1;			// Enable serial port
	TXEN=1;			// Enable transmission mode
	CREN=1;			// Enable reception mod

//printf("\n OK");
  putc('O');
  putc('K');
  putc('\n');
  putc('\r');


  CS0
  SDA0
  CLK1

  RESET1 
 delay_ms(10);	
  RESET0
 delay_ms(10);
  RESET1

  CLK1
  SDA1
  CLK1

  delay_ms(10);

   //Software Reset
  sendCMD(0x01);

  //Sleep Out
  sendCMD(0x11);

  //Booster ON
  sendCMD(0x03);

  delay_ms(10);

  //Display On
  sendCMD(0x29);

  //Normal display mode
  sendCMD(0x13);

  //Display inversion off
  sendCMD(0x20);

  //Data order
  sendCMD(0xBA);

  //Memory data access control
  sendCMD(0x36);

  //sendData(8|64);   //rgb + MirrorX
  //sendData(8|128);   //rgb + MirrorY
  //sendData(0xC8)
  sendData(0x00);

  sendCMD(0x3A);
  sendData(3);   //12-Bit per Pixel (default)

  //Set Constrast
  sendCMD(0x25);
  sendData(63);


  //Column Adress Set
  sendCMD(0x2A);
  sendData(0);
  sendData(131);

  //Page Adress Set
  sendCMD(0x2B);
  sendData(0);
  sendData(131);

  //Memory Write
  sendCMD(0x2C);

   //Test-Picture

  //red bar
  for (i=0;i<132*33;i++) {
    setPixel(255,0,0);
  }

  //green bar
  for (i=0;i<132*33;i++) {
    setPixel(0,255,0);
  }

  //blue bar
  for (i=0;i<132*33;i++) {
    setPixel(0,0,255);
  }

  //white bar
  for (i=0;i<132*33;i++) {
    setPixel(255,255,255);
  }


  //wait for RGB-Data on serial line and display on lcd

 // printf("Se chegou ate aki ta ok");
  putc('F');
  putc('I');
  putc('M');
  putc('\n');
  putc('\r');		
  /*
  while(1==1)
  {
   RB0=1;
   delay_ms(100);
   RB0=0;
  }
  */
   while(1==1) {
/*
    r = getc();
    g = getc();
    b = getc();
    setPixel(r,g,b);
    putc('.');
}
*/

  for (i=0;i<132*64;i++) 
  {
    setPixel(255,255,255);
  }

  for (i=0;i<132*64;i++) 
  {
    setPixel(0,0,0);
  }	

  for (ix=0;ix<132;ix++) 
  { 
    ponto(ix,ix,255,0,0);
	ponto(ix,100,255,255,0);
  }
  
  for (ix=0;ix<132;ix++) 
  { 
    ponto(ix,ix+2,0,255,0);
	ponto(ix,90,0,255,255);
  }

  for (ix=0;ix<132;ix++) 
  {
    ponto(ix,ix+4,0,0,255);
    ponto(ix,80,255,0,255);
  }
   //Software Reset
  sendCMD(0x01);
//Column Adress Set
  sendCMD(0x2A);
  sendData(0);
  sendData(131);
  //Page Adress Set
  sendCMD(0x2B);
  sendData(0);
  sendData(131);
 //Memory Write
  sendCMD(0x2C);

}
}

/*
	while(!RCIF);	// Wait until data recieved
		i=RCREG;	// Store for later

		while(!TRMT);	// Wait until we're free to transmit
		TXREG=i;
*/
