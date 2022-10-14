#ifndef CPWINDOW1 
#define CPWINDOW1

#include<lxrad.h>

class CPWindow1:public CPWindow
{
  public:
  /*#Controls*/
  CDraw draw1;
  CEdit edit1;
  CText text1;
  CButton button1;
  CButton button2;
  CButton button3;
  CFileDialog filedialog1;
  /*#Events*/
  void _EvOnCreate(CControl * control);
  void _EvOnShow(CControl * control);
  void draw1_EvMouseButtonPress(CControl * control, uint button, uint x, uint y,uint state);
  void draw1_EvOnDraw(CControl * control);
  void button1_EvMouseButtonClick(CControl * control, uint button, uint x, uint y,uint state);
  void button2_EvMouseButtonClick(CControl * control, uint button, uint x, uint y,uint state);
  void button3_EvMouseButtonClick(CControl * control, uint button, uint x, uint y,uint state);

  /*#Others*/
  CPWindow1(void);
};

extern CPWindow1 Window1 ;

#endif /*#CPWINDOW1*/

