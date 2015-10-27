#ifndef CPWINDOW1 
#define CPWINDOW1

#define TIMER           0.1      //timer period in s
#define CPUTUSEMAX      0.1      //max period cpu use in s 
#define NSTEPKT     25000.0      //TIMER constant 1MHz/(4.0*timer_freq) 
#define NSTEPKF        40.0      //Freq constant 4.0*timer_freq

extern char SERIALDEVICE[100];

#include<lxrad/lxrad.h>
#include "../picsim.h"
#include "lcd.h"
#include "mi2c.h"
#include "rtc.h"
#include "rtc2.h"
#include "board.h"

#include<wx/sound.h>
#include "wx/stdpaths.h"

#define JUMP 100 //sampled IO refresh

int mplabxd_testbp(_pic *pic);


class CPWindow1:public CPWindow
{
  public:
  /*#Controls*/
  CTimer timer1;
  CTimer timer2;
  CFileDialog filedialog1;
  CDraw draw1;
  CCombo combo1;
  CLabel label1;
  CCombo combo2;
  CMenu menu1;
  CPMenu menu1_File;
  CPMenu menu1_Help;
  CItemMenu menu1_File_LoadHex;
  CItemMenu menu1_File_ReloadLast;
  CItemMenu menu1_File_Configure;
  CItemMenu menu1_File_Exit;
  CItemMenu menu1_Help_Contents;
  CItemMenu menu1_Help_Examples;
  CItemMenu menu1_Help_About;
  CStatusbar statusbar1;
  CLabel label2;
  CLabel label7;
  CCombo combo3;

  /*#Events*/
  void _EvOnCreate(CControl * control);
  void _EvOnDestroy(CControl * control);
  void _EvOnShow(CControl * control);
  void timer1_EvOnTime(CControl * control);
  void timer2_EvOnTime(CControl * control);
  void draw1_EvMouseButtonPress(CControl * control, uint button, uint x, uint y,uint state);
  void draw1_EvMouseButtonRelease(CControl * control, uint button, uint x, uint y,uint state);
  void draw1_EvKeyboardPress(CControl * control, uint key, uint x, uint y,uint mask);
  void draw1_EvKeyboardRelease(CControl * control, uint key, uint x, uint y,uint mask);
  void combo1_EvOnComboChange(CControl * control);
  void combo2_EvOnComboChange(CControl * control);
  void menu1_File_LoadHex_EvMenuActive(CControl * control);
  void menu1_File_ReloadLast_EvMenuActive(CControl * control);
  void menu1_File_Configure_EvMenuActive(CControl * control);
  void menu1_File_Exit_EvMenuActive(CControl * control);
  void menu1_Help_Contents_EvMenuActive(CControl * control);
  void menu1_Help_Examples_EvMenuActive(CControl * control);
  void menu1_Help_About_EvMenuActive(CControl * control);
  void combo3_EvOnComboChange(CControl * control);

  /*#Others*/
  CPWindow1(void);
  void saveprefs(String name, String value);

  String GetSharePath(void){return share;};
  void SetplWidth(int pw){plWidth=pw;};
  void SetplHeight(int ph){plHeight=ph;};
  double GetScale(void){return scale;};
  int Get_picpwr(void){return picpwr;};
  int Get_picrun(void){return picrun;};
  int Get_picrst(void){return picrst;};
  int Get_picdbg(void){return picdbg;};
  void Set_picpwr(int pp){picpwr =pp;};
  void Set_picrun(int pr){picrun =pr;};
  void Set_picrst(int pr){picrst =pr;};
  void Set_picdbg(int pd){picdbg =pd;};
  void SetPATH(String path){PATH=path;};
  void SetFNAME(String fname){FNAME=fname;};
  long int GetNSTEP(void){return NSTEP;};
  long int GetNSTEPJ(void){return NSTEPJ;};
 private: 
  String share;
 
  int picrun;
  int picpwr;
  int picrst;
  int picdbg;

  int plWidth;
  int plHeight;
  double scale;
  long int NSTEP;
  long int NSTEPJ;
  String PATH;
  String FNAME;
  
  CStringList prefs;

  int lab;
  int lab_;

  int proc_;

  _pic pic;

  board *pboard;

  float over;
  int create;
};

extern CPWindow1 Window1 ;

#endif /*#CPWINDOW1*/

