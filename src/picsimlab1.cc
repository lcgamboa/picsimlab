/* ########################################################################

   PICsimLab - PIC laboratory simulator

   ########################################################################

   Copyright (c) : 2010-2016  Luis Claudio Gambôa Lopes

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

   For e-mail suggestions :  lcgamboa@yahoo.com
   ######################################################################## */


#include"picsimlab1.h"
#include"picsimlab1_d.cc"

#include"oscilloscope/oscilloscope1.h"


#include"boards_defs.h"

CPWindow1 Window1;


//Implementation

#include"picsimlab2.h"
#include"picsimlab3.h"

#include<dirent.h>

//extern char PROGDEVICE[100];
char SERIALDEVICE[100];

/*
//picstart plus
int prog_init(void);
int prog_loop(_pic * pic);
int prog_end(void);
*/

//mplabx debugger
int mplabxd_init(void);
int mplabxd_loop(_pic *pic);
void mplabxd_end(void);
int mplabxd_testbp(_pic *pic);

int crt;

void
CPWindow1::timer1_EvOnTime(CControl * control)
{

    if(!ondraw)
    {
     ondraw=1;   
     if(!crt)   
       label1.SetColor(wxT("black"));
     crt=0;
     thread1.Run();//parallel thread
     pboard->Draw(&pic,&draw1,scale);
    }
    else
    {
      label1.SetColor(wxT("red"));
      crt=1;
    }
};

void
CPWindow1::thread1_EvThreadRun(CControl*)
{
 //clock_t cstart=0, cend=0;
 //double cpu_time_used;

  //cstart = clock();

  pboard->Run_CPU(&pic);

  //cend = clock();
  //cpu_time_used = ((double) (cend - cstart)) / CLOCKS_PER_SEC;


   //printf("time=%f \n",cpu_time_used);

//FIXME verificar real time   
/*
   if(cpu_time_used > CPUTUSEMAX)
   {
      over+=(cpu_time_used-CPUTUSEMAX);
   
      //printf("over= %f   \n",over);

      if(over > 0.5)
      {

          over=0;

         int newf=((NSTEP-NSTEPKT)/NSTEPKT);//diminui clock
         
         if(newf <= 1)newf=1;    

         //printf("New Freq=%i\n",newf); 
 

         combo1.SetText(itoa(newf));//diminui clock
         combo1_EvOnComboChange(this);
 
       }  
   }
   else
   {
      over=0;
   }
*/  
  
//   cstart=0;
  // Application->ProcessEvents();

  
}

void
CPWindow1::thread1_EvThreadEnd(CControl*)
{
   if(picpwr)
   { 
      //prog_loop(&pic);
      mplabxd_loop(&pic);
   }
   ondraw=0;
}


void
CPWindow1::timer2_EvOnTime(CControl * control)
{
   pboard->RefreshStatus(&pic); 
};



void
CPWindow1::draw1_EvMouseButtonPress(CControl * control, uint button, uint x, uint y,uint state)
{

  x=x/scale;
  y=y/scale;

  pboard->MouseButtonPress(&pic,button,x,y,state);
 
};


void
CPWindow1::draw1_EvMouseButtonRelease(CControl * control, uint button, uint x, uint y,uint state)
{
  
  x=x/scale;
  y=y/scale;
  
  pboard->MouseButtonRelease(&pic,button,x,y,state);

};


void
CPWindow1::draw1_EvKeyboardPress(CControl * control, uint key, uint x, uint y,uint mask)
{
  x=x/scale;
  y=y/scale;   
    
  pboard->KeyPress(&pic, key, x, y, mask); 

 
};


void
CPWindow1::draw1_EvKeyboardRelease(CControl * control, uint key, uint x, uint y,uint mask)
{
  x=x/scale;
  y=y/scale;  

  pboard->KeyRelease(&pic, key, x, y, mask);

};

void
CPWindow1::_EvOnCreate(CControl * control)
{
//  FILE *fin;


  char line[256];
  char fname[1024];


  char *name;
  char *value;

  char home[1024];
    
  int i;
  int lc;
  float clk;
  
  String status;

  
  picrun=1;
  picpwr=1;
  picrst=0;
  
  

if(!create)
{
  PATH=wxGetCwd(); 

  
#ifndef _WIN_
    share=String(_SHARE_);
#else
    share=wxGetCwd()+wxT("/")+String(_SHARE_);
#endif
    

#ifdef _WIN_
 Window3.combo1.SetItems(wxT("COM1,COM2,COM3,COM4,COM5,COM6,COM7,COM8,COM9,COM10,"));
 Window3.combo2.SetItems(wxT("COM1,COM2,COM3,COM4,COM5,COM6,COM7,COM8,COM9,COM10,"));
#else
 Window3.combo1.SetItems(wxT("/dev/ttyS0,/dev/ttyS1,/dev/ttyS2,/dev/ttyS3,/dev/tnt0,/dev/tnt1,/dev/tnt2,/dev/tnt3,/dev/tnt4,/dev/tnt5,"));
 Window3.combo2.SetItems(wxT("/dev/ttyS0,/dev/ttyS1,/dev/ttyS2,/dev/ttyS3,/dev/tnt0,/dev/tnt1,/dev/tnt2,/dev/tnt3,/dev/tnt4,/dev/tnt5,"));
#endif

 combo2.DeleteItems();
 for(i=1;i<=BOARDS_LAST;i++)
 {
     combo2.AddItem(itoa(i));
 }
}
create++;

//load options
  

  wxStandardPathsBase& stdp = wxStandardPaths::Get();
  strcpy(home,(char*)stdp.GetUserDataDir().char_str());

   sprintf(fname,"%s/picsimlab.ini",home);
  
   SERIALDEVICE[0]=0;
   //PROGDEVICE[0]=0;
   pboard=NULL;
   
  prefs.Clear(); 
  
  if(prefs.LoadFromFile(fname)) 
  {
    for(lc=0;lc< (int)prefs.GetLinesCount();lc++)      
    {
      strcpy(line,prefs.GetLine(lc).c_str());  
        
      name=strtok(line,"\t= ");
      strtok(NULL," ");
      value=strtok(NULL,"\"");
      if((name == NULL)||(value==NULL))continue;
#ifndef _WIN_
      if(!strcmp("lser",name))strcpy(SERIALDEVICE,value);
      //if(!strcmp("lprog",name))strcpy(PROGDEVICE,value);
#else
      if(!strcmp("wser",name))strcpy(SERIALDEVICE,value);
      //if(!strcmp("wprog",name))strcpy(PROGDEVICE,value);
#endif
      
      if(!strcmp(name,"lab"))
      {
        sscanf(value,"%i",&i);
        combo2.SetText(itoa(i));
        lab=i;
        lab_=i; 

        pboard = create_board(&lab,&lab_);         

        combo3.SetItems(pboard->GetSupportedDevices());
      }
      if(!strcmp(name,"clock"))
      {
        sscanf(value,"%f",&clk);
        if(clk < 1)
        {
          combo1.SetText(String::Format("%2.1f",clk)); 
        }
        else
        {
          combo1.SetText(String::Format("%2.0f",clk));
        }
        combo1_EvOnComboChange(control);
      }

      if(!strcmp(name,"lpath"))
      {
         PATH=String(value,wxConvUTF8);
      }
      
      if(!strcmp(name,"lfile"))
      {
         FNAME=String(value,wxConvUTF8);
      }

      if(pboard != NULL)
        pboard->ReadPreferences(name,value); 


    }
  }
  else
  {
    printf("Error open config file \"%s\"!\n",fname);
    
    lab=1;//default  
    lab_=1;//default  
    combo2.SetText(wxT("1"));
    pboard= new cboard_1();

#ifndef _WIN_   
   strcpy(SERIALDEVICE,"/dev/tnt2");
   //strcpy(PROGDEVICE,"/dev/tnt4");
#else
   strcpy(SERIALDEVICE,"com6");
   //strcpy(PROGDEVICE,"com8");
#endif
  }
   
 
  combo3.SetText(String::FromAscii(getnamebyproc(pboard->proc,fname)));   
  proc_=pboard->proc;
 

  filedialog1.SetDir(PATH);
 
     
 
  
  draw1.SetImgFileName(share+pboard->GetPictureFileName(),scale,scale);

  //picrun=
  pic_set_serial(&pic,SERIALDEVICE,0,0,0);

  
  sprintf(fname,"%s/mdump_%02i_%s.hex",home,lab,getnamebyproc(pboard->proc,line));
 
  pic_init(&pic,pboard->proc,fname,1,NSTEP*NSTEPKF);

  pboard->Reset(&pic);
      

  /*
  if(prog_init() >= 0 ) 
     status=wxT("PStart:  Ok ");
  else
     status=wxT("PStart:Error");
  */
  status=wxT("");  

  if(mplabxd_init() == 0 ) 
     statusbar1.SetField(1,status+wxT("    MplabxD:  Ok "));
  else
     statusbar1.SetField(1,status+wxT("    MplabxD:Error"));
  

 
  statusbar1.SetField(0,wxT("Running..."));

  Application->ProcessEvents();

  timer1.SetRunState(1);
  timer2.SetRunState(1);
  
  oscilloscope::Window1.SetBaseTimer(&pic);
};


//Change  frequency
void
CPWindow1::combo1_EvOnComboChange(CControl * control)
{

  NSTEP= (int)(atof(combo1.GetText())*NSTEPKT); 
  
  NSTEPJ=NSTEP/JUMPSTEPS;

  pic.freq=NSTEP*NSTEPKF;
         
  oscilloscope::Window1.SetBaseTimer(&pic);
  
  Application->ProcessEvents();
};




//Change board
void
CPWindow1::combo2_EvOnComboChange(CControl * control)
{

  lab_= lab;
  lab= (int)(atof(combo2.GetText()));

  _EvOnDestroy(control);
  _EvOnCreate(control);
  _EvOnShow(control);
  
};


void
CPWindow1::saveprefs(String name, String value)
{
  char line[512];
  char *pname;  
  char *pvalue;
  
   for(int lc=0;lc< (int)prefs.GetLinesCount();lc++)      
    {
      strcpy(line,prefs.GetLine(lc).c_str());  
        
      pname=strtok(line,"\t= ");
      strtok(NULL," ");
      pvalue=strtok(NULL,"\"");
   
      if((pname == NULL)||(pvalue==NULL))continue;
      
      if(String(pname) == name)
      {
         prefs.SetLine(name+wxT("\t= \"")+value+wxT("\""),lc);
         
         //printf("refresh %s=%s\n",(const char *)name.c_str(),(const char *)value.c_str());
         return; 
      }
    }
    //printf("add     %s=%s\n",(const char *)name.c_str(),(const char *)value.c_str());
    prefs.AddLine(name+wxT("\t= \"")+value+wxT("\""));
}

void
CPWindow1::_EvOnDestroy(CControl * control)
{
  //code here:)
  //mprint(wxT("_EvOnDestroy\n"));
  char home[256];
  char fname[256];
  char line[256];
  //FILE * fout; 
  DIR  *dp;
  //int i; 
  
  timer1.SetRunState(0);
  timer2.SetRunState(0);
 
 
  //while(ondraw) //Wait thread finish
  while(thread1.runstate) //Wait thread finish
  {
#ifndef _WIN_
      usleep(100);
#else
      Sleep(1);
#endif 
  }


/*
#ifndef _WIN_
  usleep(100);
#else
  Sleep(1);
#endif 
*/

//write options
  wxTheApp->SetAppName(_T("picsimlab"));
  wxStandardPathsBase& stdp = wxStandardPaths::Get();
  strcpy(home,(char*)stdp.GetUserDataDir().char_str());
 
   dp = opendir (home);

   if(dp)
   {
     closedir(dp);
   }
   else
   {
     remove(home);
#ifndef _WIN_
     mkdir(home, S_IWUSR| S_IRUSR | S_IXUSR | S_IRGRP | S_IROTH ); 
#else     
     mkdir(home);
#endif 
   }

  sprintf(fname,"%s/picsimlab.ini",home);
  

  
    saveprefs(wxT("lab"),String::Format("%i",lab));
    saveprefs(wxT("clock"),combo1.GetText());

#ifndef _WIN_
    saveprefs(wxT("lser"),SERIALDEVICE);
    //saveprefs(wxT("lprog"),PROGDEVICE);
#else
    saveprefs("wser",SERIALDEVICE);
    //saveprefs("wprog",PROGDEVICE);
#endif
    saveprefs(wxT("lpath"),PATH);
    saveprefs(wxT("lfile"),FNAME);
        
    pboard->WritePreferences();
    
    prefs.SaveToFile(fname);
    
//write memory

  sprintf(fname,"%s/mdump_%02i_%s.hex",home,lab_,getnamebyproc(proc_,line));
  
  switch(getfprocbynumber(proc_))
  {
    case P16:
    case P16E:
    case P16E2:    
      write_ihx(&pic,fname);
      break;
    case P18:
      write_ihx18(&pic,fname);
      break;
  }

  pic_end(&pic);
  //prog_end();
  mplabxd_end();
  
  delete pboard;
  pboard =NULL;
  
  //refresh window position to window reopen in same position
  GetX();
  GetY();

};


void
CPWindow1::menu1_File_LoadHex_EvMenuActive(CControl * control)
{
  int pa;

        pa=picpwr; 
        picpwr=0;
        if(filedialog1.Run())
        {
  	  pic_end(&pic);
          pic_set_serial(&pic,SERIALDEVICE,0,0,0);
          picrun=pic_init(&pic,pboard->proc,filedialog1.GetFileName().char_str(),1,NSTEP*NSTEPKF);
          pic.config[0] |= 0x0800; //disable DEBUG
          
          
          pboard->Reset(&pic);
  
          
 
          if(picrun) 
            Window1.SetTitle(wxT("PicsimLab - ")+basename(filedialog1.GetFileName()));          
          else
            Window1.SetTitle(wxT("PicsimLab"));
          
        
          PATH=filedialog1.GetDir();
          FNAME=filedialog1.GetFileName();
        }
        picpwr=pa;
};

void
CPWindow1::menu1_File_Exit_EvMenuActive(CControl * control)
{
  WDestroy();
};

void
CPWindow1::menu1_Help_Contents_EvMenuActive(CControl * control)
{
  Window2.html1.SetLoadFile(share+wxT("doc/picsimlab.html"));   
  Window2.Show();
};


void
CPWindow1::menu1_Help_About_EvMenuActive(CControl * control)
{
  Message(wxT("Developed by L.C. Gamboa\n <lcgamboa@yahoo.com>\n Version: ")+String(wxT(_VERSION_)));
};

void
CPWindow1::menu1_Help_Examples_EvMenuActive(CControl * control)
{
  int pa;

        pa=picpwr; 
        picpwr=0;
        
        filedialog1.SetDir(share+wxT("/examples/hex/"));

        if(filedialog1.Run())
        {
  	  pic_end(&pic);
          pic_set_serial(&pic,SERIALDEVICE,0,0,0);
          picrun=pic_init(&pic,pboard->proc,filedialog1.GetFileName().char_str(),1,NSTEP*NSTEPKF);
          pic.config[0] |= 0x0800; //disable DEBUG
          
          pboard->Reset(&pic);
  
 
          if(picrun) 
            Window1.SetTitle(wxT("PicsimLab - ")+basename(filedialog1.GetFileName()));          
          else
            Window1.SetTitle(wxT("PicsimLab"));
          
        }
        filedialog1.SetDir(PATH);
        picpwr=pa;
};


//Resize
void
CPWindow1::_EvOnShow(CControl * control)
{
 double scalex,scaley;   

if(timer1.GetRunState())
{
  scalex=((Window1.GetWidth()-185)*1.0)/plWidth;
#ifdef _WIN_
  scaley=((Window1.GetHeight()-75)*1.0)/plHeight;
#else
  scaley=((Window1.GetHeight()-90)*1.0)/plHeight; 
#endif

  if(scalex < scaley)
    scale=scalex;
  else
    scale=scaley;


  draw1.SetWidth(plWidth*scale);
  draw1.SetHeight(plHeight*scale);
  


  pboard->OnShow();

   

  draw1.SetImgFileName(share+pboard->GetPictureFileName(),scale,scale);
};

};


void
CPWindow1::menu1_File_Configure_EvMenuActive(CControl * control)
{
  Window3.ShowExclusive();
};


//change processor
void
CPWindow1::combo3_EvOnComboChange(CControl * control)
{
   proc_=pboard->proc;
   pboard->proc=getprocbyname((char *)combo3.GetText().char_str());
   
   if(pboard->proc == 0)
     printf("Unknown processor %s !!\n",(char *)combo3.GetText().char_str()) ;  

  _EvOnDestroy(control);
  _EvOnCreate(control);
  _EvOnShow(control);

};


void
CPWindow1::menu1_File_ReloadLast_EvMenuActive(CControl * control)
{
  int pa;

        pa=picpwr; 
        picpwr=0;

        //if(filedialog1.Run())
        //{
  	  pic_end(&pic);
  	  pic_set_serial(&pic,SERIALDEVICE,0,0,0);
          picrun=pic_init(&pic,pboard->proc,FNAME.char_str(),1,NSTEP*NSTEPKF);
          pic.config[0] |= 0x0800; //disable DEBUG
    
          pboard->Reset(&pic);
    
 
          if(picrun) 
            Window1.SetTitle(wxT("PicsimLab - ")+basename(filedialog1.GetFileName()));          
          else
            Window1.SetTitle(wxT("PicsimLab"));
          
        
        //}
        picpwr=pa;
};

void 
CPWindow1::board_Event(CControl * control)
{
  pboard->board_Event(control);
};


void
CPWindow1::menu1_Modules_Oscilloscope_EvMenuActive(CControl * control)
{
  oscilloscope::Window1.Show ();
  //TODO disable oscilloscope when oscilloscope window is close
};
