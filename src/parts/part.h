/* ########################################################################

   PICsimLab - PIC laboratory simulator

   ########################################################################

   Copyright (c) : 2010-2018  Luis Claudio Gambôa Lopes

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

#ifndef PART_H
#define	PART_H
    
#include "../boards/board.h"
//#include <lxrad/lxrad.h>    
//#include <picsim/picsim.h>    
    

class part
{
  public:
      virtual String GetName(void)=0;
      virtual void Draw(void)=0; //Called ever 100ms to draw board 
      virtual void Process (void){}; //Called every CPU step
      virtual String GetPictureFileName(void)=0; //Return the filename of board picture 
      virtual String GetInputMapFile(void)=0;    //Return the filename of board picture input map  
      virtual String GetOutputMapFile(void)=0;   //Return the filename of board picture output map  
      virtual String GetPropiertiesWindowFile(void)=0;//Return the filename of propierties window XML file
      virtual void Reset(void){};           //Reset part status
      virtual void MouseButtonPress(uint button, uint x, uint y,uint state){};    //Event on the board
      virtual void MouseButtonRelease(uint button, uint x, uint y,uint state){};  //Event on the board
      virtual void MouseMove(uint button, uint x, uint y,uint state){};  //Event on the board
      virtual void KeyPress(uint key, uint x, uint y,uint mask){};  //Event on the board
      virtual void KeyRelease(uint key, uint x, uint y,uint mask){};//Event on the board
      virtual String WritePreferences(void)=0;   //Called to save part preferences in configuration file
      virtual void ReadPreferences(String value)=0; //Called whe configuration file load  preferences 
      virtual unsigned short get_in_id(char * name)=0; //return the input ids numbers of names used in input map
      virtual unsigned short get_out_id(char * name)=0; //return the output ids numbers of names used in output map
      virtual void board_Event(CControl * control){};   
      virtual void ConfigurePropiertsWindow(CPWindow *  wprop)=0;
      virtual void ReadPropiertsWindow(void)=0;
      part();           //Called once on part creation
      virtual ~part(void){};  //Called once on part destruction
      wxBitmap * GetBitmap(void){return Bitmap;};
      int GetX(void){return X;};
      int GetY(void){return Y;};
      void SetX(int x){X=x;};
      void SetY(int y){Y=y;};
      unsigned int GetWidth(void){return Width;};
      unsigned int GetHeight(void){return Height;};
      int PointInside(int x, int y);
 protected:
      input_t  input[100];  //input map elements
      output_t output[100]; //output map elements 
      int inputc;           //input map elements counter 
      int outputc;          //output map elements counter   
      void ReadMaps(void); //read maps 
      unsigned int Height;
      unsigned int Width;
      int X;
     int Y;
      wxBitmap * Bitmap; 
      CCanvas canvas; 
      unsigned int refresh;
 private:      
      void ReadInputMap(String fname);
      void ReadOutputMap(String fname);
};


#endif	/* PART_H */

