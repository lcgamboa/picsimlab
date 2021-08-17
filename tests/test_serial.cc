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
test_serial(void * arg)
{
 char data = '!';
 char ret;
 printf ("test serial \n");

 if (!test_load ("serial/serial_uno.pzw"))
  {
   return 0;
  }

 for (int i = 0; i < 94; i++)
  {
   if (!test_serial_send (data))
    {
     printf ("Error on send\n");
     test_end ();
     return 0;
    }

   if (!test_serial_recv_wait (&ret, 500))
    {
     printf ("Error on recv\n");
     test_end ();
     return 0;
    }
   printf ("send %c -> %c \n", data, ret);

   if (data != ret)
    {
     printf ("Error on data value \n");
     test_end ();
     return 0;
    }

   data++;
  }

 return test_end ();
}


register_test("Uno Serial", test_serial, NULL);


