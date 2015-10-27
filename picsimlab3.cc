#include"picsimlab3.h"
#include"picsimlab3_d.cc"

#include"picsimlab1.h"

CPWindow3 Window3;

extern char PROGDEVICE[100];

extern char SERIALDEVICE[100];


//Implementation

void
CPWindow3::_EvOnCreate(CControl * control)
{
  combo1.SetText(String::FromAscii(SERIALDEVICE));
  combo2.SetText(String::FromAscii(PROGDEVICE));
};


void
CPWindow3::button1_EvMouseButtonClick(CControl * control, uint button, uint x, uint y,uint state)
{

  if(combo1.GetText()==combo2.GetText())
  {
    Message(wxT("Use diferent ports!"));
    return;
  }	

  strcpy(SERIALDEVICE,(char*)combo1.GetText().char_str());
  strcpy(PROGDEVICE,(char*)combo2.GetText().char_str());
  
  Window1._EvOnDestroy(control);
  Window1._EvOnCreate(control);

  WDestroy();
};






