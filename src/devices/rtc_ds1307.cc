/* ########################################################################

   PICSimLab - Programmable IC Simulator Laboratory

   ########################################################################

   Copyright (c) : 2010-2024  Luis Claudio Gambôa Lopes <lcgamboa@yahoo.com>

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

#include "rtc_ds1307.h"
#include "../lib/board.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// #define _DEBUG

#define dprintf \
    if (1) {    \
    } else      \
        printf

enum { DS_SECONDS = 0, DS_MINUTES, DS_HOURS, DS_DAY, DS_DATE, DS_MONTH, DS_YEAR, DS_CONTROL };

#ifdef _DEBUG
static char buff[30];
const char* get_addr_str(const unsigned char addr, const unsigned char data) {
    switch (addr) {
        case DS_SECONDS:
            sprintf(buff, "SECONDS [%02X] = %02X", addr, data);
            break;
        case DS_MINUTES:
            sprintf(buff, "MINUTES [%02X] = %02X", addr, data);
            break;
        case DS_HOURS:
            if ((data & 0x40)) {  // 12h mode
                sprintf(buff, "HOURS   [%02X] = %02X  (%02X %s)", addr, data, data & 0x1F, (data & 0x20) ? "PM" : "AM");
            } else {  // 24h mode
                sprintf(buff, "HOURS   [%02X] = %02X  (%02X)", addr, data, data & 0x3F);
            }
            break;
        case DS_DAY: {
            const char wdays[8][4] = {"***", "MON", "TUE", "WED", "THU", "FRI", "SAT", "SUN"};
            sprintf(buff, "DAY     [%02X] = %02X  (%s)", addr, data, wdays[data]);
        } break;
        case DS_DATE:
            sprintf(buff, "DATE    [%02X] = %02X", addr, data);
            break;
        case DS_MONTH:
            sprintf(buff, "MONTH   [%02X] = %02X", addr, data);
            break;
        case DS_YEAR:
            sprintf(buff, "YEAR    [%02X] = %02X", addr, data);
            break;
        case DS_CONTROL:
            sprintf(buff, "CONTROL [%02X] = %02X", addr, data);
            break;
        default:
            sprintf(buff, "RAM     [%02X] = %02X", addr, data);
            break;
    }

    return buff;
}
#endif

void rtc_ds1307_rst(rtc_ds1307_t* rtc) {
    bitbang_i2c_rst(&rtc->bb_i2c);

    rtc->addr = 0;
    rtc->ucont = 0;
    rtc->alarm = 0;
    dprintf("rtc rst\n");
}

static void rtc_ds1307_callback(void* arg) {
    rtc_ds1307_t* rtc = (rtc_ds1307_t*)arg;

    if ((rtc->data[0] & 0x80) == 0) {
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
        time_t utime = mktime(&rtc->dtime) + 1;
#ifdef _WIN_
        localtime_s(&rtc->dtime, &utime);
#else
        localtime_r(&utime, &rtc->dtime);
#endif
        rtc->data[DS_SECONDS] = ((rtc->dtime.tm_sec / 10) << 4) | (rtc->dtime.tm_sec % 10);
        rtc->data[DS_MINUTES] = ((rtc->dtime.tm_min / 10) << 4) | (rtc->dtime.tm_min % 10);
        if (rtc->data[DS_HOURS] & 0x40) {  // 12h mode
            if (rtc->dtime.tm_hour < 12) {
                int hour = rtc->dtime.tm_hour;
                if (hour == 0) {
                    hour = 12;
                }
                rtc->data[DS_HOURS] = (rtc->data[DS_HOURS] & 0xC0) | ((((hour / 10) << 4) | (hour % 10)) & 0x1F);
            } else {
                int hour = rtc->dtime.tm_hour;
                if (hour > 12) {
                    hour -= 12;
                }
                rtc->data[DS_HOURS] = (rtc->data[DS_HOURS] & 0xC0) | 0x20 | ((((hour / 10) << 4) | (hour % 10)) & 0x1F);
            }
        } else {  // 24h mode
            rtc->data[DS_HOURS] =
                (rtc->data[DS_HOURS] & 0xC0) | ((((rtc->dtime.tm_hour / 10) << 4) | (rtc->dtime.tm_hour % 10)) & 0x3F);
        }
        rtc->data[DS_DAY] = rtc->dtime.tm_wday;
        if (rtc->data[DS_DAY] == 0)
            rtc->data[DS_DAY] = 7;
        rtc->data[DS_DATE] = ((rtc->dtime.tm_mday / 10) << 4) | (rtc->dtime.tm_mday % 10);
        rtc->data[DS_MONTH] = (((rtc->dtime.tm_mon + 1) / 10) << 4) | ((rtc->dtime.tm_mon + 1) % 10);
        rtc->data[DS_YEAR] = (((rtc->dtime.tm_year % 100) / 10) << 4) | (rtc->dtime.tm_year % 10);
    }

    // alarm
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
    if (rtc->ucont == 10) {
#ifdef _DEBUG
        printf("test sync...  ");
#endif
        time_t now = time(NULL);
        long int dsys = now - rtc->systime;
        long int drtc = rtc_ds1307_getUtime(rtc) - rtc->rtctime;
        int drift = dsys - drtc;
#ifdef _DEBUG
        printf("sys=%li rtc=%li drift=%li\n", dsys, drtc, dsys - drtc);
#endif

        if (drift > 0) {
#ifdef _DEBUG
            printf("resync ...\n");
#endif
            rtc_ds1307_setUtime(rtc, rtc_ds1307_getUtime(rtc) + drift);
            rtc->systime = now;
            rtc->rtctime = rtc_ds1307_getUtime(rtc);
        }

        rtc->ucont = 0;
    }
}

void rtc_ds1307_init(rtc_ds1307_t* rtc, board* pboard_) {
    time_t utime;
    dprintf("rtc init\n");
    rtc->pboard = pboard_;

    bitbang_i2c_init(&rtc->bb_i2c, 0x68);

    memset(rtc->data, 0xFF, 8);
    rtc->data[DS_HOURS] &= 0x3F;  // 24h mode
    rtc->data[DS_CONTROL] = 0;
    rtc_ds1307_rst(rtc);

    utime = time(NULL);
    rtc_ds1307_setUtime(rtc, utime);

    rtc->TimerID = rtc->pboard->TimerRegister_ms(1000, rtc_ds1307_callback, rtc);
}

void rtc_ds1307_setUtime(rtc_ds1307_t* rtc, time_t utime) {
    rtc->systime = time(NULL);
    rtc->rtctime = utime;
    rtc->ucont = 0;
#ifdef _WIN_
    localtime_s(&rtc->dtime, &utime);
#else
    localtime_r(&utime, &rtc->dtime);
#endif
    rtc->data[DS_SECONDS] = ((rtc->dtime.tm_sec / 10) << 4) | (rtc->dtime.tm_sec % 10);
    rtc->data[DS_MINUTES] = ((rtc->dtime.tm_min / 10) << 4) | (rtc->dtime.tm_min % 10);
    if (rtc->data[DS_HOURS] & 0x40) {  // 12h mode
        if (rtc->dtime.tm_hour < 12) {
            int hour = rtc->dtime.tm_hour;
            if (hour == 0) {
                hour = 12;
            }
            rtc->data[DS_HOURS] = (rtc->data[DS_HOURS] & 0xC0) | ((((hour / 10) << 4) | (hour % 10)) & 0x1F);
        } else {
            int hour = rtc->dtime.tm_hour;
            if (hour > 12) {
                hour -= 12;
            }
            rtc->data[DS_HOURS] = (rtc->data[DS_HOURS] & 0xC0) | 0x20 | ((((hour / 10) << 4) | (hour % 10)) & 0x1F);
        }
    } else {  // 24h mode
        rtc->data[DS_HOURS] =
            (rtc->data[DS_HOURS] & 0xC0) | ((((rtc->dtime.tm_hour / 10) << 4) | (rtc->dtime.tm_hour % 10)) & 0x3F);
    }
    rtc->data[DS_DAY] = rtc->dtime.tm_wday;
    if (rtc->data[DS_DAY] == 0)
        rtc->data[DS_DAY] = 7;
    rtc->data[DS_DATE] = ((rtc->dtime.tm_mday / 10) << 4) | (rtc->dtime.tm_mday % 10);
    rtc->data[DS_MONTH] = (((rtc->dtime.tm_mon + 1) / 10) << 4) | ((rtc->dtime.tm_mon + 1) % 10);
    rtc->data[DS_YEAR] = (((rtc->dtime.tm_year % 100) / 10) << 4) | (rtc->dtime.tm_year % 10);
}

time_t rtc_ds1307_getUtime(rtc_ds1307_t* rtc) {
    return mktime(&rtc->dtime);
}

void rtc_ds1307_end(rtc_ds1307_t* rtc) {
    dprintf("rtc end\n");
    rtc->pboard->TimerUnregister(rtc->TimerID);
}

unsigned char rtc_ds1307_I2C_io(rtc_ds1307_t* rtc, unsigned char scl, unsigned char sda) {
    unsigned char ret = bitbang_i2c_io(&rtc->bb_i2c, scl, sda);

    switch (bitbang_i2c_get_status(&rtc->bb_i2c)) {
        case I2C_DATAW:
            if (rtc->bb_i2c.byte == 2) {
                rtc->addr = rtc->bb_i2c.datar;
            } else {
#ifdef _DEBUG
                dprintf("> rtc write %s\n", get_addr_str(rtc->addr, rtc->bb_i2c.datar));
#endif
                rtc->data[rtc->addr] = rtc->bb_i2c.datar;

                switch (rtc->addr) {
                    case DS_SECONDS:
                        rtc->dtime.tm_sec = (((rtc->bb_i2c.datar & 0xF0) >> 4) * 10) + (rtc->bb_i2c.datar & 0x0F);
                        break;
                    case DS_MINUTES:
                        rtc->dtime.tm_min = (((rtc->bb_i2c.datar & 0xF0) >> 4) * 10) + (rtc->bb_i2c.datar & 0x0F);
                        break;
                    case DS_HOURS:
                        if (rtc->bb_i2c.datar & 0x40) {      // 12h mode
                            if (rtc->bb_i2c.datar & 0x20) {  // PM
                                rtc->dtime.tm_hour =
                                    (((rtc->bb_i2c.datar & 0x10) >> 4) * 10) + (rtc->bb_i2c.datar & 0x0F);
                                if (rtc->dtime.tm_hour < 12) {
                                    rtc->dtime.tm_hour += 12;
                                }
                            } else {  // AM
                                rtc->dtime.tm_hour =
                                    (((rtc->bb_i2c.datar & 0x10) >> 4) * 10) + (rtc->bb_i2c.datar & 0x0F);
                                if (rtc->dtime.tm_hour == 12) {
                                    rtc->dtime.tm_hour = 0;
                                }
                            }
                        } else {  // 24h mode
                            rtc->dtime.tm_hour = (((rtc->bb_i2c.datar & 0x30) >> 4) * 10) + (rtc->bb_i2c.datar & 0x0F);
                        }
                        break;
                    case DS_DATE:
                        rtc->dtime.tm_mday = (((rtc->bb_i2c.datar & 0xF0) >> 4) * 10) + (rtc->bb_i2c.datar & 0x0F);
                        break;
                    case DS_DAY: {
                        int wday = rtc->bb_i2c.datar;
                        if (wday == 7)
                            wday = 0;
                        rtc->dtime.tm_wday = (((wday & 0xF0) >> 4) * 10) + (wday & 0x0F);
                    } break;
                    case DS_MONTH:
                        rtc->dtime.tm_mon =
                            ((((rtc->bb_i2c.datar - 1) & 0xF0) >> 4) * 10) + ((rtc->bb_i2c.datar - 1) & 0x0F);
                        break;
                    case DS_YEAR:
                        rtc->dtime.tm_year = (rtc->dtime.tm_year & 0xFF00) |
                                             ((((rtc->bb_i2c.datar & 0xF0) >> 4) * 10) + (rtc->bb_i2c.datar & 0x0F));
                        break;
                    default:
                        break;
                }

                rtc->addr++;
                if (rtc->addr > 63) {
                    rtc->addr -= 64;
                }
                rtc->systime = time(NULL);
                rtc->rtctime = rtc_ds1307_getUtime(rtc);
                rtc->ucont = 0;
            }
            break;
        case I2C_DATAR:
            bitbang_i2c_send(&rtc->bb_i2c, rtc->data[rtc->addr]);
#ifdef _DEBUG
            dprintf("< rtc read  %s\n", get_addr_str(rtc->addr, rtc->data[rtc->addr]));
#endif
            rtc->addr++;
            if (rtc->addr > 63) {
                rtc->addr -= 64;
            }
            break;
    }

    return ret;
}

// TODO int output