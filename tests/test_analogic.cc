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

#include<stdio.h>
#include<unistd.h>

#include"tests.h"

static int
test_analogic(void * arg)
{
 const int pwm_pins[6] = {5, 11, 12, 15, 16, 17};
 char cmd[100];
 int value;

 printf ("test analogic \n");

 if (!test_load ("analogic/analogic_uno.pzw"))
  {
   return 0;
  }

 for (float v = 0; v <= 5.0; v += 0.5)
  {
   int ival = v * 40;

   for (int i = 0; i < 6; i++)
    {
     sprintf (cmd, "set apin[%02i] %5.3f", i + 23, v);
     if (!test_send_rcmd (cmd))
      {
       printf ("Error send rcmd \n");
       test_end ();
       return 0;
      }
    }

   //wait stabilization
   usleep (500000);

   for (int i = 0; i < 6; i++)
    {
     sprintf (cmd, "get pinm[%02i]", pwm_pins[i]);
     if (!test_send_rcmd (cmd))
      {
       printf ("Error send rcmd \n");
       test_end ();
       return 0;
      }
     sscanf (test_get_cmd_resp () + 8, "%03d", &value);
     printf ("pwm[%02i] %i = %i\n", pwm_pins[i], ival, value);

     if ((value < (ival - 4)) || (value > (ival + 4)))
      {
       printf ("Error value \n");
       test_end ();
       return 0;
      }
    }
  }
 return test_end ();
}


register_test(analogic, test_analogic, NULL);


