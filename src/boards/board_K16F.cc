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
#include"board_K16F.h"

#ifdef __EMSCRIPTEN__
#include<emscripten.h>
#endif

/* outputs */
enum
{
 O_RST,
 O_RA1,
 O_RA2,
 O_RA6,
 O_RA7,
 O_LPWR,
 O_LCD,
 O_MP
};

/*inputs*/
enum
{
 I_RST,
 I_PWR,
 I_ICSP,
 I_TC1,
 I_TC2,
 I_TC3,
 I_TC4,
 I_TC5,
 I_TC6,
 I_TC7,
 I_TC8,
 I_TC9,
 I_TCA,
 I_TC0,
 I_TCT,
 I_VIEW
};

cboard_K16F::cboard_K16F(void) :
font(10, lxFONTFAMILY_TELETYPE, lxFONTSTYLE_NORMAL, lxFONTWEIGHT_BOLD)
{
 char fname[1024];
 FILE * fout;

 Proc = "PIC16F628A";

 clko = 0;
 d = 0;
 lcde = 0;

 lcd_init (&lcd, 16, 2);
 mi2c_init (&mi2c, 512);
 rtc_pfc8563_init (&rtc);
 ReadMaps ();

 strncpy (fname, (char*) lxGetUserDataDir (lxT ("picsimlab")).char_str (), 1023);
 strncat (fname, "/mdump_K16F_EEPROM.bin", 1023);

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

cboard_K16F::~cboard_K16F(void)
{
 mi2c_end (&mi2c);
 rtc_pfc8563_end (&rtc);
 unlink (mi2c_tmp_name);
}

void
cboard_K16F::MDumpMemory(const char * mfname)
{
 FILE * fout;
 char fname[1024];

 strncpy (fname, (char*) lxGetUserDataDir (lxT ("picsimlab")).char_str (), 1023);
 strncat (fname, "/mdump_K16F_EEPROM.bin", 1023);

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
cboard_K16F::Draw(CDraw *draw)
{
 int i;
 int update = 0; //verifiy if updated is needed

 lcd_blink (&lcd);

 //lab2 draw 
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

       draw->Canvas.SetFgColor (30, 0, 0);

       switch (output[i].id)
        {
        case O_LCD: draw->Canvas.SetColor (0, 90 * Window1.Get_mcupwr () + 40, 0);
         break;
        case O_RST:
         draw->Canvas.SetColor (100, 100, 100);
         break;
        case O_MP:
         draw->Canvas.SetColor (26, 26, 26);
         break;
        }

       //draw lcd text 

       if (output[i].id == O_LCD)
        {
         if (lcd.update)
          {
           draw->Canvas.Rectangle (1, output[i].x1 - 1, output[i].y1 - 1, output[i].x2 - output[i].x1 + 2, output[i].y2 - output[i].y1 + 3);
           lcd_draw (&lcd, &draw->Canvas, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1, Window1.Get_mcupwr ());
          }
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
       else if (output[i].id == O_MP)
        {
         draw->Canvas.SetFont (font);
         draw->Canvas.Rectangle (1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
         draw->Canvas.SetColor (230, 230, 230);
         draw->Canvas.RotatedText (Proc, output[i].x2, output[i].y1 + 5, -90);
        }
       else
        {
         draw->Canvas.Rectangle (1, output[i].x1, output[i].y1, output[i].x2 - output[i].x1, output[i].y2 - output[i].y1);
        }
      }
     else //circle
      {
       draw->Canvas.SetFgColor (0, 0, 0);


       switch (output[i].id)
        {
        case O_RA1: draw->Canvas.SetBgColor (pic.pins[17].oavalue, 0, 0);
         break;
        case O_RA2: draw->Canvas.SetBgColor (pic.pins[0].oavalue, 0, 0);
         break;
        case O_RA6: draw->Canvas.SetBgColor (pic.pins[14].oavalue, 0, 0);
         break;
        case O_RA7: draw->Canvas.SetBgColor (pic.pins[15].oavalue, 0, 0);
         break;
        case O_LPWR: draw->Canvas.SetBgColor (0, 200 * Window1.Get_mcupwr () + 55, 0);
         break;
        }

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
    }
  }

 //end draw
 if (update)
  {
   draw->Canvas.End ();
   draw->Update ();
  }

 rtc_pfc8563_update (&rtc);
}

void
cboard_K16F::Run_CPU(void)
{
 int i;
 int j;
 unsigned char pi;
 const picpin * pins;
 unsigned int alm[18]; //luminosidade media


 int JUMPSTEPS = Window1.GetJUMPSTEPS ();
 long int NSTEP = Window1.GetNSTEP () / pic.PINCOUNT;

 pins = pic.pins;

 memset (alm, 0, 18 * sizeof (unsigned int));

 if (use_spare)Window5.PreProcess ();

 j = JUMPSTEPS;
 if (Window1.Get_mcupwr ())
  for (i = 0; i < Window1.GetNSTEP (); i++)
   {
    if (j >= JUMPSTEPS)
     {
      pic_set_pin (pic.mclr, p_RST);

      pic_set_pin (18, 0);
      pic_set_pin (1, 0);
      pic_set_pin (15, 0);
      pic_set_pin (16, 0);
      pic_set_pin (13, 0);
      pic_set_pin (12, 0);
      pic_set_pin (11, 0);

     }

    //keyboard 

    if (p_KEY1)
     {
      pic_set_pin (18, pic_get_pin (13));
      pic_set_pin (13, pic_get_pin (18));
     }


    if (p_KEY2)
     {
      pic_set_pin (18, pic_get_pin (12));
      pic_set_pin (12, pic_get_pin (18));
     }


    if (p_KEY3)
     {
      pic_set_pin (18, pic_get_pin (11));
      pic_set_pin (11, pic_get_pin (18));
     }


    if (p_KEY4)
     {
      pic_set_pin (1, pic_get_pin (13));
      pic_set_pin (13, pic_get_pin (1));
     }


    if (p_KEY5)
     {
      pic_set_pin (1, pic_get_pin (12));
      pic_set_pin (12, pic_get_pin (1));
     }


    if (p_KEY6)
     {
      pic_set_pin (1, pic_get_pin (11));
      pic_set_pin (11, pic_get_pin (1));
     }


    if (p_KEY7)
     {
      pic_set_pin (15, pic_get_pin (13));
      pic_set_pin (13, pic_get_pin (15));
     }


    if (p_KEY8)
     {
      pic_set_pin (15, pic_get_pin (12));
      pic_set_pin (12, pic_get_pin (15));
     }


    if (p_KEY9)
     {
      pic_set_pin (15, pic_get_pin (11));
      pic_set_pin (11, pic_get_pin (15));
     }


    if (p_KEY10)
     {
      pic_set_pin (16, pic_get_pin (13));
      pic_set_pin (13, pic_get_pin (16));
     }


    if (p_KEY11)
     {
      pic_set_pin (16, pic_get_pin (12));
      pic_set_pin (12, pic_get_pin (16));
     }


    if (p_KEY12)
     {
      pic_set_pin (16, pic_get_pin (11));
      pic_set_pin (11, pic_get_pin (16));
     }




    if (!mplabxd_testbp ())pic_step ();
    if (use_oscope)Window4.SetSample ();
    if (use_spare)Window5.Process ();

    //increment mean value counter if pin is high
    alm[i % pic.PINCOUNT] += pins[i % pic.PINCOUNT].value;

    if (j >= JUMPSTEPS)
     {
      j = -1;
     }
    j++;

    //serial lcd display code
    if ((pins[9].value)&&(!clko))
     {
      d = (d << 1) | pins[8].value;
     }

    clko = pins[9].value;

    if ((!pins[16].dir)&&(!pins[16].value))
     {
      if (!lcde)
       {

        if ((!pins[8].dir)&&(!pins[8].value))
         {
          lcd_cmd (&lcd, d);
         }
        else if ((!pins[8].dir)&&(pins[8].value))
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
    if (pins[2].dir)
     {
      sda = 1;
     }
    else
     {
      sda = pins[2].value;
     }

    if (pins[1].dir)
     {
      sck = 1;
      pic_set_pin (2, 1);
     }
    else
     {
      sck = pins[1].value;
     }
    pic_set_pin (3, mi2c_io (&mi2c, sck, sda) | rtc_pfc8563_I2C_io (&rtc, sck, sda));
   }
 //fim STEP


 for (pi = 0; pi < pic.PINCOUNT; pi++)
  {
   pic.pins[pi].oavalue = (int) (((200.0 * alm[pi]) / NSTEP) + 55);
  }

 if (use_spare)Window5.PostProcess ();

 if (lcd.update) output_ids[O_LCD]->update = 1;

 //verifiy if LEDS need update 
 if (output_ids[O_RA1]->value != pic.pins[17].oavalue)
  {
   output_ids[O_RA1]->value = pic.pins[17].oavalue;
   output_ids[O_RA1]->update = 1;
  }
 if (output_ids[O_RA2]->value != pic.pins[0].oavalue)
  {
   output_ids[O_RA2]->value = pic.pins[0].oavalue;
   output_ids[O_RA2]->update = 1;
  }
 if (output_ids[O_RA6]->value != pic.pins[14].oavalue)
  {
   output_ids[O_RA6]->value = pic.pins[14].oavalue;
   output_ids[O_RA6]->update = 1;
  }
 if (output_ids[O_RA7]->value != pic.pins[15].oavalue)
  {
   output_ids[O_RA7]->value = pic.pins[15].oavalue;
   output_ids[O_RA7]->update = 1;
  }
}

void
cboard_K16F::Reset(void)
{

 pic_reset (1);

 lcd_rst (&lcd);
 mi2c_rst (&mi2c);
 rtc_pfc8563_rst (&rtc);
 pic_set_pin_DOV (18, 0);
 pic_set_pin_DOV (1, 0);
 pic_set_pin_DOV (15, 0);
 pic_set_pin_DOV (16, 0);


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

 pic_set_pin (13, 0);
 pic_set_pin (12, 0);
 pic_set_pin (11, 0);

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
   pic.pins[pi].oavalue = 0;
  }

 if (use_spare)Window5.Reset ();

 RegisterRemoteControl ();
}

void
cboard_K16F::RegisterRemoteControl(void)
{

 for (int i = 0; i < inputc; i++)
  {
   switch (input[i].id)
    {
    case I_TC1:
     input[i].status = &p_KEY1;
     break;
    case I_TC2:
     input[i].status = &p_KEY2;
     break;
    case I_TC3:
     input[i].status = &p_KEY3;
     break;
    case I_TC4:
     input[i].status = &p_KEY4;
     break;
    case I_TC5:
     input[i].status = &p_KEY5;
     break;
    case I_TC6:
     input[i].status = &p_KEY6;
     break;
    case I_TC7:
     input[i].status = &p_KEY7;
     break;
    case I_TC8:
     input[i].status = &p_KEY8;
     break;
    case I_TC9:
     input[i].status = &p_KEY9;
     break;
    case I_TCA:
     input[i].status = &p_KEY10;
     break;
    case I_TC0:
     input[i].status = &p_KEY11;
     break;
    case I_TCT:
     input[i].status = &p_KEY12;
     break;
    }
  }

 for (int i = 0; i < outputc; i++)
  {
   switch (output[i].id)
    {

    case O_RA1:
     output[i].status = &pic.pins[17].oavalue;
     break;
    case O_RA2:
     output[i].status = &pic.pins[0].oavalue;
     break;
    case O_RA6:
     output[i].status = &pic.pins[14].oavalue;
     break;
    case O_RA7:
     output[i].status = &pic.pins[15].oavalue;
     break;
    case O_LCD:
     output[i].status = &lcd;
     break;
    }
  }
}

void
cboard_K16F::EvMouseButtonPress(uint button, uint x, uint y, uint state)
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

      case I_PWR:
       {
        if (Window1.Get_mcupwr ())
         {
          Window1.Set_mcurun (0);
          Window1.Set_mcupwr (0);
          Reset ();

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
      case I_TC1:
       {
        p_KEY1 = 1;
       }
       break;
      case I_TC2:
       {
        p_KEY2 = 1;
       }
       break;
      case I_TC3:
       {
        p_KEY3 = 1;
       }
       break;

      case I_TC4:
       {
        p_KEY4 = 1;
       }
       break;
      case I_TC5:
       {
        p_KEY5 = 1;
       }
       break;
      case I_TC6:
       {
        p_KEY6 = 1;
       }
       break;

      case I_TC7:
       {
        p_KEY7 = 1;
       }
       break;
      case I_TC8:
       {
        p_KEY8 = 1;
       }
       break;
      case I_TC9:
       {
        p_KEY9 = 1;
       }
       break;

      case I_TCA:
       {
        p_KEY10 = 1;
       }
       break;
      case I_TC0:
       {
        p_KEY11 = 1;
       }
       break;
      case I_TCT:
       {
        p_KEY12 = 1;
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
cboard_K16F::EvMouseButtonRelease(uint button, uint x, uint y, uint state)
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

      case I_TC1:
       {
        p_KEY1 = 0;
       }
       break;
      case I_TC4:
       {
        p_KEY4 = 0;
       }
       break;
      case I_TC7:
       {
        p_KEY7 = 0;
       }
       break;
      case I_TCA:
       {
        p_KEY10 = 0;
       }
       break;

      case I_TC2:
       {
        p_KEY2 = 0;
       }
       break;

      case I_TC5:
       {
        p_KEY5 = 0;
       }
       break;

      case I_TC8:
       {
        p_KEY8 = 0;
       }
       break;

      case I_TC0:
       {
        p_KEY11 = 0;
       }
       break;

      case I_TC3:
       {
        p_KEY3 = 0;
       }
       break;
      case I_TC6:
       {
        p_KEY6 = 0;
       }
      case I_TC9:
       {
        p_KEY9 = 0;
       }
      case I_TCT:
       {
        p_KEY12 = 0;
       }
       break;
      }
    }
  }
}

void
cboard_K16F::EvKeyPress(uint key, uint mask)
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

}

void
cboard_K16F::EvKeyRelease(uint key, uint mask)
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
}

void
cboard_K16F::EvOnShow(void)
{
 lcd.update = 1;
 board::EvOnShow ();
}

unsigned short
cboard_K16F::get_in_id(char * name)
{
 if (strcmp (name, "PB_RST") == 0)return I_RST;
 if (strcmp (name, "SW_PWR") == 0)return I_PWR;
 if (strcmp (name, "PG_ICSP") == 0)return I_ICSP;
 if (strcmp (name, "KB_TC1") == 0)return I_TC1;
 if (strcmp (name, "KB_TC2") == 0)return I_TC2;
 if (strcmp (name, "KB_TC3") == 0)return I_TC3;
 if (strcmp (name, "KB_TC4") == 0)return I_TC4;
 if (strcmp (name, "KB_TC5") == 0)return I_TC5;
 if (strcmp (name, "KB_TC6") == 0)return I_TC6;
 if (strcmp (name, "KB_TC7") == 0)return I_TC7;
 if (strcmp (name, "KB_TC8") == 0)return I_TC8;
 if (strcmp (name, "KB_TC9") == 0)return I_TC9;
 if (strcmp (name, "KB_TC*") == 0)return I_TCA;
 if (strcmp (name, "KB_TC0") == 0)return I_TC0;
 if (strcmp (name, "KB_TC#") == 0)return I_TCT;

 if (strcmp (name, "MD_VIEW") == 0)return I_VIEW;

 printf ("Erro input '%s' don't have a valid id! \n", name);
 return -1;
}

unsigned short
cboard_K16F::get_out_id(char * name)
{

 if (strcmp (name, "LD_RA1") == 0)return O_RA1;
 if (strcmp (name, "LD_RA2") == 0)return O_RA2;
 if (strcmp (name, "LD_RA6") == 0)return O_RA6;
 if (strcmp (name, "LD_RA7") == 0)return O_RA7;
 if (strcmp (name, "LD_LPWR") == 0)return O_LPWR;
 if (strcmp (name, "DS_LCD") == 0)return O_LCD;
 if (strcmp (name, "PB_RST") == 0)return O_RST;

 if (strcmp (name, "MP_CPU") == 0)return O_MP;

 printf ("Erro output '%s' don't have a valid id! \n", name);
 return 1;
}

void
cboard_K16F::WritePreferences(void)
{

 Window1.saveprefs (lxT ("K16F_proc"), Proc);
 Window1.saveprefs (lxT ("K16F_clock"), lxString ().Format ("%2.1f", Window1.GetClock ()));
}

void
cboard_K16F::ReadPreferences(char *name, char *value)
{

 if (!strcmp (name, "K16F_proc"))
  {
   Proc = value;
  }

 if (!strcmp (name, "K16F_clock"))
  {

   Window1.SetClock (atof (value));
  }
}

void
cboard_K16F::RefreshStatus(void)
{

#ifndef _WIN_
 if (pic.serial[0].serialfd > 0)
#else
 if (pic.serial[0].serialfd != INVALID_HANDLE_VALUE)
#endif
  Window1.statusbar1.SetField (2, lxT ("Serial: ") + lxString::FromAscii (SERIALDEVICE) + lxT (":") + itoa (pic.serial[0].serialbaud) + lxT ("(") + lxString ().Format ("%4.1f", fabs ((100.0 * pic.serial[0].serialexbaud - 100.0 * pic.serial[0].serialbaud) / pic.serial[0].serialexbaud)) + lxT ("%)"));
 else
  Window1.statusbar1.SetField (2, lxT ("Serial: ") + lxString::FromAscii (SERIALDEVICE) + lxT (" (ERROR)"));

}


board_init(BOARD_K16F_Name, cboard_K16F);
