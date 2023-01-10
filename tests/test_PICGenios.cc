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
#include <time.h>
#include <unistd.h>

#include "tests.h"

#define RB1 2

static char LCD_L1[20];
static char LCD_L2[20];

static int ReadLCD(void) {
    if (!test_send_rcmd("get board.out[48]")) {
        printf("Error send rcmd \n");
        return 0;
    }
    strncpy(LCD_L1, test_get_cmd_resp() + 23, 16);
    strncpy(LCD_L2, test_get_cmd_resp() + 64, 16);
    return 1;
}

static unsigned int ssegdisp;

static int Read7Seg(void) {
    int val[4];

    if (!test_send_rcmd("info")) {
        printf("Error send rcmd \n");
        return 0;
    }

    if (!test_send_rcmd("get board.out[56]")) {
        printf("Error send rcmd \n");
        return 0;
    }
    sscanf(test_get_cmd_resp() + 19, "%i", &val[0]);

    if (!test_send_rcmd("get board.out[64]")) {
        printf("Error send rcmd \n");
        return 0;
    }
    sscanf(test_get_cmd_resp() + 19, "%i", &val[1]);

    if (!test_send_rcmd("get board.out[72]")) {
        printf("Error send rcmd \n");
        return 0;
    }
    sscanf(test_get_cmd_resp() + 19, "%i", &val[2]);

    if (!test_send_rcmd("get board.out[80]")) {
        printf("Error send rcmd \n");
        return 0;
    }
    sscanf(test_get_cmd_resp() + 19, "%i", &val[3]);

    ssegdisp = (val[0] * 1000) + (val[1] * 100) + (val[2] * 10) + (val[3]);
    return 1;
}

static int test_picgenios(void* arg) {
    char ret[256];
    char cmd[256];
    char data, datar;

    printf("test PICGenios \n");

    if (!test_load("PICGenios/PICGenios.pzw")) {
        return 0;
    }

    char buff[256];

    if (!testPressButton(RB1)) {
        test_end();
        return 0;
    }
    // LCD test
    usleep(500000);

    if (!ReadLCD()) {
        test_end();
        return 0;
    }

    if (strncmp(LCD_L1, "   Test LCD L1  ", 16) || strncmp(LCD_L2, "   Test LCD L2  ", 16)) {
        printf("Failed in LCD test\n");
        test_end();
        return 0;
    }

    if (!testPressButton(RB1)) {
        test_end();
        return 0;
    }
    // 7 segments

    usleep(500000);

    if (!Read7Seg()) {
        test_end();
        return 0;
    }

    if (ssegdisp != 2222) {
        printf("Failed in 7 segments test 1\n");
        test_end();
        return 0;
    }

    if (!testPressButton(RB1)) {
        test_end();
        return 0;
    }

    usleep(500000);

    if (!Read7Seg()) {
        test_end();
        return 0;
    }

    if (ssegdisp != 8888) {
        printf("Failed in 7 segments test 2\n");
        test_end();
        return 0;
    }

    if (!testPressButton(RB1)) {
        test_end();
        return 0;
    }

    // Test  serial
    // TX

    if (!test_serial_vt_init(0)) {
        printf("Error init vt \n");
        test_end();
        return 0;
    }
    // clear serial console
    while (test_serial_recv_str(buff, 256, 1000)) {
    };

    usleep(500000);
    if (!testPressButton(RB1)) {
        test_end();
        return 0;
    }

    // RX
    usleep(500000);

    if (!test_serial_recv_str(ret, 256, 1000)) {
        printf("Error on recv\n");
        test_end();
        return 0;
    }

    if (!test_serial_recv_str(ret, 256, 1000)) {
        printf("Error on recv\n");
        test_end();
        return 0;
    }

    data = 'a';
    for (int i = 0; i < 16; i++) {
        test_serial_send(data);
        test_serial_recv_wait(&datar, 1000);

        if (data != datar) {
            printf("Failed Serial Test %c != %c\n", data, datar);
            test_end();
            return 0;
        }

        data++;
    }

    if (!test_serial_vt_end()) {
        printf("Error end vt \n");
        test_end();
        return 0;
    }

    // Test ADC 1
    usleep(500000);

    for (int i = 0; i < 250; i += 50) {
        sprintf(cmd, "set board.in[21] %i", i);
        if (!test_send_rcmd(cmd)) {
            printf("Error send rcmd \n");
            test_end();
            return 0;
        }
        usleep(200000);
        if (!ReadLCD()) {
            test_end();
            return 0;
        }
        sprintf(cmd, "      %03.1fV      ", (50 * i / 200) / 10.0);
        if (strncmp(LCD_L2, cmd, 16)) {
            printf("Failed in ADC 1 test [%s] != [%s]\n", LCD_L2, cmd);
            test_end();
            return 0;
        }
    }

    if (!testPressButton(RB1)) {
        test_end();
        return 0;
    }

    // Test ADC2
    usleep(500000);

    for (int i = 0; i < 250; i += 50) {
        sprintf(cmd, "set board.in[20] %i", i);
        if (!test_send_rcmd(cmd)) {
            printf("Error send rcmd \n");
            test_end();
            return 0;
        }
        usleep(200000);
        if (!ReadLCD()) {
            test_end();
            return 0;
        }
        sprintf(cmd, "      %03.1fV      ", (50 * i / 200) / 10.0);
        if (strncmp(LCD_L2, cmd, 16)) {
            printf("Failed in ADC 2 test [%s] != [%s]\n", LCD_L2, cmd);
            test_end();
            return 0;
        }
    }

    if (!testPressButton(RB1)) {
        test_end();
        return 0;
    }

    // Test TEMP

    float temp0, temph, tempc;

    usleep(500000);

    usleep(200000);
    if (!ReadLCD()) {
        test_end();
        return 0;
    }

    sscanf(LCD_L2 + 5, "%f", &temp0);

    if (!testPressButton(RB1)) {
        test_end();
        return 0;
    }

    // Test heater
    usleep(1000000);

    if (!ReadLCD()) {
        test_end();
        return 0;
    }

    sscanf(LCD_L2 + 5, "%f", &temph);

    if (!testPressButton(RB1)) {
        test_end();
        return 0;
    }

    // Test Cooler
    usleep(1000000);
    if (!ReadLCD()) {
        test_end();
        return 0;
    }
    sscanf(LCD_L2 + 2, "%f", &tempc);

    if ((temph <= temp0) || (tempc >= temph)) {
        printf("Failed in temperature test. t0=%4.1f  th=%4.1f, tc=%4.1f\n", temp0, temph, tempc);
        test_end();
        return 0;
    }

    if (!testPressButton(RB1)) {
        test_end();
        return 0;
    }

    // Test RTC
    struct tm dtime;

    usleep(500000);
    if (!ReadLCD()) {
        test_end();
        return 0;
    }
    int day, month, year;
    sscanf(LCD_L1 + 5, "%d/%d/%d", &day, &month, &year);

    time_t systime = time(NULL);
#ifdef _WIN_
    localtime_s(&dtime, &systime);
#else
    localtime_r(&systime, &dtime);
#endif

    if ((dtime.tm_mday != day) || ((dtime.tm_mon + 1) != month) || ((dtime.tm_year % 100) != year)) {
        printf("Failed in %s [%02i/%02i/%02i != %02i/%02i/%02i] \n", LCD_L1, day, month, year, dtime.tm_mday,
               dtime.tm_mon + 1, dtime.tm_year);
        test_end();
        return 0;
    }

    if (!testPressButton(RB1, 0, 500)) {
        test_end();
        return 0;
    }

    // Test Keyboard

    for (int i = 0; i < 12; i++) {
        usleep(100000);

        if (!testPressButton(i + 8, 1, 100)) {
            test_end();
            return 0;
        }
    }
    usleep(100000);
    if (!ReadLCD()) {
        test_end();
        return 0;
    }

    if (strncmp(LCD_L2, " 123456789:0<   ", 16)) {
        printf("Failed in keyboard test [%s]\n", LCD_L2);
        test_end();
        return 0;
    }
    if (!testPressButton(RB1)) {
        test_end();
        return 0;
    }

    // Test EEPROM INT
    usleep(500000);

    if (!testPressButton(RB1)) {
        test_end();
        return 0;
    }

    usleep(500000);
    if (!ReadLCD()) {
        test_end();
        return 0;
    }

    if (strncmp(LCD_L2, "       OK       ", 16)) {
        printf("Failed in EEPROM INT test [%s]\n", LCD_L2);
        test_end();
        return 0;
    }

    if (!testPressButton(RB1)) {
        test_end();
        return 0;
    }

    // Test EEPROM EXT
    usleep(500000);

    if (!testPressButton(RB1)) {
        test_end();
        return 0;
    }

    usleep(500000);
    if (strncmp(LCD_L2, "       OK       ", 16)) {
        printf("Failed in EEPROM EXT test [%s]\n", LCD_L2);
        test_end();
        return 0;
    }

    if (!testPressButton(RB1)) {
        test_end();
        return 0;
    }

    // END
    usleep(500000);

    return test_end();
}

register_test("PICGenios", test_picgenios, NULL);
