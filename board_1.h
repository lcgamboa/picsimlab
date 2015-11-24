/* ########################################################################

   PICsimLab - PIC laboratory simulator

   ########################################################################

   Copyright (c) : 2010-2015  Luis Claudio Gambôa Lopes

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

#ifndef BOARD_1_H
#define	BOARD_1_H

#include<lxrad/lxrad.h>

class cboard_1:public board
{
  private:
     int p_BT1; 
     int p_BT2;  
     int p_BT3; 
     int p_BT4;     
     int jmp[1];
     unsigned int lm[50]; //luminosidade media
     unsigned int lm1[50]; //luminosidade media display
     unsigned int lm2[50]; //luminosidade media display
 
   public:
      cboard_1(void);
      ~cboard_1(void);
      void Draw(_pic *pic, CDraw *draw,double scale);
      String GetSupportedDevices(void){return wxT("PIC16F628A,PIC16F648A,");};
      String GetPictureFileName(void){return wxT("picsimlab1.png");};
      String GetInputMapFile(void){return wxT("input1.map");};
      String GetOutputMapFile(void){return wxT("output1.map");};
      void Reset(_pic *pic);
      void MouseButtonPress(_pic *pic, uint button, uint x, uint y,uint state);
      void MouseButtonRelease(_pic *pic, uint button, uint x, uint y,uint state);
      void KeyPress(_pic *pic, uint key, uint x, uint y,uint mask);
      void KeyRelease(_pic *pic, uint key, uint x, uint y,uint mask);
      void WritePreferences(void);
      void ReadPreferences(char *name,char *value);
      unsigned short get_in_id(char * name);
      unsigned short get_out_id(char * name);
};





#endif	/* BOARD_1_H */

