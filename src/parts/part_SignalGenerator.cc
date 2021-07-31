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

#include "../picsimlab1.h"
#include "../picsimlab4.h"
#include "../picsimlab5.h"
#include "part_SignalGenerator.h"

/* outputs */
enum
{
 O_P1, O_P2, O_P3, O_PO1, O_PO2, O_PO3, O_TP, O_AMPL, O_OFFS, O_FREQ, O_MF
};

/* inputs */
enum
{
 I_PO1, I_PO2, I_PO3, I_TP, I_MF
};

cpart_SignalGenerator::cpart_SignalGenerator(unsigned x, unsigned y) :
font(9, lxFONTFAMILY_TELETYPE, lxFONTSTYLE_NORMAL, lxFONTWEIGHT_BOLD)
{
 X = x;
 Y = y;
 ReadMaps ();

 lxImage image (&Window5);
 image.LoadFile (lxGetLocalFile (Window1.GetSharePath () + lxT ("parts/") + GetPictureFileName ()), Orientation, Scale, Scale);

 Bitmap = new lxBitmap (&image, &Window5);
 image.Destroy ();

 canvas.Create (Window5.GetWWidget (), Bitmap);


 input_pins[0] = 0;
 input_pins[1] = 0;

 values[0] = 0;
 values[1] = 0;
 values[2] = 0;

 active[0] = 0;
 active[1] = 0;
 active[2] = 0;

 type = 0;
 ts = 0;
 maxfreq = 1;
 lastd = 2;
}

cpart_SignalGenerator::~cpart_SignalGenerator(void)
{
 delete Bitmap;
 canvas.Destroy ();
}

void
cpart_SignalGenerator::Draw(void)
{

 int i, j;
 lxString temp;
 float v[2];
 float tsi;
 int sizex;
 int sizey;

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
      case O_P2:
      case O_P3:
       canvas.SetColor (49, 61, 99);
       canvas.Rectangle (1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
       canvas.SetFgColor (255, 255, 255);
       if (input_pins[output[i].id - O_P2] == 0)
        canvas.RotatedText ("NC", output[i].x1, output[i].y1, 0);
       else
        canvas.RotatedText (Window5.GetPinName (input_pins[output[i].id - O_P2]), output[i].x1, output[i].y1, 0);
       break;
      case O_P1:
       canvas.SetColor (49, 61, 99);
       canvas.Rectangle (1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
       canvas.SetFgColor (255, 255, 255);
       canvas.RotatedText ("GND", output[i].x1, output[i].y1, 0);
       break;
      case O_PO1:
      case O_PO2:
      case O_PO3:
       draw_potentiometer (&output[i], 200 - values[output[i].id - O_PO1], "", font);
       break;
      case O_TP:
       canvas.SetColor (255, 255, 255);
       canvas.Rectangle (1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
       canvas.SetFgColor (0, 0, 0);

       v[0] = 0;
       tsi = 0;
       sizex = output[i].x2 - output[i].x1;
       sizey = output[i].y2 - output[i].y1;

       for (j = 1; j < sizex; j++)
        {
         v[1] = v[0];
         switch (type)
          {
          case 0:
           v[0] = (sin (tsi));
           break;
          case 1:
           v[0] = ((sin (tsi) > 0) - 0.5)*2;
           break;
          case 2:
           v[0] = ((acos (sin (tsi)) / 1.5708) - 1);
           break;
          }
         tsi += 3 * 6.28 / sizex;
         if (j > 0)
          {
           canvas.Line (output[i].x1 + j - 1, output[i].y1 + ((v[1] + 2.0) * sizey / 4.0), output[i].x1 + j, output[i].y1 + ((v[0] + 2.0) * sizey / 4.0));
          }
        }

       break;
      case O_AMPL:
       temp.Printf ("%5.2f", ampl);
       canvas.SetColor (49, 61, 99);
       canvas.Rectangle (1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
       canvas.SetFgColor (255, 255, 255);
       canvas.RotatedText (temp, output[i].x1, output[i].y1, 0);
       break;
      case O_OFFS:
       temp.Printf ("%5.2f", offs);
       canvas.SetColor (49, 61, 99);
       canvas.Rectangle (1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
       canvas.SetFgColor (255, 255, 255);
       canvas.RotatedText (temp, output[i].x1, output[i].y1, 0);
       break;
      case O_FREQ:
       temp.Printf ("%5.2f", freq);
       canvas.SetColor (49, 61, 99);
       canvas.Rectangle (1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
       canvas.SetFgColor (255, 255, 255);
       canvas.RotatedText (temp, output[i].x1, output[i].y1, 0);
       break;
      case O_MF:
       canvas.SetColor (255, 255, 255);
       canvas.Rectangle (1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
       canvas.SetColor (0, 0, 0);
       temp.Printf ("F x %i", maxfreq);
       canvas.RotatedText (temp, output[i].x1, output[i].y1, 0);
       break;
      }
    }
  }

 if (Update)
  {
   canvas.End ();
  }
}

void
cpart_SignalGenerator::PreProcess(void)
{
 JUMPSTEPS_ = (Window1.GetBoard ()->MGetInstClockFreq () / 250000);
 mcount = JUMPSTEPS_;

 freq = (maxfreq * values[1] / 200.0);
 ampl = (5.0 * values[0] / 200.0);
 offs = (5.0 * values[2] / 200.0);
}

void
cpart_SignalGenerator::Process(void)
{

 mcount++;

 if (mcount > JUMPSTEPS_)
  {
   float v = 0;
   float wt = freq * 2.0 * M_PI * ts;

   switch (type)
    {
    case 0:
     v = (ampl * sin (wt)) + offs;
     break;
    case 1:
     v = ((sin (wt) > 0) - 0.5)*2 * ampl + offs;
     break;
    case 2:
     v = ((acos (sin (wt)) / 1.5708) - 1) * ampl + offs;
     break;
    }
   ts += 4e-6;

   if (wt >= 2.0 * M_PI)
    {
     ts = ts - (1.0 / freq);
    }

   Window5.SetAPin (input_pins[0], v);
   Window5.SetAPin (input_pins[1], v);

   unsigned char vald = v > offs;
   if (vald != lastd)
    {
     lastd = vald;
     Window5.SetPin (input_pins[0], vald);
     Window5.SetPin (input_pins[1], vald);
    }
   mcount = -1;
  }

}

void
cpart_SignalGenerator::EvMouseButtonPress(uint button, uint x, uint y, uint state)
{

 int i;

 for (i = 0; i < inputc; i++)
  {
   if (PointInside (x, y, input[i]))
    {
     switch (Orientation)
      {
      case 1:
       y = Height - x;
       break;
      case 2:
       y = Height - y;
       break;
      case 3:
       y = x;
       break;
      }
     switch (input[i].id)
      {
      case I_PO1:
       values[0] = (y - input[i].y1)*1.66;
       if (values[0] > 200)values[0] = 200;
       active[0] = 1;
       output_ids[O_PO1]->update = 1;
       output_ids[O_AMPL]->update = 1;
       break;
      case I_PO2:
       values[1] = (y - input[i].y1)*1.66;
       if (values[1] > 200)values[1] = 200;
       active[1] = 1;
       output_ids[O_PO2]->update = 1;
       output_ids[O_FREQ]->update = 1;
       break;
      case I_PO3:
       values[2] = (y - input[i].y1)*1.66;
       if (values[2] > 200)values[2] = 200;
       active[2] = 1;
       output_ids[O_PO3]->update = 1;
       output_ids[O_OFFS]->update = 1;
       break;
      case I_TP:
       type++;
       if (type > 2)type = 0;
       output_ids[O_TP]->update = 1;
       break;
      case I_MF:
       maxfreq *= 10;
       if (maxfreq > 10000)maxfreq = 1;
       output_ids[O_MF]->update = 1;
       break;
      }
    }
  }

}

void
cpart_SignalGenerator::EvMouseButtonRelease(uint button, uint x, uint y, uint state)
{
 int i;

 for (i = 0; i < inputc; i++)
  {
   if (PointInside (x, y, input[i]))
    {
     switch (input[i].id)
      {
      case I_PO1:
       active[0] = 0;
       output_ids[O_PO1]->update = 1;
       output_ids[O_AMPL]->update = 1;
       break;
      case I_PO2:
       active[1] = 0;
       output_ids[O_PO2]->update = 1;
       output_ids[O_FREQ]->update = 1;
       break;
      case I_PO3:
       active[2] = 0;
       output_ids[O_PO3]->update = 1;
       output_ids[O_OFFS]->update = 1;
       break;
      }
    }
  }
}

void
cpart_SignalGenerator::EvMouseMove(uint button, uint x, uint y, uint state)
{

 int i;

 for (i = 0; i < inputc; i++)
  {
   if (PointInside (x, y, input[i]))
    {
     switch (Orientation)
      {
      case 1:
       y = Height - x;
       break;
      case 2:
       y = Height - y;
       break;
      case 3:
       y = x;
       break;
      }
     switch (input[i].id)
      {
      case I_PO1:
       if (active[0])
        {
         values[0] = (y - input[i].y1)*1.66;
         if (values[0] > 200)values[0] = 200;
         output_ids[O_PO1]->update = 1;
         output_ids[O_AMPL]->update = 1;
        }
       break;
      case I_PO2:
       if (active[1])
        {
         values[1] = (y - input[i].y1)*1.66;
         if (values[1] > 200)values[1] = 200;
         output_ids[O_PO2]->update = 1;
         output_ids[O_FREQ]->update = 1;
        }
       break;
      case I_PO3:
       if (active[2])
        {
         values[2] = (y - input[i].y1)*1.66;
         if (values[2] > 200)values[2] = 200;
         output_ids[O_PO3]->update = 1;
         output_ids[O_OFFS]->update = 1;
        }
       break;
      }
    }
  }
}

unsigned short
cpart_SignalGenerator::get_in_id(char * name)
{

 if (strcmp (name, "PO_1") == 0)return I_PO1;
 if (strcmp (name, "PO_2") == 0)return I_PO2;
 if (strcmp (name, "PO_3") == 0)return I_PO3;
 if (strcmp (name, "PB_TP") == 0)return I_TP;
 if (strcmp (name, "PB_MF") == 0)return I_MF;

 printf ("Erro input '%s' don't have a valid id! \n", name);
 return -1;
}

unsigned short
cpart_SignalGenerator::get_out_id(char * name)
{

 if (strcmp (name, "PN_1") == 0)return O_P1;
 if (strcmp (name, "PN_2") == 0)return O_P2;
 if (strcmp (name, "PN_3") == 0)return O_P3;

 if (strcmp (name, "PO_1") == 0)return O_PO1;
 if (strcmp (name, "PO_2") == 0)return O_PO2;
 if (strcmp (name, "PO_3") == 0)return O_PO3;
 if (strcmp (name, "PB_TP") == 0)return O_TP;
 if (strcmp (name, "DI_AMPL") == 0)return O_AMPL;
 if (strcmp (name, "DI_OFFS") == 0)return O_OFFS;
 if (strcmp (name, "DI_FREQ") == 0)return O_FREQ;
 if (strcmp (name, "PB_MF") == 0)return O_MF;

 printf ("Erro output '%s' don't have a valid id! \n", name);
 return 1;
}

lxString
cpart_SignalGenerator::WritePreferences(void)
{
 char prefs[256];

 sprintf (prefs, "%hhu,%hhu,%hhu,%hhu,%u,%hhu,%hhu", input_pins[0], values[0], values[1], type, maxfreq,
          input_pins[1], values[2]);

 return prefs;
}

void
cpart_SignalGenerator::ReadPreferences(lxString value)
{
 sscanf (value.c_str (), "%hhu,%hhu,%hhu,%hhu,%u,%hhu,%hhu", &input_pins[0], &values[0], &values[1],
         &type, &maxfreq, &input_pins[1], &values[2]);

 RegisterRemoteControl ();
}

void
cpart_SignalGenerator::RegisterRemoteControl(void)
{
 for (int i = 0; i < outputc; i++)
  {
   switch (output[i].id)
    {
    case O_AMPL:
     output[i].status = (void *) &ampl;
     break;
    case O_FREQ:
     output[i].status = (void *) &freq;
     break;
    case O_OFFS:
     output[i].status = (void *) &offs;
     break;
    }
  }
}

void
cpart_SignalGenerator::ConfigurePropertiesWindow(CPWindow * WProp)
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


 ((CButton*) WProp->GetChildByName ("button1"))->EvMouseButtonRelease = EVMOUSEBUTTONRELEASE & CPWindow5::PropButtonRelease;
 ((CButton*) WProp->GetChildByName ("button1"))->SetTag (1);

 ((CButton*) WProp->GetChildByName ("button2"))->EvMouseButtonRelease = EVMOUSEBUTTONRELEASE & CPWindow5::PropButtonRelease;
}

void
cpart_SignalGenerator::ReadPropertiesWindow(CPWindow * WProp)
{
 input_pins[0] = atoi (((CCombo*) WProp->GetChildByName ("combo1"))->GetText ());
 input_pins[1] = atoi (((CCombo*) WProp->GetChildByName ("combo2"))->GetText ());
 RegisterRemoteControl ();
}

part_init(PART_SIGNALGENERATOR_Name, cpart_SignalGenerator, "Virtual");

