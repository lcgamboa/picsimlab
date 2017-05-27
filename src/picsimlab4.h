#ifndef CPWINDOW4 
#define CPWINDOW4

#include<lxrad/lxrad.h>
#include <picsim/picsim.h>

    
class CPWindow4:public CPWindow
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
  CLabel label1;
  CLabel label2;
  CLabel label3;
  CLabel label4;
  CLabel label5;
  CLabel label6;
  CLabel label7;
  CLabel label8;
  CLabel label9;
  CLabel label10;
  CLabel label11;
  /*#Events*/
  void _EvOnDestroy(CControl * control);
  void _EvOnShow(CControl * control);
  void _EvOnHide(CControl * control);
  void draw1_EvMouseButtonClick(CControl * control, uint button, uint x, uint y,uint state);
  void spind5_EvOnChangeSpinDouble(CControl * control);
  void spind6_EvOnChangeSpinDouble(CControl * control);
  void button1_EvMouseButtonClick(CControl * control, uint button, uint x, uint y,uint state);
  void button2_EvMouseButtonClick(CControl * control, uint button, uint x, uint y,uint state);
  void togglebutton5_EvOnToggleButton(CControl * control);
  void spind7_EvOnChangeSpinDouble(CControl * control);
  void timer1_EvOnTime(CControl * control);
  void combo2_EvOnComboChange(CControl * control);
  void combo3_EvOnComboChange(CControl * control);

  /*#Others*/
//lxrad automatic generated block end, don't edit above!
  CPWindow4(void);
          
  void WritePreferences(void);
  void ReadPreferences(char *name,char *value);
  void SetBaseTimer(void);
  void SetSample(void);    
  void DrawScreen(void);
private:
  double Dt;   
  double Rt;
  int usetrigger;
  double triggerlv;
  int chpin[2];
  int toffset;
};

extern CPWindow4 Window4;


#endif /*#CPWINDOW4*/

