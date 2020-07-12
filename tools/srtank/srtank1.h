#ifndef CPWINDOW1 
#define CPWINDOW1

#include<lxrad.h>

#ifdef _WIN_
#include <wx/filename.h>
#include <wx/stdpaths.h>
#endif

class CPWindow1:public CPWindow
{
  public:
  /*#Controls*/
  CDraw draw1;
  CButton button1;
  CCombo combo1;
  CLabel label1;
  CEdit edit1;
  CEdit edit2;
  CEdit edit3;
  CEdit edit4;
  CEdit edit5;
  CEdit edit6;
  CEdit edit7;
  CEdit edit8;
  CCheckBox checkbox1;
  CCheckBox checkbox2;
  CCheckBox checkbox3;
  CCheckBox checkbox4;
  CCheckBox checkbox5;
  CCheckBox checkbox6;
  CCheckBox checkbox7;
  CCheckBox checkbox8;
  CEdit edit9;
  CLabel label2;
  CLabel label3;
  CLabel label4;
  CLabel label5;
  CLabel label6;
  CLabel label7;
  CLabel label8;
  CLabel label9;
  CEdit edit10;
  CCheckBox checkbox9;
  CEdit edit11;
  CEdit edit12;
  CLabel label10;
  CLabel label11;
  CLabel label12;
  CLabel label13;
  CLabel label14;
  CLabel label15;
  CTimer timer1;
  CText text1;
  CMenu menu1;
  CPMenu menu1_File;
  CPMenu menu1_Help;
  CItemMenu menu1_File_DefaultConfig;
  CItemMenu menu1_File_SaveConfig;
  CItemMenu menu1_File_LoadConfig;
  CItemMenu menu1_File_Exit;
  CItemMenu menu1_Help_Contents;
  CItemMenu menu1_Help_About;
  CFileDialog filedialog1;
  /*#Events*/
  void _EvOnCreate(CControl * control);
  void _EvOnDestroy(CControl * control);
  void draw1_EvMouseButtonPress(CControl * control, uint button, uint x, uint y,uint state);
  void button1_EvMouseButtonClick(CControl * control, uint button, uint x, uint y,uint state);
  void timer1_EvOnTime(CControl * control);
  void menu1_File_DefaultConfig_EvMenuActive(CControl * control);
  void menu1_File_SaveConfig_EvMenuActive(CControl * control);
  void menu1_File_LoadConfig_EvMenuActive(CControl * control);
  void menu1_File_Exit_EvMenuActive(CControl * control);
  void menu1_Help_Contents_EvMenuActive(CControl * control);
  void menu1_Help_About_EvMenuActive(CControl * control);
  void filedialog1_EvOnClose(int retId);

  /*#Others*/
  CPWindow1(void);
};

extern CPWindow1 Window1 ;

#endif /*#CPWINDOW1*/

