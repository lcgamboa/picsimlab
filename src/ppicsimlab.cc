/* ########################################################################

   PICsimLab - PIC laboratory simulator

   ########################################################################

   Copyright (c) : 2010-2021  Luis Claudio Gambôa Lopes

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

//#include<lxrad.h>
#include"picsimlab1.h"
#include"picsimlab2.h"
#include"picsimlab3.h"
#include"picsimlab4.h"
#include"picsimlab5.h"

//Program____________________________________________________________

Initialize
{
  Application->Start ();
  Window4.SetVisible (0);
  Window5.SetVisible (0);
  Application->ACreateWindow (&Window1);
  Application->ACreateWindow (&Window2);
  Application->ACreateWindow (&Window3);
  Application->ACreateWindow (&Window4);
  Application->ACreateWindow (&Window5);
  Application->Load ();

  return 0;
}

