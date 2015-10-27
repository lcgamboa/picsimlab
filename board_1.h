
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
     CButton *button;
 
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

