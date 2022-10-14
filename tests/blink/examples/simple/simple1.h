#ifndef CPWINDOW1 
#define CPWINDOW1

#include<lxrad.h>

class CPWindow1:public CPWindow
{
  public:
  /*#Controls*/
  CButton button2;
  CButton button3;
  CButton button4;
  CEdit edit1;
  CEdit edit2;
  CEdit edit3;
  CLabel label1;
  CLabel label2;
  CButton button5;
  CButton button1;
  CMenu menu1;
  CPMenu menu1_File;
  CPMenu menu1_Help;
  CItemMenu menu1_File_Exit;
  CItemMenu menu1_Help_About;
  /*#Events*/
  void _EvMouseButtonPress(CControl * control, uint button, uint x, uint y,uint state);
  void button2_EvMouseButtonPress(CControl * control, uint button, uint x, uint y,uint state);
  void button3_EvMouseButtonPress(CControl * control, uint button, uint x, uint y,uint state);
  void button4_EvMouseButtonPress(CControl * control, uint button, uint x, uint y,uint state);
  void button5_EvMouseButtonPress(CControl * control, uint button, uint x, uint y,uint state);
  void button1_EvMouseButtonPress(CControl * control, uint button, uint x, uint y,uint state);
  void menu1_File_Exit_EvMenuActive(CControl * control);
  void menu1_Help_About_EvMenuActive(CControl * control);

  /*#Others*/
  CPWindow1(void);
};
 extern CPWindow1 Window1 ;

#endif /*#CPWINDOW1*/

