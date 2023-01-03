/* ########################################################################

   PICsimLab - PIC laboratory simulator

   ########################################################################

   Copyright (c) : 2020-2022  Luis Claudio Gamboa Lopes

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

   For e-mail suggestions :  lcgamboa@yahoo.com
   ######################################################################## */

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "tests.h"

static int bmp280_test(const char *tname, const char *fname, const char *resp)
{
    printf("test %s\n", tname);

    if (!test_load(fname))
    {
        return 0;
    }

    char buff[256];
    // read serial console
    while (test_serial_recv_str(buff, 256, 1000))
    {
        // printf("%s\n", buff);
    }

    sleep(1);
    test_serial_send('a');
    sleep(1);
    test_serial_recv_str(buff, 256, 1000);

    // check the last line
    if (strcmp(buff, resp))
    {
        printf("[%s]\n", buff);
        printf("Failed in %s \n", tname);
        test_end();
        return 0;
    }

    return test_end();
}

static int test_SPI_ESP32(void *arg)
{
    return bmp280_test("SPI ESP32", "spi/esp32_bmp280_spi.pzw", "T= 35.00 P= 780.00\r");
}
register_test("SPI ESP32", test_SPI_ESP32, NULL);

static int test_I2C_ESP32(void *arg)
{
    return bmp280_test("I2C ESP32", "i2c/esp32_bmp280_i2c.pzw", "T= 35.34 P= 774.83\r");
}
register_test("I2C ESP32", test_I2C_ESP32, NULL);

static int test_SPI_AVR(void *arg)
{
    return bmp280_test("SPI AVR", "spi/uno_bmp280_spi.pzw", "T= 35.00 P= 780.00\r");
}
register_test("SPI AVR", test_SPI_AVR, NULL);

static int test_I2C_AVR(void *arg)
{
    return bmp280_test("I2C AVR", "i2c/uno_bmp280_i2c.pzw", "T= 35.00 P= 780.00\r");
}
register_test("I2C AVR", test_I2C_AVR, NULL);

static int test_SPI_STM32(void *arg)
{
    return bmp280_test("SPI STM32", "spi/stm32_bmp280_spi.pzw", "T= 35.00 P= 780.00\r");
}
register_test("SPI STM32", test_SPI_STM32, NULL);

static int test_I2C_STM32(void *arg)
{
    return bmp280_test("I2C STM32", "i2c/stm32_bmp280_i2c.pzw", "T= 35.00 P= 780.00\r");
}
register_test("I2C STM32", test_I2C_STM32, NULL);

static int test_SPI_PIC18F(void *arg)
{
    return bmp280_test("SPI PIC18F", "spi/pic18f_bmp280_spi.pzw", "T=  35.00 P= 780.00");
}
register_test("SPI PIC18F", test_SPI_PIC18F, NULL);

static int test_I2C_PIC18F(void *arg)
{
    return bmp280_test("I2C PIC18F", "i2c/pic18f_bmp280_i2c.pzw", "T=  35.00 P= 780.00");
}
register_test("I2C PIC18F", test_I2C_PIC18F, NULL);