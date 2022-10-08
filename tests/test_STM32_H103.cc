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
#include <unistd.h>

#include "tests.h"

static int test_STM32_H103(void *arg) {
  int state;

  printf("test STM32_H103 \n");

  if (!test_load("STM32_H103/STM32_H103.pzw")) {
    return 0;
  }

  char buff[256];
  // clear serial console
  while (test_serial_recv_str(buff, 256, 1000)) {
  };

  for (int i = 0; i < 5; i++) {
    if (!test_send_rcmd("set board.in[02] 1")) {
      printf("Error send rcmd \n");
      test_end();
      return 0;
    }
    usleep(500000);

    if (!test_send_rcmd("get board.out[03]")) {
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

    if (!test_send_rcmd("set board.in[02] 0")) {
      printf("Error send rcmd \n");
      test_end();
      return 0;
    }
    usleep(500000);

    if (!test_send_rcmd("get board.out[03]")) {
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

  return test_end();
}

register_test("STM32_H103", test_STM32_H103, NULL);
