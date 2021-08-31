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

cpart_Jumpers::cpart_Jumpers(unsigned x, unsigned y) :
font(10, lxFONTFAMILY_TELETYPE, lxFONTSTYLE_NORMAL, lxFONTWEIGHT_BOLD)
{
 char buff[2];

 ID = countID++;

 X = x;
 Y = y;
 aways_update = 1;
 ReadMaps ();
 Bitmap = NULL;

 lxImage image (&Window5);

 image.LoadFile (lxGetLocalFile(Window1.GetSharePath () + lxT ("parts/") + GetPictureFileName ()), Orientation, Scale, Scale);


 Bitmap = new lxBitmap (&image, &Window5);
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

 type = JWT_MM;

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
      case O_L1 ... O_L16:
       c = ppins[output_pins[output[i].id - O_L1] - 1].oavalue;
       canvas.SetColor (c, c, 0);
       canvas.Rectangle (1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
       break;
      case O_I1 ... O_I16:
       canvas.SetColor (49, 61, 99);
       canvas.Rectangle (1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
       pname.Printf ("%2i-%s", output[i].id - O_I1 + 1, Window5.GetPinName (input_pins[output[i].id - O_I1]).c_str ());
       canvas.SetFgColor (255, 255, 255);
       canvas.RotatedText (pname, output[i].x1, output[i].y2, 90.0);
       break;
      case O_O1 ... O_O16:
       canvas.SetColor (49, 61, 99);
       canvas.Rectangle (1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
       pname.Printf ("%2i-%s", output[i].id - O_O1 + 1, Window5.GetPinName (output_pins[output[i].id - O_O1]).c_str ());
       canvas.SetFgColor (255, 255, 255);
       canvas.RotatedText (pname, output[i].x1, output[i].y2, 90.0);
       break;
      default:
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
cpart_Jumpers::get_in_id(char * name)
{
 printf ("Erro input '%s' don't have a valid id! \n", name);
 return -1;
};

unsigned short
cpart_Jumpers::get_out_id(char * name)
{

 if (strcmp (name, "PN_I1") == 0)return O_I1;
 if (strcmp (name, "PN_I2") == 0)return O_I2;
 if (strcmp (name, "PN_I3") == 0)return O_I3;
 if (strcmp (name, "PN_I4") == 0)return O_I4;
 if (strcmp (name, "PN_I5") == 0)return O_I5;
 if (strcmp (name, "PN_I6") == 0)return O_I6;
 if (strcmp (name, "PN_I7") == 0)return O_I7;
 if (strcmp (name, "PN_I8") == 0)return O_I8;
 if (strcmp (name, "PN_I9") == 0)return O_I9;
 if (strcmp (name, "PN_I10") == 0)return O_I10;
 if (strcmp (name, "PN_I11") == 0)return O_I11;
 if (strcmp (name, "PN_I12") == 0)return O_I12;
 if (strcmp (name, "PN_I13") == 0)return O_I13;
 if (strcmp (name, "PN_I14") == 0)return O_I14;
 if (strcmp (name, "PN_I15") == 0)return O_I15;
 if (strcmp (name, "PN_I16") == 0)return O_I16;

 if (strcmp (name, "PN_O1") == 0)return O_O1;
 if (strcmp (name, "PN_O2") == 0)return O_O2;
 if (strcmp (name, "PN_O3") == 0)return O_O3;
 if (strcmp (name, "PN_O4") == 0)return O_O4;
 if (strcmp (name, "PN_O5") == 0)return O_O5;
 if (strcmp (name, "PN_O6") == 0)return O_O6;
 if (strcmp (name, "PN_O7") == 0)return O_O7;
 if (strcmp (name, "PN_O8") == 0)return O_O8;
 if (strcmp (name, "PN_O9") == 0)return O_O9;
 if (strcmp (name, "PN_O10") == 0)return O_O10;
 if (strcmp (name, "PN_O11") == 0)return O_O11;
 if (strcmp (name, "PN_O12") == 0)return O_O12;
 if (strcmp (name, "PN_O13") == 0)return O_O13;
 if (strcmp (name, "PN_O14") == 0)return O_O14;
 if (strcmp (name, "PN_O15") == 0)return O_O15;
 if (strcmp (name, "PN_O16") == 0)return O_O16;

 if (strcmp (name, "LD_1") == 0)return O_L1;
 if (strcmp (name, "LD_2") == 0)return O_L2;
 if (strcmp (name, "LD_3") == 0)return O_L3;
 if (strcmp (name, "LD_4") == 0)return O_L4;
 if (strcmp (name, "LD_5") == 0)return O_L5;
 if (strcmp (name, "LD_6") == 0)return O_L6;
 if (strcmp (name, "LD_7") == 0)return O_L7;
 if (strcmp (name, "LD_8") == 0)return O_L8;
 if (strcmp (name, "LD_9") == 0)return O_L9;
 if (strcmp (name, "LD_10") == 0)return O_L10;
 if (strcmp (name, "LD_11") == 0)return O_L11;
 if (strcmp (name, "LD_12") == 0)return O_L12;
 if (strcmp (name, "LD_13") == 0)return O_L13;
 if (strcmp (name, "LD_14") == 0)return O_L14;
 if (strcmp (name, "LD_15") == 0)return O_L15;
 if (strcmp (name, "LD_16") == 0)return O_L16;

 printf ("Erro output '%s' don't have a valid id! \n", name);
 return 1;
};

lxString
cpart_Jumpers::WritePreferences(void)
{
 char prefs[256];

 char c = jname[0];
 sprintf (prefs, "%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%c",
          input_pins[0], input_pins[1], input_pins[2], input_pins[3], input_pins[4], input_pins[5], input_pins[6], input_pins[7],
          input_pins[8], input_pins[9], input_pins[10], input_pins[11], input_pins[12], input_pins[13], input_pins[14], input_pins[15],
          output_pins[0], output_pins[1], output_pins[2], output_pins[3], output_pins[4], output_pins[5], output_pins[6], output_pins[7],
          output_pins[8], output_pins[9], output_pins[10], output_pins[11], output_pins[12], output_pins[13], output_pins[14], output_pins[15],
          type, c);

 return prefs;
}

void
cpart_Jumpers::ReadPreferences(lxString value)
{
 unsigned char inp[16];
 unsigned char outp[16];

 char c;
 unsigned char ntype;

 sscanf (value.c_str (), "%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%hhu,%c",
         &inp[0], &inp[1], &inp[2], &inp[3], &inp[4], &inp[5], &inp[6], &inp[7],
         &inp[8], &inp[9], &inp[10], &inp[11], &inp[12], &inp[13], &inp[14], &inp[15],
         &outp[0], &outp[1], &outp[2], &outp[3], &outp[4], &outp[5], &outp[6], &outp[7],
         &outp[8], &outp[9], &outp[10], &outp[11], &outp[12], &outp[13], &outp[14], &outp[15],
         &ntype, &c);

 ChangeType (ntype);

 if ((ntype & 0x02))
  {
   for (int i = 0; i < 16; i++)
    {
     input_pins[i] = inp[i];
    }
  }
 else
  {
   if ((input_pins[0] != inp[0]) || (ID != (c - 'A')))
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
      }

     input_pins[0] = Window5.RegisterIOpin (jname + lxT ("I0"), inp[0], PD_IN);
     input_pins[1] = Window5.RegisterIOpin (jname + lxT ("I1"), inp[1], PD_IN);
     input_pins[2] = Window5.RegisterIOpin (jname + lxT ("I2"), inp[2], PD_IN);
     input_pins[3] = Window5.RegisterIOpin (jname + lxT ("I3"), inp[3], PD_IN);
     input_pins[4] = Window5.RegisterIOpin (jname + lxT ("I4"), inp[4], PD_IN);
     input_pins[5] = Window5.RegisterIOpin (jname + lxT ("I5"), inp[5], PD_IN);
     input_pins[6] = Window5.RegisterIOpin (jname + lxT ("I6"), inp[6], PD_IN);
     input_pins[7] = Window5.RegisterIOpin (jname + lxT ("I7"), inp[7], PD_IN);
     input_pins[8] = Window5.RegisterIOpin (jname + lxT ("I8"), inp[8], PD_IN);
     input_pins[9] = Window5.RegisterIOpin (jname + lxT ("I9"), inp[9], PD_IN);
     input_pins[10] = Window5.RegisterIOpin (jname + lxT ("IA"), inp[10], PD_IN);
     input_pins[11] = Window5.RegisterIOpin (jname + lxT ("IB"), inp[11], PD_IN);
     input_pins[12] = Window5.RegisterIOpin (jname + lxT ("IC"), inp[12], PD_IN);
     input_pins[13] = Window5.RegisterIOpin (jname + lxT ("ID"), inp[13], PD_IN);
     input_pins[14] = Window5.RegisterIOpin (jname + lxT ("IE"), inp[14], PD_IN);
     input_pins[15] = Window5.RegisterIOpin (jname + lxT ("IF"), inp[15], PD_IN);
    }
  }

 if (ntype & 0x01)
  {
   for (int i = 0; i < 16; i++)
    {
     output_pins[i] = outp[i];
    }
  }
 else
  {
   if ((output_pins[0] != outp[0]) || (ID != (c - 'A')))
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
       Window5.UnregisterIOpin (output_pins[i]);
      }

     output_pins[0] = Window5.RegisterIOpin (jname + lxT ("O0"), outp[0]);
     output_pins[1] = Window5.RegisterIOpin (jname + lxT ("O1"), outp[1]);
     output_pins[2] = Window5.RegisterIOpin (jname + lxT ("O2"), outp[2]);
     output_pins[3] = Window5.RegisterIOpin (jname + lxT ("O3"), outp[3]);
     output_pins[4] = Window5.RegisterIOpin (jname + lxT ("O4"), outp[4]);
     output_pins[5] = Window5.RegisterIOpin (jname + lxT ("O5"), outp[5]);
     output_pins[6] = Window5.RegisterIOpin (jname + lxT ("O6"), outp[6]);
     output_pins[7] = Window5.RegisterIOpin (jname + lxT ("O7"), outp[7]);
     output_pins[8] = Window5.RegisterIOpin (jname + lxT ("O8"), outp[8]);
     output_pins[9] = Window5.RegisterIOpin (jname + lxT ("O9"), outp[9]);
     output_pins[10] = Window5.RegisterIOpin (jname + lxT ("OA"), outp[10]);
     output_pins[11] = Window5.RegisterIOpin (jname + lxT ("OB"), outp[11]);
     output_pins[12] = Window5.RegisterIOpin (jname + lxT ("OC"), outp[12]);
     output_pins[13] = Window5.RegisterIOpin (jname + lxT ("OD"), outp[13]);
     output_pins[14] = Window5.RegisterIOpin (jname + lxT ("OE"), outp[14]);
     output_pins[15] = Window5.RegisterIOpin (jname + lxT ("OF"), outp[15]);
    }
  }

 Reset ();
 RegisterRemoteControl ();
}

void
cpart_Jumpers::RegisterRemoteControl(void)
{
 const picpin * ppins = Window5.GetPinsValues ();
 for (int i = 0; i < outputc; i++)
  {
   switch (output[i].id)
    {
    case O_L1:
     if (input_pins[0])
      {
       output[i].status = (void *) &ppins[input_pins[0] - 1].oavalue;
      }
     break;
    case O_L2:
     if (input_pins[1])
      {
       output[i].status = (void *) &ppins[input_pins[1] - 1].oavalue;
      }
     break;
    case O_L3:
     if (input_pins[2])
      {
       output[i].status = (void *) &ppins[input_pins[2] - 1].oavalue;
      }
     break;
    case O_L4:
     if (input_pins[3])
      {
       output[i].status = (void *) &ppins[input_pins[3] - 1].oavalue;
      }
     break;
    case O_L5:
     if (input_pins[4])
      {
       output[i].status = (void *) &ppins[input_pins[4] - 1].oavalue;
      }
     break;
    case O_L6:
     if (input_pins[5])
      {
       output[i].status = (void *) &ppins[input_pins[5] - 1].oavalue;
      }
     break;
    case O_L7:
     if (input_pins[6])
      {
       output[i].status = (void *) &ppins[input_pins[6] - 1].oavalue;
      }
     break;
    case O_L8:
     if (input_pins[7])
      {
       output[i].status = (void *) &ppins[input_pins[7] - 1].oavalue;
      }
     break;
    }
  }
}

void
cpart_Jumpers::ConfigurePropertiesWindow(CPWindow * WProp)
{
 lxString Items = Window5.GetPinsNames ();
 lxString spin;
 lxString childname;
 CCombo* cc;


 cc = ((CCombo*) WProp->GetChildByName ("combo1"));
 if (type & 0x02)
  cc->SetText ("F");
 else
  cc->SetText ("M");

 cc = ((CCombo*) WProp->GetChildByName ("combo18"));
 if (type & 0x01)
  cc->SetText ("F");
 else
  cc->SetText ("M");


 for (int i = 0; i < 16; i++)
  {
   //input
   childname.Printf ("combo%i", i + 2);
   cc = (CCombo*) WProp->GetChildByName (childname);

   cc->SetItems (Items);
   if (input_pins[i] == 0)
    cc->SetText ("0  NC");
   else
    {
     spin = Window5.GetPinName (input_pins[i]);
     cc->SetText (itoa (input_pins[i]) + "  " + spin);
    }

   if (type & 0x02)
    {
     cc->SetEnable (1);
    }
   else
    {
     cc->SetEnable (0);
    }

   //output
   childname.Printf ("combo%i", i + 19);
   cc = (CCombo*) WProp->GetChildByName (childname);

   cc->SetItems (Items);
   if (output_pins[i] == 0)
    cc->SetText ("0  NC");
   else
    {
     spin = Window5.GetPinName (output_pins[i]);
     cc->SetText (itoa (output_pins[i]) + "  " + spin);
    }

   if (type & 0x01)
    {
     cc->SetEnable (1);
    }
   else
    {

     cc->SetEnable (0);
    }

  }



 ((CButton*) WProp->GetChildByName ("button1"))->EvMouseButtonRelease = EVMOUSEBUTTONRELEASE & CPWindow5::PropButtonRelease;
 ((CButton*) WProp->GetChildByName ("button1"))->SetTag (1);

 ((CButton*) WProp->GetChildByName ("button2"))->EvMouseButtonRelease = EVMOUSEBUTTONRELEASE & CPWindow5::PropButtonRelease;

 ((CCombo*) WProp->GetChildByName ("combo1"))->EvOnComboChange = EVONCOMBOCHANGE & CPWindow5::PropComboChange;
 ((CCombo*) WProp->GetChildByName ("combo18"))->EvOnComboChange = EVONCOMBOCHANGE & CPWindow5::PropComboChange;

}

void
cpart_Jumpers::ReadPropertiesWindow(CPWindow * WProp)
{

 lxString childname;
 CCombo* cc;


 cc = ((CCombo*) WProp->GetChildByName ("combo1"));
 if (cc->GetText ().Cmp ("M"))
  type |= 0x02;
 else
  type &= ~0x02;


 cc = ((CCombo*) WProp->GetChildByName ("combo18"));
 if (cc->GetText ().Cmp ("M"))
  type |= 0x01;
 else
  type &= ~0x01;


 for (int i = 0; i < 16; i++)
  {

   //input
   if (type & 0x02)
    {
     childname.Printf ("combo%i", i + 2);
     cc = (CCombo*) WProp->GetChildByName (childname);
     input_pins[i] = atoi (cc->GetText ());
    }
   //output
   if (type & 0x01)
    {
     childname.Printf ("combo%i", i + 19);
     cc = (CCombo*) WProp->GetChildByName (childname);
     output_pins[i] = atoi (cc->GetText ());
    }
  }

 RegisterRemoteControl ();
}

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
     if (type & 0x01)
      {
       Window5.SetPin (output_pins[i], ppins[input_pins[i] - 1].value);
      }
     else
      {
       Window5.WritePin (output_pins[i], ppins[input_pins[i] - 1].value);
      }
    }
   /*
   if (ppins[input_pins[i] - 1].avalue != ppins[output_pins[i] - 1].avalue)
   {
Window5.WritePinA (output_pins[i], ppins[input_pins[i] - 1].avalue);
   }
    */
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
   Window5.WritePinOA (output_pins[i], (ppins[output_pins[i] - 1].oavalue + ((output_pins_alm[i]*200.0) / NSTEPJ) + 55) / 2);

   if (output_pins[i] && (output_ids[O_L1 + i]->value != ppins[output_pins[i] - 1].oavalue))
    {
     output_ids[O_L1 + i]->value = ppins[output_pins[i] - 1].oavalue;
     output_ids[O_L1 + i]->update = 1;
    }
  }
}

void
cpart_Jumpers::ChangeType(unsigned char ntype)
{


 //if same

 if (type == ntype) return;

 if (!(type & 0x02) && (ntype & 0x02))
  {
   for (int i = 0; i < 16; i++)
    {
     Window5.UnregisterIOpin (input_pins[i]);
     input_pins[i] = 0;
    }
  }
 if ((type & 0x02) && !(ntype & 0x02))
  {
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
  }

 if (!(type & 0x01) && (ntype & 0x01))
  {
   for (int i = 0; i < 16; i++)
    {
     Window5.UnregisterIOpin (output_pins[i]);
     output_pins[i] = 0;
    }
  }

 if ((type & 0x01) && !(ntype & 0x01))
  {
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
  }

 type = ntype;

}

void
cpart_Jumpers::ComboChange(CCombo * control, lxString value)
{

 unsigned char ntype = type;

 if (!control->GetName ().Cmp ("combo1"))
  {
   if (!value.compare (lxT ("F")))
    {
     ntype |= 0x02;
    }
   else
    {
     ntype &= ~0x02;
    }

  }
 else
  {
   if (!value.compare (lxT ("F")))
    {
     ntype |= 0x01;
    }
   else
    {
     ntype &= ~0x01;
    }
  }
 ChangeType (ntype);
}

part_init(PART_JUMPERS_Name, cpart_Jumpers, "Other");

