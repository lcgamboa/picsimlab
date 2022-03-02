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

#define dprint \
    if (1) {   \
    } else     \
        printf

#include "lcd_pcd8544.h"

void lcd_pcd8544_rst(lcd_pcd8544_t* lcd) {
    int i, j;
    for (i = 0; i < 84; i++)
        for (j = 0; j < 6; j++)
            lcd->ram[i][j] = 0xFF00;
    lcd->update = 1;
    lcd->dat = 0;
    lcd->x = 0;
    lcd->y = 0;
    bitbang_spi_rst(&lcd->bb_spi);
}

void lcd_pcd8544_init(lcd_pcd8544_t* lcd) {
    lcd->hrst = 0;
    lcd->h = 0;
    lcd->v = 0;
    lcd->d = 0;
    lcd->e = 0;
    lcd->pd = 0;
    bitbang_spi_init(&lcd->bb_spi);
    lcd_pcd8544_rst(lcd);
}

void lcd_pcd8544_update(lcd_pcd8544_t* lcd) {
    int i, j;
    lcd->update = 1;
    for (i = 0; i < 84; i++)
        for (j = 0; j < 6; j++)
            lcd->ram[i][j] |= 0xFF00;
}
// void lcd_pcd8544_end(lcd_pcd8544_t *lcd){}

unsigned char lcd_pcd8544_io(lcd_pcd8544_t* lcd, unsigned char din, unsigned char clk, unsigned char ncs,
                             unsigned char nrst, unsigned char dc) {
    // reset
    if (nrst == 0) {
        if (!lcd->hrst) {
            lcd_pcd8544_rst(lcd);
            dprint("Hard reset\n");
            lcd->hrst = 1;
        }
        return 1;
    } else
        lcd->hrst = 0;

    bitbang_spi_io(&lcd->bb_spi, clk, din, ncs);

    switch (bitbang_spi_get_status(&lcd->bb_spi)) {
        case SPI_DATA:
            lcd->dat = lcd->bb_spi.data8;
            if (dc == 0)  // command
            {
                dprint("command:%#04X  ", lcd->dat);
                switch (lcd->dat) {
                    case 0x00:
                        dprint("NOP");
                        break;
                    case 0x20 ... 0x27:
                        dprint("Function set");
                        lcd->h = lcd->dat & 0x01;
                        lcd->v = lcd->dat & 0x02;
                        lcd->pd = lcd->dat & 0x04;
                        lcd_pcd8544_update(lcd);
                        break;
                    case 0x04 ... 0x07:
                        dprint("Temperature control");
                        break;
                    case 0x08 ... 0x0F:
                        dprint("Display control");
                        lcd->d = lcd->dat & 0x04;
                        lcd->e = lcd->dat & 0x01;
                        lcd_pcd8544_update(lcd);
                        break;
                    case 0x10 ... 0x17:
                        dprint("Bias system");
                        break;
                    case 0x40 ... 0x47:
                        dprint("Set Y address of RAM");
                        lcd->y = lcd->dat & 0x07;
                        if (lcd->y > 5)
                            lcd->y -= 5;
                        break;
                    case 0x80 ... 0xFF:
                        if (lcd->h)
                            dprint("Set VOP");
                        else {
                            dprint("Set X address of RAM");
                            lcd->x = lcd->dat & 0x7F;
                            if (lcd->x > 83)
                                lcd->x -= 83;
                        }
                        break;
                    default:
                        dprint("Unknown command not implemented!!!!");
                        break;
                }
                dprint("\n");
            } else  // data
            {
                dprint("data[%i][%i]:%#02X  \n", lcd->x, lcd->y, lcd->dat);
                lcd->ram[lcd->x][lcd->y] = 0xFF00 | lcd->dat;
                lcd->update = 1;
                if (lcd->v) {
                    lcd->y++;
                    if (lcd->y > 5) {
                        lcd->y = 0;
                        lcd->x++;
                        if (lcd->x > 83)
                            lcd->x = 0;
                    }
                } else {
                    lcd->x++;
                    if (lcd->x > 83) {
                        lcd->x = 0;
                        lcd->y++;
                        if (lcd->y > 5)
                            lcd->y = 0;
                    }
                }
            }
            break;
    }

    return 1;
}

void lcd_pcd8544_draw(lcd_pcd8544_t* lcd, CCanvas* canvas, int x1, int y1, int w1, int h1, int picpwr) {
    unsigned char x, y, z;

    if (lcd->e) {
        canvas->SetFgColor(0, 0, 0);
        canvas->SetColor(0, 0, 0);
    } else {
        canvas->SetFgColor(82, 129, 111);
        canvas->SetColor(82, 129, 111);
    }

    // canvas->Rectangle (1, x1, y1, w1, h1);//erase all

    lcd->update = 0;

    if ((lcd->pd) || (!lcd->d))
        return;

    for (x = 0; x < 84; x++) {
        for (y = 0; y < 6; y++) {
            if (lcd->ram[x][y] & 0xFF00) {
                lcd->ram[x][y] &= 0x00FF;  // clear draw
                for (z = 0; z < 8; z++) {
                    if (!(lcd->ram[x][y] & (0x01 << z)) != (!lcd->e)) {
                        canvas->SetFgColor(0, 0, 0);
                        canvas->SetColor(0, 0, 0);
                    } else {
                        canvas->SetFgColor(82, 129, 111);
                        canvas->SetColor(82, 129, 111);
                    }
                    canvas->Rectangle(1, x1 + (x * 2), y1 + (y * 8 * 2) + (z * 2), 2, 2);
                    // canvas->Point (x1 + x, y1 + y*8 +z);
                }
            }
        }
    }
}
