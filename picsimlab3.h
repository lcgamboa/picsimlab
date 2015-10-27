#ifndef CPWINDOW3 
#define CPWINDOW3

#include<lxrad/lxrad.h>

class CPWindow3:public CPWindow
{
  public:
  /*#Controls*/
  CLabel label1;
  CLabel label2;
  CButton button1;
  CCombo combo1;
  CCombo combo2;
  /*#Events*/
  void _EvOnCreate(CControl * control);
  void button1_EvMouseButtonClick(CControl * control, uint button, uint x, uint y,uint state);

  /*#Others*/
  CPWindow3(void);
};

extern CPWindow3 Window3 ;

#endif /*#CPWINDOW3*/

