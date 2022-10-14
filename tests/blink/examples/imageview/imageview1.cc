
#include"imageview1.h"
#include"imageview1_d.cc"

CPWindow1 Window1;

//Implementation


void
CPWindow1::filelist1_EvFileListSelectFile(CControl * control)
{ 
  lxString file=filelist1.GetSelectedFile();
	
  if((file.find(lxT(".gif")) >0)||
     (file.find(lxT(".jpg")) >0)||
     (file.find(lxT(".png")) >0)||
     (file.find(lxT(".tiff"))>0)||
     (file.find(lxT(".ps"))  >0)||
     (file.find(lxT(".bmp")) >0)||
     (file.find(lxT(".xpm")) >0)||
     (file.find(lxT(".ppm")) >0)||
     (file.find(lxT(".GIF")) >0)||
     (file.find(lxT(".JPG")) >0)||
     (file.find(lxT(".PNG")) >0)||
     (file.find(lxT(".TIFF"))>0)||
     (file.find(lxT(".PS"))  >0)||
     (file.find(lxT(".BMP")) >0)||
     (file.find(lxT(".XPM")) >0)||
     (file.find(lxT(".PPM")) >0))
       draw1.SetImgFileName(filelist1.GetSelectedFile());
};

void
CPWindow1::menu1_File_Exit_EvMenuActive(CControl * control)
{
  WDestroy();
};

void
CPWindow1::menu1_Help_About_EvMenuActive(CControl * control)
{
  Message(lxT("Create By LCGamboa"));
};






