
#ifndef BOARD_4_H
#define	BOARD_4_H


class cboard_4:public board
{
  private:
     int p_BT1; 
     int p_BT2;  
     int p_BT3; 
     int p_BT4; 
     int p_BT5; 
     int p_BT6; 
     int p_BT7;  
     
     int p_CL1; 
     int p_CL2; 
     int p_CL3; 
     
     int vtc;
     int vt;
     
     lcd_t lcd;

     mi2c_t mi2c;
     rtc2_t rtc2;

     int lcde;

     int sound_on;
     wxSound buzz;

     float vp1in;
     float vp2in;
     float vp2[2];
     float temp[2];
     float ref;

     int rpmstp;
     int rpmc;

    unsigned char d;
    unsigned char sda,sck;

    int dip[20];
    unsigned int lm[50]; //luminosidade media
    unsigned int lm1[50]; //luminosidade media display
    unsigned int lm2[50]; //luminosidade media display
    unsigned int lm3[50]; //luminosidade media display
    unsigned int lm4[50]; //luminosidade media display
    wxBitmap * vent[2];
    
    CGauge *gauge1;
    CGauge *gauge2;
    CScroll *scroll1;
    CScroll *scroll2;
    CLabel *label1;
    CLabel *label2;
    CLabel *label3;
    CLabel *label4;
    CLabel *label5;
     
  public:
      cboard_4(void);
      ~cboard_4(void);
      void Draw(_pic *pic, CDraw *draw,double scale);
      String GetSupportedDevices(void){return wxT("PIC16F777,PIC16F877A,PIC18F452,PIC18F4550,PIC18F4620,");};
      String GetPictureFileName(void){return wxT("picsimlab4.png");};
      String GetInputMapFile(void){return wxT("input4.map");};
      String GetOutputMapFile(void){return wxT("output4.map");};
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


#endif	/* BOARD_4_H */

