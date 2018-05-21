#ifndef CPWINDOW1 
#define CPWINDOW1

#include<lxrad/lxrad.h>

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
  /*#Events*/
  void _EvOnCreate(CControl * control);
  void _EvOnDestroy(CControl * control);
  void draw1_EvMouseButtonPress(CControl * control, uint button, uint x, uint y,uint state);
  void button1_EvMouseButtonClick(CControl * control, uint button, uint x, uint y,uint state);
  void timer1_EvOnTime(CControl * control);

  /*#Others*/
  CPWindow1(void);
};

extern CPWindow1 Window1 ;

#endif /*#CPWINDOW1*/

