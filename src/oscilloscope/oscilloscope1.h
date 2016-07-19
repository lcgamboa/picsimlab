#ifndef CPWINDOW1O 
#define CPWINDOW1O

#include<lxrad/lxrad.h>
#include <picsim/picsim.h>

namespace oscilloscope{
    
class CPWindow1:public CPWindow
{
  public:
//lxrad automatic generated block start, don't edit below!
  /*#Controls*/
  CDraw draw1;
  CSpind spind1;
  CSpind spind2;
  CSpind spind3;
  CSpind spind4;
  CSpind spind5;
  CSpind spind6;
  CToggleButton togglebutton1;
  CButton button1;
  CToggleButton togglebutton2;
  CButton button2;
  CToggleButton togglebutton3;
  CToggleButton togglebutton4;
  CColorDialog colordialog1;
  CToggleButton togglebutton5;
  CSpind spind7;
  CCombo combo1;
  CTimer timer1;
  CCombo combo2;
  CCombo combo3;
  /*#Events*/
  void draw1_EvMouseButtonClick(CControl * control, uint button, uint x, uint y,uint state);
  void spind5_EvOnChangeSpinDouble(CControl * control);
  void button1_EvMouseButtonClick(CControl * control, uint button, uint x, uint y,uint state);
  void button2_EvMouseButtonClick(CControl * control, uint button, uint x, uint y,uint state);
  void togglebutton5_EvOnToggleButton(CControl * control);
  void spind7_EvOnChangeSpinDouble(CControl * control);
  void timer1_EvOnTime(CControl * control);
  void combo2_EvOnComboChange(CControl * control);
  void combo3_EvOnComboChange(CControl * control);

  /*#Others*/
//lxrad automatic generated block end, don't edit above!
  CPWindow1(void);
  
  void SetBaseTimer(_pic * pic);
  void SetSample(_pic * pic);    
  void DrawScreen(void);
private:
  double Dt;   
  double Rt;
  int usetrigger;
  double triggerlv;
  int chpin[2];
};

extern CPWindow1 Window1;

}


#endif /*#CPWINDOW1*/

