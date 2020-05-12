#include"srtank2.h"
#include"srtank2_d.cc"

CPWindow2 Window2;


//Implementation

#ifndef EXT_BROWSER
void
CPWindow2::_EvOnShow(CControl * control)
{
   html1.SetWidth(Window2.GetWidth()-30);
   html1.SetHeight(Window2.GetHeight()-50); 
}

#endif
