#include"espmsim2.h"
#include"espmsim2_d.cc"

CPWindow2 Window2;


//Implementation

void
CPWindow2::_EvOnShow(CControl * control)
{
   html1.SetWidth(Window2.GetWidth()-30);
   html1.SetHeight(Window2.GetHeight()-50); 
};




