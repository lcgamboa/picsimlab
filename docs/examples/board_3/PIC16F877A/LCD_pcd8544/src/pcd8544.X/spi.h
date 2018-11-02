//SPI library for PIC16F877A
//by R. Pelayo

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef XC_HEADER_TEMPLATE_H
#define	XC_HEADER_TEMPLATE_H

#include <xc.h> // include processor files - each processor file is guarded.  

typedef enum
{
  MASTER_OSC_DIV4  = 0b00100000,
  MASTER_OSC_DIV16 = 0b00100001,
  MASTER_OSC_DIV64 = 0b00100010,
  MASTER_TMR2_DIV2 = 0b00100011,
  SLAVE_SS_EN      = 0b00100100,
  SLAVE_SS_DIS     = 0b00100101
}Spi_Mode;

typedef enum
{
  SAMPLE_MIDDLE = 0b00000000,
  SAMPLE_END    = 0b10000000
}Spi_Data_Sample;

typedef enum
{
  IDLE_HIGH = 0b00001000,
  IDLE_LOW  = 0b00000000
}Spi_Clock_Pol;

typedef enum
{
  IDLE_TO_ACTIVE = 0b00000000,
  ACTIVE_TO_IDLE = 0b01000000
}Spi_Clock_Edge;

 #define spi_clk   RC3   // pino de clock
 #define spi_dta   RC5   // pino de dados
 //#define spi_cs    RC2   // pino de seleção
 #define spi_reset RC1   // pino de reset

 #define CLK0 spi_clk=0;
 #define CLK1 spi_clk=1;
 #define SDA0 spi_dta=0;
 #define SDA1 spi_dta=1;

void spiBegin(Spi_Mode sMode, Spi_Data_Sample sDataSample, Spi_Clock_Edge sClockEdge, Spi_Clock_Pol sClockPol)
{
  TRISC5 = 0; //MOSI
  TRISC3 = 0; //CLK
}

void spiWrite(char b){

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


#endif	/* XC_HEADER_TEMPLATE_H */