/* ########################################################################

   PICsimLab - PIC laboratory simulator

   ########################################################################

   Copyright (c) : 2010-2021  Luis Claudio Gamboa Lopes

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

//main window

//#define CONVERTER_MODE

//timer debug
//#define TDEBUG

#include"picsimlab1.h"
#include"picsimlab1_d.cc"

CPWindow1 Window1;

//Implementation

#include"picsimlab2.h"
#include"picsimlab3.h"
#include"picsimlab4.h"
#include"picsimlab5.h"

#include"devices/rcontrol.h"



#ifdef __EMSCRIPTEN__
#include<emscripten.h>
#endif

#ifdef _USE_PICSTARTP_
extern char PROGDEVICE[100];
#endif
char SERIALDEVICE[100];

#ifdef _USE_PICSTARTP_
//picstart plus
int prog_init(void);
int prog_loop(_pic * pic);
int prog_end(void);
#endif

#ifdef _WIN_
#define msleep(x) Sleep(x)

void
usleep(unsigned int usec)
{
 HANDLE timer;
 LARGE_INTEGER ft;

 ft.QuadPart = -(10 * (__int64)usec);

 timer = CreateWaitableTimer (NULL, TRUE, NULL);
 SetWaitableTimer (timer, &ft, 0, NULL, NULL, 0);
 WaitForSingleObject (timer, INFINITE);
 CloseHandle (timer);
}
#else
#define msleep(x) usleep(x*1000)
#endif        

#ifdef CONVERTER_MODE
static lxString cvt_fname;
#endif

#if defined(TDEBUG) || defined(_NOTHREAD)  
#ifdef _WIN_

double
cpuTime()
{
 FILETIME a, b, c, d;
 if (GetProcessTimes (GetCurrentProcess (), &a, &b, &c, &d) != 0)
  {
   //  Returns total user time.
   //  Can be tweaked to include kernel times as well.
   return
   (double) (d.dwLowDateTime |
             ((unsigned long long) d.dwHighDateTime << 32)) * 0.0000001;
  }
 else
  {
   //  Handle error
   return 0;
  }
}
#else
#include <time.h>
#include <sys/time.h>

double
cpuTime()
{
 return (double) clock () / CLOCKS_PER_SEC;
}
#endif
#endif

void
CPWindow1::timer1_EvOnTime(CControl * control)
{
 sync = 1;
 status.st[0] |= ST_T1;


 if ((!tgo)&&(timer1.GetTime () == 100))
  {
   if (crt)
    {
     label2.SetColor (0, 0, 0);
     label2.Draw ();
    }
   crt = 0;
  }
 else
  {
   if (!crt)
    {
     label2.SetColor (255, 0, 0);
     label2.Draw ();
    }
   crt = 1;
  }

 if (!tgo)
  {
   zerocount++;

   if (zerocount > 3)
    {
     zerocount = 0;

     if (timer1.GetTime () > 100)
      {
       timer1.SetTime (timer1.GetTime () - 5);
      }
    }
  }
 else
  {
   zerocount = 0;
  }

 tgo++;
#ifndef _NOTHREAD
 cpu_mutex->Lock ();
 cpu_cond->Signal ();
 cpu_mutex->Unlock ();
#endif

 if (tgo > 3)
  {
   timer1.SetTime (timer1.GetTime () + 5);
   tgo = 1;
  }

 if (need_resize == 1)
  {
   double scalex, scaley, scale_temp;

   scalex = ((Window1.GetWidth () - 185)*1.0) / plWidth;
   scaley = ((Window1.GetHeight () - 90)*1.0) / plHeight;


   if (scalex < 0.1)scalex = 0.1;
   if (scaley < 0.1)scaley = 0.1;
   if (scalex > 4)scalex = 4;
   if (scaley > 4)scaley = 4;

   if (scalex < scaley)
    scale_temp = scalex;
   else
    scale_temp = scaley;

   if (scale != scale_temp)
    {
     scale = scale_temp;

     int nw = (plWidth * scale);
     if (nw == 0)nw = 1;
     int nh = (plHeight * scale);
     if (nh == 0)nh = 1;

     scale = ((double) nw) / plWidth;

     draw1.SetWidth (nw);
     draw1.SetHeight (nh);

     draw1.SetImgFileName (share + lxT ("boards/") + pboard->GetPictureFileName (), scale, scale);
    }

   pboard->SetScale (scale);
   pboard->EvOnShow ();

   if (osc_on)
    {
     menu1_Modules_Oscilloscope_EvMenuActive (this);
     osc_on = 0;
    }
   if (spare_on)
    {
     menu1_Modules_Spareparts_EvMenuActive (this);
     spare_on = 0;
    }
  }

 need_resize++;

 pboard->Draw (&draw1);

 status.st[0] &= ~ST_T1;
}

void
CPWindow1::thread1_EvThreadRun(CControl*)
{
#if defined(TDEBUG) || defined(_NOTHREAD)  
 double t0, t1;
#endif
 do
  {

   if (tgo)
    {
#if defined(TDEBUG) || defined(_NOTHREAD)     
     t0 = cpuTime ();
#endif     
     status.st[1] |= ST_TH;
     pboard->Run_CPU ();
     if (debug)pboard->DebugLoop ();
     tgo--;
     status.st[1] &= ~ST_TH;
#if defined(TDEBUG) || defined(_NOTHREAD)       
     t1 = cpuTime ();
#endif     
#if defined(_NOTHREAD)     
     if ((t1 - t0) / (Window1.timer1.GetTime ()*1e-5) > 110)
      {
       tgo++;
      }
     else
      {
       tgo = 0;
      }
#endif     
#ifdef TDEBUG      
     printf ("PTime= %lf  tgo= %2i  zeroc= %2i  Timer= %3u Perc.= %4.1lf\n",
             t1 - t0, tgo, zerocount, Window1.timer1.GetTime (),
             (t1 - t0) / (Window1.timer1.GetTime ()*1e-5));
#endif
    }
   else
    {
#ifndef _NOTHREAD         
     cpu_mutex->Lock ();
     cpu_cond->Wait ();
     cpu_mutex->Unlock ();
#endif     
    }

  }
 while (!thread1.TestDestroy ());
}

void
CPWindow1::thread2_EvThreadRun(CControl*)
{
 do
  {
   usleep (1000);
   if (rcontrol_loop ())
    {
     usleep (100000);
    }
  }
 while (!thread2.TestDestroy ());
}

void
CPWindow1::timer2_EvOnTime(CControl * control)
{
 //avoid run again before terminate previous
 if (status.st[0] & ST_T2)return;

 status.st[0] |= ST_T2;
 if (pboard != NULL)
  {
   pboard->RefreshStatus ();

   switch (cpustate)
    {
    case CPU_RUNNING:
     statusbar1.SetField (0, lxT ("Running..."));
     break;
    case CPU_STEPPING:
     statusbar1.SetField (0, lxT ("Stepping..."));
     break;
    case CPU_HALTED:
     statusbar1.SetField (0, lxT ("Halted!"));
     break;
    case CPU_BREAKPOINT:
     statusbar1.SetField (0, lxT ("BreakPoint!"));
     break;
    case CPU_ERROR:
     statusbar1.SetField (0, lxT ("Error!"));
     break;
    }
  }

 label2.SetText (lxString ().Format ("Spd: %3.2fx", 100.0 / timer1.GetTime ()));


 if (Errors.GetLinesCount ())
  {
   Message_sz (Errors.GetLine (0), 450, 200);
   Errors.DelLine (0);
  }

 status.st[0] &= ~ST_T2;

#ifdef CONVERTER_MODE
 if (cvt_fname.Length () > 3)
  {
   SaveWorkspace (cvt_fname);
  }
#endif 

 if (settodestroy)
  {
   WDestroy ();
  }

}

void
CPWindow1::draw1_EvMouseMove(CControl * control, uint button, uint x, uint y, uint state)
{

 x = x / scale;
 y = y / scale;

 pboard->EvMouseMove (button, x, y, state);

}

void
CPWindow1::draw1_EvMouseButtonPress(CControl * control, uint button, uint x, uint y, uint state)
{

 x = x / scale;
 y = y / scale;

 pboard->EvMouseButtonPress (button, x, y, state);

}

void
CPWindow1::draw1_EvMouseButtonRelease(CControl * control, uint button, uint x, uint y, uint state)
{

 x = x / scale;
 y = y / scale;

 pboard->EvMouseButtonRelease (button, x, y, state);

}

void
CPWindow1::draw1_EvKeyboardPress(CControl * control, const uint key, const uint hkey, const uint mask)
{
 pboard->EvKeyPress (key, mask);
}

void
CPWindow1::draw1_EvKeyboardRelease(CControl * control, const uint key, const uint hkey, const uint mask)
{
 pboard->EvKeyRelease (key, mask);
}

void
CPWindow1::_EvOnCreate(CControl * control)
{
 char home[1024];
 lxFileName fn;
 lxFileName fn_spare;
 int use_default_board = 0;

 strncpy (home, (char*) lxGetUserDataDir (lxT ("picsimlab")).char_str (), 1023);

 HOME = home;

 PATH = lxGetCwd ();

 share = dirname (lxGetExecutablePath ()) + lxT ("/") + lxString (_SHARE_);

 if (Application->Aargc == 2)
  {
   fn.Assign (Application->Aargv[1]);
   fn.MakeAbsolute ();
   use_default_board = 1;
  }
 else if ((Application->Aargc >= 3) && (Application->Aargc <= 5))
  {
   char fname[1200];

   if (Application->Aargc >= 4)
    {
     fn.Assign (Application->Aargv[3]);
     fn.MakeAbsolute ();
    }
   if (Application->Aargc == 5)
    {
     fn_spare.Assign (Application->Aargv[4]);
     fn_spare.MakeAbsolute ();
    }
   lab = -1;
   for (int i = 0; i < BOARDS_LAST; i++)
    {
     if (!strcmp (boards_list[i].name_, Application->Aargv[1]))
      {
       lab = i;
       break;
      }
    }

   if (lab != -1)
    {
     snprintf (fname, 1199, "%s/picsimlab.ini", home);
     prefs.Clear ();
     if (lxFileExists (fname))
      {
       if (prefs.LoadFromFile (fname))
        {
         saveprefs (lxT ("picsimlab_lab"), boards_list[lab].name_);
         saveprefs (lxString (boards_list[lab].name_) + lxT ("_proc"), Application->Aargv[2]);
         if (Application->Aargc == 5)
          {
           saveprefs (lxT ("spare_on"), lxT ("1"));
          }
         prefs.SaveToFile (fname);
        }
      }
    }
   else
    {
     Application->Aargc = 1;
     printf ("PICSimLab: Unknown board %s !\n", Application->Aargv[1]);
    }
  }


 //load options
 Configure (home, use_default_board, 1);


 //search for file name 
 if (Application->Aargc == 2)
  {
   LoadWorkspace (fn.GetFullPath ());
  }
 else if ((Application->Aargc == 4) || (Application->Aargc == 5))
  {
   LoadHexFile (fn.GetFullPath ());
   if (Application->Aargc == 5)
    {
     Window5.LoadConfig (fn_spare.GetFullPath ());
    }
  }

 //board menu
 for (int i = 0; i < BOARDS_LAST; i++)
  {
   MBoard[i].SetFOwner (this);
   MBoard[i].SetName (itoa (i));
   MBoard[i].SetText (boards_list[i].name);
   MBoard[i].EvMenuActive = EVMENUACTIVE & CPWindow1::menu1_EvBoard;
   menu1_Board.CreateChild (&MBoard[i]);
  }


}

void
CPWindow1::Configure(const char * home, int use_default_board, int create)
{

 char line[1024];
 char fname[1024];

 char *name;
 char *value;

 int i, j;
 int lc;

 lxString status;

 mcurun = 1;
 mcupwr = 1;
 mcurst = 0;

#ifndef _NOTHREAD    
 if (cpu_mutex == NULL)
  {
   cpu_mutex = new lxMutex ();
   cpu_cond = new lxCondition (*cpu_mutex);
  }
#endif

 //TODO: verify initialization errors
 snprintf (fname, 1023, "%s/picsimlab.ini", home);

 SERIALDEVICE[0] = ' ';
 SERIALDEVICE[1] = 0;
#ifdef _USE_PICSTARTP_ 
 PROGDEVICE[0] = ' ';
 PROGDEVICE[1] = 0;
#endif   
 pboard = NULL;

 prefs.Clear ();
 if (lxFileExists (fname))
  {
   if (prefs.LoadFromFile (fname))
    {
     for (lc = 0; lc < (int) prefs.GetLinesCount (); lc++)
      {
       strncpy (line, prefs.GetLine (lc).c_str (), 1023);

       name = strtok (line, "\t= ");
       strtok (NULL, " ");
       value = strtok (NULL, "\"");
       if ((name == NULL) || (value == NULL))continue;
#ifndef _WIN_
       if (!strcmp ("picsimlab_lser", name))strcpy (SERIALDEVICE, value);
#ifdef _USE_PICSTARTP_      
       if (!strcmp ("picsimlab_lprog", name))strcpy (PROGDEVICE, value);
#endif      
#else
       if (!strcmp ("picsimlab_wser", name))strcpy (SERIALDEVICE, value);
#ifdef _USE_PICSTARTP_      
       if (!strcmp ("picsimlab_wprog", name))strcpy (PROGDEVICE, value);
#endif      
#endif

       if (!strcmp (name, "picsimlab_lab"))
        {
         if (use_default_board)
          {
           lab = 0;
           lab_ = 0;
          }
         else
          {
           for (i = 0; i < BOARDS_LAST; i++)
            {
             if (!strcmp (boards_list[i].name_, value))
              {
               break;
              }
            }
           lab = i;
           lab_ = i;
          }

         pboard = create_board (&lab, &lab_);
         if (pboard->GetScale () < scale)
          {
           scale = pboard->GetScale ();
          }
         else
          {
           pboard->SetScale (scale);
          }
         SetClock (2.0); //Default clock

        }

       if (!strcmp (name, "picsimlab_debug"))
        {
         sscanf (value, "%i", &debug);
         togglebutton1.SetCheck (debug);
        }

       if (!strcmp (name, "picsimlab_debugt"))
        {
         sscanf (value, "%i", &debug_type);
        }

       if (!strcmp (name, "picsimlab_remotecp"))
        {
         sscanf (value, "%hu", &remotec_port);
        }

       if (!strcmp (name, "picsimlab_debugp"))
        {
         sscanf (value, "%hu", &debug_port);
        }

       if (!strcmp (name, "osc_on"))
        {
         sscanf (value, "%i", &osc_on);
        }

       if (!strcmp (name, "spare_on"))
        {
         sscanf (value, "%i", &spare_on);
        }


       if (!strcmp (name, "picsimlab_position"))
        {
         sscanf (value, "%i,%i", &i, &j);
         SetX (i);
         SetY (j);
        }

       if (!strcmp (name, "picsimlab_scale"))
        {
         if (create)
          {
           sscanf (value, "%lf", &scale);
           draw1.SetWidth (plWidth * scale);
           SetWidth (185 + plWidth * scale);
           draw1.SetHeight (plHeight * scale);
           SetHeight (90 + plHeight * scale);
           pboard->SetScale (scale);
          }
        }

       if (!strcmp (name, "picsimlab_lpath"))
        {
         PATH = lxString (value, lxConvUTF8);
        }

       if (!strcmp (name, "picsimlab_lfile"))
        {
         FNAME = lxString (value, lxConvUTF8);
         if (FNAME.length () > 1)
          menu1_File_ReloadLast.SetEnable (1);
         else
          menu1_File_ReloadLast.SetEnable (0);
        }

       if (pboard != NULL)
        pboard->ReadPreferences (name, value);

       Window4.ReadPreferences (name, value);
       Window5.ReadPreferences (name, value);
      }
    }
  }

 if (pboard == NULL)
  {
   printf ("Error open config file \"%s\"!\n", fname);

   lab = 0; //default  
   lab_ = 0; //default  

   pboard = boards_list[0].bcreate ();

#ifndef _WIN_   
   strcpy (SERIALDEVICE, "/dev/tnt2");
#ifdef _USE_PICSTARTP_   
   strcpy (PROGDEVICE, "/dev/tnt4");
#endif   
#else
   strcpy (SERIALDEVICE, "com6");
#ifdef _USE_PICSTARTP_   
   strcpy (PROGDEVICE, "com8");
#endif   
#endif
  }

 menu1_Microcontroller.DestroyChilds ();
 lxString sdev = pboard->GetSupportedDevices ();
 int f;
 int dc = 0;
 while (sdev.size () > 0)
  {
   f = sdev.find (lxT (","));
   if (f < 0)break;
   MMicro[dc].SetFOwner (this);
   MMicro[dc].SetName ("Micro_" + itoa (dc + 1));
   MMicro[dc].SetText (sdev.substr (0, f));
   MMicro[dc].EvMenuActive = EVMENUACTIVE & CPWindow1::menu1_EvMicrocontroller;
   menu1_Microcontroller.CreateChild (&MMicro[dc]);
   MMicro[dc].SetVisible (true);
   sdev = sdev.substr (f + 1, sdev.size () - f - 1);
   dc++;

   if (dc >= MAX_MIC)
    {
     printf ("PICSimLab: microcontroller menu only support %i entries!\n", MAX_MIC);
     exit (-1);
    }
  }


 filedialog1.SetDir (PATH);


 draw1.SetImgFileName (share + lxT ("boards/") + pboard->GetPictureFileName (), scale, scale);

 pboard->MSetSerial (SERIALDEVICE);

 sprintf (fname, "%s/mdump_%s_%s.hex", home, boards_list[lab].name_, (const char *) pboard->GetProcessorName ().c_str ());


 switch (pboard->MInit (pboard->GetProcessorName (), fname, NSTEP * NSTEPKF))
  {
  case HEX_NFOUND:
   printf ("File not found!\n");
   break;
  case HEX_CHKSUM:
   printf ("File checksum error!\n");
   pboard->MEraseFlash ();
   break;
  }

 pboard->Reset ();

 proc_ = pboard->GetProcessorName ();

 SetTitle (lxT ("PICSimLab - ") + lxString (boards_list[lab].name) + lxT (" - ") + pboard->GetProcessorName ());


#ifdef _USE_PICSTARTP_
 if (prog_init () >= 0)
  status = lxT ("PStart:  Ok ");
 else
  status = lxT ("PStart:Error");
#else
 status = lxT ("");
#endif


#ifdef NO_DEBUG
 statusbar1.SetField (1, lxT (" "));
#else 
 if (debug)
  {
   int ret = pboard->DebugInit (debug_type);
   if (ret < 0)
    {
     statusbar1.SetField (1, status + lxT ("Debug: Error"));
    }
   else
    {
     statusbar1.SetField (1, status + lxT ("Debug: ") + pboard->GetDebugName () + ":" + itoa (debug_port));
    }
  }
 else
  {
   statusbar1.SetField (1, status + lxT ("Debug: Off"));
  }
#endif

 statusbar1.SetField (0, lxT ("Running..."));


 thread1.Run (); //parallel thread
#ifndef __EMSCRIPTEN__ 
 //FIXME remote control disabled 
 thread2.Run (); //parallel thread
#endif 
 timer1.SetRunState (1);
 timer2.SetRunState (1);

 Application->ProcessEvents ();

 Window4.SetBaseTimer ();

 sprintf (fname, "%s/parts_%s.pcf", home, boards_list[lab].name_);
 Window5.LoadConfig (fname);
 sprintf (fname, "%s/palias_%s.ppa", home, boards_list[lab_].name_);
 Window5.LoadPinAlias (fname);


 if ((!pboard->GetProcessorName ().Cmp ("atmega328p")) || (!pboard->GetProcessorName ().Cmp ("atmega2560")))
  {
   menu1_Tools_ArduinoBootloader.SetEnable (true);
  }
 else
  {
   menu1_Tools_ArduinoBootloader.SetEnable (false);
  }

#ifndef __EMSCRIPTEN__
 rcontrol_init (remotec_port);
#endif 
}

//Change  frequency

void
CPWindow1::combo1_EvOnComboChange(CControl * control)
{

 NSTEP = (int) (atof (combo1.GetText ()) * NSTEPKT);

 NSTEPJ = NSTEP / JUMPSTEPS;

 pboard->MSetFreq (NSTEP * NSTEPKF);
 Window4.SetBaseTimer ();


 Application->ProcessEvents ();

 tgo = 1;
 timer1.SetTime (100);
}

void
CPWindow1::saveprefs(lxString name, lxString value)
{
 char line[1024];
 char *pname;
 char *pvalue;

 for (int lc = 0; lc < (int) prefs.GetLinesCount (); lc++)
  {
   strncpy (line, prefs.GetLine (lc).c_str (), 1023);

   pname = strtok (line, "\t= ");
   strtok (NULL, " ");
   pvalue = strtok (NULL, "\"");

   if ((pname == NULL) || (pvalue == NULL))continue;

   if (lxString (pname) == name)
    {
     prefs.SetLine (name + lxT ("\t= \"") + value + lxT ("\""), lc);

     return;
    }
  }
 prefs.AddLine (name + lxT ("\t= \"") + value + lxT ("\""));
}

void
CPWindow1::_EvOnDestroy(CControl * control)
{
 rcontrol_server_end ();
 pboard->EndServers ();
 EndSimulation ();
}

void
CPWindow1::EndSimulation(void)
{
 char home[1024];
 char fname[1280];


 Window4.Hide ();
 Window5.Hide ();
#ifndef __EMSCRIPTEN__
 rcontrol_end ();
#endif
 timer1.SetRunState (0);
 timer2.SetRunState (0);
 msleep (100);
 while (status.status)
  {
   msleep (1);
   Application->ProcessEvents ();
  }
 tgo = 100000;
#ifndef _NOTHREAD    
 cpu_mutex->Lock ();
 cpu_cond->Signal ();
 cpu_mutex->Unlock ();
#endif 
 thread1.Destroy ();
 tgo = 0;

#ifndef __EMSCRIPTEN__
 thread2.Destroy ();
#endif

 //write options
 strcpy (home, (char*) lxGetUserDataDir (lxT ("picsimlab")).char_str ());

 lxCreateDir (home);

 sprintf (fname, "%s/picsimlab.ini", home);


 saveprefs (lxT ("picsimlab_version"), _VERSION_);
 saveprefs (lxT ("picsimlab_lab"), boards_list[lab].name_);
 saveprefs (lxT ("picsimlab_debug"), itoa (debug));
 saveprefs (lxT ("picsimlab_debugt"), itoa (debug_type));
 saveprefs (lxT ("picsimlab_debugp"), itoa (debug_port));
 saveprefs (lxT ("picsimlab_remotecp"), itoa (remotec_port));
 saveprefs (lxT ("picsimlab_position"), itoa (GetX ()) + lxT (",") + itoa (GetY ()));
 saveprefs (lxT ("picsimlab_scale"), ftoa (scale));
 saveprefs (lxT ("osc_on"), itoa (pboard->GetUseOscilloscope ()));
 saveprefs (lxT ("spare_on"), itoa (pboard->GetUseSpareParts ()));
#ifndef _WIN_
 saveprefs (lxT ("picsimlab_lser"), SERIALDEVICE);
#ifdef _USE_PICSTARTP_    
 saveprefs (lxT ("picsimlab_lprog"), PROGDEVICE);
#endif    
#else
 saveprefs ("picsimlab_wser", SERIALDEVICE);
#ifdef _USE_PICSTARTP_    
 saveprefs ("picsimlab_wprog", PROGDEVICE);
#endif
#endif

 if (PATH.size () > 0)
  {
   saveprefs (lxT ("picsimlab_lpath"), PATH);
  }
 else
  {
   saveprefs (lxT ("picsimlab_lpath"), " ");
  }
 saveprefs (lxT ("picsimlab_lfile"), FNAME);

 pboard->WritePreferences ();

 Window4.WritePreferences ();
 Window5.WritePreferences ();

 prefs.SaveToFile (fname);

 //write memory

 sprintf (fname, "%s/mdump_%s_%s.hex", home, boards_list[lab_].name_, (const char*) proc_.c_str ());

 pboard->MDumpMemory (fname);


 pboard->MEnd ();


 sprintf (fname, "%s/parts_%s.pcf", home, boards_list[lab_].name_);
 Window5.SaveConfig (fname);
 Window5.DeleteParts ();
 sprintf (fname, "%s/palias_%s.ppa", home, boards_list[lab_].name_);
 Window5.SavePinAlias (fname);


 delete pboard;
 pboard = NULL;

 //refresh window position to window reopen in same position
 GetX ();
 GetY ();

 scale = 1.0;

#ifndef _NOTHREAD    
 delete cpu_cond;
 delete cpu_mutex;
 cpu_cond = NULL;
 cpu_mutex = NULL;
#endif
}

void
CPWindow1::menu1_File_LoadHex_EvMenuActive(CControl * control)
{
#ifdef __EMSCRIPTEN__
 EM_ASM_ ({toggle_load_panel ();});
#else     
 filedialog1.SetType (lxFD_OPEN | lxFD_CHANGE_DIR);
 filedialog1.Run ();
#endif 
}

void
CPWindow1::menu1_File_SaveHex_EvMenuActive(CControl * control)
{
 pa = mcupwr;
 filedialog1.SetType (lxFD_SAVE | lxFD_CHANGE_DIR);
#ifdef __EMSCRIPTEN__
 filedialog1.SetDir ("/tmp/");
 filedialog1.SetFileName ("untitled.hex");
 filedialog1_EvOnClose (1);
#else 
 filedialog1.Run ();
#endif 
}

void
CPWindow1::filedialog1_EvOnClose(int retId)
{
 pa = mcupwr;
 mcupwr = 0;

 while (status.st[1] & ST_TH)usleep (100); //wait thread

 if (retId && (filedialog1.GetType () == (lxFD_OPEN | lxFD_CHANGE_DIR)))
  {

   LoadHexFile (filedialog1.GetFileName ());

   PATH = filedialog1.GetDir ();
   FNAME = filedialog1.GetFileName ();
   menu1_File_ReloadLast.SetEnable (1);
  }

 if (retId && (filedialog1.GetType () == (lxFD_SAVE | lxFD_CHANGE_DIR)))
  {
   pboard->MDumpMemory (filedialog1.GetFileName ());
#ifdef __EMSCRIPTEN__
   EM_ASM_ ({
            var filename = UTF8ToString ($0);
            var buf = FS.readFile (filename);
            var blob = new Blob ([buf],
             {
              "type" : "application/octet-stream" });
            var text = URL.createObjectURL (blob);

            var element = document.createElement ('a');
            element.setAttribute ('href', text);
            element.setAttribute ('download', filename);

            element.style.display = 'none';
            document.body.appendChild (element);

            element.click ();

            document.body.removeChild (element);
            URL.revokeObjectURL (text);
   }, filedialog1.GetFileName ().c_str ());
#endif 
  }

 mcupwr = pa;
}

void
CPWindow1::menu1_File_Exit_EvMenuActive(CControl * control)
{
 WDestroy ();
}

void
CPWindow1::menu1_Help_Contents_EvMenuActive(CControl * control)
{
#ifdef EXT_BROWSER
 //lxLaunchDefaultBrowser(lxT("file://")+share + lxT ("docs/picsimlab.html"));
 lxString stemp;
 stemp.Printf (lxT ("https://lcgamboa.github.io/picsimlab_docs/%s/index.html"), lxT (_VERSION_));
 lxLaunchDefaultBrowser (stemp);
#else 
 Window2.html1.SetLoadFile (share + lxT ("docs/picsimlab.html"));
 Window2.Show ();
#endif 
}

void
CPWindow1::menu1_Help_Board_EvMenuActive(CControl * control)
{
 char bname[20];
 strncpy (bname, boards_list[lab].name_, 12);

 char * ptr;
 //remove _ from names
 while ((ptr = strchr (bname, '_')))
  {
   strcpy (ptr, ptr + 1);
  }

 lxString stemp;
 stemp.Printf (lxT ("https://lcgamboa.github.io/picsimlab_docs/%s/%s.html"), lxT (_VERSION_), bname);
 lxLaunchDefaultBrowser (stemp);
}

void
CPWindow1::menu1_Help_About_Board_EvMenuActive(CControl * control)
{
 Message_sz (lxT ("Board ") + lxString (boards_list[lab].name) + lxT ("\nDeveloped by ") + pboard->GetAboutInfo (), 400, 200);
}

void
CPWindow1::menu1_Help_About_PICSimLab_EvMenuActive(CControl * control)
{
 lxString stemp;
 stemp.Printf (lxT ("Developed by L.C. Gamboa\n <lcgamboa@yahoo.com>\n Version: %s %s %s"), lxT (_VERSION_), lxT (_DATE_), lxT (_ARCH_));
 Message_sz (stemp, 400, 200);
}

void
CPWindow1::menu1_Help_Examples_EvMenuActive(CControl * control)
{

#ifdef EXT_BROWSER_EXAMPLES
 //lxLaunchDefaultBrowser(lxT("file://")+share + lxT ("docs/picsimlab.html"));
 lxLaunchDefaultBrowser (lxT ("https://lcgamboa.github.io/picsimlab_examples/examples/examples_index.html#board_" + lxString (boards_list[lab].name_) + lxT ("_") + pboard->GetProcessorName ()));
 WDestroy ();
#else 
 OldPath = filedialog2.GetDir ();

 filedialog2.SetDir (share + lxT ("/docs/hex/board_") + lxString (boards_list[lab].name_) + lxT ("/") + pboard->GetProcessorName () + lxT ("/"));

 menu1_File_LoadWorkspace_EvMenuActive (control);
#endif
}


//Resize

void
CPWindow1::_EvOnShow(CControl * control)
{
 need_resize = 0;
}

void
CPWindow1::menu1_File_Configure_EvMenuActive(CControl * control)
{
 Window3.ShowExclusive ();
}

void
CPWindow1::LoadHexFile(lxString fname)
{
 int pa;

 pa = mcupwr;
 mcupwr = 0;

 timer1.SetRunState (0);
 msleep (100);
 while (status.status & 0x41)
  {
   msleep (1);
   Application->ProcessEvents ();
  }

 pboard->MEnd ();
 pboard->MSetSerial (SERIALDEVICE);

 switch (pboard->MInit (pboard->GetProcessorName (), fname.char_str (), NSTEP * NSTEPKF))
  {
  case HEX_NFOUND:
   Message (lxT ("File not found!"));
   mcurun = 0;
   break;
  case HEX_CHKSUM:
   Message (lxT ("File checksum error!"));
   pboard->MEraseFlash ();
   mcurun = 0;
   break;
  case 0:
   mcurun = 1;
   break;
  }

 pboard->Reset ();


 if (mcurun)
  SetTitle (lxT ("PICSimLab - ") + lxString (boards_list[lab].name) + lxT (" - ") + pboard->GetProcessorName () + lxT (" - ") + basename (filedialog1.GetFileName ()));
 else
  SetTitle (lxT ("PICSimLab - ") + lxString (boards_list[lab].name) + lxT (" - ") + pboard->GetProcessorName ());


 mcupwr = pa;
 timer1.SetRunState (1);

#ifdef NO_DEBUG
 statusbar1.SetField (1, lxT (" "));
#else 
 if (debug)
  {
   int ret = pboard->DebugInit (debug_type);
   if (ret < 0)
    {
     statusbar1.SetField (1, lxT ("Debug: Error"));
    }
   else
    {
     statusbar1.SetField (1, lxT ("Debug: ") + pboard->GetDebugName () + ":" + itoa (debug_port));
    }
  }
 else
  {
   statusbar1.SetField (1, lxT ("Debug: Off"));
  }
#endif
}

void
CPWindow1::menu1_File_ReloadLast_EvMenuActive(CControl * control)
{
 LoadHexFile (FNAME);
}

void
CPWindow1::board_Event(CControl * control)
{
 pboard->board_Event (control);
}

void
CPWindow1::menu1_Modules_Oscilloscope_EvMenuActive(CControl * control)
{
 pboard->SetUseOscilloscope (1);
 Window4.Show ();
 Window4.timer1.SetRunState (1);
}

board *
CPWindow1::GetBoard(void)
{
 return pboard;
}

void
CPWindow1::SetCpuState(unsigned char cs)
{
 cpustate = cs;
}

void
CPWindow1::menu1_Modules_Spareparts_EvMenuActive(CControl * control)
{


 pboard->SetUseSpareParts (1);
 Window5.Show ();
 Window5.timer1.SetRunState (1);
 pboard->Reset ();

}


//Change board

void
CPWindow1::menu1_EvBoard(CControl * control)
{
 lab_ = lab;
 lab = atoi (((CItemMenu*) control)->GetName ());

 FNAME = lxT (" ");
 EndSimulation ();
 Configure (HOME);
 need_resize = 0;
}

//change microcontroller

void
CPWindow1::menu1_EvMicrocontroller(CControl * control)
{
 proc_ = pboard->GetProcessorName ();
 pboard->SetProcessorName (((CItemMenu*) control)->GetText ());

 SetTitle (lxT ("PICSimLab - ") + lxString (boards_list[lab].name) + lxT (" - ") + pboard->GetProcessorName ());

 FNAME = lxT (" ");
 EndSimulation ();
 Configure (HOME);
 need_resize = 0;
}

void
CPWindow1::togglebutton1_EvOnToggleButton(CControl * control)
{
 int osc_on = pboard->GetUseOscilloscope ();
 int spare_on = pboard->GetUseSpareParts ();

 debug = togglebutton1.GetCheck ();

 EndSimulation ();
 Configure (HOME);
 need_resize = 0;

 if (osc_on) menu1_Modules_Oscilloscope_EvMenuActive (this);
 if (spare_on) menu1_Modules_Spareparts_EvMenuActive (this);


}

void
CPWindow1::menu1_File_SaveWorkspace_EvMenuActive(CControl * control)
{
 filedialog2.SetType (lxFD_SAVE | lxFD_CHANGE_DIR);
#ifdef __EMSCRIPTEN__
 filedialog2.SetDir ("/tmp/");
 filedialog2.SetFileName ("untitled.pzw");
 filedialog2_EvOnClose (1);
#else 
 filedialog2.Run ();
#endif 
}

//legacy format support before 0.8.2
static const char old_board_names[6][20] = {"Breadboard", "McLab1", "K16F", "McLab2", "PICGenios", "Arduino_Uno"};

void
CPWindow1::LoadWorkspace(lxString fnpzw)
{
 char home[1024];
 char fzip[1280];

 if (!lxFileExists (fnpzw))
  {
   printf ("PICSimLab: file %s not found!\n", (const char *) fnpzw.c_str ());
   return;
  }
 if (!fnpzw.Contains (".pzw"))
  {
   printf ("PICSimLab: file %s is not a .pzw file!\n", (const char *) fnpzw.c_str ());
   return;
  }
 //write options
 strncpy (fzip, (char*) lxGetTempDir (lxT ("picsimlab")).char_str (), 1023);
 strncat (fzip, "/", 1023);

 strncpy (home, (char*) lxGetTempDir (lxT ("picsimlab")).char_str (), 1023);
 strncat (home, "/picsimlab_workspace/", 1023);

 lxRemoveDir (home);

 lxUnzipDir (fnpzw, fzip);

 EndSimulation ();

 snprintf (fzip, 1279, "%s/picsimlab.ini", home);
 lxStringList prefsw;
 prefsw.Clear ();
 int lc;
 char * value;
 char * name;
#ifdef LEGACY081 
 char name_[100];
 char value_[400];
 int llab = 0;
#endif

 char line[1024];
 if (prefsw.LoadFromFile (fzip))
  {
   for (lc = 0; lc < (int) prefsw.GetLinesCount (); lc++)
    {
     strncpy (line, prefsw.GetLine (lc).c_str (), 1023);
     name = strtok (line, "\t= ");
     strtok (NULL, " ");
     value = strtok (NULL, "\"");
     if ((name == NULL) || (value == NULL))continue;

     if (!strcmp (name, "picsimlab_version"))
      {
       int maj, min, rev, ser;
       int maj_, min_, rev_, ser_;

       pzw_ver = value;

       sscanf (_VERSION_, "%i.%i.%i", &maj, &min, &rev);
       ser = maj * 10000 + min * 100 + rev;

       sscanf (value, "%i.%i.%i", &maj_, &min_, &rev_);
       ser_ = maj_ * 10000 + min_ * 100 + rev_;

       if (ser_ > ser)
        {
         printf ("PICSimLab: .pzw file version %i newer than PICSimLab %i!\n", ser_, ser);
         RegisterError ("The loaded workspace was made with a newer version " + pzw_ver + ".\n Please update your PICSimLab " _VERSION_ " .");
        }

       continue;
      }
#ifndef LEGACY081
     saveprefs (name, value);
#else     
     strncpy (name_, name, 99);
     strncpy (value_, value, 399);

     //legacy mode compatibility
     if (!strcmp (name_, "lab"))
      {
       char oldname[1500];

       strcpy (name_, "picsimlab_lab");
       sscanf (value_, "%i", &llab);

       strcpy (value_, old_board_names[llab]);

       snprintf (oldname, 1499, "%sparts_%02i.pcf", home, llab);

       if (lxFileExists (oldname))
        {
         char newname[1500];
         snprintf (newname, 1499, "%sparts_%s.pcf", home, old_board_names[llab]);
         lxRenameFile (oldname, newname);
        }
      }
     if (!strcmp (name_, "debug"))
      {
       strcpy (name_, "picsimlab_debug");
      }
     if (!strcmp (name_, "position"))
      {
       strcpy (name_, "picsimlab_position");
      }
     if (!strcmp (name_, "lfile"))
      {
       strcpy (name_, "picsimlab_lfile");
      }
     if (!strcmp (name_, "clock"))
      {
       sprintf (name_, "%s_clock", old_board_names[llab]);
      }
     if (!strcmp (name_, "p0_proc"))
      {
       sprintf (name_, "%s_proc", old_board_names[0]);

       char oldname[1500];
       char newname[1500];
       snprintf (oldname, 1499, "%smdump_00_%s.hex", home, value);
       snprintf (newname, 1499, "%smdump_%s_%s.hex", home, old_board_names[0], value);
       lxRenameFile (oldname, newname);
      }
     if (!strcmp (name_, "p1_proc"))
      {
       sprintf (name_, "%s_proc", old_board_names[1]);

       char oldname[1500];
       char newname[1500];
       snprintf (oldname, 1499, "%smdump_01_%s.hex", home, value);
       snprintf (newname, 1499, "%smdump_%s_%s.hex", home, old_board_names[1], value);
       lxRenameFile (oldname, newname);
      }
     if (!strcmp (name_, "p2_proc"))
      {
       sprintf (name_, "%s_proc", old_board_names[2]);

       char oldname[1500];
       char newname[1500];
       snprintf (oldname, 1499, "%smdump_02_%s.hex", home, value);
       snprintf (newname, 1499, "%smdump_%s_%s.hex", home, old_board_names[2], value);
       lxRenameFile (oldname, newname);
      }
     if (!strcmp (name_, "p3_proc"))
      {
       sprintf (name_, "%s_proc", old_board_names[3]);

       char oldname[1500];
       char newname[1500];
       snprintf (oldname, 1499, "%smdump_03_%s.hex", home, value);
       snprintf (newname, 1499, "%smdump_%s_%s.hex", home, old_board_names[3], value);
       lxRenameFile (oldname, newname);
      }
     if (!strcmp (name_, "p4_proc"))
      {
       sprintf (name_, "%s_proc", old_board_names[4]);

       char oldname[1500];
       char newname[1500];
       snprintf (oldname, 1499, "%smdump_04_%s.hex", home, value);
       snprintf (newname, 1499, "%smdump_%s_%s.hex", home, old_board_names[4], value);
       lxRenameFile (oldname, newname);
      }
     if (!strcmp (name_, "p5_proc"))
      {
       sprintf (name_, "%s_proc", old_board_names[5]);

       char oldname[1500];
       char newname[1500];
       snprintf (oldname, 1499, "%smdump_05_%s.hex", home, value);
       snprintf (newname, 1499, "%smdump_%s_%s.hex", home, old_board_names[5], value);
       lxRenameFile (oldname, newname);
      }

     char * ptr;
     if ((ptr = strstr (name, "p0_")))
      {
       sprintf (name_, "%s_%s", old_board_names[0], ptr + 3);
      }
     if ((ptr = strstr (name, "p1_")))
      {
       sprintf (name_, "%s_%s", old_board_names[1], ptr + 3);
      }
     if ((ptr = strstr (name, "p2_")))
      {
       sprintf (name_, "%s_%s", old_board_names[2], ptr + 3);
      }
     if ((ptr = strstr (name, "p3_")))
      {
       sprintf (name_, "%s_%s", old_board_names[3], ptr + 3);
      }
     if ((ptr = strstr (name, "p4_")))
      {
       sprintf (name_, "%s_%s", old_board_names[4], ptr + 3);
      }
     if ((ptr = strstr (name, "p5_")))
      {
       sprintf (name_, "%s_%s", old_board_names[5], ptr + 3);
      }

     saveprefs (name_, value_);
#endif     
    }
  }
 prefs.SaveToFile (fzip);

 Configure (home);

 need_resize = 0;

#ifdef CONVERTER_MODE
 fnpzw.replace (fnpzw.Length () - 4, 5, "_.pzw");
 cvt_fname = fnpzw;
#else
 snprintf (fzip, 1279, "%s/Readme.html", home);
 if (lxFileExists (fzip))
  {
#ifdef EXT_BROWSER
#ifndef __EMSCRIPTEN__
   lxLaunchDefaultBrowser (lxT ("file://") + lxString (fzip));
#endif
#else    
   Window2.html1.SetLoadFile (fzip);
   Window2.Show ();
#endif   
  }
 else
  {
   snprintf (fzip, 1279, "%s/Readme.txt", home);
   if (lxFileExists (fzip))
    {
#ifdef EXT_BROWSER
#ifndef __EMSCRIPTEN__
     lxLaunchDefaultBrowser (lxT ("file://") + lxString (fzip));
#endif
#else      
     Window2.html1.SetLoadFile (fzip);
     Window2.Show ();
#endif     
    }
  }
#endif 

}

void
CPWindow1::SaveWorkspace(lxString fnpzw)
{
 char home[1024];
 char fname[1280];

#if  !defined(__EMSCRIPTEN__) &&  !defined(CONVERTER_MODE)
 if (lxFileExists (fnpzw))
  {

   if (!Dialog (lxString ("Overwriting file: ") + basename (fnpzw) + "?"))
    return;
  }
#endif

 //write options

 strncpy (home, (char*) lxGetUserDataDir (lxT ("picsimlab")).char_str (), 1023);
 snprintf (fname, 1279, "%s/picsimlab.ini", home);
 prefs.SaveToFile (fname);

 strncpy (home, (char*) lxGetTempDir (lxT ("picsimlab")).char_str (), 1023);
 strncat (home, "/picsimlab_workspace/", 1023);

#ifdef CONVERTER_MODE
 snprintf (fname, 1279, "rm -rf %s/*.ini", home);
 system (fname);
 snprintf (fname, 1279, "rm -rf %s/*.pcf", home);
 system (fname);
 snprintf (fname, 1279, "rm -rf %s/*.hex", home);
 system (fname);
#else 
 lxRemoveDir (home);
 lxCreateDir (home);
#endif

 snprintf (fname, 1279, "%s/picsimlab.ini", home);
 prefs.Clear ();
 saveprefs (lxT ("picsimlab_version"), _VERSION_);
 saveprefs (lxT ("picsimlab_lab"), boards_list[lab].name_);
 saveprefs (lxT ("picsimlab_debug"), itoa (debug));
 saveprefs (lxT ("picsimlab_debugt"), itoa (debug_type));
 saveprefs (lxT ("picsimlab_debugp"), itoa (debug_port));
 saveprefs (lxT ("picsimlab_position"), itoa (GetX ()) + lxT (",") + itoa (GetY ()));
 saveprefs (lxT ("picsimlab_scale"), ftoa (scale));
 saveprefs (lxT ("osc_on"), itoa (pboard->GetUseOscilloscope ()));
 saveprefs (lxT ("spare_on"), itoa (pboard->GetUseSpareParts ()));
 saveprefs (lxT ("picsimlab_lfile"), lxT (" "));

 pboard->WritePreferences ();

 if (pboard->GetUseOscilloscope ())
  Window4.WritePreferences ();

 if (pboard->GetUseSpareParts ())
  {
   Window5.WritePreferences ();
  }

 prefs.SaveToFile (fname);

 //write memory
 snprintf (fname, 1279, "%s/mdump_%s_%s.hex", home, boards_list[lab_].name_, (const char*) proc_.c_str ());

 pboard->MDumpMemory (fname);

 if (pboard->GetUseSpareParts ())
  {
   snprintf (fname, 1279, "%s/parts_%s.pcf", home, boards_list[lab_].name_);
   Window5.SaveConfig (fname);
   sprintf (fname, "%s/palias_%s.ppa", home, boards_list[lab_].name_);
   Window5.SavePinAlias (fname);
  }

 lxZipDir (home, fnpzw);

 lxRemoveDir (home);


 strncpy (home, (char*) lxGetUserDataDir (lxT ("picsimlab")).char_str (), 1023);
 snprintf (fname, 1279, "%s/picsimlab.ini", home);
 prefs.Clear ();
 prefs.LoadFromFile (fname);

#ifdef __EMSCRIPTEN__
 EM_ASM_ ({
          var filename = UTF8ToString ($0);
          var buf = FS.readFile (filename);
          var blob = new Blob ([buf],
           {
            "type" : "application/octet-stream" });
          var text = URL.createObjectURL (blob);

          var element = document.createElement ('a');
          element.setAttribute ('href', text);
          element.setAttribute ('download', filename);

          element.style.display = 'none';
          document.body.appendChild (element);

          element.click ();

          document.body.removeChild (element);
          URL.revokeObjectURL (text);
 }, fnpzw.c_str ());
#endif 

#ifdef CONVERTER_MODE
 WDestroy ();
#endif
}

void
CPWindow1::menu1_File_LoadWorkspace_EvMenuActive(CControl * control)
{
#ifdef __EMSCRIPTEN__
 EM_ASM_ ({toggle_load_panel ();});
#else     
 filedialog2.SetType (lxFD_OPEN | lxFD_CHANGE_DIR);
 filedialog2.Run ();
#endif
}

void
CPWindow1::filedialog2_EvOnClose(int retId)
{
 if (retId && (filedialog2.GetType () == (lxFD_OPEN | lxFD_CHANGE_DIR)))
  {
   LoadWorkspace (filedialog2.GetFileName ());
   if (OldPath.size () > 1)
    {
     filedialog2.SetDir (OldPath);
     OldPath = lxT ("");
    }
  }

 if (retId && (filedialog2.GetType () == (lxFD_SAVE | lxFD_CHANGE_DIR)))
  {
   SaveWorkspace (filedialog2.GetFileName ());
  }

}

void
CPWindow1::menu1_Tools_SerialTerm_EvMenuActive(CControl * control)
{
#ifdef _WIN_  
 lxExecute (share + lxT ("/../tools/cutecom/cutecom.exe"));
#else
 //lxExecute (dirname(lxGetExecutablePath ())+"/cutecom", lxEXEC_MAKE_GROUP_LEADER);
 //using system binary
 lxExecute ("cutecom");

 if (!(lxFileExists (dirname (lxGetExecutablePath ()) + "/cutecom")
       || lxFileExists ("/usr/bin/cutecom")
       || lxFileExists ("/usr/local/bin/cutecom")))
  {
   printf ("cutecom não instalado\n");
   Message_sz ("The cutecom application is not found!\n\nPlease install cutecom in your system!\n\n In Debian based distro use: sudo apt-get install cutecom", 500, 240);
  }
#endif  
}

void
CPWindow1::menu1_Tools_SerialRemoteTank_EvMenuActive(CControl * control)
{
#ifdef _WIN_  
 lxExecute (share + lxT ("/../srtank.exe"));
#else

 lxExecute (dirname (lxGetExecutablePath ()) + "/srtank");
#endif  
}

void
CPWindow1::menu1_Tools_Esp8266ModemSimulator_EvMenuActive(CControl * control)
{
#ifdef _WIN_  
 lxExecute (share + lxT ("/../espmsim.exe"));
#else
 lxExecute (dirname (lxGetExecutablePath ()) + "/espmsim");
#endif  
}

void
CPWindow1::menu1_Tools_ArduinoBootloader_EvMenuActive(CControl * control)
{
 LoadHexFile (share + "bootloaders/arduino_" + pboard->GetProcessorName () + ".hex");
}

void
CPWindow1::menu1_Tools_MPLABXDebuggerPlugin_EvMenuActive(CControl * control)
{
 lxLaunchDefaultBrowser (lxT ("https://github.com/lcgamboa/picsimlab_md/releases"));
}

void
CPWindow1::menu1_Tools_PinViewer_EvMenuActive(CControl * control)
{
#ifdef _WIN_  
 lxExecute (share + lxT ("/../PinViewer.exe " + itoa (remotec_port)));
#else
 lxExecute (dirname (lxGetExecutablePath ()) + "/PinViewer " + itoa (remotec_port));
#endif  
}

void
CPWindow1::Set_mcudbg(int pd)
{
 mcudbg = pd;

 if (mcudbg)
  {
   SetJUMPSTEPS (0);
  }
 else
  {
   SetJUMPSTEPS (DEFAULTJS);
  }
}

void
CPWindow1::SetJUMPSTEPS(int js)
{
 JUMPSTEPS = js;
 if (JUMPSTEPS)
  {
   NSTEPJ = NSTEP / JUMPSTEPS;
  }
 else
  {
   NSTEPJ = NSTEP;
  }
}

void
CPWindow1::SetClock(float clk)
{
 if (clk < 1)
  {
   combo1.SetText (lxString ().Format ("%2.1f", clk));
  }
 else
  {
   combo1.SetText (lxString ().Format ("%2.0f", clk));
  }

 NSTEP = (int) (atof (combo1.GetText ()) * NSTEPKT);
 NSTEPJ = NSTEP / JUMPSTEPS;
 pboard->MSetFreq (NSTEP * NSTEPKF);
}

float
CPWindow1::GetClock(void)
{
 return atof (combo1.GetText ());
}

void
CPWindow1::Set_debug_port(unsigned short dp)
{
 debug_port = dp;
 if (debug)
  {
   togglebutton1.SetCheck (0);
   togglebutton1_EvOnToggleButton (this);
  }
}

void
CPWindow1::Set_remotec_port(unsigned short rcp)
{
 remotec_port = rcp;
#ifndef __EMSCRIPTEN__
 rcontrol_end ();
 rcontrol_init (remotec_port);
#endif 
}

void
CPWindow1::SetToDestroy(void)
{
 settodestroy = 1;
}


#ifdef __EMSCRIPTEN__
extern "C"
{

 void
 file_ready(const char *fname)
 {
  if (strstr (fname, ".pzw"))
   {
    printf ("Loading .pzw...\n");
    Window1.filedialog2.SetType (lxFD_OPEN | lxFD_CHANGE_DIR);
    Window1.filedialog2.SetDir ("/tmp/");
    Window1.filedialog2.SetFileName (fname);
    Window1.filedialog2_EvOnClose (1);
   }
  else if (strstr (fname, ".hex"))
   {
    printf ("Loading .hex...\n");
    Window1.filedialog1.SetType (lxFD_OPEN | lxFD_CHANGE_DIR);
    Window1.filedialog1.SetDir ("/tmp/");
    Window1.filedialog1.SetFileName (fname);
    Window1.filedialog1_EvOnClose (1);
   }
  else if (strstr (fname, ".pcf"))
   {
    char buff[1024];
    strncpy (buff, "/tmp/", 1023);
    strncat (buff, fname, 1023);
    printf ("Loading .pcf...\n");
    Window5.LoadConfig (buff);
    Window1.menu1_Modules_Spareparts_EvMenuActive (&Window1);
   }
  else if (strstr (fname, ".ppa"))
   {
    char buff[1024];
    strncpy (buff, "/tmp/", 1023);
    strncat (buff, fname, 1023);
    printf ("Loading .ppa...\n");
    Window5.LoadPinAlias (buff);
   }
  else
   {
    printf ("Unknow file %s !!\n", fname);
   }
 }
}
#endif

void
CPWindow1::RegisterError(const lxString error)
{
 Errors.AddLine (error);
}
