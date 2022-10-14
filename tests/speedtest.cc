/* ########################################################################

   PICsimLab - PIC laboratory simulator

   ########################################################################

   Copyright (c) : 2020-2021  Luis Claudio Gamboa Lopes

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
#include <unistd.h>

#include "tests.h"

static int test_speedtest(void *arg) {

  char cmd[100];
  float speed[2];

  printf("test test_speedtest \n");

  if (!test_load("blink/blink.pzw")) {
    return 0;
  }

  for (float clk = 4; clk <= 64; clk += 4) {

    sprintf(cmd, "clk %2.1f", clk);
    if (!test_send_rcmd(cmd)) {
      printf("Error send rcmd \n");
      test_end();
      return 0;
    }

    // wait stabilization
    usleep(100000);

    speed[0] = 0;
    do {
      usleep(2000000);

      if (!test_send_rcmd("sim")) {
        printf("Error send rcmd \n");
        test_end();
        return 0;
      }

      speed[1] = speed[0];
      sscanf(test_get_cmd_resp() + 20, "%f", &speed[0]);

    } while (speed[1] != speed[0]);
    printf("CLK %6.2f MHz  speed %4.2f \n", clk, speed[0]);
  }
  return test_end();
}

register_test("Speed Test", test_speedtest, NULL);
