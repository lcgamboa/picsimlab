#ifndef CPWINDOW1 
#define CPWINDOW1

#include<lxrad.h>

class CPWindow1:public CPWindow
{
  public:
  /*#Controls*/
  CImage draw1;
  CFileList filelist1;
  CMenu menu1;
  CPMenu menu1_File;
  CPMenu menu1_Help;
  CItemMenu menu1_File_Exit;
  CItemMenu menu1_Help_About;
  /*#Events*/
  void filelist1_EvFileListSelectFile(CControl * control);
  void menu1_File_Exit_EvMenuActive(CControl * control);
  void menu1_Help_About_EvMenuActive(CControl * control);

  /*#Others*/
  CPWindow1(void);
};
 extern CPWindow1 Window1 ;


#endif /*#CPWINDOW1*/

