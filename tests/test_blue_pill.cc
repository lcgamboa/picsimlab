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

static int test_Blue_Pill(void *arg) {
  int state;
  int cont = 0;

  printf("test Blue_Pill \n");

  if (!test_load("Blue_Pill/Blue_Pill.pzw")) {
    return 0;
  }

  char buff[256];
  // clear serial console
  while (test_serial_recv_str(buff, 256, 1000)) {
  }

  cont = 0;
  do {
    usleep(1000);
    if (!test_send_rcmd("get board.out[02]")) {
      printf("Error send rcmd \n");
      test_end();
      return 0;
    }
    sscanf(test_get_cmd_resp() + 22, "%i", &state);
    cont++;
  } while (state && (cont < 1000));

  if (cont >= 1000) {
    printf("Failed in LED Test \n");
    test_end();
    return 0;
  }

  cont = 0;
  do {
    usleep(1000);
    if (!test_send_rcmd("get board.out[02]")) {
      printf("Error send rcmd \n");
      test_end();
      return 0;
    }
    sscanf(test_get_cmd_resp() + 22, "%i", &state);
    cont++;
  } while (!state && (cont < 1000));

  if (cont >= 1000) {
    printf("Failed in LED Test \n");
    test_end();
    return 0;
  }

  return test_end();
}

register_test("Blue_Pill", test_Blue_Pill, NULL);
