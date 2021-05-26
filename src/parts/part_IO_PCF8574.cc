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
#include"part_IO_PCF8574.h"

/* outputs */
enum
{
 O_P1, O_P2, O_P3, O_P4, O_P5, O_P6, O_P7, O_P8, O_P9, O_P10, O_P11, O_P12,
 O_P13, O_P14, O_P15, O_P16, O_IC
};

const char pin_names[16][10] = {
 "A0",
 "A1",
 "A2",
 "P0",
 "P1",
 "P2",
 "P3",
 "VSS",
 "P4",
 "P5",
 "P6",
 "P7",
 "/INT",
 "SCL",
 "SDA",
 "VDD"
};

const char pin_values[16][10] = {
 {2},
 {3},
 {4},
 {5},
 {6},
 {7},
 {8},
 "GND",
 {9},
 {10},
 {11},
 {12},
 {13},
 {0},
 {1},
 "+5V"
};

cpart_IO_PCF8574::cpart_IO_PCF8574(unsigned x, unsigned y) :
font(8, lxFONTFAMILY_TELETYPE, lxFONTSTYLE_NORMAL, lxFONTWEIGHT_BOLD)
{
 X = x;
 Y = y;
 ReadMaps ();
 Bitmap = NULL;

 lxImage image (&Window5);

 image.LoadFile (Window1.GetSharePath () + lxT ("parts/") + GetPictureFileName (), Orientation, Scale, Scale);


 Bitmap = new lxBitmap (&image, &Window5);
 image.Destroy ();
 canvas.Create (Window5.GetWWidget (), Bitmap);

 io_PCF8574_init (&ioe8);
 io_PCF8574_rst (&ioe8);

 input_pins[0] = 0;
 input_pins[1] = 0;
 input_pins[2] = 0;
 input_pins[3] = 0;
 input_pins[4] = 0;

 output_pins[0] = Window5.RegisterIOpin (lxT ("P0"));
 output_pins[1] = Window5.RegisterIOpin (lxT ("P1"));
 output_pins[2] = Window5.RegisterIOpin (lxT ("P2"));
 output_pins[3] = Window5.RegisterIOpin (lxT ("P3"));
 output_pins[4] = Window5.RegisterIOpin (lxT ("P4"));
 output_pins[5] = Window5.RegisterIOpin (lxT ("P5"));
 output_pins[6] = Window5.RegisterIOpin (lxT ("P6"));
 output_pins[7] = Window5.RegisterIOpin (lxT ("P7"));
 output_pins[8] = Window5.RegisterIOpin (lxT ("/INT"));

 mcount = 0;
 memset (output_pins_alm, 0, 9 * sizeof (unsigned long));

 _ret = 255;
}

cpart_IO_PCF8574::~cpart_IO_PCF8574(void)
{
 for (int i = 0; i < 9; i++)
  Window5.UnregisterIOpin (output_pins[i]);
 delete Bitmap;
 canvas.Destroy ();
}

void
cpart_IO_PCF8574::Draw(void)
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
       canvas.SetColor (26, 26, 26);
       canvas.Rectangle (1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
       canvas.SetFgColor (255, 255, 255);
       canvas.RotatedText ("PCF8574", output[i].x1 + 14, output[i].y2 - 15, 0.0);
       break;
      default:
       canvas.SetColor (49, 61, 99);
       canvas.Rectangle (1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);

       canvas.SetFgColor (255, 255, 255);
       canvas.RotatedText (pin_names[output[i].id - O_P1], output[i].x1, output[i].y2, 90.0);

       int pinv = pin_values[output[i].id - O_P1][0];
       if (pinv > 13)
        {
         canvas.SetFgColor (155, 155, 155);
         canvas.RotatedText (pin_values[output[i].id - O_P1], output[i].x1, output[i].y2 - 30, 90.0);
        }
       else if (pinv < 5)
        {
         if (input_pins[pinv] == 0)
          canvas.RotatedText ("NC", output[i].x1, output[i].y2 - 30, 90.0);
         else
          canvas.RotatedText (Window5.GetPinName (input_pins[pinv]), output[i].x1, output[i].y2 - 30, 90.0);
        }
       else
        {
         if (output_pins[pinv - 5] == 0)
          canvas.RotatedText ("NC", output[i].x1, output[i].y2 - 30, 90.0);
         else
          canvas.RotatedText (itoa (output_pins[pinv - 5]) /*+ lxT (" ") + Window5.GetPinName (output_pins[pinv - 5])*/, output[i].x1, output[i].y2 - 30, 90.0);
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
cpart_IO_PCF8574::get_in_id(char * name)
{
 printf ("Erro input '%s' don't have a valid id! \n", name);
 return -1;
};

unsigned short
cpart_IO_PCF8574::get_out_id(char * name)
{

 if (strcmp (name, "PN_1") == 0)return O_P1;
 if (strcmp (name, "PN_2") == 0)return O_P2;
 if (strcmp (name, "PN_3") == 0)return O_P3;
 if (strcmp (name, "PN_4") == 0)return O_P4;
 if (strcmp (name, "PN_5") == 0)return O_P5;
 if (strcmp (name, "PN_6") == 0)return O_P6;
 if (strcmp (name, "PN_7") == 0)return O_P7;
 if (strcmp (name, "PN_8") == 0)return O_P8;
 if (strcmp (name, "PN_9") == 0)return O_P9;
 if (strcmp (name, "PN_10") == 0)return O_P10;
 if (strcmp (name, "PN_11") == 0)return O_P11;
 if (strcmp (name, "PN_12") == 0)return O_P12;
 if (strcmp (name, "PN_13") == 0)return O_P13;
 if (strcmp (name, "PN_14") == 0)return O_P14;
 if (strcmp (name, "PN_15") == 0)return O_P15;
 if (strcmp (name, "PN_16") == 0)return O_P16;

 if (strcmp (name, "IC_16") == 0)return O_IC;

 printf ("Erro output '%s' don't have a valid id! \n", name);
 return 1;
};

lxString
cpart_IO_PCF8574::WritePreferences(void)
{
 char prefs[256];

 sprintf (prefs, "%hhu,%hhu,%hhu,%hhu,%hhu,%hhu", input_pins[0], input_pins[1], input_pins[2], input_pins[3], input_pins[4], output_pins[0]);

 return prefs;
}

void
cpart_IO_PCF8574::ReadPreferences(lxString value)
{
 unsigned char outp;
 sscanf (value.c_str (), "%hhu,%hhu,%hhu,%hhu,%hhu,%hhu", &input_pins[0], &input_pins[1], &input_pins[2], &input_pins[3], &input_pins[4], &outp);

 if (output_pins[0] != outp)
  {

   for (int i = 0; i < 9; i++)
    Window5.UnregisterIOpin (output_pins[i]);

   output_pins[0] = Window5.RegisterIOpin (lxT ("P0"), outp++);
   output_pins[1] = Window5.RegisterIOpin (lxT ("P1"), outp++);
   output_pins[2] = Window5.RegisterIOpin (lxT ("P2"), outp++);
   output_pins[3] = Window5.RegisterIOpin (lxT ("P3"), outp++);
   output_pins[4] = Window5.RegisterIOpin (lxT ("P4"), outp++);
   output_pins[5] = Window5.RegisterIOpin (lxT ("P5"), outp++);
   output_pins[6] = Window5.RegisterIOpin (lxT ("P6"), outp++);
   output_pins[7] = Window5.RegisterIOpin (lxT ("P7"), outp++);
   output_pins[8] = Window5.RegisterIOpin (lxT ("/INT"), outp++);
  }

 Reset ();
}

void
cpart_IO_PCF8574::ConfigurePropertiesWindow(CPWindow * WProp)
{
 lxString Items = Window5.GetPinsNames ();
 lxString spin;

 for (int i = 0; i < 16; i++)
  {
   lxString value = "";

   int pinv = pin_values[i][0];
   if (pinv > 13)
    {
     value = lxT ("          ") + lxString (pin_values[i]);
    }
   else if (pinv >= 5)
    {
     if (output_pins[pinv - 5] == 0)
      value = "          NC";
     else
      value = lxT ("          ") + itoa (output_pins[pinv - 5]); //+ lxT (" ") + Window5.GetPinName (output_pins[pinv - 5]);
    }


   ((CLabel*) WProp->GetChildByName ("label" + itoa (i + 1)))->SetText (itoa (i + 1) + lxT ("-") + pin_names[i] + value);
  }

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



 ((CButton*) WProp->GetChildByName ("button1"))->EvMouseButtonRelease = EVMOUSEBUTTONRELEASE & CPWindow5::PropButtonRelease;
 ((CButton*) WProp->GetChildByName ("button1"))->SetTag (1);

 ((CButton*) WProp->GetChildByName ("button2"))->EvMouseButtonRelease = EVMOUSEBUTTONRELEASE & CPWindow5::PropButtonRelease;
}

void
cpart_IO_PCF8574::ReadPropertiesWindow(CPWindow * WProp)
{
 input_pins[0] = atoi (((CCombo*) WProp->GetChildByName ("combo1"))->GetText ());
 input_pins[1] = atoi (((CCombo*) WProp->GetChildByName ("combo2"))->GetText ());
 input_pins[2] = atoi (((CCombo*) WProp->GetChildByName ("combo3"))->GetText ());
 input_pins[3] = atoi (((CCombo*) WProp->GetChildByName ("combo4"))->GetText ());
 input_pins[4] = atoi (((CCombo*) WProp->GetChildByName ("combo5"))->GetText ());
}

void
cpart_IO_PCF8574::PreProcess(void)
{
 const picpin * ppins = Window5.GetPinsValues ();
 unsigned char addr = 0x27;

 memset (output_pins_alm, 0, 9 * sizeof (unsigned long));

 JUMPSTEPS_ = Window1.GetJUMPSTEPS ();
 mcount = JUMPSTEPS_;



 if (input_pins[2])
  {
   if (!ppins[input_pins[2] - 1].value)addr &= ~0x01;
  }
 if (input_pins[3])
  {
   if (!ppins[input_pins[3] - 1].value)addr &= ~0x02;
  }
 if (input_pins[4])
  {
   if (!ppins[input_pins[4] - 1].value)addr &= ~0x04;
  }

 io_PCF8574_set_addr (&ioe8, addr);

}

void
cpart_IO_PCF8574::Process(void)
{
 const picpin * ppins = Window5.GetPinsValues ();

 if ((input_pins[0] > 0)&&(input_pins[1] > 0))
  Window5.Set_i2c_bus (input_pins[1] - 1, io_PCF8574_I2C_io (&ioe8, ppins[input_pins[0] - 1].value, ppins[input_pins[1] - 1].value));
 if (input_pins[1] > 0)
  Window5.SetPin (input_pins[1], Window5.Get_i2c_bus (input_pins[1] - 1));


 if (_ret != ioe8.data)
  {
   Window5.WritePin (output_pins[0], (ioe8.data & 0x01) != 0);
   Window5.WritePin (output_pins[1], (ioe8.data & 0x02) != 0);
   Window5.WritePin (output_pins[2], (ioe8.data & 0x04) != 0);
   Window5.WritePin (output_pins[3], (ioe8.data & 0x08) != 0);
   Window5.WritePin (output_pins[4], (ioe8.data & 0x10) != 0);
   Window5.WritePin (output_pins[5], (ioe8.data & 0x20) != 0);
   Window5.WritePin (output_pins[6], (ioe8.data & 0x40) != 0);
   Window5.WritePin (output_pins[7], (ioe8.data & 0x80) != 0);
   //Window5.WritePin (output_pins[8], (ioe8.data & 0x100) != 0);
  }
 _ret = ioe8.data;


 mcount++;
 if (mcount >= JUMPSTEPS_)
  {
   if (ppins[output_pins[0] - 1].value)output_pins_alm[0]++;
   if (ppins[output_pins[1] - 1].value)output_pins_alm[1]++;
   if (ppins[output_pins[2] - 1].value)output_pins_alm[2]++;
   if (ppins[output_pins[3] - 1].value)output_pins_alm[3]++;
   if (ppins[output_pins[4] - 1].value)output_pins_alm[4]++;
   if (ppins[output_pins[5] - 1].value)output_pins_alm[5]++;
   if (ppins[output_pins[6] - 1].value)output_pins_alm[6]++;
   if (ppins[output_pins[7] - 1].value)output_pins_alm[7]++;
   if (ppins[output_pins[8] - 1].value)output_pins_alm[8]++;

   mcount = -1;
  }


}

void
cpart_IO_PCF8574::PostProcess(void)
{
 long int NSTEPJ = Window1.GetNSTEPJ ();
 const picpin * ppins = Window5.GetPinsValues ();

 Window5.WritePinOA (output_pins[0], (ppins[output_pins[0] - 1].oavalue + ((output_pins_alm[0]*200.0) / NSTEPJ) + 55) / 2);
 Window5.WritePinOA (output_pins[1], (ppins[output_pins[1] - 1].oavalue + ((output_pins_alm[1]*200.0) / NSTEPJ) + 55) / 2);
 Window5.WritePinOA (output_pins[2], (ppins[output_pins[2] - 1].oavalue + ((output_pins_alm[2]*200.0) / NSTEPJ) + 55) / 2);
 Window5.WritePinOA (output_pins[3], (ppins[output_pins[3] - 1].oavalue + ((output_pins_alm[3]*200.0) / NSTEPJ) + 55) / 2);
 Window5.WritePinOA (output_pins[4], (ppins[output_pins[4] - 1].oavalue + ((output_pins_alm[4]*200.0) / NSTEPJ) + 55) / 2);
 Window5.WritePinOA (output_pins[5], (ppins[output_pins[5] - 1].oavalue + ((output_pins_alm[5]*200.0) / NSTEPJ) + 55) / 2);
 Window5.WritePinOA (output_pins[6], (ppins[output_pins[6] - 1].oavalue + ((output_pins_alm[6]*200.0) / NSTEPJ) + 55) / 2);
 Window5.WritePinOA (output_pins[7], (ppins[output_pins[7] - 1].oavalue + ((output_pins_alm[7]*200.0) / NSTEPJ) + 55) / 2);
 Window5.WritePinOA (output_pins[8], (ppins[output_pins[8] - 1].oavalue + ((output_pins_alm[8]*200.0) / NSTEPJ) + 55) / 2);

}

part_init("IO PCF8574", cpart_IO_PCF8574, "Other");

