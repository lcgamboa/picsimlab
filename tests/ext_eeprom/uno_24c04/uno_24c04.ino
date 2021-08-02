#include "Wire.h"

//24c04 test

#define EEPROM_I2C_ADDRESS 0x50

void setup()
{
  Wire.begin();
  Serial.begin(9600);
  int address = 0;
  byte val;
  byte readVal;
  byte sum = 0;
  int line = 10;

  Serial.print("Erasing...");
  for (address = 0; address < 512; address++ )
  {
    writeAddress(address, 0xAA);
    Serial.print(".");
    line++;
    if (line > 63)
    {
      Serial.println("");
      line = 0;
    }
  }
  
  Serial.println("");
  Serial.print("Writing...");
  line=10;
  int errors = 0;
  for (address = 0; address < 512; address++ )
  {
    val = 0xFF^address;
    writeAddress(address, val);
    readVal = readAddress(address);

    if (val != readVal)
    {
      Serial.print("Error addr:");
      Serial.print(address);
      Serial.print("  ");
      Serial.print(val);
      Serial.print(" != ");
      Serial.println(readVal);
      errors++;
    }
    else
    {
      Serial.print(".");
      line++;
      if (line > 63)
      {
        Serial.println("");
        line = 0;
      }
    }
    sum = sum + readVal;
  }
  Serial.println("");

  Serial.print("checksum: ");
  Serial.println(sum);

  Serial.print("Errors: ");
  Serial.println(errors);
}

void loop()
{

}

void writeAddress(int address, byte val)
{
  if (address > 255)
  {
    Wire.beginTransmission(EEPROM_I2C_ADDRESS | 1); //page1
  }
  else
  {
    Wire.beginTransmission(EEPROM_I2C_ADDRESS);//page0
  }
  Wire.write((int)(address & 0xFF)); // LSB

  Wire.write(val);
  Wire.endTransmission();
  delay(5);
}

byte readAddress(int address)

{
  byte rData = 0xFF;
  if (address > 255)
  {
    Wire.beginTransmission(EEPROM_I2C_ADDRESS | 1); //page1
  }
  else
  {
    Wire.beginTransmission(EEPROM_I2C_ADDRESS);//page0
  }
  Wire.write((int)(address & 0xFF)); // LSB
  Wire.endTransmission();
  if (address > 255)
  {
    Wire.requestFrom(EEPROM_I2C_ADDRESS | 1, 1);; //page1
  }
  else
  {
    Wire.requestFrom(EEPROM_I2C_ADDRESS, 1);//page0
  }
  rData = Wire.read();
  return rData;
}
