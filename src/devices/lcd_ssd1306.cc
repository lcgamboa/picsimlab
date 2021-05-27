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


#define dprint if (1) {} else printf
#define dcprint if (1) {} else printf

#include"lcd_ssd1306.h"

void
lcd_ssd1306_rst(lcd_ssd1306_t *lcd)
{
 int i, j;
 for (i = 0; i < 128; i++)
  for (j = 0; j < 8; j++)
   lcd->ram[i][j] = 0xFF00;

 bitbang_i2c_rst (&lcd->bb_i2c);
 bitbang_spi_rst (&lcd->bb_spi);

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
 lcd->col_end = 127;
 lcd->pag_start = 0;
 lcd->pag_end = 7;

}

void
lcd_ssd1306_init(lcd_ssd1306_t *lcd)
{
 lcd->hrst = 0;
 bitbang_i2c_init (&lcd->bb_i2c, 0x3C);
 bitbang_spi_init (&lcd->bb_spi);
 lcd_ssd1306_rst (lcd);
}

void
lcd_ssd1306_update(lcd_ssd1306_t *lcd)
{
 int i, j;
 lcd->update = 1;
 for (i = 0; i < 128; i++)
  for (j = 0; j < 8; j++)
   lcd->ram[i][j] |= 0xFF00;
}

static void
lcd_ssd1306_process(lcd_ssd1306_t *lcd)
{

 if (lcd->dc == 0)//command
  {
   if (lcd->cmd_argc > 0)
    {
     lcd->cmd_val = lcd->dat;
     lcd->dat = lcd->last_cmd;
     dprint ("command:%#04X  argument[%i]:%#04X ", lcd->dat, lcd->cmd_argc, lcd->cmd_val);
    }
   else
    {
     dprint ("command:%#04X  ", lcd->dat);
    }
   switch (lcd->dat)
    {
     //Fundamental Command Table 
    case 0x81:
     dprint ("Set Contrast Control\n");
     if (!lcd->cmd_argc)
      {
       lcd->last_cmd = lcd->dat;
       lcd->cmd_argc = 1;
      }
     else
      {
       lcd->cmd_argc = 0;
      }
     break;
    case 0XA4:
    case 0XA5:
     dprint ("Entire display on\n");
     break;
    case 0xA6:
    case 0xA7:
     dprint ("Set Normal/Inverse display\n");
     lcd->inv = lcd->dat & 0x01;
     lcd_ssd1306_update (lcd);
     break;
    case 0xAE:
    case 0xAF:
     dprint ("Set Display ON/OFF\n");
     lcd->on = lcd->dat & 0x01;
     lcd_ssd1306_update (lcd);
     break;
     //Scrolling Command Table  
    case 0x26:
    case 0x27:
     dprint ("***********Continuous Horizontal Scroll Setup\n");
     if (!lcd->cmd_argc)
      {
       lcd->last_cmd = lcd->dat;
       lcd->cmd_argc = 6;
      }
     else
      {
       //TODO ssd1306 Scrolling Command Table
       lcd->cmd_argc--;
      }
     break;
    case 0x29:
    case 0x2A:
     dprint ("***********Continuous Vertical and Horizontal Scroll Setup\n");
     if (!lcd->cmd_argc)
      {
       lcd->last_cmd = lcd->dat;
       lcd->cmd_argc = 5;
      }
     else
      {
       //TODO ssd1306 Continuous Vertical and Horizontal Scroll Setup 
       lcd->cmd_argc--;
      }
     break;
    case 0x2E:
     dprint ("***********Deactivate scroll\n");
     break;
    case 0x2F:
     dprint ("***********Activate scroll\n");
     break;
    case 0xA3:
     dprint ("***********Set Vertical Scroll\n");
     if (!lcd->cmd_argc)
      {
       lcd->last_cmd = lcd->dat;
       lcd->cmd_argc = 2;
      }
     else
      {
       //TODO ssd1306 Set Vertical Scroll
       lcd->cmd_argc--;
      }
     break;
     //Addressing Setting Command Table
    case 0x00 ... 0x0F:
     dprint ("Set Lower Column Start Address for Page Addressing Mode\n");
     lcd->x = (lcd->x & 0xF0) | (lcd->dat & 0x0F);
     break;
    case 0x10 ... 0x1F:
     dprint ("Set Higher Column Start Address for Page Addressing Mode\n");
     lcd->x = (lcd->x & 0x0F) | ((lcd->dat & 0x0F) << 4);
     break;
    case 0x20:
     dprint ("Set Memory Addressing Mode\n");
     if (!lcd->cmd_argc)
      {
       lcd->last_cmd = lcd->dat;
       lcd->cmd_argc = 1;
      }
     else
      {
       lcd->am = lcd->cmd_val & 0x03;
       lcd->cmd_argc = 0;
      }
     break;
    case 0x21:
     dprint ("Set Column Address\n");
     if (!lcd->cmd_argc)
      {
       lcd->last_cmd = lcd->dat;
       lcd->cmd_argc = 2;
      }
     else
      {
       switch (lcd->cmd_argc)
        {
        case 2:
         lcd->col_start = lcd->cmd_val & 0x7F;
         lcd->x = lcd->col_start;
         break;
        case 1:
         lcd->col_end = lcd->cmd_val & 0x7F;
         break;
        }
       lcd->cmd_argc--;
      }
     break;
     //Addressing Setting Command Table   
    case 0x22:
     dprint ("Set Page Address\n");
     if (!lcd->cmd_argc)
      {
       lcd->last_cmd = lcd->dat;
       lcd->cmd_argc = 2;
      }
     else
      {
       switch (lcd->cmd_argc)
        {
        case 2:
         lcd->pag_start = lcd->cmd_val & 0x07;
         lcd->y = lcd->pag_start;
         break;
        case 1:
         lcd->pag_end = lcd->cmd_val & 0x07;
         break;
        }
       lcd->cmd_argc--;
      }
     break;
    case 0xB0 ... 0xB7:
     dprint ("Set Page Start Address for Page Addressing Mode\n");
     lcd->y = lcd->dat & 0x07;
     break;
     //Hardware Configuration  
    case 0x40 ... 0x7F:
     dprint ("Set Display Start Line\n");
     break;
    case 0xA0:
    case 0xA1:
     dprint ("Set Segment Re-map\n");
     break;
    case 0xA8:
     dprint ("Set Multiplex Ratio\n");
     if (!lcd->cmd_argc)
      {
       lcd->last_cmd = lcd->dat;
       lcd->cmd_argc = 1;
      }
     else
      {
       lcd->cmd_argc = 0;
      }
     break;
    case 0xC0:
    case 0xC8:
     dprint ("Set COM Output Scan Direction\n");
     break;
    case 0xD3:
     dprint ("***********Set Display Offset\n");
     if (!lcd->cmd_argc)
      {
       lcd->last_cmd = lcd->dat;
       lcd->cmd_argc = 1;
      }
     else
      {
       //TODO ssd1306 Set Display Offset
       lcd->cmd_argc = 0;
      }
     break;
    case 0xDA:
     dprint ("Set COM Pins Hardware Configuration\n");
     if (!lcd->cmd_argc)
      {
       lcd->last_cmd = lcd->dat;
       lcd->cmd_argc = 1;
      }
     else
      {
       lcd->cmd_argc = 0;
      }
     break;
     //Timing & Driving Scheme Setting Command Table  
    case 0xD5:
     dprint ("Set Display Clock Divide Ratio/Oscillator Frequency\n");
     if (!lcd->cmd_argc)
      {
       lcd->last_cmd = lcd->dat;
       lcd->cmd_argc = 1;
      }
     else
      {
       lcd->cmd_argc = 0;
      }
     break;
    case 0xD9:
     dprint ("Set Pre-charge Period\n");
     if (!lcd->cmd_argc)
      {
       lcd->last_cmd = lcd->dat;
       lcd->cmd_argc = 1;
      }
     else
      {
       lcd->cmd_argc = 0;
      }
     break;
    case 0xDB:
     dprint ("Set V COMH Deselect Level\n");
     if (!lcd->cmd_argc)
      {
       lcd->last_cmd = lcd->dat;
       lcd->cmd_argc = 1;
      }
     else
      {
       lcd->cmd_argc = 0;
      }
     break;
    case 0xE3:
     dprint ("NOP\n");
     break;
    default:
     dprint ("Unknown command not implemented!!!!\n");
     break;
    }
  }
 else //data
  {
   dcprint ("data[%i][%i]:%#02X  \n", lcd->x, lcd->y, lcd->dat);
   lcd->ram[lcd->x][lcd->y] = 0xFF00 | lcd->dat;
   lcd->update = 1;
   switch (lcd->am)
    {
    case 1: //vertical 
     lcd->y++;
     if (lcd->y > lcd->pag_end)
      {
       lcd->y = lcd->pag_start;
       lcd->x++;
       if (lcd->x > lcd->col_end)lcd->x = lcd->col_start;
      }
     break;
    case 0: //horizontal
     lcd->x++;
     if (lcd->x > lcd->col_end)
      {
       lcd->x = lcd->col_start;
       lcd->y++;
       if (lcd->y > lcd->pag_end)lcd->y = lcd->pag_start;
      }
     break;
    case 2://page
     lcd->x++;
     if (lcd->x > lcd->col_end)
      {
       lcd->x = lcd->col_start;
      }
     break;
    }

  }
}

unsigned char
lcd_ssd1306_SPI_io(lcd_ssd1306_t *lcd, unsigned char din, unsigned char clk, unsigned char ncs, unsigned char nrst, unsigned char dc)
{

 //reset
 if (nrst == 0)
  {
   if (!lcd->hrst)
    {
     lcd_ssd1306_rst (lcd);
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
   lcd_ssd1306_process (lcd);
   break;
  }

 return 1;
}

unsigned char
lcd_ssd1306_I2C_io(lcd_ssd1306_t *lcd, unsigned char sda, unsigned char scl)
{
 unsigned char ret = bitbang_i2c_io (&lcd->bb_i2c, scl, sda);

 switch (bitbang_i2c_get_status (&lcd->bb_i2c))
  {
  case I2C_DATAW:
   if (lcd->bb_i2c.byte == 2)
    {
     lcd->dc = (lcd->bb_i2c.datar & 0x40) > 0;
     dcprint ("lcd ctrl = %02X\n", lcd->bb_i2c.datar);
    }
   else
    {
     lcd->dat = lcd->bb_i2c.datar;
     dcprint ("write lcd =%02X\n", lcd->dat);
     lcd_ssd1306_process (lcd);
    }
   break;
  }

 return ret;
}

void
lcd_ssd1306_draw(lcd_ssd1306_t *lcd, CCanvas * canvas, int x1, int y1, int w1, int h1, int picpwr)
{
 unsigned char x, y, z;

 lxColor front (0xb4, 0xff, 0xfc);
 lxColor back (0x0f, 0x0f, 0x17);


 lcd->update = 0;

 if (!lcd->on) return;

 for (x = 0; x < 128; x++)
  {
   for (y = 0; y < 8; y++)
    {

     if (lcd->ram[x][y] & 0xFF00)
      {
       lcd->ram[x][y] &= 0x00FF; //clear draw
       for (z = 0; z < 8; z++)
        {
         if (!(lcd->ram[x][y] & (0x01 << z)) != (!lcd->inv))
          {
           canvas->SetColor (front);
          }
         else
          {
           canvas->SetColor (back);
          }
         //canvas->Rectangle (1, x1 + (x * 2), y1 + (y * 8 * 2)+(z * 2), 2, 2);
         canvas->Point (x1 + x, y1 + y * 8 + z);
        }
      }
    }
  }

}








