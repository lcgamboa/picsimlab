#ifndef CPWINDOW1 
#define CPWINDOW1

#include<lxrad.h>

class CPWindow1:public CPWindow
{
  public:
  /*#Controls*/
  CEdit edit1;
  CEdit edit2;
  CEdit edit3;
  CEdit edit4;
  CEdit edit5;
  CButton button1;
  CButton button2;
  CButton button3;
  CButton button4;
  CButton button5;
  CButton button6;
  CButton button7;
  CButton button8;
  CButton button9;
  CButton button10;
  CButton button11;
  /*#Events*/
  void button1_EvMouseButtonPress(CControl * control, uint button, uint x, uint y,uint state);
  void button2_EvMouseButtonPress(CControl * control, uint button, uint x, uint y,uint state);
  void button3_EvMouseButtonPress(CControl * control, uint button, uint x, uint y,uint state);
  void button4_EvMouseButtonPress(CControl * control, uint button, uint x, uint y,uint state);
  void button5_EvMouseButtonPress(CControl * control, uint button, uint x, uint y,uint state);
  void button6_EvMouseButtonPress(CControl * control, uint button, uint x, uint y,uint state);
  void button7_EvMouseButtonPress(CControl * control, uint button, uint x, uint y,uint state);
  void button8_EvMouseButtonPress(CControl * control, uint button, uint x, uint y,uint state);
  void button9_EvMouseButtonPress(CControl * control, uint button, uint x, uint y,uint state);
  void button10_EvMouseButtonPress(CControl * control, uint button, uint x, uint y,uint state);
  void button11_EvMouseButtonPress(CControl * control, uint button, uint x, uint y,uint state);

  /*#Others*/
  CPWindow1(void);
};
 extern CPWindow1 Window1 ;

#endif /*#CPWINDOW1*/

