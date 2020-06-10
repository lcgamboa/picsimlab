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
    
    /**
     * @brief PART class
     *
     * class definition of parts used in picsimlab spare parts window.
     */
class part
{
  public:
      virtual String GetName(void)=0; ///< Return the name of part 
      virtual void Draw(void)=0;         ///< Called ever 100ms to draw part
      virtual void PreProcess (void){};  ///< Called every start of CPU process
      virtual void Process (void){};     ///< Called every CPU step
      virtual void PostProcess (void){}; ///< Called every end of CPU process
      virtual String GetPictureFileName(void)=0; ///< Return the filename of part picture 
      virtual String GetInputMapFile(void)=0;    ///< Return the filename of part picture input map  
      virtual String GetOutputMapFile(void)=0;   ///< Return the filename of part picture output map  
      virtual String GetPropertiesWindowFile(void)=0; ///< Return the filename of properties window XML file
      virtual void Reset(void){};   ///< Reset part status
      virtual void EvMouseButtonPress(uint button, uint x, uint y,uint state){};    ///< Event on the part
      virtual void EvMouseButtonRelease(uint button, uint x, uint y,uint state){};  ///< Event on the part
      virtual void EvMouseMove(uint button, uint x, uint y,uint state){};  ///< Event on the part
      virtual void EvKeyPress(uint key, uint mask){};  ///< Event on the part
      virtual void EvKeyRelease(uint key,uint mask){};///< Event on the part
      virtual String WritePreferences(void)=0;   ///< Called to save part preferences in configuration file
      virtual void ReadPreferences(String value)=0; ///< Called whe configuration file load  preferences 
      virtual unsigned short get_in_id(char * name)=0; ///< return the input ids numbers of names used in input map
      virtual unsigned short get_out_id(char * name)=0; ///< return the output ids numbers of names used in output map  
      virtual void ConfigurePropertiesWindow(CPWindow *  WProp)=0;///< Called to configure the properties window
      virtual void ReadPropertiesWindow(CPWindow *  WProp)=0; ///< Called when properties window close 
      virtual void ComboChange(String value){}; ///< Used by properties window combos
      virtual void filedialog_EvOnClose(int retId){}; ///< Used by properties window filedialogs
      part();           ///< Called once on part creation
      virtual ~part(void){};  ///< Called once on part destruction
      lxBitmap * GetBitmap(void){return Bitmap;}; ///< Return the Bitmap of part
      int GetX(void){return X;}; ///< Return X position of part
      int GetY(void){return Y;}; ///< Return Y position of part
      void SetX(int x){X=x;}; ///< Set X position of part
      void SetY(int y){Y=y;}; ///< Set X position of part
      unsigned int GetWidth(void){return Width;}; ///< Return width of part
      unsigned int GetHeight(void){return Height;}; ///< Return height of part
      int PointInside(int x, int y); ///< Return if point x,y is inside of part
      int id; ///< part ID
 protected:
      input_t  input[100];  ///< input map elements
      output_t output[100]; ///< output map elements 
      int inputc;           ///< input map elements counter 
      int outputc;          ///< output map elements counter   
      void ReadMaps(void);  ///< read maps 
      unsigned int Height;  ///< Height of part
      unsigned int Width;   ///< Width of part
      int X;                ///< X position of part
      int Y;                ///< Y position of part
      lxBitmap * Bitmap;    ///< Internal Bitmap   
      CCanvas canvas;       ///< Internal Canvas to draw in bitmap
      unsigned int refresh; ///< redraw is needed 
 private:      
      void ReadInputMap(String fname); ///< Read the Input Map
      void ReadOutputMap(String fname);///< Read the Output Map
};

#endif	/* PART_H */

