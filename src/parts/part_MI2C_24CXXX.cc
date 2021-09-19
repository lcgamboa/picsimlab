/* ########################################################################

   PICsimLab - PIC laboratory simulator

   ########################################################################

   Copyright (c) : 2019-2021  Luis Claudio Gambôa Lopes

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
#include"part_MI2C_24CXXX.h"

#ifdef __EMSCRIPTEN__
#include<emscripten.h>
#endif

/* inputs */
enum
{
 I_LOAD, I_SAVE, I_VIEW
};

/* outputs */
enum
{
 O_P1, O_P2, O_P3, O_P4, O_P5, O_P6, O_P7, O_P8, O_IC
};

const char pin_names[8][10] = {"A0", "A1", "A2", "VSS", "SDA", "SCL", "WP", "VCC"};
const char pin_values[8][10] = {
 {0},
 {1},
 {2},
 "GND",
 {3},
 {4},
 "GND",
 "+5V"
};

cpart_MI2C_24CXXX::cpart_MI2C_24CXXX(unsigned x, unsigned y) :
font(8, lxFONTFAMILY_TELETYPE, lxFONTSTYLE_NORMAL, lxFONTWEIGHT_BOLD)
{
 X = x;
 Y = y;
 ReadMaps ();
 Bitmap = NULL;

 lxImage image (&Window5);

 image.LoadFile (lxGetLocalFile (Window1.GetSharePath () + lxT ("parts/") + GetPictureFileName ()), Orientation, Scale, Scale);

 Bitmap = new lxBitmap (&image, &Window5);
 image.Destroy ();
 canvas.Create (Window5.GetWWidget (), Bitmap);

 kbits = 4;

 mi2c_init (&mi2c, kbits);
 mi2c_rst (&mi2c);

 input_pins[0] = 0;
 input_pins[1] = 0;
 input_pins[2] = 0;
 input_pins[3] = 0;
 input_pins[4] = 0;

 f_mi2c_name[0] = '*';
 f_mi2c_name[1] = 0;
 f_mi2c = NULL;

 snprintf (f_mi2c_tmp_name, 200, "%s/picsimlab-XXXXXX", (const char *) lxGetTempDir ("PICSimLab").c_str ());
 close (mkstemp (f_mi2c_tmp_name));
 unlink (f_mi2c_tmp_name);

 strncat (f_mi2c_tmp_name, ".txt", 200);

}

cpart_MI2C_24CXXX::~cpart_MI2C_24CXXX(void)
{
 mi2c_end (&mi2c);
 delete Bitmap;
 canvas.Destroy ();
 unlink (f_mi2c_tmp_name);
}

void
cpart_MI2C_24CXXX::Draw(void)
{

 int i;

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
      case O_IC:
       char buff[10];
       snprintf (buff, 9, "24C%02i", kbits);
       canvas.SetColor (26, 26, 26);
       canvas.Rectangle (1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
       canvas.SetFgColor (255, 255, 255);
       canvas.RotatedText (buff, output[i].x1, output[i].y2 - 15, 0);
       break;
      default:
       canvas.SetColor (49, 61, 99);
       canvas.Rectangle (1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);

       canvas.SetFgColor (255, 255, 255);
       canvas.RotatedText (pin_names[output[i].id - O_P1], output[i].x1, output[i].y2, 90.0);

       int pinv = pin_values[output[i].id - O_P1][0];
       if (pinv > 10)
        {
         canvas.SetFgColor (155, 155, 155);
         canvas.RotatedText (pin_values[output[i].id - O_P1], output[i].x1, output[i].y2 - 30, 90.0);
        }
       else
        {
         if (input_pins[pinv] == 0)
          canvas.RotatedText ("NC", output[i].x1, output[i].y2 - 30, 90.0);
         else
          canvas.RotatedText (Window5.GetPinName (input_pins[pinv]), output[i].x1, output[i].y2 - 30, 90.0);
        }
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
cpart_MI2C_24CXXX::get_in_id(char * name)
{
 if (strcmp (name, "PB_LOAD") == 0)return I_LOAD;
 if (strcmp (name, "PB_SAVE") == 0)return I_SAVE;
 if (strcmp (name, "PB_VIEW") == 0)return I_VIEW;

 printf ("Erro input '%s' don't have a valid id! \n", name);
 return -1;
}

unsigned short
cpart_MI2C_24CXXX::get_out_id(char * name)
{

 if (strcmp (name, "PN_1") == 0)return O_P1;
 if (strcmp (name, "PN_2") == 0)return O_P2;
 if (strcmp (name, "PN_3") == 0)return O_P3;
 if (strcmp (name, "PN_4") == 0)return O_P4;
 if (strcmp (name, "PN_5") == 0)return O_P5;
 if (strcmp (name, "PN_6") == 0)return O_P6;
 if (strcmp (name, "PN_7") == 0)return O_P7;
 if (strcmp (name, "PN_8") == 0)return O_P8;

 if (strcmp (name, "IC_8") == 0)return O_IC;

 printf ("Erro output '%s' don't have a valid id! \n", name);
 return 1;
}

lxString
cpart_MI2C_24CXXX::WritePreferences(void)
{
 char prefs[256];

 sprintf (prefs, "%hhu,%hhu,%hhu,%hhu,%hhu,%u,%s", input_pins[0], input_pins[1], input_pins[2], input_pins[3], input_pins[4], kbits, f_mi2c_name);

 if (f_mi2c_name[0] != '*')
  {
   FILE * fout;
   fout = fopen (f_mi2c_name, "wb");
   if (fout)
    {
     fwrite (mi2c.data, mi2c.SIZE, 1, fout);
     fclose (fout);
    }
   else
    {
     printf ("Error saving to file: %s \n", f_mi2c_name);
    }
  }

 return prefs;
}

void
cpart_MI2C_24CXXX::ReadPreferences(lxString value)
{
 sscanf (value.c_str (), "%hhu,%hhu,%hhu,%hhu,%hhu,%u,%s", &input_pins[0], &input_pins[1], &input_pins[2], &input_pins[3], &input_pins[4], &kbits, f_mi2c_name);

 mi2c_end (&mi2c);
 mi2c_init (&mi2c, kbits);
 mi2c_rst (&mi2c);

 if (f_mi2c_name[0] != '*')
  {
   FILE * fout;
   fout = fopen (f_mi2c_name, "rb");
   if (fout)
    {
     fread (mi2c.data, mi2c.SIZE, 1, fout);
     fclose (fout);
    }
   else
    {
     printf ("Error loading from file: %s \n", f_mi2c_name);
    }
  }
 Reset ();
}

void
cpart_MI2C_24CXXX::ConfigurePropertiesWindow(CPWindow * WProp)
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


 ((CCombo*) WProp->GetChildByName ("combo5"))->SetItems (Items);
 if (input_pins[3] == 0)
  ((CCombo*) WProp->GetChildByName ("combo5"))->SetText ("0  NC");
 else
  {
   spin = Window5.GetPinName (input_pins[3]);
   ((CCombo*) WProp->GetChildByName ("combo5"))->SetText (itoa (input_pins[3]) + "  " + spin);
  }

 ((CCombo*) WProp->GetChildByName ("combo6"))->SetItems (Items);
 if (input_pins[4] == 0)
  ((CCombo*) WProp->GetChildByName ("combo6"))->SetText ("0  NC");
 else
  {

   spin = Window5.GetPinName (input_pins[4]);
   ((CCombo*) WProp->GetChildByName ("combo6"))->SetText (itoa (input_pins[4]) + "  " + spin);
  }

 ((CCombo*) WProp->GetChildByName ("combo9"))->SetText (itoa (kbits));


 ((CButton*) WProp->GetChildByName ("button1"))->EvMouseButtonRelease = EVMOUSEBUTTONRELEASE & CPWindow5::PropButtonRelease;
 ((CButton*) WProp->GetChildByName ("button1"))->SetTag (1);

 ((CButton*) WProp->GetChildByName ("button2"))->EvMouseButtonRelease = EVMOUSEBUTTONRELEASE & CPWindow5::PropButtonRelease;
}

void
cpart_MI2C_24CXXX::ReadPropertiesWindow(CPWindow * WProp)
{
 input_pins[0] = atoi (((CCombo*) WProp->GetChildByName ("combo1"))->GetText ());
 input_pins[1] = atoi (((CCombo*) WProp->GetChildByName ("combo2"))->GetText ());
 input_pins[2] = atoi (((CCombo*) WProp->GetChildByName ("combo3"))->GetText ());
 input_pins[3] = atoi (((CCombo*) WProp->GetChildByName ("combo5"))->GetText ());
 input_pins[4] = atoi (((CCombo*) WProp->GetChildByName ("combo6"))->GetText ());

 int nkbits = atoi (((CCombo*) WProp->GetChildByName ("combo9"))->GetText ());

 if (nkbits != kbits)
  {

   kbits = nkbits;
   mi2c_end (&mi2c);
   mi2c_init (&mi2c, kbits);
   mi2c_rst (&mi2c);
   f_mi2c_name[0] = '*';
   f_mi2c_name[1] = 0;
  }
}

void
cpart_MI2C_24CXXX::PreProcess(void)
{
 const picpin * ppins = Window5.GetPinsValues ();
 unsigned char addr = 0x50;

 if (input_pins[0])
  {
   if (ppins[input_pins[0] - 1].value)addr |= 0x01;
  }
 if (input_pins[1])
  {
   if (ppins[input_pins[1] - 1].value)addr |= 0x02;
  }
 if (input_pins[2])
  {

   if (ppins[input_pins[2] - 1].value)addr |= 0x04;
  }

 mi2c_set_addr (&mi2c, addr);

 if (input_pins[3] > 0)
  {
   Window5.Reset_i2c_bus (input_pins[3] - 1);
  }
}

void
cpart_MI2C_24CXXX::Process(void)
{
 const picpin * ppins = Window5.GetPinsValues ();

 if ((input_pins[3] > 0)&&(input_pins[4] > 0))
  Window5.Set_i2c_bus (input_pins[3] - 1, mi2c_io (&mi2c, ppins[input_pins[4] - 1].value, ppins[input_pins[3] - 1].value));

}

void
cpart_MI2C_24CXXX::EvMouseButtonPress(uint button, uint x, uint y, uint state)
{
 int i;

 for (i = 0; i < inputc; i++)
  {
   if (PointInside (x, y, input[i]))
    {

     switch (input[i].id)
      {
      case I_LOAD:
       Window5.filedialog1.SetType (lxFD_OPEN | lxFD_CHANGE_DIR);
       Window5.filedialog1.SetFilter (lxT ("PICSimLab Binary File (*.bin)|*.bin"));
       Window5.filedialog1.SetFileName (lxT ("untitled.bin"));
       Window5.Setfdtype (id);
       Window5.filedialog1.Run ();
       break;
      case I_SAVE:
       Window5.filedialog1.SetType (lxFD_SAVE | lxFD_CHANGE_DIR);
       Window5.filedialog1.SetFilter (lxT ("PICSimLab Binary File (*.bin)|*.bin"));
       Window5.filedialog1.SetFileName (lxT ("untitled.bin"));
       Window5.Setfdtype (id);
       Window5.filedialog1.Run ();
       break;
      case I_VIEW:
       FILE * fout;
       fout = fopen (f_mi2c_tmp_name, "w");
       if (fout)
        {
         for (unsigned int i = 0; i < mi2c.SIZE; i += 16)
          {
           fprintf (fout, "%04X: ", i);
           for (int j = 0; j < 16; j++)
            {
             fprintf (fout, "%02X ", mi2c.data[j + i ]);
            }
           fprintf (fout, "\r\n");
          }
         fclose (fout);
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
         }, f_mi2c_tmp_name);
#else 
         lxLaunchDefaultApplication (f_mi2c_tmp_name);
#endif         
        }
       else
        {
         printf ("Error saving to file: %s \n", f_mi2c_tmp_name);
        }
       break;
      }
    }
  }
}

void
cpart_MI2C_24CXXX::filedialog_EvOnClose(int retId)
{

 if (retId)
  {

   if ((Window5.filedialog1.GetType () == (lxFD_SAVE | lxFD_CHANGE_DIR)))
    {
     if (lxFileExists (Window5.filedialog1.GetFileName ()))
      {

       if (!Dialog (lxString ("Overwriting file: ") + basename (Window5.filedialog1.GetFileName ()) + "?"))
        return;
      }

     FILE * fout;
     fout = fopen (Window5.filedialog1.GetFileName (), "wb");
     if (fout)
      {
       fwrite (mi2c.data, mi2c.SIZE, 1, fout);
       fclose (fout);
       strncpy (f_mi2c_name, Window5.filedialog1.GetFileName (), 199);
      }
     else
      {
       printf ("Error saving to file: %s \n", (const char *) Window5.filedialog1.GetFileName ().c_str ());
      }
    }

   if ((Window5.filedialog1.GetType () == (lxFD_OPEN | lxFD_CHANGE_DIR)))
    {
     FILE * fout;
     fout = fopen (Window5.filedialog1.GetFileName (), "rb");
     if (fout)
      {
       fread (mi2c.data, mi2c.SIZE, 1, fout);
       fclose (fout);
      }
     else
      {
       printf ("Error loading from file: %s \n", (const char *) Window5.filedialog1.GetFileName ().c_str ());
      }
    }
  }
}

part_init(PART_MI2C_24CXXX_Name, cpart_MI2C_24CXXX, "Other");

