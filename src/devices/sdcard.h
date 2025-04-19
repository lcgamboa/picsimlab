/* ########################################################################

   PICSimLab - Programmable IC Simulator Laboratory

   ########################################################################

   Copyright (c) : 2020-2025  Luis Claudio Gambôa Lopes <lcgamboa@yahoo.com>

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

#ifndef SDCARD
#define SDCARD

#include <stdio.h>
#include "bitbang_spi.h"

// SD card commands
#define CMD0 0X00
#define CMD1 0X01
// no spi #define CMD2 0X02 /** ALL_SEND_CID - Asks any card to send the CID. */
// no spi #define CMD3 0X03 /** SEND_RELATIVE_ADDR - Ask the card to publish a new RCA. */
// #define CMD6 0X06 /** SWITCH_FUNC - Switch Function Command */
// no spi #define CMD7  0X07 /** SELECT/DESELECT_CARD - toggles between the stand-by and transfer states. */
#define CMD8 0X08
#define CMD9 0X09
#define CMD10 0X0A
#define CMD12 0X0C
#define CMD13 0X0D
#define CMD16 0X10
#define CMD17 0X11
#define CMD18 0X12
#define CMD24 0X18
#define CMD25 0X19
#define CMD32 0X20
#define CMD33 0X21
#define CMD38 0X26
#define CMD55 0X37
#define CMD58 0X3A
#define CMD59 0X3B
//  no spi  #define ACMD6 0X06 /** SET_BUS_WIDTH - Defines the data bus width for data transfer. */
#define ACMD13 0X0D
#define ACMD23 0X17
#define ACMD41 0X29
#define ACMD42 0X2A
#define ACMD51 0X33

#define MAX_REPLY 72

typedef struct {
    FILE* fd;
    unsigned char card_present;
    bitbang_spi_t bb_spi;
    unsigned long arg;
    unsigned char cmd;
    unsigned char crc;
    unsigned short replyc;
    unsigned char reply[MAX_REPLY];
    unsigned char ap_cmd;
    unsigned short data_rc;
    unsigned short data_wc;
    unsigned char multi_rd;
    unsigned char multi_wr;
    unsigned long disk_size;  // in kb
    unsigned long ebstart;
    unsigned long ebend;
    unsigned char R1;
    unsigned char crc_on;
    unsigned int cmd_count;  // used for boot
    unsigned char cmd_buff[5];
    unsigned short crc16;
} sdcard_t;

void sdcard_rst(sdcard_t* sd);
void sdcard_init(sdcard_t* sd);
void sdcard_end(sdcard_t* sd);
void sdcard_set_card_present(sdcard_t* sd, unsigned char cp);
void sdcard_set_filename(sdcard_t* sd, const char* fname);

unsigned short sdcard_io(sdcard_t* sd, unsigned char mosi, unsigned char clk, unsigned char ss);

#endif  // SDCARD
