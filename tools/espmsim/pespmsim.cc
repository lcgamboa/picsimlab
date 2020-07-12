#include<lxrad.h>
#include"espmsim1.h"
#include"espmsim2.h"

//Program____________________________________________________________

Initialize
{
  Application->Start ();
  Application->ACreateWindow (&Window1);
  Application->ACreateWindow (&Window2);
  Application->Load ();

  return 0;
};

