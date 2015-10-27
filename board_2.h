
#ifndef BOARD_2_H
#define	BOARD_2_H


class cboard_2:public board
{
  private:
    int p_CL1; 
    int p_CL2; 
    int p_CL3;    

     lcd_t lcd;

     mi2c_t mi2c;
     rtc_t rtc;
     
     int lcde;

     unsigned char clko;
     unsigned char d;
     unsigned char sda,sck;
     
     unsigned int lm[50]; //luminosidade media    
    
  public:
      cboard_2(void);
      ~cboard_2(void);
      void Draw(_pic *pic, CDraw *draw,double scale);
      String GetSupportedDevices(void){return wxT("PIC16F628A,PIC16F648A,");};
      String GetPictureFileName(void){return wxT("picsimlab2.png");};
      String GetInputMapFile(void){return wxT("input2.map");};
      String GetOutputMapFile(void){return wxT("output2.map");};
      void Reset(_pic *pic);
      void MouseButtonPress(_pic *pic, uint button, uint x, uint y,uint state);
      void MouseButtonRelease(_pic *pic, uint button, uint x, uint y,uint state);
      void KeyPress(_pic *pic, uint key, uint x, uint y,uint mask);
      void KeyRelease(_pic *pic, uint key, uint x, uint y,uint mask);      
      void OnShow(void);
      void RefreshStatus(_pic *pic);
      void WritePreferences(void);
      void ReadPreferences(char *name,char *value);
      unsigned short get_in_id(char * name);
      unsigned short get_out_id(char * name);
};



#endif	/* BOARD_1_H */

