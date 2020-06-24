/* MCP23S17 library example */

#include <MCP23S17.h>

#ifdef __PIC32MX__
// chipKIT uses the DSPI library instead of the SPI library as it's better
#include <DSPI.h>
DSPI0 SPI;
#else
// Everytying else uses the SPI library
#include <SPI.h>
#endif

const uint8_t chipSelect = 10;

// Create an object for each chip
// Bank 0 is address 0
// Bank 1 is address 1.
// Increase the addresses by 2 for each BA value.

MCP23S17 Bank1(&SPI, chipSelect, 0);

void setup() {
  Bank1.begin();
  Bank1.pinMode(15, OUTPUT);
  Bank1.pinMode(14, OUTPUT);
  Bank1.pinMode(13, OUTPUT);
  Bank1.pinMode(12, OUTPUT);
  Bank1.pinMode(11, OUTPUT);
  Bank1.pinMode(10, OUTPUT);
  Bank1.pinMode(9, OUTPUT);
  Bank1.pinMode(8, OUTPUT);
  Bank1.pinMode(7, OUTPUT);
  Bank1.pinMode(6, OUTPUT);
  Bank1.pinMode(5, OUTPUT);
  Bank1.pinMode(4, OUTPUT);
  Bank1.pinMode(3, OUTPUT);
  Bank1.pinMode(2, OUTPUT);
  Bank1.pinMode(1, OUTPUT);
  Bank1.pinMode(0, OUTPUT);
}

unsigned short val = 1;
void loop() {
  Bank1.digitalWrite(15, (val & 0x8000) > 0 );
  Bank1.digitalWrite(14, (val & 0x4000) > 0 );
  Bank1.digitalWrite(13, (val & 0x2000) > 0 );
  Bank1.digitalWrite(12, (val & 0x1000) > 0 );
  Bank1.digitalWrite(11, (val & 0x0800) > 0 );
  Bank1.digitalWrite(10, (val & 0x0400) > 0 );
  Bank1.digitalWrite(9, (val & 0x0200) > 0 );
  Bank1.digitalWrite(8, (val & 0x0100) > 0 );
  Bank1.digitalWrite(7, (val & 0x0080) > 0 );
  Bank1.digitalWrite(6, (val & 0x0040) > 0 );
  Bank1.digitalWrite(5, (val & 0x0020) > 0 );
  Bank1.digitalWrite(4, (val & 0x0010) > 0 );
  Bank1.digitalWrite(3, (val & 0x0008) > 0 );
  Bank1.digitalWrite(2, (val & 0x0004) > 0 );
  Bank1.digitalWrite(1, (val & 0x0002) > 0 );
  Bank1.digitalWrite(0, (val & 0x0001) > 0 );

  val = val << 1;
  if (val == 0)val = 1;

  delay(1000);
}
