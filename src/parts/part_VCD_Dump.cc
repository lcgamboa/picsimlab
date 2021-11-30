/* ########################################################################

   PICsimLab - PIC laboratory simulator

   ########################################################################

   Copyright (c) : 2010-2021  Luis Claudio Gambôa Lopes

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

#include"../picsimlab1.h"
#include"../picsimlab4.h"
#include"../picsimlab5.h"
#include"part_VCD_Dump.h"

#ifdef __EMSCRIPTEN__
#include<emscripten.h>
#endif

static const char markers[] = "!$%&[()]";

/* outputs */
enum
{
 O_P1, O_P2, O_P3, O_P4, O_P5, O_P6, O_P7, O_P8, O_L1, O_L2, O_L3, O_L4, O_L5, O_L6, O_L7, O_L8, O_NAME, O_REC
};

/*inputs*/
enum
{
 I_START, I_VIEW
};

cpart_VCD_Dump::cpart_VCD_Dump(unsigned x, unsigned y) :
font(9, lxFONTFAMILY_TELETYPE, lxFONTSTYLE_NORMAL, lxFONTWEIGHT_BOLD)
{
 X = x;
 Y = y;
 aways_update = 1;
 ReadMaps ();

 lxImage image (&Window5);
 image.LoadFile (lxGetLocalFile (Window1.GetSharePath () + lxT ("parts/") + GetPictureFileName ()), Orientation, Scale, Scale);

 Bitmap = new lxBitmap (&image, &Window5);
 image.Destroy ();
 canvas.Create (Window5.GetWWidget (), Bitmap);

 input_pins[0] = 0;
 input_pins[1] = 0;
 input_pins[2] = 0;
 input_pins[3] = 0;
 input_pins[4] = 0;
 input_pins[5] = 0;
 input_pins[6] = 0;
 input_pins[7] = 0;

 old_value_pins[0] = 2;
 old_value_pins[1] = 2;
 old_value_pins[2] = 2;
 old_value_pins[3] = 2;
 old_value_pins[4] = 2;
 old_value_pins[5] = 2;
 old_value_pins[6] = 2;
 old_value_pins[7] = 2;


 snprintf (f_vcd_name, 200, "%s/picsimlab-XXXXXX", (const char *) lxGetTempDir ("PICSimLab").c_str ());
 close (mkstemp (f_vcd_name));
 unlink (f_vcd_name);

 strncat (f_vcd_name, ".vcd", 200);

 f_vcd = fopen (f_vcd_name, "w");
 fclose (f_vcd);
 f_vcd = NULL;

 rec = 0;
 vcd_count = 0;

 /*
#ifdef _WIN_
 viewer = Window1.GetSharePath () + lxT ("/../tools/gtkwave/bin/gtkwave.exe");
#else
 viewer = dirname (lxGetExecutablePath ()) + lxString ("/gtkwave");
#endif
*/
 RegisterRemoteControl ();
}

void
cpart_VCD_Dump::RegisterRemoteControl(void)
{
 for (int i = 0; i < inputc; i++)
  {
   switch (input[i].id)
    {
    case I_START:
     //TODO add support to remote control
     //input[i].status = &rec;
     break;
    }
  }
}

cpart_VCD_Dump::~cpart_VCD_Dump(void)
{
 delete Bitmap;
 canvas.Destroy ();

 if (f_vcd)
  {
   fclose (f_vcd);
   f_vcd = NULL;
  }
 unlink (f_vcd_name);
}

void
cpart_VCD_Dump::Draw(void)
{

 int i;
 int to;
 int r, g, b;

 const picpin * ppins = Window5.GetPinsValues ();

 Update = 0;

 for (i = 0; i < outputc; i++)
  {
   if (output[i].update)//only if need update
    {
     output[i].update = 0;

     if (!Update)
      {
       canvas.Init (Scale, Scale, Orientation);
       canvas.SetFont (font);
      }
     Update++; //set to update buffer

     switch (output[i].id)
      {
      case O_P1:
      case O_P2:
      case O_P3:
      case O_P4:
      case O_P5:
      case O_P6:
      case O_P7:
      case O_P8:
       canvas.SetColor (49, 61, 99);
       canvas.Rectangle (1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
       canvas.SetFgColor (255, 255, 255);
       if (input_pins[output[i].id - O_P1] == 0)
        canvas.RotatedText ("NC", output[i].x1, output[i].y1, 0);
       else
        canvas.RotatedText (Window5.GetPinName (input_pins[output[i].id - O_P1]), output[i].x1, output[i].y1, 0);
       break;
      case O_NAME:
       canvas.SetColor (49, 61, 99);
       canvas.Rectangle (1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
       canvas.SetFgColor (255, 255, 255);
       to = strlen (f_vcd_name);
       if (to < 48)
        {
         to = 0;
        }
       else
        {
         to = to - 48;
        }
       canvas.RotatedText (f_vcd_name + to, output[i].x1, output[i].y1, 0);
       break;
      case O_L1:
      case O_L2:
      case O_L3:
      case O_L4:
      case O_L5:
      case O_L6:
      case O_L7:
      case O_L8:
       if (input_pins[output[i].id - O_L1] > 0)
        {
         canvas.SetColor (ppins[input_pins[output[i].id - O_L1] - 1].oavalue, 0, 0);
        }
       else
        {
         canvas.SetColor (30, 0, 0);
        }
       canvas.SetFgColor (0, 0, 0);
       //draw a circle
       color1 = canvas.GetBgColor ();
       r = color1.Red () - 120;
       g = color1.Green () - 120;
       b = color1.Blue () - 120;
       if (r < 0)r = 0;
       if (g < 0)g = 0;
       if (b < 0)b = 0;
       color2.Set (r, g, b);
       canvas.SetBgColor (color2);
       canvas.Circle (1, output[i].x1, output[i].y1, output[i].r);
       canvas.SetBgColor (color1);
       canvas.Circle (1, output[i].x1, output[i].y1, output[i].r - 3);
       break;
      case O_REC:
       if (rec > 0)
        {
         canvas.SetColor (0, 255, 0);
        }
       else
        {
         canvas.SetColor (255, 0, 0);
        }
       canvas.SetFgColor (0, 0, 0);
       //draw a circle
       color1 = canvas.GetBgColor ();
       r = color1.Red () - 120;
       g = color1.Green () - 120;
       b = color1.Blue () - 120;
       if (r < 0)r = 0;
       if (g < 0)g = 0;
       if (b < 0)b = 0;
       color2.Set (r, g, b);
       canvas.SetBgColor (color2);
       canvas.Circle (1, output[i].x1, output[i].y1, output[i].r);
       canvas.SetBgColor (color1);
       canvas.Circle (1, output[i].x1, output[i].y1, output[i].r - 3);
       break;
      }
    }
  }

 if (Update)
  {
   canvas.End ();
  }
}

unsigned short
cpart_VCD_Dump::get_in_id(char * name)
{
 if (strcmp (name, "PB_START") == 0)return I_START;
 if (strcmp (name, "PB_VIEW") == 0)return I_VIEW;

 printf ("Erro input '%s' don't have a valid id! \n", name);
 return -1;
};

unsigned short
cpart_VCD_Dump::get_out_id(char * name)
{

 if (strcmp (name, "PN_1") == 0)return O_P1;
 if (strcmp (name, "PN_2") == 0)return O_P2;
 if (strcmp (name, "PN_3") == 0)return O_P3;
 if (strcmp (name, "PN_4") == 0)return O_P4;
 if (strcmp (name, "PN_5") == 0)return O_P5;
 if (strcmp (name, "PN_6") == 0)return O_P6;
 if (strcmp (name, "PN_7") == 0)return O_P7;
 if (strcmp (name, "PN_8") == 0)return O_P8;

 if (strcmp (name, "LD_1") == 0)return O_L1;
 if (strcmp (name, "LD_2") == 0)return O_L2;
 if (strcmp (name, "LD_3") == 0)return O_L3;
 if (strcmp (name, "LD_4") == 0)return O_L4;
 if (strcmp (name, "LD_5") == 0)return O_L5;
 if (strcmp (name, "LD_6") == 0)return O_L6;
 if (strcmp (name, "LD_7") == 0)return O_L7;
 if (strcmp (name, "LD_8") == 0)return O_L8;

 if (strcmp (name, "DI_NAME") == 0)return O_NAME;
 if (strcmp (name, "LD_REC") == 0)return O_REC;

 printf ("Erro output '%s' don't have a valid id! \n", name);
 return 1;
}

lxString
cpart_VCD_Dump::WritePreferences(void)
{
 char prefs[256];

 sprintf (prefs, "%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu", input_pins[0], input_pins[1], input_pins[2], input_pins[3], input_pins[4], input_pins[5], input_pins[6], input_pins[7] /*(const char *)viewer.c_str ()*/);

 return prefs;
}

void
cpart_VCD_Dump::ReadPreferences(lxString value)
{
 sscanf (value.c_str (), "%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu", &input_pins[0], &input_pins[1], &input_pins[2], &input_pins[3], &input_pins[4], &input_pins[5], &input_pins[6], &input_pins[7]);

 /*
 char buff[2048];
 
 sscanf (value.c_str (), "%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%s", &input_pins[0], &input_pins[1], &input_pins[2], &input_pins[3], &input_pins[4], &input_pins[5], &input_pins[6], &input_pins[7], buff);

 viewer = buff;

 if (!lxFileExists (viewer)) //use default
  {
#ifdef _WIN_
   viewer = Window1.GetSharePath () + lxT ("/../tools/gtkwave/bin/gtkwave.exe");
#else
   viewer = dirname (lxGetExecutablePath ()) + lxString ("/gtkwave");
#endif
  }
  */
}

void
cpart_VCD_Dump::ConfigurePropertiesWindow(CPWindow * WProp)
{
 lxString Items = Window5.GetPinsNames ();
 lxString spin;

 ((CCombo*) WProp->GetChildByName ("combo1"))->SetItems (Items);
 if (input_pins[0] == 0)
  ((CCombo*) WProp->GetChildByName ("combo1"))->SetText ("0  NC");
 else
  {
   spin = Window5.GetPinName (input_pins[0]);
   ((CCombo*) WProp->GetChildByName ("combo1"))->SetText (itoa (input_pins[0]) + "  " + spin);
  }

 ((CCombo*) WProp->GetChildByName ("combo2"))->SetItems (Items);
 if (input_pins[1] == 0)
  ((CCombo*) WProp->GetChildByName ("combo2"))->SetText ("0  NC");
 else
  {
   spin = Window5.GetPinName (input_pins[1]);
   ((CCombo*) WProp->GetChildByName ("combo2"))->SetText (itoa (input_pins[1]) + "  " + spin);
  }

 ((CCombo*) WProp->GetChildByName ("combo3"))->SetItems (Items);
 if (input_pins[2] == 0)
  ((CCombo*) WProp->GetChildByName ("combo3"))->SetText ("0  NC");
 else
  {
   spin = Window5.GetPinName (input_pins[2]);
   ((CCombo*) WProp->GetChildByName ("combo3"))->SetText (itoa (input_pins[2]) + "  " + spin);
  }

 ((CCombo*) WProp->GetChildByName ("combo4"))->SetItems (Items);
 if (input_pins[3] == 0)
  ((CCombo*) WProp->GetChildByName ("combo4"))->SetText ("0  NC");
 else
  {
   spin = Window5.GetPinName (input_pins[3]);
   ((CCombo*) WProp->GetChildByName ("combo4"))->SetText (itoa (input_pins[3]) + "  " + spin);
  }

 ((CCombo*) WProp->GetChildByName ("combo5"))->SetItems (Items);
 if (input_pins[4] == 0)
  ((CCombo*) WProp->GetChildByName ("combo5"))->SetText ("0  NC");
 else
  {
   spin = Window5.GetPinName (input_pins[4]);
   ((CCombo*) WProp->GetChildByName ("combo5"))->SetText (itoa (input_pins[4]) + "  " + spin);
  }

 ((CCombo*) WProp->GetChildByName ("combo6"))->SetItems (Items);
 if (input_pins[5] == 0)
  ((CCombo*) WProp->GetChildByName ("combo6"))->SetText ("0  NC");
 else
  {
   spin = Window5.GetPinName (input_pins[5]);
   ((CCombo*) WProp->GetChildByName ("combo6"))->SetText (itoa (input_pins[5]) + "  " + spin);
  }

 ((CCombo*) WProp->GetChildByName ("combo7"))->SetItems (Items);
 if (input_pins[6] == 0)
  ((CCombo*) WProp->GetChildByName ("combo7"))->SetText ("0  NC");
 else
  {
   spin = Window5.GetPinName (input_pins[6]);
   ((CCombo*) WProp->GetChildByName ("combo7"))->SetText (itoa (input_pins[6]) + "  " + spin);
  }

 ((CCombo*) WProp->GetChildByName ("combo8"))->SetItems (Items);
 if (input_pins[7] == 0)
  ((CCombo*) WProp->GetChildByName ("combo8"))->SetText ("0  NC");
 else
  {
   spin = Window5.GetPinName (input_pins[7]);
   ((CCombo*) WProp->GetChildByName ("combo8"))->SetText (itoa (input_pins[7]) + "  " + spin);
  }

 //((CEdit*) WProp->GetChildByName ("edit1"))->SetText (viewer);

 ((CButton*) WProp->GetChildByName ("button1"))->EvMouseButtonRelease = EVMOUSEBUTTONRELEASE & CPWindow5::PropButtonRelease;
 ((CButton*) WProp->GetChildByName ("button1"))->SetTag (1);

 ((CButton*) WProp->GetChildByName ("button2"))->EvMouseButtonRelease = EVMOUSEBUTTONRELEASE & CPWindow5::PropButtonRelease;

 //((CButton*) WProp->GetChildByName ("button3"))->EvMouseButtonRelease = EVMOUSEBUTTONRELEASE & CPWindow5::PropButtonRelease;
 //((CButton*) WProp->GetChildByName ("button3"))->SetTag (2 + id);

 //Window5.filedialog1.SetFileName (viewer);

 VDWProp = WProp;

}

void
cpart_VCD_Dump::ReadPropertiesWindow(CPWindow * WProp)
{
 input_pins[0] = atoi (((CCombo*) WProp->GetChildByName ("combo1"))->GetText ());
 input_pins[1] = atoi (((CCombo*) WProp->GetChildByName ("combo2"))->GetText ());
 input_pins[2] = atoi (((CCombo*) WProp->GetChildByName ("combo3"))->GetText ());
 input_pins[3] = atoi (((CCombo*) WProp->GetChildByName ("combo4"))->GetText ());
 input_pins[4] = atoi (((CCombo*) WProp->GetChildByName ("combo5"))->GetText ());
 input_pins[5] = atoi (((CCombo*) WProp->GetChildByName ("combo6"))->GetText ());
 input_pins[6] = atoi (((CCombo*) WProp->GetChildByName ("combo7"))->GetText ());
 input_pins[7] = atoi (((CCombo*) WProp->GetChildByName ("combo8"))->GetText ());

 /*
  viewer = ((CEdit*) WProp->GetChildByName ("edit1"))->GetText ();

  if (!lxFileExists (viewer)) //use default
   {
 #ifdef _WIN_
    viewer = Window1.GetSharePath () + lxT ("/../tools/gtkwave/bin/gtkwave.exe");
 #else
    viewer = dirname (lxGetExecutablePath ()) + lxString ("/gtkwave");
 #endif
   }
  */
}

/*
void
cpart_VCD_Dump::filedialog_EvOnClose(int retId)
{
 if (retId)
  {
   if ((Window5.filedialog1.GetType () == (lxFD_OPEN | lxFD_CHANGE_DIR)))
    {
     if (lxFileExists (Window5.filedialog1.GetFileName ()))
      {
       viewer = Window5.filedialog1.GetFileName ();
       ((CEdit*) VDWProp->GetChildByName ("edit1"))->SetText (viewer);
      }
     else//use default
      {
#ifdef _WIN_
       viewer = Window1.GetSharePath () + lxT ("/../tools/gtkwave/bin/gtkwave.exe");
#else

       viewer = dirname (lxGetExecutablePath ()) + lxString ("/gtkwave");
#endif
       ((CEdit*) VDWProp->GetChildByName ("edit1"))->SetText (viewer);
      }
    }
  }
}
 */

void
cpart_VCD_Dump::Process(void)
{

 if (rec)
  {
   const picpin * ppins = Window5.GetPinsValues ();

   vcd_count++;
   int tprint = 0;

   for (int i = 0; i < 8; i++)
    {
     if (input_pins[i] != 0)
      {
       if (ppins[input_pins[i] - 1].value != old_value_pins[i])
        {
         if (!tprint)
          {
           tprint = 1;
           fprintf (f_vcd, "#%li\n", vcd_count);
          }
         old_value_pins[i] = ppins[input_pins[i] - 1].value;
         fprintf (f_vcd, "%i%c\n", old_value_pins[i], markers[i]);
         fflush (f_vcd);
        }
      }
    }
  }
}

void
cpart_VCD_Dump::PostProcess(void)
{

 const picpin * ppins = Window5.GetPinsValues ();

 for (int i = 0; i < 8; i++)
  {
   if (input_pins[i] && (output_ids[O_L1 + i]->value != ppins[input_pins[i] - 1].oavalue))
    {

     output_ids[O_L1 + i]->value = ppins[input_pins[i] - 1].oavalue;
     output_ids[O_L1 + i]->update = 1;
    }
  }
}

void
cpart_VCD_Dump::EvMouseButtonPress(uint button, uint x, uint y, uint state)
{
 int i;

 for (i = 0; i < inputc; i++)
  {
   if (PointInside (x, y, input[i]))
    {

     switch (input[i].id)
      {
      case I_START:
       if (!rec)
        {
         float tscale = 1.0e12 / Window1.GetBoard ()->MGetInstClockFreq (); //ps step

         f_vcd = fopen (f_vcd_name, "w");
         vcd_count = 0;

         fprintf (f_vcd,
                  "$version Generated by PICSimLab $end\n"
                  "$timescale %ips $end\n"
                  "$scope module logic $end\n"
                  "$var wire 1 !  1-%s $end\n"
                  "$var wire 1 $  2-%s $end\n"
                  "$var wire 1 %%  3-%s $end\n"
                  "$var wire 1 &  4-%s $end\n"
                  "$var wire 1 [  5-%s $end\n"
                  "$var wire 1 (  6-%s $end\n"
                  "$var wire 1 )  7-%s $end\n"
                  "$var wire 1 ]  8-%s $end\n"
                  "$upscope $end\n"
                  "$enddefinitions $end\n"
                  "$dumpvars\n"
                  "x!\n"
                  "x$\n"
                  "x%%\n"
                  "x&\n"
                  "x[\n"
                  "x(\n"
                  "x)\n"
                  "x[\n"
                  "$end\n", (int) tscale,
                  (const char *) Window5.GetPinName (input_pins[0]).c_str (),
                  (const char *) Window5.GetPinName (input_pins[1]).c_str (),
                  (const char *) Window5.GetPinName (input_pins[2]).c_str (),
                  (const char *) Window5.GetPinName (input_pins[3]).c_str (),
                  (const char *) Window5.GetPinName (input_pins[4]).c_str (),
                  (const char *) Window5.GetPinName (input_pins[5]).c_str (),
                  (const char *) Window5.GetPinName (input_pins[6]).c_str (),
                  (const char *) Window5.GetPinName (input_pins[7]).c_str ()
                  );
         rec = 1;
        }
       else
        {
         rec = 0;
         if (f_vcd)
          {
           fclose (f_vcd);
           f_vcd = NULL;
          }
        }
       output_ids[O_REC]->update = 1;
       break;
      case I_VIEW:
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
       }, f_vcd_name);
#else
#ifdef _WIN_
       //lxExecute (viewer + lxT (" ") + f_vcd_name);
       lxExecute (Window1.GetSharePath () + lxT ("/../tools/gtkwave/bin/gtkwave.exe ") + f_vcd_name);
#else
       //lxExecute (viewer + lxT (" ") + f_vcd_name, lxEXEC_MAKE_GROUP_LEADER);
       lxLaunchDefaultApplication (f_vcd_name);
#endif
#endif
       break;
      }
    }
  }
}

part_init(PART_VCD_DUMP_Name, cpart_VCD_Dump, "Virtual");

