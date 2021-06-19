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
#include"board_McLab2.h"

#ifdef __EMSCRIPTEN__
#include<emscripten.h>
#endif

/* outputs */
enum
{
 O_SS1,
 O_SS2,
 O_SS3,
 O_SS4,
 O_POT1,
 O_RB0,
 O_RB1,
 O_RB2,
 O_RB3,
 O_LPWR,
 O_LCD,
 O_A1,
 O_B1,
 O_C1,
 O_D1,
 O_E1,
 O_F1,
 O_G1,
 O_P1,
 O_A2,
 O_B2,
 O_C2,
 O_D2,
 O_E2,
 O_F2,
 O_G2,
 O_P2,
 O_A3,
 O_B3,
 O_C3,
 O_D3,
 O_E3,
 O_F3,
 O_G3,
 O_P3,
 O_A4,
 O_B4,
 O_C4,
 O_D4,
 O_E4,
 O_F4,
 O_G4,
 O_P4,
 O_JP1,
 O_JP2,
 O_JP3,
 O_JP4,
 O_JP5,
 O_JP6,
 O_RST,
 O_BRB0,
 O_BRB1,
 O_BRB2,
 O_BRB3,
 O_VT,
 O_MP
};

/*inputs*/
enum
{
 I_POT1,
 I_RST,
 I_PWR,
 I_ICSP,
 I_JP1,
 I_JP2,
 I_JP3,
 I_JP4,
 I_JP5,
 I_JP6,
 I_RB0,
 I_RB1,
 I_RB2,
 I_RB3,
 I_VIEW
};

//TODO jumper support

cboard_McLab2::cboard_McLab2(void) :
font(10, lxFONTFAMILY_TELETYPE, lxFONTSTYLE_NORMAL, lxFONTWEIGHT_BOLD)
{
 char fname[1024];
 FILE * fout;

 Proc = "PIC18F452";

 vp2in = 2.5;
 vp2[0] = 2.5;
 vp2[1] = 2.5;
 temp[0] = 27.5;
 temp[1] = 27.5;
 ref = 0;

 rpmstp = 0; //(NSTEP*2)/100;
 rpmc = 0;

 vtc = 0;
 vt = 0;

 d = 0;
 lcde = 0;
 sound_on = 0;

 lcd_init (&lcd, 16, 2);
 mi2c_init (&mi2c, 4);

 ReadMaps ();

 jmp[0] = 0;
 jmp[1] = 0;
 jmp[2] = 0;
 jmp[3] = 0;
 jmp[4] = 0;
 jmp[5] = 0;

 buzzer.Init ();

 pot1 = 100;

 active = 0;

 lxImage image (&Window1);
 image.LoadFile (Window1.GetSharePath () + lxT ("boards/Common/VT1.svg"));
 vent[0] = new lxBitmap (&image, &Window1);
 image.LoadFile (Window1.GetSharePath () + lxT ("boards/Common/VT2.svg"));
 vent[1] = new lxBitmap (&image, &Window1);

 image.Destroy ();

 //gauge1
 gauge1 = new CGauge ();
 gauge1->SetFOwner (&Window1);
 gauge1->SetName (lxT ("gauge1_p3"));
 gauge1->SetX (13);
 gauge1->SetY (242);
 gauge1->SetWidth (140);
 gauge1->SetHeight (20);
 gauge1->SetEnable (1);
 gauge1->SetVisible (1);
 gauge1->SetRange (100);
 gauge1->SetValue (0);
 gauge1->SetType (4);
 Window1.CreateChild (gauge1);
 //gauge2
 gauge2 = new CGauge ();
 gauge2->SetFOwner (&Window1);
 gauge2->SetName (lxT ("gauge2_p3"));
 gauge2->SetX (12);
 gauge2->SetY (190);
 gauge2->SetWidth (140);
 gauge2->SetHeight (20);
 gauge2->SetEnable (1);
 gauge2->SetVisible (1);
 gauge2->SetRange (100);
 gauge2->SetValue (0);
 gauge2->SetType (4);
 Window1.CreateChild (gauge2);
 //label2
 label2 = new CLabel ();
 label2->SetFOwner (&Window1);
 label2->SetName (lxT ("label2_p3"));
 label2->SetX (12);
 label2->SetY (166);
 label2->SetWidth (60);
 label2->SetHeight (20);
 label2->SetEnable (1);
 label2->SetVisible (1);
 label2->SetText (lxT ("Heater"));
 label2->SetAlign (1);
 Window1.CreateChild (label2);
 //label3
 label3 = new CLabel ();
 label3->SetFOwner (&Window1);
 label3->SetName (lxT ("label3_p3"));
 label3->SetX (13);
 label3->SetY (217);
 label3->SetWidth (60);
 label3->SetHeight (20);
 label3->SetEnable (1);
 label3->SetVisible (1);
 label3->SetText (lxT ("Cooler"));
 label3->SetAlign (1);
 Window1.CreateChild (label3);
 //label4
 label4 = new CLabel ();
 label4->SetFOwner (&Window1);
 label4->SetName (lxT ("label4_p3"));
 label4->SetX (13);
 label4->SetY (272);
 label4->SetWidth (120);
 label4->SetHeight (24);
 label4->SetEnable (1);
 label4->SetVisible (1);
 label4->SetText (lxT ("Temp: 00.0°C"));
 label4->SetAlign (1);
 Window1.CreateChild (label4);

 strncpy (fname, (char*) lxGetUserDataDir (lxT ("picsimlab")).char_str (), 1023);
 strncat (fname, "/mdump_McLab2_EEPROM.bin", 1023);

 fout = fopen (fname, "rb");
 if (fout)
  {
   fread (mi2c.data, mi2c.SIZE, 1, fout);
   fclose (fout);
  }
 else
  {
   printf ("Error loading from file: %s \n", fname);
  }

 snprintf (mi2c_tmp_name, 200, "%s/picsimlab-XXXXXX", (const char *) lxGetTempDir ("PICSimLab").c_str ());
 close (mkstemp (mi2c_tmp_name));
 unlink (mi2c_tmp_name);
 strncat (mi2c_tmp_name, ".txt", 200);

}

cboard_McLab2::~cboard_McLab2(void)
{
 buzzer.BeepStop ();
 buzzer.End ();
 mi2c_end (&mi2c);
 delete vent[0];
 delete vent[1];
 vent[0] = NULL;
 vent[1] = NULL;

 Window1.DestroyChild (gauge1);
 Window1.DestroyChild (gauge2);
 Window1.DestroyChild (label2);
 Window1.DestroyChild (label3);
 Window1.DestroyChild (label4);

 unlink (mi2c_tmp_name);
}

void
cboard_McLab2::MDumpMemory(const char * mfname)
{
 FILE * fout;
 char fname[1024];

 strncpy (fname, (char*) lxGetUserDataDir (lxT ("picsimlab")).char_str (), 1023);
 strncat (fname, "/mdump_McLab2_EEPROM.bin", 1023);

 fout = fopen (fname, "wb");
 if (fout)
  {
   fwrite (mi2c.data, mi2c.SIZE, 1, fout);
   fclose (fout);
  }
 else
  {
   printf ("Error saving to file: %s \n", fname);
  }

 bsim_picsim::MDumpMemory (mfname);
}

void
cboard_McLab2::Draw(CDraw *draw)
{
 int i;
 int update = 0; //verifiy if updated is needed

 lcd_blink (&lcd);

 //lab3 draw
 for (i = 0; i < outputc; i++)
  {
   if (output[i].update)//only if need update
    {
     output[i].update = 0;

     if (!update)
      {
       draw->Canvas.Init (Scale, Scale);
      }
     update++; //set to update buffer

     if (!output[i].r)//rectangle
      {

       draw->Canvas.SetFgColor (0, 0, 0);

       switch (output[i].id)
        {
        case O_A1: draw->Canvas.SetBgColor (lm1[18], 0, 0);
         break;
        case O_B1: draw->Canvas.SetBgColor (lm1[19], 0, 0);
         break;
        case O_C1: draw->Canvas.SetBgColor (lm1[20], 0, 0);
         break;
        case O_D1: draw->Canvas.SetBgColor (lm1[21], 0, 0);
         break;
        case O_E1: draw->Canvas.SetBgColor (lm1[26], 0, 0);
         break;
        case O_F1: draw->Canvas.SetBgColor (lm1[27], 0, 0);
         break;
        case O_G1: draw->Canvas.SetBgColor (lm1[28], 0, 0);
         break;

        case O_A2: draw->Canvas.SetBgColor (lm2[18], 0, 0);
         break;
        case O_B2: draw->Canvas.SetBgColor (lm2[19], 0, 0);
         break;
        case O_C2: draw->Canvas.SetBgColor (lm2[20], 0, 0);
         break;
        case O_D2: draw->Canvas.SetBgColor (lm2[21], 0, 0);
         break;
        case O_E2: draw->Canvas.SetBgColor (lm2[26], 0, 0);
         break;
        case O_F2: draw->Canvas.SetBgColor (lm2[27], 0, 0);
         break;
        case O_G2: draw->Canvas.SetBgColor (lm2[28], 0, 0);
         break;

        case O_A3: draw->Canvas.SetBgColor (lm3[18], 0, 0);
         break;
        case O_B3: draw->Canvas.SetBgColor (lm3[19], 0, 0);
         break;
        case O_C3: draw->Canvas.SetBgColor (lm3[20], 0, 0);
         break;
        case O_D3: draw->Canvas.SetBgColor (lm3[21], 0, 0);
         break;
        case O_E3: draw->Canvas.SetBgColor (lm3[26], 0, 0);
         break;
        case O_F3: draw->Canvas.SetBgColor (lm3[27], 0, 0);
         break;
        case O_G3: draw->Canvas.SetBgColor (lm3[28], 0, 0);
         break;

        case O_A4: draw->Canvas.SetBgColor (lm4[18], 0, 0);
         break;
        case O_B4: draw->Canvas.SetBgColor (lm4[19], 0, 0);
         break;
        case O_C4: draw->Canvas.SetBgColor (lm4[20], 0, 0);
         break;
        case O_D4: draw->Canvas.SetBgColor (lm4[21], 0, 0);
         break;
        case O_E4: draw->Canvas.SetBgColor (lm4[26], 0, 0);
         break;
        case O_F4: draw->Canvas.SetBgColor (lm4[27], 0, 0);
         break;
        case O_G4: draw->Canvas.SetBgColor (lm4[28], 0, 0);
         break;

        case O_LCD: draw->Canvas.SetColor (0, 90 * Window1.Get_mcupwr () + 40, 0);
         break;

        case O_BRB0:
        case O_BRB1:
        case O_BRB2:
        case O_BRB3:
        case O_RST:
         draw->Canvas.SetColor (100, 100, 100);
         break;

        case O_SS1:
         output_ids[O_A1]->update = 1;
         output_ids[O_B1]->update = 1;
         output_ids[O_C1]->update = 1;
         output_ids[O_D1]->update = 1;
         output_ids[O_E1]->update = 1;
         output_ids[O_F1]->update = 1;
         output_ids[O_G1]->update = 1;
         output_ids[O_P1]->update = 1;
         draw->Canvas.SetColor (10, 10, 10);
         break;
        case O_SS2:
         output_ids[O_A2]->update = 1;
         output_ids[O_B2]->update = 1;
         output_ids[O_C2]->update = 1;
         output_ids[O_D2]->update = 1;
         output_ids[O_E2]->update = 1;
         output_ids[O_F2]->update = 1;
         output_ids[O_G2]->update = 1;
         output_ids[O_P2]->update = 1;
         draw->Canvas.SetColor (10, 10, 10);
         break;
        case O_SS3:
         output_ids[O_A3]->update = 1;
         output_ids[O_B3]->update = 1;
         output_ids[O_C3]->update = 1;
         output_ids[O_D3]->update = 1;
         output_ids[O_E3]->update = 1;
         output_ids[O_F3]->update = 1;
         output_ids[O_G3]->update = 1;
         output_ids[O_P3]->update = 1;
         draw->Canvas.SetColor (10, 10, 10);
         break;
        case O_SS4:
         output_ids[O_A4]->update = 1;
         output_ids[O_B4]->update = 1;
         output_ids[O_C4]->update = 1;
         output_ids[O_D4]->update = 1;
         output_ids[O_E4]->update = 1;
         output_ids[O_F4]->update = 1;
         output_ids[O_G4]->update = 1;
         output_ids[O_P4]->update = 1;
         draw->Canvas.SetColor (10, 10, 10);
         break;

        case O_POT1:
         draw->Canvas.SetBgColor (66, 109, 246);
         break;

        case O_MP:
         draw->Canvas.SetColor (26, 26, 26);
         break;

        default: if ((output[i].name[0] == 'J')&&(output[i].name[1] == 'P')) draw->Canvas.SetColor (150, 150, 150);
         break;
        }


       if ((output[i].id >= O_BRB0)&&(output[i].id <= O_BRB3))
        {
         draw->Canvas.Circle (1, output[i].cx, output[i].cy, 22);
         if (p_BT[output[i].id - O_BRB0])
          {
           draw->Canvas.SetColor (15, 15, 15);
          }
         else
          {
           draw->Canvas.SetColor (55, 55, 55);
          }
         draw->Canvas.Circle (1, output[i].cx, output[i].cy, 19);
        }
       else if (output[i].id == O_RST)
        {
         draw->Canvas.Circle (1, output[i].cx, output[i].cy, 11);
         if (p_RST)
          {
           draw->Canvas.SetColor (15, 15, 15);
          }
         else
          {
           draw->Canvas.SetColor (55, 55, 55);
          }
         draw->Canvas.Circle (1, output[i].cx, output[i].cy, 9);
        }
       else if (output[i].id == O_POT1)
        {
         draw->Canvas.Rectangle (1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);

         draw->Canvas.SetBgColor (250, 250, 250);
         draw->Canvas.Circle (1, output[i].cx, output[i].cy, 20);

         draw->Canvas.SetBgColor (150, 150, 150);
         int x = -15 * sin ((5.585 * (pot1 / 200.0)) + 0.349);
         int y = 15 * cos ((5.585 * (pot1 / 200.0)) + 0.349);
         draw->Canvas.Circle (1, output[i].cx + x, output[i].cy + y, 3);

        }
       else if (output[i].id == O_MP)
        {
         draw->Canvas.SetFont (font);
         draw->Canvas.Rectangle (1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
         draw->Canvas.SetColor (230, 230, 230);
         draw->Canvas.RotatedText (Proc, output[i].x1 + 10, output[i].y2 - 50, 90);
        }
       else if (output[i].id == O_VT)
        {
         draw->Canvas.PutBitmap (vent[vt], output[i].x1, output[i].y1);
        }
       else if (output[i].id == O_LCD)
        {
         if (lcd.update)
          {
           draw->Canvas.Rectangle (1, output[i].x1 - 1, output[i].y1 - 1, output[i].x2 - output[i].x1 + 2, output[i].y2 - output[i].y1 + 3);
           lcd_draw (&lcd, &draw->Canvas, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1, Window1.Get_mcupwr ());
          }
        }
       else if ((output[i].name[0] == 'J')&&(output[i].name[1] == 'P'))
        {
         if (!jmp[output[i].name[3] - 0x31])
          {
           draw->Canvas.SetColor (70, 70, 70);
           draw->Canvas.Rectangle (1, output[i].x1, output[i].y1, (int) ((output[i].x2 - output[i].x1)*0.65), output[i].y2 - output[i].y1);
           draw->Canvas.SetColor (220, 220, 0);
           draw->Canvas.Circle (1, output[i].x1 + (int) ((output[i].x2 - output[i].x1)*0.80), output[i].y1 + ((output[i].y2 - output[i].y1) / 2), 3);
          }
         else
          {
           draw->Canvas.SetColor (70, 70, 70);
           draw->Canvas.Rectangle (1, output[i].x1 + ((int) ((output[i].x2 - output[i].x1)*0.35)), output[i].y1, (int) ((output[i].x2 - output[i].x1)*0.65), output[i].y2 - output[i].y1);
           draw->Canvas.SetColor (220, 220, 0);
           draw->Canvas.Circle (1, output[i].x1 + (int) ((output[i].x2 - output[i].x1)*0.20), output[i].y1 + ((output[i].y2 - output[i].y1) / 2), 3);
          }
        }
       else
        {
         draw->Canvas.Rectangle (1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
        }
      }
     else //circle
      {
       int led = 1;

       draw->Canvas.SetFgColor (55, 0, 0);


       switch (output[i].id)
        {
        case O_RB0: draw->Canvas.SetBgColor (pic.pins[32].oavalue, 0, 0);
         break;
        case O_RB1: draw->Canvas.SetBgColor (pic.pins[33].oavalue, 0, 0);
         break;
        case O_RB2: draw->Canvas.SetBgColor (pic.pins[34].oavalue, 0, 0);
         break;
        case O_RB3: draw->Canvas.SetBgColor (pic.pins[35].oavalue, 0, 0);
         break;
        case O_LPWR: draw->Canvas.SetBgColor (0, 200 * Window1.Get_mcupwr () + 55, 0);
         break;
        case O_P1: led = 0;
         draw->Canvas.SetBgColor (lm1[29], 0, 0);
         break;
        case O_P2: led = 0;
         draw->Canvas.SetBgColor (lm2[29], 0, 0);
         break;
        case O_P3: led = 0;
         draw->Canvas.SetBgColor (lm3[29], 0, 0);
         break;
        case O_P4: led = 0;
         draw->Canvas.SetBgColor (lm4[29], 0, 0);
         break;
        }

       if (led)
        {
         //draw a LED
         color1 = draw->Canvas.GetBgColor ();
         int r = color1.Red () - 120;
         int g = color1.Green () - 120;
         int b = color1.Blue () - 120;
         if (r < 0)r = 0;
         if (g < 0)g = 0;
         if (b < 0)b = 0;
         color2.Set (r, g, b);
         draw->Canvas.SetBgColor (color2);
         draw->Canvas.Circle (1, output[i].x1, output[i].y1, output[i].r + 1);
         draw->Canvas.SetBgColor (color1);
         draw->Canvas.Circle (1, output[i].x1, output[i].y1, output[i].r - 2);
        }
       else
        {
         draw->Canvas.Circle (1, output[i].x1, output[i].y1, output[i].r);
        }
      }
    }
  }
 //end draw

 if (update)
  {
   draw->Canvas.End ();
   draw->Update ();
  }


 if ((((pic.pins[6].oavalue - 55) / 2) > 40) && Window1.Get_mcupwr ())
  {
   if (!sound_on)
    {
     buzzer.BeepStart ();
     sound_on = 1;
    }
  }
 else
  {
   buzzer.BeepStop ();
   sound_on = 0;
  }

 //Cooler
 gauge1->SetValue ((pic.pins[15].oavalue - 55) / 2);
 if (gauge1->GetValue () > 20) vtc++;
 if (vtc > (4 - 0.04 * gauge1->GetValue ()))
  {
   vtc = 0;
   vt ^= 1;
   output_ids[O_VT]->update = 1;
  }

 //Heater
 gauge2->SetValue ((pic.pins[16].oavalue - 55) / 2);

 //thacometer
 rpmstp = ((float) Window1.GetNSTEPJ ()) / (0.7196 * (pic.pins[15].oavalue - 54));

 //potentiometer
 vp2in = (5.0 * pot1 / 199);

 //temp
 ref = (0.25 * (pic.pins[16].oavalue - 55))-(0.25 * (pic.pins[15].oavalue - 55));

 if (ref < 0)
  ref = 0;

 temp[1] = temp[0];
 temp[0] = ((27.5 + ref)*0.003) + temp[1]*(0.997);

 pic_set_apin (2, (10.0 / 255.0)*(temp[0] + 15.0));

 //referencia
 pic_set_apin (5, 2.5);

}

void
cboard_McLab2::Run_CPU(void)
{
 int i;
 int j;
 unsigned char pi;
 unsigned char pinv;
 const picpin * pins;
 unsigned int alm[40]; //luminosidade media
 unsigned int alm1[40]; //luminosidade media display
 unsigned int alm2[40]; //luminosidade media display
 unsigned int alm3[40]; //luminosidade media display
 unsigned int alm4[40]; //luminosidade media display     


 int JUMPSTEPS = Window1.GetJUMPSTEPS ();
 long int NSTEPJ = Window1.GetNSTEPJ ();

 memset (alm, 0, 40 * sizeof (unsigned int));
 memset (alm1, 0, 40 * sizeof (unsigned int));
 memset (alm2, 0, 40 * sizeof (unsigned int));
 memset (alm3, 0, 40 * sizeof (unsigned int));
 memset (alm4, 0, 40 * sizeof (unsigned int));

 pins = pic.pins;

 if (use_spare)Window5.PreProcess ();

 j = JUMPSTEPS;
 if (Window1.Get_mcupwr ())
  for (i = 0; i < Window1.GetNSTEP (); i++)
   {


    if (j >= JUMPSTEPS)
     {
      pic_set_pin (pic.mclr, p_RST);

      pic_set_pin (33, p_BT[0]);
      pic_set_pin (34, p_BT[1]);
      pic_set_pin (35, p_BT[2]);
      pic_set_pin (36, p_BT[3]);

      rpmc++;
      if (rpmc > rpmstp)
       {
        rpmc = 0;
        pic_set_pin (15, !pic_get_pin (15));
       }
     }

    if (!mplabxd_testbp ())pic_step ();
    if (use_oscope)Window4.SetSample ();
    if (use_spare)Window5.Process ();

    //increment mean value counter if pin is high
    alm[i % pic.PINCOUNT] += pins[i % pic.PINCOUNT].value;

    if (j >= JUMPSTEPS)
     {

      for (pi = 18; pi < 30; pi++)
       {
        pinv = pins[pi].value;
        if ((pinv)&&(pins[39].value)) alm1[pi]++;
        if ((pinv)&&(pins[38].value)) alm2[pi]++;
        if ((pinv)&&(pins[37].value)) alm3[pi]++;
        if ((pinv)&&(pins[36].value)) alm4[pi]++;
       }


      j = -1;
     }
    j++;

    //potênciometro p2
    //p2 rc circuit

    if (!pins[2].dir)
     {
      //decarga por RA1  
      vp2[1] = vp2[0] = 5 * pins[2].value;
     }

    vp2[1] = vp2[0];

    vp2[0] = vp2in * 0.00021 + vp2[1]*0.99979;

    if (pins[2].ptype < 3)
     pic_set_pin (3, vp2[0] > 1.25);
    else
     pic_set_apin (3, vp2[0]);


    //lcd dipins[2].dirsplay code
    if ((!pins[8].dir)&&(!pins[8].value))
     {
      if (!lcde)
       {
        d = 0;
        if (pins[29].value) d |= 0x80;
        if (pins[28].value) d |= 0x40;
        if (pins[27].value) d |= 0x20;
        if (pins[26].value) d |= 0x10;
        if (pins[21].value) d |= 0x08;
        if (pins[20].value) d |= 0x04;
        if (pins[19].value) d |= 0x02;
        if (pins[18].value) d |= 0x01;

        if ((!pins[7].dir)&&(!pins[7].value))
         {
          lcd_cmd (&lcd, d);
         }
        else if ((!pins[7].dir)&&(pins[7].value))
         {
          lcd_data (&lcd, d);
         }
        lcde = 1;
       }

     }
    else
     {
      lcde = 0;
     }

    //i2c code
    if (pins[22].dir)
     {
      sda = 1;
     }
    else
     {
      sda = pins[22].value;
     }

    if (pins[17].dir)
     {
      sck = 1;
      pic_set_pin (18, 1);
     }
    else
     {
      sck = pins[17].value;
     }
    pic_set_pin (23, mi2c_io (&mi2c, sck, sda));

   }
 //fim STEP

 for (pi = 0; pi < pic.PINCOUNT; pi++)
  {
   if (pic.pins[pi].port == P_VDD)
    pic.pins[pi].oavalue = 255;
   else
    pic.pins[pi].oavalue = (int) (((200.0 * alm[pi]) / (Window1.GetNSTEP () / pic.PINCOUNT)) + 55);

   lm1[pi] = (int) (((600.0 * alm1[pi]) / NSTEPJ) + 30);
   lm2[pi] = (int) (((600.0 * alm2[pi]) / NSTEPJ) + 30);
   lm3[pi] = (int) (((600.0 * alm3[pi]) / NSTEPJ) + 30);
   lm4[pi] = (int) (((600.0 * alm4[pi]) / NSTEPJ) + 30);
   if (lm1[pi] > 255)lm1[pi] = 255;
   if (lm2[pi] > 255)lm2[pi] = 255;
   if (lm3[pi] > 255)lm3[pi] = 255;
   if (lm4[pi] > 255)lm4[pi] = 255;
  }

 if (use_spare)Window5.PostProcess ();

 if (lcd.update) output_ids[O_LCD]->update = 1;

 //verifiy if LEDS need update 
 for (i = 0; i < 4; i++)
  {
   //PORTB LEDS
   if (output_ids[O_RB0 + i]->value != pic.pins[32 + i].oavalue)
    {
     output_ids[O_RB0 + i]->value = pic.pins[32 + i].oavalue;
     output_ids[O_RB0 + i]->update = 1;
    }
  }

 for (i = 0; i < 8; i++)
  {

   if (i < 4)
    {
     j = i;
    }
   else
    {
     j = i + 4;
    }
   //7s DISP
   if (output_ids[O_A1 + i]->value != lm1[18 + j])
    {
     output_ids[O_A1 + i]->value = lm1[18 + j];
     output_ids[O_SS1]->update = 1;
    }
   if (output_ids[O_A2 + i]->value != lm2[18 + j])
    {
     output_ids[O_A2 + i]->value = lm2[18 + j];
     output_ids[O_SS2]->update = 1;
    }
   if (output_ids[O_A3 + i]->value != lm3[18 + j])
    {
     output_ids[O_A3 + i]->value = lm3[18 + j];
     output_ids[O_SS3]->update = 1;
    }
   if (output_ids[O_A4 + i]->value != lm4[18 + j])
    {
     output_ids[O_A4 + i]->value = lm4[18 + j];
     output_ids[O_SS4]->update = 1;
    }
  }
}

void
cboard_McLab2::Reset(void)
{

 pic_reset (1);
 lcd_rst (&lcd);
 mi2c_rst (&mi2c);


 p_BT[0] = 1;
 p_BT[1] = 1;
 p_BT[2] = 1;
 p_BT[3] = 1;


 pic_set_pin (33, p_BT[0]);
 pic_set_pin (34, p_BT[1]);
 pic_set_pin (35, p_BT[2]);
 pic_set_pin (36, p_BT[3]);

#ifndef _WIN_
 if (pic.serial[0].serialfd > 0)
#else
 if (pic.serial[0].serialfd != INVALID_HANDLE_VALUE)
#endif
  Window1.statusbar1.SetField (2, lxT ("Serial: ") + lxString::FromAscii (SERIALDEVICE) + lxT (":") + itoa (pic.serial[0].serialbaud) + lxT ("(") + lxString ().Format ("%4.1f", fabs ((100.0 * pic.serial[0].serialexbaud - 100.0 * pic.serial[0].serialbaud) / pic.serial[0].serialexbaud)) + lxT ("%)"));
 else
  Window1.statusbar1.SetField (2, lxT ("Serial: ") + lxString::FromAscii (SERIALDEVICE) + lxT (" (ERROR)"));

 for (int pi = 0; pi < pic.PINCOUNT; pi++)
  {
   lm1[pi] = 0;
   lm2[pi] = 0;
   lm3[pi] = 0;
   lm4[pi] = 0;
  }
 if (use_spare)Window5.Reset ();

 RegisterRemoteControl ();
}

void
cboard_McLab2::RegisterRemoteControl(void)
{
 for (int i = 0; i < inputc; i++)
  {
   switch (input[i].id)
    {
    case I_RB0:
     input[i].status = &p_BT[0];
     break;
    case I_RB1:
     input[i].status = &p_BT[1];
     break;
    case I_RB2:
     input[i].status = &p_BT[2];
     break;
    case I_RB3:
     input[i].status = &p_BT[3];
     break;
    case I_POT1:
     input[i].status = &pot1;
     break;
    }
  }

 for (int i = 0; i < outputc; i++)
  {
   switch (output[i].id)
    {
    case O_RB0:
     output[i].status = &pic.pins[32].oavalue;
     break;
    case O_RB1:
     output[i].status = &pic.pins[33].oavalue;
     break;
    case O_RB2:
     output[i].status = &pic.pins[34].oavalue;
     break;
    case O_RB3:
     output[i].status = &pic.pins[35].oavalue;
     break;
    case O_LCD:
     output[i].status = &lcd;
     break;
    case O_A1:
     output[i].status = &lm1[18];
     break;
     output[i].status = &lm1[19];
     break;
    case O_C1:
     output[i].status = &lm1[20];
     break;
    case O_D1:
     output[i].status = &lm1[21];
     break;
    case O_E1:
     output[i].status = &lm1[26];
     break;
    case O_F1:
     output[i].status = &lm1[27];
     break;
    case O_G1:
     output[i].status = &lm1[28];
     break;
    case O_P1:
     output[i].status = &lm1[29];
     break;

    case O_A2:
     output[i].status = &lm2[18];
     break;
    case O_B2:
     output[i].status = &lm2[19];
     break;
    case O_C2:
     output[i].status = &lm2[20];
     break;
    case O_D2:
     output[i].status = &lm2[21];
     break;
    case O_E2:
     output[i].status = &lm2[26];
     break;
    case O_F2:
     output[i].status = &lm2[27];
     break;
    case O_G2:
     output[i].status = &lm2[28];
     break;
    case O_P2:
     output[i].status = &lm2[29];
     break;

    case O_A3:
     output[i].status = &lm3[18];
     break;
    case O_B3:
     output[i].status = &lm3[19];
     break;
    case O_C3:
     output[i].status = &lm3[20];
     break;
    case O_D3:
     output[i].status = &lm3[21];
     break;
    case O_E3:
     output[i].status = &lm3[26];
     break;
    case O_F3:
     output[i].status = &lm3[27];
     break;
    case O_G3:
     output[i].status = &lm3[28];
     break;
    case O_P3:
     output[i].status = &lm3[29];
     break;

    case O_A4:
     output[i].status = &lm4[18];
     break;
    case O_B4:
     output[i].status = &lm4[19];
     break;
    case O_C4:
     output[i].status = &lm4[20];
     break;
    case O_D4:
     output[i].status = &lm4[21];
     break;
    case O_E4:
     output[i].status = &lm4[26];
     break;
    case O_F4:
     output[i].status = &lm4[27];
     break;
    case O_G4:
     output[i].status = &lm4[28];
     break;
    case O_P4:
     output[i].status = &lm4[29];
     break;
    }
  }
}

void
cboard_McLab2::EvMouseButtonPress(uint button, uint x, uint y, uint state)
{

 int i;

 for (i = 0; i < inputc; i++)
  {
   if (((input[i].x1 <= x)&&(input[i].x2 >= x))&&((input[i].y1 <= y)&&(input[i].y2 >= y)))
    {

     switch (input[i].id)
      {
      case I_ICSP:
       {
        Window1.menu1_File_LoadHex_EvMenuActive (NULL);
       }
       break;

      case I_JP1:
       {
        jmp[0] ^= 0x01;
        output_ids[O_JP1]->update = 1;
       }
       break;
      case I_JP2:
       {
        jmp[1] ^= 0x01;
        output_ids[O_JP2]->update = 1;
       }
       break;
      case I_JP3:
       {
        jmp[2] ^= 0x01;
        output_ids[O_JP3]->update = 1;
       }
       break;
      case I_JP4:
       {
        jmp[3] ^= 0x01;
        output_ids[O_JP4]->update = 1;
       }
       break;
      case I_JP5:
       {
        jmp[4] ^= 0x01;
        output_ids[O_JP5]->update = 1;
       }
       break;
      case I_JP6:
       {
        jmp[5] ^= 0x01;
        output_ids[O_JP6]->update = 1;
       }
       break;


      case I_PWR:
       {
        if (Window1.Get_mcupwr ())
         {
          Window1.Set_mcurun (0);
          Window1.Set_mcupwr (0);
          Reset ();

          p_BT[0] = 0;
          p_BT[1] = 0;
          p_BT[2] = 0;
          p_BT[3] = 0;
          Window1.statusbar1.SetField (0, lxT ("Stoped"));
         }
        else
         {
          Window1.Set_mcupwr (1);
          Window1.Set_mcurun (1);
          Reset ();


          Window1.statusbar1.SetField (0, lxT ("Running..."));
         }
       }
       output_ids[O_LPWR]->update = 1;
       break;
      case I_RST:
       {
        if (Window1.Get_mcupwr () && pic_reset (-1))
         {
          Window1.Set_mcupwr (0);
          Window1.Set_mcurst (1);
         }
        p_RST = 0;
        output_ids[O_RST]->update = 1;
       }
       break;

      case I_RB0:
       {
        p_BT[0] = 0;
        output_ids[O_BRB0]->update = 1;
       }
       break;
      case I_RB1:
       {
        p_BT[1] = 0;
        output_ids[O_BRB1]->update = 1;
       }
       break;
      case I_RB2:
       {
        p_BT[2] = 0;
        output_ids[O_BRB2]->update = 1;
       }
       break;
      case I_RB3:
       {
        p_BT[3] = 0;
        output_ids[O_BRB3]->update = 1;
       }
       break;
      case I_POT1:
       {
        active = 1;
        pot1 = CalcAngle (i, x, y);
        output_ids[O_POT1]->update = 1;
       }
       break;
      case I_VIEW:
       FILE * fout;
       fout = fopen (mi2c_tmp_name, "w");
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
         }, mi2c_tmp_name);
#else
         lxLaunchDefaultApplication (mi2c_tmp_name);
#endif          
        }
       else
        {
         printf ("Error saving to file: %s \n", mi2c_tmp_name);
        }
       break;
      }
    }
  }
}

void
cboard_McLab2::EvMouseMove(uint button, uint x, uint y, uint state)
{
 int i;

 for (i = 0; i < inputc; i++)
  {
   switch (input[i].id)
    {
    case I_POT1:
     if (((input[i].x1 <= x)&&(input[i].x2 >= x))&&((input[i].y1 <= y)&&(input[i].y2 >= y)))
      {
       if (active)
        {
         pot1 = CalcAngle (i, x, y);
         output_ids[O_POT1]->update = 1;
        }
      }
     break;
    }
  }
}

void
cboard_McLab2::EvMouseButtonRelease(uint button, uint x, uint y, uint state)
{
 int i;

 for (i = 0; i < inputc; i++)
  {
   if (((input[i].x1 <= x)&&(input[i].x2 >= x))&&((input[i].y1 <= y)&&(input[i].y2 >= y)))
    {
     switch (input[i].id)
      {
      case I_RST:
       {
        if (Window1.Get_mcurst ())
         {
          Window1.Set_mcupwr (1);
          Window1.Set_mcurst (0);

          if (pic_reset (-1))
           {
            Reset ();
           }
         }
        p_RST = 1;
        output_ids[O_RST]->update = 1;
       }
       break;

      case I_RB0:
       {
        p_BT[0] = 1;
        output_ids[O_BRB0]->update = 1;
       }
       break;
      case I_RB1:
       {
        p_BT[1] = 1;
        output_ids[O_BRB1]->update = 1;
       }
       break;
      case I_RB2:
       {
        p_BT[2] = 1;
        output_ids[O_BRB2]->update = 1;
       }
       break;
      case I_RB3:
       {
        p_BT[3] = 1;
        output_ids[O_BRB3]->update = 1;
       }
       break;
      case I_POT1:
       {
        active = 0;
        output_ids[O_POT1]->update = 1;
       }
       break;
      }
    }
  }

}

void
cboard_McLab2::EvKeyPress(uint key, uint mask)
{
 if (key == '1')
  {
   p_BT[0] = 0;
   output_ids[O_BRB0]->update = 1;
  }
 if (key == '2')
  {
   p_BT[1] = 0;
   output_ids[O_BRB1]->update = 1;
  }
 if (key == '3')
  {
   p_BT[2] = 0;
   output_ids[O_BRB2]->update = 1;
  }
 if (key == '4')
  {
   p_BT[3] = 0;
   output_ids[O_BRB3]->update = 1;
  }
}

void
cboard_McLab2::EvKeyRelease(uint key, uint mask)
{
 if (key == '1')
  {
   p_BT[0] = 1;
   output_ids[O_BRB0]->update = 1;
  }

 if (key == '2')
  {
   p_BT[1] = 1;
   output_ids[O_BRB1]->update = 1;
  }

 if (key == '3')
  {
   p_BT[2] = 1;
   output_ids[O_BRB2]->update = 1;
  }

 if (key == '4')
  {
   p_BT[3] = 1;
   output_ids[O_BRB3]->update = 1;
  }
}

void
cboard_McLab2::EvOnShow(void)
{
 lcd.update = 1;
 board::EvOnShow ();
}

unsigned short
cboard_McLab2::get_in_id(char * name)
{
 if (strcmp (name, "PB_RST") == 0)return I_RST;

 if (strcmp (name, "SW_PWR") == 0)return I_PWR;
 if (strcmp (name, "PG_ICSP") == 0)return I_ICSP;

 if (strcmp (name, "PB_RB0") == 0)return I_RB0;
 if (strcmp (name, "PB_RB1") == 0)return I_RB1;
 if (strcmp (name, "PB_RB2") == 0)return I_RB2;
 if (strcmp (name, "PB_RB3") == 0)return I_RB3;

 if (strcmp (name, "JP_1") == 0)return I_JP1;
 if (strcmp (name, "JP_2") == 0)return I_JP2;
 if (strcmp (name, "JP_3") == 0)return I_JP3;
 if (strcmp (name, "JP_4") == 0)return I_JP4;
 if (strcmp (name, "JP_5") == 0)return I_JP5;
 if (strcmp (name, "JP_6") == 0)return I_JP6;

 if (strcmp (name, "MD_VIEW") == 0)return I_VIEW;

 if (strcmp (name, "PO_2") == 0)return I_POT1;

 printf ("Erro input '%s' don't have a valid id! \n", name);
 return -1;
}

unsigned short
cboard_McLab2::get_out_id(char * name)
{

 if (strcmp (name, "LD_RB0") == 0)return O_RB0;
 if (strcmp (name, "LD_RB1") == 0)return O_RB1;
 if (strcmp (name, "LD_RB2") == 0)return O_RB2;
 if (strcmp (name, "LD_RB3") == 0)return O_RB3;

 if (strcmp (name, "SS_1") == 0)return O_SS1;
 if (strcmp (name, "SS_2") == 0)return O_SS2;
 if (strcmp (name, "SS_3") == 0)return O_SS3;
 if (strcmp (name, "SS_4") == 0)return O_SS4;

 if (strcmp (name, "SS_A1") == 0)return O_A1;
 if (strcmp (name, "SS_B1") == 0)return O_B1;
 if (strcmp (name, "SS_C1") == 0)return O_C1;
 if (strcmp (name, "SS_D1") == 0)return O_D1;
 if (strcmp (name, "SS_E1") == 0)return O_E1;
 if (strcmp (name, "SS_F1") == 0)return O_F1;
 if (strcmp (name, "SS_G1") == 0)return O_G1;
 if (strcmp (name, "SS_P1") == 0)return O_P1;

 if (strcmp (name, "SS_A2") == 0)return O_A2;
 if (strcmp (name, "SS_B2") == 0)return O_B2;
 if (strcmp (name, "SS_C2") == 0)return O_C2;
 if (strcmp (name, "SS_D2") == 0)return O_D2;
 if (strcmp (name, "SS_E2") == 0)return O_E2;
 if (strcmp (name, "SS_F2") == 0)return O_F2;
 if (strcmp (name, "SS_G2") == 0)return O_G2;
 if (strcmp (name, "SS_P2") == 0)return O_P2;

 if (strcmp (name, "LD_LPWR") == 0)return O_LPWR;
 if (strcmp (name, "DS_LCD") == 0)return O_LCD;

 if (strcmp (name, "SS_A3") == 0)return O_A3;
 if (strcmp (name, "SS_B3") == 0)return O_B3;
 if (strcmp (name, "SS_C3") == 0)return O_C3;
 if (strcmp (name, "SS_D3") == 0)return O_D3;
 if (strcmp (name, "SS_E3") == 0)return O_E3;
 if (strcmp (name, "SS_F3") == 0)return O_F3;
 if (strcmp (name, "SS_G3") == 0)return O_G3;
 if (strcmp (name, "SS_P3") == 0)return O_P3;

 if (strcmp (name, "SS_A4") == 0)return O_A4;
 if (strcmp (name, "SS_B4") == 0)return O_B4;
 if (strcmp (name, "SS_C4") == 0)return O_C4;
 if (strcmp (name, "SS_D4") == 0)return O_D4;
 if (strcmp (name, "SS_E4") == 0)return O_E4;
 if (strcmp (name, "SS_F4") == 0)return O_F4;
 if (strcmp (name, "SS_G4") == 0)return O_G4;
 if (strcmp (name, "SS_P4") == 0)return O_P4;

 if (strcmp (name, "JP_1") == 0)return O_JP1;
 if (strcmp (name, "JP_2") == 0)return O_JP2;
 if (strcmp (name, "JP_3") == 0)return O_JP3;
 if (strcmp (name, "JP_4") == 0)return O_JP4;
 if (strcmp (name, "JP_5") == 0)return O_JP5;
 if (strcmp (name, "JP_6") == 0)return O_JP6;

 if (strcmp (name, "PB_RB0") == 0)return O_BRB0;
 if (strcmp (name, "PB_RB1") == 0)return O_BRB1;
 if (strcmp (name, "PB_RB2") == 0)return O_BRB2;
 if (strcmp (name, "PB_RB3") == 0)return O_BRB3;

 if (strcmp (name, "PO_2") == 0)return O_POT1;

 if (strcmp (name, "PB_RST") == 0)return O_RST;

 if (strcmp (name, "MT_VT") == 0) return O_VT;

 if (strcmp (name, "MP_CPU") == 0)return O_MP;

 printf ("Erro output '%s' don't have a valid id! \n", name);
 return 1;
}

void
cboard_McLab2::RefreshStatus(void)
{
 label4->SetText (lxT ("Temp: ") + lxString ().Format ("%5.2f", temp[0]) + lxT ("°C"));

#ifndef _WIN_
 if (pic.serial[0].serialfd > 0)
#else
 if (pic.serial[0].serialfd != INVALID_HANDLE_VALUE)
#endif
  Window1.statusbar1.SetField (2, lxT ("Serial: ") + lxString::FromAscii (SERIALDEVICE) + lxT (":") + itoa (pic.serial[0].serialbaud) + lxT ("(") + lxString ().Format ("%4.1f", fabs ((100.0 * pic.serial[0].serialexbaud - 100.0 * pic.serial[0].serialbaud) / pic.serial[0].serialexbaud)) + lxT ("%)"));
 else
  Window1.statusbar1.SetField (2, lxT ("Serial: ") + lxString::FromAscii (SERIALDEVICE) + lxT (" (ERROR)"));

}

void
cboard_McLab2::WritePreferences(void)
{
 char line[100];
 Window1.saveprefs (lxT ("McLab2_proc"), Proc);

 line[0] = 0;
 for (int i = 0; i < 6; i++)
  sprintf (line + i, "%i", jmp[i]);

 Window1.saveprefs (lxT ("McLab2_jmp"), line);
 Window1.saveprefs (lxT ("McLab2_clock"), lxString ().Format ("%2.1f", Window1.GetClock ()));
 Window1.saveprefs (lxT ("McLab2_pot1"), lxString ().Format ("%i", pot1));
}

void
cboard_McLab2::ReadPreferences(char *name, char *value)
{
 if (!strcmp (name, "McLab2_proc"))
  {
   Proc = value;
  }

 int i;
 if (!strcmp (name, "McLab2_jmp"))
  {
   for (i = 0; i < 6; i++)
    {
     if (value[i] == '0')
      jmp[i] = 0;
     else
      jmp[i] = 1;
    }
  }

 if (!strcmp (name, "McLab2_clock"))
  {
   Window1.SetClock (atof (value));
  }

 if (!strcmp (name, "McLab2_pot1"))
  {
   pot1 = atoi (value);
  }
}


board_init(BOARD_McLab2_Name, cboard_McLab2);



