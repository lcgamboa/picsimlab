#ifndef CPWINDOW1 
#define CPWINDOW1

#ifdef CONSOLE
#include"cespmsim.h"
#else
#include<lxrad.h>
#endif

#ifdef _WIN_
#include <wx/filename.h>
#include <wx/stdpaths.h>
#endif

class CPWindow1:public CPWindow
{
  public:
//lxrad automatic generated block start, don't edit below!
  /*#Controls*/
  CCombo combo1;
  CToggleButton togglebutton1;
  CTimer timer1;
  CText text1;
  CTimer timer2;
  CText text2;
  CMenu menu1;
  CPMenu menu1_File;
  CPMenu menu1_Help;
  CItemMenu menu1_File_Exit;
  CItemMenu menu1_Help_Contents;
  CItemMenu menu1_Help_About;
  /*#Events*/
  void _EvOnCreate(CControl * control);
  void _EvOnDestroy(CControl * control);
  void togglebutton1_EvOnToggleButton(CControl * control);
  void timer1_EvOnTime(CControl * control);
  void timer2_EvOnTime(CControl * control);
  void menu1_File_Exit_EvMenuActive(CControl * control);
  void menu1_Help_Contents_EvMenuActive(CControl * control);
  void menu1_Help_About_EvMenuActive(CControl * control);

  /*#Others*/
  CPWindow1(void);
//lxrad automatic generated block end, don't edit above!
#ifdef LINUX
  int sfd;//serial descriptor
#else
  HANDLE sfd;//serial descriptor
#endif
  char buff[500];
  char * bptr;
  char blen;
  
  int CWMODE;
  char SSID[64];
  int WCON;
  int CIPMUX;
  int CIPMODE;
  int CIPSERVER;
  int PORT;
  int SKL; //listen socket
  int SRVN; //connected clients
  int SCLI[4];//socket of clients
  
  char RBUFF[2048];//receive buffer
  char SBUFF[2048];//send buffer
  
  int search_cmd(char * buff, int blen);
  void execute_cmd(char * cmd);

};

extern CPWindow1 Window1 ;

#endif /*#CPWINDOW1*/

