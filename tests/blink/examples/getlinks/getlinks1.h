#ifndef CPWINDOW1 
#define CPWINDOW1

#include<lxrad.h>

class CPWindow1:public CWindow
{
  public:
  /*#Controls*/
  CFileDialog filedialog1;
  CList list1;
  CButton button1;
  CButton button2;
  CEdit edit1;
  CLabel string2;
  CButton button3;
  CButton button4;
  /*#Events*/
  void button1_EvMouseButtonClick(CControl * control, uint button, uint x, uint y,uint state);
  void button2_EvMouseButtonClick(CControl * control, uint button, uint x, uint y,uint state);
  void button3_EvMouseButtonClick(CControl * control, uint button, uint x, uint y,uint state);
  void button4_EvMouseButtonClick(CControl * control, uint button, uint x, uint y,uint state);

  /*#Others*/
  CPWindow1(void);
};
 extern CPWindow1 Window1 ;

#endif /*#CPWINDOW1*/

