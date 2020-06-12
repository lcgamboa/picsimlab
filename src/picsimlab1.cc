/* ########################################################################

   PICsimLab - PIC laboratory simulator

   ########################################################################

   Copyright (c) : 2010-2020  Luis Claudio Gamboa Lopes

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


#include"picsimlab1.h"
#include"picsimlab1_d.cc"

CPWindow1 Window1;

//Implementation

#include"picsimlab2.h"
#include"picsimlab3.h"
#include"picsimlab4.h"
#include"picsimlab5.h"

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



int crt;

void
CPWindow1::timer1_EvOnTime(CControl * control)
{
 status.st[0] |= ST_T1;

#ifdef _NOTHREAD
 if (timer1.GetOverTime () < 10)
  {
   label1.SetColor (0, 0, 0);
  }
 else
  {
   label1.SetColor (255, 0, 0);
  }
#else   
 if (!ondraw)
  {
   if (!crt)
    label1.SetColor (0, 0, 0);
   crt = 0;
  }
 else
  {
   label1.SetColor (255, 0, 0);
   crt = 1;
  }
#endif
 pboard->Draw (&draw1, scale);
 label1.Draw ();
 ondraw = 1;

 status.st[0] &= ~ST_T1;

 if (!tgo)
  tgo = 1; //thread sync
}

void
CPWindow1::thread1_EvThreadRun(CControl*)
{

 do
  {

   if (tgo)
    {
     status.st[1] |= ST_TH;
     tgo = 0;
     pboard->Run_CPU ();
     if (debug)pboard->DebugLoop ();
     ondraw = 0;
     status.st[1] &= ~ST_TH;
    }
   else
    {
     usleep (1);
    }
  }
 while (!thread1.TestDestroy ());
}

void
CPWindow1::timer2_EvOnTime(CControl * control)
{
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
 status.st[0] &= ~ST_T2;
};

void
CPWindow1::draw1_EvMouseButtonPress(CControl * control, uint button, uint x, uint y, uint state)
{

 x = x / scale;
 y = y / scale;

 pboard->EvMouseButtonPress (button, x, y, state);

};

void
CPWindow1::draw1_EvMouseButtonRelease(CControl * control, uint button, uint x, uint y, uint state)
{

 x = x / scale;
 y = y / scale;

 pboard->EvMouseButtonRelease (button, x, y, state);

};

void
CPWindow1::draw1_EvKeyboardPress(CControl * control, const uint key, const uint hkey, const uint mask)
{
 pboard->EvKeyPress (key, mask);
};

void
CPWindow1::draw1_EvKeyboardRelease(CControl * control, const uint key, const uint hkey, const uint mask)
{
 pboard->EvKeyRelease (key, mask);
};

void
CPWindow1::_EvOnCreate(CControl * control)
{
 char home[1024];
 lxFileName fn;

 if (!create)
  {
   PATH = lxGetCwd ();

#ifndef _WIN_
   share = String (_SHARE_);
#else
   share = dirname (lxGetExecutablePath ()) + lxT ("/") + String (_SHARE_);
#endif



   if (Application->Aargc == 2)
    {
     fn.Assign (Application->Aargv[1]);
     fn.MakeAbsolute ();
    }
  }

 //load options


 strncpy (home, (char*) lxGetUserDataDir (_T ("picsimlab")).char_str (), 1023);

 Configure (control, home);

 if (!create)
  {
   //search for file name 
   if (Application->Aargc == 2)
    {
     LoadWorkspace (fn.GetFullPath ());
    }
  }
 create++;

};

void
CPWindow1::Configure(CControl * control, const char * home)
{

 char line[1024];
 char fname[1024];

 char *name;
 char *value;

 int i, j;
 int lc;
 float clk;

 String status;

 mcurun = 1;
 mcupwr = 1;
 mcurst = 0;
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
       if (!strcmp ("lser", name))strcpy (SERIALDEVICE, value);
#ifdef _USE_PICSTARTP_      
       if (!strcmp ("lprog", name))strcpy (PROGDEVICE, value);
#endif      
#else
       if (!strcmp ("wser", name))strcpy (SERIALDEVICE, value);
#ifdef _USE_PICSTARTP_      
       if (!strcmp ("wprog", name))strcpy (PROGDEVICE, value);
#endif      
#endif

       if (!strcmp (name, "lab"))
        {
         sscanf (value, "%i", &i);

         lab = i;
         lab_ = i;

         pboard = create_board (&lab, &lab_);

         menu1_Microcontroller.DestroyChilds ();
         String sdev = pboard->GetSupportedDevices ();
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
          }



        }
       if (!strcmp (name, "clock"))
        {
         sscanf (value, "%f", &clk);
         if (clk < 1)
          {
           combo1.SetText (String ().Format ("%2.1f", clk));
          }
         else
          {
           combo1.SetText (String ().Format ("%2.0f", clk));
          }
         //combo1_EvOnComboChange(control);
         NSTEP = (int) (atof (combo1.GetText ()) * NSTEPKT);
         NSTEPJ = NSTEP / JUMPSTEPS;
         pboard->MSetFreq (NSTEP * NSTEPKF);
        }

       if (!strcmp (name, "debug"))
        {
         sscanf (value, "%i", &debug);
         togglebutton1.SetCheck (debug);
        }

       if (!strcmp (name, "osc_on"))
        {
         sscanf (value, "%i", &osc_on);
        }

       if (!strcmp (name, "spare_on"))
        {
         sscanf (value, "%i", &spare_on);
        }

       if (!strcmp (name, "spare_on"))
        {
         sscanf (value, "%i", &spare_on);
        }

       if (!strcmp (name, "position"))
        {
         sscanf (value, "%i,%i", &i, &j);
         SetX (i);
         SetY (j);
        }


       if (!strcmp (name, "lpath"))
        {
         PATH = String (value, lxConvUTF8);
        }

       if (!strcmp (name, "lfile"))
        {
         FNAME = String (value, lxConvUTF8);
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
 else
  {
   printf ("Error open config file \"%s\"!\n", fname);

   lab = 1; //default  
   lab_ = 1; //default  

   pboard = new cboard_1 ();

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

 proc_ = pboard->GetProcessorName ();

 SetTitle (lxT ("PICSimLab - ") + String (boards_list[lab]) + lxT (" - ") + pboard->GetProcessorName ());



 filedialog1.SetDir (PATH);


 draw1.SetImgFileName (share + lxT ("boards/") + pboard->GetPictureFileName (), scale, scale);

 pboard->MSetSerial (SERIALDEVICE);

 sprintf (fname, "%s/mdump_%02i_%s.hex", home, lab, (const char *) pboard->GetProcessorName ().c_str ());


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
   if (pboard->DebugInit () == 0)
    statusbar1.SetField (1, status + lxT ("    Debug: On"));
   else
    statusbar1.SetField (1, status + lxT ("    Debug: Error"));
  }
 else
  {
   statusbar1.SetField (1, status + lxT ("    Debug: Off"));
  }
#endif

 statusbar1.SetField (0, lxT ("Running..."));


 thread1.Run (); //parallel thread
 timer1.SetRunState (1);
 timer2.SetRunState (1);

 Application->ProcessEvents ();

 Window4.SetBaseTimer ();

 sprintf (fname, "%s/parts_%02i.pcf", home, lab);
 Window5.LoadConfig (fname);

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
};

void
CPWindow1::saveprefs(String name, String value)
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

   if (String (pname) == name)
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
 char home[1024];
 char fname[1280];


 Window4.Hide ();
 Window5.Hide ();

 timer1.SetRunState (0);
 timer2.SetRunState (0);
 msleep (100);
 while (status.status)
  {
   msleep (1);
   Application->ProcessEvents ();
  };
 thread1.Destroy ();



 //write options
 strcpy (home, (char*) lxGetUserDataDir (_T ("picsimlab")).char_str ());

 lxCreateDir (home);

 sprintf (fname, "%s/picsimlab.ini", home);



 saveprefs (lxT ("lab"), String ().Format ("%i", lab));
 saveprefs (lxT ("clock"), combo1.GetText ());
 saveprefs (lxT ("debug"), itoa (debug));
 saveprefs (lxT ("position"), itoa (GetX ()) + lxT (",") + itoa (GetY ()));
 saveprefs (lxT ("osc_on"), itoa (pboard->GetUseOscilloscope ()));
 saveprefs (lxT ("spare_on"), itoa (pboard->GetUseSpareParts ()));
#ifndef _WIN_
 saveprefs (lxT ("lser"), SERIALDEVICE);
#ifdef _USE_PICSTARTP_    
 saveprefs (lxT ("lprog"), PROGDEVICE);
#endif    
#else
 saveprefs ("wser", SERIALDEVICE);
#ifdef _USE_PICSTARTP_    
 saveprefs ("wprog", PROGDEVICE);
#endif
#endif
 saveprefs (lxT ("lpath"), PATH);
 saveprefs (lxT ("lfile"), FNAME);

 pboard->WritePreferences ();

 Window4.WritePreferences ();
 Window5.WritePreferences ();

 prefs.SaveToFile (fname);

 //write memory

 sprintf (fname, "%s/mdump_%02i_%s.hex", home, lab_, (const char*) proc_.c_str ());

 pboard->MDumpMemory (fname);


 pboard->MEnd ();


 sprintf (fname, "%s/parts_%02i.pcf", home, lab_);
 Window5.SaveConfig (fname);
 Window5.DeleteParts ();

 delete pboard;
 pboard = NULL;

 //refresh window position to window reopen in same position
 GetX ();
 GetY ();

};

void
CPWindow1::menu1_File_LoadHex_EvMenuActive(CControl * control)
{
 filedialog1.SetType (lxFD_OPEN | lxFD_CHANGE_DIR);
 filedialog1.Run ();
}

void
CPWindow1::menu1_File_SaveHex_EvMenuActive(CControl * control)
{
 pa = mcupwr;
 filedialog1.SetType (lxFD_SAVE | lxFD_CHANGE_DIR);
 filedialog1.Run ();
}

void
CPWindow1::filedialog1_EvOnClose(int retId)
{
 pa = mcupwr;
 mcupwr = 0;
 
 while(status.st[1] & ST_TH)usleep(100);//wait thread
 
 if (retId && (filedialog1.GetType () == (lxFD_OPEN | lxFD_CHANGE_DIR)))
  {
   pboard->MEnd ();
   pboard->MSetSerial (SERIALDEVICE);

   switch (pboard->MInit (pboard->GetProcessorName (), filedialog1.GetFileName ().char_str (), NSTEP * NSTEPKF))
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
    SetTitle (lxT ("PICSimLab - ") + String (boards_list[lab]) + lxT (" - ") + pboard->GetProcessorName () + lxT (" - ") + basename (filedialog1.GetFileName ()));
   else
    SetTitle (lxT ("PICSimLab - ") + String (boards_list[lab]) + lxT (" - ") + pboard->GetProcessorName ());



   PATH = filedialog1.GetDir ();
   FNAME = filedialog1.GetFileName ();
   menu1_File_ReloadLast.SetEnable (1);
  }

 if (retId && (filedialog1.GetType () == (lxFD_SAVE | lxFD_CHANGE_DIR)))
  {
   pboard->MDumpMemory (filedialog1.GetFileName ());
   #ifdef __EMSCRIPTEN__
   EM_ASM_({
	   var filename=UTF8ToString($0);
           var buf = FS.readFile(filename);
           var blob = new Blob([buf],  {"type" : "application/octet-stream" });
           var text = URL.createObjectURL(blob);

	   var element = document.createElement('a');
           element.setAttribute('href', text);
           element.setAttribute('download', filename);

           element.style.display = 'none';
           document.body.appendChild(element);

           element.click();

           document.body.removeChild(element);
           URL.revokeObjectURL(text);
	  },filedialog1.GetFileName ().c_str ());
#endif 
  }

 mcupwr = pa;
}

void
CPWindow1::menu1_File_Exit_EvMenuActive(CControl * control)
{
 WDestroy ();
};

void
CPWindow1::menu1_Help_Contents_EvMenuActive(CControl * control)
{
#ifdef EXT_BROWSER
 //lxLaunchDefaultBrowser(lxT("file://")+share + lxT ("docs/picsimlab.html"));
 lxLaunchDefaultBrowser (lxT ("https://lcgamboa.github.io/picsimlab/"));
#else 
 Window2.html1.SetLoadFile (share + lxT ("docs/picsimlab.html"));
 Window2.Show ();
#endif 
};

void
CPWindow1::menu1_Help_About_EvMenuActive(CControl * control)
{
 Message (lxT ("Developed by L.C. Gamboa\n <lcgamboa@yahoo.com>\n Version: ") + String (lxT (_VERSION_)));
};

void
CPWindow1::menu1_Help_Examples_EvMenuActive(CControl * control)
{

#ifdef EXT_BROWSER_EXAMPLES
 //lxLaunchDefaultBrowser(lxT("file://")+share + lxT ("docs/picsimlab.html"));
 lxLaunchDefaultBrowser (lxT ("https://lcgamboa.github.io/picsimlab/examples/examples_index.html#board_" + itoa (lab) + lxT ("_") + pboard->proc));
#else 
 OldPath = filedialog2.GetDir ();

 filedialog2.SetDir (share + lxT ("/docs/hex/board_") + itoa (lab) + lxT ("/") + pboard->GetProcessorName () + lxT ("/"));

 menu1_File_LoadWorkspace_EvMenuActive (control);
#endif
}


//Resize

void
CPWindow1::_EvOnShow(CControl * control)
{
 double scalex, scaley;

 if (timer1.GetRunState ())
  {
   scalex = ((Window1.GetWidth () - 185)*1.0) / plWidth;
#ifdef _WIN_
   scaley = ((Window1.GetHeight () - 75)*1.0) / plHeight;
#else
   scaley = ((Window1.GetHeight () - 90)*1.0) / plHeight;
#endif

   if (scalex < 0.1)scalex = 0.1;
   if (scaley < 0.1)scaley = 0.1;
   if (scalex > 4)scalex = 4;
   if (scaley > 4)scaley = 4;

   if (scalex < scaley)
    scale = scalex;
   else
    scale = scaley;

   int nw = (plWidth * scale);
   if (nw == 0)nw = 1;
   int nh = (plHeight * scale);
   if (nh == 0)nh = 1;

   draw1.SetWidth (nw);
   draw1.SetHeight (nh);


   draw1.SetImgFileName (share + lxT ("boards/") + pboard->GetPictureFileName (), scale, scale);
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

}

void
CPWindow1::menu1_File_Configure_EvMenuActive(CControl * control)
{
 Window3.ShowExclusive ();
};

void
CPWindow1::menu1_File_ReloadLast_EvMenuActive(CControl * control)
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
  };

 pboard->MEnd ();
 pboard->MSetSerial (SERIALDEVICE);

 switch (pboard->MInit (pboard->GetProcessorName (), FNAME.char_str (), NSTEP * NSTEPKF))
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
  SetTitle (lxT ("PICSimLab - ") + String (boards_list[lab]) + lxT (" - ") + pboard->GetProcessorName () + lxT (" - ") + basename (filedialog1.GetFileName ()));
 else
  SetTitle (lxT ("PICSimLab - ") + String (boards_list[lab]) + lxT (" - ") + pboard->GetProcessorName ());




 mcupwr = pa;
 timer1.SetRunState (1);

};

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

};


//Change board

void
CPWindow1::menu1_EvBoard(CControl * control)
{
 lab_ = lab;
 lab = (int) (atof (((CItemMenu*) control)->GetText ()));

 FNAME = lxT (" ");
 _EvOnDestroy (control);
 _EvOnCreate (control);
 _EvOnShow (control);
};

//change microcontroller

void
CPWindow1::menu1_EvMicrocontroller(CControl * control)
{
 proc_ = pboard->GetProcessorName ();
 pboard->SetProcessorName (((CItemMenu*) control)->GetText ());

 SetTitle (lxT ("PICSimLab - ") + String (boards_list[lab]) + lxT (" - ") + pboard->GetProcessorName ());

 FNAME = lxT (" ");
 _EvOnDestroy (control);
 _EvOnCreate (control);
 _EvOnShow (control);
};

void
CPWindow1::togglebutton1_EvOnToggleButton(CControl * control)
{
 int osc_on = pboard->GetUseOscilloscope ();
 int spare_on = pboard->GetUseSpareParts ();

 debug = togglebutton1.GetCheck ();

 _EvOnDestroy (control);
 _EvOnCreate (control);
 _EvOnShow (control);

 if (osc_on) menu1_Modules_Oscilloscope_EvMenuActive (this);
 if (spare_on) menu1_Modules_Spareparts_EvMenuActive (this);


};

void
CPWindow1::menu1_File_SaveWorkspace_EvMenuActive(CControl * control)
{
 filedialog2.SetType (lxFD_SAVE | lxFD_CHANGE_DIR);
 filedialog2.Run ();
}

void
CPWindow1::LoadWorkspace(String fnpzw)
{
 char home[1024];
 char fzip[1280];

 if ((!lxFileExists (fnpzw)) || (!fnpzw.Contains (".pzw")))return;

 //write options
 strncpy (fzip, (char*) lxGetTempDir (_T ("picsimlab")).char_str (), 1023);
 strncat (fzip, "/", 1023);

 strncpy (home, (char*) lxGetTempDir (_T ("picsimlab")).char_str (), 1023);
 strncat (home, "/picsimlab_workspace/", 1023);

 lxRemoveDir (home);

 lxUnzipDir (fnpzw, fzip);

 _EvOnDestroy (this);

 snprintf (fzip, 1279, "%s/picsimlab.ini", home);
 CStringList prefsw;
 prefsw.Clear ();
 int lc;
 char * value;
 char * name;
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
     saveprefs (name, value);
    }
  }
 prefs.SaveToFile (fzip);

 Configure (this, home);

 _EvOnShow (this);

 snprintf (fzip, 1279, "%s/Readme.html", home);
 if (lxFileExists (fzip))
  {
#ifdef EXT_BROWSER
   lxLaunchDefaultBrowser (lxT ("file://") + String (fzip));
   //lxLaunchDefaultBrowser(lxT("https://lcgamboa.github.io/picsimlab/"));
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
     lxLaunchDefaultBrowser (lxT ("file://") + String (fzip));
     //lxLaunchDefaultBrowser(lxT("https://lcgamboa.github.io/picsimlab/"));
#else      
     Window2.html1.SetLoadFile (fzip);
     Window2.Show ();
#endif     
    }
  }
}

void
CPWindow1::menu1_File_LoadWorkspace_EvMenuActive(CControl * control)
{
 filedialog2.SetType (lxFD_OPEN | lxFD_CHANGE_DIR);
 filedialog2.Run ();

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
   char home[1024];
   char fname[1280];

   if (lxFileExists (filedialog2.GetFileName ()))
    {

     if (!Dialog (String ("Overwriting file: ") + basename (filedialog2.GetFileName ()) + "?"))
      return;
    }

   //write options

   strncpy (home, (char*) lxGetUserDataDir (_T ("picsimlab")).char_str (), 1023);
   snprintf (fname, 1279, "%s/picsimlab.ini", home);
   prefs.SaveToFile (fname);

   strncpy (home, (char*) lxGetTempDir (_T ("picsimlab")).char_str (), 1023);
   strncat (home, "/picsimlab_workspace/", 1023);

   lxRemoveDir (home);

   lxCreateDir (home);

   snprintf (fname, 1279, "%s/picsimlab.ini", home);
   prefs.Clear ();
   saveprefs (lxT ("lab"), String ().Format ("%i", lab));
   saveprefs (lxT ("clock"), combo1.GetText ());
   saveprefs (lxT ("debug"), itoa (debug));
   saveprefs (lxT ("position"), itoa (GetX ()) + lxT (",") + itoa (GetY ()));
   saveprefs (lxT ("osc_on"), itoa (pboard->GetUseOscilloscope ()));
   saveprefs (lxT ("spare_on"), itoa (pboard->GetUseSpareParts ()));
   saveprefs (lxT ("lfile"), lxT (" "));

   pboard->WritePreferences ();

   if (pboard->GetUseOscilloscope ())
    Window4.WritePreferences ();

   if (pboard->GetUseSpareParts ())
    Window5.WritePreferences ();


   prefs.SaveToFile (fname);

   //write memory
   snprintf (fname, 1279, "%s/mdump_%02i_%s.hex", home, lab_, (const char*) proc_.c_str ());

   pboard->MDumpMemory (fname);

   if (pboard->GetUseSpareParts ())
    {

     snprintf (fname, 1279, "%s/parts_%02i.pcf", home, lab_);
     Window5.SaveConfig (fname);
    }

   lxZipDir (home, filedialog2.GetFileName ());

   lxRemoveDir (home);


   strncpy (home, (char*) lxGetUserDataDir (_T ("picsimlab")).char_str (), 1023);
   snprintf (fname, 1279, "%s/picsimlab.ini", home);
   prefs.Clear ();
   prefs.LoadFromFile (fname);
   
#ifdef __EMSCRIPTEN__
   EM_ASM_({
	   var filename=UTF8ToString($0);
           var buf = FS.readFile(filename);
           var blob = new Blob([buf],  {"type" : "application/octet-stream" });
           var text = URL.createObjectURL(blob);

	   var element = document.createElement('a');
           element.setAttribute('href', text);
           element.setAttribute('download', filename);

           element.style.display = 'none';
           document.body.appendChild(element);

           element.click();

           document.body.removeChild(element);
           URL.revokeObjectURL(text);
	  },filedialog2.GetFileName ().c_str ());
#endif 
   
  }

}

void
CPWindow1::menu1_Tools_SerialTerm_EvMenuActive(CControl * control)
{
#ifdef _WIN_  
 lxExecute (share + lxT ("/../tools/cutecom/cutecom.exe"));
#else

 lxExecute ("cutecom", lxEXEC_MAKE_GROUP_LEADER);
#endif  
};

void
CPWindow1::menu1_Tools_SerialRemoteTank_EvMenuActive(CControl * control)
{
#ifdef _WIN_  
 lxExecute (share + lxT ("/../srtank.exe"));
#else

 lxExecute ("srtank", lxEXEC_MAKE_GROUP_LEADER);
#endif  
};

void
CPWindow1::menu1_Tools_Esp8266ModemSimulator_EvMenuActive(CControl * control)
{
#ifdef _WIN_  
 lxExecute (share + lxT ("/../espmsim.exe"));
#else
 lxExecute ("espmsim", lxEXEC_MAKE_GROUP_LEADER);
#endif  
};

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



#ifdef __EMSCRIPTEN__
extern "C"
{

 void
 file_ready(const char *fname)
 {
  if (strstr (fname, ".pzw"))
   {
    printf("Loading .pzw...\n");
    Window1.filedialog2.SetType (lxFD_OPEN | lxFD_CHANGE_DIR);
    Window1.filedialog2.SetFileName (fname);
    Window1.filedialog2_EvOnClose (1);
   }
  else if (strstr (fname, ".hex"))
   {
    printf("Loading .hex...\n");
    Window1.filedialog1.SetType (lxFD_OPEN | lxFD_CHANGE_DIR);
    Window1.filedialog1.SetFileName (fname);
    Window1.filedialog1_EvOnClose (1);
   }
  else
   {
    printf("Unknow file %s !!\n",fname);
   }
 }
}
#endif
