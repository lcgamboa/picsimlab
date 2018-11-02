/* ########################################################################

   PICsim - PIC simulator http://sourceforge.net/projects/picsim/

   ########################################################################

   Copyright (c) : 2010  Luis Claudio Gambôa Lopes

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

unsigned char display7s(unsigned char v)
{

unsigned char ret;

  switch(v)
  {
    case 0:
      ret= 0x3F;
      break;
    case 1:
      ret= 0x06;
      break;
    case 2:
      ret= 0x5B;
      break;
    case 3:
      ret= 0x4F;
      break;
    case 4:
      ret= 0x66;
      break;
    case 5:
      ret= 0x6D;
      break;
    case 6:
      ret= 0x7D;
      break;
    case 7:
      ret= 0x07;
      break;
    case 8:
      ret= 0x7F;
      break;
    case 9:
      ret= 0x6F;
      break;
    case 10:
      ret= 0x77;
      break;
    case 11:
      ret= 0x7c;
      break;
    case 12:
      ret= 0x58;
      break;
    case 13:
      ret= 0x5E;
      break;
    case 14:
      ret= 0x79;
      break;
    case 15:
      ret= 0x71;
      break;
    default:
      ret= 0;
      break;
  }

 return ret;

}


