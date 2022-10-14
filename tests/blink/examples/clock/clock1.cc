
#include"clock1.h"
#include"clock1_d.cc"


CPWindow1 Window1;

//Implementation

#include<time.h>


void
CPWindow1::timer1_EvOnTime(CControl * control)
{
  time_t t;
  t=time(NULL);
  label1.SetText(lxString::FromAscii(ctime(&t)));
};

void
CPWindow1::_EvOnCreate(CControl * control)
{
  time_t t;
  t=time(NULL);
  label1.SetText(lxString::FromAscii(ctime(&t)));
};















