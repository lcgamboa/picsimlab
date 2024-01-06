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

static int in_out_test(const char* tname, const char* fname, const int start_delay_sec = 1,
                       const unsigned long inter_delay_us = 1000000L) {
    int pwm;
    int state;
    printf("test %s\n", tname);

    if (!test_load(fname)) {
        return 0;
    }

    sleep(start_delay_sec);

    for (int i = 0; i < 3; i++) {
        if (!test_send_rcmd("set part[02].in[00] 1")) {
            printf("Error send rcmd \n");
            test_end();
            return 0;
        }
        usleep(inter_delay_us);

        if (!test_send_rcmd("get part[01].out[03]")) {
            printf("Error send rcmd \n");
            test_end();
            return 0;
        }
        sscanf(test_get_cmd_resp() + 22, "%i", &state);

        if (state < 50) {
            printf("Failed in Button Test \n");
            test_end();
            return 0;
        }

        if (!test_send_rcmd("set part[02].in[00] 0")) {
            printf("Error send rcmd \n");
            test_end();
            return 0;
        }
        usleep(inter_delay_us);

        if (!test_send_rcmd("get part[01].out[03]")) {
            printf("Error send rcmd \n");
            test_end();
            return 0;
        }
        sscanf(test_get_cmd_resp() + 22, "%i", &state);

        if (state > 150) {
            printf("Failed in Button Test \n");
            test_end();
            return 0;
        }
    }

    for (int i = 0; i < 3; i++) {
        if (!test_send_rcmd("set part[00].in[00] 0")) {
            printf("Error send rcmd \n");
            test_end();
            return 0;
        }
        usleep(inter_delay_us);

        if (!test_send_rcmd("get part[01].out[01]")) {
            printf("Error send rcmd \n");
            test_end();
            return 0;
        }
        sscanf(test_get_cmd_resp() + 22, "%i", &pwm);

        if (pwm > 5) {
            printf("Failed in PWM Test Set 0 Get %i\n", pwm);
            test_end();
            return 0;
        }

        if (!test_send_rcmd("set part[00].in[00] 100")) {
            printf("Error send rcmd \n");
            test_end();
            return 0;
        }
        usleep(inter_delay_us);

        if (!test_send_rcmd("get part[01].out[01]")) {
            printf("Error send rcmd \n");
            test_end();
            return 0;
        }
        sscanf(test_get_cmd_resp() + 22, "%i", &pwm);

        if ((pwm < 95) || (pwm > 105)) {
            printf("Failed in PWM Test Set 100 Get %i\n", pwm);
            test_end();
            return 0;
        }

        if (!test_send_rcmd("set part[00].in[00] 200")) {
            printf("Error send rcmd \n");
            test_end();
            return 0;
        }
        usleep(inter_delay_us);

        if (!test_send_rcmd("get part[01].out[01]")) {
            printf("Error send rcmd \n");
            test_end();
            return 0;
        }
        sscanf(test_get_cmd_resp() + 22, "%i", &pwm);

        if (pwm < 195) {
            printf("Failed in PWM Test Set 200 Get %i\n", pwm);
            test_end();
            return 0;
        }
    }

    return test_end();
}

static int test_IN_OUT_ESP32(void* arg) {
    return in_out_test("SPI ESP32", "in_out/in_out_esp32.pzw", 5);
}
register_test("IN/OUT ESP32", test_IN_OUT_ESP32, NULL);

static int test_IN_OUT_ESP32C3(void* arg) {
    return in_out_test("SPI ESP32C3", "in_out/in_out_esp32c3.pzw", 1, 3500000L);
}
register_test("IN/OUT ESP32C3", test_IN_OUT_ESP32C3, NULL);

static int test_IN_OUT_AVR(void* arg) {
    return in_out_test("SPI AVR", "in_out/in_out_uno.pzw");
}
register_test("IN/OUT AVR", test_IN_OUT_AVR, NULL);

static int test_IN_OUT_STM32(void* arg) {
    return in_out_test("SPI STM32", "in_out/in_out_stm32.pzw", 1, 2000000L);
}
register_test("IN/OUT STM32", test_IN_OUT_STM32, NULL);

static int test_IN_OUT_PIC18F(void* arg) {
    return in_out_test("SPI PIC18F", "in_out/in_out_pic18.pzw", 2);
}
register_test("IN/OUT PIC18F", test_IN_OUT_PIC18F, NULL);

static int test_IN_OUT_PIC18FGP(void* arg) {
    return in_out_test("SPI PIC18FGP", "in_out/in_out_pic18gp.pzw", 2);
}
register_test("IN/OUT PIC18F GP", test_IN_OUT_PIC18FGP, NULL);
