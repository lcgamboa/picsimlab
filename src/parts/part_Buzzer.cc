/* ########################################################################

   PICsimLab - PIC laboratory simulator

   ########################################################################

   Copyright (c) : 2019-2020  Luis Claudio Gambôa Lopes

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
#include"part_Buzzer.h"

/* outputs */
enum
{
 O_P1, O_P2, O_L1
};

enum
{
 ACTIVE = 0, PASSIVE
};

cpart_Buzzer::cpart_Buzzer(unsigned x, unsigned y)
{
 X = x;
 Y = y;
 ReadMaps ();

 lxImage image;
 image.LoadFile (Window1.GetSharePath () + lxT ("parts/") + GetPictureFileName ());

 Bitmap = new lxBitmap (image, &Window5);
 image.Destroy ();
 canvas.Create (Window5.GetWWidget (), Bitmap);

 input_pins[0] = 0;

 mcount = 0;

 buzzer.Init ();
 type = ACTIVE;

 samplerate = buzzer.GetSampleRate ();
 buffersize = samplerate / 5 ; //0.1 seconds
 buffer = NULL;
 maxv = buzzer.GetMax ();
 buffercount = 0;

 in[0] = 0;
 in[1] = 0;
 in[2] = 0;

 out[0] = 0;
 out[1] = 0;
 out[2] = 0;


}

cpart_Buzzer::~cpart_Buzzer(void)
{
 delete Bitmap;
 if (buffer)
  {
   delete[] buffer;
  }
 canvas.Destroy ();
 buzzer.End ();

}

void
cpart_Buzzer::Draw(void)
{

 int i;

 const picpin * ppins = Window5.GetPinsValues ();

 canvas.Init ();

 lxFont font (9, lxFONTFAMILY_TELETYPE, lxFONTSTYLE_NORMAL, lxFONTWEIGHT_BOLD);
 canvas.SetFont (font);

 for (i = 0; i < outputc; i++)
  {
   canvas.SetFgColor (30, 0, 0);

   switch (output[i].id)
    {
    case O_P1:
     canvas.SetColor (49, 61, 99);
     canvas.Rectangle (1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
     canvas.SetFgColor (255, 255, 255);
     if (input_pins[output[i].id - O_P1] == 0)
      canvas.Text ("NC", output[i].x1, output[i].y1);
     else
      canvas.Text (Window5.GetPinName (input_pins[output[i].id - O_P1]), output[i].x1, output[i].y1);
     break;
    case O_P2:
     canvas.SetColor (49, 61, 99);
     canvas.Rectangle (1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
     canvas.SetFgColor (255, 255, 255);
     canvas.Text ("GND", output[i].x1, output[i].y1);
    case O_L1:
     unsigned char r = 0;
     if (input_pins[0] > 0)
      r = ppins[input_pins[0] - 1].oavalue;
     canvas.SetColor (r, 0, 0);
     canvas.Circle (1, output[i].x1, output[i].y1, output[i].r);
     break;
    }


  }

 canvas.End ();
}

unsigned short
cpart_Buzzer::get_in_id(char * name)
{
 printf ("Erro input '%s' don't have a valid id! \n", name);
 return -1;
}

unsigned short
cpart_Buzzer::get_out_id(char * name)
{

 if (strcmp (name, "P1") == 0)return O_P1;
 if (strcmp (name, "P2") == 0)return O_P2;
 if (strcmp (name, "L1") == 0)return O_L1;

 printf ("Erro output '%s' don't have a valid id! \n", name);
 return 1;
};

lxString
cpart_Buzzer::WritePreferences(void)
{
 char prefs[256];

 sprintf (prefs, "%hhu,%hhu", input_pins[0], type);
 return prefs;
}

void
cpart_Buzzer::ReadPreferences(lxString value)
{
 unsigned char tp;
 sscanf (value.c_str (), "%hhu,%hhu", &input_pins[0], &tp);
 ChangeType (tp);
}


void
cpart_Buzzer::ConfigurePropertiesWindow(CPWindow * WProp)
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

 if (type == 0)
  ((CCombo*) WProp->GetChildByName ("combo2"))->SetText ("Active");
 else
  {
   ((CCombo*) WProp->GetChildByName ("combo2"))->SetText ("Passive");
  }

 ((CButton*) WProp->GetChildByName ("button1"))->EvMouseButtonRelease = EVMOUSEBUTTONRELEASE & CPWindow5::PropButtonRelease;
 ((CButton*) WProp->GetChildByName ("button1"))->SetTag (1);

 ((CButton*) WProp->GetChildByName ("button2"))->EvMouseButtonRelease = EVMOUSEBUTTONRELEASE & CPWindow5::PropButtonRelease;
}

void
cpart_Buzzer::ReadPropertiesWindow(CPWindow * WProp)
{
 input_pins[0] = atoi (((CCombo*) WProp->GetChildByName ("combo1"))->GetText ());
 unsigned char tp = ((CCombo*) WProp->GetChildByName ("combo2"))->GetText ().compare (lxT ("Active"));
 ChangeType (tp);
}

void
cpart_Buzzer::PreProcess(void)
{
 if (type == PASSIVE)
  {

   JUMPSTEPS_ = (Window1.GetBoard ()->MGetInstClock () / samplerate)-100 ;
   mcount = JUMPSTEPS_;

  }
}

void
cpart_Buzzer::Process(void)
{

 if (type == PASSIVE)
  {
   const picpin * ppins = Window5.GetPinsValues ();

   mcount++;

   if (mcount > JUMPSTEPS_)
    {
     if ((input_pins[0])&&(buffercount < buffersize))
      {
       /*       
              0.7837 z-1 - 0.7837 z-2
        y1:  ----------------------
             1 - 1.196 z-1 + 0.2068 z-2
        */
       in[2] = in[1];
       in[1] = in[0];
       in[0] = ((2.0 * ppins[input_pins[0] - 1].value) - 1.0) * maxv * 0.5;

       out[2] = out[1];
       out[1] = out[0];
       out[0] = 0.7837 * in[1] - 0.7837 * in[2] + 1.196 * out[1] - 0.2068 * out[2];

       buffer[buffercount++] = out[0];

      }

     mcount = 0;
    }
  }
}

void
cpart_Buzzer::PostProcess(void)
{
 const picpin * ppins = Window5.GetPinsValues ();

 if (type == ACTIVE)
  {
   if ((ppins[input_pins[0] - 1].oavalue - 30) > 10)
    {
     buzzer.BeepStart ();
    }
   else
    {
     buzzer.BeepStop ();
    }
  }
 else
  {
   buzzer.SoundPlay (buffer, buffersize);
   buffercount = 0;
  }

}

void
cpart_Buzzer::ChangeType(unsigned char tp)
{
 if (type == tp) return;

 if (type == ACTIVE)
  {
   buzzer.BeepStop ();
   if (!buffer)
    {
     buffer = new short[buffersize];
    }
   type = tp;
  }
 else //PASSIVE
  {
   if (buffer)
    {
     delete[] buffer;
     buffer = NULL;
    }
   type = tp;
  }
}

part_init("Buzzer", cpart_Buzzer);

