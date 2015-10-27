/* 
 * File:   board.h
 * Author: gamboa
 *
 * Created on 17 de Outubro de 2015, 09:49
 */

#ifndef BOARD_H
#define	BOARD_H
    
#include<lxrad/lxrad.h>    
#include "../picsim.h"    
    
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
      virtual void Draw(_pic *pic, CDraw *draw,double scale)=0; //Called ever 100ms to draw board
      virtual String GetSupportedDevices(void)=0;//Return a list of supported microcontrollers
      virtual String GetPictureFileName(void)=0; //Return the filename of board picture 
      virtual String GetInputMapFile(void)=0;    //Return the filename of board picture input map  
      virtual String GetOutputMapFile(void)=0;   //Return the filename of board picture output map  
      virtual void Reset(_pic *pic)=0;           //Reset board status
      virtual void MouseButtonPress(_pic *pic,uint button, uint x, uint y,uint state)=0;    //Event on the board
      virtual void MouseButtonRelease(_pic *pic,uint button, uint x, uint y,uint state)=0;  //Event on the board
      virtual void KeyPress(_pic *pic,uint key, uint x, uint y,uint mask)=0;  //Event on the board
      virtual void KeyRelease(_pic *pic,uint key, uint x, uint y,uint mask)=0;//Event on the board
      virtual void OnShow(void){};               //Event on the board when windows is show or resized
      virtual void RefreshStatus(_pic *pic);   //Called ever 1s to refresh status
      virtual void WritePreferences(void){};     //Called to save board preferences in configuration file
      virtual void ReadPreferences(char *name,char *value){}; //Called whe configuration file load  preferences 
      virtual unsigned short get_in_id(char * name)=0; //return the input ids numbers of names used in input map
      virtual unsigned short get_out_id(char * name)=0; //return the output ids numbers of names used in output map

      board(void);           //Called once on board creation
      virtual ~board(void);  //Called once on board destruction 
      
      int proc;             //ID of processor in use
 protected:
      input_t  input[100];  //input map elements
      output_t output[100]; //output map elements 
      int inputc; //input map elements counter 
      int outputc; //output map elements counter   
      void ReadMaps(void); //read maps 
 private:      
      void ReadInputMap(String fname);
      void ReadOutputMap(String fname);
};



#endif	/* BOARD_H */

