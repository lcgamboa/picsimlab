#ifndef CPWINDOW1 
#define CPWINDOW1

#include<lxrad.h>

class CPWindow1:public CPWindow
{
  public:
  /*#Controls*/
  CDraw draw1;
  CTimer timer1;
  /*#Events*/
  void draw1_EvMouseButtonPress(CControl * control, uint button, uint x, uint y,uint state);
  void timer1_EvOnTime(CControl * control);

  /*#Others*/
  CPWindow1(void);
};
 extern CPWindow1 Window1 ;

#endif /*#CPWINDOW1*/

