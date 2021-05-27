/* ########################################################################

   PICsimLab - PIC laboratory simulator

   ########################################################################

   Copyright (c) : 2020-2020  Luis Claudio Gambôa Lopes

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
#define dcprint if (1) {} else printf

#include"lcd_ili9341.h"

void
lcd_ili9341_rst(lcd_ili9341_t *lcd)
{
 int i, j;
 for (i = 0; i < 240; i++)
  for (j = 0; j < 320; j++)
   lcd->ram[i][j] = 0xFF000000;

 bitbang_spi_rst (&lcd->bb_spi);
 lcd->pwr = -1;
 lcd->prd = -1;
 lcd->update = 1;
 lcd->dat = 0;
 lcd->x = 0;
 lcd->y = 0;
 lcd->am = 2;
 lcd->inv = 0;
 lcd->on = 0;
 lcd->cmd_argc = 0;
 lcd->dc = 0;

 lcd->col_start = 0;
 lcd->col_end = 0x00EF; //239
 lcd->pag_start = 0;
 lcd->pag_end = 0x013F; //319

 lcd->mac = 0;
 lcd->pf = 0x66;

}

void
lcd_ili9341_init(lcd_ili9341_t *lcd)
{
 lcd->hrst = 0;
 bitbang_spi_init (&lcd->bb_spi);
 lcd_ili9341_rst (lcd);
}

void
lcd_ili9341_update(lcd_ili9341_t *lcd)
{
 int i, j;
 lcd->update = 1;
 for (i = 0; i < 240; i++)
  for (j = 0; j < 320; j++)
   lcd->ram[i][j] |= 0xFF000000;
}

static void
lcd_ili9341_readdata(lcd_ili9341_t *lcd)
{
 int lx, ly;

 if ((lcd->pf & 0x06) == 0x06)
  {
   if (!lcd->color)
    {
     lcd->color = lcd->dat;
     lcd->color = lcd->color << 16;
     lcd->color |= 0xF0000000;
    }
   else if ((lcd->color & 0xFF000000) == 0xF0000000)
    {
     lcd->color = 0xFF000000 | (lcd->dat << 8);
    }
   else
    {
     lcd->color |= lcd->dat;

     dcprint ("data[%i][%i]:%#08lX  \n", lcd->x, lcd->y, lcd->color);

     lcd->ram[lcd->x][lcd->y] = lcd->color;
     lcd->update = 1;

     lcd->x++;
     if (lcd->x > lcd->col_end)
      {
       lcd->x = lcd->col_start;
       lcd->y++;
       if (lcd->y > lcd->pag_end)lcd->y = lcd->pag_start;
      }
     lcd->color = 0;
    }
  }
 else//16bits
  {
   if (!lcd->color)
    {
     lcd->color = 0xFF000000 | ((lcd->dat & 0xF8) << 16);
     lcd->color |= (lcd->dat & 0x07) << 13;
    }
   else
    {
     lcd->color |= (lcd->dat & 0xE0) << 5;
     lcd->color |= (lcd->dat & 0x1F) << 3;

     dcprint ("data[%i][%i]:%#08lX  \n", lcd->x, lcd->y, lcd->color);


     if (lcd->mac & 0x02)
      {
       lx = lcd->y;
       ly = lcd->x;
      }
     else
      {
       lx = lcd->x;
       ly = lcd->y;
      }

     if ((lx < 240)&&(ly < 320))
      {

       if (!(lcd->mac & 0x80))//MY
        {
         ly = 319 - ly;
        }

       if ((lcd->mac & 0x40))//MX
        {
         lx = 239 - lx;
        }

       lcd->ram[lx][ly] = lcd->color;

      }
     lcd->update = 1;


     if (lcd->mac & 0x20)
      {
       lcd->y++;
       if (lcd->y > lcd->col_end)
        {
         lcd->y = lcd->col_start;
         lcd->x++;
         if (lcd->x > lcd->pag_end)lcd->x = lcd->pag_start;
        }
      }
     else
      {
       lcd->x++;
       if (lcd->x > lcd->col_end)
        {
         lcd->x = lcd->col_start;
         lcd->y++;
         if (lcd->y > lcd->pag_end)lcd->y = lcd->pag_start;
        }
      }

     lcd->color = 0;
    }
  }
}

static void
lcd_ili9341_process(lcd_ili9341_t *lcd)
{


 if ((lcd->dc == 0) || (lcd->cmd_argc))//command
  {
   if (lcd->cmd_argc > 0)
    {
     lcd->cmd_val = lcd->dat;
     lcd->dat = lcd->last_cmd;
     dprint ("command:%#04X  argument[%i]:%#04X", lcd->dat, lcd->cmd_argc, lcd->cmd_val);
    }
   else
    {
     dprint ("command:%#04X  ", lcd->dat);
    }
   switch (lcd->dat)
    {
    case 0x00:
     dprint ("NOP\n");
     break;
    case 0x01:
     dprint ("Software Reset\n");
     break;
    case 0x04:
     dprint ("Read Display Identification Information\n");
     if (!lcd->cmd_argc)
      {
       lcd->last_cmd = lcd->dat;
       lcd->cmd_argc = 4;
      }
     else
      {
       switch (lcd->cmd_argc)
        {
        case 4:
         lcd->out = 0x01;
         break;
        case 3:
         lcd->out = 0x02;
         break;
        case 2:
         lcd->out = 0x03;
         break;
        case 1:
         lcd->out = 0xFF;
         break;
        }
       lcd->cmd_argc--;
      }
     break;
    case 0x11:
     dprint ("Sleep OUT\n");
     break;
    case 0x13:
     dprint ("Normal Display Mode ON\n");
     break;
    case 0x20:
     dprint ("Display Inversion OFF\n");
     break;
    case 0x26:
     dprint ("Gamma Set\n");
     if (!lcd->cmd_argc)
      {
       lcd->last_cmd = lcd->dat;
       lcd->cmd_argc = 1;
      }
     else
      {
       lcd->cmd_argc--;
      }
     break;
    case 0x28:
     dprint ("Display OFF\n");
     lcd->on = 0;
     break;
    case 0x29:
     dprint ("Display ON\n");
     lcd->on = 1;
     break;
    case 0x2A:
     dprint ("Column Address Set\n");
     if (!lcd->cmd_argc)
      {
       lcd->last_cmd = lcd->dat;
       lcd->cmd_argc = 4;
      }
     else
      {
       switch (lcd->cmd_argc)
        {
        case 4:
         lcd->col_start = lcd->cmd_val << 8;
         break;
        case 3:
         lcd->col_start |= lcd->cmd_val;
         break;
        case 2:
         lcd->col_end = lcd->cmd_val << 8;
         break;
        case 1:
         lcd->col_end |= lcd->cmd_val;
         break;
        }
       lcd->cmd_argc--;
      }
     break;
    case 0x2B:
     dprint ("Page Address Set\n");
     if (!lcd->cmd_argc)
      {
       lcd->last_cmd = lcd->dat;
       lcd->cmd_argc = 4;
      }
     else
      {
       switch (lcd->cmd_argc)
        {
        case 4:
         lcd->pag_start = lcd->cmd_val << 8;
         break;
        case 3:
         lcd->pag_start |= lcd->cmd_val;
         break;
        case 2:
         lcd->pag_end = lcd->cmd_val << 8;
         break;
        case 1:
         lcd->pag_end |= lcd->cmd_val;
         break;
        }
       lcd->cmd_argc--;
      }
     break;
    case 0x2C:
     dprint ("Memory Write\n");
     if (!lcd->cmd_argc)
      {
       lcd->last_cmd = lcd->dat;

       if ((lcd->pf & 0x06) == 0x06)
        {
         lcd->cmd_argc = 3;
        }
       else
        {
         lcd->cmd_argc = 2;
        }

       if (lcd->mac & 0x20)
        {
         lcd->y = lcd->col_start;
         lcd->x = lcd->pag_start;
        }
       else
        {
         lcd->x = lcd->col_start;
         lcd->y = lcd->pag_start;
        }
       lcd->color = 0;
      }
     else
      {
       lcd->dat = lcd->cmd_val;
       lcd_ili9341_readdata (lcd);
       lcd->cmd_argc--;
      }
     break;
    case 0x36:
     dprint ("Memory Access Control\n");
     if (!lcd->cmd_argc)
      {
       lcd->last_cmd = lcd->dat;
       lcd->cmd_argc = 1;
      }
     else
      {
       lcd->mac = lcd->cmd_val;
       lcd->cmd_argc--;
      }
     break;
    case 0x3A:
     dprint ("Pixel Format Set\n");
     if (!lcd->cmd_argc)
      {
       lcd->last_cmd = lcd->dat;
       lcd->cmd_argc = 1;
      }
     else
      {
       lcd->pf = lcd->cmd_val;
       lcd->cmd_argc--;
      }
     break;
    case 0xB1:
     dprint ("Frame Rate Control\n");
     if (!lcd->cmd_argc)
      {
       lcd->last_cmd = lcd->dat;
       lcd->cmd_argc = 2;
      }
     else
      {
       lcd->cmd_argc--;
      }
     break;
    case 0xB6:
     dprint ("Display Function Control\n");
     if (!lcd->cmd_argc)
      {
       lcd->last_cmd = lcd->dat;
       lcd->cmd_argc = 4;
      }
     else
      {
       lcd->cmd_argc--;
      }
     break;
    case 0xB7:
     dprint ("Entry Mode Set\n");
     if (!lcd->cmd_argc)
      {
       lcd->last_cmd = lcd->dat;
       lcd->cmd_argc = 1;
      }
     else
      {
       lcd->cmd_argc--;
      }
     break;
    case 0xC0:
     dprint ("Power Control 1\n");
     if (!lcd->cmd_argc)
      {
       lcd->last_cmd = lcd->dat;
       lcd->cmd_argc = 1;
      }
     else
      {
       lcd->cmd_argc--;
      }
     break;
    case 0xC1:
     dprint ("Power Control 2\n");
     if (!lcd->cmd_argc)
      {
       lcd->last_cmd = lcd->dat;
       lcd->cmd_argc = 1;
      }
     else
      {
       lcd->cmd_argc--;
      }
     break;
    case 0xC5:
     dprint ("VCOM Control 1\n");
     if (!lcd->cmd_argc)
      {
       lcd->last_cmd = lcd->dat;
       lcd->cmd_argc = 2;
      }
     else
      {
       lcd->cmd_argc--;
      }
     break;
    case 0xC7:
     dprint ("VCOM Control 2\n");
     if (!lcd->cmd_argc)
      {
       lcd->last_cmd = lcd->dat;
       lcd->cmd_argc = 1;
      }
     else
      {
       lcd->cmd_argc--;
      }
     break;
    case 0xD3:
     dprint ("Read ID4\n");
     if (!lcd->cmd_argc)
      {
       lcd->last_cmd = lcd->dat;
       lcd->cmd_argc = 4;
      }
     else
      {
       switch (lcd->cmd_argc)
        {
        case 4:
         lcd->out = 0x00;
         break;
        case 3:
         lcd->out = 0x93;
         break;
        case 2:
         lcd->out = 0x41;
         break;
        case 1:
         lcd->out = 0xFF;
         break;
        }
       lcd->cmd_argc--;
      }
     break;
    case 0xE0:
     dprint ("Positive Gamma Correction\n");
     if (!lcd->cmd_argc)
      {
       lcd->last_cmd = lcd->dat;
       lcd->cmd_argc = 15;
      }
     else
      {
       lcd->cmd_argc--;
      }
     break;
    case 0xE1:
     dprint ("Negative Gamma Correction\n");
     if (!lcd->cmd_argc)
      {
       lcd->last_cmd = lcd->dat;
       lcd->cmd_argc = 15;
      }
     else
      {
       lcd->cmd_argc--;
      }
     break;
    case 0xCB:
     dprint ("User command\n");
     if (!lcd->cmd_argc)
      {
       lcd->last_cmd = lcd->dat;
       lcd->cmd_argc = 4;
      }
     else
      {
       lcd->cmd_argc--;
      }
     break;
    case 0xCF:
    case 0xE8:
     dprint ("User command\n");
     if (!lcd->cmd_argc)
      {
       lcd->last_cmd = lcd->dat;
       lcd->cmd_argc = 3;
      }
     else
      {
       lcd->cmd_argc--;
      }
     break;
    case 0xEA:
     dprint ("User command\n");
     if (!lcd->cmd_argc)
      {
       lcd->last_cmd = lcd->dat;
       lcd->cmd_argc = 2;
      }
     else
      {
       lcd->cmd_argc--;
      }
     break;
    case 0xF2:
     dprint ("User command\n");
     if (!lcd->cmd_argc)
      {
       lcd->last_cmd = lcd->dat;
       lcd->cmd_argc = 1;
      }
     else
      {
       lcd->cmd_argc--;
      }
     break;
    default:
     dprint ("Unknown command not implemented!!!!\n");
     break;
    }
  }
 else //data
  {
   lcd_ili9341_readdata (lcd);
  }
}

unsigned char
lcd_ili9341_SPI_io(lcd_ili9341_t *lcd, unsigned char din, unsigned char clk, unsigned char ncs, unsigned char nrst, unsigned char dc)
{

 //reset
 if (nrst == 0)
  {
   if (!lcd->hrst)
    {
     lcd_ili9341_rst (lcd);
     dcprint ("Hard reset\n");
     lcd->hrst = 1;
    }
   return 1;
  }
 else
  lcd->hrst = 0;

 bitbang_spi_io (&lcd->bb_spi, clk, din, ncs);

 switch (bitbang_spi_get_status (&lcd->bb_spi))
  {
  case SPI_DATA:
   lcd->dat = lcd->bb_spi.data;
   lcd->dc = dc;
   lcd_ili9341_process (lcd);
   break;
  }

 return 1;
}

unsigned short
lcd_ili9341_8_io(lcd_ili9341_t *lcd, unsigned char dat, unsigned char wr, unsigned char rd, unsigned char ncs, unsigned char nrst, unsigned char dc)
{
 //reset
 if (nrst == 0)
  {
   if (!lcd->hrst)
    {
     lcd_ili9341_rst (lcd);
     dcprint ("Hard reset\n");
     lcd->hrst = 1;
    }
   lcd->out = 0;
   return 0;
  }
 else
  lcd->hrst = 0;

 //cs
 if (ncs == 1)
  {
   //dprint ("No CS\n");
   lcd->pwr = 1;
   lcd->prd = 1;
   lcd->out &= ~0x0100;
   return 0;
  }

 if (!rd)
  {
   lcd->out |= 0x0100;
  }
 else
  {
   lcd->out &= ~0x0100;
  }

 //transicao WR
 if ((lcd->pwr == 0)&&(wr == 1))
  {
   lcd->dc = dc;
   lcd->dat = dat;
   lcd_ili9341_process (lcd);
  }
 else if ((lcd->prd == 0)&&(rd == 1))
  {
   lcd->dc = dc;
   lcd->dat = dat;
   lcd_ili9341_process (lcd);
   dprint ("Reading %02X\n", 0xFF & lcd->out);
  }

 lcd->pwr = wr;
 lcd->prd = rd;
 return lcd->out;
}

void
lcd_ili9341_draw(lcd_ili9341_t *lcd, CCanvas * canvas, int x1, int y1, int w1, int h1, int picpwr)
{
 unsigned short x, y;
 unsigned char r, g, b;

 lcd->update = 0;

 if (!lcd->on) return;

 for (x = 0; x < 240; x++)
  {
   for (y = 0; y < 320; y++)
    {

     if (lcd->ram[x][y] & 0xFF000000)
      {
       lcd->ram[x][y] &= 0x00FFFFFF; //clear draw

       r = (lcd->ram[x][y] & 0xFF0000) >> 16;
       g = (lcd->ram[x][y] & 0x00FF00) >> 8;
       b = (lcd->ram[x][y] & 0x0000FF);

       /*
       r = (lcd->ram[x][y] & 0x00F800) >> 8;
       g = (lcd->ram[x][y] & 0x0007E0) >> 3;
       b = (lcd->ram[x][y] & 0x00001F) << 3;
       r = ((lcd->ram[x][y] & 0x00F800) >> 11)*8.23;
       g = ((lcd->ram[x][y] & 0x0007E0) >> 5)*4.05;
       b = (lcd->ram[x][y] & 0x00001F)*8.23;
        */

       canvas->SetColor (r, g, b);

       //canvas->Rectangle (1, x1 + (x * 2), y1 + (y * 8 * 2)+(z * 2), 2, 2);

       canvas->Point (x1 + y, y1 + (239 - x));

      }
    }
  }

}








