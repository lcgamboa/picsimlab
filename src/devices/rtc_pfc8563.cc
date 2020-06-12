/* ########################################################################

   PICsimLab - PIC laboratory simulator

   ########################################################################

   Copyright (c) : 2010-2015  Luis Claudio Gambôa Lopes

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


#include"rtc_pfc8563.h"

#include<stdio.h>
#include<string.h>
#include<stdlib.h>

//#define _DEBUG
#define dprintf if (1) {} else printf

void
rtc_rst(rtc_t *rtc)
{

 rtc->sdao = 0;
 rtc->sclo = 1;
 rtc->bit = 0xFF;
 rtc->byte = 0xFF;
 rtc->datab = 0;
 rtc->ctrl = 0;
 rtc->ret = 0;
 rtc->ucont = 0;
 dprintf ("rtc rst\n");

 //for(i=0;i<16;i++)
 //printf("%02X ",rtc->data[i]);
 //printf("\n");
}

void
rtc_init(rtc_t *rtc)
{
 time_t utime;
 dprintf ("rtc init\n");

 rtc->data = (unsigned char *) calloc (16, sizeof (unsigned char));

 memset (rtc->data, 0xFF, 16);
 rtc_rst (rtc);


 utime = time (NULL);
 rtc_setUtime (rtc, utime);

}

void
rtc_setUtime(rtc_t *rtc, time_t utime)
{
 rtc->systime = time (NULL);
 rtc->rtctime = utime;
 rtc->ucont = 0;
#ifdef _WIN_  
 localtime_s (&rtc->dtime, &utime);
#else
 localtime_r (&utime, &rtc->dtime);
#endif  
 rtc->data[0] = 0;
 rtc->data[1] = 0;
 rtc->data[2] = ((rtc->dtime.tm_sec / 10) << 4) | (rtc->dtime.tm_sec % 10);
 rtc->data[3] = ((rtc->dtime.tm_min / 10) << 4) | (rtc->dtime.tm_min % 10);
 rtc->data[4] = ((rtc->dtime.tm_hour / 10) << 4) | (rtc->dtime.tm_hour % 10);
 rtc->data[5] = ((rtc->dtime.tm_mday / 10) << 4) | (rtc->dtime.tm_mday % 10);
 rtc->data[6] = rtc->dtime.tm_wday;
 rtc->data[7] = (((rtc->dtime.tm_mon + 1) / 10) << 4) | ((rtc->dtime.tm_mon + 1) % 10);
 rtc->data[8] = (((rtc->dtime.tm_year % 100) / 10) << 4) | (rtc->dtime.tm_year % 10);
}

time_t
rtc_getUtime(rtc_t *rtc)
{
 return mktime (&rtc->dtime);
}

static int rtcc = 0;
static int alarm;

void
rtc_update(rtc_t *rtc)
{
 rtcc++;

 if (rtcc >= 10)
  {
   rtcc = 0;
   if ((rtc->data[0] & 0x20) == 0)
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
     rtc->data[2] = ((rtc->dtime.tm_sec / 10) << 4) | (rtc->dtime.tm_sec % 10);
     rtc->data[3] = ((rtc->dtime.tm_min / 10) << 4) | (rtc->dtime.tm_min % 10);
     rtc->data[4] = ((rtc->dtime.tm_hour / 10) << 4) | (rtc->dtime.tm_hour % 10);
     rtc->data[5] = ((rtc->dtime.tm_mday / 10) << 4) | (rtc->dtime.tm_mday % 10);
     rtc->data[6] = rtc->dtime.tm_wday;
     rtc->data[7] = (((rtc->dtime.tm_mon + 1) / 10) << 4) | ((rtc->dtime.tm_mon + 1) % 10);
     rtc->data[8] = (((rtc->dtime.tm_year % 100) / 10) << 4) | (rtc->dtime.tm_year % 10);

    };

   //alarm
   alarm = 0;

   //minute
   if ((rtc->data[0x9]&0x80) == 0)
    {
     if ((rtc->data[0x9]&0x7F) == (rtc->data[0x3]&0x7F))
      {
       alarm = 1;
      }
     else
      {
       alarm = 0;
      }
    }

   //hour
   if ((rtc->data[0xA]&0x80) == 0)
    {
     if ((rtc->data[0xA]&0x3F) == (rtc->data[0x4]&0x3F))
      {
       alarm = 1;
      }
     else
      {
       alarm = 0;
      }
    }

   //day
   if ((rtc->data[0xB]&0x80) == 0)
    {
     if ((rtc->data[0xB]&0x3F) == (rtc->data[0x5]&0x3F))
      {
       alarm = 1;
      }
     else
      {
       alarm = 0;
      }
    }

   //week day
   if ((rtc->data[0xC]&0x80) == 0)
    {
     if ((rtc->data[0xC]&0x07) == (rtc->data[0x6]&0x07))
      {
       alarm = 1;
      }
     else
      {
       alarm = 0;
      }
    }

   if (alarm)
    {

     rtc->data[0x2] |= 0x08; //AF
     dprintf ("RTC Alarm !\n");


     //interrupt
     if (rtc->data[0x2] & 0x02)
      {
       dprintf ("RTC Alarm Interrupt!\n");
      }

    }

   rtc->ucont++;
   if (rtc->ucont == 10)
    {
#ifdef _DEBUG    
     printf ("test sync...  ");
#endif    
     time_t now = time (NULL);
     int dsys = now - rtc->systime;
     int drtc = rtc_getUtime (rtc) - rtc->rtctime;
     int drift = dsys - drtc;
#ifdef _DEBUG    
     printf ("sys=%i rtc=%i drift=%i\n", dsys, drtc, dsys - drtc);
#endif    

     if (drift > 0)
      {
#ifdef _DEBUG      
       printf ("resync ...\n");
#endif      
       rtc_setUtime (rtc, rtc_getUtime (rtc) + drift);
       rtc->systime = now;
       rtc->rtctime = rtc_getUtime (rtc);
      }


     rtc->ucont = 0;
    }
  }

}

void
rtc_end(rtc_t *rtc)
{
 dprintf ("rtc end\n");
 if (rtc->data)
  free (rtc->data);
 rtc->data = NULL;
}

unsigned char
rtc_io(rtc_t *rtc, unsigned char scl, unsigned char sda)
{

 if ((rtc->sdao == 1)&&(sda == 0)&&(scl == 1)&&(rtc->sclo == 1)) //start
  {
   rtc->bit = 0;
   rtc->byte = 0;
   rtc->datab = 0;
   rtc->ctrl = 0;
   rtc->ret = 0;
   dprintf ("rtc start!\n");
  }

 if ((rtc->sdao == 0)&&(sda == 1)&&(scl == 1)&&(rtc->sclo == 1)) //stop
  {
   rtc->bit = 0xFF;
   rtc->byte = 0xFF;
   rtc->ctrl = 0;
   rtc->ret = 0;
   dprintf ("rtc stop!\n");
  }


 if ((rtc->bit < 9)&&(rtc->sclo == 0)&&(scl == 1)) //data 
  {

   if (rtc->bit < 8)
    {
     rtc->datab |= (sda << (7 - rtc->bit));
    }

   rtc->bit++;
  }

 if ((rtc->bit < 9)&&(rtc->sclo == 1)&&(scl == 0)&&(rtc->ctrl == 0x0A3)) //data 
  {
   if (rtc->bit < 8)
    {
     rtc->ret = ((rtc->datas & (1 << (7 - rtc->bit))) > 0);
     //dprintf ("send %i %i (%02X)\n", rtc->bit, rtc->ret, rtc->datas);
    }
   else
    {
     rtc->ret = 0;
    }
  }


 if (rtc->bit == 9)
  {
   dprintf ("rtc data %02X\n", rtc->datab);

   if (rtc->byte == 0)
    {
     rtc->ctrl = rtc->datab;
     dprintf ("rtc ctrl = %02X\n", rtc->ctrl);
     rtc->ret = 0;

     if ((rtc->ctrl & 0x01) == 0x00)
      {
       rtc->addr = ((rtc->ctrl & 0x0E) << 7);
      }

    }

   if ((rtc->ctrl) == 0xA2)
    {
     if (rtc->byte == 1)rtc->addr = rtc->datab;

     if (((rtc->byte > 1)&&(rtc->ctrl & 0x01) == 0))
      {
       dprintf ("write rtc[%04X]=%02X\n", rtc->addr, rtc->datab);
       rtc->data[rtc->addr] = rtc->datab;

       switch (rtc->addr)
        {
        case 2:
         rtc->dtime.tm_sec = (((rtc->datab & 0xF0) >> 4)*10)+(rtc->datab & 0x0F);
         break;
        case 3:
         rtc->dtime.tm_min = (((rtc->datab & 0xF0) >> 4)*10)+(rtc->datab & 0x0F);
         break;
        case 4:
         rtc->dtime.tm_hour = (((rtc->datab & 0xF0) >> 4)*10)+(rtc->datab & 0x0F);
         break;
        case 5:
         rtc->dtime.tm_mday = (((rtc->datab & 0xF0) >> 4)*10)+(rtc->datab & 0x0F);
         break;
        case 6:
         rtc->dtime.tm_wday = (((rtc->datab & 0xF0) >> 4)*10)+(rtc->datab & 0x0F);
         break;
        case 7:
         rtc->dtime.tm_mon = (((rtc->datab & 0xF0) >> 4)*10)+(rtc->datab & 0x0F);
         break;
        case 8:
         rtc->dtime.tm_year = (rtc->dtime.tm_year & 0xFF00) | ((((rtc->datab & 0xF0) >> 4)*10)+(rtc->datab & 0x0F));
         break;
        default:
         break;
        }

       rtc->addr++;
       rtc->ret = 0;

       rtc->systime = time (NULL);
       rtc->rtctime = rtc_getUtime (rtc);
       rtc->ucont = 0;
      }
    }
   else if ((rtc->ctrl) == 0xA3) //read
    {
     if (rtc->byte < 16)
      {
       rtc->datas = rtc->data[rtc->addr];
       dprintf ("rtc read[%04X]=%02X\n", rtc->addr, rtc->datas);
       rtc->addr++;
      }
     else
      {
       rtc->ctrl = 0xFF;
      }
    }




   rtc->bit = 0;
   rtc->datab = 0;
   rtc->byte++;
  }


 rtc->sdao = sda;
 rtc->sclo = scl;
 return rtc->ret;
}

//TODO int output and countdown timer
