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


#define dprint if (1) {} else printf

#include"lcd_pcf8833.h"

// Philips PCF8833 LCD controller command codes
#define NOP      0x00 // nop
#define SWRESET  0x01 // software reset
#define BSTROFF  0x02 // booster voltage OFF
#define BSTRON   0x03 // booster voltage ON
#define RDDIDIF  0x04 // read display identification
#define RDDST    0x09 // read display status
#define SLEEPIN  0x10 // sleep in
#define SLEEPOUT 0x11 // sleep out
#define PTLON    0x12 // partial display mode
#define NORON    0x13 // display normal mode
#define INVOFF   0x20 // inversion OFF
#define INVON    0x21 // inversion ON
#define DALO     0x22 // all pixel OFF
#define DAL      0x23 // all pixel ON
#define SETCON   0x25 // write contrast
#define DISPOFF  0x28 // display OFF
#define DISPON   0x29 // display ON
#define CASET    0x2A // column address set
#define PASET    0x2B // page address set
#define RAMWR    0x2C // memory write
#define RGBSET   0x2D // colour set
#define PTLAR    0x30 // partial area
#define VSCRDEF  0x33 // vertical scrolling definition
#define TEOFF    0x34 // test mode
#define TEON     0x35 // test mode
#define MADCTL   0x36 // memory access control
#define SEP      0x37 // vertical scrolling start address
#define IDMOFF   0x38 // idle mode OFF
#define IDMON    0x39 // idle mode ON
#define COLMOD   0x3A // interface pixel format
#define SETVOP   0xB0 // set Vop
#define BRS      0xB4 // bottom row swap
#define TRS      0xB6 // top row swap
#define DISCTR   0xB9 // display control
#define DOR      0xBA // data order
#define TCDFE    0xBD // enable/disable DF temperature compensation
#define TCVOPE   0xBF // enable/disable Vop temp comp
#define EC       0xC0 // internal or external oscillator
#define SETMUL   0xC2 // set multiplication factor
#define TCVOPAB  0xC3 // set TCVOP slopes A and B
#define TCVOPCD  0xC4 // set TCVOP slopes c and d
#define TCDF     0xC5 // set divider frequency
#define DF8COLOR 0xC6 // set divider frequency 8-color mode
#define SETBS    0xC7 // set bias system
#define RDTEMP   0xC8 // temperature read back
#define NLI      0xC9 // n-line inversion#define RDID1
#define RDID1    0XDA // read ID1
#define RDID2    0xDB // read ID2 
#define RDID3    0xDC // read ID3


//MADCTL
#define MY 0x80
#define MX 0x40

void
lcd_pcf8833_rst(lcd_pcf8833_t *lcd)
{
 int i, j;
 for (i = 0; i < 132; i++)
  for (j = 0; j < 132; j++)
   lcd->ram[i][j] = 0xFF000000;

 lcd->tp = 0;
 lcd->update = 1;
 lcd->dat = 0;

 lcd->cmin = 0;
 lcd->cmax = 131;
 lcd->rmin = 0;
 lcd->rmax = 131;
 bitbang_spi_rst (&lcd->bb_spi);
}

void
lcd_pcf8833_init(lcd_pcf8833_t *lcd)
{

 lcd->tp = 0;
 lcd->update = 1;
 lcd->hrst = 0;
 lcd->dat = 0;
 lcd->colm = 3;

 lcd->cmin = 0;
 lcd->cmax = 131;
 lcd->rmin = 0;
 lcd->rmax = 131;

 bitbang_spi_init (&lcd->bb_spi, 9);
}

void
lcd_pcf8833_update(lcd_pcf8833_t *lcd)
{
 lcd->update = 1;
}
//void lcd_pcf8833_end(lcd_pcf8833_t *lcd){}

unsigned char
lcd_pcf8833_io(lcd_pcf8833_t *lcd, unsigned char pdat, unsigned char clk, unsigned char ncs, unsigned char nrst)
{


 //reset
 if (nrst == 0)
  {
   if (!lcd->hrst)
    {
     lcd_pcf8833_rst (lcd);
     dprint ("Hard reset\n");
     lcd->hrst = 1;
    }
   return 1;
  }
 else
  lcd->hrst = 0;

 bitbang_spi_io (&lcd->bb_spi, clk, pdat, ncs);

 switch (bitbang_spi_get_status (&lcd->bb_spi))
  {
  case SPI_DATA:
   
   lcd->tp = (lcd->bb_spi.insr & 0x100)>>8;
   lcd->dat = lcd->bb_spi.data8;
   
   if (lcd->tp == 0)//command
    {
     lcd->command = lcd->dat;
     dprint ("command:%#04X  ", lcd->command);
     switch (lcd->command)
      {
      case NOP:
       dprint ("NOP");
       break;
      case SWRESET: //Software Reset
       lcd_pcf8833_rst (lcd);
       dprint ("SWRESET");
       break;
      case BSTRON: //Booster ON
       dprint ("BSTRON");
       break;
      case SLEEPOUT: //Sleep Out
       dprint ("SLEEPOUT");
       break;
      case NORON: //Normal display mode
       dprint ("NORON");
       break;
      case INVOFF: //Display inversion on
       dprint ("INVOFF");
       break;
      case INVON: //Display inversion on
       dprint ("INVON");
       break;
      case DISPON: //Display On
       dprint ("DISPON");
       break;
      case DOR: //Data order
       dprint ("DOR");
       break;
      case MADCTL: //Memory data access control (ARG)
       dprint ("MADCTL");
       lcd->dc = 0;
       break;
      case COLMOD: //Bit per Pixel (ARG)
       dprint ("COLMOD");
       lcd->dc = 0;
       break;
      case SETCON: //Set Constrast (ARG)
       dprint ("SETCON");
       break;
      case CASET: //Column Adress Set (ARG,ARG)
       lcd->dc = 0;
       dprint ("CASET");
       break;
      case PASET: //Page Adress Set (ARG,ARG)
       lcd->dc = 0;
       dprint ("PASET");
       break;
      case 0x2D: //Define Color Table(20 bytes) 
       dprint ("Command not implemented!!!!");
       break;
      case RAMWR: //Memory Write
       lcd->x = lcd->cmin;
       lcd->y = lcd->rmin;
       lcd->dc = 0;
       dprint ("RAWWR");
       break;
      default:
       dprint ("Unknown command not implemented!!!!");
       break;
      }
     dprint ("\n");
    }
   else //data
    {
     if (lcd->command != RAMWR)
      {
       dprint ("dat:%#04X\n", lcd->dat);
      }
     if (lcd->command == CASET)
      {
       switch (lcd->dc)
        {
        case 0:
         lcd->cmin = lcd->dat;
         lcd->dc++;
         break;
        case 1:
         lcd->cmax = lcd->dat;
         lcd->dc++;
         break;
        }
      }

     if (lcd->command == PASET)
      {
       switch (lcd->dc)
        {
        case 0:
         lcd->rmin = lcd->dat;
         lcd->dc++;
         break;
        case 1:
         lcd->rmax = lcd->dat;
         lcd->dc++;
         break;
        }
      }

     if (lcd->command == COLMOD)
      {
       switch (lcd->dc)
        {
        case 0:
         lcd->colm = lcd->dat;
         lcd->dc++;
         break;
        }
      }
     if (lcd->command == MADCTL)
      {
       switch (lcd->dc)
        {
        case 0:
         lcd->madctl = lcd->dat;
         lcd->dc++;
         break;
        }
      }

     if (lcd->command == RAMWR)
      {
       switch (lcd->colm)
        {
        case 0x03:
         switch (lcd->dc)
          {
          case 0:
           lcd->r = ((lcd->dat & 0xF0) >> 4);
           lcd->g = (lcd->dat & 0x0F);
           lcd->dc++;
           break;
          case 1:
           lcd->b = ((lcd->dat & 0xF0) >> 4);

           lcd->color = 0xFF000000 | (lcd->r << 20) | (lcd->g << 12) | (lcd->b << 4);
           if ((lcd->madctl & MX) && (lcd->madctl & MX))
            lcd->ram[131 - lcd->x][131 - lcd->y] = lcd->color;
           else if (lcd->madctl & MX)
            lcd->ram[131 - lcd->x][lcd->y] = lcd->color;
           else if (lcd->madctl & MY)
            lcd->ram[lcd->x][131 - lcd->y] = lcd->color;
           else
            lcd->ram[lcd->x][lcd->y] = lcd->color;
           lcd->update = 1;
           lcd->x++;
           if (lcd->x > lcd->cmax)
            {
             lcd->x = lcd->cmin;
             lcd->y++;
             if (lcd->y > lcd->rmax)
              {
               lcd->y = lcd->rmin;
              }
            }
           lcd->r = (lcd->dat & 0x0F);
           lcd->dc++;
           break;
          default:
           lcd->g = ((lcd->dat & 0xF0) >> 4);
           lcd->b = (lcd->dat & 0x0F);

           lcd->color = 0xFF000000 | (lcd->r << 20) | (lcd->g << 12) | (lcd->b << 4);
           if ((lcd->madctl & MX) && (lcd->madctl & MX))
            lcd->ram[131 - lcd->x][131 - lcd->y] = lcd->color;
           else if (lcd->madctl & MX)
            lcd->ram[131 - lcd->x][lcd->y] = lcd->color;
           else if (lcd->madctl & MY)
            lcd->ram[lcd->x][131 - lcd->y] = lcd->color;
           else
            lcd->ram[lcd->x][lcd->y] = lcd->color;
           lcd->update = 1;
           lcd->x++;
           if (lcd->x > lcd->cmax)
            {
             lcd->x = lcd->cmin;
             lcd->y++;
             if (lcd->y > lcd->rmax)
              {
               lcd->y = lcd->rmin;
              }
            }
           lcd->dc = 0;
           break;
          }
         break;
        case 0x05:
         switch (lcd->dc)
          {
          case 0:
           lcd->r = ((lcd->dat & 0xF8) >> 3);
           lcd->g = (lcd->dat & 0x07) << 3;
           lcd->dc++;
           break;
          case 1:
           lcd->g = (lcd->g | ((lcd->dat & 0xE0) >> 5));
           lcd->b = (lcd->dat & 0x1F);

           lcd->color = 0xFF000000 | (lcd->r << 19) | (lcd->g << 10) | (lcd->b << 3);

           if ((lcd->madctl & MX) && (lcd->madctl & MX))
            lcd->ram[131 - lcd->x][131 - lcd->y] = lcd->color;
           else if (lcd->madctl & MX)
            lcd->ram[131 - lcd->x][lcd->y] = lcd->color;
           else if (lcd->madctl & MY)
            lcd->ram[lcd->x][131 - lcd->y] = lcd->color;
           else
            lcd->ram[lcd->x][lcd->y] = lcd->color;
           lcd->update = 1;
           lcd->x++;
           if (lcd->x > lcd->cmax)
            {
             lcd->x = lcd->cmin;
             lcd->y++;
             if (lcd->y > lcd->rmax)
              {
               lcd->y = lcd->rmin;
              }
            }
           lcd->dc = 0;
           break;
          }
         break;
        }
      }
    }
   break;
  }

 return 1;
}

void
lcd_pcf8833_draw(lcd_pcf8833_t *lcd, CCanvas * canvas, int x1, int y1, int w1, int h1, int picpwr)
{
 unsigned char x, y;

 //canvas->Rectangle (1, x1, y1, w1, h1);//erase all

 lcd->update = 0;

 for (x = 0; x < 132; x++)
  {
   for (y = 0; y < 132; y++)
    {


     if (lcd->ram[x][y] & 0xFF000000)
      {
       unsigned int color = lcd->ram[x][y];
       unsigned char r = ((color & 0x00FF0000) >> 16);
       unsigned char g = ((color & 0x0000FF00) >> 8);
       unsigned char b = (color & 0x000000FF);

       lcd->ram[x][y] &= 0x00FFFFFF; //clear draw

       canvas->SetFgColor (r, g, b);
       canvas->SetColor (r, g, b);
       //canvas->Rectangle (1, x1+(x*2), y1+(y*2), 2,2 );
       canvas->Point (x1 + x, y1 + y);
      }
    }
  }

}








