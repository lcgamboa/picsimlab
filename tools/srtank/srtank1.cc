#include"srtank1.h"
#include"srtank1_d.cc"

#include"math.h"

CPWindow1 Window1;


//Implementation

#include"serial.h"
#include"srtank2.h"

int conectado = 0;
char idig[8];
char odig[8];
short iana[4];
short oana[4];

#define vent idig[0]
#define vsai idig[1]
#define aque idig[2]
#define resf idig[3]
#define agit idig[4]

#define balt odig[0]
#define bbai odig[1]
#define tmin odig[2]
#define tmax odig[3]

#define amin iana[0]
#define amax iana[1]

#define volu oana[0] 
#define temp oana[1]

double vzent;
double vzsai;
double paque;
double presf;
double capac;
double temp0;
double c_esp;
double densi;

double temperatura = 22;
double volume = 0;

void
CPWindow1::_EvOnCreate (CControl * control)
{

 char * resp = slist ();

 if (resp)
  {

   combo1.SetItems (resp);
   combo1.SetText (combo1.GetItem (0));
   free (resp);
  }
 else
  {
   printf ("No serial ports found!\n");
  }
  
 amax = 40;
 amin = 10;

 vent = 0;
 vsai = 0;
 aque = 0;
 resf = 0;
 agit = 0;

 temperatura = 22;
 volume = 0;

 vzent = 100;
 vzsai = 100;
 paque = 800 * 1000;
 presf = 800 * 1000;
 capac = 3000;
 temp0 = 22;
 c_esp = 1;
 densi = 1000;

#ifdef _WIN_ 
 wxFileName fexe (wxStandardPaths::Get ().GetExecutablePath ());
 wxSetWorkingDirectory (fexe.GetPath ());
#endif
};

void
CPWindow1::button1_EvMouseButtonClick (CControl * control, uint button, uint x, uint y, uint state)
{

 if (conectado)
  {
   conectado = 0;
   sclose ();
   combo1.SetEnable (1);
   button1.SetText (lxT ("Connect"));

   edit1.SetEnable (1);
   edit2.SetEnable (1);
   edit3.SetEnable (1);
   edit4.SetEnable (1);
   edit5.SetEnable (1);
   edit6.SetEnable (1);
   edit7.SetEnable (1);
   edit8.SetEnable (1);

   amax = 40;
   amin = 10;

   vent = 0;
   vsai = 0;
   aque = 0;
   resf = 0;
   agit = 0;

   temperatura = 22;
   volume = 0;
  }
 else
  {
   strcpy (SERIALDEVICE, (char *) combo1.GetText ().char_str ());

   if (!sopen (SERIALDEVICE))
    {
     scfg ();
     conectado = 1;
     combo1.SetEnable (0);
     button1.SetText (lxT ("Disconnect"));

     edit1.SetEnable (0);
     edit2.SetEnable (0);
     edit3.SetEnable (0);
     edit4.SetEnable (0);
     edit5.SetEnable (0);
     edit6.SetEnable (0);
     edit7.SetEnable (0);
     edit8.SetEnable (0);

     vzent = atof (edit1.GetText ());
     vzsai = atof (edit2.GetText ());
     paque = atof (edit3.GetText ())*1000.0;
     presf = atof (edit4.GetText ())*1000.0;
     capac = atof (edit5.GetText ());
     temp0 = atof (edit6.GetText ());
     c_esp = atof (edit7.GetText ());
     densi = atof (edit8.GetText ());

     text1.Clear ();
    }
   else
    {
     Message (lxT (" Error open serial port: ") + combo1.GetText ());
     text1.AddLine (lxT (" Error open serial port: ") + combo1.GetText ());
    };
  }



};


int tw = 240;
int th = 360;

int aw = 80;
int daw = 10;

int tx0 = 40;
int ty0 = 40;
int tx1 = tw;
int ty1 = th;

void
CPWindow1::draw1_EvMouseButtonPress (CControl * control, uint button, uint x, uint y, uint state)
{

 draw1.Canvas.Init ();


 //tanque
 draw1.Canvas.SetBgColor (255, 255, 255);
 draw1.Canvas.Rectangle (true, 0, 0, draw1.GetWidth (), draw1.GetHeight ());

 draw1.Canvas.SetFgColor (50, 50, 200);
 draw1.Canvas.SetBgColor (50, 50, 200);
 draw1.Canvas.Rectangle (true, tx0, ty0 + th - roundf (th * (volume / capac)), tx1, roundf (ty1 * volume / capac));
 draw1.Canvas.SetFgColor (0, 0, 0);
 draw1.Canvas.Rectangle (false, tx0, ty0, tx1, ty1);

 //valv. entrada
 draw1.Canvas.SetFgColor (0, 0, 0);
 if (vent)
  {
   draw1.Canvas.SetBgColor (0, 200, 0);
   draw1.Canvas.Rectangle (true, (draw1.GetWidth () / 2) - 20, 0, 40, 20);

   draw1.Canvas.SetFgColor (50, 50, 200);
   draw1.Canvas.SetBgColor (50, 50, 200);
   draw1.Canvas.Rectangle (true, (draw1.GetWidth () / 2) - 15, 20, 30, th + 20);

   if (volume >= capac)
    {
     draw1.Canvas.Rectangle (true, tx0 - 2, ty0 - 2, tx1 + 4, 2);
     draw1.Canvas.Rectangle (true, tx0 - 4, ty0 - 2, 2, ty1 + 40);
     draw1.Canvas.Rectangle (true, tx0 + tx1 + 2, ty0 - 2, 2, ty1 + 40);
    }
  }
 else
  {
   draw1.Canvas.SetBgColor (200, 0, 0);
   draw1.Canvas.Rectangle (true, (draw1.GetWidth () / 2) - 20, 0, 40, 20);
  }

 //valv. saida
 draw1.Canvas.SetFgColor (0, 0, 0);
 if (vsai)
  {
   draw1.Canvas.SetBgColor (0, 200, 0);
   draw1.Canvas.Rectangle (true, (draw1.GetWidth () / 2) - 20, ty0 + ty1, 40, 20);

   if (volume > 0)
    {
     draw1.Canvas.SetFgColor (50, 50, 200);
     draw1.Canvas.SetBgColor (50, 50, 200);
     draw1.Canvas.Rectangle (true, (draw1.GetWidth () / 2) - 15, ty0 + ty1 + 20, 30, 20);
    }
  }
 else
  {
   draw1.Canvas.SetBgColor (200, 0, 0);
   draw1.Canvas.Rectangle (true, (draw1.GetWidth () / 2) - 20, ty0 + ty1, 40, 20);
  }

 //agit
 draw1.Canvas.SetFgColor (0, 0, 0);
 if (agit)
  {
   aw += daw;
   if (aw < 11)daw = 10;
   if (aw > 80)daw = -10;

   draw1.Canvas.SetBgColor (0, 200, 0);
   draw1.Canvas.Rectangle (true, (draw1.GetWidth () / 2) - aw / 2, ty0 + ty1 - 25, aw, 20);
  }
 else
  {
   draw1.Canvas.SetBgColor (200, 0, 0);
   draw1.Canvas.Rectangle (true, (draw1.GetWidth () / 2) - aw / 2, ty0 + ty1 - 25, aw, 20);
  }


 //aquec
 draw1.Canvas.SetFgColor (0, 0, 0);
 if (aque)
  {
   draw1.Canvas.SetBgColor (0, 200, 0);
   draw1.Canvas.Rectangle (true, tx0 + 5, ty0 + ty1 - 50, 10, 40);
  }
 else
  {
   draw1.Canvas.SetBgColor (200, 0, 0);
   draw1.Canvas.Rectangle (true, tx0 + 5, ty0 + ty1 - 50, 10, 40);
  }

 //resfr
 draw1.Canvas.SetFgColor (0, 0, 0);
 if (resf)
  {
   draw1.Canvas.SetBgColor (0, 200, 0);
   draw1.Canvas.Rectangle (true, tx0 + 5, ty0 + ty1 - 95, 10, 40);
  }
 else
  {
   draw1.Canvas.SetBgColor (200, 0, 0);
   draw1.Canvas.Rectangle (true, tx0 + 5, ty0 + ty1 - 95, 10, 40);
  }

 //balt
 draw1.Canvas.SetFgColor (0, 0, 0);
 if (balt)
  {
   draw1.Canvas.SetBgColor (0, 200, 0);
   draw1.Canvas.Rectangle (true, tx0 + tx1 - 12, ty0 + 2, 10, 10);
  }
 else
  {
   draw1.Canvas.SetBgColor (200, 0, 0);
   draw1.Canvas.Rectangle (true, tx0 + tx1 - 12, ty0 + 2, 10, 10);
  }

 //bbai
 draw1.Canvas.SetFgColor (0, 0, 0);
 if (bbai)
  {
   draw1.Canvas.SetBgColor (0, 200, 0);
   draw1.Canvas.Rectangle (true, tx0 + tx1 - 12, ty0 + ty1 - 12, 10, 10);
  }
 else
  {
   draw1.Canvas.SetBgColor (200, 0, 0);
   draw1.Canvas.Rectangle (true, tx0 + tx1 - 12, ty0 + ty1 - 12, 10, 10);
  }




 draw1.Canvas.End ();
 draw1.Update ();

};







unsigned char buff[3];
int bc = 0;


lxString line = "";

void
CPWindow1::timer1_EvOnTime (CControl * control)
{
 unsigned char dado;
 lxString str;


 draw1_EvMouseButtonPress (control, 0, 0, 0, 0);


 checkbox1.SetCheck (idig[0]);
 checkbox2.SetCheck (idig[1]);
 checkbox3.SetCheck (idig[2]);
 checkbox4.SetCheck (idig[3]);
 checkbox5.SetCheck (idig[4]);


 checkbox6.SetCheck (odig[0]);
 checkbox7.SetCheck (odig[1]);
 checkbox8.SetCheck (odig[2]);
 checkbox9.SetCheck (odig[3]);


 edit9.SetText (itoa (iana[0]));
 edit10.SetText (itoa (iana[1]));

 edit11.SetText (itoa (oana[0]));
 edit12.SetText (itoa (oana[1]));

 if (conectado)
  {
   if (srec (&dado))
    {
     buff[bc] = dado;
     bc++;

     str.Printf (lxT (">%02x "), dado);
     line += str;

    }

   if (bc > 0)
    switch (buff[0]& 0xF0)
     {
     case 0x00:
      if (bc >= 2)
       {
        idig[buff[0]&0x07] = buff[1] != 0;
        bc = 0;
        str.Printf (lxT ("  Digital Write %d = %d \n"), buff[0]&0x07, idig[buff[0]&0x07]);
        line += str;
        text1.AddLine (line);
        line = "";
       }
      break;
     case 0x10:
      if (bc >= 1)
       {
        ssend (odig[buff[0]&0x07]);
        str.Printf (lxT ("<%02x "), odig[buff[0]&0x07]);
        line += str;
        str.Printf (lxT ("  Digital Read %d = %d \n"), buff[0]&0x07, odig[buff[0]&0x07]);
        line += str;
        bc = 0;
        text1.AddLine (line);
        line = "";
       }
      break;
     case 0x20:
      if (bc >= 3)
       {
        iana[buff[0]&0x03] = buff[1] << 8 | buff[2];
        bc = 0;
        str.Printf (lxT ("  Analog Write %d = %d \n"), buff[0]&0x03, iana[buff[0]&0x03]);
        line += str;
        text1.AddLine (line);
        line = "";
       }
      break;
     case 0x30:
      if (bc >= 1)
       {
        ssend (((oana[buff[0]&0x03]&0xFF00) >> 8));
        str.Printf (lxT ("<%02x "), ((oana[buff[0]&0x03]&0xFF00) >> 8));
        line += str;
        ssend ((oana[buff[0]&0x03]&0x00FF));
        str.Printf (lxT ("<%02x "), (oana[buff[0]&0x03]&0x00FF));
        line += str;
        str.Printf (lxT ("  Analog Read %d = %d \n"), buff[0]&0x03, oana[buff[0]&0x03]);
        line += str;
        bc = 0;
        text1.AddLine (line);
        line = "";
       }
      break;
     default:
      text1.AddLine (lxT ("!!!!!!!!!!!!!!!!!!!! ") + line + lxT ("  Unknown command !!!!!!!!!!!!!!!\n"));
      line = "";
      //printf("  Unknown command!!!!!!!!\n");
      bc = 0;
      break;
     }

  }

 //planta

 volume += vent * vzent * 0.1 - vsai * vzsai * 0.1;

 if (volume < 0) volume = 0;
 if (volume > capac) volume = capac;

 if (volume > 0)
  temperatura += (1.0 / (volume * densi * c_esp))*(paque * aque * 0.1 - presf * resf * 0.1 - 1000.0 * (temperatura - temp0));
 else
  temperatura = temp0;


 balt = volume >= capac;
 bbai = volume > 0;
 tmin = temperatura < amin;
 tmax = temperatura > amax;

 temp = temperatura;
 volu = volume;

};

void
CPWindow1::_EvOnDestroy (CControl * control)
{
 timer1.SetRunState (0);

#ifdef _WIN_
 Sleep (2);
#else
 usleep (200000);
#endif
};

void
CPWindow1::menu1_File_DefaultConfig_EvMenuActive (CControl * control)
{

 edit1.SetText (lxT ("100"));
 edit2.SetText (lxT ("100"));
 edit3.SetText (lxT ("800"));
 edit4.SetText (lxT ("500"));
 edit5.SetText (lxT ("3000"));
 edit6.SetText (lxT ("22"));
 edit7.SetText (lxT ("1.0"));
 edit8.SetText (lxT ("1000"));

};

void
CPWindow1::menu1_File_SaveConfig_EvMenuActive (CControl * control)
{

 filedialog1.SetType (lxFD_SAVE | lxFD_CHANGE_DIR);

 filedialog1.Run ();

}

void
CPWindow1::menu1_File_LoadConfig_EvMenuActive (CControl * control)
{


 filedialog1.SetType (lxFD_OPEN | lxFD_CHANGE_DIR);
 filedialog1.Run ();

}

void
CPWindow1::menu1_File_Exit_EvMenuActive (CControl * control)
{
 WDestroy ();
}

void
CPWindow1::menu1_Help_Contents_EvMenuActive (CControl * control)
{
#ifdef EXT_BROWSER
 lxLaunchDefaultBrowser (lxT ("https://lcgamboa.github.io/picsimlab_docs/srtank/"));
#else	
 Window2.html1.SetLoadFile (lxString (lxT (_SHARE_)) + lxT ("docs/srtank/srtank.html"));
 Window2.Show ();
#endif 
}

void
CPWindow1::menu1_Help_About_EvMenuActive (CControl * control)
{
 Message (lxT ("Developed by L.C. Gamboa\n <lcgamboa@yahoo.com>\n Version: ") + lxString (lxT (_VERSION_)));
}

void
CPWindow1::filedialog1_EvOnClose (int retId)
{

 if (retId && (filedialog1.GetType () == (lxFD_SAVE | lxFD_CHANGE_DIR)))
  {
   lxStringList prefs;

   prefs.AddLine (edit1.GetText ());
   prefs.AddLine (edit2.GetText ());
   prefs.AddLine (edit3.GetText ());
   prefs.AddLine (edit4.GetText ());
   prefs.AddLine (edit5.GetText ());
   prefs.AddLine (edit6.GetText ());
   prefs.AddLine (edit7.GetText ());
   prefs.AddLine (edit8.GetText ());

   prefs.SaveToFile (filedialog1.GetFileName ());
  }

 if (retId && (filedialog1.GetType () == (lxFD_OPEN | lxFD_CHANGE_DIR)))
  {
   lxStringList prefs;
   if (prefs.LoadFromFile (filedialog1.GetFileName ()))
    {
     edit1.SetText (prefs.GetLine (0));
     edit2.SetText (prefs.GetLine (1));
     edit3.SetText (prefs.GetLine (2));
     edit4.SetText (prefs.GetLine (3));
     edit5.SetText (prefs.GetLine (4));
     edit6.SetText (prefs.GetLine (5));
     edit7.SetText (prefs.GetLine (6));
     edit8.SetText (prefs.GetLine (7));
    }
  }
}


