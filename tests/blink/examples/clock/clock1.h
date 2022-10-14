#ifndef CPWINDOW1 
#define CPWINDOW1

#include<lxrad.h>

class CPWindow1:public CPWindow
{
  public:
  /*#Controls*/
  CTimer timer1;
  CLabel label1;
  /*#Events*/
  void _EvOnCreate(CControl * control);
  void timer1_EvOnTime(CControl * control);

  /*#Others*/
  CPWindow1(void);
};
 extern CPWindow1 Window1 ;

#endif /*#CPWINDOW1*/

