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
#include<string.h>

#include"tests.h"

static int
test_ext_eeprom(void * arg)
{
 char ret[256];

 printf ("test external eeprom i2c \n");

 if (!test_load ("ext_eeprom/extee_uno.pzw"))
  {
   return 0;
  }


 do
  {

   if (!test_serial_recv_str (ret, 256, 1000))
    {
     printf ("Error on recv\n");
     test_end ();
     return 0;
    }
   printf ("%s\n", ret);

  }
 while (!strstr (ret, "Errors: "));

 if (strncmp (ret, "Errors: 0", 9))
  {
   test_end ();
   return 0;
  }

 return test_end ();
}


register_test("Uno ext. eeprom 24C04", test_ext_eeprom, NULL);


