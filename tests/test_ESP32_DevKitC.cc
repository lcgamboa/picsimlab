/* ########################################################################

   PICsimLab - PIC laboratory simulator

   ########################################################################

   Copyright (c) : 2020-2023  Luis Claudio Gamboa Lopes

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

static int test_ESP32_DevKitC(void* arg) {
    printf("test ESP32_DevKitC \n");

    if (!test_load("ESP32_DevKitC/ESP32_DevKitC.pzw")) {
        return 0;
    }

    char buff[256];
    // read serial console
    while (test_serial_recv_str(buff, 256, 1000)) {
        // printf("%s\n", buff);
    }

    // check the last line
    if (strcmp(buff, "Hello World! esp32\r")) {
        printf("Failed in ESP32_DeviktC serial\n");
        test_end();
        return 0;
    }

    return test_end();
}

register_test("ESP32_DevKitC", test_ESP32_DevKitC, NULL);
