/* ########################################################################

   PICsimLab - PIC laboratory simulator

   ########################################################################

   Copyright (c) : 2015  Luis Claudio Gambôa Lopes

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


#include"rtc.h"

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<time.h>


time_t utime;
struct tm *dtime;

void
rtc_rst(rtc_t *rtc)
{
//int i;

rtc->sclo=1;
rtc->bit=0xFF;
rtc->byte=0xFF;
rtc->datab=0;
rtc->ctrl=0;
rtc->ret=0;
  //printf("rtc rst\n");

//for(i=0;i<16;i++)
   //printf("%02X ",rtc->data[i]);
//printf("\n");
}


void 
rtc_init(rtc_t *rtc)
{
  //printf("rtc init\n");
  
  rtc->data=(unsigned char *) calloc(16,sizeof(unsigned char));

  memset(rtc->data,0xFF,16);
  rtc_rst(rtc);
         

  utime=time(NULL);
  dtime=localtime(&utime);
  rtc->data[0]=0;
  rtc->data[1]=0;
  rtc->data[2]=((dtime->tm_sec/10)<<4)|(dtime->tm_sec%10);
  rtc->data[3]=((dtime->tm_min/10)<<4)|(dtime->tm_min%10);
  rtc->data[4]=((dtime->tm_hour/10)<<4)|(dtime->tm_hour%10);
  rtc->data[5]=((dtime->tm_mday/10)<<4)|(dtime->tm_mday%10);
  rtc->data[6]=dtime->tm_wday;
  rtc->data[7]=(((dtime->tm_mon+1)/10)<<4)|((dtime->tm_mon+1)%10);
  rtc->data[8]=(((dtime->tm_year%100)/10)<<4)|(dtime->tm_year%10);


}

int rtcc=0;
int alarm;

void rtc_update(rtc_t *rtc)
{
  rtcc++;

  if(rtcc >= 10)
  {
    rtcc=0;
    if((rtc->data[0] & 0x20) == 0 )
    {
       
      dtime->tm_sec++;
      if(dtime->tm_sec == 60)
      {
        dtime->tm_sec =0;
        dtime->tm_min++;
      } 
      if(dtime->tm_min == 60)
      {
        dtime->tm_min=0;
        dtime->tm_hour++;  
      }
      if(dtime->tm_hour == 24)
      {
        dtime->tm_hour=0;
        dtime->tm_mday++;
        dtime->tm_wday++;
      } 
      if(dtime->tm_wday == 7)dtime->tm_wday=0;
 
      if(dtime->tm_mday == 31)
      {
        dtime->tm_mday=0;
        dtime->tm_mon++;
      } 
      if(dtime->tm_mon == 13)
      {
        dtime->tm_mon=1;
        dtime->tm_year++;
      }  

      rtc->data[2]=((dtime->tm_sec/10)<<4)|(dtime->tm_sec%10);
      rtc->data[3]=((dtime->tm_min/10)<<4)|(dtime->tm_min%10);
      rtc->data[4]=((dtime->tm_hour/10)<<4)|(dtime->tm_hour%10);
      rtc->data[5]=((dtime->tm_mday/10)<<4)|(dtime->tm_mday%10);
      rtc->data[6]=dtime->tm_wday;
      rtc->data[7]=(((dtime->tm_mon+1)/10)<<4)|((dtime->tm_mon+1)%10);
      rtc->data[8]=(((dtime->tm_year%100)/10)<<4)|(dtime->tm_year%10);
    };

//alarm
  alarm=0;

//minute
  if((rtc->data[0x9]&0x80) == 0)
  {
    if((rtc->data[0x9]&0x7F)==(rtc->data[0x3]&0x7F))
    {
      alarm=1;
    }
    else
    {
      alarm=0; 
    }
  }

//hour
  if((rtc->data[0xA]&0x80) == 0)
  {
    if((rtc->data[0xA]&0x3F)==(rtc->data[0x4]&0x3F))
    {
      alarm=1;
    }
    else
    {
      alarm=0; 
    }
  }

//day
  if((rtc->data[0xB]&0x80) == 0)
  {
    if((rtc->data[0xB]&0x3F)==(rtc->data[0x5]&0x3F))
    {
      alarm=1;
    }
    else
    {
      alarm=0; 
    }
  }

//week day
  if((rtc->data[0xC]&0x80) == 0)
  {
    if((rtc->data[0xC]&0x07)==(rtc->data[0x6]&0x07))
    {
      alarm=1;
    }
    else
    {
      alarm=0; 
    }
  }

  if(alarm)
  {

   rtc->data[0x2]|=0x08; //AF
   //printf("RTC Alarm !\n");

   
   //interrupt
   if(rtc->data[0x2] & 0x02) 
   {
      //printf("RTC Alarm Interrupt!\n");
   }

  }


 }

};

void
rtc_end(rtc_t *rtc)
{
  //printf("rtc end\n");
  if(rtc->data)
    free(rtc->data);
  rtc->data=NULL;
}



unsigned char
rtc_io(rtc_t *rtc, unsigned char scl, unsigned char sda)
{

  if((rtc->sdao == 1)&&(sda ==0)&&(scl==1)&&(rtc->sclo == 1)) //start
  {
    rtc->bit=0;  
    rtc->byte=0;
    rtc->datab=0;
    rtc->ctrl=0;
    rtc->ret=0;
    //printf("rtc start!\n");	 
  }
  
  if((rtc->sdao == 0)&&(sda ==1)&&(scl==1)&&(rtc->sclo == 1)) //stop
  {
    rtc->bit=0xFF;  
    rtc->byte=0xFF;  
    rtc->ctrl=0;
    rtc->ret=0;
    //printf("rtc stop!\n");	 
  }


  if((rtc->bit < 9)&&(rtc->sclo == 0)&&(scl==1))  //data 
  {

    if(rtc->bit < 8)
    {
     rtc->datab|=(sda<<(7-rtc->bit));
    }

    rtc->bit++; 
  }
  
  if((rtc->bit < 9)&&(rtc->sclo == 1)&&(scl==0)&&(rtc->ctrl == 0x0A3))  //data 
  {
    if(rtc->bit < 8)
    {
     rtc->ret=((rtc->datas & (1<<(7-rtc->bit)))>0);
    ////printf("send %i %i (%02X)\n",rtc->bit,rtc->ret,rtc->datas);  
    }
    else
    {
     rtc->ret=0;
    }
  }


  if(rtc->bit == 9)
  {
     //printf("rtc data %02X\n",rtc->datab);

     if(rtc->byte == 0)
     {
       rtc->ctrl=rtc->datab;
       //printf("rtc ctrl = %02X\n",rtc->ctrl);		
       rtc->ret=0;
       
       if( (rtc->ctrl& 0x01) == 0x00)
       {     
           rtc->addr=((rtc->ctrl&0x0E)<<7);	
       };
       
     }

     if((rtc->ctrl) == 0xA2)
     {
         if(rtc->byte == 1)rtc->addr=rtc->datab;	
  
         if(((rtc->byte > 1)&&(rtc->ctrl &0x01) == 0))
         {
            //printf("write rtc[%04X]=%02X\n",rtc->addr,rtc->datab); 
            rtc->data[rtc->addr]=rtc->datab;
 
            switch(rtc->addr)
            {
              case 2:
                 dtime->tm_sec=(((rtc->datab&0xF0)>>4)*10)+(rtc->datab&0x0F); 
                 break; 
              case 3:
                 dtime->tm_min=(((rtc->datab&0xF0)>>4)*10)+(rtc->datab&0x0F); 
                 break; 
              case 4:
                 dtime->tm_hour=(((rtc->datab&0xF0)>>4)*10)+(rtc->datab&0x0F); 
                 break; 
              case 5:
                 dtime->tm_mday=(((rtc->datab&0xF0)>>4)*10)+(rtc->datab&0x0F); 
                 break; 
              case 6:
                 dtime->tm_wday=(((rtc->datab&0xF0)>>4)*10)+(rtc->datab&0x0F); 
                 break; 
              case 7:
                 dtime->tm_mon=(((rtc->datab&0xF0)>>4)*10)+(rtc->datab&0x0F); 
                 break; 
              case 8:
                 dtime->tm_year= (dtime->tm_year&0xFF00)|((((rtc->datab&0xF0)>>4)*10)+(rtc->datab&0x0F)); 
                 break; 
             default:
                break;   
            } 
 
            rtc->addr++;
            rtc->ret=0;
          }
     }  
     else if((rtc->ctrl ) == 0xA3)  //read
     {
       if(rtc->byte < 16)
       {  
         rtc->datas=rtc->data[rtc->addr];
         //printf("read rtc[%04X]=%02X\n",rtc->addr,rtc->datas); 
         rtc->addr++;
       }
       else
       {
         rtc->ctrl=0xFF;
       }
     }
 
     


     rtc->bit=0; 
     rtc->datab=0;
     rtc->byte++;
  }	


  rtc->sdao=sda;
  rtc->sclo=scl;
return rtc->ret;
}
