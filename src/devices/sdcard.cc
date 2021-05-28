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


#include"sdcard.h"

#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>


#define dprintf if (1) {} else printf
#define dsprintf if (1) {} else printf

void
sdcard_rst(sdcard_t *sd)
{
 bitbang_spi_rst (&sd->bb_spi);
 sd->cmd = 0;
 sd->arg = 0;
 sd->crc = 0;
 sd->replyc = 0;
 sd->ap_cmd = 0;
 sd->data_rc = 0;
 sd->data_wc = 0;
 sd->multi = 0;
 dprintf ("rst sdcard\n");
}

void
sdcard_init(sdcard_t *sd)
{
 sd->card_present = 0;
 bitbang_spi_init (&sd->bb_spi);
 sdcard_rst (sd);
 sd->fd = NULL;
 sd->disk_size = 0;
 dprintf ("init sdcard\n");
}

void
sdcard_end(sdcard_t *sd)
{

 if (sd->fd)
  {
   fclose (sd->fd);
   sd->fd = NULL;
  }

}

void
sdcard_set_card_present(sdcard_t *sd, unsigned char cp)
{
 if (sd->fd)
  {
   sd->card_present = cp;
  }
 else
  {
   sd->card_present = 0;
  }
}

static char buff[512];

unsigned short
sdcard_io(sdcard_t *sd, unsigned char mosi, unsigned char clk, unsigned char ss)
{
 unsigned int offset = 0;

 if (!sd->card_present)
  {
   return 1;
  }

 if (ss)//not selected
  {
   sd->bb_spi.insr = 0;
   sd->bb_spi.outsr = 0;
   sd->bb_spi.bit = 0;
   sd->replyc = 0;
   sd->data_rc = 0;
   sd->data_wc = 0;
   sd->multi = 0;
   return 1;
  }

 bitbang_spi_io (&sd->bb_spi, clk, mosi, ss);

 switch (bitbang_spi_get_status (&sd->bb_spi))
  {
  case SPI_DATA:

   dsprintf ("sdcard byte in 0x%02X  out 0x%02X\n", sd->bb_spi.insr & 0xFF, sd->bb_spi.outsr >> 8);

   if ((sd->data_wc)&&(sd->bb_spi.byte == 1))
    {

     sd->bb_spi.outsr = (sd->bb_spi.outsr & 0xFF00) | 0xFF;
     if (sd->data_wc == 515)//token
      {
       //must be 0xFE
       if (((sd->bb_spi.insr & 0xFF) != 0xFE) && ((sd->bb_spi.insr & 0xFF) != 0xFC))
        {
         sd->data_wc = 516;
        }
      }
     else if (sd->data_wc < 3) //CRC
      {
       //not used

       if (sd->data_wc == 1)
        {
         //write response
         sd->bb_spi.outsr = (sd->bb_spi.outsr & 0xFF00) | 0x05;
        }
      }
     else
      {
       buff[512 - (sd->data_wc - 2)] = sd->bb_spi.insr & 0xFF;
       dsprintf ("sdcard write buff[%i]= 0x%02X \n", 512 - (sd->data_wc - 2), sd->bb_spi.insr & 0xFF);
       if ((sd->data_wc - 3) == 0)
        {
         fwrite (buff, 512, 1, sd->fd);
         dprintf ("sdcard 512 bytes writed end %li \n", ftell (sd->fd) / 512);
        }
      }

     sd->data_wc--;

     if ((sd->multi)&&(!sd->data_wc))
      {
       sd->data_wc = 515;
      }
    }
   else
    {
     switch (sd->bb_spi.byte << 3)
      {
      case 0: //nothing
      case 8: //command

       sd->cmd = sd->bb_spi.insr;
       if ((sd->cmd & 0xC0) == 0x40)
        {
         sd->cmd = sd->bb_spi.insr & 0x3F;
         sd->arg = 0;
        }
       else
        {
         sd->bb_spi.outsr = 0xFF;
         sd->bb_spi.bit = 0;
         sd->bb_spi.byte = 0;
         sd->replyc = 0;
        }
       break;
      case 16: //1/4 parameter
       sd->arg |= (sd->bb_spi.insr & 0xFF) << 24;
       break;
      case 24: //2/4 parameter
       sd->arg |= (sd->bb_spi.insr & 0xFF) << 16;
       break;
      case 32: //3/4 parameter
       sd->arg |= (sd->bb_spi.insr & 0xFF) << 8;
       break;
      case 40: // 4/4parameter
       sd->arg |= (sd->bb_spi.insr & 0xFF);
       break;
      case 48: //crc
       sd->crc = (sd->bb_spi.insr & 0xFF);
       dprintf ("----------------------------------------\n");
       dprintf ("sdcard cmd=0x%02X  arg=0x%08lX  CRC=0x%02X \n", sd->cmd, sd->arg, sd->crc);

       if (sd->ap_cmd)
        {
         switch (sd->cmd)
          {
          case ACMD23: // SET_WR_BLK_ERASE_COUNT - Set the number of write blocks to be pre-erased before writing 
           sd->bb_spi.outsr = 0xFF; //fill byte
           sd->replyc = 2;
           sd->reply[0] = 0x00; //R1  |0|idle|EraseR|IllegalC|CRCE|EraseE|AddressE|ParameterE| 
           dprintf ("sdcard pre-erased %li\n", sd->arg);
           break;
          case ACMD41:// SD_SEND_OP_COMD - Sends host capacity support information and activates the card's initialization process 
           sd->bb_spi.outsr = 0xFF; //fill byte
           sd->replyc = 2;
           sd->reply[0] = 0x00; //R1  |0|idle|EraseR|IllegalC|CRCE|EraseE|AddressE|ParameterE| 
           //sd->reply[1] = 0x00;
           //sd->reply[2] = 0x00;
           //sd->reply[3] = 0x00;
           //sd->reply[4] = 0x00;
           dprintf ("sdcard send_op_comd\n");
           break;
          default:
           sd->bb_spi.bit = 0;
           sd->bb_spi.byte = 0;
           printf ("sdcard command not implemented!!!\n");
           break;
          }
         sd->ap_cmd = 0;
        }
       else
        {
         switch (sd->cmd)
          {
          case CMD0: //GO_IDLE_STATE - init card in spi mode if CS low
           sd->bb_spi.outsr = 0xFF; //fill byte
           sd->reply[0] = 0x01; //R1  |0|idle|EraseR|IllegalC|CRCE|EraseE|AddressE|ParameterE| 
           sd->replyc = 2;
           dprintf ("sdcard soft reset \n");
           break;
          case CMD8: // SEND_IF_COND - verify SD Memory Card interface operating condition.
           sd->bb_spi.outsr = 0xFF; //fill byte
           sd->replyc = 6;
           sd->reply[0] = 0x00; //R1  |0|idle|EraseR|IllegalC|CRCE|EraseE|AddressE|ParameterE| 
           sd->reply[1] = 0x00;
           sd->reply[2] = 0x00;
           sd->reply[3] = 0x01; //voltage
           sd->reply[4] = sd->arg & 0xFF; //check patern
           dprintf ("sdcard send_if_cond  %li\n", sd->arg);
           break;
          case CMD9: // SEND_CSD - read the Card Specific Data (CSD register) 
           sd->bb_spi.outsr = 0xFF; //fill byte
           sd->reply[0] = 0x00; //R1  |0|idle|EraseR|IllegalC|CRCE|EraseE|AddressE|ParameterE| 
           sd->reply[1] = 0xFE; //start block
           //CSDV2
           sd->reply[2] = 0x40; //csd_ver:2 reserved1:6
           sd->reply[3] = 0X0E; //taac:8 fixed 0x0E
           sd->reply[4] = 0x00; //nsac:8 fixed 0
           sd->reply[5] = 0x5A; //tran_speed:8
           sd->reply[6] = 0x5B; //ccc_high:8
           sd->reply[7] = 0x59; //ccc_low:4  read_bl_len:4   fixed to 0x09
           sd->reply[8] = 0x00; //read_bl_partial:1 write_blk_misalign:1  read_blk_misalign:1  dsr_imp:1  reserved2:4 fixed to 0x00
           sd->reply[9] = (((sd->disk_size / 512) - 1) >> 16)&0xFF; //reserved3:2 c_size_high:6  
           sd->reply[10] = (((sd->disk_size / 512) - 1) >> 8)&0xFF; //c_size_mid:8
           sd->reply[11] = ((sd->disk_size / 512) - 1)&0xFF; //c_size_low:8
           sd->reply[12] = 0x7F; //reserved4:1 erase_blk_en:1 sector_size_high:6 
           sd->reply[13] = 0x80; //sector_size_low:1 wp_grp_size:7 
           sd->reply[14] = 0x0A; //wp_grp_enable:1 reserved5:2  r2w_factor:3 write_bl_len_high:2   
           sd->reply[15] = 0x40; //write_bl_len_low:2  write_partial:1 reserved6:5  
           sd->reply[16] = 0x00; //file_format_grp:1  perm_write_protect:1 copy:1  tmp_write_protect:1 file_format:2 reserved7:2    
           sd->reply[18] = 0x01; //crc:7 always1:1 
           sd->replyc = 20;
           dprintf ("sdcard send CSD\n");
           break;
          case CMD10: //SEND_CID - read the card identification information (CID register)
           sd->bb_spi.outsr = 0xFF; //fill byte
           sd->reply[0] = 0x00; //R1  |0|idle|EraseR|IllegalC|CRCE|EraseE|AddressE|ParameterE| 
           sd->reply[1] = 0xFE; //start block
           //CID
           sd->reply[2] = 0x01; // Manufacturer ID 
           sd->reply[3] = 0X34; // OEM/Application high
           sd->reply[4] = 0x32; // OEM/Application low
           sd->reply[5] = 'S'; // Product name
           sd->reply[6] = 'D'; // Product name
           sd->reply[7] = 'S'; // Product name
           sd->reply[8] = 'I'; // Product name
           sd->reply[9] = 'M'; // Product name
           sd->reply[10] = 0x01; //  prv_n:4 prv_m:4
           sd->reply[11] = 0x12; // psn
           sd->reply[12] = 0x34; // psn
           sd->reply[13] = 0x56; // psn
           sd->reply[14] = 0x78; // psn
           sd->reply[15] = 0x00; // reserved:4 mdt_year_high:4 
           sd->reply[16] = 0x04; // mdt_year_low:4 mdt_month:4
           sd->reply[18] = 0x01; // crc:7 always1:1 
           sd->replyc = 20;
           dprintf ("sdcard send CID \n");
           break;
          case CMD12: // STOP_TRANSMISSION - end multiple block read sequence  
           sd->bb_spi.outsr = 0xFF; //fill byte
           sd->reply[0] = 0x00; //R1  |0|idle|EraseR|IllegalC|CRCE|EraseE|AddressE|ParameterE| 
           sd->replyc = 2;
           sd->multi = 0;
           dprintf ("sdcard stop transmission\n");
           break;
          case CMD13: // SEND_STATUS - read the card status register  
           sd->bb_spi.outsr = 0xFF; //fill byte
           sd->reply[0] = 0x00; //R1  |0|idle|EraseR|IllegalC|CRCE|EraseE|AddressE|ParameterE| 
           sd->reply[1] = 0x00; //R2
           sd->replyc = 3;
           dprintf ("sdcard send status\n");
           break;
          case CMD17: // READ_SINGLE_BLOCK - read a single data block from the card 
           sd->bb_spi.outsr = 0xFF; //fill byte
           sd->reply[0] = 0x00; //R1  |0|idle|EraseR|IllegalC|CRCE|EraseE|AddressE|ParameterE| 
           sd->reply[1] = 0xFE; //start block
           sd->replyc = 3;
           sd->data_rc = 512;

           fseek (sd->fd, sd->arg /* 512*/, SEEK_SET);
           fread (buff, 512, 1, sd->fd);
           dprintf ("sdcard reading block %li\n", sd->arg / 512);
           break;
          case CMD24: // WRITE_BLOCK - write a single data block to the card  
           sd->bb_spi.outsr = 0xFF; //fill byte
           sd->reply[0] = 0x00; //R1  |0|idle|EraseR|IllegalC|CRCE|EraseE|AddressE|ParameterE| 
           sd->replyc = 2;
           sd->data_wc = 515; //include 0xFE initial token and crc
           fseek (sd->fd, sd->arg /* 512*/, SEEK_SET);
           dprintf ("sdcard writing block %li\n", sd->arg / 512);
           break;
          case CMD25: // WRITE_MULTIPLE_BLOCK - write blocks of data until a STOP_TRANSMISSION  
           sd->bb_spi.outsr = 0xFF; //fill byte
           sd->reply[0] = 0x00; //R1  |0|idle|EraseR|IllegalC|CRCE|EraseE|AddressE|ParameterE| 
           sd->replyc = 2;
           sd->data_wc = 515; //include 0xFC initial token and crc
           sd->multi = 1;
           fseek (sd->fd, sd->arg /* 512*/, SEEK_SET);
           dprintf ("sdcard writing multiple blocks start at %li\n", sd->arg / 512);
           break;
          case CMD32: // ERASE_WR_BLK_START - sets the address of the first block to be erased 
           sd->bb_spi.outsr = 0xFF; //fill byte
           sd->reply[0] = 0x00; //R1  |0|idle|EraseR|IllegalC|CRCE|EraseE|AddressE|ParameterE| 
           sd->replyc = 2;
           sd->ebstart = sd->arg / 512;
           dprintf ("sdcard erase block start %li\n", sd->ebstart);
           break;
          case CMD33: // ERASE_WR_BLK_END - sets the address of the last block of the continuous range to be erased
           sd->bb_spi.outsr = 0xFF; //fill byte
           sd->reply[0] = 0x00; //R1  |0|idle|EraseR|IllegalC|CRCE|EraseE|AddressE|ParameterE| 
           sd->replyc = 2;
           sd->ebend = sd->arg / 512;
           dprintf ("sdcard erase block end %li\n", sd->ebend);
           break;
          case CMD38: // ERASE - erase all previously selected blocks 
           sd->bb_spi.outsr = 0xFF; //fill byte
           sd->reply[0] = 0x00; //R1  |0|idle|EraseR|IllegalC|CRCE|EraseE|AddressE|ParameterE| 
           sd->replyc = 2;
           dprintf ("sdcard erasing blocks %li to %li\n", sd->ebstart, sd->ebend);
           fseek (sd->fd, sd->ebstart, SEEK_SET);
           memset (buff, 0, 512);
           for (offset = sd->ebstart; offset < sd->ebend; offset++)
            {
             fwrite (buff, 512, 1, sd->fd);
            }
           break;
          case CMD55: // APP_CMD - escape for application specific command 
           sd->bb_spi.outsr = 0xFF; //fill byte
           sd->reply[0] = 0x00; //R1  |0|idle|EraseR|IllegalC|CRCE|EraseE|AddressE|ParameterE| 
           sd->replyc = 2;
           sd->ap_cmd = 1;
           dprintf ("sdcard app command\n");
           break;
          case CMD58: // READ_OCR - read the OCR register of a card 
           sd->bb_spi.outsr = 0xFF; //fill byte
           sd->replyc = 6;
           sd->reply[0] = 0x00; //R1  |0|idle|EraseR|IllegalC|CRCE|EraseE|AddressE|ParameterE| 
           if (sd->disk_size >= 4194304L)
            {
             sd->reply[1] = 0xC1; //Busy:1 CCS:1 resrved:5  S18A:1
            }
           else
            {
             sd->reply[1] = 0x81; //Busy:1 CCS:1 resrved:5  S18A:1  
            }
           sd->reply[2] = 0xFF; //voltage window
           sd->reply[3] = 0x80; //voltage window:1 reseved:7
           sd->reply[4] = 0x00; //reseved:8
           dprintf ("sdcard read OCR \n");
           break;
          default:
           sd->bb_spi.bit = 0;
           sd->bb_spi.byte = 0;
           printf ("sdcard command not implemented!!!\n");
           break;
          }
        }
       break;
      default: //data
       offset = ((sd->bb_spi.byte << 3) - 48) / 8;

       if (!sd->data_rc)
        {
         if (offset >= sd->replyc)
          {
           sd->bb_spi.outsr = (sd->bb_spi.outsr & 0xFF00) | 0xFF;
           sd->bb_spi.bit = 0;
           sd->bb_spi.byte = 0;
           if (sd->cmd == CMD0)
            {
             sdcard_rst (sd);
            }
          }
         else
          {
           sd->bb_spi.outsr = (sd->bb_spi.outsr & 0xFF00) | sd->reply[offset - 1];
          }
        }
       else
        {
         if (offset < sd->replyc)
          {
           sd->bb_spi.outsr = (sd->bb_spi.outsr & 0xFF00) | sd->reply[offset - 1];
          }
         else
          {
           if (sd->data_rc)
            {
             sd->bb_spi.outsr = (sd->bb_spi.outsr & 0xFF00) | buff[512 - sd->data_rc];
             sd->data_rc--;
            }
           else
            {
             sd->bb_spi.outsr = (sd->bb_spi.outsr & 0xFF00) | 0; //crc
             sd->bb_spi.bit = 0;
             sd->bb_spi.byte = 0;
            }

          }
        }

       dsprintf ("sdcard data %03i   out=0x%02X  in=0x%02X  cmd=0x%02X  \n", offset, sd->bb_spi.outsr >> 8, sd->bb_spi.insr & 0xFF, sd->cmd);
       break;
      }
    }
   break;
  }

 return ((sd->bb_spi.outsr & 0x080) > 0);
}

void
sdcard_set_filename(sdcard_t *sd, const char * fname)
{
 if (sd->fd)
  {
   fclose (sd->fd);
  }

 sd->fd = fopen (fname, "r+");
 if (sd->fd)
  {
   struct stat sb;

   sb.st_mode = 0;

   stat (fname, &sb);
   sd->card_present = 1;

   sd->disk_size = sb.st_size >> 10;
   dprintf ("sdcard size=%li kb  ->  %li blocks\n", sd->disk_size, sb.st_size / 512);

  }
}
