/* ########################################################################

   PICsimLab - PIC laboratory simulator

   ########################################################################

   Copyright (c) : 2020-2021  Luis Claudio Gambôa Lopes

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
#include"part_ds1621.h"

/* outputs */
enum
{
 O_P1, O_P2, O_P3, O_P4, O_P5, O_P6, O_P7, O_P8, O_IC, O_PO1
};

/* inputs */
enum
{
 I_PO1
};


const char pin_names[8][10] = {"SDA", "SCL", "Tout", "GND", "A2", "A1", "A0", "VCC"};
const char pin_values[8][10] = {
 {0},
 {1},
 {2},
 "GND",
 {3},
 {4},
 {5},
 "+5V"
};

cpart_ds1621::cpart_ds1621(unsigned x, unsigned y) :
font(8, lxFONTFAMILY_TELETYPE, lxFONTSTYLE_NORMAL, lxFONTWEIGHT_BOLD),
font_p(6, lxFONTFAMILY_TELETYPE, lxFONTSTYLE_NORMAL, lxFONTWEIGHT_BOLD)
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

 sen_ds1621_init (&ds1621);
 sen_ds1621_rst (&ds1621);

 input_pins[0] = 0;
 input_pins[1] = 0;
 input_pins[2] = 0;
 input_pins[3] = 0;
 input_pins[4] = 0;
 input_pins[5] = 0;

 value = 0;
 active = 0;
}

cpart_ds1621::~cpart_ds1621(void)
{
 sen_ds1621_end (&ds1621);
 delete Bitmap;
 canvas.Destroy ();
}

void
cpart_ds1621::Draw(void)
{

 int i;
 char val[10];

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
       canvas.SetFont (font_p);
       canvas.SetColor (26, 26, 26);
       canvas.Rectangle (1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
       canvas.SetFgColor (255, 255, 255);
       canvas.RotatedText ("DS1621", output[i].x1, output[i].y2 - 15, 0.0);
       break;
      case O_PO1:
       snprintf (val, 10, "%5.1f", (0.9 * (200 - value) - 55));
       draw_slider (&output[i], value, val, font_p);
       canvas.SetFont (font);
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
cpart_ds1621::get_in_id(char * name)
{
 if (strcmp (name, "PO_1") == 0)return I_PO1;

 printf ("Erro input '%s' don't have a valid id! \n", name);
 return -1;
}

unsigned short
cpart_ds1621::get_out_id(char * name)
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

 if (strcmp (name, "PO_1") == 0)return O_PO1;

 printf ("Erro output '%s' don't have a valid id! \n", name);
 return 1;
}

lxString
cpart_ds1621::WritePreferences(void)
{
 char prefs[256];

 sprintf (prefs, "%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu", input_pins[0], input_pins[1], input_pins[2]
          , input_pins[3], input_pins[4], input_pins[5], value);

 return prefs;
}

void
cpart_ds1621::ReadPreferences(lxString value_)
{
 sscanf (value_.c_str (), "%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu", &input_pins[0], &input_pins[1], &input_pins[2]
         , &input_pins[3], &input_pins[4], &input_pins[5], &value);
 Reset ();
}

void
cpart_ds1621::ConfigurePropertiesWindow(CPWindow * WProp)
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

 ((CCombo*) WProp->GetChildByName ("combo7"))->SetItems (Items);
 if (input_pins[5] == 0)
  ((CCombo*) WProp->GetChildByName ("combo7"))->SetText ("0  NC");
 else
  {
   spin = Window5.GetPinName (input_pins[5]);
   ((CCombo*) WProp->GetChildByName ("combo7"))->SetText (itoa (input_pins[5]) + "  " + spin);
  }

 ((CButton*) WProp->GetChildByName ("button1"))->EvMouseButtonRelease = EVMOUSEBUTTONRELEASE & CPWindow5::PropButtonRelease;
 ((CButton*) WProp->GetChildByName ("button1"))->SetTag (1);

 ((CButton*) WProp->GetChildByName ("button2"))->EvMouseButtonRelease = EVMOUSEBUTTONRELEASE & CPWindow5::PropButtonRelease;
}

void
cpart_ds1621::ReadPropertiesWindow(CPWindow * WProp)
{
 input_pins[0] = atoi (((CCombo*) WProp->GetChildByName ("combo1"))->GetText ());
 input_pins[1] = atoi (((CCombo*) WProp->GetChildByName ("combo2"))->GetText ());
 input_pins[2] = atoi (((CCombo*) WProp->GetChildByName ("combo3"))->GetText ());
 input_pins[3] = atoi (((CCombo*) WProp->GetChildByName ("combo5"))->GetText ());
 input_pins[4] = atoi (((CCombo*) WProp->GetChildByName ("combo6"))->GetText ());
 input_pins[5] = atoi (((CCombo*) WProp->GetChildByName ("combo7"))->GetText ());
}

void
cpart_ds1621::PreProcess(void)
{
 const picpin * ppins = Window5.GetPinsValues ();
 unsigned char addr = 0x48;

 if (input_pins[5])
  {
   if (ppins[input_pins[5] - 1].value)addr |= 0x01;
  }
 if (input_pins[4])
  {
   if (ppins[input_pins[4] - 1].value)addr |= 0x02;
  }
 if (input_pins[3])
  {
   if (ppins[input_pins[3] - 1].value)addr |= 0x04;
  }

 sen_ds1621_set_addr (&ds1621, addr);

 if (input_pins[0] > 0)
  {
   sen_ds1621_setTemp (&ds1621, (0.9 * (200 - value) - 55));
   //TODO set addr
   Window5.Reset_i2c_bus (input_pins[0] - 1);
  }
}

void
cpart_ds1621::Process(void)
{
 const picpin * ppins = Window5.GetPinsValues ();

 if ((input_pins[0] > 0)&&(input_pins[1] > 0))
  Window5.Set_i2c_bus (input_pins[0] - 1, sen_ds1621_I2C_io (&ds1621, ppins[input_pins[1] - 1].value, ppins[input_pins[0] - 1].value));

 //TODO implement Tout output
}

void
cpart_ds1621::EvMouseButtonPress(uint button, uint x, uint y, uint state)
{

 int i;

 for (i = 0; i < inputc; i++)
  {
   if (PointInside (x, y, input[i]))
    {
     RotateCoords (&x, &y);
     switch (input[i].id)
      {
      case I_PO1:
       value = (y - input[i].y1)*1.66;
       if (value > 200)value = 200;
       active = 1;
       output_ids[O_PO1]->update = 1;
       break;
      }
    }
  }

}

void
cpart_ds1621::EvMouseButtonRelease(uint button, uint x, uint y, uint state)
{
 int i;

 for (i = 0; i < inputc; i++)
  {
   if (PointInside (x, y, input[i]))
    {
     switch (input[i].id)
      {
      case I_PO1:
       active = 0;
       output_ids[O_PO1]->update = 1;
       break;
      }
    }
  }
}

void
cpart_ds1621::EvMouseMove(uint button, uint x, uint y, uint state)
{

 int i;

 for (i = 0; i < inputc; i++)
  {
   if (PointInside (x, y, input[i]))
    {
     RotateCoords (&x, &y);

     if (active)
      {
       value = (y - input[i].y1)*1.66;
       if (value > 200)value = 200;
       output_ids[O_PO1]->update = 1;
      }
    }
   else
    {
     active = 0;
    }
  }
}

part_init(PART_DS1621_Name, cpart_ds1621, "Input");

