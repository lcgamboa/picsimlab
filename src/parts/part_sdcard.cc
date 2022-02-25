/* ########################################################################

   PICsimLab - PIC laboratory simulator

   ########################################################################

   Copyright (c) : 2020-2022  Luis Claudio Gambôa Lopes

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
#include"part_sdcard.h"

/* outputs */
enum
{
 O_P3, O_P4, O_P5, O_P6, O_FILE
};

/* inputs */
enum
{
 I_CONN
};

/*
const char pin_names[10][10] = {
 "GND",
 "VCC",
 "MISO",
 "MOSI",
 "SCK",
 "CS"
};

const char pin_values[10][10] = {
 "GND",
 "+5V",
 {0},
 {1},
 {2},
 {3}
};
 */


cpart_SDCard::cpart_SDCard(unsigned x, unsigned y) :
font(8, lxFONTFAMILY_TELETYPE, lxFONTSTYLE_NORMAL, lxFONTWEIGHT_BOLD)
{
 X = x;
 Y = y;
 ReadMaps ();
 Bitmap = NULL;

 LoadImage();

 sdcard_init (&sd);
 sdcard_rst (&sd);

 input_pins[0] = 0;
 input_pins[1] = 0;
 input_pins[2] = 0;

 output_pins[0] = 0;

 _ret = -1;

 sdcard_fname[0] = '*';
 sdcard_fname[1] = 0;
}

cpart_SDCard::~cpart_SDCard(void)
{
 delete Bitmap;
 canvas.Destroy ();
 sdcard_end (&sd);
}

void
cpart_SDCard::Reset(void)
{
 sdcard_rst (&sd);
}

void
cpart_SDCard::Draw(void)
{

 int i;
 int to;

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
      case O_FILE:
       canvas.SetColor (49, 61, 99);
       canvas.Rectangle (1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
       canvas.SetFgColor (255, 255, 255);
       to = strlen (sdcard_fname) + 4;
       if (to < 38)
        {
         to = 0;
        }
       else
        {
         to = to - 38;
        }
       canvas.RotatedText (lxT ("Img:") + lxString (sdcard_fname + to), output[i].x1, output[i].y1, 0);
       break;
      default:
       canvas.SetColor (49, 61, 99);
       canvas.Rectangle (1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);

       canvas.SetFgColor (155, 155, 155);

       int pinv = output[i].id - O_P3;
       int pin = 0;
       switch (pinv)
        {
        case 0:
        case 4:
         pin = pinv > 1;
         if (output_pins[pin] == 0)
          canvas.RotatedText ("NC", output[i].x1, output[i].y2, 90.0);
         else
          canvas.RotatedText (Window5.GetPinName (output_pins[pin]), output[i].x1, output[i].y2, 90.0);
         break;
        case 1:
        case 2:
        case 3:
         pinv++;
        case 5:
         pin = pinv - 2;
         if (input_pins[pin] == 0)
          canvas.RotatedText ("NC", output[i].x1, output[i].y2, 90.0);
         else
          canvas.RotatedText (Window5.GetPinName (input_pins[pin]), output[i].x1, output[i].y2, 90.0);
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
cpart_SDCard::get_in_id(char * name)
{
 if (strcmp (name, "CN_CONN") == 0)return I_CONN;

 printf ("Erro input '%s' don't have a valid id! \n", name);
 return -1;
}

unsigned short
cpart_SDCard::get_out_id(char * name)
{

 if (strcmp (name, "PN_3") == 0)return O_P3;
 if (strcmp (name, "PN_4") == 0)return O_P4;
 if (strcmp (name, "PN_5") == 0)return O_P5;
 if (strcmp (name, "PN_6") == 0)return O_P6;
 if (strcmp (name, "DI_FILE") == 0)return O_FILE;


 printf ("Erro output '%s' don't have a valid id! \n", name);
 return 1;
}

lxString
cpart_SDCard::WritePreferences(void)
{
 char prefs[256];

 sprintf (prefs, "%hhu,%hhu,%hhu,%hhu,%s", input_pins[0], input_pins[1], input_pins[2], output_pins[0], sdcard_fname);

 return prefs;
}

void
cpart_SDCard::ReadPreferences(lxString value)
{
 sscanf (value.c_str (), "%hhu,%hhu,%hhu,%hhu,%s", &input_pins[0], &input_pins[1], &input_pins[2], &output_pins[0], sdcard_fname);

 Reset ();
 if (sdcard_fname[0] != '*')
  {
#ifdef _WIN_   
   if (!strncmp (sdcard_fname, "/tmp/", 5))
    {
     char buff[200];
     strcpy (buff, (const char *) lxGetTempDir ("PICSimLab").c_str ());
     strcat (buff, sdcard_fname + 4);
     strcpy (sdcard_fname, buff);
    }
#endif   
   sdcard_set_filename (&sd, sdcard_fname);
   sdcard_set_card_present (&sd, 1);
  }
 else
  {
   sdcard_set_card_present (&sd, 0);
  }
}

void
cpart_SDCard::ConfigurePropertiesWindow(CPWindow * WProp)
{
 lxString Items = Window5.GetPinsNames ();
 lxString spin;

 ((CCombo*) WProp->GetChildByName ("combo1"))->SetItems (Items);
 if (output_pins[0] == 0)
  ((CCombo*) WProp->GetChildByName ("combo1"))->SetText ("0  NC");
 else
  {
   spin = Window5.GetPinName (output_pins[0]);
   ((CCombo*) WProp->GetChildByName ("combo1"))->SetText (itoa (output_pins[0]) + "  " + spin);
  }

 ((CCombo*) WProp->GetChildByName ("combo2"))->SetItems (Items);
 if (input_pins[0] == 0)
  ((CCombo*) WProp->GetChildByName ("combo2"))->SetText ("0  NC");
 else
  {
   spin = Window5.GetPinName (input_pins[0]);
   ((CCombo*) WProp->GetChildByName ("combo2"))->SetText (itoa (input_pins[0]) + "  " + spin);
  }

 ((CCombo*) WProp->GetChildByName ("combo3"))->SetItems (Items);
 if (input_pins[1] == 0)
  ((CCombo*) WProp->GetChildByName ("combo3"))->SetText ("0  NC");
 else
  {
   spin = Window5.GetPinName (input_pins[1]);
   ((CCombo*) WProp->GetChildByName ("combo3"))->SetText (itoa (input_pins[1]) + "  " + spin);
  }


 ((CCombo*) WProp->GetChildByName ("combo4"))->SetItems (Items);
 if (input_pins[2] == 0)
  ((CCombo*) WProp->GetChildByName ("combo4"))->SetText ("0  NC");
 else
  {

   spin = Window5.GetPinName (input_pins[2]);
   ((CCombo*) WProp->GetChildByName ("combo4"))->SetText (itoa (input_pins[2]) + "  " + spin);
  }

 ((CButton*) WProp->GetChildByName ("button1"))->EvMouseButtonRelease = EVMOUSEBUTTONRELEASE & CPWindow5::PropButtonRelease;
 ((CButton*) WProp->GetChildByName ("button1"))->SetTag (1);

 ((CButton*) WProp->GetChildByName ("button2"))->EvMouseButtonRelease = EVMOUSEBUTTONRELEASE & CPWindow5::PropButtonRelease;
}

void
cpart_SDCard::ReadPropertiesWindow(CPWindow * WProp)
{
 output_pins[0] = atoi (((CCombo*) WProp->GetChildByName ("combo1"))->GetText ());
 input_pins [0] = atoi (((CCombo*) WProp->GetChildByName ("combo2"))->GetText ());
 input_pins [1] = atoi (((CCombo*) WProp->GetChildByName ("combo3"))->GetText ());
 input_pins [2] = atoi (((CCombo*) WProp->GetChildByName ("combo4"))->GetText ());
}

void
cpart_SDCard::PreProcess(void) { }

void
cpart_SDCard::Process(void)
{
 const picpin * ppins = Window5.GetPinsValues ();

 unsigned short ret = 0;

 ret = sdcard_io (&sd, ppins[input_pins[0] - 1].value, ppins[input_pins[1] - 1].value, ppins[input_pins[2] - 1].value);

 if (!ppins[input_pins[2] - 1].value)//if SS is active, update output 
  {
   if (_ret != ret)
    {
     Window5.SetPin (output_pins[0], ret);
    }
   _ret = ret;
  }
 else
  {
   _ret = 0xFF; //invalid value
  }
}

void
cpart_SDCard::EvMouseButtonPress(uint button, uint x, uint y, uint state)
{
 int i;

 for (i = 0; i < inputc; i++)
  {
   if (PointInside (x, y, input[i]))
    {
     switch (input[i].id)
      {
      case I_CONN:
       Window5.filedialog1.SetType (lxFD_OPEN | lxFD_CHANGE_DIR);
       Window5.filedialog1.SetFilter (lxT ("SD Card image (*.img)|*.img"));
       if (sdcard_fname[0] == '*')
        {
         Window5.filedialog1.SetFileName (lxT ("untitled.img"));
        }
       else
        {
         Window5.filedialog1.SetFileName (sdcard_fname);
        }
       Window5.Setfdtype (id);
       Window5.filedialog1.Run ();
       break;
      }
    }
  }
}

void
cpart_SDCard::filedialog_EvOnClose(int retId)
{

 if (retId)
  {

   if ((Window5.filedialog1.GetType () == (lxFD_OPEN | lxFD_CHANGE_DIR)))
    {
     if (lxFileExists (Window5.filedialog1.GetFileName ()))
      {
       strncpy (sdcard_fname, Window5.filedialog1.GetFileName ().c_str (), 199);
       sdcard_set_filename (&sd, sdcard_fname);
       sdcard_set_card_present (&sd, 1);
      }
     else
      {
       sdcard_set_card_present (&sd, 0);
      }

    }
  }
 output_ids[O_FILE]->update = 1;
}

void
cpart_SDCard::PostProcess(void) { }

part_init(PART_SDCARD_Name, cpart_SDCard, "Other");

