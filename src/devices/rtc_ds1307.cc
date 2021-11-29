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


#include"rtc_ds1307.h"

#include<stdio.h>
#include<string.h>
#include<stdlib.h>

//#define _DEBUG

#define dprintf if (1) {} else printf

void
rtc_ds1307_rst(rtc_ds1307_t *rtc)
{
 bitbang_i2c_rst (&rtc->bb_i2c);

 rtc->addr = 0;
 rtc->ucont = 0;
 rtc->rtcc = 0;
 rtc->alarm = 0;
 dprintf ("rtc rst\n");
}

void
rtc_ds1307_init(rtc_ds1307_t *rtc)
{
 time_t utime;
 dprintf ("rtc init\n");

 bitbang_i2c_init (&rtc->bb_i2c, 0x68);

 memset (rtc->data, 0xFF, 8);
 rtc_ds1307_rst (rtc);

 utime = time (NULL);
 rtc_ds1307_setUtime (rtc, utime);
}

void
rtc_ds1307_setUtime(rtc_ds1307_t *rtc, time_t utime)
{
 rtc->systime = time (NULL);
 rtc->rtctime = utime;
 rtc->ucont = 0;
#ifdef _WIN_
 localtime_s (&rtc->dtime, &utime);
#else
 localtime_r (&utime, &rtc->dtime);
#endif  
 rtc->data[0] = ((rtc->dtime.tm_sec / 10) << 4) | (rtc->dtime.tm_sec % 10);
 rtc->data[1] = ((rtc->dtime.tm_min / 10) << 4) | (rtc->dtime.tm_min % 10);
 rtc->data[2] = ((rtc->dtime.tm_hour / 10) << 4) | (rtc->dtime.tm_hour % 10);
 rtc->data[3] = rtc->dtime.tm_wday;
 rtc->data[4] = ((rtc->dtime.tm_mday / 10) << 4) | (rtc->dtime.tm_mday % 10);
 rtc->data[5] = (((rtc->dtime.tm_mon + 1) / 10) << 4) | ((rtc->dtime.tm_mon + 1) % 10);
 rtc->data[6] = (((rtc->dtime.tm_year % 100) / 10) << 4) | (rtc->dtime.tm_year % 10);
}

time_t
rtc_ds1307_getUtime(rtc_ds1307_t *rtc)
{
 return mktime (&rtc->dtime);
}

void
rtc_ds1307_update(rtc_ds1307_t *rtc)
{
 rtc->rtcc++;

 if (rtc->rtcc >= 10)
  {
   rtc->rtcc = 0;
   if ((rtc->data[0] & 0x80) == 0)
    {
     /*      
          rtc->dtime.tm_sec++;
          if(rtc->dtime.tm_sec == 60)
          {
            rtc->dtime.tm_sec =0;
            rtc->dtime.tm_min++;
          } 
          if(rtc->dtime.tm_min == 60)
          {
            rtc->dtime.tm_min=0;
            rtc->dtime.tm_hour++;  
          }
          if(rtc->dtime.tm_hour == 24)
          {
            rtc->dtime.tm_hour=0;
            rtc->dtime.tm_mday++;
            rtc->dtime.tm_wday++;
          } 
          if(rtc->dtime.tm_wday == 7)rtc->dtime.tm_wday=0;
 
          if(rtc->dtime.tm_mday == 31)
          {
            rtc->dtime.tm_mday=0;
            rtc->dtime.tm_mon++;
          } 
          if(rtc->dtime.tm_mon == 13)
          {
            rtc->dtime.tm_mon=1;
            rtc->dtime.tm_year++;
          }  
      */
     time_t utime = mktime (&rtc->dtime) + 1;
#ifdef _WIN_  
     localtime_s (&rtc->dtime, &utime);
#else
     localtime_r (&utime, &rtc->dtime);
#endif 
     rtc->data[0] = ((rtc->dtime.tm_sec / 10) << 4) | (rtc->dtime.tm_sec % 10);
     rtc->data[1] = ((rtc->dtime.tm_min / 10) << 4) | (rtc->dtime.tm_min % 10);
     rtc->data[2] = ((rtc->dtime.tm_hour / 10) << 4) | (rtc->dtime.tm_hour % 10);
     rtc->data[3] = rtc->dtime.tm_wday;
     rtc->data[4] = ((rtc->dtime.tm_mday / 10) << 4) | (rtc->dtime.tm_mday % 10);
     rtc->data[5] = (((rtc->dtime.tm_mon + 1) / 10) << 4) | ((rtc->dtime.tm_mon + 1) % 10);
     rtc->data[6] = (((rtc->dtime.tm_year % 100) / 10) << 4) | (rtc->dtime.tm_year % 10);
    }

   //alarm
   rtc->alarm = 0;
   /*
   //minute
     if((rtc->data[0x9]&0x80) == 0)
     {
       if((rtc->data[0x9]&0x7F)==(rtc->data[0x3]&0x7F))
       {
         alarm2=1;
       }
       else
       {
         alarm2=0; 
       }
     }

   //hour
     if((rtc->data[0xA]&0x80) == 0)
     {
       if((rtc->data[0xA]&0x3F)==(rtc->data[0x4]&0x3F))
       {
         alarm2=1;
       }
       else
       {
         alarm2=0; 
       }
     }

   //day
     if((rtc->data[0xB]&0x80) == 0)
     {
       if((rtc->data[0xB]&0x3F)==(rtc->data[0x5]&0x3F))
       {
         alarm2=1;
       }
       else
       {
         alarm2=0; 
       }
     }

   //week day
     if((rtc->data[0xC]&0x80) == 0)
     {
       if((rtc->data[0xC]&0x07)==(rtc->data[0x6]&0x07))
       {
         alarm2=1;
       }
       else
       {
         alarm2=0; 
       }
     }

     if(alarm2)
     {

      rtc->data[0x2]|=0x08; //AF
      //printf("RTC Alarm !\n");

   
      //interrupt
      if(rtc->data[0x2] & 0x02) 
      {
         //printf("RTC Alarm Interrupt!\n");
      }

     }

    */
   rtc->ucont++;
   if (rtc->ucont == 10)
    {
#ifdef _DEBUG    
     printf ("test sync...  ");
#endif    
     time_t now = time (NULL);
     long int dsys = now - rtc->systime;
     long int drtc = rtc_ds1307_getUtime (rtc) - rtc->rtctime;
     int drift = dsys - drtc;
#ifdef _DEBUG    
     printf ("sys=%li rtc=%li drift=%li\n", dsys, drtc, dsys - drtc);
#endif    

     if (drift > 0)
      {
#ifdef _DEBUG      
       printf ("resync ...\n");
#endif      
       rtc_ds1307_setUtime (rtc, rtc_ds1307_getUtime (rtc) + drift);
       rtc->systime = now;
       rtc->rtctime = rtc_ds1307_getUtime (rtc);
      }

     rtc->ucont = 0;
    }
  }

}

void
rtc_ds1307_end(rtc_ds1307_t *rtc)
{
 dprintf ("rtc end\n");
}

unsigned char
rtc_ds1307_I2C_io(rtc_ds1307_t *rtc, unsigned char scl, unsigned char sda)
{


 unsigned char ret = bitbang_i2c_io (&rtc->bb_i2c, scl, sda);

 switch (bitbang_i2c_get_status (&rtc->bb_i2c))
  {
  case I2C_DATAW:
   if (rtc->bb_i2c.byte == 2)
    {
     rtc->addr = rtc->bb_i2c.datar;
    }
   else
    {
     dprintf ("write rtc[%04X]=%02X\n", rtc->addr, rtc->bb_i2c.datar);
     rtc->data[rtc->addr] = rtc->bb_i2c.datar;

     switch (rtc->addr)
      {
      case 0:
       rtc->dtime.tm_sec = (((rtc->bb_i2c.datar & 0xF0) >> 4)*10)+(rtc->bb_i2c.datar & 0x0F);
       break;
      case 1:
       rtc->dtime.tm_min = (((rtc->bb_i2c.datar & 0xF0) >> 4)*10)+(rtc->bb_i2c.datar & 0x0F);
       break;
      case 2:
       rtc->dtime.tm_hour = (((rtc->bb_i2c.datar & 0xF0) >> 4)*10)+(rtc->bb_i2c.datar & 0x0F);
       break;
      case 4:
       rtc->dtime.tm_mday = (((rtc->bb_i2c.datar & 0xF0) >> 4)*10)+(rtc->bb_i2c.datar & 0x0F);
       break;
      case 3:
       rtc->dtime.tm_wday = (((rtc->bb_i2c.datar & 0xF0) >> 4)*10)+(rtc->bb_i2c.datar & 0x0F);
       break;
      case 5:
       rtc->dtime.tm_mon = ((((rtc->bb_i2c.datar - 1) & 0xF0) >> 4)*10)+((rtc->bb_i2c.datar - 1) & 0x0F);
       break;
      case 6:
       rtc->dtime.tm_year = (rtc->dtime.tm_year & 0xFF00) | ((((rtc->bb_i2c.datar & 0xF0) >> 4)*10)+(rtc->bb_i2c.datar & 0x0F));
       break;
      default:
       break;
      }

     rtc->addr++;
     if (rtc->addr > 63)
      {
       rtc->addr -= 64;
      }
     rtc->systime = time (NULL);
     rtc->rtctime = rtc_ds1307_getUtime (rtc);
     rtc->ucont = 0;
    }
   break;
  case I2C_DATAR:
   bitbang_i2c_send (&rtc->bb_i2c, rtc->data[rtc->addr]);
   dprintf ("rtc read [%04X]=%02X\n", rtc->addr, rtc->data[rtc->addr]);
   rtc->addr++;
   if (rtc->addr > 63)
    {
     rtc->addr -= 64;
    }
   break;
  }


 return ret;
}


//TODO int output