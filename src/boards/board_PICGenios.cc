/* ########################################################################

   PICsimLab - PIC laboratory simulator

   ########################################################################

   Copyright (c) : 2010-2020  Luis Claudio Gambôa Lopes

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
#include"board_PICGenios.h"

#ifdef __EMSCRIPTEN__
#include<emscripten.h>
#endif

/* outputs */
enum
{
 O_RB0, O_RB1, O_RB2, O_RB3, O_RB4, O_RB5, O_RB6, O_RB7, O_LPWR, O_LCD,
 O_A1, O_B1, O_C1, O_D1, O_E1, O_F1, O_G1, O_P1,
 O_A2, O_B2, O_C2, O_D2, O_E2, O_F2, O_G2, O_P2,
 O_A3, O_B3, O_C3, O_D3, O_E3, O_F3, O_G3, O_P3,
 O_A4, O_B4, O_C4, O_D4, O_E4, O_F4, O_G4, O_P4,
 O_RD0, O_RD1, O_RD2, O_RD3, O_RD4, O_RD5, O_RD6, O_RD7,
 O_RL1, O_RL2, O_D01, O_D02, O_D03, O_D04, O_D05, O_D06, O_D07, O_D08,
 O_D09, O_D10, O_D11, O_D12, O_D13, O_D14, O_D15, O_D16, O_D17, O_D18,
 O_D19, O_D20, O_VT, O_PRG, O_RUN, O_JP1
};

/*inputs*/

enum
{
 I_RST, I_RA1, I_RA2, I_RA3, I_RA4, I_RA5, I_PWR, I_ICSP,
 I_RB0, I_RB1, I_RB2, I_RB3, I_RB4, I_RB5,
 I_TC1, I_TC2, I_TC3, I_TC4, I_TC5, I_TC6, I_TC7, I_TC8, I_TC9,
 I_TCA, I_TC0, I_TCT, I_D01, I_D02, I_D03, I_D04, I_D05, I_D06, I_D07,
 I_D08, I_D09, I_D10, I_D11, I_D12, I_D13, I_D14, I_D15, I_D16, I_D17,
 I_D18, I_D19, I_D20, I_JP1, I_VIEW
};


//TODO TEMP cooler must don't work with AQUE=0

cboard_PICGenios::cboard_PICGenios(void)
{
 char fname[1024];
 FILE * fout;

 Proc = "PIC18F452";

 vtc = 0;
 vt = 0;

 vp1in = 2.5;
 vp2in = 2.5;
 vp2[0] = 2.5;
 vp2[1] = 2.5;
 temp[0] = 27.5;
 temp[1] = 27.5;
 ref = 0;

 rpmstp = 0; //(NSTEP*2)/100;
 rpmc = 0;


 d = 0;
 lcde = 0;
 sound_on = 0;

 lxImage image;
 image.LoadFile (Window1.GetSharePath () + lxT ("boards/Common/VT1.png"));
 vent[0] = new lxBitmap (image, &Window1);
 image.LoadFile (Window1.GetSharePath () + lxT ("boards/Common/VT2.png"));
 vent[1] = new lxBitmap (image, &Window1);

 image.LoadFile (Window1.GetSharePath () + lxT ("boards/Common/lcd2.png"));
 lcdbmp[0] = new lxBitmap (image, &Window1);
 image.LoadFile (Window1.GetSharePath () + lxT ("boards/Common/lcd4.png"));
 lcdbmp[1] = new lxBitmap (image, &Window1);

 image.Destroy ();

 lcd_init (&lcd, 16, 2);
 mi2c_init (&mi2c, 4);
 rtc2_init (&rtc2);

 ReadMaps ();

 jmp[0] = 0;

 for (int i = 0; i < 20; i++)
  dip[i] = 1;

 buzzer.Init ();

 //scroll1
 scroll1 = new CScroll ();
 scroll1->SetFOwner (&Window1);
 scroll1->SetName (lxT ("scroll1_p4"));
 scroll1->SetX (12);
 scroll1->SetY (193 + 20);
 scroll1->SetWidth (140);
 scroll1->SetHeight (22);
 scroll1->SetEnable (1);
 scroll1->SetVisible (1);
 scroll1->SetRange (100);
 scroll1->SetPosition (50);
 scroll1->SetType (4);
 Window1.CreateChild (scroll1);
 //scroll2
 scroll2 = new CScroll ();
 scroll2->SetFOwner (&Window1);
 scroll2->SetName (lxT ("scroll2_p4"));
 scroll2->SetX (12);
 scroll2->SetY (141 + 20);
 scroll2->SetWidth (140);
 scroll2->SetHeight (22);
 scroll2->SetEnable (1);
 scroll2->SetVisible (1);
 scroll2->SetRange (100);
 scroll2->SetPosition (50);
 scroll2->SetType (4);
 Window1.CreateChild (scroll2);
 //gauge1
 gauge1 = new CGauge ();
 gauge1->SetFOwner (&Window1);
 gauge1->SetName (lxT ("gauge1_p4"));
 gauge1->SetX (13);
 gauge1->SetY (302 + 20);
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
 gauge2->SetName (lxT ("gauge2_p4"));
 gauge2->SetX (12);
 gauge2->SetY (250 + 20);
 gauge2->SetWidth (140);
 gauge2->SetHeight (20);
 gauge2->SetEnable (1);
 gauge2->SetVisible (1);
 gauge2->SetRange (100);
 gauge2->SetValue (0);
 gauge2->SetType (4);
 Window1.CreateChild (gauge2);
 //label1
 label1 = new CLabel ();
 label1->SetFOwner (&Window1);
 label1->SetName (lxT ("label1_p4"));
 label1->SetX (12);
 label1->SetY (117 + 20);
 label1->SetWidth (60);
 label1->SetHeight (20);
 label1->SetEnable (1);
 label1->SetVisible (1);
 label1->SetText (lxT ("Pot. P1"));
 label1->SetAlign (1);
 Window1.CreateChild (label1);
 //label2
 label2 = new CLabel ();
 label2->SetFOwner (&Window1);
 label2->SetName (lxT ("label2_p4"));
 label2->SetX (12);
 label2->SetY (169 + 20);
 label2->SetWidth (60);
 label2->SetHeight (20);
 label2->SetEnable (1);
 label2->SetVisible (1);
 label2->SetText (lxT ("Pot. P2"));
 label2->SetAlign (1);
 Window1.CreateChild (label2);
 //label3
 label3 = new CLabel ();
 label3->SetFOwner (&Window1);
 label3->SetName (lxT ("label3_p4"));
 label3->SetX (12);
 label3->SetY (226 + 20);
 label3->SetWidth (60);
 label3->SetHeight (20);
 label3->SetEnable (1);
 label3->SetVisible (1);
 label3->SetText (lxT ("Heater"));
 label3->SetAlign (1);
 Window1.CreateChild (label3);
 //label4
 label4 = new CLabel ();
 label4->SetFOwner (&Window1);
 label4->SetName (lxT ("label4_p4"));
 label4->SetX (13);
 label4->SetY (277 + 20);
 label4->SetWidth (60);
 label4->SetHeight (20);
 label4->SetEnable (1);
 label4->SetVisible (1);
 label4->SetText (lxT ("Cooler"));
 label4->SetAlign (1);
 Window1.CreateChild (label4);
 //label5
 label5 = new CLabel ();
 label5->SetFOwner (&Window1);
 label5->SetName (lxT ("label5_p4"));
 label5->SetX (13);
 label5->SetY (332 + 20);
 label5->SetWidth (120);
 label5->SetHeight (24);
 label5->SetEnable (1);
 label5->SetVisible (1);
 label5->SetText (lxT ("Temp: 00.0°C"));
 label5->SetAlign (1);
 Window1.CreateChild (label5);
 //label6
 label6 = new CLabel ();
 label6->SetFOwner (&Window1);
 label6->SetName (lxT ("label6_p4"));
 label6->SetX (13);
 label6->SetY (54 + 20);
 label6->SetWidth (120);
 label6->SetHeight (24);
 label6->SetEnable (1);
 label6->SetVisible (1);
 label6->SetText (lxT ("LCD"));
 label6->SetAlign (1);
 Window1.CreateChild (label6);
 //combo1
 combo1 = new CCombo ();
 combo1->SetFOwner (&Window1);
 combo1->SetName (lxT ("combo1_p4"));
 combo1->SetX (13);
 combo1->SetY (78 + 20);
 combo1->SetWidth (130);
 combo1->SetHeight (24);
 combo1->SetEnable (1);
 combo1->SetVisible (1);
 combo1->SetText (lxT ("hd44780 16x2"));
 combo1->SetItems (lxT ("hd44780 16x2,hd44780 16x4,"));
 combo1->EvOnComboChange = EVONCOMBOCHANGE & CPWindow1::board_Event;
 Window1.CreateChild (combo1);

 strncpy (fname, (char*) lxGetUserDataDir (_T ("picsimlab")).char_str (), 1023);
 strncat (fname, "/mdump_PICGenios_EEPROM.bin", 1023);

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

cboard_PICGenios::~cboard_PICGenios(void)
{
 buzzer.BeepStop ();
 buzzer.End ();
 delete vent[0];
 delete vent[1];
 vent[0] = NULL;
 vent[1] = NULL;

 delete lcdbmp[0];
 delete lcdbmp[1];
 lcdbmp[0] = NULL;
 lcdbmp[1] = NULL;

 mi2c_end (&mi2c);
 rtc2_end (&rtc2);

 Window1.DestroyChild (scroll1);
 Window1.DestroyChild (scroll2);
 Window1.DestroyChild (gauge1);
 Window1.DestroyChild (gauge2);
 Window1.DestroyChild (label1);
 Window1.DestroyChild (label2);
 Window1.DestroyChild (label3);
 Window1.DestroyChild (label4);
 Window1.DestroyChild (label5);
 Window1.DestroyChild (label6);
 Window1.DestroyChild (combo1);

 unlink (mi2c_tmp_name);
}

void
cboard_PICGenios::MDumpMemory(const char * mfname)
{
 FILE * fout;
 char fname[1024];

 strncpy (fname, (char*) lxGetUserDataDir (_T ("picsimlab")).char_str (), 1023);
 strncat (fname, "/mdump_PICGenios_EEPROM.bin", 1023);

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

 board_picsim::MDumpMemory (mfname);
}

void
cboard_PICGenios::Draw(CDraw *draw, double scale)
{
 int i;

 pic_set_pin (39, 1);
 pic_set_pin (40, 1);

 pic_set_pin (19, 1);
 pic_set_pin (20, 1);
 pic_set_pin (21, 1);
 pic_set_pin (22, 1);
 pic_set_pin (27, 1);
 pic_set_pin (28, 1);
 pic_set_pin (29, 1);
 pic_set_pin (30, 1);


 draw->Canvas.Init (scale, scale);


 lcd_blink (&lcd);


 //lab4 draw 
 for (i = 0; i < outputc; i++)
  {
   if (!output[i].r)
    {

     draw->Canvas.SetFgColor (30, 0, 0);

     switch (output[i].id)
      {
      case O_A1: draw->Canvas.SetColor (0, lm1[18], 0);
       break;
      case O_B1: draw->Canvas.SetColor (0, lm1[19], 0);
       break;
      case O_C1: draw->Canvas.SetColor (0, lm1[20], 0);
       break;
      case O_D1: draw->Canvas.SetColor (0, lm1[21], 0);
       break;
      case O_E1: draw->Canvas.SetColor (0, lm1[26], 0);
       break;
      case O_F1: draw->Canvas.SetColor (0, lm1[27], 0);
       break;
      case O_G1: draw->Canvas.SetColor (0, lm1[28], 0);
       break;
      case O_P1: draw->Canvas.SetColor (0, lm1[29], 0);
       break;

      case O_A2: draw->Canvas.SetColor (0, lm2[18], 0);
       break;
      case O_B2: draw->Canvas.SetColor (0, lm2[19], 0);
       break;
      case O_C2: draw->Canvas.SetColor (0, lm2[20], 0);
       break;
      case O_D2: draw->Canvas.SetColor (0, lm2[21], 0);
       break;
      case O_E2: draw->Canvas.SetColor (0, lm2[26], 0);
       break;
      case O_F2: draw->Canvas.SetColor (0, lm2[27], 0);
       break;
      case O_G2: draw->Canvas.SetColor (0, lm2[28], 0);
       break;
      case O_P2: draw->Canvas.SetColor (0, lm2[29], 0);
       break;

      case O_A3: draw->Canvas.SetColor (0, lm3[18], 0);
       break;
      case O_B3: draw->Canvas.SetColor (0, lm3[19], 0);
       break;
      case O_C3: draw->Canvas.SetColor (0, lm3[20], 0);
       break;
      case O_D3: draw->Canvas.SetColor (0, lm3[21], 0);
       break;
      case O_E3: draw->Canvas.SetColor (0, lm3[26], 0);
       break;
      case O_F3: draw->Canvas.SetColor (0, lm3[27], 0);
       break;
      case O_G3: draw->Canvas.SetColor (0, lm3[28], 0);
       break;
      case O_P3: draw->Canvas.SetColor (0, lm3[29], 0);
       break;

      case O_A4: draw->Canvas.SetColor (0, lm4[18], 0);
       break;
      case O_B4: draw->Canvas.SetColor (0, lm4[19], 0);
       break;
      case O_C4: draw->Canvas.SetColor (0, lm4[20], 0);
       break;
      case O_D4: draw->Canvas.SetColor (0, lm4[21], 0);
       break;
      case O_E4: draw->Canvas.SetColor (0, lm4[26], 0);
       break;
      case O_F4: draw->Canvas.SetColor (0, lm4[27], 0);
       break;
      case O_G4: draw->Canvas.SetColor (0, lm4[28], 0);
       break;
      case O_P4: draw->Canvas.SetColor (0, lm4[29], 0);
       break;

      case O_RL1:
       if (dip[3])
        draw->Canvas.SetColor (0, pic.pins[14].oavalue, 0);
       else
        draw->Canvas.SetColor (0, 15, 0);
       break;
      case O_RL2:
       if (dip[4])
        draw->Canvas.SetColor (0, pic.pins[7].oavalue, 0);
       else
        draw->Canvas.SetColor (0, 15, 0);
       break;


      case O_LCD: draw->Canvas.SetColor (0, 90 * Window1.Get_mcupwr () + 40, 0);
       break;

      default:
       if ((output[i].name[0] == 'D')&&(output[i].name[1] == 'P'))
        {
         draw->Canvas.SetBgColor (250, 250, 250);
         break;
        }
      }
     if (output[i].id == O_JP1) draw->Canvas.SetColor (150, 150, 150);

     if ((output[i].id != O_LCD)&&(output[i].id != O_VT))
      draw->Canvas.Rectangle (1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);


     //draw cooler 
     if (output[i].id == O_VT)
      {
       if (gauge1->GetValue () > 20) vtc++;

       if (vtc > (4 - 0.04 * gauge1->GetValue ()))
        {
         vtc = 0;
         draw->Canvas.PutBitmap (vent[vt], output[i].x1, output[i].y1);
         vt ^= 1;
        }

      }

     if ((output[i].id == O_LCD)&&(lcd.update))
      {
       if (lcd.lnum == 2)
        draw->Canvas.PutBitmap (lcdbmp[0], output[i].x1 - 18, output[i].y1 - 48);
       else
        draw->Canvas.PutBitmap (lcdbmp[1], output[i].x1 - 18, output[i].y1 - 48);
      }


     //draw lcd text 
     if (dip[0])
      {
       if ((output[i].id == O_LCD)&&(lcd.update))
        {
         lcd_draw (&lcd, &draw->Canvas, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1, Window1.Get_mcupwr ());
        }
      }
     else if (output[i].id == O_LCD)
      {
       draw->Canvas.Rectangle (1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
      }

     if ((output[i].name[0] == 'D')&&(output[i].name[1] == 'P'))
      {
       if (dip[(((output[i].name[2] - 0x30)*10)+(output[i].name[3] - 0x30)) - 1 ])
        {
         draw->Canvas.SetBgColor (70, 70, 70);
         draw->Canvas.Rectangle (1, output[i].x1, output[i].y1 + ((int) ((output[i].y2 - output[i].y1)*0.35)), output[i].x2 - output[i].x1, (int) ((output[i].y2 - output[i].y1)*0.65));
        }
       else
        {
         draw->Canvas.SetBgColor (70, 70, 70);
         draw->Canvas.Rectangle (1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, (int) ((output[i].y2 - output[i].y1)*0.65));
        }
      }




     if (output[i].id == O_JP1)
      {
       if (!jmp[0])
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
        };
      }

    }
   else
    {
     draw->Canvas.SetFgColor (0, 0, 0);


     draw->Canvas.SetColor (25, 15, 15);

     if (dip[9])
      switch (output[i].id)
       {
       case O_RB0: draw->Canvas.SetColor (pic.pins[32].oavalue, 0, 0);
        break;
       case O_RB1: draw->Canvas.SetColor (pic.pins[33].oavalue, 0, 0);
        break;
       case O_RB2: draw->Canvas.SetColor (pic.pins[34].oavalue, 0, 0);
        break;
       case O_RB3: draw->Canvas.SetColor (pic.pins[35].oavalue, 0, 0);
        break;
       case O_RB4: draw->Canvas.SetColor (pic.pins[36].oavalue, 0, 0);
        break;
       case O_RB5: draw->Canvas.SetColor (pic.pins[37].oavalue, 0, 0);
        break;
       case O_RB6: draw->Canvas.SetColor (pic.pins[38].oavalue, 0, 0);
        break;
       case O_RB7: draw->Canvas.SetColor (pic.pins[39].oavalue, 0, 0);
        break;
       }


     if (dip[8])
      switch (output[i].id)
       {
       case O_RD0: draw->Canvas.SetColor (pic.pins[18].oavalue, 0, 0);
        break;
       case O_RD1: draw->Canvas.SetColor (pic.pins[19].oavalue, 0, 0);
        break;
       case O_RD2: draw->Canvas.SetColor (pic.pins[20].oavalue, 0, 0);
        break;
       case O_RD3: draw->Canvas.SetColor (pic.pins[21].oavalue, 0, 0);
        break;
       case O_RD4: draw->Canvas.SetColor (pic.pins[26].oavalue, 0, 0);
        break;
       case O_RD5: draw->Canvas.SetColor (pic.pins[27].oavalue, 0, 0);
        break;
       case O_RD6: draw->Canvas.SetColor (pic.pins[28].oavalue, 0, 0);
        break;
       case O_RD7: draw->Canvas.SetColor (pic.pins[29].oavalue, 0, 0);
        break;
       }

     if (output[i].id == O_LPWR)draw->Canvas.SetColor (0, 255 * Window1.Get_mcupwr (), 0);

     draw->Canvas.Circle (1, output[i].x1, output[i].y1, output[i].r);
    };

  };
 rtc2_update (&rtc2);
 //end draw

 draw->Canvas.End ();
 draw->Update ();


 if (((0.4444 * (pic.pins[15].oavalue - 30)) > 10)&&(Window1.Get_mcupwr ()) && jmp[0])
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



 //Ventilador
 gauge1->SetValue (0.4444 * (pic.pins[16].oavalue - 30));
 //Aquecedor
 gauge2->SetValue (0.4444 * (pic.pins[23].oavalue - 30));

 //sensor ventilador
 rpmstp = ((float) Window1.GetNSTEPJ ()) / (0.64 * (pic.pins[16].oavalue - 29));

 //tensão p2
 vp2in = ((5.0 * (scroll1->GetPosition ())) / (scroll1->GetRange () - 1));
 vp1in = ((5.0 * (scroll2->GetPosition ())) / (scroll2->GetRange () - 1));

 //temperatura 
 ref = ((0.2222 * (pic.pins[23].oavalue - 30)))-(0.2222 * (pic.pins[16].oavalue - 30));

 temp[1] = temp[0];
 temp[0] = ((27.5 + ref)*0.003) + temp[1]*(0.997);

 if (temp[0] < 27.5)
  temp[0] = 27.5;

 if (dip[16])
  pic_set_apin (4, temp[0] / 100.0);
 else
  pic_set_apin (4, 0);

 //referencia
 //pic_set_apin(pic,5,2.5);

}

void
cboard_PICGenios::Run_CPU(void)
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

 if (use_spare)Window5.PreProcess ();

 memset (alm, 0, 40 * sizeof (unsigned int));
 memset (alm1, 0, 40 * sizeof (unsigned int));
 memset (alm2, 0, 40 * sizeof (unsigned int));
 memset (alm3, 0, 40 * sizeof (unsigned int));
 memset (alm4, 0, 40 * sizeof (unsigned int));

 pins = pic.pins;


 j = JUMPSTEPS;

 if (Window1.Get_mcupwr ())
  for (i = 0; i < Window1.GetNSTEP (); i++)
   {

    if (j >= JUMPSTEPS)
     {

      pic_set_pin (pic.mclr, p_MCLR);

      pic_set_pin (33, p_BT1);
      pic_set_pin (34, p_BT2);
      pic_set_pin (35, p_BT3);
      pic_set_pin (36, p_BT4);
      pic_set_pin (37, p_BT5);
      pic_set_pin (38, p_BT6);
      pic_set_pin (7, p_BT7);

      pic_set_pin (39, 1);
      pic_set_pin (40, 1);

      /*
      pic_set_pin(19,1);
      pic_set_pin(20,1);
      pic_set_pin(21,1);
      pic_set_pin(22,1);
      pic_set_pin(27,1);
      pic_set_pin(28,1);
      pic_set_pin(29,1);
      pic_set_pin(30,1);
       */


      //keyboard 

      if (p_KEY1)
       {
        pic_set_pin (22, pic_get_pin (33));
        pic_set_pin (33, pic_get_pin (22));
       }

      if (p_KEY2)
       {
        pic_set_pin (22, pic_get_pin (34));
        pic_set_pin (34, pic_get_pin (22));
       }

      if (p_KEY3)
       {
        pic_set_pin (22, pic_get_pin (35));
        pic_set_pin (35, pic_get_pin (22));
       }

      if (p_KEY4)
       {
        pic_set_pin (21, pic_get_pin (33));
        pic_set_pin (33, pic_get_pin (21));
       }

      if (p_KEY5)
       {
        pic_set_pin (21, pic_get_pin (34));
        pic_set_pin (34, pic_get_pin (21));
       }

      if (p_KEY6)
       {
        pic_set_pin (21, pic_get_pin (35));
        pic_set_pin (35, pic_get_pin (21));
       }

      if (p_KEY7)
       {
        pic_set_pin (20, pic_get_pin (33));
        pic_set_pin (33, pic_get_pin (20));
       }

      if (p_KEY8)
       {
        pic_set_pin (20, pic_get_pin (34));
        pic_set_pin (34, pic_get_pin (20));
       }

      if (p_KEY9)
       {
        pic_set_pin (20, pic_get_pin (35));
        pic_set_pin (35, pic_get_pin (20));
       }

      if (p_KEY10)
       {
        pic_set_pin (19, pic_get_pin (33));
        pic_set_pin (33, pic_get_pin (19));
       }

      if (p_KEY11)
       {
        pic_set_pin (19, pic_get_pin (34));
        pic_set_pin (34, pic_get_pin (19));
       }

      if (p_KEY12)
       {
        pic_set_pin (19, pic_get_pin (35));
        pic_set_pin (35, pic_get_pin (19));
       }

      if (pic.pins[16].oavalue > 30)
       {
        rpmc++;
        if (rpmc > rpmstp)
         {
          rpmc = 0;
          if (dip[14])
           pic_set_pin (15, !pins[14].value);
         }
       }
      else
       pic_set_pin (15, 0);


     }

    if (!mplabxd_testbp ())pic_step ();

    if (use_oscope)Window4.SetSample ();
    if (use_spare)Window5.Process ();
    
    //increment mean value counter if pin is high 
    if (j < pic.PINCOUNT)
     alm[j] += pins[j].value;

    if (j >= JUMPSTEPS)
     {

      for (pi = 18; pi < 30; pi++)
       {
        pinv = pins[pi].value;
        if ((pinv)&&(pins[3].value)&&(dip[10])) alm1[pi]++;
        if ((pinv)&&(pins[4].value)&&(dip[11])) alm2[pi]++;
        if ((pinv)&&(pins[5].value)&&(dip[12])) alm3[pi]++;
        if ((pinv)&&(pins[6].value)&&(dip[13])) alm4[pi]++;
       }

      if (dip[7])alm[32] = 0;


      // potenciometro p1 e p2
      if (dip[18])pic_set_apin (2, vp1in);
      if (dip[19])pic_set_apin (3, vp2in);


      j = -1;
     }
    j++;

    //lcd dipins[2].display code

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

        if ((!pins[9].dir)&&(!pins[9].value))
         {
          lcd_cmd (&lcd, d);
         }
        else if ((!pins[9].dir)&&(pins[9].value))
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
    //end display code     

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
    pic_set_pin (23, mi2c_io (&mi2c, sck, sda) | rtc2_io (&rtc2, sck, sda));

   }

 //fim STEP

 if (!dip[15])alm[23] = 0; //aquecedor
 if (!dip[17])alm[16] = 0; //ventilador


 for (i = 0; i < pic.PINCOUNT; i++)
  {
   if (pic.pins[i].port == P_VDD)
    pic.pins[i].oavalue = 255;
   else
    pic.pins[i].oavalue = (int) (((225.0 * alm[i]) / NSTEPJ) + 30);

   lm1[i] = (int) (((600.0 * alm1[i]) / NSTEPJ) + 30);
   lm2[i] = (int) (((600.0 * alm2[i]) / NSTEPJ) + 30);
   lm3[i] = (int) (((600.0 * alm3[i]) / NSTEPJ) + 30);
   lm4[i] = (int) (((600.0 * alm4[i]) / NSTEPJ) + 30);
   if (lm1[i] > 255)lm1[i] = 255;
   if (lm2[i] > 255)lm2[i] = 255;
   if (lm3[i] > 255)lm3[i] = 255;
   if (lm4[i] > 255)lm4[i] = 255;
  }

 if (use_spare)Window5.PostProcess ();

}

void
cboard_PICGenios::Reset(void)
{

 pic_reset (1);

 lcd_rst (&lcd);
 mi2c_rst (&mi2c);
 rtc2_rst (&rtc2);

 p_BT1 = 1;
 p_BT2 = 1;
 p_BT3 = 1;
 p_BT4 = 1;
 p_BT5 = 1;
 p_BT6 = 1;
 p_BT7 = 1;

 pic_set_pin (33, p_BT1);
 pic_set_pin (34, p_BT2);
 pic_set_pin (35, p_BT3);
 pic_set_pin (36, p_BT4);
 pic_set_pin (37, p_BT5);
 pic_set_pin (38, p_BT6);
 pic_set_pin (7, p_BT7);
 p_KEY1 = 0;
 p_KEY2 = 0;
 p_KEY3 = 0;
 p_KEY4 = 0;
 p_KEY5 = 0;
 p_KEY6 = 0;
 p_KEY7 = 0;
 p_KEY8 = 0;
 p_KEY9 = 0;
 p_KEY10 = 0;
 p_KEY11 = 0;
 p_KEY12 = 0;
 pic_set_pin (33, 0);
 pic_set_pin (34, 0);
 pic_set_pin (35, 0);

 pic_set_pin (20, 0);
 pic_set_pin (21, 0);
 pic_set_pin (22, 0);
 pic_set_pin (23, 0);


#ifndef _WIN_
 if (pic.serial[0].serialfd > 0)
#else
 if (pic.serial[0].serialfd != INVALID_HANDLE_VALUE)
#endif
  Window1.statusbar1.SetField (2, lxT ("Serial: ") + lxString::FromAscii (SERIALDEVICE) + lxT (":") + itoa (pic.serial[0].serialbaud) + lxT ("(") + lxString ().Format ("%4.1f", fabs ((100.0 * pic.serial[0].serialexbaud - 100.0 * pic.serial[0].serialbaud) / pic.serial[0].serialexbaud)) + lxT ("%)"));
 else
  Window1.statusbar1.SetField (2, lxT ("Serial: ") + lxString::FromAscii (SERIALDEVICE) + lxT (" (ERROR)"));


 for (int i = 0; i < pic.PINCOUNT; i++)
  {
   lm1[i] = 0;
   lm2[i] = 0;
   lm3[i] = 0;
   lm4[i] = 0;
  }

 if (use_spare)Window5.Reset ();
}

void
cboard_PICGenios::EvMouseButtonPress(uint button, uint x, uint y, uint state)
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
       };
       break;


      case I_D01:
       {
        dip[0] ^= 0x01;
       };
       break;
      case I_D02:
       {
        dip[1] ^= 0x01;
       };
       break;
      case I_D03:
       {
        dip[2] ^= 0x01;
       };
       break;
      case I_D04:
       {
        dip[3] ^= 0x01;
       };
       break;
      case I_D05:
       {
        dip[4] ^= 0x01;
       };
       break;
      case I_D06:
       {
        dip[5] ^= 0x01;
       };
       break;
      case I_D07:
       {
        dip[6] ^= 0x01;
       };
       break;
      case I_D08:
       {
        dip[7] ^= 0x01;
       };
       break;
      case I_D09:
       {
        dip[8] ^= 0x01;
       };
       break;
      case I_D10:
       {
        dip[9] ^= 0x01;
       };
       break;
      case I_D11:
       {
        dip[10] ^= 0x01;
       };
       break;
      case I_D12:
       {
        dip[11] ^= 0x01;
       };
       break;
      case I_D13:
       {
        dip[12] ^= 0x01;
       };
       break;
      case I_D14:
       {
        dip[13] ^= 0x01;
       };
       break;
      case I_D15:
       {
        dip[14] ^= 0x01;
       };
       break;
      case I_D16:
       {
        dip[15] ^= 0x01;
       };
       break;
      case I_D17:
       {
        dip[16] ^= 0x01;
       };
       break;
      case I_D18:
       {
        dip[17] ^= 0x01;
       };
       break;
      case I_D19:
       {
        dip[18] ^= 0x01;
       };
       break;
      case I_D20:
       {
        dip[19] ^= 0x01;
       };
       break;

      case I_PWR:
       {
        if (Window1.Get_mcupwr ())
         {
          Window1.Set_mcurun (0);
          Window1.Set_mcupwr (0);
          Reset ();

          p_BT1 = 0;
          p_BT2 = 0;
          p_BT3 = 0;
          p_BT4 = 0;
          Window1.statusbar1.SetField (0, lxT ("Stoped"));
         }
        else
         {
          Window1.Set_mcupwr (1);
          Window1.Set_mcurun (1);
          Reset ();


          Window1.statusbar1.SetField (0, lxT ("Running..."));
         }
       };
       break;

      case I_RST:
       {
        if (Window1.Get_mcupwr () && pic_reset (-1))
         {
          Window1.Set_mcupwr (0);
          Window1.Set_mcurst (1);
         }
        p_MCLR = 0;
       };
       break;

      case I_JP1:
       {
        jmp[0] ^= 0x01;
       };
       break;
       /*
       case I_RA1:
         {
           p_BT1=0;
         };break;
       case I_RA2:
         {
           p_BT2=0; 
         };break;
       case I_RA3:
         {
           p_BT3=0; 
         };break;
       case I_RA4:
         {
           p_BT4=0; 
         };break;
        */
      case I_RA5:
       {
        p_BT7 = 0;
       };
       break;
      case I_RB0:
       {
        p_BT1 = 0;
       };
       break;
      case I_RB1:
       {
        p_BT2 = 0;
       };
       break;
      case I_RB2:
       {
        p_BT3 = 0;
       };
       break;
      case I_RB3:
       {
        p_BT4 = 0;
       };
       break;
      case I_RB4:
       {
        p_BT5 = 0;
       };
       break;
      case I_RB5:
       {
        p_BT6 = 0;
       };
       break;

      case I_TC1:
       {
        p_KEY1 = 1;
       };
       break;
      case I_TC2:
       {
        p_KEY2 = 1;
       };
       break;
      case I_TC3:
       {
        p_KEY3 = 1;
       };
       break;

      case I_TC4:
       {
        p_KEY4 = 1;
       };
       break;
      case I_TC5:
       {
        p_KEY5 = 1;
       };
       break;
      case I_TC6:
       {
        p_KEY6 = 1;
       };
       break;

      case I_TC7:
       {
        p_KEY7 = 1;
       };
       break;
      case I_TC8:
       {
        p_KEY8 = 1;
       };
       break;
      case I_TC9:
       {
        p_KEY9 = 1;
       };
       break;

      case I_TCA:
       {
        p_KEY10 = 1;
       };
       break;
      case I_TC0:
       {
        p_KEY11 = 1;
       };
       break;
      case I_TCT:
       {
        p_KEY12 = 1;
       };
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
cboard_PICGenios::EvMouseButtonRelease(uint button, uint x, uint y, uint state)
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
        p_MCLR = 1;
       };
       break;
       /*
         case I_RA1:
         {
           p_BT1=1; 
         };break;
         case I_RA2:
         {
           p_BT2=1; 
         };break;
         case I_RA3:
         {
           p_BT3=1; 
         };break;
         case I_RA4:
         {
           p_BT4=1; 
         };break;
        */
      case I_RA5:
       {
        p_BT7 = 1;
       };
       break;
      case I_RB0:
       {
        p_BT1 = 1;
       };
       break;
      case I_RB1:
       {
        p_BT2 = 1;
       };
       break;
      case I_RB2:
       {
        p_BT3 = 1;
       };
       break;
      case I_RB3:
       {
        p_BT4 = 1;
       };
       break;
      case I_RB4:
       {
        p_BT5 = 1;
       };
       break;
      case I_RB5:
       {
        p_BT6 = 1;
       };
       break;

      case I_TC1:
       {
        p_KEY1 = 0;
       };
       break;
      case I_TC2:
       {
        p_KEY2 = 0;
       };
       break;
      case I_TC3:
       {
        p_KEY3 = 0;
       };
       break;

      case I_TC4:
       {
        p_KEY4 = 0;
       };
       break;
      case I_TC5:
       {
        p_KEY5 = 0;
       };
       break;
      case I_TC6:
       {
        p_KEY6 = 0;
       };
       break;

      case I_TC7:
       {
        p_KEY7 = 0;
       };
       break;
      case I_TC8:
       {
        p_KEY8 = 0;
       };
       break;
      case I_TC9:
       {
        p_KEY9 = 0;
       };
       break;

      case I_TCA:
       {
        p_KEY10 = 0;
       };
       break;
      case I_TC0:
       {
        p_KEY11 = 0;
       };
       break;
      case I_TCT:
       {
        p_KEY12 = 0;
       };
       break;
      }
    }
  }

}

void
cboard_PICGenios::EvKeyPress(uint key, uint mask)
{
 if (key == '1')
  {
   p_KEY1 = 1;
  }
 if (key == '2')
  {
   p_KEY2 = 1;
  }
 if (key == '3')
  {
   p_KEY3 = 1;
  }

 if (key == '4')
  {
   p_KEY4 = 1;
  }
 if (key == '5')
  {
   p_KEY5 = 1;
  }
 if (key == '6')
  {
   p_KEY6 = 1;
  }

 if (key == '7')
  {
   p_KEY7 = 1;
  }
 if (key == '8')
  {
   p_KEY8 = 1;
  }
 if (key == '9')
  {
   p_KEY9 = 1;
  }

 if (key == '*')
  {
   p_KEY10 = 1;
  }
 if (key == '0')
  {
   p_KEY11 = 1;
  }
 if (key == '#')
  {
   p_KEY12 = 1;
  }

 if (key == 'Q')
  {
   p_BT1 = 0;
  }
 if (key == 'W')
  {
   p_BT2 = 0;
  }
 if (key == 'E')
  {
   p_BT3 = 0;
  }
 if (key == 'R')
  {
   p_BT4 = 0;
  }
 if (key == 'T')
  {
   p_BT5 = 0;
  }
 if (key == 'Y')
  {
   p_BT6 = 0;
  }
}

void
cboard_PICGenios::EvKeyRelease(uint key, uint mask)
{

 if (key == '1')
  {
   p_KEY1 = 0;
  }
 if (key == '2')
  {
   p_KEY2 = 0;
  }
 if (key == '3')
  {
   p_KEY3 = 0;
  }

 if (key == '4')
  {
   p_KEY4 = 0;
  }
 if (key == '5')
  {
   p_KEY5 = 0;
  }
 if (key == '6')
  {
   p_KEY6 = 0;
  }

 if (key == '7')
  {
   p_KEY7 = 0;
  }
 if (key == '8')
  {
   p_KEY8 = 0;
  }
 if (key == '9')
  {
   p_KEY9 = 0;
  }

 if (key == '*')
  {
   p_KEY10 = 0;
  }
 if (key == '0')
  {
   p_KEY11 = 0;
  }
 if (key == '#')
  {
   p_KEY12 = 0;
  }

 if (key == 'Q')
  {
   p_BT1 = 1;
  }
 if (key == 'W')
  {
   p_BT2 = 1;
  }
 if (key == 'E')
  {
   p_BT3 = 1;
  }
 if (key == 'R')
  {
   p_BT4 = 1;
  }
 if (key == 'T')
  {
   p_BT5 = 1;
  }
 if (key == 'Y')
  {
   p_BT6 = 1;
  }
}

void
cboard_PICGenios::EvOnShow(void)
{
 lcd.update = 1;
}

unsigned short
cboard_PICGenios::get_in_id(char * name)
{
 if (strcmp (name, "RST") == 0)return I_RST;
 if (strcmp (name, "RA1") == 0)return I_RA1;
 if (strcmp (name, "RA2") == 0)return I_RA2;
 if (strcmp (name, "RA3") == 0)return I_RA3;
 if (strcmp (name, "RA4") == 0)return I_RA4;
 if (strcmp (name, "RA5") == 0)return I_RA5;
 if (strcmp (name, "PWR") == 0)return I_PWR;
 if (strcmp (name, "ICSP") == 0)return I_ICSP;

 if (strcmp (name, "RB0") == 0)return I_RB0;
 if (strcmp (name, "RB1") == 0)return I_RB1;
 if (strcmp (name, "RB2") == 0)return I_RB2;
 if (strcmp (name, "RB3") == 0)return I_RB3;
 if (strcmp (name, "RB4") == 0)return I_RB4;
 if (strcmp (name, "RB5") == 0)return I_RB5;

 if (strcmp (name, "TC1") == 0)return I_TC1;
 if (strcmp (name, "TC2") == 0)return I_TC2;
 if (strcmp (name, "TC3") == 0)return I_TC3;
 if (strcmp (name, "TC4") == 0)return I_TC4;
 if (strcmp (name, "TC5") == 0)return I_TC5;
 if (strcmp (name, "TC6") == 0)return I_TC6;
 if (strcmp (name, "TC7") == 0)return I_TC7;
 if (strcmp (name, "TC8") == 0)return I_TC8;
 if (strcmp (name, "TC9") == 0)return I_TC9;
 if (strcmp (name, "TC*") == 0)return I_TCA;
 if (strcmp (name, "TC0") == 0)return I_TC0;
 if (strcmp (name, "TC#") == 0)return I_TCT;

 if (strcmp (name, "DP01") == 0)return I_D01;
 if (strcmp (name, "DP02") == 0)return I_D02;
 if (strcmp (name, "DP03") == 0)return I_D03;
 if (strcmp (name, "DP04") == 0)return I_D04;
 if (strcmp (name, "DP05") == 0)return I_D05;
 if (strcmp (name, "DP06") == 0)return I_D06;
 if (strcmp (name, "DP07") == 0)return I_D07;
 if (strcmp (name, "DP08") == 0)return I_D08;
 if (strcmp (name, "DP09") == 0)return I_D09;
 if (strcmp (name, "DP10") == 0)return I_D10;
 if (strcmp (name, "DP11") == 0)return I_D11;
 if (strcmp (name, "DP12") == 0)return I_D12;
 if (strcmp (name, "DP13") == 0)return I_D13;
 if (strcmp (name, "DP14") == 0)return I_D14;
 if (strcmp (name, "DP15") == 0)return I_D15;
 if (strcmp (name, "DP16") == 0)return I_D16;
 if (strcmp (name, "DP17") == 0)return I_D17;
 if (strcmp (name, "DP18") == 0)return I_D18;
 if (strcmp (name, "DP19") == 0)return I_D19;
 if (strcmp (name, "DP20") == 0)return I_D20;

 if (strcmp (name, "JP1") == 0)return I_JP1;
 if (strcmp (name, "VIEW") == 0)return I_VIEW;

 printf ("Erro input '%s' don't have a valid id! \n", name);
 return -1;
};

unsigned short
cboard_PICGenios::get_out_id(char * name)
{

 if (strcmp (name, "RB0") == 0)return O_RB0;
 if (strcmp (name, "RB1") == 0)return O_RB1;
 if (strcmp (name, "RB2") == 0)return O_RB2;
 if (strcmp (name, "RB3") == 0)return O_RB3;
 if (strcmp (name, "RB4") == 0)return O_RB4;
 if (strcmp (name, "RB5") == 0)return O_RB5;
 if (strcmp (name, "RB6") == 0)return O_RB6;
 if (strcmp (name, "RB7") == 0)return O_RB7;

 if (strcmp (name, "RD0") == 0)return O_RD0;
 if (strcmp (name, "RD1") == 0)return O_RD1;
 if (strcmp (name, "RD2") == 0)return O_RD2;
 if (strcmp (name, "RD3") == 0)return O_RD3;
 if (strcmp (name, "RD4") == 0)return O_RD4;
 if (strcmp (name, "RD5") == 0)return O_RD5;
 if (strcmp (name, "RD6") == 0)return O_RD6;
 if (strcmp (name, "RD7") == 0)return O_RD7;

 if (strcmp (name, "RL1") == 0)return O_RL1;
 if (strcmp (name, "RL2") == 0)return O_RL2;

 if (strcmp (name, "PRG") == 0)return O_PRG;
 if (strcmp (name, "RUN") == 0)return O_RUN;

 if (strcmp (name, "A1") == 0)return O_A1;
 if (strcmp (name, "B1") == 0)return O_B1;
 if (strcmp (name, "C1") == 0)return O_C1;
 if (strcmp (name, "D1") == 0)return O_D1;
 if (strcmp (name, "E1") == 0)return O_E1;
 if (strcmp (name, "F1") == 0)return O_F1;
 if (strcmp (name, "G1") == 0)return O_G1;
 if (strcmp (name, "P1") == 0)return O_P1;

 if (strcmp (name, "A2") == 0)return O_A2;
 if (strcmp (name, "B2") == 0)return O_B2;
 if (strcmp (name, "C2") == 0)return O_C2;
 if (strcmp (name, "D2") == 0)return O_D2;
 if (strcmp (name, "E2") == 0)return O_E2;
 if (strcmp (name, "F2") == 0)return O_F2;
 if (strcmp (name, "G2") == 0)return O_G2;
 if (strcmp (name, "P2") == 0)return O_P2;

 if (strcmp (name, "LPWR") == 0)return O_LPWR;
 if (strcmp (name, "LCD") == 0)return O_LCD;

 if (strcmp (name, "A3") == 0)return O_A3;
 if (strcmp (name, "B3") == 0)return O_B3;
 if (strcmp (name, "C3") == 0)return O_C3;
 if (strcmp (name, "D3") == 0)return O_D3;
 if (strcmp (name, "E3") == 0)return O_E3;
 if (strcmp (name, "F3") == 0)return O_F3;
 if (strcmp (name, "G3") == 0)return O_G3;
 if (strcmp (name, "P3") == 0)return O_P3;

 if (strcmp (name, "A4") == 0)return O_A4;
 if (strcmp (name, "B4") == 0)return O_B4;
 if (strcmp (name, "C4") == 0)return O_C4;
 if (strcmp (name, "D4") == 0)return O_D4;
 if (strcmp (name, "E4") == 0)return O_E4;
 if (strcmp (name, "F4") == 0)return O_F4;
 if (strcmp (name, "G4") == 0)return O_G4;
 if (strcmp (name, "P4") == 0)return O_P4;


 if (strcmp (name, "DP01") == 0)return O_D01;
 if (strcmp (name, "DP02") == 0)return O_D02;
 if (strcmp (name, "DP03") == 0)return O_D03;
 if (strcmp (name, "DP04") == 0)return O_D04;
 if (strcmp (name, "DP05") == 0)return O_D05;
 if (strcmp (name, "DP06") == 0)return O_D06;
 if (strcmp (name, "DP07") == 0)return O_D07;
 if (strcmp (name, "DP08") == 0)return O_D08;
 if (strcmp (name, "DP09") == 0)return O_D09;
 if (strcmp (name, "DP10") == 0)return O_D10;
 if (strcmp (name, "DP11") == 0)return O_D11;
 if (strcmp (name, "DP12") == 0)return O_D12;
 if (strcmp (name, "DP13") == 0)return O_D13;
 if (strcmp (name, "DP14") == 0)return O_D14;
 if (strcmp (name, "DP15") == 0)return O_D15;
 if (strcmp (name, "DP16") == 0)return O_D16;
 if (strcmp (name, "DP17") == 0)return O_D17;
 if (strcmp (name, "DP18") == 0)return O_D18;
 if (strcmp (name, "DP19") == 0)return O_D19;
 if (strcmp (name, "DP20") == 0)return O_D20;
 if (strcmp (name, "VT") == 0) return O_VT;

 if (strcmp (name, "JP1") == 0)return O_JP1;

 printf ("Erro output '%s' don't have a valid id! \n", name);
 return 1;
};

void
cboard_PICGenios::RefreshStatus(void)
{
 label5->SetText (lxT ("Temp: ") + lxString ().Format ("%5.2f", temp[0]) + lxT ("°C"));

#ifndef _WIN_
 if (pic.serial[0].serialfd > 0)
#else
 if (pic.serial[0].serialfd != INVALID_HANDLE_VALUE)
#endif
  Window1.statusbar1.SetField (2, lxT ("Serial: ") + lxString::FromAscii (SERIALDEVICE) + lxT (":") + itoa (pic.serial[0].serialbaud) + lxT ("(") + lxString ().Format ("%4.1f", fabs ((100.0 * pic.serial[0].serialexbaud - 100.0 * pic.serial[0].serialbaud) / pic.serial[0].serialexbaud)) + lxT ("%)"));
 else
  Window1.statusbar1.SetField (2, lxT ("Serial: ") + lxString::FromAscii (SERIALDEVICE) + lxT (" (ERROR)"));

};

void
cboard_PICGenios::WritePreferences(void)
{
 char line[100];
 Window1.saveprefs (lxT ("PICGenios_proc"), Proc);

 Window1.saveprefs (lxT ("PICGenios_jmp"), lxString ().Format ("%i", jmp[0]));

 Window1.saveprefs (lxT ("PICGenios_lcd"), combo1->GetText ());

 line[0] = 0;
 for (int i = 0; i < 20; i++)
  sprintf (line + i, "%i", dip[i]);

 Window1.saveprefs (lxT ("PICGenios_dip"), line);
 Window1.saveprefs (lxT ("PICGenios_clock"), lxString ().Format ("%2.1f", Window1.GetClock()));
}

void
cboard_PICGenios::ReadPreferences(char *name, char *value)
{
 int i;

 if (!strcmp (name, "PICGenios_proc"))
  {
   Proc = value;
  }

 if (!strcmp (name, "PICGenios_jmp"))
  {
   for (i = 0; i < 1; i++)
    {
     if (value[i] == '0')
      jmp[i] = 0;
     else
      jmp[i] = 1;
    }
  }


 if (!strcmp (name, "PICGenios_dip"))
  {
   for (i = 0; i < 20; i++)
    {
     if (value[i] == '0')
      dip[i] = 0;
     else
      dip[i] = 1;
    }
  }

 if (!strcmp (name, "PICGenios_lcd"))
  {
   combo1->SetText (value);
   if (combo1->GetText ().Cmp (lxT ("hd44780 16x2")) == 0)
    lcd_init (&lcd, 16, 2);
   else
    lcd_init (&lcd, 16, 4);
  }
 
  if (!strcmp (name, "PICGenios_clock"))
  {
   Window1.SetClock (atof(value));
  }
}

//Change lcd

void
cboard_PICGenios::board_Event(CControl * control)
{

 if (combo1->GetText ().Cmp (lxT ("hd44780 16x2")) == 0)
  lcd_init (&lcd, 16, 2);
 else if (combo1->GetText ().Cmp (lxT ("hd44780 16x4")) == 0)
  lcd_init (&lcd, 16, 4);
 else if (combo1->GetText ().Cmp (lxT ("hd44780 20x2")) == 0)
  lcd_init (&lcd, 20, 2);
 else if (combo1->GetText ().Cmp (lxT ("hd44780 20x4")) == 0)
  lcd_init (&lcd, 20, 4);
}

board_init("PICGenios", cboard_PICGenios);


