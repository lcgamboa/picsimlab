#ifndef CPWINDOW1 
#define CPWINDOW1

#ifndef _WIN_
#include<sys/types.h>
#include<sys/socket.h>
#include<sys/un.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include <fcntl.h>
#else
#include<winsock2.h>
#include<ws2tcpip.h>
#define SHUT_RDWR SD_BOTH 
#define MSG_NOSIGNAL 0
#endif
//system headers independent
#include<errno.h>
#include<stdarg.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>

#include<lxrad.h>

typedef struct
{
    unsigned char type;
    unsigned char dir;
    unsigned char dvalue;
    float avalue;    
} pin_t;

class CPWindow1:public CPWindow
{
  public:
//lxrad automatic generated block start, don't edit below!
  /*#Controls*/
  CMenu menu1;
  CStatusbar statusbar1;
  CDraw draw1;
  CPMenu menu1_File;
  CItemMenu menu1_File_Exit;
  CPMenu menu1_Help;
  CItemMenu menu1_Help_Contents;
  CItemMenu menu1_Help_About;
  CTimer timer1;
  /*#Events*/
  void _EvOnCreate(CControl * control); 
  void _EvOnHide(CControl * control); 
  void _EvOnDestroy(CControl * control);
  void _EvOnShow(CControl * control);
  void draw1_EvMouseMove(CControl * control, const uint button, const uint x,const  uint y, const uint state);
  void draw1_EvMouseButtonPress(CControl * control, const uint button, const uint x,const  uint y, const uint state);
  void draw1_EvMouseButtonRelease(CControl * control, const uint button, const uint x,const  uint y, const uint state);
  void draw1_EvMouseWheel(CControl * control, const int rotation);
  void menu1_File_Exit_EvMenuActive(CControl * control);
  void menu1_Help_Contents_EvMenuActive(CControl * control);
  void menu1_Help_About_EvMenuActive(CControl * control);
  void timer1_EvOnTime(CControl * control);

  /*#Others*/
  CPWindow1(void);
//lxrad automatic generated block end, don't edit above!
  int need_resize;
  int sockfd;
  struct sockaddr_in servaddr;
  int connected;
  char buff[4096];
  char line [200];
  int send_cmd(const char *  message);
  int PointInside (int i, int x, int y);
  int pincount;
  pin_t pins[200];
  int mdx, mdy;
  float scale;
  int offsetx;
  int offsety;
  int mouse_scroll;
  lxFont * font;
};

extern CPWindow1 Window1 ;

#endif /*#CPWINDOW1*/

