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

#ifndef BOARD_H
#define	BOARD_H
    
#include <lxrad/lxrad.h>    
#include <picsim/picsim.h>    
    
typedef struct
{
unsigned int x1;
unsigned int x2;
unsigned int y1;
unsigned int y2;
char name[10];
unsigned short id;
}input_t;

typedef struct
{
unsigned int x1;
unsigned int x2;
unsigned int y1;
unsigned int y2;
unsigned int r;
char name[10];
unsigned short id;
//int lval;
}output_t;    

class board
{
  public:
      virtual void Draw(CDraw *draw,double scale)=0; //Called ever 100ms to draw board 
      virtual void Run_CPU(void)=0; //Paralle thread called ever 100ms to run cpu code
      virtual String GetSupportedDevices(void)=0;//Return a list of supported microcontrollers
      virtual String GetPictureFileName(void)=0; //Return the filename of board picture 
      virtual String GetInputMapFile(void)=0;    //Return the filename of board picture input map  
      virtual String GetOutputMapFile(void)=0;   //Return the filename of board picture output map  
      virtual void Reset(void)=0;           //Reset board status
      virtual void MouseButtonPress(uint button, uint x, uint y,uint state)=0;    //Event on the board
      virtual void MouseButtonRelease(uint button, uint x, uint y,uint state)=0;  //Event on the board
      virtual void KeyPress(uint key, uint x, uint y,uint mask)=0;  //Event on the board
      virtual void KeyRelease(uint key, uint x, uint y,uint mask)=0;//Event on the board
      virtual void OnShow(void){};               //Event on the board when windows is show or resized
      virtual void RefreshStatus(void);   //Called ever 1s to refresh status
      virtual void WritePreferences(void){};   //Called to save board preferences in configuration file
      virtual void ReadPreferences(char *name,char *value){}; //Called whe configuration file load  preferences 
      virtual unsigned short get_in_id(char * name)=0; //return the input ids numbers of names used in input map
      virtual unsigned short get_out_id(char * name)=0; //return the output ids numbers of names used in output map
      virtual void board_Event(CControl * control){};
      
      board(void);           //Called once on board creation
      virtual ~board(void);  //Called once on board destruction 
      void SetUseOscilloscope(int uo); //Enable/disable oscilloscope measurement
      int proc;             //ID of processor in use
 protected:
      input_t  input[100];  //input map elements
      output_t output[100]; //output map elements 
      int inputc;           //input map elements counter 
      int outputc;          //output map elements counter   
      void ReadMaps(void); //read maps 
      int use_oscope;
 private:      
      void ReadInputMap(String fname);
      void ReadOutputMap(String fname);
};



#endif	/* BOARD_H */

