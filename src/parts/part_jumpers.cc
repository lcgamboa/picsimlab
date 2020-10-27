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
#include"part_jumpers.h"
#include "part_7s_Display.h"

//cpart_Jumpers::countID=0;

/* outputs */
enum
{
 O_I1, O_I2, O_I3, O_I4, O_I5, O_I6, O_I7, O_I8, O_I9, O_I10, O_I11, O_I12, O_I13, O_I14, O_I15, O_I16,
 O_O1, O_O2, O_O3, O_O4, O_O5, O_O6, O_O7, O_O8, O_O9, O_O10, O_O11, O_O12, O_O13, O_O14, O_O15, O_O16,
 O_L1, O_L2, O_L3, O_L4, O_L5, O_L6, O_L7, O_L8, O_L9, O_L10, O_L11, O_L12, O_L13, O_L14, O_L15, O_L16
};

int cpart_Jumpers::countID = 0;

cpart_Jumpers::cpart_Jumpers(unsigned x, unsigned y)
{
 char buff[2];

 ID = countID++;

 X = x;
 Y = y;
 ReadMaps ();
 Bitmap = NULL;

 lxImage image;

 image.LoadFile (Window1.GetSharePath () + lxT ("parts/") + GetPictureFileName ());


 Bitmap = new lxBitmap (image, &Window5);
 image.Destroy ();
 canvas.Create (Window5.GetWWidget (), Bitmap);

 buff[0] = 'A' + ID;
 buff[1] = 0;

 jname = buff;

 input_pins[0] = Window5.RegisterIOpin (jname + lxT ("I0"), 0, PD_IN);
 input_pins[1] = Window5.RegisterIOpin (jname + lxT ("I1"), 0, PD_IN);
 input_pins[2] = Window5.RegisterIOpin (jname + lxT ("I2"), 0, PD_IN);
 input_pins[3] = Window5.RegisterIOpin (jname + lxT ("I3"), 0, PD_IN);
 input_pins[4] = Window5.RegisterIOpin (jname + lxT ("I4"), 0, PD_IN);
 input_pins[5] = Window5.RegisterIOpin (jname + lxT ("I5"), 0, PD_IN);
 input_pins[6] = Window5.RegisterIOpin (jname + lxT ("I6"), 0, PD_IN);
 input_pins[7] = Window5.RegisterIOpin (jname + lxT ("I7"), 0, PD_IN);
 input_pins[8] = Window5.RegisterIOpin (jname + lxT ("I8"), 0, PD_IN);
 input_pins[9] = Window5.RegisterIOpin (jname + lxT ("I9"), 0, PD_IN);
 input_pins[10] = Window5.RegisterIOpin (jname + lxT ("IA"), 0, PD_IN);
 input_pins[11] = Window5.RegisterIOpin (jname + lxT ("IB"), 0, PD_IN);
 input_pins[12] = Window5.RegisterIOpin (jname + lxT ("IC"), 0, PD_IN);
 input_pins[13] = Window5.RegisterIOpin (jname + lxT ("ID"), 0, PD_IN);
 input_pins[14] = Window5.RegisterIOpin (jname + lxT ("IE"), 0, PD_IN);
 input_pins[15] = Window5.RegisterIOpin (jname + lxT ("IF"), 0, PD_IN);

 output_pins[0] = Window5.RegisterIOpin (jname + lxT ("O0"));
 output_pins[1] = Window5.RegisterIOpin (jname + lxT ("O1"));
 output_pins[2] = Window5.RegisterIOpin (jname + lxT ("O2"));
 output_pins[3] = Window5.RegisterIOpin (jname + lxT ("O3"));
 output_pins[4] = Window5.RegisterIOpin (jname + lxT ("O4"));
 output_pins[5] = Window5.RegisterIOpin (jname + lxT ("O5"));
 output_pins[6] = Window5.RegisterIOpin (jname + lxT ("O6"));
 output_pins[7] = Window5.RegisterIOpin (jname + lxT ("O7"));
 output_pins[8] = Window5.RegisterIOpin (jname + lxT ("O8"));
 output_pins[9] = Window5.RegisterIOpin (jname + lxT ("O9"));
 output_pins[10] = Window5.RegisterIOpin (jname + lxT ("OA"));
 output_pins[11] = Window5.RegisterIOpin (jname + lxT ("OB"));
 output_pins[12] = Window5.RegisterIOpin (jname + lxT ("OC"));
 output_pins[13] = Window5.RegisterIOpin (jname + lxT ("OD"));
 output_pins[14] = Window5.RegisterIOpin (jname + lxT ("OE"));
 output_pins[15] = Window5.RegisterIOpin (jname + lxT ("OF"));


 mcount = 0;
 memset (output_pins_alm, 0, 16 * sizeof (unsigned long));

}

cpart_Jumpers::~cpart_Jumpers(void)
{
 for (int i = 0; i < 16; i++)
  {
   Window5.UnregisterIOpin (input_pins[i]);
   Window5.UnregisterIOpin (output_pins[i]);
  }
 delete Bitmap;
 canvas.Destroy ();
}

void
cpart_Jumpers::Draw(void)
{

 int i;
 const picpin * ppins = Window5.GetPinsValues ();
 lxString pname;
 unsigned char c;

 canvas.Init ();

 lxFont font (10, lxFONTFAMILY_TELETYPE, lxFONTSTYLE_NORMAL, lxFONTWEIGHT_BOLD);
 canvas.SetFont (font);

 for (i = 0; i < outputc; i++)
  {

   switch (output[i].id)
    {
    case O_L1 ... O_L16:
     c = ppins[output_pins[output[i].id - O_L1] - 1].oavalue;
     canvas.SetColor (c, c, 0);
     canvas.Rectangle (1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
     break;
    case O_I1 ... O_I16:
     canvas.SetColor (49, 61, 99);
     canvas.Rectangle (1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
     pname.Printf ("%2i-%s", output[i].id - O_I1 + 1, Window5.GetPinName (input_pins[output[i].id - O_I1]));
     canvas.SetFgColor (255, 255, 255);
     canvas.RotatedText (pname, output[i].x1, output[i].y2, 90.0);
     break;
    case O_O1 ... O_O16:
     canvas.SetColor (49, 61, 99);
     canvas.Rectangle (1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
     pname.Printf ("%2i-%s", output[i].id - O_O1 + 1, Window5.GetPinName (output_pins[output[i].id - O_O1]));
     canvas.SetFgColor (255, 255, 255);
     canvas.RotatedText (pname, output[i].x1, output[i].y2, 90.0);
     break;
    default:
     break;
    }
  }
 canvas.End ();

}

unsigned short
cpart_Jumpers::get_in_id(char * name)
{
 printf ("Erro input '%s' don't have a valid id! \n", name);
 return -1;
};

unsigned short
cpart_Jumpers::get_out_id(char * name)
{

 if (strcmp (name, "I1") == 0)return O_I1;
 if (strcmp (name, "I2") == 0)return O_I2;
 if (strcmp (name, "I3") == 0)return O_I3;
 if (strcmp (name, "I4") == 0)return O_I4;
 if (strcmp (name, "I5") == 0)return O_I5;
 if (strcmp (name, "I6") == 0)return O_I6;
 if (strcmp (name, "I7") == 0)return O_I7;
 if (strcmp (name, "I8") == 0)return O_I8;
 if (strcmp (name, "I9") == 0)return O_I9;
 if (strcmp (name, "I10") == 0)return O_I10;
 if (strcmp (name, "I11") == 0)return O_I11;
 if (strcmp (name, "I12") == 0)return O_I12;
 if (strcmp (name, "I13") == 0)return O_I13;
 if (strcmp (name, "I14") == 0)return O_I14;
 if (strcmp (name, "I15") == 0)return O_I15;
 if (strcmp (name, "I16") == 0)return O_I16;

 if (strcmp (name, "O1") == 0)return O_O1;
 if (strcmp (name, "O2") == 0)return O_O2;
 if (strcmp (name, "O3") == 0)return O_O3;
 if (strcmp (name, "O4") == 0)return O_O4;
 if (strcmp (name, "O5") == 0)return O_O5;
 if (strcmp (name, "O6") == 0)return O_O6;
 if (strcmp (name, "O7") == 0)return O_O7;
 if (strcmp (name, "O8") == 0)return O_O8;
 if (strcmp (name, "O9") == 0)return O_O9;
 if (strcmp (name, "O10") == 0)return O_O10;
 if (strcmp (name, "O11") == 0)return O_O11;
 if (strcmp (name, "O12") == 0)return O_O12;
 if (strcmp (name, "O13") == 0)return O_O13;
 if (strcmp (name, "O14") == 0)return O_O14;
 if (strcmp (name, "O15") == 0)return O_O15;
 if (strcmp (name, "O16") == 0)return O_O16;

 if (strcmp (name, "L1") == 0)return O_L1;
 if (strcmp (name, "L2") == 0)return O_L2;
 if (strcmp (name, "L3") == 0)return O_L3;
 if (strcmp (name, "L4") == 0)return O_L4;
 if (strcmp (name, "L5") == 0)return O_L5;
 if (strcmp (name, "L6") == 0)return O_L6;
 if (strcmp (name, "L7") == 0)return O_L7;
 if (strcmp (name, "L8") == 0)return O_L8;
 if (strcmp (name, "L9") == 0)return O_L9;
 if (strcmp (name, "L10") == 0)return O_L10;
 if (strcmp (name, "L11") == 0)return O_L11;
 if (strcmp (name, "L12") == 0)return O_L12;
 if (strcmp (name, "L13") == 0)return O_L13;
 if (strcmp (name, "L14") == 0)return O_L14;
 if (strcmp (name, "L15") == 0)return O_L15;
 if (strcmp (name, "L16") == 0)return O_L16;

 printf ("Erro output '%s' don't have a valid id! \n", name);
 return 1;
};

lxString
cpart_Jumpers::WritePreferences(void)
{
 char prefs[256];

 char c = jname[0];
 sprintf (prefs, "%hhu,%c", input_pins[0], c);

 return prefs;
}

void
cpart_Jumpers::ReadPreferences(lxString value)
{
 unsigned char inp;
 char c;

 sscanf (value.c_str (), "%hhu,%c", &inp, &c);

 if ((input_pins[0] != inp)||(   ID != (c-'A')))
  {
   char buff[2];

   buff[0] = c;
   buff[1] = 0;

   jname = buff;

   ID = c - 'A';
   if (ID >= countID)
    {
     countID = ID + 1;
    }

   for (int i = 0; i < 16; i++)
    {
     Window5.UnregisterIOpin (input_pins[i]);
     Window5.UnregisterIOpin (output_pins[i]);
    }

   input_pins[0] = Window5.RegisterIOpin (jname + lxT ("I0"), inp++, PD_IN);
   input_pins[1] = Window5.RegisterIOpin (jname + lxT ("I1"), inp++, PD_IN);
   input_pins[2] = Window5.RegisterIOpin (jname + lxT ("I2"), inp++, PD_IN);
   input_pins[3] = Window5.RegisterIOpin (jname + lxT ("I3"), inp++, PD_IN);
   input_pins[4] = Window5.RegisterIOpin (jname + lxT ("I4"), inp++, PD_IN);
   input_pins[5] = Window5.RegisterIOpin (jname + lxT ("I5"), inp++, PD_IN);
   input_pins[6] = Window5.RegisterIOpin (jname + lxT ("I6"), inp++, PD_IN);
   input_pins[7] = Window5.RegisterIOpin (jname + lxT ("I7"), inp++, PD_IN);
   input_pins[8] = Window5.RegisterIOpin (jname + lxT ("I8"), inp++, PD_IN);
   input_pins[9] = Window5.RegisterIOpin (jname + lxT ("I9"), inp++, PD_IN);
   input_pins[10] = Window5.RegisterIOpin (jname + lxT ("IA"), inp++, PD_IN);
   input_pins[11] = Window5.RegisterIOpin (jname + lxT ("IB"), inp++, PD_IN);
   input_pins[12] = Window5.RegisterIOpin (jname + lxT ("IC"), inp++, PD_IN);
   input_pins[13] = Window5.RegisterIOpin (jname + lxT ("ID"), inp++, PD_IN);
   input_pins[14] = Window5.RegisterIOpin (jname + lxT ("IE"), inp++, PD_IN);
   input_pins[15] = Window5.RegisterIOpin (jname + lxT ("IF"), inp++, PD_IN);

   output_pins[0] = Window5.RegisterIOpin (jname + lxT ("O0"), inp++);
   output_pins[1] = Window5.RegisterIOpin (jname + lxT ("O1"), inp++);
   output_pins[2] = Window5.RegisterIOpin (jname + lxT ("O2"), inp++);
   output_pins[3] = Window5.RegisterIOpin (jname + lxT ("O3"), inp++);
   output_pins[4] = Window5.RegisterIOpin (jname + lxT ("O4"), inp++);
   output_pins[5] = Window5.RegisterIOpin (jname + lxT ("O5"), inp++);
   output_pins[6] = Window5.RegisterIOpin (jname + lxT ("O6"), inp++);
   output_pins[7] = Window5.RegisterIOpin (jname + lxT ("O7"), inp++);
   output_pins[8] = Window5.RegisterIOpin (jname + lxT ("O8"), inp++);
   output_pins[9] = Window5.RegisterIOpin (jname + lxT ("O9"), inp++);
   output_pins[10] = Window5.RegisterIOpin (jname + lxT ("OA"), inp++);
   output_pins[11] = Window5.RegisterIOpin (jname + lxT ("OB"), inp++);
   output_pins[12] = Window5.RegisterIOpin (jname + lxT ("OC"), inp++);
   output_pins[13] = Window5.RegisterIOpin (jname + lxT ("OD"), inp++);
   output_pins[14] = Window5.RegisterIOpin (jname + lxT ("OE"), inp++);
   output_pins[15] = Window5.RegisterIOpin (jname + lxT ("OF"), inp++);
  }

 Reset ();
}

void
cpart_Jumpers::ConfigurePropertiesWindow(CPWindow * WProp)
{
 ((CButton*) WProp->GetChildByName ("button2"))->EvMouseButtonRelease = EVMOUSEBUTTONRELEASE & CPWindow5::PropButtonRelease;
}

void
cpart_Jumpers::ReadPropertiesWindow(CPWindow * WProp) { }

void
cpart_Jumpers::PreProcess(void)
{
 memset (output_pins_alm, 0, 16 * sizeof (unsigned long));
 JUMPSTEPS_ = Window1.GetJUMPSTEPS ();
 mcount = JUMPSTEPS_;
}

void
cpart_Jumpers::Process(void)
{
 int i;

 const picpin * ppins = Window5.GetPinsValues ();


 for (i = 0; i < 16; i++)
  {
   if (ppins[input_pins[i] - 1].value != ppins[output_pins[i] - 1].value)
    {
     Window5.WritePin (output_pins[i], ppins[input_pins[i] - 1].value);
    }
  }

 mcount++;
 if (mcount >= JUMPSTEPS_)
  {
   for (i = 0; i < 16; i++)
    {
     if (ppins[output_pins[i] - 1].value)output_pins_alm[i]++;
    }
   mcount = -1;
  }
}

void
cpart_Jumpers::PostProcess(void)
{
 long int NSTEPJ = Window1.GetNSTEPJ ();
 const picpin * ppins = Window5.GetPinsValues ();

 for (int i = 0; i < 16; i++)
  {
   Window5.WritePinOA (output_pins[i], (ppins[output_pins[i] - 1].oavalue + ((output_pins_alm[i]*255.0) / NSTEPJ)) / 2);
  }
}

part_init("Jumper Wires", cpart_Jumpers);

